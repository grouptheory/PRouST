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
static char const _SimEvent_cc_rcsid_[] =
"$Id: SimEvent.cc,v 1.1 1999/01/13 19:12:37 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/kernel/SimEvent.h>
#include <FW/kernel/Kernel.h>

// ----------------------- SimEvent -------------------------
SimEvent::SimEvent(SimEntity* from, SimEntity* to, int code)
  : _from(from), _to(to), _code(code), _reg(false) { }

SimEvent::~SimEvent() 
{  
  if (IsRegistered()) Deregister();  
}

bool SimEvent::Deregister(void)
{
  bool res = false;
  if ((_reg) && (! (res = theKernel().Deregister(_token))))
#ifdef DEBUG_FW
    cerr << "ERROR: Unable to deregister SimEvent with Kernel." << endl;
#else 
    ;
#endif
  _reg = false;
  return res;
}

bool SimEvent::Register(void) {  return theKernel().Register(this); }

bool SimEvent::IsRegistered(void) const { return theKernel().IsRegistered(_token); }

void SimEvent::SetToken(KernelToken kt) { _token = kt; }

void SimEvent::Print(ostream & os)
{
  os << "Event " << _token << " from " << _from << " destined for " 
     << _to << " with a code of " << _code << "." << endl;
}

