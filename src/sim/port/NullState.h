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
 * Version: $Id: NullState.h,v 1.7 1997/08/19 22:28:56 talmage Exp $
 * Purpose: Implements a do-nothing FSM.  It just passes Visitors along.
 * BUGS:
 */

#ifndef __NULL_STATE_H__
#define __NULL_STATE_H__
#ifndef LINT
static char const _NullState_h_rcsid_[] =
"$Id: NullState.h,v 1.7 1997/08/19 22:28:56 talmage Exp $";
#endif

#include <FW/actors/State.h>

// When deriving a new State these methods must be defined:
//
//      // Handles the passed Visitor and returns itself or a new State object.
//      State * Handle(Visitor * v);
//

// NullState passes a Visitor along and returns itself (thus there is only one State).
class SimEvent;

class NullState: public State {
public:

  NullState(void);

  virtual State * Handle(Visitor* v);
  virtual void Interrupt(SimEvent *);

protected:
  virtual ~NullState();
};

#endif
