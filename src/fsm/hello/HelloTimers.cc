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
static char const _HelloTimers_cc_rcsid_[] =
"$Id: HelloTimers.cc,v 1.37 1998/08/06 04:04:51 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/kernel/Kernel.h>
#include <FW/kernel/SimEntity.h>
#include <FW/kernel/SimEvent.h>
#include <FW/kernel/Handlers.h>

#include <fsm/hello/HelloTimers.h>
#include <fsm/hello/HelloState.h>
#include <fsm/hello/RCCHelloState.h>
#include <fsm/hello/LgnHelloState.h>

HelloTimer::HelloTimer(HelloState *h, double ht):TimerHandler(h,ht),_owner(h),_t(ht) {};

HelloTimer::~HelloTimer(){};

void HelloTimer::Callback(void)
{
  _owner->GetCS()->ExpHelloTimer(_owner);
  ExpiresIn(_t); Handler::Register(this);  // Changed BK Jan 6
}

InactivityTimer::InactivityTimer(HelloState *h, double it):TimerHandler(h,it),_owner(h),_t(it){};

InactivityTimer::~InactivityTimer(){};

void InactivityTimer::Callback(void)
{
  _owner->GetCS()->ExpInactivityTimer(_owner);
  ExpiresIn(_t); Handler::Register(this);  // Changed BK Jan 6
}


//---------------- LGN ----------------------------------
LgnInactivityTimer::LgnInactivityTimer(LgnHelloState *h, double it)
  : TimerHandler(h, it), _owner(h), _t(it) { }

LgnInactivityTimer::~LgnInactivityTimer() { }

void LgnInactivityTimer::Callback(void)
{
  InternalLgnHelloState * ilhs = (InternalLgnHelloState *)_owner->GetCS();
  ilhs->RcvHLinkInactivityExpiry(_owner);
  ExpiresIn(_t); Handler::Register(this);  // Changed BK Jan 6
}

LgnIntegrityTimer::LgnIntegrityTimer(LgnHelloState *h, double ht):TimerHandler(h,ht),_owner(h),_t(ht){};

LgnIntegrityTimer::~LgnIntegrityTimer(){};

void LgnIntegrityTimer::Callback(void)
{
  ExpiresIn(_t); Handler::Register(this);  // Changed BK Jan 6
}

//---------------- RCCHelloState ----------------------------
RCCHelloTimer::RCCHelloTimer(RCCHelloState * h, double ht)
	: TimerHandler(h, ht), _owner(h), _t(ht) { }

RCCHelloTimer::~RCCHelloTimer() { }

void RCCHelloTimer::Callback(void)
{
  ((InternalRCCHelloState *)(_owner->GetCS()))->ExpHelloTimer(_owner);
  // ExpiresIn(_t); Handler::Register(this);  // Changed BK Jan 6
}

// ----------------- InactivityTimer -----------------------
RCCInactivityTimer::RCCInactivityTimer(RCCHelloState * h, double it)
	: TimerHandler(h, it), _owner(h), _t(it) { }

RCCInactivityTimer::~RCCInactivityTimer() { }

void RCCInactivityTimer::Callback(void)
{
  ((InternalRCCHelloState *)(_owner->GetCS()))->ExpInactivityTimer(_owner);
  // ExpiresIn(_t); Handler::Register(this);  // Changed BK Jan 6
}

// ------------------------- IntegrityTimer ---------------------
RCCIntegrityTimer::RCCIntegrityTimer(RCCHelloState * h, double ht)
  : TimerHandler(h, ht), _owner(h), _t(ht) { }

RCCIntegrityTimer::~RCCIntegrityTimer() { } 

bool RCCIntegrityTimer::IsRunning(void) const
{  return Handler::IsRegistered();  }

void RCCIntegrityTimer::Callback(void)
{
  ((InternalRCCHelloState *)(_owner->GetCS()))->ExpIntegrityTimer(_owner);
}

// ------------------- InitTimer ----------------------
RCCInitTimer::RCCInitTimer(RCCHelloState * s, double t)
  : TimerHandler(s, t), _owner(s), _t(t) { }

RCCInitTimer::~RCCInitTimer() { }

void RCCInitTimer::Callback(void)
{
  ((InternalRCCHelloState *)(_owner->GetCS()))->ExpInitLGNSVCTimer(_owner);
}

// ---------------------- RetryTimer -------------------
RetryLGNSVCTimer::RetryLGNSVCTimer(RCCHelloState *s, double t)
  : TimerHandler(s, t), _owner(s), _t(t) { }

RetryLGNSVCTimer::~RetryLGNSVCTimer() { }

void RetryLGNSVCTimer::Callback(void)
{
  ((InternalRCCHelloState *)(_owner->GetCS()))->ExpRetryLGNSVCTimer(_owner);
}
