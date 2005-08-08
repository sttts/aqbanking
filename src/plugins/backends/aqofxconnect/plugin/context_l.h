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

#ifndef AO_CONTEXT_L_H
#define AO_CONTEXT_L_H


#include <aqofxconnect/user.h>
#include <aqofxconnect/bank.h>
#include <aqbanking/job.h>
#include <aqbanking/imexporter.h>

/* this is a temporary hack to circumvent an ugly bug in LibOFX 0.7 */
#define OFX_AQUAMANIAC_UGLY_HACK1
#include <libofx/libofx.h>


typedef struct AO_CONTEXT AO_CONTEXT;


AO_CONTEXT *AO_Context_new(AO_BANK *bank, AO_USER *user, AB_JOB *job,
                           AB_IMEXPORTER_CONTEXT *ictx);
void AO_Context_free(AO_CONTEXT *ctx);

AO_BANK *AO_Context_GetBank(const AO_CONTEXT *ctx);
AO_USER *AO_Context_GetUser(const AO_CONTEXT *ctx);
AB_JOB *AO_Context_GetJob(const AO_CONTEXT *ctx);

AB_IMEXPORTER_CONTEXT *AO_Context_GetImExContext(const AO_CONTEXT *ctx);

struct OfxFiLogin *AO_Context_GetFi(const AO_CONTEXT *ctx);
struct OfxAccountInfo *AO_Context_GetAi(const AO_CONTEXT *ctx);

LibofxContextPtr AO_Context_GetOfxContext(const AO_CONTEXT *ctx);

AB_IMEXPORTER_ACCOUNTINFO*
  AO_Context_GetLastAccountInfo(const AO_CONTEXT *ctx);
void AO_Context_SetLastAccountInfo(AO_CONTEXT *ctx,
                                   AB_IMEXPORTER_ACCOUNTINFO *ai);

int AO_Context_GetAbort(const AO_CONTEXT *ctx);


int AO_Context_Update(AO_CONTEXT *ctx);

int AO_Context_ProcessImporterContext(AO_CONTEXT *ctx);


#endif