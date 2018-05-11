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
static char const _NodePeerTimers_cc_rcsid_[] =
"$Id: NodePeerTimers.cc,v 1.12 1998/08/17 16:06:50 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <fsm/nodepeer/NodePeerTimers.h>
#include <fsm/nodepeer/NodePeerState.h>

DSRxTimer::DSRxTimer(NodePeerState *s, double time)
  : TimerHandler(s, time), _owner(s) { }

DSRxTimer::~DSRxTimer() { }

void DSRxTimer::Callback(void)
{
  _owner->ExpireDSTimer();
  
}

ReqRxTimer::ReqRxTimer(NodePeerState *s, double time)
  : TimerHandler(s, time), _owner(s) { }

ReqRxTimer::~ReqRxTimer() { }

void ReqRxTimer::Callback(void)
{ 
  _owner->ExpireReqTimer();
}

AckTimer::AckTimer(NodePeerState *s, double time)
  : TimerHandler(s, time), _owner(s) { }

AckTimer::~AckTimer() { }

void AckTimer::Callback(void)
{ 
  _owner->ExpireAckTimer();
}

PTSPRxTimer::PTSPRxTimer(NodePeerState *s, PTSPPkt *p, double time)
  : TimerHandler(s, time), _owner(s), _for(p){ }

PTSPRxTimer::~PTSPRxTimer() { }

void PTSPRxTimer::Callback(void)
{
  _owner->ExpirePTSPTimer(_for, this);
}

