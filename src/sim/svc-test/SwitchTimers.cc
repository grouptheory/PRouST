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
 * File: SwitchTimers.cc
 * @author: talmage
 * Version: $Id: SwitchTimers.cc,v 1.4 1998/08/06 04:05:45 bilal Exp $
 *
 * Purpose: Implements classes that create or sever a connection
 * between two switches after some period of time.  
 */

#ifndef LINT
static char const _SwitchTimers_cc_rcsid_[] =
"$Id: SwitchTimers.cc,v 1.4 1998/08/06 04:05:45 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include "SwitchTimers.h"
#include <FW/basics/diag.h>
#include <sim/port/LinkStateWrapper.h>
#include <sim/switch/SwitchFunctions.h>
#include <sim/switch/SwitchTerminal.h>

// ------------------ UnlinkSwitchesTimer -----------------------------
UnlinkSwitchesTimer::UnlinkSwitchesTimer(double time, 
					 double unlink_interval, 
					 LinkStateWrapper *link,
					 double relink_interval,
					 LinkSwitchesTimer *relinker):
  TimerHandler(time), _unlink_interval(unlink_interval), _link(link), 
  _relink_interval(relink_interval), _relinker(relinker)
{
}

UnlinkSwitchesTimer::~UnlinkSwitchesTimer() { }

void UnlinkSwitchesTimer::Callback(void) 
{
  Conduit *switch1 = _link->LeftNeighbor();
  Conduit *switch2 = _link->RightNeighbor();
  int port1 = _link->LeftPort();
  int port2 = _link->RightPort();
  LinkStateWrapper *new_link = 0;

  DIAG("sim.switch.unlink", DIAG_DEBUG, 
       cout << "At time " << theKernel().CurrentTime() 
       << ": Unlinking switches at either end of link "
       << hex << _link << dec << endl; );

  if (UnlinkSwitches(_link))
    diag("sim.switch.unlink", DIAG_DEBUG, "Succeeded unlinking\n");
  else
    diag("sim.switch.unlink", DIAG_DEBUG, "Failed unlinking\n");

  if (_relink_interval > 0.0) {

    DIAG("sim.switch.unlink", DIAG_DEBUG, 
	 cout << "Will relink in " << _relink_interval << " seconds"
	 << endl; );


    if (_relinker == 0) {
      _relinker = new LinkSwitchesTimer(_relink_interval,
					_relink_interval,
					switch1, port1, 
					switch2, port2,
					_unlink_interval,
					this);

      Register(_relinker);
    } else {
      _relinker->ExpiresIn(_relink_interval);
    }
  }

  _link = 0;
}


void UnlinkSwitchesTimer::LinkState(LinkStateWrapper *link)
{
  if (_link) delete _link;

  _link = link;
}


// ------------------ LinkSwitchesTimer -----------------------------
LinkSwitchesTimer::LinkSwitchesTimer(double time, 
				     double link_interval, 
				     Conduit *switch1, int port1,
				     Conduit *switch2, int port2,
				     double unlink_interval,
				     UnlinkSwitchesTimer *unlinker) :
  TimerHandler(time), _relink_interval(link_interval),
  _switch1(switch1), _port1(port1),
  _switch2(switch2), _port2(port2),
  _unlink_interval(unlink_interval), _unlinker(unlinker),
  _link(0)
{
}


LinkSwitchesTimer::~LinkSwitchesTimer() 
{
  delete _link;
}

void LinkSwitchesTimer::Callback(void) 
{
  LinkStateWrapper *new_link = 0;

  DIAG("sim.switch.link", DIAG_DEBUG, 
       cout << "At time " << theKernel().CurrentTime() 
       << ": Linking switch "
       << hex << _switch1 << dec << " port " << _port1 
       << " with switch "
       << hex << _switch2 << dec << " port " << _port2 
       << endl; );

  // This will fail if either of the switches is already linked.
  if ((new_link = LinkSwitches(_switch1, _port1, _switch2, _port2)) != 0) {
    delete _link;	// Can't hurt.
    _link = new_link;
    diag("sim.switch.link", DIAG_DEBUG, "Succeeded linking\n");
  } else diag("sim.switch.link", DIAG_DEBUG, "Failed linking\n");

  if (_unlink_interval > 0) {
    DIAG("sim.switch.link", DIAG_DEBUG, 
	 cout << "Will unlink in " << _unlink_interval << " seconds"
	 << endl; );

    if (_unlinker == 0) {
      _unlinker = new UnlinkSwitchesTimer(_unlink_interval, _unlink_interval, 
					  new_link, 
					  _relink_interval, this);
      Register(_unlinker);
    } else {
      _unlinker->LinkState(new_link);
      _unlinker->ExpiresIn(_unlink_interval);
    }
  }
}
