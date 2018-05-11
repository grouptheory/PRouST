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
 * File: SwitchFunctions.h
 * Author: talmage
 * Version: $Id: SwitchFunctions.h,v 1.30 1999/02/19 21:22:54 marsh Exp $
 * Purpose: Shortcuts to creating and connecting switches.
 * BUGS:
 */

#ifndef __SWITCH_FUNCTIONS_H__
#define __SWITCH_FUNCTIONS_H__

#ifndef LINT
static char const _SwitchFunctions_h_rcsid_[] =
"$Id: SwitchFunctions.h,v 1.30 1999/02/19 21:22:54 marsh Exp $";
#endif

class Addr;
class Conduit;
class LinkStateWrapper;
class NodeID;
class ig_resrc_avail_info;
class ds_String;

#include <DS/containers/list.h>


//
// Create a switch with name switchName.  Its characteristics come from
// the Configurator using switchKey as the key.
//
// To pass a Visitor into the top of the switch, wrap the
// Conduit in A_half().  I.e. A_half(switch)->Accept(visitor);
//
extern Conduit *MakeSwitch(ds_String *switchKey,
			   ds_String *switchName, 
			   NodeID *& myNode,
			   Conduit *& controlTerminal);


extern const u_long OC3;
extern const u_long OC12;
extern const u_long OC48;

//
// Link port1 of switch1 to port2 of switch2.
// Port2 defaults to port1.  PortVisitors that leave switch1
// through port1 arrive at switch2 port2 with their port number
// stamped as port2.  PortVisitors that leave switch2
// through port2 arrive at switch1 port1 with their port number
// stamped as port1.
//
// Returns a pointer to a LinkStateWrapper, an object that contains
// the context of the linking of the two ports.
//
extern LinkStateWrapper * LinkSwitches(Conduit *switch1, int port1, 
				       Conduit *switch2, int port2 = -1,
				       list<ig_resrc_avail_info *> * fwd_raigs = 0,
				       list<ig_resrc_avail_info *> * fwd_raigs = 0,
				       // The distinguished value 0 conveys the 
				       // meaning that the link does not care 
				       // about aggregation behavior.
				       u_int aggregation_token = 0);

extern LinkStateWrapper * LinkSwitches(Conduit * switch1, int port1, 
				       Conduit * switch2, int port2,
				       long fwd_bandwidth, 
				       long bwd_bandwidth,
				       // The distinguished value 0 conveys the 
				       // meaning that the link does not care 
				       // about aggregation behavior.
				       u_int aggregation_token = 0);

//
// The link is aggregated using the forward raigs aggregation token.
//
extern LinkStateWrapper * LinkSwitches(Conduit *switch1, int port1, 
				       Conduit *switch2, int port2,
				       ds_String *fwd_raigs_key, 
				       ds_String *bwd_raigs_key);
//
// Unlink two switch ports using the context returned by LinkSwitches().
//
extern bool UnlinkSwitches(LinkStateWrapper *link);

#endif // __SWITCH_FUNCTIONS_H__
