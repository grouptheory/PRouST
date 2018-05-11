// -*- C++ -*-
// +++++++++++++++
//   P R o u S T     ---  PNNI Routing Simulation Toolkit  ---
// +++++++++++++++
// Version: 1.0 (Fri Mar  5 14:35:33 EST 1999)
// 
// 			  Copyright (c) 1998
// 		 Naval Research Laboratory (NRL/CCS)
// 			       and the
// 	  Defense Advanced Research Projects Agency (DARPA)
// 
// 			 All Rights Reserved.
// 
// Permission to use, copy, and modify this software and its
// documentation is hereby granted, provided that both the copyright notice and
// this permission notice appear in all copies of the software, derivative
// works or modified versions, and any portions thereof, and that both notices
// appear in supporting documentation.
// 
// NRL AND DARPA ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
// DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM
// THE USE OF THIS SOFTWARE.
// 
// NRL and DARPA request users of this software to return modifications,
// improvements or extensions that they make to:
// 
//                 proust-dev@cmf.nrl.navy.mil
//                         -or-
//                Naval Research Laboratory, Code 5590
//                Center for Computation Science
//                Washington, D.C.  20375
// 
// and grant NRL and DARPA the rights to redistribute these changes in
// future upgrades.
//

// -*- C++ -*-
#ifndef LINT
static char const _DBtrapper_cc_rcsid_[] =
"$Id: DBtrapper.cc,v 1.8 1998/08/06 04:04:49 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <stdio.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/Visitor.h>
#include <FW/behaviors/Protocol.h>
#include <FW/actors/State.h>
#include <FW/kernel/Kernel.h>
#include <FW/basics/diag.h>

#include <fsm/visitors/DBVisitors.h>
#include <fsm/hello/DBtrapper.h>

#include <fsm/visitors/LinkVisitor.h>
#include <fsm/visitors/LinkUpVisitor.h>

LinkupSendingTimer::LinkupSendingTimer(DBtrapper *t, Visitor *v1, Visitor* v2)
  : TimerHandler(t,0),_t(t),_v1(v1),_v2(v2) { }

LinkupSendingTimer::~LinkupSendingTimer() { }

void LinkupSendingTimer::Callback(void)
{  _t->BootupLink(_v1,_v2);  }


//------------------------------------------
State * DBtrapper::Handle(Visitor * v)
{
  const VisitorType * vt2 = QueryRegistry(DB_INTRO_VISITOR_NAME);
  VisitorType vt1 = v->GetType();

  if (vt2 && vt1.Is_A(vt2)) {
    DIAG(FSM, DIAG_DEBUG, cout <<  OwnerName() << ":" << " Got the sucker !!" << endl;);
    _gotit = 1;
    v->Suicide();
  } else 
    PassThru(v);
  delete vt2;
  return this;
}

DBtrapper::DBtrapper(void)
{
  Visitor *v1 = new LinkUpVisitor(1);
  Visitor *v2 = new LinkUpVisitor(1);
  _go1 = new LinkupSendingTimer(this,v1,v2);
  Register(_go1);
}

DBtrapper::~DBtrapper() { } 

void DBtrapper::Interrupt(SimEvent *ev) { }

void DBtrapper::BootupLink(Visitor* a, Visitor* b)
{
  PassVisitorToA(a);
  PassVisitorToB(b);
}

