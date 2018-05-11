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
static char const _test_cc_rcsid_[] =
"$Id: test.cc,v 1.4 1998/08/10 22:10:08 mountcas Exp $";
#endif
#include <common/cprototypes.h>
#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/UNI40_td.h>
#include <codec/uni_ie/broadband_bearer_cap.h>
#include <codec/uni_ie/called_party_num.h>
#include <codec/uni_ie/calling_party_num.h>
#include <codec/uni_ie/conn_id.h>
#include <codec/uni_ie/qos_param.h>
#include <codec/q93b_msg/setup.h>

generic_q93b_msg * build_setup(void)
{
  Addr * tempAddr = 0;
  int bandwidth = 470000;
  InfoElem * ie_array[num_ie];
  for (int i = 0; i < num_ie; i++) ie_array[i] = 0;

  // the 1st four mandatory IE's don't exist in our implementation ...
  ie_array[InfoElem::ie_traffic_desc_ix] = new UNI40_traffic_desc();
  ((UNI40_traffic_desc *)
   ie_array[InfoElem::ie_traffic_desc_ix])->set_BE(bandwidth, bandwidth);

  // next BBC A
  ie_array[InfoElem::ie_broadband_bearer_cap_ix] = 
    new A_Bbc(ie_broadband_bearer_cap::not_clipped, 
	      ie_broadband_bearer_cap::p2p);

  // next Called Party number
  tempAddr = newAddr("0x47.0005.80.ffde00.0000.0000.0104.000000000001.00");
  ie_array[InfoElem::ie_called_party_num_ix] =
    new ie_called_party_num(tempAddr);

  delete tempAddr;

  // next Calling Party number
  tempAddr = newAddr("0x47.0005.80.ffde00.0000.0000.0104.000000000002.00");
  ie_array[InfoElem::ie_calling_party_num_ix] = 
    new ie_calling_party_num(tempAddr);

  delete tempAddr;

  // next Connection Identifier
  ie_array[InfoElem::ie_conn_identifier_ix] =
    new ie_conn_id(0, 5);

  // next QoS Parameter
  ie_array[InfoElem::ie_qos_param_ix] = 
    new ie_qos_param(ie_qos_param::qos0, ie_qos_param::qos0);

  u_long cref = 1;
  generic_q93b_msg * message = new q93b_setup_message(ie_array, cref, 0);
  return message;
}

void main(void)
{
  cout << "Building setup ..." << endl;
  generic_q93b_msg * msg = build_setup();
  for (int i = 0; i < 10000; i++) {
    cout << "Attempting to create copy ..." << endl;
    generic_q93b_msg * copy = msg->copy();
    delete copy;
  }
}
