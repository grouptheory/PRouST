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

#if !defined(LINT)
static char const _hexread_cc_rcsid_[] =
"$Id: hexread.cc,v 1.7 1998/09/28 09:12:00 bilal Exp $";
#endif
#include <common/cprototypes.h>
// C++
#include <iostream.h>
#include <fstream.h>
#include <codec/uni_ie/ie.h>



int main(int argc,char *argv[])
{
  ifstream fp(argv[1]);
  int pkt_cnt, cnt = 0, ret;
  u_int data;
  u_char pkt[500];
  u_char buffer[500];
  u_char *cptr = buffer;
  NSAP_E164_addr * addr = new NSAP_E164_addr;
  
  if (argc != 2) {  
    cout << "usage: " << argv[0] << " filename\n";
    exit(1);
  }
  pkt_cnt = 0;
  while (!(fp.eof())) {
    fp.getline(buffer, 500);
    if (*buffer) { // Not a blank line, add it to buffer
      cptr = buffer;
      while (*cptr && (ret = sscanf(cptr, "efffefe0", &data)) != EOF) {
	pkt[cnt++] = (u_char) data;
	/* move to non-blank character */
	while (*cptr != ' ')
	  cptr++;
	cptr++;
      }
      // End of buffer, continue;
    } else { // have a blank line, process this packet and reset pkt
      // Print the packet
      for (int l = 0; l < cnt; l++) {
	if (0 == l               )
	  cout << endl;
	if (pkt[l] < 16)
	  printf("0EFFFEFE0 ", pkt[l]);
	else
	  printf("EFFFEFE0 ", pkt[l]);
      }
      cout << endl;
      // Now try and decode the ie properly.
      InfoElem * ie;
      u_short id = ntohs(pkt[0]);
      switch (id) {
       case InfoElem::ie_cause_id:
	 ie = new ie_cause( );
	 break;
       case InfoElem::ie_call_state_id:
	 ie = new ie_call_state((ie_call_state::call_state_values)0);
	 break;
       case InfoElem::ie_end_pt_ref_id:
	 ie = new ie_end_pt_ref(0);
	 break;
       case InfoElem::ie_end_pt_state_id:
	 ie = new ie_end_pt_state((ie_end_pt_state::party_state)0);
	 break;
       case InfoElem::ie_aal_param_id:
	 ie = new aal5((aal5::aal5_sscs)0, 0, 0);
	 break;
       case InfoElem::ie_conn_identifier_id:
	 ie = new ie_conn_id(0, 0);
	 break;
       case InfoElem::ie_qos_param_id:
	 ie = new ie_qos_param((ie_qos_param::qos_indicator)0, 
			       (ie_qos_param::qos_indicator)0);
	 break;
       case InfoElem::ie_bhli_id:
	 switch (ntohs(pkt[4])) {
	   case ie_bhli::iso:
	     ie = new iso_bhli();
	     break;
	   case ie_bhli::user_specific:
	     ie = new user_bhli();
	     break;
	   case ie_bhli::vendor_specific_application_id:
	     ie = new vendor_bhli(1,7);
	     break;
	 }
	 break;
       case InfoElem::ie_broadband_bearer_cap_id:
	 ie = new ie_bbc();
	 break;

       case InfoElem::ie_blli_id:
	 ie = new ie_blli; // No args.
	 break;
#if 0
       case InfoElem::ie_broadband_locking_shift_id:
	 strcpy(name,"Lock Shift");
	 ie = new ie_broadband_locking_shift();
	 break;
       case InfoElem::ie_broadband_nlocking_shift_id:
	 strcpy(name,"Nlock Shift");
	 ie = new ie_broadband_nlocking_shift_id( );
	 break;
#endif
       case InfoElem::ie_broadband_send_comp_ind_id:
	 ie = new ie_broadband_send_comp_ind; // No args.
	 break;
       case InfoElem::ie_broadband_repeat_ind_id:
	 ie = new ie_broadband_repeat_ind;  // No args.
	 break;
       case InfoElem::ie_calling_party_num_id: {
	 ie = new ie_calling_party_num();
	 break;
       }
       case InfoElem::ie_calling_party_subaddr_id: {
	 // addr = new NSAP_E164_addr;
	 ie = new ie_calling_party_subaddr(addr);
	 break;
       }
       case InfoElem::ie_called_party_num_id: {
	 // addr = new NSAP_E164_addr;
	 ie = new ie_called_party_num(addr);
	 break;
       }
       case InfoElem::ie_called_party_subaddr_id: {
	 // addr = new NSAP_E164_addr;
	 ie = new ie_called_party_subaddr(addr);
	 break;
       }
       case InfoElem::ie_transit_net_sel_id:
	 ie = new ie_transit_net_sel((char *)0);
	 break;
       case InfoElem::ie_restart_ind_id:
	 ie = new ie_restart_ind((ie_restart_ind::restart_indicator)0);
	 break;
#ifndef UNI40
       case InfoElem::ie_traffic_desc_id:
	 ie = new ie_traffic_desc;  // No args.
	 break;
#else
       case InfoElem::ie_traffic_desc_id:
	 ie = new UNI40_traffic_desc;  // No args.
	 break;
       case InfoElem::UNI40_e2e_transit_delay_id:
	 ie = new UNI40_e2e_transit_delay(0);
	 break;
       case InfoElem::UNI40_notification_ind_id:
	 ie = new UNI40_notification_indicator;  // No args
	 break;
       case InfoElem::UNI40_generic_id_id:
	 ie = new UNI40_generic_id((UNI40_generic_id::id_related_std_app)0,
				   (UNI40_generic_id::id_types)0);
	 break;
       case InfoElem::UNI40_min_traff_desc_id:
	 ie = new UNI40_min_traffic_desc;  // No Args.
	 break;
       case InfoElem::UNI40_alt_traff_desc_id:
	 ie = new UNI40_alt_traffic_desc;  // No Args.
	 break;
       case InfoElem::UNI40_abr_setup_id:
	 ie = new UNI40_abr_setup(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	 break;
       case InfoElem::PNNI_called_party_soft_pvpc_id:
	 ie = new PNNI_called_party_soft_pvpc(0, 0, 0);
	 break;
       case InfoElem::PNNI_crankback_id:
	 // BUG- Should reach into pk and get the contents.
	 ie = new PNNI_crankback(0, PNNI_crankback::SucceedingEndOfInterface,
				 PNNI_crankback::TransitNetworkUnreachable);
	 break;
       case InfoElem::PNNI_designated_transit_list_id:
	 ie = new PNNI_designated_transit_list(0, 0, 0);
	 break;
       case InfoElem::PNNI_calling_party_soft_pvpc_id:
	 ie = new PNNI_calling_party_soft_pvpc(0, 0);
	 break;
       case InfoElem::UNI40_abr_params_id:
	 ie = new UNI40_abr_param(0, 0);
	 break;
       case InfoElem::UNI40_leaf_call_id:
	 ie = new UNI40_lij_call_id(0, 0);
	 break;
       case InfoElem::UNI40_leaf_params_id:
	 ie = new UNI40_lij_params(0);
	 break;
       case InfoElem::UNI40_leaf_sequence_num_id:
	 ie = new UNI40_leaf_sequence_num(0);
	 break;
       case InfoElem::UNI40_conn_scope_id:
	 ie = new UNI40_conn_scope_sel((UNI40_conn_scope_sel::connection_scope_types)0,
			(UNI40_conn_scope_sel::connection_scope_sels)0);
	 break;
       case InfoElem::UNI40_xqos_param_id:
	 ie = new UNI40_xqos_param((UNI40_xqos_param::origin) 0, (UNI40_xqos_param::fb_ind)3,0,0,0,0,0,0);
	 break;
#endif
       default:
         cerr << "Invalid IE ID [" << id << "]" << endl;
         break;
      }
      if (ie) {
	int nid;
	InfoElem::ie_status err;
	if (err = ie->decode(pkt, nid)) {
	  switch (err) {
	  case InfoElem::bad_id:
	    cout << "Error::bad_id" << endl;
	    break;
	  case InfoElem::empty:
	    cout << "Error::empty" << endl;
	    break;
	  case InfoElem::incomplete:
	    cout << "Error::incomplete" << endl;
	    break;
	  case InfoElem::invalid_contents:
	    cout << "Error::invalid_contents" << endl;
	    break;
	  case InfoElem::unsupported:
	    cout << "Error::unsupported" << endl;
	    break;
          }
        } else
	  cout << *(ie) << endl;
	delete ie;
      }
      pkt_cnt++;          
      cnt = 0;
    }
  }
  delete addr;
  cout << pkt_cnt << " packets decoded." << endl;
  return 0;  
}



