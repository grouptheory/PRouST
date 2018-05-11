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
static char const _ElectionTimers_cc_rcsid_[] =
"$Id: ElectionTimers.cc,v 1.9 1998/09/03 17:32:32 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/SimEntity.h>
#include <FW/kernel/SimEvent.h>
#include <FW/kernel/Handlers.h>
#include <fsm/election/ElectionTimers.h>
#include <fsm/election/ElectionState.h>
#include <fsm/election/InternalElection.h>

SearchPeerTimer::SearchPeerTimer(ElectionState *e, double t) : 
  TimerHandler(e, t), _owner(e) { ; }

SearchPeerTimer::~SearchPeerTimer(){};

void SearchPeerTimer::Callback(void)
{
  _owner->GetCS()->SearchPeerTimerExpiry(_owner);
  // ExpiresIn(_t); Handler::Register(this);
}


ReelectionTimer::ReelectionTimer(ElectionState *e, double t):TimerHandler(e,t),_owner(e){};

ReelectionTimer::~ReelectionTimer(){};

void ReelectionTimer::Callback(void)
{
  _owner->GetCS()->ReelectionTimerExpiry(_owner);
  // ExpiresIn(_t); Handler::Register(this);
}


PGLInitTimer::PGLInitTimer(ElectionState *e, double t):TimerHandler(e,t),_owner(e){};

PGLInitTimer::~PGLInitTimer(){};

void PGLInitTimer::Callback(void)
{
  _owner->GetCS()->PGLInitTimerExpiry(_owner);
  // ExpiresIn(_t); Handler::Register(this);
}


OverrideUnanimityTimer::OverrideUnanimityTimer(ElectionState *e, double t):TimerHandler(e,t),_owner(e){};

OverrideUnanimityTimer::~OverrideUnanimityTimer(){};

void OverrideUnanimityTimer::Callback(void)
{
  _owner->GetCS()->OverrideUnanimityExpiry(_owner);
  // ExpiresIn(_t); Handler::Register(this);
}








