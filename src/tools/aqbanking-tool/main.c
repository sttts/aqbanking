/***************************************************************************
 $RCSfile$
 -------------------
 cvs         : $Id$
 begin       : Tue May 03 2005
 copyright   : (C) 2005 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <gwenhywfar/logger.h>
#include <gwenhywfar/db.h>
#include <gwenhywfar/debug.h>

#include <gwenhywfar/logger.h>
#include <gwenhywfar/debug.h>
#include <aqbanking/banking.h>


#include "cbanking.h"
#include "globals.h"



AB_TRANSACTION *mkTransfer(AB_ACCOUNT *a, GWEN_DB_NODE *db) {
  AB_TRANSACTION *t;
  const char *s;
  int i;

  assert(a);
  assert(db);
  t=AB_Transaction_new();

  /* local account */
  s=AB_Account_GetCountry(a);
  if (!s || !*s)
    s="de";
  AB_Transaction_SetLocalCountry(t, s);
  AB_Transaction_SetRemoteCountry(t, s);

  s=AB_Account_GetBankCode(a);
  if (s && *s)
    AB_Transaction_SetLocalBankCode(t, s);
  s=AB_Account_GetAccountNumber(a);
  if (s && *s)
    AB_Transaction_SetLocalAccountNumber(t, s);
  s=AB_Account_GetOwnerName(a);
  if (s && *s)
    AB_Transaction_SetLocalName(t, s);

  /* remote account */
  s=GWEN_DB_GetCharValue(db, "remoteBankId", 0, 0);
  if (s && *s)
    AB_Transaction_SetRemoteBankCode(t, s);
  else {
    DBG_ERROR(0, "No remote bank id given");
    AB_Transaction_free(t);
    return 0;
  }
  s=GWEN_DB_GetCharValue(db, "remoteAccountId", 0, 0);
  if (s && *s)
    AB_Transaction_SetRemoteAccountNumber(t, s);
  for (i=0; i<10; i++) {
    s=GWEN_DB_GetCharValue(db, "remoteName", i, 0);
    if (!s)
      break;
    if (*s)
      AB_Transaction_AddRemoteName(t, s, 0);
  }
  if (i<1) {
    DBG_ERROR(0, "No remote name given");
    AB_Transaction_free(t);
    return 0;
  }

  /* transfer data */
  for (i=0; i<20; i++) {
    s=GWEN_DB_GetCharValue(db, "purpose", i, 0);
    if (!s)
      break;
    if (*s)
      AB_Transaction_AddPurpose(t, s, 0);
  }
  if (i<1) {
    DBG_ERROR(0, "No purpose given");
    AB_Transaction_free(t);
    return 0;
  }

  i=GWEN_DB_GetIntValue(db, "textkey", 0, -1);
  if (i>0)
    AB_Transaction_SetTextKey(t, i);

  s=GWEN_DB_GetCharValue(db, "value", 0, 0);
  if (s && *s) {
    AB_VALUE *v;

    v=AB_Value_fromString(s);
    assert(v);
    if (AB_Value_IsNegative(v) || AB_Value_IsZero(v)) {
      DBG_ERROR(0, "Only positive non-zero amount allowed");
      AB_Transaction_free(t);
      return 0;
    }
    AB_Transaction_SetValue(t, v);
    AB_Value_free(v);
  }
  else {
    DBG_ERROR(0, "No value given");
    AB_Transaction_free(t);
    return 0;
  }

  return t;
}




int main(int argc, char **argv) {
  GWEN_DB_NODE *db;
  const char *cmd;
  const char *pinFile;
  int rv;
  AB_BANKING *ab;
  const GWEN_ARGS args[]={
  {
    GWEN_ARGS_FLAGS_HAS_ARGUMENT, /* flags */
    GWEN_ArgsTypeChar,            /* type */
    "cfgfile",                    /* name */
    0,                            /* minnum */
    1,                            /* maxnum */
    "C",                          /* short option */
    "cfgfile",                    /* long option */
    "Specify the configuration file",     /* short description */
    "Specify the configuration file"      /* long description */
  },
  {
    GWEN_ARGS_FLAGS_HAS_ARGUMENT, /* flags */
    GWEN_ArgsTypeChar,            /* type */
    "pinfile",                    /* name */
    0,                            /* minnum */
    1,                            /* maxnum */
    "P",                          /* short option */
    "pinfile",                    /* long option */
    "Specify the PIN file",       /* short description */
    "Specify the PIN file"        /* long description */
  },
  {
    GWEN_ARGS_FLAGS_HELP | GWEN_ARGS_FLAGS_LAST, /* flags */
    GWEN_ArgsTypeInt,             /* type */
    "help",                       /* name */
    0,                            /* minnum */
    0,                            /* maxnum */
    "h",                          /* short option */
    "help",                       /* long option */
    "Show this help screen",      /* short description */
    "Show this help screen"       /* long description */
  }
  };

#ifdef HAVE_I18N
  setlocale(LC_ALL,"");
  if (bindtextdomain(PACKAGE,  LOCALEDIR)==0)
    fprintf(stderr, "Error binding locale\n");
#endif

  GWEN_Logger_Open("aqbanking-tool", "aqbanking-tool", 0,
                   GWEN_LoggerTypeConsole,
                   GWEN_LoggerFacilityUser);
  GWEN_Logger_SetLevel("aqbanking-tool", GWEN_LoggerLevelNotice);
  GWEN_Logger_SetLevel(0, GWEN_LoggerLevelNotice);

  db=GWEN_DB_Group_new("arguments");
  rv=GWEN_Args_Check(argc, argv, 1,
		     GWEN_ARGS_MODE_ALLOW_FREEPARAM |
		     GWEN_ARGS_MODE_STOP_AT_FREEPARAM,
		     args,
		     db);
  if (rv==GWEN_ARGS_RESULT_ERROR) {
    fprintf(stderr, "ERROR: Could not parse arguments main\n");
    return -1;
  }
  else if (rv==GWEN_ARGS_RESULT_HELP) {
    GWEN_BUFFER *ubuf;

    ubuf=GWEN_Buffer_new(0, 1024, 0, 1);
    if (GWEN_Args_Usage(args, ubuf, GWEN_ArgsOutTypeTXT)) {
      fprintf(stderr, "ERROR: Could not create help string\n");
      return 1;
    }
    fprintf(stderr, "%s\n", GWEN_Buffer_GetStart(ubuf));
    GWEN_Buffer_free(ubuf);
    return 0;
  }
  if (rv) {
    argc-=rv-1;
    argv+=rv-1;
  }

  cmd=GWEN_DB_GetCharValue(db, "params", 0, 0);
  if (!cmd) {
    fprintf(stderr, "ERROR: Command needed.\n");
    return 1;
  }

  ab=CBanking_new("aqbanking-tool",
                  GWEN_DB_GetCharValue(db, "cfgfile", 0, 0));
  CBanking_SetCharSet(ab, "ISO-8859-15");
  pinFile=GWEN_DB_GetCharValue(db, "pinFile", 0, 0);
  if (pinFile) {
    GWEN_DB_NODE *dbPins;

    dbPins=GWEN_DB_Group_new("pins");
    if (GWEN_DB_ReadFile(dbPins, pinFile,
			 GWEN_DB_FLAGS_DEFAULT |
			 GWEN_PATH_FLAGS_CREATE_GROUP)) {
      DBG_ERROR(0, "Error reading pinfile \"%s\"", pinFile);
      return 2;
    }
    CBanking_SetPinDb(ab, dbPins);
  }

  if (strcasecmp(cmd, "listaccs")==0) {
    rv=listAccs(ab, db, argc, argv);
  }
  else if (strcasecmp(cmd, "request")==0) {
    rv=request(ab, db, argc, argv);
  }
  else if (strcasecmp(cmd, "exec")==0) {
    rv=qexec(ab, db, argc, argv);
  }
  else if (strcasecmp(cmd, "listtrans")==0) {
    rv=listTrans(ab, db, argc, argv);
  }
  else if (strcasecmp(cmd, "transfer")==0) {
    rv=transfer(ab, db, argc, argv);
  }
  else {
    fprintf(stderr, "ERROR: Unknown command \"%s\".\n", cmd);
    rv=1;
  }

  return rv;
}


