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

/* -*- C++ -*-
 * File: NullState.cc
 * Author: 
 * Version: $Id: NullState.cc,v 1.6 1997/09/15 18:54:17 talmage Exp $
 * Purpose: Implements a do-nothing FSM.  It just passes Visitors along.
 * BUGS:
 */

#if !defined(LINT)
static char const rcsid[] =
"$Id: NullState.cc,v 1.6 1997/09/15 18:54:17 talmage Exp $";
#endif


#include <FW/basics/diag.h>
#include "NullState.h"
#include <iostream.h>

// ----------------------- NullState -------------------

NullState::NullState(void) : State() { }

NullState::~NullState() { }

State * NullState::Handle(Visitor * v)
{
  DIAG(SIM, DIAG_DEBUG, 
       cout << OwnerName() << " received " << v->GetType() 
       << "(" << hex << v << dec << ")." << endl;)

  switch (VisitorFrom(v)) {
  case Visitor::A_SIDE:
    PassVisitorToB(v);
    break;
  case Visitor::B_SIDE:
    PassVisitorToA(v);
    break;
  case Visitor::OTHER:
    diag(SIM, DIAG_ERROR, 
	 "State ERROR: A visitor entered the Protocol from an unexpected \
source.\n");
    break;
  }

  return this;
}


void NullState::Interrupt(SimEvent *event)
{
  DIAG(SIM, DIAG_DEBUG, 
       cout << OwnerName() << " interrupted by " << (int)event << endl;)
}

