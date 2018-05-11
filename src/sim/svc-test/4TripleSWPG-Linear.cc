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
static char const _4PG_cc_rcsid_[] =
"$Id: 4TripleSWPG-Linear.cc,v 1.7 1998/09/14 20:07:40 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include <FW/behaviors/Adapter.h>
#include <FW/behaviors/Protocol.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/Handlers.h>
#include <FW/actors/Terminal.h>
#include <FW/actors/State.h>
#include <codec/pnni_ig/id.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <sim/ctrl/ResourceManager.h>
#include <sim/switch/SwitchFunctions.h>
#include <sim/switch/SwitchTerminal.h>
#include <sim/port/LinkStateWrapper.h>

// Number of ports
const int    PORTS   = 10;
// Queue service interval
const double SVC_INT = 0.01;
// Queue depth
const int    QDEPTH  = 100;
// Bandwidth
const u_long BW      = OC3;

// -- The below are to facilitate clean-up --
// list of switches
list<Conduit *>          _swlist;
// list of connections
list<LinkStateWrapper *> _connlist;

// Constructs a Switch with the specific characteristics
Conduit * MakeMeASwitch(const char * name, NodeID * nid, 
			NodeID * pnid, NodeID * prefPGL, 
			SwitchTerminal *& tptr,
			const char * filename = 0)
{
  // Create a switch
  tptr   = new SwitchTerminal();
  Adapter  * adapt  = new Adapter((Terminal *)tptr);
  Conduit  * swterm = new Conduit("Terminal", adapt);

  Conduit  * sw = MakeSwitch(name, PORTS, SVC_INT, QDEPTH,
				  nid, pnid, prefPGL, swterm, 
				  filename);
  assert(sw != 0);
  // Wham! You're a switch.
  return sw;
}

// The addresses used to construct the network.
u_char base_addr[22] = {
  96, 160, 0x47, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

// Constructs a peer group of 3 nodes
void MakePG(int n, Conduit *& one, Conduit *& thr)
{
  Conduit * two = 0;

  u_char tmp_addr[22];
  memcpy(tmp_addr, base_addr, 22);

  for (int i = 0; i < 3; i++) {
    int rem = ((n << 4) | (0xa + i)), prf = ((n << 4) | 0xc);
    tmp_addr[13] = ((n << 4) | 0x0);

    memset(tmp_addr + 14, rem, 7);
    NodeID * node = new NodeID( tmp_addr );

    memset(tmp_addr + 14, prf, 7);
    NodeID * pref = new NodeID( tmp_addr );

    char buf[256];
    sprintf(buf, "PG%dSwitch%c", n, 'A' + i);
    SwitchTerminal * term = 0;
    Conduit * tmp = MakeMeASwitch(buf, node, 0, pref, term);
    delete pref; // They make copies

    switch (i) {
    case 0:  // this keeps the PGL in the middle of the PG
      two = tmp; break;
    case 1:
      one = tmp; break;
    case 2:
      thr = tmp; break;
    }

    _swlist.append(tmp);
    cout << "Created Switch " << buf << " " << *node << " for PG " << n << endl;
  }

  // Link the 3 switches in a circle
  LinkStateWrapper * link = 0;
  assert(link = LinkSwitches(one, 1, two, 1, BW, ((rand() % 3) + 1)));
  _connlist.append( link ); link = 0;
  assert(link = LinkSwitches(two, 2, thr, 2, BW, ((rand() % 3) + 1)));
  _connlist.append( link ); link = 0;
  assert(link = LinkSwitches(thr, 3, one, 3, BW, ((rand() % 3) + 1)));
  _connlist.append( link ); link = 0;
}

// Test SVC Setups with a 2 or 4 switch example.
// NOTE: this example is dependent upon the 
// Switch Expander and all of its components.
void main(int argc, char ** argv)
{
  struct timeval tp;
  gettimeofday(&tp, 0);
  srand(tp.tv_sec);

  double stop_time = 1000.0;

  if (argc > 1)
    VisPipe(argv[1]);
  else
    VisPipe("/dev/null");

  DiagLevel("sim.aggregator", DIAG_INFO);
  DiagLevel("sim.leadership", DIAG_INFO);
  DiagLevel("fsm.visitors.dumplog", DIAG_ERROR);
  DiagLevel("fsm.nodepeer",   DIAG_INFO);
  DiagLevel("sim.logos",      DIAG_INFO);

  Conduit * b1, * b2, * b3, * b4;

  // make the first PG
  MakePG(1, b1, b2);
  // make the second PG
  MakePG(2, b3, b4);
  // Link PG1 and PG2
  LinkStateWrapper * link_1_2 = 0;
  assert(link_1_2 = LinkSwitches(b2, 4, b3, 4, BW, ((rand() % 3) + 1)));
  // make the third PG
  MakePG(3, b1, b2);
  // Link PG2 and PG3
  LinkStateWrapper * link_2_3 = 0;
  assert(link_2_3 = LinkSwitches(b4, 5, b1, 5, BW, ((rand() % 3) + 1)));
  // make the fourth and final PG
  MakePG(4, b3, b4);
  // Link PG3 and PG4
  LinkStateWrapper * link_3_4 = 0;
  assert(link_3_4 = LinkSwitches(b2, 6, b3, 6, BW, ((rand() % 3) + 1)));

  if (argc > 2)
    stop_time = atof(argv[2]);

  // Get the stop time then run the Kernel
  theKernel().StopIn(stop_time);
  theKernel().Run();

  // Unlink the switches
  UnlinkSwitches(link_1_2);
  UnlinkSwitches(link_2_3);
  UnlinkSwitches(link_3_4);
  delete link_1_2;
  delete link_2_3;
  delete link_3_4;

  // Unlink all intra-peer group connections
  list_item li;
  forall_items(li, _connlist) {
    link_1_2 = _connlist.inf(li);
    UnlinkSwitches(link_1_2);
    delete link_1_2;
  }

  cout << "Simulation has concluded." << endl;
}
