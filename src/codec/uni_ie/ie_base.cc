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
#ifndef LINT
static char const _ie_base_cc_rcsid_[] =
"$Id: ie_base.cc,v 1.3 1998/09/28 09:12:00 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/ie_base.h>

int InfoElem::equals(const E164_addr* x) const { return 0; }
int InfoElem::equals(const NSAP_addr* x) const { return 0; }
int InfoElem::equals(const NSAP_DCC_ICD_addr* x) const { return 0; }
int InfoElem::equals(const NSAP_E164_addr* x) const { return 0; }
int InfoElem::equals(const iso_bhli* x) const { return 0; }
int InfoElem::equals(const user_bhli* x) const { return 0; }
int InfoElem::equals(const vendor_bhli* x) const { return 0; }
int InfoElem::equals(const ie_blli* x) const { return 0; }
int InfoElem::equals(const ie_bbc* x) const { return 0; }
// int InfoElem::equals(const A_Bbc* x) const { return 0; }
// int InfoElem::equals(const C_Bbc* x) const { return 0; }
// int InfoElem::equals(const X_Bbc* x) const { return 0; }
int InfoElem::equals(const aal5 * x) const { return 0; }
int InfoElem::equals(const ie_broadband_repeat_ind* x) const { return 0; }
int InfoElem::equals(const ie_broadband_send_comp_ind* x) const { return 0; }
int InfoElem::equals(const ie_call_state* x) const { return 0; }
int InfoElem::equals(const ie_called_party_num* x) const { return 0; }
int InfoElem::equals(const ie_called_party_subaddr* x) const { return 0; }
int InfoElem::equals(const ie_calling_party_num* x) const { return 0; }
int InfoElem::equals(const ie_calling_party_subaddr* x) const { return 0; }
int InfoElem::equals(const ie_cause* x) const { return 0; }
int InfoElem::equals(const ie_conn_id* x) const { return 0; }
int InfoElem::equals(const ie_end_pt_ref* x) const { return 0; }
int InfoElem::equals(const ie_end_pt_state* x) const { return 0; }
int InfoElem::equals(const ie_qos_param* x) const { return 0; }
int InfoElem::equals(const ie_restart_ind* x) const { return 0; }
int InfoElem::equals(const ie_traffic_desc* x) const { return 0; }
int InfoElem::equals(const ie_transit_net_sel* x) const { return 0; }
int InfoElem::equals(const UNI40_lij_call_id* x) const { return 0; }
int InfoElem::equals(const UNI40_lij_params * x) const { return 0; }
int InfoElem::equals(const UNI40_leaf_sequence_num* x) const { return 0; }
int InfoElem::equals(const UNI40_abr_params * x) const { return 0; }
int InfoElem::equals(const UNI40_abr_setup * x) const { return 0; }
int InfoElem::equals(const UNI40_generic_id * x) const { return 0; }
int InfoElem::equals(const UNI40_e2e_transit_delay * x) const { return 0; }
int InfoElem::equals(const UNI40_alt_traffic_desc * x) const { return 0; }
int InfoElem::equals(const UNI40_min_traffic_desc * x) const { return 0; }
int InfoElem::equals(const PNNI_called_party_soft_pvpc * x) const { return 0;}
int InfoElem::equals(const PNNI_crankback * x) const { return 0; }
int InfoElem::equals(const PNNI_designated_transit_list * x) const { return 0;}
int InfoElem::equals(const PNNI_calling_party_soft_pvpc * x) const { return 0;}
void InfoElem::special_print(u_char * buf, int len, ostream & os) {
  if ( len > 0 )  os << "0x";
  for (int i = 0; i < len; i++) {
    int val = (int) (buf[i]);
    if (val < 16)  os << "0";
    os << hex << val << dec ;
    if ( i == 1) os << " ";
  }
  os << endl;
}
