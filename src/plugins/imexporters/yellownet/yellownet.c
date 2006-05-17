/***************************************************************************
 $RCSfile$
                             -------------------
    cvs         : $Id: openhbci1.c 566 2005-08-23 06:25:03Z aquamaniac $
    begin       : Mon Mar 01 2004
    copyright   : (C) 2004 by Martin Preuss
    email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "yellownet_p.h"
#include "i18n_l.h"
#include <aqbanking/banking.h>
#include <gwenhywfar/debug.h>
#include <gwenhywfar/misc.h>
#include <gwenhywfar/waitcallback.h>
#include <gwenhywfar/inherit.h>
#include <gwenhywfar/xml.h>
#include <gwenhywfar/text.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


GWEN_INHERIT(AB_IMEXPORTER, AB_IMEXPORTER_YELLOWNET);


AB_IMEXPORTER *yellownet_factory(AB_BANKING *ab, GWEN_DB_NODE *db){
  AB_IMEXPORTER *ie;
  AB_IMEXPORTER_YELLOWNET *ieh;

  ie=AB_ImExporter_new(ab, "yellownet");
  GWEN_NEW_OBJECT(AB_IMEXPORTER_YELLOWNET, ieh);
  GWEN_INHERIT_SETDATA(AB_IMEXPORTER, AB_IMEXPORTER_YELLOWNET, ie, ieh,
                       AB_ImExporterYN_FreeData);
  ieh->dbData=db;

  AB_ImExporter_SetImportFn(ie, AB_ImExporterYN_Import);
  AB_ImExporter_SetCheckFileFn(ie, AB_ImExporterYN_CheckFile);
  return ie;
}



void AB_ImExporterYN_FreeData(void *bp, void *p){
  AB_IMEXPORTER_YELLOWNET *ieh;

  ieh=(AB_IMEXPORTER_YELLOWNET*)p;
  GWEN_FREE_OBJECT(ieh);
}



AB_IMEXPORTER_ACCOUNTINFO*
AB_ImExporterYN__ReadAccountInfo(AB_IMEXPORTER *ie,
				 AB_IMEXPORTER_CONTEXT *ctx,
				 GWEN_XMLNODE *doc) {
  GWEN_XMLNODE *n;
  AB_IMEXPORTER_ACCOUNTINFO *ai;

  ai=AB_ImExporterAccountInfo_new();

  n=GWEN_XMLNode_FindFirstTag(doc, "SG2", 0, 0);
  if (n)
    n=GWEN_XMLNode_FindFirstTag(n, "FII", 0, 0);
  if (n) {
    GWEN_XMLNODE *nn;
    const char *s;

    nn=GWEN_XMLNode_FindFirstTag(n, "C078", 0, 0);
    if (nn) {
      /* account number */
      s=GWEN_XMLNode_GetCharValue(nn, "D_3194", 0);
      if (s)
	AB_ImExporterAccountInfo_SetAccountNumber(ai, s);
      /* IBAN */
      s=GWEN_XMLNode_GetCharValue(nn, "D_3192", 0);
      if (s)
	AB_ImExporterAccountInfo_SetIban(ai, s);
    }

    /* account number */
    s=GWEN_XMLNode_GetCharValue(n, "PF:D_5388", 0);
    if (s)
      AB_ImExporterAccountInfo_SetAccountName(ai, s);
  }

  /* owner name */
  n=GWEN_XMLNode_GetNodeByXPath(doc, "SG3[1]/NAD",
				GWEN_PATH_FLAGS_NAMEMUSTEXIST);
  if (n) {
    GWEN_XMLNODE *nn;

    nn=GWEN_XMLNode_FindFirstTag(n, "D_3035", 0, 0);
    if (nn) {
      const char *s;

      s=GWEN_XMLNode_GetProperty(nn, "Value", 0);
      if (s && strcasecmp(s, "HQ")==0) {
	nn=GWEN_XMLNode_FindFirstTag(n, "C058", 0, 0);
	if (nn) {
	  s=GWEN_XMLNode_GetCharValue(nn, "D_3124", 0);
	  if (s && *s)
	    AB_ImExporterAccountInfo_SetOwner(ai, s);
	}
      }
    }
  }

  return ai;
}



AB_VALUE *AB_ImExporterYN__ReadValue(AB_IMEXPORTER *ie,
				     GWEN_XMLNODE *node,
				     int value) {
  GWEN_XMLNODE *n;
  AB_VALUE *val=0;

  n=GWEN_XMLNode_FindFirstTag(node, "C516", 0, 0);
  if (n) {
    GWEN_XMLNODE *nn;
    int v=0;

    nn=GWEN_XMLNode_FindFirstTag(n, "D_5025", 0, 0);
    if (nn)
      v=atoi(GWEN_XMLNode_GetProperty(nn, "Value", "0"));
    if (value==0 || v==value) {
      const char *sValue;
      const char *sSign;

      sValue=GWEN_XMLNode_GetCharValue(n, "D_5004", 0);
      sSign=GWEN_XMLNode_GetCharValue(node, "PF:D_5003", 0);

      if (sValue) {
	val=AB_Value_fromString(sValue);
	if (val && sSign && strcmp(sSign, "-")==0)
	  AB_Value_Negate(val);
      }
    }
  }

  return val;
}



GWEN_TIME *AB_ImExporterYN__ReadTime(AB_IMEXPORTER *ie,
				     GWEN_XMLNODE *node,
				     int value) {
  GWEN_XMLNODE *n;
  GWEN_TIME *ti=0;

  n=GWEN_XMLNode_GetNodeByXPath(node, "C507",
				GWEN_PATH_FLAGS_NAMEMUSTEXIST);
  if (n) {
    GWEN_XMLNODE *nn;
    int v=0;

    nn=GWEN_XMLNode_FindFirstTag(n, "D_2005", 0, 0);
    if (nn)
      v=atoi(GWEN_XMLNode_GetProperty(nn, "Value", "0"));

    if (value==0 || v==value) {
      const char *s;

      s=GWEN_XMLNode_GetCharValue(n, "D_2380", 0);
      if (s)
	ti=AB_ImExporter_DateFromString(s, "YYYYMMDD", 0);
    }
  }

  return ti;
}



AB_TRANSACTION *AB_ImExporterYN__ReadLNE(AB_IMEXPORTER *ie,
					 GWEN_XMLNODE *node) {
  GWEN_XMLNODE *n;

  n=GWEN_XMLNode_FindFirstTag(node, "SG6", 0, 0);
  if (n) {
    AB_TRANSACTION *t;
    GWEN_XMLNODE *nn;
    GWEN_TIME *ti=0;
    AB_VALUE *val=0;

    t=AB_Transaction_new();

    /* get date */
    nn=GWEN_XMLNode_FindFirstTag(n, "DTM", 0, 0);
    if (nn)
      ti=AB_ImExporterYN__ReadTime(ie, nn, 209);
    AB_Transaction_SetValutaDate(t, ti);
    GWEN_Time_free(ti);
    ti=0;

    /* read amount */
    nn=GWEN_XMLNode_FindFirstTag(n, "MOA", 0, 0);
    if (nn) {
      /* Gutschrift */
      val=AB_ImExporterYN__ReadValue(ie, nn, 210);
      if (val) {
	if (AB_Value_IsZero(val)) {
	  AB_Value_free(val);
	  val=0;
	}
      }
      if (val==0) {
	val=AB_ImExporterYN__ReadValue(ie, nn, 211);
	if (val)
	  AB_Value_Negate(val);
      }
    }
    if (val==0)
      val=AB_Value_new(0.0, 0);

    AB_Transaction_SetValue(t, val);
    AB_Value_free(val);
    val=0;

    /* read purpose */
    nn=GWEN_XMLNode_GetNodeByXPath(n, "FTX/C108",
				   GWEN_PATH_FLAGS_NAMEMUSTEXIST);
    if (nn) {
      GWEN_XMLNODE *nnn;

      nnn=GWEN_XMLNode_FindFirstTag(nn, "D_4440", 0, 0);
      while(nnn) {
	GWEN_XMLNODE *nData;
  
	nData=GWEN_XMLNode_GetFirstData(nnn);
	if (nData)
	  AB_Transaction_AddPurpose(t, GWEN_XMLNode_GetData(nData), 0);
	nnn=GWEN_XMLNode_FindNextTag(nnn, "D_4440", 0, 0);
      }
    }

    return t;
  }

  return 0;
}



int AB_ImExporterYN__ReadTransactions(AB_IMEXPORTER *ie,
                                      AB_IMEXPORTER_ACCOUNTINFO *ai,
				      GWEN_XMLNODE *doc) {
  GWEN_XMLNODE *n;

  n=GWEN_XMLNode_FindFirstTag(doc, "SG4", 0, 0);
  while(n) {
    GWEN_XMLNODE *nn;

    nn=GWEN_XMLNode_GetNodeByXPath(n, "LIN/PF:D_0805",
				   GWEN_PATH_FLAGS_NAMEMUSTEXIST);
    if (nn) {
      const char *s;

      s=GWEN_XMLNode_GetProperty(nn, "Value", 0);
      if (s && strcasecmp(s, "LNE")==0) {
	AB_TRANSACTION *t;

	t=AB_ImExporterYN__ReadLNE(ie, n);
	if (t) {
	  const char *s;

	  s=AB_ImExporterAccountInfo_GetAccountNumber(ai);
          AB_Transaction_SetLocalAccountNumber(t, s);
	  s=AB_ImExporterAccountInfo_GetIban(ai);
	  AB_Transaction_SetLocalIban(t, s);
	  AB_ImExporterAccountInfo_AddTransaction(ai, t);
	}
      }
    }

    n=GWEN_XMLNode_FindNextTag(n, "SG4", 0, 0);
  }

  return 0;
}



int AB_ImExporterYN__ReadAccountStatus(AB_IMEXPORTER *ie,
				       AB_IMEXPORTER_ACCOUNTINFO *ai,
				       GWEN_XMLNODE *doc) {
  GWEN_XMLNODE *n;

  n=GWEN_XMLNode_FindFirstTag(doc, "SG4", 0, 0);
  while(n) {
    GWEN_XMLNODE *nn;

    nn=GWEN_XMLNode_GetNodeByXPath(n, "LIN/PF:D_0805",
				   GWEN_PATH_FLAGS_NAMEMUSTEXIST);
    if (nn) {
      const char *s;

      s=GWEN_XMLNode_GetProperty(nn, "Value", 0);
      if (s && strcasecmp(s, "LEN")==0) {
	GWEN_XMLNODE *nnn;
	GWEN_TIME *ti=0;
        AB_VALUE *val=0;

        /* read time */
	nnn=GWEN_XMLNode_GetNodeByXPath(n, "SG5/DTM",
					GWEN_PATH_FLAGS_NAMEMUSTEXIST);
	if (nnn)
	  ti=AB_ImExporterYN__ReadTime(ie, nnn, 202);

	/* read value */
	nnn=GWEN_XMLNode_GetNodeByXPath(n, "SG5/MOA",
					GWEN_PATH_FLAGS_NAMEMUSTEXIST);
	if (nnn)
	  val=AB_ImExporterYN__ReadValue(ie, nnn, 343);

	if (val) {
	  AB_ACCOUNT_STATUS *ast;
	  AB_BALANCE *bal;

	  ast=AB_AccountStatus_new();
	  bal=AB_Balance_new(val, ti);

	  AB_AccountStatus_SetBookedBalance(ast, bal);
	  AB_Balance_free(bal);

	  AB_ImExporterAccountInfo_AddAccountStatus(ai, ast);
	}
	AB_Value_free(val);
        GWEN_Time_free(ti);
      }
    }

    n=GWEN_XMLNode_FindNextTag(n, "SG4", 0, 0);
  }

  return 0;
}



int AB_ImExporterYN_Import(AB_IMEXPORTER *ie,
			   AB_IMEXPORTER_CONTEXT *ctx,
			   GWEN_BUFFEREDIO *bio,
			   GWEN_DB_NODE *params){
  AB_IMEXPORTER_YELLOWNET *ieh;
  GWEN_DB_NODE *dbSubParams;
  GWEN_XMLNODE *doc;
  GWEN_XMLNODE *node;
  int rv;

  assert(ie);
  ieh=GWEN_INHERIT_GETDATA(AB_IMEXPORTER, AB_IMEXPORTER_YELLOWNET, ie);
  assert(ieh);

  dbSubParams=GWEN_DB_GetGroup(params, GWEN_PATH_FLAGS_NAMEMUSTEXIST,
			       "params");

  doc=GWEN_XMLNode_new(GWEN_XMLNodeTypeTag, "root");
  rv=GWEN_XML_Parse(doc, bio, GWEN_XML_FLAGS_HANDLE_HEADERS);
  if (rv) {
    DBG_ERROR(AQBANKING_LOGDOMAIN,
	      "Could not parse XML stream (%d)", rv);
    GWEN_XMLNode_free(doc);
    return AB_ERROR_BAD_DATA;
  }

  node=GWEN_XMLNode_FindFirstTag(doc, "IC", 0, 0);
  node=GWEN_XMLNode_FindFirstTag(node, "KONAUS", 0, 0);
  while(node) {
    AB_IMEXPORTER_ACCOUNTINFO *ai;

    ai=AB_ImExporterYN__ReadAccountInfo(ie, ctx, node);
    assert(ai);

    rv=AB_ImExporterYN__ReadAccountStatus(ie, ai, node);
    if (rv==0)
      rv=AB_ImExporterYN__ReadTransactions(ie, ai, node);
    if (rv) {
      AB_ImExporterAccountInfo_free(ai);
      GWEN_XMLNode_free(doc);
      return rv;
    }

    AB_ImExporterContext_AddAccountInfo(ctx, ai);
    node=GWEN_XMLNode_FindNextTag(node, "KONAUS", 0, 0);
  }

  GWEN_XMLNode_free(doc);

  return 0;
}



int AB_ImExporterYN_CheckFile(AB_IMEXPORTER *ie, const char *fname){
  AB_IMEXPORTER_YELLOWNET *ieh;
  int fd;
  GWEN_BUFFEREDIO *bio;

  assert(ie);
  ieh=GWEN_INHERIT_GETDATA(AB_IMEXPORTER, AB_IMEXPORTER_YELLOWNET, ie);
  assert(ieh);

  assert(fname);

  fd=open(fname, O_RDONLY);
  if (fd==-1) {
    /* error */
    DBG_ERROR(AQBANKING_LOGDOMAIN,
	      "open(%s): %s", fname, strerror(errno));
    return AB_ERROR_NOT_FOUND;
  }

  bio=GWEN_BufferedIO_File_new(fd);
  GWEN_BufferedIO_SetReadBuffer(bio, 0, 256);

  while(!GWEN_BufferedIO_CheckEOF(bio)) {
    char lbuffer[1024];
    GWEN_ERRORCODE err;
    const char *p;
    unsigned int bsize;

    bsize=sizeof(lbuffer)-1;
    err=GWEN_BufferedIO_ReadRawForced(bio, lbuffer, &bsize);
    if (!GWEN_Error_IsOk(err)) {
      DBG_INFO(AQBANKING_LOGDOMAIN,
               "File \"%s\" is not supported by this plugin",
               fname);
      GWEN_BufferedIO_Close(bio);
      GWEN_BufferedIO_free(bio);
      return AB_ERROR_BAD_DATA;
    }
    lbuffer[bsize]=0;

    p=strstr(lbuffer, "<?xml");
    if (p)
      p=strstr(p, "<KONAUS>");
    if (p) {
      /* match */
      DBG_INFO(AQBANKING_LOGDOMAIN,
	       "File \"%s\" is supported by this plugin",
	       fname);
      GWEN_BufferedIO_Close(bio);
      GWEN_BufferedIO_free(bio);
      return 0;
    }
  } /* while */

  GWEN_BufferedIO_Close(bio);
  GWEN_BufferedIO_free(bio);
  return AB_ERROR_BAD_DATA;
}


