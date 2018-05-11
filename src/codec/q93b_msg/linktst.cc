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
"$Id: linktst.cc,v 1.6 1998/08/06 04:03:01 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>
#include <codec/q93b_msg/q93b_msg.h>

extern void DeleteFW(void);

void main(int argc, char** argv)
{
  generic_q93b_msg * msg = 0;

  msg = new q93b_add_party_message((u_char *)"0000000000000000000000000000000000000000000000000000", 52);
  cout << "Encoded add party message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new q93b_add_party_ack_message((u_char *)"0000000000000000", 16);
  cout << "Encoded add party ack message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new q93b_add_party_rej_message((u_char *)"0000000000000000000000", 22);
  cout << "Encoded add party reject message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;
  
  msg = new q93b_call_proceeding_message((u_char *)"000000000", 9);
  cout << "Encoded call proceeding message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;
  
  msg = new q93b_connect_message((u_char *)"000000000", 9);
  cout << "Encoded connect message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new q93b_connect_ack_message((u_char *)"000000000", 9);
  cout << "Encoded connect ack message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new q93b_drop_party_message((u_char *)"0000000000000000000000", 22);
  cout << "Encoded drop party message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new q93b_drop_party_ack_message((u_char *)"00000000000000000000", 20);
  cout << "Encoded drop party ack message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new q93b_freeform_message((u_char *)"000000000000000000000000000000000000000", 38);
  cout << "Encoded free form message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new UNI40_leaf_setup_failure_message((u_char *)"00000000000000000000000", 23);
  cout << "Encoded leaf setup failure message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new UNI40_leaf_setup_request_message((u_char *)"0000000000000000000000000000000000", 33);
  cout << "Encoded leaf setup request message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;
  
  msg = new q93b_release_message((u_char *)"000000000000000", 15);
  cout << "Encoded release message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;
  
  msg = new q93b_release_comp_message((u_char *)"0000000000000", 13);
  cout << "Encoded release complete message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new q93b_restart_message((u_char *)"00000000000000", 14);
  cout << "Encoded restart message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new q93b_restart_ack_message((u_char *)"00000000000000", 14);
  cout << "Encoded restart ack message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new q93b_setup_message((u_char *)"000000000000000000000000000000000000000", 38);
  cout << "Encoded setup message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;
  
  msg = new q93b_status_message((u_char *)"000000000000000000000", 20);
  cout << "Encoded status message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  msg = new q93b_status_enq_message((u_char *)"000000000", 9);
  cout << "Encoded status enquiry message in " << msg->encoded_bufferlen() << " bytes." << endl;
  delete msg;

  DeleteFW();
  // done.
}
