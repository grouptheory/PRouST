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
static char const _linktst_cc_rcsid_[] =
"$Id: linktst.cc,v 1.12 1999/02/12 16:49:56 talmage Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>
#include <codec/uni_ie/ie.h>

extern void DeleteFW(void);

void main(int argc, char ** argv)
{
  u_char buf[128];
  int  len, id;

  PNNI_called_party_soft_pvpc *ie_000 = new PNNI_called_party_soft_pvpc();
  PNNI_called_party_soft_pvpc *ie_000_copy = 0;
  len = ie_000->encode(buf);
  cout << "Encoded PNNI called party soft PVPC in " << len << " bytes." 
       << endl;
  ie_000->decode(buf, id);
  ie_000_copy = new 
    PNNI_called_party_soft_pvpc(*ie_000);
  delete ie_000;
  delete ie_000_copy;

  PNNI_calling_party_soft_pvpc *ie_001 = new PNNI_calling_party_soft_pvpc();
  PNNI_calling_party_soft_pvpc *ie_001_copy = 0;
  len = ie_001->encode(buf);
  cout << "Encoded PNNI calling party soft PVPC in " << len << " bytes." 
       << endl;
  ie_001->decode(buf, id);
  ie_001_copy = new PNNI_calling_party_soft_pvpc(*ie_001);
  delete ie_001;
  delete ie_001_copy;

  PNNI_crankback *ie_002 = 
    new PNNI_crankback(0, PNNI_crankback::SucceedingEndOfInterface, 
		       PNNI_crankback::TransitNetworkUnreachable);
  PNNI_crankback *ie_002_copy = 0;
  len = ie_002->encode(buf);
  cout << "Encoded PNNI crankback in " << len << " bytes." << endl;
  ie_002->decode(buf, id);
  ie_002_copy = new PNNI_crankback(*ie_002);
  delete ie_002;
  delete ie_002_copy;

  PNNI_designated_transit_list *ie_003 = new PNNI_designated_transit_list();
  PNNI_designated_transit_list *ie_003_copy = 0;
  len = ie_003->encode(buf);
  cout << "Encoded PNNI designated transit list in " << len << " bytes." 
       << endl;
  ie_003->decode(buf, id);
  ie_003_copy = new PNNI_designated_transit_list(*ie_003);
  delete ie_003;
  delete ie_003_copy;

  UNI40_abr_param *ie_004 = new UNI40_abr_param();
  UNI40_abr_param *ie_004_copy = 0;
  len = ie_004->encode(buf);
  cout << "Encoded UNI40 ABR Params in " << len << " bytes." << endl;
  ie_004->decode(buf, id);
  ie_004_copy = new UNI40_abr_param(*ie_004);
  delete ie_004;
  delete ie_004_copy;

  UNI40_conn_scope_sel *ie_005 = new UNI40_conn_scope_sel();
  UNI40_conn_scope_sel *ie_005_copy = 0;
  len = ie_005->encode(buf);
  cout << "Encoded UNI40 Connection Scope Selection in " << len << " bytes."
       << endl;
  ie_005->decode(buf, id);
  ie_005_copy = new UNI40_conn_scope_sel(*ie_005);
  delete ie_005;
  delete ie_005_copy;

  UNI40_e2e_transit_delay *ie_006 = new UNI40_e2e_transit_delay();
  UNI40_e2e_transit_delay *ie_006_copy = 0;
  len = ie_006->encode(buf);
  cout << "Encoded UNI40 End to end transit delay in " << len << " bytes." 
       << endl;
  ie_006->decode(buf, id);
  ie_006_copy = new UNI40_e2e_transit_delay(*ie_006);

  cout << "UNI40 End to end transit delay:" << endl;
  cout << *ie_006 << endl;
  delete ie_006;
  delete ie_006_copy;

  UNI40_generic_id *ie_007 = new UNI40_generic_id();
  UNI40_generic_id *ie_007_copy = 0;
  len = ie_007->encode(buf);
  cout << "Encoded UNI40 Generic ID in " << len << " bytes." << endl;
  ie_007->decode(buf, id);
  ie_007_copy = new UNI40_generic_id(*ie_007);
  delete ie_007;  
  delete ie_007_copy;

  UNI40_lij_call_id *ie_008 = new UNI40_lij_call_id();
  UNI40_lij_call_id *ie_008_copy = 0;
  len = ie_008->encode(buf);
  cout << "Encoded UNI40 Leaf Initiated Join Call ID in " << len << " bytes."
       << endl;
  ie_008->decode(buf, id);
  ie_008_copy = new UNI40_lij_call_id(*ie_008);
  delete ie_008;  
  delete ie_008_copy;

  UNI40_lij_params *ie_009 = new UNI40_lij_params();
  UNI40_lij_params *ie_009_copy = 0;
  len = ie_009->encode(buf);
  cout << "Encoded UNI40 Leaf Initiated Join Call Params in " << len 
       << " bytes." << endl;
  ie_009->decode(buf, id);
  ie_009_copy = new UNI40_lij_params(*ie_009);
  delete ie_009;  
  delete ie_009_copy;

  UNI40_leaf_sequence_num *ie_010 = new UNI40_leaf_sequence_num();
  UNI40_leaf_sequence_num *ie_010_copy = 0;
  len = ie_010->encode(buf);
  cout << "Encoded UNI40 Leaf Initiated Join Seq Num in " << len << " bytes." << endl;
  ie_010->decode(buf, id);
  ie_010_copy = new UNI40_leaf_sequence_num(*ie_010);
  delete ie_010;  
  delete ie_010_copy;

  UNI40_notification_indicator *ie_011 = new UNI40_notification_indicator();
  UNI40_notification_indicator *ie_011_copy = 0;
  len = ie_011->encode(buf);
  cout << "Encoded UNI40 Notification Indicator in " << len << " bytes." 
       << endl;
  ie_011->decode(buf, id);
  ie_011_copy = new UNI40_notification_indicator(*ie_011);
  delete ie_011;  
  delete ie_011_copy;

  UNI40_traffic_desc *ie_012 = new UNI40_traffic_desc();
  UNI40_traffic_desc *ie_012_copy = 0;
  len = ie_012->encode(buf);
  cout << "Encoded UNI40 Traffic Descriptor in " << len << " bytes." << endl;
  ie_012->decode(buf, id);
  ie_012_copy = new UNI40_traffic_desc(*ie_012);
  delete ie_012;  
  delete ie_012_copy;

  UNI40_alt_traffic_desc *ie_013 = new UNI40_alt_traffic_desc();
  UNI40_alt_traffic_desc *ie_013_copy = 0;
  len = ie_013->encode(buf);
  cout << "Encoded UNI40 Alternative Traffic Descriptor in " << len 
       << " bytes." << endl;
  ie_013->decode(buf, id);
  ie_013_copy = new UNI40_alt_traffic_desc(*ie_013);
  delete ie_013;  
  delete ie_013_copy;

  UNI40_min_traffic_desc *ie_014 = new UNI40_min_traffic_desc();
  UNI40_min_traffic_desc *ie_014_copy = 0;
  len = ie_014->encode(buf);
  cout << "Encoded UNI40 Minimum Traffic Descriptor in " << len
       << " bytes." << endl;
  ie_014->decode(buf, id);
  ie_014_copy = new UNI40_min_traffic_desc(*ie_014);
  delete ie_014;  
  delete ie_014_copy;

  UNI40_xqos_param *ie_015 = new UNI40_xqos_param();
  UNI40_xqos_param *ie_015_copy = 0;
  len = ie_015->encode(buf);
  cout << "Encoded UNI40 extended quality of service parameters in " << len 
       << " bytes." << endl;
  ie_015->decode(buf, id);
  ie_015_copy = new UNI40_xqos_param(*ie_015);
  delete ie_015;  
  delete ie_015_copy;

  aal5 *ie_016 = new aal5();
  aal5 *ie_016_copy = 0;
  len = ie_016->encode(buf);
  cout << "Encoded ATM Adaption Layer 5 in " << len << " bytes." << endl;
  ie_016->decode(buf, id);
  ie_016_copy = new aal5(*ie_016);
  delete ie_016;  
  delete ie_016_copy;

  E164_addr * addr_000 = new E164_addr((u_char *)"000000", 6);
  E164_addr * addr_000_copy = 0;
  len = addr_000->encode(buf);
  cout << "Encoded E164 Addr in " << len << " bytes." << endl;
  addr_000->decode(buf, id);
  addr_000_copy = new E164_addr(*addr_000);
  delete addr_000;  
  delete addr_000_copy;

  NSAP_DCC_ICD_addr *addr_001  = new NSAP_DCC_ICD_addr();
  NSAP_DCC_ICD_addr *addr_001_copy = 0;
  len = addr_001->encode(buf);
  cout << "Encoded NSAP DCC Addr in " << len << " bytes." << endl;
  addr_001->decode(buf, id);
  addr_001_copy = new NSAP_DCC_ICD_addr(*addr_001);
  delete addr_001;  
  delete addr_001_copy;

  NSAP_E164_addr *addr_002 = new NSAP_E164_addr();
  NSAP_E164_addr *addr_002_copy = 0;
  len = addr_002->encode(buf);
  cout << "Encoded NSAP E164 Addr in " << len << " bytes." << endl;
  addr_002->decode(buf, id);
  addr_002_copy = new NSAP_E164_addr(addr_002);
  delete addr_002;  
  delete addr_002_copy;

  iso_bhli *ie_017 = new iso_bhli();
  iso_bhli *ie_017_copy = 0;
  len = ie_017->encode(buf);
  cout << "Encoded ISO BHLI in " << len << " bytes." << endl;
  ie_017->decode(buf, id);
  ie_017_copy = new iso_bhli(*ie_017);
  delete ie_017;  
  delete ie_017_copy;

  user_bhli *ie_018 = new user_bhli();
  user_bhli *ie_018_copy = 0;
  len = ie_018->encode(buf);
  cout << "Encoded User BHLI in " << len << " bytes." << endl;
  ie_018->decode(buf, id);
  ie_018_copy = new user_bhli(*ie_018);
  delete ie_018;  
  delete ie_018_copy;

  vendor_bhli *ie_019 = new vendor_bhli();
  vendor_bhli *ie_019_copy = 0;
  len = ie_019->encode(buf);
  cout << "Encoded Vendor BHLI in " << len << " bytes." << endl;
  ie_019->decode(buf, id);
  ie_019_copy = new vendor_bhli(*ie_019);
  delete ie_019;  
  delete ie_019_copy;

  ie_blli *ie_020 = new ie_blli();
  ie_blli *ie_020_copy = 0;
  len = ie_020->encode(buf);
  cout << "Encoded BLLI in " << len << " bytes." << endl;
  ie_020->decode(buf, id);
  ie_020_copy = new ie_blli(*ie_020);
  delete ie_020;  
  delete ie_020_copy;

#if 0
  A_Bbc *ie_021 = new A_Bbc();
  A_Bbc *ie_021_copy = 0;
  len = ie_021->encode(buf);
  cout << "Encoded A Broadband Bearer Cap in " << len << " bytes." << endl;
  ie_021->decode(buf, id);
  ie_021_copy = new A_Bbc(*ie_021);
  delete ie_021;  
  delete ie_021_copy;
#endif

#if 0
  C_Bbc *ie_022 = new C_Bbc();
  C_Bbc *ie_022_copy = 0;
  len = ie_022->encode(buf);
  cout << "Encoded C Broadband Bearer Cap in " << len << " bytes." << endl;
  ie_022->decode(buf, id);
  ie_022_copy = new C_Bbc(*ie_022);
  delete ie_022;  
  delete ie_022_copy;
#endif

#if 0
  X_Bbc *ie_023 = new X_Bbc();
  X_Bbc *ie_023_copy = 0;
  len = ie_023->encode(buf);
  cout << "Encoded X Broadband Bearer Cap in " << len << " bytes." << endl;
  ie_023->decode(buf, id);
  ie_023_copy = new X_Bbc(*ie_023);
  delete ie_023;  
  delete ie_023_copy;
#endif

  ie_broadband_repeat_ind *ie_024 = new ie_broadband_repeat_ind();
  ie_broadband_repeat_ind *ie_024_copy = 0;
  len = ie_024->encode(buf);
  cout << "Encoded Broadband Repeat Indicator in " << len << " bytes." << endl;
  ie_024->decode(buf, id);
  ie_024_copy = new ie_broadband_repeat_ind(*ie_024);
  delete ie_024;  
  delete ie_024_copy;

  ie_broadband_send_comp_ind *ie_025 = new ie_broadband_send_comp_ind();
  ie_broadband_send_comp_ind *ie_025_copy = 0;
  len = ie_025->encode(buf);
  cout << "Encoded Broadband Send Comp Indicator in " << len << " bytes." 
       << endl;
  ie_025->decode(buf, id);
  ie_025_copy = new ie_broadband_send_comp_ind(*ie_025);
  delete ie_025;  
  delete ie_025_copy;

  ie_call_state *ie_026 = new ie_call_state();
  ie_call_state *ie_026_copy = 0;
  len = ie_026->encode(buf);
  cout << "Encoded Call State in " << len << " bytes." << endl;
  ie_026->decode(buf, id);
  ie_026_copy = new ie_call_state(*ie_026);
  delete ie_026;  
  delete ie_026_copy;

  addr_001 = 0;
  ie_called_party_num *ie_027 = new ie_called_party_num(addr_001);
  ie_called_party_num *ie_027_copy = 0;
  len = ie_027->encode(buf);
  cout << "Encoded Called Party Number in " << len << " bytes." << endl;
  ie_027->decode(buf, id);
  ie_027_copy = new ie_called_party_num(*ie_027);
  delete ie_027;  
  delete ie_027_copy;

  ie_called_party_subaddr *ie_028 = new ie_called_party_subaddr(addr_001);
  ie_called_party_subaddr *ie_028_copy = 0;
  len = ie_028->encode(buf);
  cout << "Encoded Called Party Subaddress in " << len << " bytes." << endl;
  ie_028->decode(buf, id);
  ie_028_copy = new ie_called_party_subaddr(*ie_028);
  delete ie_028;  
  delete ie_028_copy;

  ie_calling_party_num *ie_029 = new ie_calling_party_num(addr_001);
  ie_calling_party_num *ie_029_copy = 0;
  len = ie_029->encode(buf);
  cout << "Encoded Calling Party Number in " << len << " bytes." << endl;
  ie_029->decode(buf, id);
  ie_029_copy = new ie_calling_party_num(*ie_029);
  delete ie_029;  
  delete ie_029_copy;

  ie_calling_party_subaddr *ie_030 = new ie_calling_party_subaddr(addr_001);
  ie_calling_party_subaddr *ie_030_copy = 0;
  len = ie_030->encode(buf);
  cout << "Encoded Calling Party Subaddress in " << len << " bytes." << endl;
  ie_030->decode(buf, id);
  ie_030_copy = new ie_calling_party_subaddr(*ie_030);
  delete ie_030;  
  delete ie_030_copy;

  ie_cause *ie_031 = new ie_cause();
  ie_cause *ie_031_copy = 0;
  len = ie_031->encode(buf);
  cout << "Encoded Cause in " << len << " bytes." << endl;
  ie_031->decode(buf, id);
  ie_031_copy = new ie_cause(*ie_031);
  delete ie_031;  
  delete ie_031_copy;

  ie_conn_id *ie_032 = new ie_conn_id(0, 5);
  ie_conn_id *ie_032_copy = 0;
  len = ie_032->encode(buf);
  cout << "Encoded Conn ID in " << len << " bytes." << endl;
  ie_032->decode(buf, id);
  ie_032_copy = new ie_conn_id(*ie_032);
  delete ie_032;  
  delete ie_032_copy;

  ie_end_pt_ref *ie_033 = new ie_end_pt_ref(0);
  ie_end_pt_ref *ie_033_copy = 0;
  len = ie_033->encode(buf);
  cout << "Encoded End point reference in " << len << " bytes." << endl;
  ie_033->decode(buf, id);
  ie_033_copy = new ie_end_pt_ref(*ie_033);
  delete ie_033;  
  delete ie_033_copy;

  ie_end_pt_state *ie_034 = new ie_end_pt_state();
  ie_end_pt_state *ie_034_copy = 0;
  len = ie_034->encode(buf);
  cout << "Encoded End point state in " << len << " bytes." << endl;
  ie_034->decode(buf, id);
  ie_034_copy = new ie_end_pt_state(*ie_034);
  delete ie_034;  
  delete ie_034_copy;

  ie_qos_param *ie_035 = new ie_qos_param();
  ie_qos_param *ie_035_copy = 0;
  len = ie_035->encode(buf);
  cout << "Encoded QOS Param in " << len << " bytes." << endl;
  ie_035->decode(buf, id);
  ie_035_copy = new ie_qos_param(*ie_035);
  delete ie_035;  
  delete ie_035_copy;

  ie_restart_ind *ie_036 = new ie_restart_ind();
  ie_restart_ind *ie_036_copy = 0;
  len = ie_036->encode(buf);
  cout << "Encoded Restart Indicator in " << len << " bytes." << endl;
  ie_036->decode(buf, id);
  ie_036_copy = new ie_restart_ind(*ie_036);
  delete ie_036;  
  delete ie_036_copy;

  ie_traffic_desc *ie_037 = new ie_traffic_desc();
  ie_traffic_desc *ie_037_copy = 0;
  len = ie_037->encode(buf);
  cout << "Encoded Traffic Descriptor in " << len << " bytes." << endl;
  ie_037->decode(buf, id);
  ie_037_copy = new ie_traffic_desc(*ie_037);
  delete ie_037;  
  delete ie_037_copy;

  ie_transit_net_sel *ie_038 = new ie_transit_net_sel();
  ie_transit_net_sel *ie_038_copy = 0;
  len = ie_038->encode(buf);
  cout << "Encoded Transit Net Sel in " << len << " bytes." << endl;
  ie_038->decode(buf, id);
  ie_038_copy = new ie_transit_net_sel(*ie_038);
  delete ie_038;  
  delete ie_038_copy;

#if 0
  ie_wildcard *ie_999 = new ie_wildcard();
  ie_wildcard *ie_999_copy = 0;
  len = ie_999->encode(buf);
  cout << "Encoded Wildcard in " << len << " bytes." << endl;
  ie_999->decode(buf, id);
  ie_999_copy = new ie_wildcard(*ie_999);
  delete ie_999;  
  delete ie_999_copy;
#endif

  DeleteFW();
}
