/***************************************************************************
 $RCSfile$
 -------------------
 cvs         : $Id$
 begin       : Mon Mar 01 2004
 copyright   : (C) 2004 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "banking_p.h"
#include "provider_l.h"
#include <gwenhywfar/debug.h>
#include <gwenhywfar/misc.h>
#include <gwenhywfar/directory.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


GWEN_INHERIT_FUNCTIONS(AB_BANKING);

#include <aqbanking/error.h>


AB_BANKING *AB_Banking_new(const char *appName, const char *fname){
  AB_BANKING *ab;
  GWEN_BUFFER *buf;

  assert(appName);
  buf=0;
  if (!fname) {
    char home[256];

    if (GWEN_Directory_GetHomeDirectory(home, sizeof(home))) {
      DBG_ERROR(0, "Could not determine home directory, aborting.");
      return 0;
    }
    buf=GWEN_Buffer_new(0, 256, 0, 1);
    GWEN_Buffer_AppendString(buf, home);
    GWEN_Buffer_AppendString(buf, "/" AB_BANKING_CONFIGFILE);
    fname=GWEN_Buffer_GetStart(buf);
  }

  GWEN_NEW_OBJECT(AB_BANKING, ab);
  ab->providers=AB_Provider_List_new();
  ab->wizzards=AB_ProviderWizzard_List_new();
  ab->accounts=AB_Account_List_new();
  ab->enqueuedJobs=AB_Job_List_new();
  ab->appName=strdup(appName);
  ab->data=GWEN_DB_Group_new("BankingData");
  ab->configFile=strdup(fname);
  GWEN_Buffer_free(buf);
  return ab;
}



void AB_Banking_free(AB_BANKING *ab){
  if (ab) {
    AB_Job_List_free(ab->enqueuedJobs);
    AB_Account_List_free(ab->accounts);
    AB_ProviderWizzard_List_free(ab->wizzards);
    AB_Provider_List_free(ab->providers);
    GWEN_DB_Group_free(ab->data);
    free(ab->appName);
    free(ab->configFile);
    GWEN_FREE_OBJECT(ab);
  }
}



AB_JOB_LIST2 *AB_Banking_GetEnqueuedJobs(const AB_BANKING *ab){
  AB_JOB_LIST2 *jl;
  AB_JOB *j;

  assert(ab);
  if (AB_Job_List_GetCount(ab->enqueuedJobs)==0) {
    DBG_INFO(0, "No jobs");
    return 0;
  }

  jl=AB_Job_List2_new();
  j=AB_Job_List_First(ab->enqueuedJobs);
  assert(j);
  while(j) {
    AB_Job_List2_PushBack(jl, j);
    j=AB_Job_List_Next(j);
  } /* while */

  return jl;
}



GWEN_TYPE_UINT32 AB_Banking_GetUniqueId(AB_BANKING *ab){
  assert(ab);
  return ++(ab->lastUniqueId);
}



const char *AB_Banking_GetAppName(const AB_BANKING *ab){
  assert(ab);
  return ab->appName;
}



GWEN_DB_NODE *AB_Banking_GetProviderData(AB_BANKING *ab,
                                         const AB_PROVIDER *pro){
  const char *name;
  GWEN_DB_NODE *db;

  name=AB_Provider_GetName(pro);
  assert(name);

  db=GWEN_DB_GetGroup(ab->data, GWEN_DB_FLAGS_DEFAULT,
                      "static/providers");
  assert(db);
  db=GWEN_DB_GetGroup(ab->data, GWEN_DB_FLAGS_DEFAULT, name);
  assert(db);
  return db;
}



GWEN_DB_NODE *AB_Banking_GetAppData(AB_BANKING *ab) {
  GWEN_DB_NODE *db;

  assert(ab->appName);
  db=GWEN_DB_GetGroup(ab->data, GWEN_DB_FLAGS_DEFAULT,
                      "static/apps");
  assert(db);
  db=GWEN_DB_GetGroup(ab->data, GWEN_DB_FLAGS_DEFAULT,
                      ab->appName);
  assert(db);
  return db;
}



GWEN_DB_NODE *AB_Banking_GetWizzardData(AB_BANKING *ab,
                                        const AB_PROVIDER_WIZZARD *pw){
  AB_PROVIDER *pro;
  GWEN_DB_NODE *db;
  const char *name;

  name=AB_ProviderWizzard_GetName(pw);
  assert(name);
  pro=AB_ProviderWizzard_GetProvider(pw);
  assert(pro);
  db=AB_Banking_GetProviderData(ab, pro);
  assert(db);
  db=GWEN_DB_GetGroup(db, GWEN_DB_FLAGS_DEFAULT, name);
  assert(db);
  return db;
}










void AB_Banking_SetMessageBoxFn(AB_BANKING *ab,
                                AB_BANKING_MESSAGEBOX_FN f){
  assert(ab);
  ab->messageBoxFn=f;
}



void AB_Banking_SetInputBoxFn(AB_BANKING *ab,
                              AB_BANKING_INPUTBOX_FN f){
  assert(ab);
  ab->inputBoxFn=f;
}



void AB_Banking_SetShowBoxFn(AB_BANKING *ab,
                             AB_BANKING_SHOWBOX_FN f){
  assert(ab);
  ab->showBoxFn=f;
}



void AB_Banking_SetHideBoxFn(AB_BANKING *ab,
                             AB_BANKING_HIDEBOX_FN f){
  assert(ab);
  ab->hideBoxFn=f;
}



void AB_Banking_SetProgressStartFn(AB_BANKING *ab,
                                   AB_BANKING_PROGRESS_START_FN f){
  assert(ab);
  ab->progressStartFn=f;
}



void AB_Banking_SetProgressAdvanceFn(AB_BANKING *ab,
                                     AB_BANKING_PROGRESS_ADVANCE_FN f){
  assert(ab);
  ab->progressAdvanceFn=f;
}



void AB_Banking_SetProgressLogFn(AB_BANKING *ab,
                                 AB_BANKING_PROGRESS_LOG_FN f){
  assert(ab);
  ab->progressLogFn=f;
}



void AB_Banking_SetProgressEndFn(AB_BANKING *ab,
                                 AB_BANKING_PROGRESS_END_FN f){
  assert(ab);
  ab->progressEndFn=f;
}








int AB_Banking_MessageBox(AB_BANKING *ab,
                          AB_BANKING_MSGTYPE mt,
                          const char *title,
                          const char *text,
                          const char *b1,
                          const char *b2,
                          const char *b3){
  assert(ab);
  if (ab->messageBoxFn) {
    return ab->messageBoxFn(ab, mt, title, text, b1, b2, b3);
  }
  DBG_WARN(0, "No messageBox function set");
  return 0;
}



int AB_Banking_InputBox(AB_BANKING *ab,
                        const char *title,
                        const char *text,
                        char *buffer,
                        int minLen,
                        int maxLen,
                        GWEN_TYPE_UINT32 flags){
  assert(ab);
  if (ab->inputBoxFn) {
    return ab->inputBoxFn(ab, title, text, buffer, minLen, maxLen, flags);
  }
  DBG_ERROR(0, "No inputBox function set");
  return AB_ERROR_NOFN;
}



GWEN_TYPE_UINT32 AB_Banking_ShowBox(AB_BANKING *ab,
                                    const char *title,
                                    const char *text){
  assert(ab);
  if (ab->showBoxFn) {
    return ab->showBoxFn(ab, title, text);
  }
  DBG_WARN(0, "No showBox function set");
  return 0;
}



void AB_Banking_HideBox(AB_BANKING *ab, GWEN_TYPE_UINT32 id){
  assert(ab);
  if (ab->hideBoxFn) {
    return ab->hideBoxFn(ab, id);
  }
  DBG_WARN(0, "No hideBox function set");
}



GWEN_TYPE_UINT32 AB_Banking_ProgressStart(AB_BANKING *ab,
                                          const char *title,
                                          const char *text,
                                          GWEN_TYPE_UINT32 total){
  assert(ab);
  if (ab->progressStartFn) {
    return ab->progressStartFn(ab, title, text, total);
  }
  DBG_WARN(0, "No progressStart function set");
  return 0;
}



int AB_Banking_ProgressAdvance(AB_BANKING *ab,
                               GWEN_TYPE_UINT32 id,
                               GWEN_TYPE_UINT32 progress){
  assert(ab);
  if (ab->progressAdvanceFn) {
    return ab->progressAdvanceFn(ab, id, progress);
  }
  DBG_WARN(0, "No progressAdvance function set");
  return 0;
}



int AB_Banking_ProgressLog(AB_BANKING *ab,
                           GWEN_TYPE_UINT32 id,
                           AB_BANKING_LOGLEVEL level,
                           const char *text){
  assert(ab);
  if (ab->progressLogFn) {
    return ab->progressLogFn(ab, id, level, text);
  }
  DBG_WARN(0, "No progressLog function set");
  return 0;
}



int AB_Banking_ProgressEnd(AB_BANKING *ab, GWEN_TYPE_UINT32 id){
  assert(ab);
  if (ab->progressEndFn) {
    return ab->progressEndFn(ab, id);
  }
  DBG_WARN(0, "No progressEnd function set");
  return 0;
}



AB_PROVIDER *AB_Banking_FindProvider(AB_BANKING *ab, const char *name) {
  AB_PROVIDER *pro;

  assert(ab);
  assert(name);
  pro=AB_Provider_List_First(ab->providers);
  while(pro) {
    if (strcasecmp(AB_Provider_GetName(pro), name)==0)
      break;
    pro=AB_Provider_List_Next(pro);
  } /* while */

  return pro;
}



AB_PROVIDER *AB_Banking_GetProvider(AB_BANKING *ab, const char *name) {
  AB_PROVIDER *pro;

  assert(ab);
  assert(name);
  pro=AB_Banking_FindProvider(ab, name);
  if (pro)
    return pro;
  pro=AB_Provider_LoadPlugin(ab, name);
  if (pro) {
    AB_Provider_List_Add(pro, ab->providers);
  }

  return pro;
}



AB_PROVIDER_WIZZARD *AB_Banking_FindWizzard(AB_BANKING *ab,
                                            AB_PROVIDER *pro,
                                            const char *name){
  AB_PROVIDER_WIZZARD *pw;

  assert(ab);
  assert(pro);
  assert(name);
  pw=AB_ProviderWizzard_List_First(ab->wizzards);
  while(pw) {
    if ((AB_ProviderWizzard_GetProvider(pw)==pro) &&
        (strcasecmp(AB_ProviderWizzard_GetName(pw), name)==0))
      break;
    pw=AB_ProviderWizzard_List_Next(pw);
  } /* while */

  return pw;
}



AB_PROVIDER_WIZZARD *AB_Banking_GetWizzard(AB_BANKING *ab,
                                           AB_PROVIDER *pro,
                                           const char *name) {
  AB_PROVIDER_WIZZARD *pw;

  assert(ab);
  assert(pro);
  assert(name);
  pw=AB_Banking_FindWizzard(ab, pro, name);
  if (pw)
    return pw;
  pw=AB_ProviderWizzard_LoadPlugin(pro, name);
  if (pw) {
    AB_ProviderWizzard_List_Add(pw, ab->wizzards);
  }

  return pw;
}



AB_ACCOUNT_LIST2 *AB_Banking_GetAccounts(const AB_BANKING *ab){
  AB_ACCOUNT_LIST2 *al;
  AB_ACCOUNT *a;

  assert(ab);
  if (AB_Account_List_GetCount(ab->accounts)==0) {
    DBG_INFO(0, "No accounts");
    return 0;
  }
  al=AB_Account_List2_new();
  a=AB_Account_List_First(ab->accounts);
  assert(a);
  while(a) {
    AB_Account_List2_PushBack(al, a);
    a=AB_Account_List_Next(a);
  } /* while */

  return al;
}



AB_ACCOUNT *AB_Banking_GetAccount(const AB_BANKING *ab,
                                  GWEN_TYPE_UINT32 uniqueId){
  AB_ACCOUNT *a;

  assert(ab);
  if (AB_Account_List_GetCount(ab->accounts)==0) {
    DBG_INFO(0, "No accounts");
    return 0;
  }
  a=AB_Account_List_First(ab->accounts);
  assert(a);
  while(a) {
    if (AB_Account_GetUniqueId(a)==uniqueId)
      break;
    a=AB_Account_List_Next(a);
  } /* while */

  return a;
}




int AB_Banking_Init(AB_BANKING *ab) {
  GWEN_DB_NODE *dbT;
  GWEN_DB_NODE *dbTsrc;

  assert(ab);

  if (access(ab->configFile, F_OK)) {
    DBG_NOTICE(0,
               "Configuration file \"%s\" does not exist, "
               "will create it later.", ab->configFile);
    return 0;
  }

  dbT=GWEN_DB_Group_new("config");
  assert(dbT);
  if (GWEN_DB_ReadFile(dbT, ab->configFile,
                       GWEN_DB_FLAGS_DEFAULT |
                       GWEN_PATH_FLAGS_CREATE_GROUP)) {
    GWEN_DB_Group_free(dbT);
    return AB_ERROR_BAD_CONFIG_FILE;
  }

  ab->lastUniqueId=GWEN_DB_GetIntValue(dbT, "lastUniqueId", 0, 0);

  /* read data */
  dbTsrc=GWEN_DB_GetGroup(dbT, GWEN_PATH_FLAGS_NAMEMUSTEXIST, "banking");
  if (dbTsrc) {
    GWEN_DB_NODE *dbTdst;

    dbTdst=GWEN_DB_GetGroup(ab->data, GWEN_DB_FLAGS_DEFAULT, "static");
    GWEN_DB_AddGroupChildren(dbTdst, dbTsrc);
  }

  /* read accounts */
  dbTsrc=GWEN_DB_GetGroup(dbT, GWEN_PATH_FLAGS_NAMEMUSTEXIST, "accounts");
  if (dbTsrc) {
    GWEN_DB_NODE *dbA;

    dbA=GWEN_DB_FindFirstGroup(dbTsrc, "account");
    while(dbA) {
      AB_ACCOUNT *a;

      a=AB_Account_fromDb(ab, dbA);
      if (a) {
        DBG_INFO(0, "Adding account");
        AB_Account_List_Add(a, ab->accounts);
      }
      dbA=GWEN_DB_FindNextGroup(dbA, "account");
    } /* while */
  }

  /* read jobs */
  dbTsrc=GWEN_DB_GetGroup(dbT, GWEN_PATH_FLAGS_NAMEMUSTEXIST, "jobs");
  if (dbTsrc) {
    GWEN_DB_NODE *dbJ;

    dbJ=GWEN_DB_FindFirstGroup(dbTsrc, "job");
    while(dbJ) {
      AB_JOB *j;

      j=AB_Job_fromDb(ab, dbJ);
      if (j) {
        DBG_INFO(0, "Adding job");
        AB_Job_List_Add(j, ab->enqueuedJobs);
      }
      dbJ=GWEN_DB_FindNextGroup(dbJ, "job");
    } /* while */
  }

  GWEN_DB_Group_free(dbT);

  return 0;
}




int AB_Banking_Fini(AB_BANKING *ab) {
  GWEN_DB_NODE *db;
  GWEN_DB_NODE *dbT;
  AB_ACCOUNT *a;
  AB_JOB *j;

  assert(ab);

  db=GWEN_DB_Group_new("config");
  assert(db);

  /* save accounts */
  a=AB_Account_List_First(ab->accounts);
  while(a) {
    GWEN_DB_NODE *dbTdst;
    int rv;

    dbTdst=GWEN_DB_GetGroup(db,
                            GWEN_DB_FLAGS_DEFAULT |
                            GWEN_PATH_FLAGS_CREATE_GROUP,
                            "accounts/account");
    assert(dbTdst);
    rv=AB_Account_toDb(a, dbTdst);
    if (rv) {
      DBG_ERROR(0, "Error saving account \"%08x\"",
                AB_Account_GetUniqueId(a));
      GWEN_DB_Group_free(db);
      return rv;
    }
    a=AB_Account_List_Next(a);
  } /* while */

  /* save enqueued jobs */
  j=AB_Job_List_First(ab->enqueuedJobs);
  while(j) {
    GWEN_DB_NODE *dbTdst;
    int rv;
    AB_JOB_STATUS jst;

    jst=AB_Job_GetStatus(j);
    if (jst==AB_Job_StatusEnqueued ||
        jst==AB_Job_StatusSent) {
      /* only save pending jobs. Jobs already executed are NOT saved */
      dbTdst=GWEN_DB_GetGroup(db,
                              GWEN_DB_FLAGS_DEFAULT |
                              GWEN_PATH_FLAGS_CREATE_GROUP,
                              "jobs/job");
      assert(dbTdst);
      rv=AB_Job_toDb(j, dbTdst);
      if (rv) {
        DBG_ERROR(0, "Error saving job");
        GWEN_DB_Group_free(db);
        return rv;
      }
    }
    j=AB_Job_List_Next(j);
  } /* while */

  /* store static config data as "banking" */
  dbT=GWEN_DB_GetGroup(ab->data, GWEN_PATH_FLAGS_NAMEMUSTEXIST, "static");
  if (dbT) {
    GWEN_DB_NODE *dbTdst;

    dbTdst=GWEN_DB_GetGroup(db, GWEN_DB_FLAGS_DEFAULT, "banking");
    assert(dbTdst);
    GWEN_DB_AddGroupChildren(dbTdst, dbT);
  }

  GWEN_DB_SetIntValue(db, GWEN_DB_FLAGS_OVERWRITE_VARS,
                      "lastUniqueId", ab->lastUniqueId);


  /* write config file. TODO: make backups */
  if (GWEN_DB_WriteFile(db, ab->configFile,
                        GWEN_DB_FLAGS_DEFAULT)) {
    DBG_ERROR(0, "Could not save configuration");
    GWEN_DB_Group_free(db);
    return AB_ERROR_BAD_CONFIG_FILE;
  }

  GWEN_DB_Group_free(db);

  AB_Job_List_Clear(ab->enqueuedJobs);
  AB_Account_List_Clear(ab->accounts);
  AB_ProviderWizzard_List_Clear(ab->wizzards);
  AB_Provider_List_Clear(ab->providers);
  GWEN_DB_ClearGroup(ab->data, 0);

  return 0;
}




GWEN_PLUGIN_DESCRIPTION_LIST2 *AB_Banking_GetProviderDescrs(AB_BANKING *ab){
  return GWEN_LoadPluginDescrs(AQBANKING_PLUGINS "/providers");
}



GWEN_PLUGIN_DESCRIPTION_LIST2 *AB_Banking_GetWizzardDescrs(AB_BANKING *ab,
                                                           const char *pn){
  GWEN_BUFFER *pbuf;
  GWEN_PLUGIN_DESCRIPTION_LIST2 *wdl;

  pbuf=GWEN_Buffer_new(0, 256, 0, 1);
  GWEN_Buffer_AppendString(pbuf,
                           AQBANKING_PLUGINS
                           "/"
                           AB_PROVIDER_WIZZARD_FOLDER
                           "/");
  GWEN_Buffer_AppendString(pbuf, pn);

  wdl=GWEN_LoadPluginDescrs(GWEN_Buffer_GetStart(pbuf));

  GWEN_Buffer_free(pbuf);
  return wdl;
}



int AB_Banking__MergeInAccount(AB_BANKING *ab, AB_ACCOUNT *a) {
  AB_ACCOUNT *ta;
  const char *accountId;
  const char *bankCode;
  GWEN_DB_NODE *dbNew;
  GWEN_DB_NODE *dbOld;
  const char *p;

  accountId=AB_Account_GetAccountNumber(a);
  assert(accountId);
  bankCode=AB_Account_GetBankCode(a);
  assert(bankCode);

  ta=AB_Account_List_First(ab->accounts);
  if (!ta) {
    DBG_NOTICE(0, "No accounts.");
  }
  while(ta) {
    if (AB_Account_GetProvider(a)==AB_Account_GetProvider(ta)) {
      const char *caccountId;
      const char *cbankCode;

      caccountId=AB_Account_GetAccountNumber(ta);
      assert(caccountId);
      cbankCode=AB_Account_GetBankCode(ta);
      assert(cbankCode);

      DBG_NOTICE(0, "Comparing \"%s\" against \"%s\"",
                 caccountId, accountId);
      if ((strcasecmp(accountId, caccountId)==0) &&
          (strcasecmp(bankCode, cbankCode)==0)) {
        DBG_NOTICE(0, "Match");
        break;
      }
    }
    ta=AB_Account_List_Next(ta);
  } /* while */

  if (!ta) {
    /* account is new, simply add it */
    DBG_NOTICE(0, "Adding account");
    AB_Account_SetUniqueId(a, AB_Banking_GetUniqueId(ab));
    AB_Account_List_Add(a, ab->accounts);
    return 0;
  }

  /* copy new provider data over old data */
  DBG_NOTICE(0, "Updating account");
  dbNew=AB_Account_GetProviderData(ta);
  assert(dbNew);
  dbOld=AB_Account_GetProviderData(a);
  assert(dbOld);
  GWEN_DB_ClearGroup(dbOld, 0);
  GWEN_DB_AddGroupChildren(dbOld, dbNew);

  /* copy new data over old one */
  p=AB_Account_GetAccountName(a);
  if (p)
    AB_Account_SetAccountName(ta, p);
  p=AB_Account_GetOwnerName(a);
  if (p)
    AB_Account_SetOwnerName(ta, p);

  AB_Account_free(a);
  return 0;
}



int AB_Banking_ImportProviderAccounts(AB_BANKING *ab, const char *backend){
  AB_ACCOUNT_LIST2 *al;
  AB_ACCOUNT_LIST2_ITERATOR *ait;
  AB_ACCOUNT *a;
  AB_PROVIDER *pro;
  int successful;

  pro=AB_Banking_GetProvider(ab, backend);
  if (!pro) {
    DBG_ERROR(0, "Backend \"%s\" not found", backend);
    return AB_ERROR_NOT_FOUND;
  }

  al=AB_Provider_GetAccountList(pro);
  if (!al) {
    DBG_ERROR(0, "Backend \"%s\" has no accounts", backend);
    return AB_ERROR_EMPTY;
  }

  ait=AB_Account_List2_First(al);
  assert(ait);
  a=AB_Account_List2Iterator_Data(ait);
  successful=0;
  assert(a);
  while(a) {
    if (AB_Banking__MergeInAccount(ab, a)) {
      DBG_WARN(0, "Could not merge in account");
    }
    else
      successful++;
    a=AB_Account_List2Iterator_Next(ait);
  }
  AB_Account_List2Iterator_free(ait);

  if (!successful) {
    DBG_INFO(0, "No account imported");
    return AB_ERROR_EMPTY;
  }
  return 0;
}



int AB_Banking_EnqueueJob(AB_BANKING *ab, AB_JOB *j){
  int rv;

  assert(ab);
  assert(j);
  rv=AB_Job_CheckAvailability(j);
  if (rv) {
    DBG_ERROR(0, "Job is not available, refusing to enqueue.");
    return rv;
  }
  AB_Job_Attach(j);
  AB_Job_List_Add(j, ab->enqueuedJobs);
  return 0;
}



int AB_Banking_DequeueJob(AB_BANKING *ab, AB_JOB *j){
  assert(ab);
  assert(j);
  AB_Job_List_Del(j);
  AB_Job_free(j);
  return 0;
}



int AB_Banking__ExecuteQueue(AB_BANKING *ab){
  AB_PROVIDER *pro;
  int succ;

  assert(ab);
  pro=AB_Provider_List_First(ab->providers);
  succ=0;

  while(pro) {
    AB_JOB *j;
    int jobs;
    int rv;

    j=AB_Job_List_First(ab->enqueuedJobs);
    jobs=0;
    while(j) {
      if (AB_Job_GetStatus(j)==AB_Job_StatusEnqueued) {
        AB_ACCOUNT *a;

        a=AB_Job_GetAccount(j);
        assert(a);
        if (AB_Account_GetProvider(a)==pro) {
          /* same provider, add job */
          rv=AB_Provider_AddJob(pro, j);
          if (rv) {
            DBG_INFO(0, "Could not add job (%d)", rv);
            AB_Job_SetStatus(j, AB_Job_StatusError);
            AB_Job_SetResultText(j, "Refused by backend");
          }
          else
            jobs++;
        }
      } /* if job enqueued */
      j=AB_Job_List_Next(j);
    } /* while */
    if (jobs) {
      DBG_INFO(0, "Letting backend \"%s\" work",
               AB_Provider_GetName(pro));
      rv=AB_Provider_Execute(pro);
      if (rv) {

        DBG_INFO(0, "Error executing backend's queue");
        if (AB_Provider_List_Next(pro)) {
          int lrv;

          lrv=AB_Banking_MessageBox(ab,
                                    AB_Banking_MsgTypeError,
                                    "Error",
                                    "Error executing backend's queue.\n"
                                    "What shall we do ?",
                                    "Continue", "Abort", 0);
          if (lrv!=1) {
            DBG_INFO(0, "Aborted by user");
            return AB_ERROR_USER_ABORT;
          }
        } /* if more backends to go */
      }
      else
        succ++;
    } /* if jobs in backend's queue */

    pro=AB_Provider_List_Next(pro);
  } /* while */

  if (!succ) {
    DBG_ERROR(0, "Not a single job successfully executed");
    return AB_ERROR_GENERIC;
  }

  return 0;
}



int AB_Banking_ExecuteQueue(AB_BANKING *ab){
  int rv;
  AB_JOB *j;

  assert(ab);

  rv=AB_Banking__ExecuteQueue(ab);

  /* clear queue from jobs with status !=enqueued */
  j=AB_Job_List_First(ab->enqueuedJobs);
  while(j) {
    AB_JOB *nj;

    nj=AB_Job_List_Next(j);
    if (AB_Job_GetStatus(j)!=AB_Job_StatusEnqueued) {
      DBG_INFO(0, "Removing job");
      AB_Job_List_Del(j);
      AB_Job_free(j);
    }
    j=nj;
  } /* while */

  return rv;
}


















