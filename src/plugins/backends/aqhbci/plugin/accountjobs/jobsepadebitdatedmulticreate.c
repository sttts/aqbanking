/***************************************************************************
    begin       : Wed Jan 08 2014
    copyright   : (C) 2014 by Martin Preuss
    email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#include "jobsepadebitdatedmulticreate_p.h"
#include "jobtransferbase_l.h"
#include "aqhbci_l.h"
#include "accountjob_l.h"
#include "job_l.h"
#include "provider_l.h"
#include "hhd_l.h"
#include <gwenhywfar/debug.h>
#include <gwenhywfar/misc.h>
#include <gwenhywfar/inherit.h>
#include <gwenhywfar/text.h>

#include <aqbanking/jobsepadebitnote_be.h>
#include <aqbanking/job_be.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>



GWEN_INHERIT(AH_JOB, AH_JOB_CREATESEPAMULTIDEBIT);




/* --------------------------------------------------------------- FUNCTION */
AH_JOB *AH_Job_SepaDebitDatedMultiCreate_new(AB_USER *u, AB_ACCOUNT *account) {
  AH_JOB *j;
  AH_JOB_CREATESEPAMULTIDEBIT *aj;
  GWEN_DB_NODE *dbParams;
  GWEN_DB_NODE *dbArgs;
  const char *s;

  j=AH_Job_TransferBase_new("JobSepaDebitDatedMultiCreate",
                            AB_Transaction_TypeSepaDebitNote,
                            AB_Transaction_SubTypeStandard,
                            u, account);
  if (!j)
    return 0;

  AH_Job_SetChallengeClass(j, 32);

  GWEN_NEW_OBJECT(AH_JOB_CREATESEPAMULTIDEBIT, aj);
  GWEN_INHERIT_SETDATA(AH_JOB, AH_JOB_CREATESEPAMULTIDEBIT, j, aj,
                       AH_Job_SepaDebitDatedMultiCreate_FreeData);

  /* overwrite some virtual functions */
  AH_Job_SetPrepareFn(j, AH_Job_SepaDebitDatedMultiCreate_Prepare);
  AH_Job_SetAddChallengeParamsFn(j, AH_Job_SepaDebitDatedMultiCreate_AddChallengeParams);

  /* overwrite virtual functions of transferBase class */
  AH_Job_TransferBase_SetExchangeParamsFn(j, AH_Job_SepaDebitDatedMultiCreate_ExchangeParams);
  AH_Job_TransferBase_SetExchangeArgsFn(j, AH_Job_TransferBase_ExchangeArgs_SepaDated);

  /* get params */
  dbParams=AH_Job_GetParams(j);
  assert(dbParams);

  AH_Job_SetMaxTransfers(j, GWEN_DB_GetIntValue(dbParams, "maxTransfers", 0, 0));

  s=GWEN_DB_GetCharValue(dbParams, "sumFieldNeeded", 0, "j");
  if (s && toupper(*s)=='J')
    aj->sumFieldNeeded=1;
  else
    aj->sumFieldNeeded=0;

  s=GWEN_DB_GetCharValue(dbParams, "singleBookingAllowed", 0, "j");
  if (s && toupper(*s)=='J')
    aj->singleBookingAllowed=1;
  else
    aj->singleBookingAllowed=0;

  /* set some known arguments */
  dbArgs=AH_Job_GetArguments(j);
  assert(dbArgs);


  return j;
}



/* --------------------------------------------------------------- FUNCTION */
void GWENHYWFAR_CB AH_Job_SepaDebitDatedMultiCreate_FreeData(void *bp, void *p){
  AH_JOB_CREATESEPAMULTIDEBIT *aj;

  aj=(AH_JOB_CREATESEPAMULTIDEBIT*)p;

  free(aj->fiid);
  AB_Value_free(aj->sumValues);

  GWEN_FREE_OBJECT(aj);
}



/* --------------------------------------------------------------- FUNCTION */
int AH_Job_SepaDebitDatedMultiCreate_ExchangeParams(AH_JOB *j, AB_JOB *bj,
                                                    AB_IMEXPORTER_CONTEXT *ctx) {
  AH_JOB_CREATESEPAMULTIDEBIT *aj;
  AB_TRANSACTION_LIMITS *lim;
  GWEN_DB_NODE *dbParams;
  int i, i1, i2;

  DBG_INFO(AQHBCI_LOGDOMAIN, "Exchanging params");

  assert(j);
  aj=GWEN_INHERIT_GETDATA(AH_JOB, AH_JOB_CREATESEPAMULTIDEBIT, j);
  assert(aj);

  dbParams=AH_Job_GetParams(j);
  DBG_DEBUG(AQHBCI_LOGDOMAIN, "Have this parameters to exchange:");
  if (GWEN_Logger_GetLevel(AQHBCI_LOGDOMAIN)>=GWEN_LoggerLevel_Debug)
    GWEN_DB_Dump(dbParams, 2);

  /* set some default limits */
  lim=AB_TransactionLimits_new();
  AB_TransactionLimits_SetMaxLenPurpose(lim, 35);
  AB_TransactionLimits_SetMaxLenRemoteName(lim, 70);
  AB_TransactionLimits_SetMaxLinesRemoteName(lim, 1);
  AB_TransactionLimits_SetMaxLinesPurpose(lim, 4);

  /* set info from BPD */
  i1=GWEN_DB_GetIntValue(dbParams, "minDelay_FNAL_RCUR", 0, 0);
  i2=GWEN_DB_GetIntValue(dbParams, "minDelay_FRST_OOFF", 0, 0);
  i=(i1>i2)?i1:i2;
  AB_TransactionLimits_SetMinValueSetupTime(lim, i);

  i1=GWEN_DB_GetIntValue(dbParams, "maxDelay_FNAL_RCUR", 0, 0);
  i2=GWEN_DB_GetIntValue(dbParams, "maxDelay_FRST_OOFF", 0, 0);
  i=(i1<i2)?i1:i2;
  AB_TransactionLimits_SetMaxValueSetupTime(lim, i);

  AB_Job_SetFieldLimits(bj, lim);
  AB_TransactionLimits_free(lim);

  return 0;
}



/* --------------------------------------------------------------- FUNCTION */
int AH_Job_SepaDebitDatedMultiCreate_AddChallengeParams(AH_JOB *j, int hkTanVer, GWEN_DB_NODE *dbMethod) {
  AH_JOB_CREATESEPAMULTIDEBIT *aj;
  const AB_TRANSACTION *t;
  const char *s;
  int tanVer=AH_JOB_TANVER_1_4;

  DBG_ERROR(AQHBCI_LOGDOMAIN, "AddChallengeParams function called");

  assert(j);
  aj=GWEN_INHERIT_GETDATA(AH_JOB, AH_JOB_CREATESEPAMULTIDEBIT, j);
  assert(aj);

  /* get data from first transaction */
  t=AH_Job_GetFirstTransfer(j);
  if (t==NULL) {
    DBG_ERROR(AQHBCI_LOGDOMAIN, "No transaction");
    return GWEN_ERROR_INVALID;
  }

  s=GWEN_DB_GetCharValue(dbMethod, "zkaTanVersion", 0, NULL);
  if (s && *s && strncasecmp(s, "1.3", 3)==0) {
    DBG_ERROR(AQHBCI_LOGDOMAIN, "TAN version is 1.3 (%s)", s);
    tanVer=AH_JOB_TANVER_1_3;
  }

  if (tanVer==AH_JOB_TANVER_1_4) {
    int rv;

    DBG_ERROR(AQHBCI_LOGDOMAIN, "TAN version is 1.4.x");
    rv=AH_HHD14_AddChallengeParams_32(j,
                                      AH_Job_GetTransferCount(j),
                                      aj->sumValues,
                                      AB_Transaction_GetLocalIban(t),
                                      AB_Transaction_GetDate(t));
    if (rv<0) {
      DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d)", rv);
      return rv;
    }
  }
  else {
    DBG_ERROR(AQHBCI_LOGDOMAIN, "Unhandled tan version %d for now", tanVer);
    return GWEN_ERROR_INTERNAL;
  }
  return 0;
}



/* --------------------------------------------------------------- FUNCTION */
int AH_Job_SepaDebitDatedMultiCreate_Prepare(AH_JOB *j) {
  AH_JOB_CREATESEPAMULTIDEBIT *aj;
  GWEN_DB_NODE *dbArgs;
  AB_BANKING *ab;
  AB_USER *u;
  int rv;
  const char *profileName="";
  const char *descriptor="";
  const char *s;
  AB_TRANSACTION *t;
  GWEN_BUFFER *dbuf;

  DBG_INFO(AQHBCI_LOGDOMAIN, "Preparing transfers");

  assert(j);
  aj=GWEN_INHERIT_GETDATA(AH_JOB, AH_JOB_CREATESEPAMULTIDEBIT, j);
  assert(aj);

  ab=AH_Job_GetBankingApi(j);
  assert(ab);

  u=AH_Job_GetUser(j);
  assert(u);

  dbArgs=AH_Job_GetArguments(j);

  /* choose from HISPAS */
  /* first check for any descriptor for pain 008.002.02 */
  s=AH_User_FindSepaDescriptor(u, "*008.002.02*");
  if (s) {
    profileName="008_003_02";
    descriptor=s;
  }
  else {
    /* look for pain 008.001.01 */
    s=AH_User_FindSepaDescriptor(u, "*008.001.01*");
    if (s) {
      profileName="008_001_01";
      descriptor=s;
    }
  }

  /* check for valid descriptor */
  if (!(descriptor && *descriptor)) {
    DBG_ERROR(AQHBCI_LOGDOMAIN, "No SEPA descriptor found, please update your SEPA account information");
    return GWEN_ERROR_GENERIC;
  }
  DBG_INFO(AQHBCI_LOGDOMAIN, "Using SEPA descriptor %s", descriptor);


  /* add transactions to ImExporter context, calculate sum on the fly */
  AB_Value_free(aj->sumValues);
  aj->sumValues=AB_Value_new();
  t=AH_Job_GetFirstTransfer(j);
  if (t==NULL) {
    DBG_ERROR(AQHBCI_LOGDOMAIN, "No transaction in job");
    assert(t); /* debug */
    return GWEN_ERROR_INTERNAL;
  }

  /* export transfers to SEPA */
  dbuf=GWEN_Buffer_new(0, 256, 0, 1);
  rv=AH_Job_TransferBase_SepaExportTransactions(j, profileName, dbuf);
  if (rv<0) {
    DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d)", rv);
    GWEN_Buffer_free(dbuf);
    return rv;
  }

  /* store descriptor */
  GWEN_DB_SetCharValue(dbArgs,
                       GWEN_DB_FLAGS_OVERWRITE_VARS,
                       "descriptor",
                       descriptor);
  /* store transfer */
  GWEN_DB_SetBinValue(dbArgs,
                      GWEN_DB_FLAGS_OVERWRITE_VARS,
                      "transfer",
                      GWEN_Buffer_GetStart(dbuf),
                      GWEN_Buffer_GetUsedBytes(dbuf));
  GWEN_Buffer_free(dbuf);

  /* set singleBookingAllowed */
  if (aj->singleBookingAllowed)
    GWEN_DB_SetCharValue(dbArgs, GWEN_DB_FLAGS_OVERWRITE_VARS, "singleBookingWanted", "J");
  else
    GWEN_DB_SetCharValue(dbArgs, GWEN_DB_FLAGS_OVERWRITE_VARS, "singleBookingWanted", "N");

  /* store sum value */
  if (aj->sumValues) {
    GWEN_DB_NODE *dbV;

    dbV=GWEN_DB_GetGroup(dbArgs, GWEN_DB_FLAGS_OVERWRITE_GROUPS, "value");
    assert(dbV);
    rv=AH_Provider_WriteValueToDb(aj->sumValues, dbV);
    if (rv<0) {
      DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d)", rv);
      return rv;
    }
  }
  return 0;
}



