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
#ifndef __DELAYEDSENDTIMER_H__
#define __DELAYEDSENDTIMER_H__

#ifndef LINT
static char const _DelayedSendTimer_h_rcsid_[] =
"$Id: DelayedSendTimer.h,v 1.1 1998/03/30 17:21:22 mountcas Exp $";
#endif

#include <FW/kernel/Handlers.h>
#include <FW/basics/Visitor.h>

class DelayedSendTimer : public TimerHandler {
public:

  DelayedSendTimer(State * s, Visitor * v, 
		   Visitor::which_side side = Visitor::A_SIDE, 
		   double time = 0.0);
  virtual ~DelayedSendTimer();

  void Callback(void);

private:

  State               * _s;
  Visitor             * _v;
  Visitor::which_side   _side;
};

#endif
