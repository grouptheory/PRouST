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
#ifndef _Q93B_MSG_H_
#define _Q93B_MSG_H_

#ifndef LINT
static char const _q93b_msg_h_rcsid_[] =
"$Id: q93b_msg.h,v 1.3 1997/09/24 18:44:00 mountcas Exp $";
#endif

#include <codec/q93b_msg/add_party.h>
#include <codec/q93b_msg/generic_q93b_msg.h>
#include <codec/q93b_msg/add_party_ack.h>
#include <codec/q93b_msg/header_parser.h>
#include <codec/q93b_msg/add_party_rej.h>
#include <codec/q93b_msg/release.h>
#include <codec/q93b_msg/body_parser.h>
#include <codec/q93b_msg/release_comp.h>
#include <codec/q93b_msg/call_proceeding.h>
#include <codec/q93b_msg/restart.h>
#include <codec/q93b_msg/connect.h>
#include <codec/q93b_msg/restart_ack.h>
#include <codec/q93b_msg/connect_ack.h>
#include <codec/q93b_msg/setup.h>
#include <codec/q93b_msg/drop_party.h>
#include <codec/q93b_msg/status.h>
#include <codec/q93b_msg/drop_party_ack.h>
#include <codec/q93b_msg/status_enq.h>
#include <codec/q93b_msg/freeform.h>
// UNI40
#include <codec/q93b_msg/leaf_setup.h>

#endif // _Q93B_MSG_H_
