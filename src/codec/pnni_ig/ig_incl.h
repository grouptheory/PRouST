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
#ifndef __IG_INCL_H__
#define __IG_INCL_H__

#ifndef LINT
static char const _ig_incl_h_rcsid_[] =
"$Id: ig_incl.h,v 1.10 1998/03/03 15:04:05 mountcas Exp $";
#endif

#include <iostream.h>
#include <common/cprototypes.h>

extern "C" {
#include <sys/types.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
};

#include <codec/pnni_ig/InfoGroup.h>
#include <codec/pnni_ig/aggregation_token.h>
#include <codec/pnni_ig/pnni_errmsg.h>
#include <codec/pnni_ig/external_reachable_atm_addr.h>
#include <codec/pnni_ig/DBKey.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/nodal_ptse_ack.h>
#include <codec/pnni_ig/nodal_ptse_summary.h>
#include <codec/pnni_ig/nodal_state_params.h>
#include <codec/pnni_ig/optional_gcac_params.h>
#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/req_ptse_header.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/internal_reachable_atm_addr.h>
#include <codec/pnni_ig/system_capabilities.h>
#include <codec/pnni_ig/lgn_horizontal_link_ext.h>
#include <codec/pnni_ig/transit_net_id.h>
#include <codec/pnni_ig/next_hi_level_binding_info.h>
#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/pnni_ig/nodal_hierarchy_list.h>
#include <codec/pnni_ig/uplinks.h>

void HexDump(const u_char * buffer, int len, ostream & os, bool nid = false);

//@Include: InfoGroup.h aggregation_token.h pnni_errmsg.h ext_reach_atm_addr.h
//@Include: horizon_links.h ig.h int_reach_atm_addr.h
//@Include: lgn_horiz_link.h
//@Include: nodal_info.h nodal_list.h nodal_ptse_ack.h 
//@Include: nodal_ptse_summary.h nodal_state.h nxt_hilvl_bind_info.h
//@Include: opt_gcac_params.h ptse.h req_ptse_header.h
//@Include: uplink_info.h uplinks.h resrc_avail_info.h syscap.h
//@Include: transit_net_id.h

#endif // __IG_INCL_H__
