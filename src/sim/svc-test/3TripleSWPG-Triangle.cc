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
static char const _3PG_cc_rcsid_[] =
"$Id: 3TripleSWPG-Triangle.cc,v 1.30 1998/09/14 20:31:34 mountcas Exp $";
#endif
#include <common/cprototypes.h>

/**
 *  Purpose:  Test the higherarchy at multiple levels.
 *  Results:  3PG contains 3 PeerGroups:
 *
 *            96:160:470511111111111111111110
 *            96:160:470511111111111111111120
 *            96:160:470511111111111111111230
 *
 *            The first two should join into one PeerGroup around 
 *            level 88, and then the third should join the level 
 *            after that.  The aggregator should generate horizontal 
 *            links at the appropriate levels between these PeerGroups.
 */

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
u_char addresses[][22] = {

  // PG 1
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,              // 0
    0x11, 0x11, 0x10, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x00 },    
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
    0x11, 0x11, 0x10, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x00 },
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x10, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00 },
  // preferred parent
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,               // 3
    0x11, 0x11, 0x10, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x00 },     

  // PG 2
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,              // 4
    0x11, 0x11, 0x20, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x00 },    
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x20, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x00 },
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x20, 0x2c, 0x2c, 0x2c, 0x2c, 0x2c, 0x2c, 0x2c, 0x00 },
  // preferred parent
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,               // 7
    0x11, 0x11, 0x20, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x00 },

  // PG 3 
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,              // 8
    0x11, 0x12, 0x30, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x00 },    
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x12, 0x30, 0x3b, 0x3b, 0x3b, 0x3b, 0x3b, 0x3b, 0x3b, 0x00 },
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x12, 0x30, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x00 },
  // preferred parent
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,               // 11
    0x11, 0x12, 0x30, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x00 }
};

// Constructs a peer group of 3 nodes
void MakePG(int n, Conduit *& b1, Conduit *& b2)
{
  n = n * 4;  // This gives us the proper starting index into addresses
  
  Conduit * one = 0, * two = 0, * thr = 0;

  for (int i = 0; i < 3; i++) {
    NodeID * node = new NodeID(addresses[(i + n)]),
           * pref = new NodeID(addresses[n]);
    //           * pgl  = new NodeID(addresses[(n + 3)]);

    char buf[256];
    sprintf(buf, "PG%dSwitch%c", (n/4) + 1, (i == 0 ? 'A' : (i == 1 ? 'B' : 'C')));
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
    cout << "Created Switch " << buf << " " << *node << " for PG " << (n/4)+1 << endl;
  }

  // Link the 3 switches in a circle
  LinkStateWrapper * link = 0;
  assert(link = LinkSwitches(one, 1, two, 1, BW, ((rand() % 3) + 1)));
  _connlist.append( link ); link = 0;
  assert(link = LinkSwitches(two, 2, thr, 2, BW, ((rand() % 3) + 1)));
  _connlist.append( link ); link = 0;
  assert(link = LinkSwitches(thr, 3, one, 3, BW, ((rand() % 3) + 1)));
  _connlist.append( link ); link = 0;

  if ((n/4) != 1) {
    b1 = thr;
    b2 = one;
  } else {
    b1 = one;
    b2 = thr;
  }
}

// Test SVC Setups with a 2 or 4 switch example.
// NOTE: this example is dependent upon the 
// Switch Expander and all of its components.
void main(int argc, char ** argv)
{
  // InitFW();

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

  Conduit * b1, * b2;
  MakePG(0, b1, b2);
  Conduit * b3, * b4;
  MakePG(1, b3, b4);
  // Link PG1 and PG2
  LinkStateWrapper * link_1_2 = 0;
  assert(link_1_2 = LinkSwitches(b2, 4, b3, 4, BW, ((rand() % 3) + 1)));
  Conduit * b5, * b6;
  MakePG(2, b5, b6);
  // Link PG2 and PG3
  LinkStateWrapper * link_2_3 = 0;
  assert(link_2_3 = LinkSwitches(b4, 5, b5, 5, BW, ((rand() % 3) + 1)));
  // Link PG3 and PG1
  LinkStateWrapper * link_3_1 = 0;
  assert(link_3_1 = LinkSwitches(b6, 6, b1, 6, BW, ((rand() % 3) + 1)));

  if (argc > 2)
    stop_time = atof(argv[2]);

  // Get the stop time then run the Kernel
  theKernel().StopIn(stop_time);
  theKernel().Run();

  // Unlink the switches
  UnlinkSwitches(link_1_2);
  UnlinkSwitches(link_3_1);
  UnlinkSwitches(link_2_3);
  delete link_1_2;
  delete link_3_1;
  delete link_2_3;

  // Unlink all intra-peer group connections
  list_item li;
  forall_items(li, _connlist) {
    link_1_2 = _connlist.inf(li);
    UnlinkSwitches(link_1_2);
    delete link_1_2;
  }

#if 0
  // Free the memory
  forall_items(li, _swlist) {
    Conduit * sw = _swlist.inf(li);
    delete sw;
  }
#endif
  cout << "Simulation has concluded." << endl;
}
