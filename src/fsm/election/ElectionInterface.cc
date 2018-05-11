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
static char const _ElectionInterface_cc_rcsid_[] =
"$Id: ElectionInterface.cc,v 1.9 1999/03/05 17:30:32 marsh Exp $";
#endif

#include "ElectionInterface.h"
#include <fsm/election/ElectionState.h>
#include <fsm/election/InternalElection.h>
#include <fsm/election/ElectionVisitor.h>

void ElectionInterface::ElectionUpdate(ElectionVisitor * v)
{
  // Perhaps we should assert that the caller is getting
  //  the correct ElectionState
  assert( v->GetSourceNID()->equals( _eState->GetLocalNID() ) );

  if (v->GetPurpose() == ElectionVisitor::InsertedNodalInfo)
    _eState->GetCS()->InsertNodeID( _eState, v );
  else if (v->GetPurpose() == ElectionVisitor::HorizontalLinkUp ||
	   v->GetPurpose() == ElectionVisitor::HorizontalLinkDown)
    _eState->GetCS()->InsertHorzID( _eState, v );
  v->Suicide();
}

ElectionInterface::ElectionInterface(ElectionState * e)
  : fw_Interface(e), _eState(e) { }

ElectionInterface::~ElectionInterface() { }

void ElectionInterface::ShareableDeath(void)
{ 
  _eState = 0; // assert( !good() ); 
}
