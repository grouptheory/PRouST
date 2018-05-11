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

/// To be included in all clients of the UNI IE lib.
#ifndef __IE_H__
#define __IE_H__
#if !defined(LINT)
static char const _ie_h_rcsid_[] =
"$Id: ie.h,v 1.4 1998/09/28 09:12:00 bilal Exp $";
#endif

#ifndef UNI40
#define UNI40
#endif

#include <codec/uni_ie/aal.h>
#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/bhli.h>
#include <codec/uni_ie/blli.h>
#include <codec/uni_ie/broadband_repeat_ind.h>
#include <codec/uni_ie/broadband_send_comp_ind.h>
#include <codec/uni_ie/cache.h>
#include <codec/uni_ie/call_state.h>
#include <codec/uni_ie/called_party_num.h>
#include <codec/uni_ie/called_party_subaddr.h>
#include <codec/uni_ie/calling_party_num.h>
#include <codec/uni_ie/calling_party_subaddr.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/conn_id.h>
#include <codec/uni_ie/end_pt_ref.h>
#include <codec/uni_ie/end_pt_state.h>
#include <codec/uni_ie/errmsg.h>
#include <codec/uni_ie/qos_param.h>
#include <codec/uni_ie/restart_ind.h>
#include <codec/uni_ie/subaddr.h>
#include <codec/uni_ie/transit_net_sel.h>
#include <codec/uni_ie/td.h>

// Dependencies for UNI4.0/3.1
#ifdef UNI40
#include <codec/uni_ie/UNI40_abr.h>
#include <codec/uni_ie/UNI40_bbc.h>
#include <codec/uni_ie/UNI40_conn_scope.h>
#include <codec/uni_ie/UNI40_e2e_trans_delay.h>
#include <codec/uni_ie/UNI40_generic.h>
#include <codec/uni_ie/UNI40_leaf.h>
#include <codec/uni_ie/UNI40_notif.h>
#include <codec/uni_ie/UNI40_td.h>
#include <codec/uni_ie/UNI40_xqos.h>
#include <codec/uni_ie/PNNI_called_party_pvpc.h>
#include <codec/uni_ie/PNNI_calling_party_pvpc.h>
#include <codec/uni_ie/PNNI_crankback.h>
#include <codec/uni_ie/PNNI_designated_transit_list.h>
#endif // UNI40

//@Include: aal.h UNI40_abr.h addr.h bhli.h blli.h broadband_bearer_cap.h
//@Include: broadband_repeat_ind.h broadband_send_comp_ind.h cache.h
//@Include: call_state.h called_party_num.h PNNI_called_party_pvpc.h
//@Include: called_party_subaddr.h calling_party_num.h 
//@Include: PNNI_calling_party_pvpc.h calling_party_subaddr.h cause.h
//@Include: conn_id.h UNI40_conn_scope.h PNNI_crankback.h
//@Include: PNNI_designated_transit_list.h UNI40_e2e_trans_delay.h end_pt_ref.h
//@Include: end_pt_state.h errmsg.h
//@Include: UNI40_generic.h ie.h UNI40_ie.h ie_base.h UNI40_leaf.h
//@Include: UNI40_notif.h qos_param.h restart_ind.h subaddr.h td.h
//@Include: UNI40_xqos.h transit_net_sel.h UNI40_td.h

#endif
