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


#ifndef AQBANKING_JOBGETSTANDINGORDERS_P_H
#define AQBANKING_JOBGETSTANDINGORDERS_P_H


#include <aqbanking/job.h>
#include "jobgetstandingorders_l.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct AB_JOB_GETSTANDINGORDERS AB_JOB_GETSTANDINGORDERS;
struct AB_JOB_GETSTANDINGORDERS {
  /* arguments */

  /* parameters */

  /* responses */
  AB_TRANSACTION_LIST2 *standingOrders;

};

void AB_JobGetStandingOrders_FreeData(void *bp, void *p);



#ifdef __cplusplus
}
#endif


#endif /* AQBANKING_JOBGETSTANDINGORDERS_P_H */

