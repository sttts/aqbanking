/* This file is auto-generated from "eutransferinfo.xml" by the typemaker
   tool of Gwenhywfar. 
   Do not edit this file -- all changes will be lost! */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "eutransferinfo_p.h"
#include <gwenhywfar/misc.h>
#include <gwenhywfar/db.h>
#include <gwenhywfar/debug.h>
#include <assert.h>
#include <stdlib.h>


GWEN_LIST_FUNCTIONS(AB_EUTRANSFER_INFO, AB_EuTransferInfo)
GWEN_LIST2_FUNCTIONS(AB_EUTRANSFER_INFO, AB_EuTransferInfo)


AB_EUTRANSFER_INFO *AB_EuTransferInfo_new() {
  AB_EUTRANSFER_INFO *st;

  GWEN_NEW_OBJECT(AB_EUTRANSFER_INFO, st)
  st->_usage=1;
  GWEN_LIST_INIT(AB_EUTRANSFER_INFO, st)
  return st;
}


void AB_EuTransferInfo_free(AB_EUTRANSFER_INFO *st) {
  if (st) {
    assert(st->_usage);
    if (--(st->_usage)==0) {
  if (st->countryCode)
    free(st->countryCode);
  if (st->fieldLimits)
    AB_TransactionLimits_free(st->fieldLimits);
  if (st->limitLocalValue)
    AB_Value_free(st->limitLocalValue);
  if (st->limitForeignValue)
    AB_Value_free(st->limitForeignValue);
  GWEN_LIST_FINI(AB_EUTRANSFER_INFO, st)
  GWEN_FREE_OBJECT(st);
    }
  }

}


AB_EUTRANSFER_INFO *AB_EuTransferInfo_dup(const AB_EUTRANSFER_INFO *d) {
  AB_EUTRANSFER_INFO *st;

  assert(d);
  st=AB_EuTransferInfo_new();
  if (d->countryCode)
    st->countryCode=strdup(d->countryCode);
  if (d->fieldLimits)
    st->fieldLimits=AB_TransactionLimits_dup(d->fieldLimits);
  if (d->limitLocalValue)
    st->limitLocalValue=AB_Value_dup(d->limitLocalValue);
  if (d->limitForeignValue)
    st->limitForeignValue=AB_Value_dup(d->limitForeignValue);
  return st;
}


int AB_EuTransferInfo_toDb(const AB_EUTRANSFER_INFO *st, GWEN_DB_NODE *db) {
  assert(st);
  assert(db);
  if (st->countryCode)
    if (GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_OVERWRITE_VARS, "countryCode", st->countryCode))
      return -1;
  if (st->fieldLimits)
    if (AB_TransactionLimits_toDb(st->fieldLimits, GWEN_DB_GetGroup(db, GWEN_DB_FLAGS_DEFAULT, "fieldLimits")))
      return -1;
  if (st->limitLocalValue)
    if (AB_Value_toDb(st->limitLocalValue, GWEN_DB_GetGroup(db, GWEN_DB_FLAGS_DEFAULT, "limitLocalValue")))
      return -1;
  if (st->limitForeignValue)
    if (AB_Value_toDb(st->limitForeignValue, GWEN_DB_GetGroup(db, GWEN_DB_FLAGS_DEFAULT, "limitForeignValue")))
      return -1;
  return 0;
}


AB_EUTRANSFER_INFO *AB_EuTransferInfo_fromDb(GWEN_DB_NODE *db) {
AB_EUTRANSFER_INFO *st;

  assert(db);
  st=AB_EuTransferInfo_new();
  AB_EuTransferInfo_SetCountryCode(st, GWEN_DB_GetCharValue(db, "countryCode", 0, 0));
  if (1) {
    GWEN_DB_NODE *dbT;

    dbT=GWEN_DB_GetGroup(db, GWEN_PATH_FLAGS_NAMEMUSTEXIST, "fieldLimits");
    if (dbT)  AB_EuTransferInfo_SetFieldLimits(st, AB_TransactionLimits_fromDb(dbT));
  }
  if (1) {
    GWEN_DB_NODE *dbT;

    dbT=GWEN_DB_GetGroup(db, GWEN_PATH_FLAGS_NAMEMUSTEXIST, "limitLocalValue");
    if (dbT)  AB_EuTransferInfo_SetLimitLocalValue(st, AB_Value_fromDb(dbT));
  }
  if (1) {
    GWEN_DB_NODE *dbT;

    dbT=GWEN_DB_GetGroup(db, GWEN_PATH_FLAGS_NAMEMUSTEXIST, "limitForeignValue");
    if (dbT)  AB_EuTransferInfo_SetLimitForeignValue(st, AB_Value_fromDb(dbT));
  }
  st->_modified=0;
  return st;
}


const char *AB_EuTransferInfo_GetCountryCode(const AB_EUTRANSFER_INFO *st) {
  assert(st);
  return st->countryCode;
}


void AB_EuTransferInfo_SetCountryCode(AB_EUTRANSFER_INFO *st, const char *d) {
  assert(st);
  if (st->countryCode)
    free(st->countryCode);
  if (d)
    st->countryCode=strdup(d);
  else
    st->countryCode=0;
  st->_modified=1;
}


const AB_TRANSACTION_LIMITS *AB_EuTransferInfo_GetFieldLimits(const AB_EUTRANSFER_INFO *st) {
  assert(st);
  return st->fieldLimits;
}


void AB_EuTransferInfo_SetFieldLimits(AB_EUTRANSFER_INFO *st, const AB_TRANSACTION_LIMITS *d) {
  assert(st);
  if (st->fieldLimits)
    AB_TransactionLimits_free(st->fieldLimits);
  if (d)
    st->fieldLimits=AB_TransactionLimits_dup(d);
  else
    st->fieldLimits=0;
  st->_modified=1;
}


const AB_VALUE *AB_EuTransferInfo_GetLimitLocalValue(const AB_EUTRANSFER_INFO *st) {
  assert(st);
  return st->limitLocalValue;
}


void AB_EuTransferInfo_SetLimitLocalValue(AB_EUTRANSFER_INFO *st, const AB_VALUE *d) {
  assert(st);
  if (st->limitLocalValue)
    AB_Value_free(st->limitLocalValue);
  if (d)
    st->limitLocalValue=AB_Value_dup(d);
  else
    st->limitLocalValue=0;
  st->_modified=1;
}


const AB_VALUE *AB_EuTransferInfo_GetLimitForeignValue(const AB_EUTRANSFER_INFO *st) {
  assert(st);
  return st->limitForeignValue;
}


void AB_EuTransferInfo_SetLimitForeignValue(AB_EUTRANSFER_INFO *st, const AB_VALUE *d) {
  assert(st);
  if (st->limitForeignValue)
    AB_Value_free(st->limitForeignValue);
  if (d)
    st->limitForeignValue=AB_Value_dup(d);
  else
    st->limitForeignValue=0;
  st->_modified=1;
}


int AB_EuTransferInfo_IsModified(const AB_EUTRANSFER_INFO *st) {
  assert(st);
  return st->_modified;
}


void AB_EuTransferInfo_SetModified(AB_EUTRANSFER_INFO *st, int i) {
  assert(st);
  st->_modified=i;
}


void AB_EuTransferInfo_Attach(AB_EUTRANSFER_INFO *st) {
  assert(st);
  st->_usage++;
}
AB_EUTRANSFER_INFO *AB_EuTransferInfo_List2__freeAll_cb(AB_EUTRANSFER_INFO *st, void *user_data) {
  AB_EuTransferInfo_free(st);
return 0;
}


void AB_EuTransferInfo_List2_freeAll(AB_EUTRANSFER_INFO_LIST2 *stl) {
  if (stl) {
    AB_EuTransferInfo_List2_ForEach(stl, AB_EuTransferInfo_List2__freeAll_cb, 0);
    AB_EuTransferInfo_List2_free(stl); 
  }
}




AB_EUTRANSFER_INFO_LIST *AB_EuTransferInfo_List_dup(const AB_EUTRANSFER_INFO_LIST *stl) {
  if (stl) {
    AB_EUTRANSFER_INFO_LIST *nl;
    AB_EUTRANSFER_INFO *e;

    nl=AB_EuTransferInfo_List_new();
    e=AB_EuTransferInfo_List_First(stl);
    while(e) {
      AB_EUTRANSFER_INFO *ne;

      ne=AB_EuTransferInfo_dup(e);
      assert(ne);
      AB_EuTransferInfo_List_Add(ne, nl);
      e=AB_EuTransferInfo_List_Next(e);
    } /* while (e) */
    return nl;
  }
  else
    return 0;
}


