/* This file is auto-generated from "split.xml" by the typemaker
   tool of Gwenhywfar. 
   Do not edit this file -- all changes will be lost! */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "split_p.h"
#include <gwenhywfar/misc.h>
#include <gwenhywfar/db.h>
#include <gwenhywfar/debug.h>
#include <assert.h>
#include <stdlib.h>


GWEN_INHERIT_FUNCTIONS(AB_SPLIT)
GWEN_LIST_FUNCTIONS(AB_SPLIT, AB_Split)
GWEN_LIST2_FUNCTIONS(AB_SPLIT, AB_Split)


AB_SPLIT *AB_Split_new() {
  AB_SPLIT *st;

  GWEN_NEW_OBJECT(AB_SPLIT, st)
  st->_usage=1;
  GWEN_INHERIT_INIT(AB_SPLIT, st)
  GWEN_LIST_INIT(AB_SPLIT, st)
  st->name=GWEN_StringList_new();
  st->purpose=GWEN_StringList_new();
  return st;
}


void AB_Split_free(AB_SPLIT *st) {
  if (st) {
    assert(st->_usage);
    if (--(st->_usage)==0) {
  GWEN_INHERIT_FINI(AB_SPLIT, st)
  if (st->country)
    free(st->country);
  if (st->bankCode)
    free(st->bankCode);
  if (st->branchId)
    free(st->branchId);
  if (st->accountNumber)
    free(st->accountNumber);
  if (st->suffix)
    free(st->suffix);
  if (st->name)
    GWEN_StringList_free(st->name);
  if (st->value)
    AB_Value_free(st->value);
  if (st->purpose)
    GWEN_StringList_free(st->purpose);
  GWEN_LIST_FINI(AB_SPLIT, st)
  GWEN_FREE_OBJECT(st);
    }
  }

}


AB_SPLIT *AB_Split_dup(const AB_SPLIT *d) {
  AB_SPLIT *st;

  assert(d);
  st=AB_Split_new();
  if (d->country)
    st->country=strdup(d->country);
  if (d->bankCode)
    st->bankCode=strdup(d->bankCode);
  if (d->branchId)
    st->branchId=strdup(d->branchId);
  if (d->accountNumber)
    st->accountNumber=strdup(d->accountNumber);
  if (d->suffix)
    st->suffix=strdup(d->suffix);
  if (d->name)
    st->name=GWEN_StringList_dup(d->name);
  if (d->value)
    st->value=AB_Value_dup(d->value);
  if (d->purpose)
    st->purpose=GWEN_StringList_dup(d->purpose);
  return st;
}


int AB_Split_toDb(const AB_SPLIT *st, GWEN_DB_NODE *db) {
  assert(st);
  assert(db);
  if (st->country)
    if (GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_OVERWRITE_VARS, "country", st->country))
      return -1;
  if (st->bankCode)
    if (GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_OVERWRITE_VARS, "bankCode", st->bankCode))
      return -1;
  if (st->branchId)
    if (GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_OVERWRITE_VARS, "branchId", st->branchId))
      return -1;
  if (st->accountNumber)
    if (GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_OVERWRITE_VARS, "accountNumber", st->accountNumber))
      return -1;
  if (st->suffix)
    if (GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_OVERWRITE_VARS, "suffix", st->suffix))
      return -1;
  if (st->name)
    {
      GWEN_STRINGLISTENTRY *se;

      GWEN_DB_DeleteVar(db, "name");
      se=GWEN_StringList_FirstEntry(st->name);
      while(se) {
        const char *s;

        s=GWEN_StringListEntry_Data(se);
        assert(s);
        if (GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT, "name", s))
          return -1;
        se=GWEN_StringListEntry_Next(se);
      } /* while */
    }
  if (st->value)
    if (AB_Value_toDb(st->value, GWEN_DB_GetGroup(db, GWEN_DB_FLAGS_DEFAULT, "value")))
      return -1;
  if (st->purpose)
    {
      GWEN_STRINGLISTENTRY *se;

      GWEN_DB_DeleteVar(db, "purpose");
      se=GWEN_StringList_FirstEntry(st->purpose);
      while(se) {
        const char *s;

        s=GWEN_StringListEntry_Data(se);
        assert(s);
        if (GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT, "purpose", s))
          return -1;
        se=GWEN_StringListEntry_Next(se);
      } /* while */
    }
  return 0;
}


AB_SPLIT *AB_Split_fromDb(GWEN_DB_NODE *db) {
AB_SPLIT *st;

  assert(db);
  st=AB_Split_new();
  AB_Split_SetCountry(st, GWEN_DB_GetCharValue(db, "country", 0, 0));
  AB_Split_SetBankCode(st, GWEN_DB_GetCharValue(db, "bankCode", 0, 0));
  AB_Split_SetBranchId(st, GWEN_DB_GetCharValue(db, "branchId", 0, 0));
  AB_Split_SetAccountNumber(st, GWEN_DB_GetCharValue(db, "accountNumber", 0, 0));
  AB_Split_SetSuffix(st, GWEN_DB_GetCharValue(db, "suffix", 0, 0));
  if (1) {
    int i;

    for (i=0; ; i++) {
      const char *s;

      s=GWEN_DB_GetCharValue(db, "name", i, 0);
      if (!s)
        break;
      AB_Split_AddName(st, s, 0);
    } /* for */
  }
  if (1) {
    GWEN_DB_NODE *dbT;

    dbT=GWEN_DB_GetGroup(db, GWEN_PATH_FLAGS_NAMEMUSTEXIST, "value");
    if (dbT)  AB_Split_SetValue(st, AB_Value_fromDb(dbT));
  }
  if (1) {
    int i;

    for (i=0; ; i++) {
      const char *s;

      s=GWEN_DB_GetCharValue(db, "purpose", i, 0);
      if (!s)
        break;
      AB_Split_AddPurpose(st, s, 0);
    } /* for */
  }
  st->_modified=0;
  return st;
}


const char *AB_Split_GetCountry(const AB_SPLIT *st) {
  assert(st);
  return st->country;
}


void AB_Split_SetCountry(AB_SPLIT *st, const char *d) {
  assert(st);
  if (st->country)
    free(st->country);
  if (d)
    st->country=strdup(d);
  else
    st->country=0;
  st->_modified=1;
}


const char *AB_Split_GetBankCode(const AB_SPLIT *st) {
  assert(st);
  return st->bankCode;
}


void AB_Split_SetBankCode(AB_SPLIT *st, const char *d) {
  assert(st);
  if (st->bankCode)
    free(st->bankCode);
  if (d)
    st->bankCode=strdup(d);
  else
    st->bankCode=0;
  st->_modified=1;
}


const char *AB_Split_GetBranchId(const AB_SPLIT *st) {
  assert(st);
  return st->branchId;
}


void AB_Split_SetBranchId(AB_SPLIT *st, const char *d) {
  assert(st);
  if (st->branchId)
    free(st->branchId);
  if (d)
    st->branchId=strdup(d);
  else
    st->branchId=0;
  st->_modified=1;
}


const char *AB_Split_GetAccountNumber(const AB_SPLIT *st) {
  assert(st);
  return st->accountNumber;
}


void AB_Split_SetAccountNumber(AB_SPLIT *st, const char *d) {
  assert(st);
  if (st->accountNumber)
    free(st->accountNumber);
  if (d)
    st->accountNumber=strdup(d);
  else
    st->accountNumber=0;
  st->_modified=1;
}


const char *AB_Split_GetSuffix(const AB_SPLIT *st) {
  assert(st);
  return st->suffix;
}


void AB_Split_SetSuffix(AB_SPLIT *st, const char *d) {
  assert(st);
  if (st->suffix)
    free(st->suffix);
  if (d)
    st->suffix=strdup(d);
  else
    st->suffix=0;
  st->_modified=1;
}


const GWEN_STRINGLIST *AB_Split_GetName(const AB_SPLIT *st) {
  assert(st);
  return st->name;
}


void AB_Split_SetName(AB_SPLIT *st, const GWEN_STRINGLIST *d) {
  assert(st);
  if (st->name)
    GWEN_StringList_free(st->name);
  if (d)
    st->name=GWEN_StringList_dup(d);
  else
    st->name=0;
  st->_modified=1;
}


void AB_Split_AddName(AB_SPLIT *st, const char *d, int chk){
  assert(st);
  assert(d);
  if (GWEN_StringList_AppendString(st->name, d, 0, chk))
    st->_modified=1;
}


void AB_Split_RemoveName(AB_SPLIT *st, const char *d) {
  if (GWEN_StringList_RemoveString(st->name, d))
    st->_modified=1;
}


void AB_Split_ClearName(AB_SPLIT *st) {
  if (GWEN_StringList_Count(st->name)) {
    GWEN_StringList_Clear(st->name);
    st->_modified=1;
  }
}


int AB_Split_HasName(AB_SPLIT *st, const char *d) {
  return GWEN_StringList_HasString(st->name, d);
}


const AB_VALUE *AB_Split_GetValue(const AB_SPLIT *st) {
  assert(st);
  return st->value;
}


void AB_Split_SetValue(AB_SPLIT *st, const AB_VALUE *d) {
  assert(st);
  if (st->value)
    AB_Value_free(st->value);
  if (d)
    st->value=AB_Value_dup(d);
  else
    st->value=0;
  st->_modified=1;
}


const GWEN_STRINGLIST *AB_Split_GetPurpose(const AB_SPLIT *st) {
  assert(st);
  return st->purpose;
}


void AB_Split_SetPurpose(AB_SPLIT *st, const GWEN_STRINGLIST *d) {
  assert(st);
  if (st->purpose)
    GWEN_StringList_free(st->purpose);
  if (d)
    st->purpose=GWEN_StringList_dup(d);
  else
    st->purpose=0;
  st->_modified=1;
}


void AB_Split_AddPurpose(AB_SPLIT *st, const char *d, int chk){
  assert(st);
  assert(d);
  if (GWEN_StringList_AppendString(st->purpose, d, 0, chk))
    st->_modified=1;
}


void AB_Split_RemovePurpose(AB_SPLIT *st, const char *d) {
  if (GWEN_StringList_RemoveString(st->purpose, d))
    st->_modified=1;
}


void AB_Split_ClearPurpose(AB_SPLIT *st) {
  if (GWEN_StringList_Count(st->purpose)) {
    GWEN_StringList_Clear(st->purpose);
    st->_modified=1;
  }
}


int AB_Split_HasPurpose(AB_SPLIT *st, const char *d) {
  return GWEN_StringList_HasString(st->purpose, d);
}


int AB_Split_IsModified(const AB_SPLIT *st) {
  assert(st);
  return st->_modified;
}


void AB_Split_SetModified(AB_SPLIT *st, int i) {
  assert(st);
  st->_modified=i;
}


void AB_Split_Attach(AB_SPLIT *st) {
  assert(st);
  st->_usage++;
}
AB_SPLIT *AB_Split_List2__freeAll_cb(AB_SPLIT *st, void *user_data) {
  AB_Split_free(st);
return 0;
}


void AB_Split_List2_freeAll(AB_SPLIT_LIST2 *stl) {
  if (stl) {
    AB_Split_List2_ForEach(stl, AB_Split_List2__freeAll_cb, 0);
    AB_Split_List2_free(stl); 
  }
}




AB_SPLIT_LIST *AB_Split_List_dup(const AB_SPLIT_LIST *stl) {
  if (stl) {
    AB_SPLIT_LIST *nl;
    AB_SPLIT *e;

    nl=AB_Split_List_new();
    e=AB_Split_List_First(stl);
    while(e) {
      AB_SPLIT *ne;

      ne=AB_Split_dup(e);
      assert(ne);
      AB_Split_List_Add(ne, nl);
      e=AB_Split_List_Next(e);
    } /* while (e) */
    return nl;
  }
  else
    return 0;
}



