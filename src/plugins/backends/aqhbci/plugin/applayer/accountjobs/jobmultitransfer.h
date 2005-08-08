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


#ifndef AH_JOBMULTITRANSFER_H
#define AH_JOBMULTITRANSFER_H


#include <aqhbci/accountjob.h>

#include <gwenhywfar/gwentime.h>


#ifdef __cplusplus
extern "C" {
#endif


AH_JOB *AH_Job_MultiTransfer_new(AH_CUSTOMER *cu,
                                 AH_ACCOUNT *account);

AH_JOB *AH_Job_MultiDebitNote_new(AH_CUSTOMER *cu,
                                  AH_ACCOUNT *account);


int AH_Job_MultiTransferBase_GetTransferCount(AH_JOB *j);

int AH_Job_MultiTransferBase_GetMaxTransfers(AH_JOB *j);


#ifdef __cplusplus
}
#endif


#endif /* AH_JOBMULTITRANSFER_H */

