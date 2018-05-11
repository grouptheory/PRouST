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
 * @file ACAC_DIAG.cc
 * @author talmage
 * @version $Id: ACAC_DIAG.h,v 1.4 1999/02/11 20:58:01 talmage Exp $
 *
 * This is the place where the diag masks for ACAC are defined.
 */

#if !defined(_ACAC_DIAG_H_)
#define _ACAC_DIAG_H_

#ifndef LINT
static char const _ACAC_DIAG_h_rcsid_[] =
"$Id: ACAC_DIAG.h,v 1.4 1999/02/11 20:58:01 talmage Exp $";
#endif

//
// Use SIM_ACAC_CALL to learn about the details of call admission and
// release.
//
#define SIM_ACAC_CALL		"sim.acac.call"

//
// Use SIM_ACAC_CALL_ADMISSION to learn about the details of
// call admission control.
//
#define SIM_ACAC_CALL_ADMISSION	"sim.acac.call.admission"

//
// Use SIM_ACAC_CALL_ADMISSION_POLICY to learn about the details of
// call admission control policy including the reason for rejecting a
// call SETUP.
//
#define SIM_ACAC_CALL_ADMISSION_POLICY	"sim.acac.call.admission.policy"

//
// Use SIM_ACAC_CALL_ADMISSION_RECORDS to learn about the details of
// call admission control record keeping including how much bandwidth
// is available after call admission.
//
#define SIM_ACAC_CALL_ADMISSION_RECORDS	"sim.acac.call.admission.records"

//
// Use SIM_ACAC_CALL_RELEASE to learn about the details of releasing
// calls, including crankback.
//
#define SIM_ACAC_CALL_RELEASE	"sim.acac.call.release"

//
// Use SIM_ACAC_CALL_RELEASE_RECORDS to learn about the details of
// call release record keeping, including how much bandwidth is
// available after call release.
//
#define SIM_ACAC_CALL_RELEASE_RECORDS	"sim.acac.call.release.records"

//
// Use SIM_ACAC_BORDER to learn about border nodes going up
// and down.
//
#define SIM_ACAC_BORDER		"sim.acac.border"

//
// Use SIM_ACAC_BORDER_DOWN to learn about border nodes going down.
//
#define SIM_ACAC_BORDER_DOWN	"sim.acac.border.down"

//
// Use SIM_ACAC_BORDER_UP to learn about border nodes going up.
//
#define SIM_ACAC_BORDER_UP	"sim.acac.border.up"

//
// Use SIM_ACAC_IG to learn about the origination, reorigination, and
// expiration of information groups.
//
#define SIM_ACAC_IG		"sim.acac.ig"

//
// Use SIM_ACAC_IG_HLINK to learn about the origination,
// reorigination, and expiration of horizontal link IGs.
//
#define SIM_ACAC_IG_HLINK	"sim.acac.ig.hlink"

//
// Use SIM_ACAC_IG_UPLINK to learn about the origination,
// reorigination, and expiration of uplink IGs.
//
#define SIM_ACAC_IG_UPLINK	"sim.acac.ig.uplink"

//
// Use SIM_ACAC_NODE_PEER to learn about NodePeer FSMs transitions
// into and out of FullState.  This applies to all kinds of ports.
//
#define SIM_ACAC_NODE_PEER	"sim.acac.node-peer"

//
// Use SIM_ACAC_PORT to learn about physical ports going up
// and down. 
//
#define SIM_ACAC_PORT		"sim.acac.port"

//
// Use SIM_ACAC_PORT_DOWN to learn about physical ports going down. 
//
#define SIM_ACAC_PORT_DOWN	"sim.acac.port.down"

//
// Use SIM_ACAC_PORT_UP to learn about physical ports going up.
//
#define SIM_ACAC_PORT_UP	"sim.acac.port.up"

#endif
