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
 * File: SwitchTimers.h
 * @author: talmage
 * Version: $Id: SwitchTimers.h,v 1.2 1998/05/12 18:15:21 talmage Exp talmage $
 * Purpose: After some period of time, it severs a connection 
 * between two switches.
 */

#if !defined(__UNLINK_SWITCHES_TIMER_H__)
#define __UNLINK_SWITCHES_TIMER_H__

#if !defined(LINT)
static char const _SwitchTimers_h_rcsid_[] =
"$Id: SwitchTimers.h,v 1.2 1998/05/12 18:15:21 talmage Exp talmage $";
#endif

#include <FW/kernel/Handlers.h>
class LinkStateWrapper;
class LinkSwitchesTimer;
class SwitchTerminal;

class UnlinkSwitchesTimer : public TimerHandler {
public:

  UnlinkSwitchesTimer(double t, double unlink_interval,
		      LinkStateWrapper *link,
		      double relink_interval = 0.0, 
		      LinkSwitchesTimer *relinker = 0);
  virtual ~UnlinkSwitchesTimer();

  virtual void Callback(void);

  virtual void LinkState(LinkStateWrapper *link = 0);

protected:
  double _unlink_interval;
  LinkStateWrapper *_link;
  double _relink_interval;
  LinkSwitchesTimer *_relinker;
};


class LinkSwitchesTimer : public TimerHandler {
public:

  LinkSwitchesTimer(double t, double link_interval,
		    Conduit *switch1, int port1,
		    Conduit *switch2, int port2,
		    double unlink_interval = 0.0, 
		    UnlinkSwitchesTimer *unlinker = 0);
  virtual ~LinkSwitchesTimer();

  virtual void Callback(void);

protected:
  double _relink_interval;

  Conduit *_switch1;
  Conduit *_switch2;
  int _port1;
  int _port2;

  double _unlink_interval;
  UnlinkSwitchesTimer *_unlinker;

  LinkStateWrapper *_link;
};

#endif
