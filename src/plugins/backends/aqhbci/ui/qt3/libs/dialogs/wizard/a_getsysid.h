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


#ifndef AQHBCI_A_GETSYSID_H
#define AQHBCI_A_GETSYSID_H

#include "waction.h"
#include "winfo.h"



class ActionGetSysId: public WizardAction {
public:
  ActionGetSysId(Wizard *w);
  virtual ~ActionGetSysId();

  virtual bool apply();

};


#endif