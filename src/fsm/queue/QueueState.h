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
 * Author: 
 * Version: $Id: QueueState.h,v 1.7 1999/02/19 22:42:18 marsh Exp $
 *
 * Purpose: Implements a queue of depth n.  Every once in a while, the
 * ds_queue sends some data in each direction.  Does not queue
 * QueueOverrider-type Visitors because they model the hardware,
 * not the data that flows on it.  Kills QueueKillMe-type Visitors.
 *
 * 
 * BUGS: 
 */

#ifndef __QUEUE_STATE_H__
#define __QUEUE_STATE_H__
#ifndef LINT
static char const _QueueState_h_rcsid_[] =
"$Id: QueueState.h,v 1.7 1999/02/19 22:42:18 marsh Exp $";
#endif

#include <FW/actors/State.h>
#include <fsm/visitors/QueueVisitor.h>
#include <DS/containers/queue.h>

// Queue up to 'depth' Visitors in each direction.  Every 'service_time'
// seconds, send one visitor in each direction.  
//
//service_time		depth		Interpretation
//------------		-----		--------------
//	<= 0				Treat as if service_time == 0.0001.
//
//	> 0		> 0		Queue at most 'depth' Visitors in each
//					direction.  Send one in each direction
//					every 'service_time' seconds.
//
//	> 0		<= 0		Queue at most QueueState::DEFAULT_DEPTH
//					Visitors.  Send one in each direction
//					every 'service_time' seconds.
//


//
// The QUEUE_SERVICE_EVENT tells a QueueState object to
// service its queues.  That is, to deliver some of the data 
// it has queued since the last QUEUE_SERVICE_EVENT.
//
#define QUEUE_SERVICE_EVENT 1000

#define QUEUE_OVERRIDER_NAME "QueueOverrider"
#define QUEUE_KILL_ME_NAME "QueueKillMe"

class QueueState: public State {
public:

  static const int DEFAULT_DEPTH = 100;

  QueueState(double service_time, int depth = QueueState::DEFAULT_DEPTH);

  virtual State * Handle(Visitor* v);
  virtual void Interrupt(SimEvent *event);

  static bool Is_An_OverrideType(const VisitorType t);
  static bool Is_A_KillMeType(const VisitorType t);

protected:

  virtual ~QueueState();
  int _depth;
  double _service_time;
  ds_queue<Visitor *> _qFromA;	// Visitors that enter on the A side
  ds_queue<Visitor *> _qFromB;	// Visitors that enter on the B side

  SimEvent * _serviceEvent;	/* Occurs every _service_time seconds.
				 * Reused throughout the lifetime of
				 * the QueueState.  Destroyed by the
				 * destructor.
				 */
};

#endif

