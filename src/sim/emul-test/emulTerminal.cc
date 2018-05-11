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

#ifndef LINT
static char const _emulTerminal_cc_rcsid_[] =
"$Id: emulTerminal.cc,v 1.1 1999/03/05 19:25:26 battou Exp $";
#endif

#include "emulTerminal.h"

#include <FW/kernel/SimEvent.h>
#include <FW/kernel/SimEntity.h>
#include <fsm/visitors/FileIOVisitors.h>

EmulTerminal::EmulTerminal(const NodeID * n) : SwitchTerminal(n) {
  SimEvent* se = new SimEvent(this,this,0);

  char *ttlstr = getenv("TTL");

  if (ttlstr)
    _ttl = atof(ttlstr);
  else
    _ttl = 240.0;

  _ctr = 1;

  cout << "TOPGUN--- " << _ttl << " seconds from now" << endl;
  Deliver(se, _ttl );
}

void EmulTerminal::Absorb(Visitor * v) {
}

void EmulTerminal::Interrupt(SimEvent * event) {
  int code = event->GetCode();
  cout << "TOPGUN-- code=" << code << endl;
  delete event;

  char buf[255];
  sprintf(buf,"netgraph.%d",_ctr);

  SaveVisitor* sv = new SaveVisitor(buf);
  cout << "TOPGUN--- Sending SaveVisitor " << _ctr << endl;
  Inject(sv);
  cout << "TOPGUN--- Sent SaveVisitor" << _ctr << endl;

  SimEvent* se = new SimEvent(this,this,1);
  Deliver(se, _ttl);
  _ctr++;
}


void EmulTerminal::InjectionNotification(const Visitor * v) const {
}



