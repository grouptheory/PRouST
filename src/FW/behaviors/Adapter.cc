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
static char const _Adapter_cc_rcsid_[] =
"$Id: Adapter.cc,v 1.1 1999/01/13 19:12:50 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/behaviors/Adapter.h>

#include <FW/actors/Terminal.h>
#include <FW/basics/Visitor.h>
#include <FW/kernel/KDB.h>

//------------- ADAPTER ----------------------------
Adapter::Adapter(Terminal * t) : Behavior(t, Behavior::ADAPTER) { }

Adapter::~Adapter() { }

void Adapter::Accept(Visitor * v, int is_breakpoint) 
{
  if (is_breakpoint) {
    ALERT_DEBUGGER;
  }
  v->_at(this, (Terminal *)_actor);
}

bool Adapter::ConnectB(Conduit * b, Visitor * v) { return false; }
bool Adapter::DisconnectB(Conduit * b, Visitor * v) { return false; }
