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
static char const _SignallingEvent_cc_rcsid_[] =
"$Id: SignallingEvent.cc,v 1.4 1998/08/06 04:04:48 bilal Exp $";
#endif
#include <common/cprototypes.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/SimEntity.h>
#include <FW/kernel/Handlers.h>
#include <fsm/events/SignallingEvent.h>


SignallingEvent::SignallingEvent(SimEntity * from, SimEntity * to,
				 int vpi,int vci) :
  SimEvent(from, to, SIGNALLING_EVENT_CODE), _vpi(vpi), _vci(vci)
{ }

SignallingEvent::~SignallingEvent() { }

int SignallingEvent::GetVPI(void) const { return _vpi; }
int SignallingEvent::GetVCI(void) const { return _vci; }

void SignallingEvent::SetVPI(int vpi) { _vpi = vpi; }
void SignallingEvent::SetVCI(int vci) { _vci = vci; }
