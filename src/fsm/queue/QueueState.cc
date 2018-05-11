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
 * File: QueueState.cc
 * Author: talmage
 * Version: $Id: QueueState.cc,v 1.12 1998/09/28 19:00:51 mountcas Exp $
 *
 * Purpose: Implements a queue of depth n.  Every once in a while, the
 * queue sends some data in each direction.  Does not queue
 * LinkDownVisitors or LinkUpVisitors because they model the hardware, not 
 * the data that flows on it.
 *
 * BUGS: 
 */

#ifndef LINT
static char const rcsid[] =
"$Id: QueueState.cc,v 1.12 1998/09/28 19:00:51 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <iostream.h>
#include <FW/kernel/SimEvent.h>
#include <FW/basics/diag.h>
#include <fsm/visitors/LinkDownVisitor.h>
#include <fsm/visitors/LinkUpVisitor.h>
#include <fsm/visitors/QueueVisitor.h>
#include "QueueState.h"

#define MIN_SERVICE_TIME 0.0001

// ----------------------- QueueState -------------------
QueueState::QueueState(double service_time, int depth) 
  : State(), _service_time(service_time), _depth(depth)
{
  if (_service_time <= 0.0)
    _service_time = MIN_SERVICE_TIME;
  if (_depth != 0) // Zero is special case for non-queuing
    _serviceEvent = new SimEvent(this, this, QUEUE_SERVICE_EVENT);
}


QueueState::~QueueState()
{
  Visitor * v;

  // Drain the A-side queue
  while (!_qFromA.empty()) {
    v = _qFromA.pop();
    v->Suicide();
  }
  // Drain the B-side queue
  while (!_qFromB.empty()) {
    v = _qFromB.pop();
    v->Suicide();
  }
  delete _serviceEvent;
}


//
// If the _service_time is greater than or equal to zero, queue the
// Visitor or Suicide it if the queue is full.
//
// If the _service_time is less than zero, send the Visitor to the
// next Conduit without delay.
//
State * QueueState::Handle(Visitor * v)
{
  VisitorType vt = v->GetType();
  
  switch (VisitorFrom(v)) {
    case Visitor::A_SIDE:
      if (Is_An_OverrideType(vt) || !_depth)
	PassVisitorToB(v);
      else if (Is_A_KillMeType(vt))
	v->Suicide();
      else if (_service_time >= 0.0) {
	if ((_qFromA.size() == 0) &&
	    (_qFromB.size() == 0))               // Special case for empty queues
	  Deliver(_serviceEvent, _service_time); // Begin servicing it again

	if (_depth < 0 || _qFromA.size() < _depth)
	  _qFromA.append(v);
	else {
	  diag("fsm.queue", DIAG_DEBUG, "%s is Full -- deleting %s (%x).\n",
	       OwnerName(), v->GetType().Name(), v);
	  v->Suicide();
	}
      }
      break;

    case Visitor::B_SIDE:
      if (Is_An_OverrideType(vt))
	PassVisitorToA(v);
      else if (Is_A_KillMeType(vt))
	v->Suicide();
      else if (_service_time >= 0.0) {
	if ((_qFromA.size() == 0) &&
	    (_qFromB.size() == 0))	// Special case for empty queues
	  Deliver(_serviceEvent, _service_time);	// Begin servicing it again
	if (_depth <= 0 || _qFromB.size() < _depth)
	  _qFromB.append(v);
	else {
	  diag("fsm.queue", DIAG_DEBUG, "%s is Full -- deleting %s (%x).\n",
	       OwnerName(), v->GetType().Name(), v);
	  v->Suicide();
	}
      }
      break;

    case Visitor::OTHER:
      cerr << "State ERROR: A visitor entered the Protocol from an unexpected source.\n";
      v->Suicide();
      break;
  }
  
  diag("fsm.queue", DIAG_DEBUG, "Post Push: QueueA = %d, QueueB = %d\n", 
       _qFromA.size(), _qFromB.size());
  return this;
}


void QueueState::Interrupt(SimEvent *event)
{
  diag("fsm.queue", DIAG_DEBUG, "%s interrupted by %d.\n", 
       OwnerName(), (int)(*event));

  if ((int)(*event) == QUEUE_SERVICE_EVENT) {
    //
    // Send one in the queue from side A to side B.
    //
    if (!_qFromA.empty()) {
      Visitor *v = _qFromA.pop();
      PassVisitorToB(v);
    }
    //
    // Send one in the queue from side B to side A.
    //
    if (!_qFromB.empty()) {
      Visitor *v = _qFromB.pop();
      PassVisitorToA(v);
    }
    //
    // Schedule another service event
    //
    if (!_qFromA.empty() || !_qFromB.empty())
      ReturnToSender(event, _service_time);
    else {
      Cancel(event);
      diag("fsm.queue", DIAG_DEBUG, "%s both queues are empty.\n", 
	   OwnerName());
    }
  } else {
    diag("fsm.queue", DIAG_DEBUG, "QueueState::Interrupt(event = %x, ID = %ld, code = %d)\n",
	 event, (int)(event->GetID()), (int)(*event));
  }
}

bool QueueState::Is_An_OverrideType(const VisitorType t)
{
  VisitorType ot(QueueOverrideType());
  return t.Is_A(&ot);
}


bool QueueState::Is_A_KillMeType(const VisitorType t)
{
  VisitorType ot(QueueKillMeType());
  return t.Is_A(&ot);
}
