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
"$Id: linktst.cc,v 1.17 1999/02/16 21:42:10 talmage Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>
#include <codec/pnni_ig/ig_incl.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/DBKey.h>
#include <codec/uni_ie/addr.h>

u_char node_id[] = {   72,  160, 
		       0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		       0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
		       0x11, 0x11 };
u_char peer_id[] = {   96, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
		     0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };

u_char addresses[][22] = {
  // PG ...0x10 0x10
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },    
  // PG ...0x20 0x20
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  // PG ...0x10 0x10
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },     
  // PG ...0x20 0x20
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  // PG ...0x00 0x00
  { 56, 88, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
};

void main(int argc, char ** argv)
{
  u_char buf[266];
  int  len;
  InfoGroup * igptr = 0;
  errmsg * err = 0;
  NodeID * n1 = new NodeID(addresses[0]),
         * n2 = new NodeID(addresses[1]),
         * pn1 = new NodeID(addresses[2]),
         * pn2 = new NodeID(addresses[3]);

  NodeID nid(node_id);
  PeerID pid(peer_id);
  PeerID *p1 = new PeerID("96:47000580ffde00000011338800");

  cout << "Testing encoding and decoding of NodeIDs" << endl;
  cout << "n1=         " << *n1 << endl;
  cout << "n1 as NSAP= " << *(n1->GetAddr()) << endl;
  cout << "n2=         " << *n2 << endl;
  cout << "n2 as NSAP= " << *(n2->GetAddr()) << endl;
  cout << "pn1=         " << *pn1 << endl;
  cout << "pn1 as NSAP= " << *(pn1->GetAddr()) << endl;
  cout << "pn2=         " << *pn2 << endl;
  cout << "pn2 as NSAP= " << *(pn2->GetAddr()) << endl;

  cout << "Testing encoding and decoding of PeerIDs" << endl;
  cout << "p1=          " << *p1 << endl;

  DBKey * dbk = new DBKey(InfoGroup::ig_ptse_id, &nid, 0, 0, &pid, 140);
  delete dbk;

  igptr = new ig_aggregation_token(0xcafef00d);
  igptr->encode(buf, len);
  cout << "Aggregation Token encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_external_reachable_atm_addr(1, 0xDA, 2);
  igptr->encode(buf, len);
  cout << "External Reachable ATM Address encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_horizontal_links(0x00, &nid, 2, 1, 1);
  igptr->encode(buf, len);
  cout << "Horizontal Link encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_internal_reachable_atm_addr(1, 0xDA, 2);
  igptr->encode(buf, len);
  cout << "Internal Reachable ATM Address encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_lgn_horizontal_link_ext();
  igptr->encode(buf, len);
  cout << "LGN Horizontal Link encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_nodal_info_group((u_char *)"00000000000000000000", (u_char)1, 
				  (u_char)0x00, (u_char *)nid.GetNID(), 
				  (ig_next_hi_level_binding_info *)0);
  igptr->encode(buf, len);
  cout << "Nodal Information Group encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_nodal_hierarchy_list(0xcafef00d);
  igptr->encode(buf, len);
  cout << "Nodal Hierarchy List encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_nodal_ptse_ack(&nid);
  igptr->encode(buf, len);
  cout << "Nodal PTSE Ack encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_nodal_ptse_summary(&nid, &pid);
  igptr->encode(buf, len);
  cout << "Nodal PTSE Summary encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_nodal_state_params(0x00, 0, 0);
  igptr->encode(buf, len);
  cout << "Nodal State Parameters encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_next_hi_level_binding_info((u_char *)nid.GetNID(), (u_char *)"000000000000000000000", 
					    (u_char *)pid.GetPID(), (u_char *)nid.GetNID());
  igptr->encode(buf, len);
  cout << "Next Higher Level Binding Information encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_optional_gcac_params(1, 1);
  igptr->encode(buf, len);
  cout << "Optional GCAC Parameters encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  ig_ptse * ptse = new ig_ptse(InfoGroup::ig_unknown_id, 1, 1, 1, 1);
  ptse->encode(buf, len);
  cout << "PTSE encoded in " << len << " bytes." << endl;
  if (err = ptse->decode(buf))
    delete err;
  ptse->UnReference();

  igptr = new ig_req_ptse_header(&nid);
  igptr->encode(buf, len);
  cout << "Request PTSE Header encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_resrc_avail_info(ig_resrc_avail_info::outgoing, 0xf800,
				  10, 8000, 8000, 10, 10, 8, 8);
  igptr->encode(buf, len);
  cout << "Resource Availibilty Info encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_system_capabilities(5, 0, (u_char *)"00000");
  igptr->encode(buf, len);
  cout << "System Capabilities encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_transit_net_id(5, (u_char *)"00000", 5);
  igptr->encode(buf, len);
  cout << "Transit Net ID encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_uplink_info_attr(0xcafef00d);
  igptr->encode(buf, len);
  cout << "Uplink Information Attribute encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;

  igptr = new ig_uplinks(0x0, (u_char *)nid.GetNID(), (u_char *)pid.GetPID(), 
			 1, 0, (u_char *)"00000000000000000000");
  igptr->encode(buf, len);
  cout << "Uplinks encoded in " << len << " bytes." << endl;
  if (err = igptr->decode(buf))
    delete err;
  delete igptr;
  
  // done.
}
