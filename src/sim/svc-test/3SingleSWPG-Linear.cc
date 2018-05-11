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
"$Id: 3SingleSWPG-Linear.cc,v 1.17 1998/08/20 15:20:35 mountcas Exp $";
#endif

#include <common/cprototypes.h>

/**
 *  Purpose:  Test the higherarchy at multiple levels.
 *  Results:  2PG contains 3 PeerGroups:
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
#include <FW/interface/ShareableRegistry.h>
#include <codec/pnni_ig/id.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <sim/ctrl/ResourceManager.h>
#include <sim/switch/SwitchFunctions.h>
#include <sim/switch/SwitchTerminal.h>
#include <sim/port/LinkStateWrapper.h>

// Number of ports
const int    PORTS   = 4;
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
			Conduit *& term,
			const char * filename = 0)
{
  // Create a switch
  Conduit  * sw = MakeSwitch(name, PORTS, SVC_INT, QDEPTH,
				  nid, pnid, prefPGL, term, 
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
           * pref = new NodeID(addresses[n]),
           * pgl  = new NodeID(addresses[(n + 3)]);

    char buf[256];
    sprintf(buf, "Switch-%d%c", (n/4) + 1, (i == 0 ? 'A' : (i == 1 ? 'B' : 'C')));
    Conduit * term = 0;
    Conduit * tmp = MakeMeASwitch(buf, node, pgl, pref, term);

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
  LinkStateWrapper * link_1_2 = 0;
  assert(link_1_2 = LinkSwitches(one, 1, two, 1, BW, ((rand() % 3) + 1)));
  _connlist.append( link_1_2 );
  assert(link_1_2 = LinkSwitches(two, 2, thr, 2, BW, ((rand() % 3) + 1)));
  _connlist.append( link_1_2 );
  assert(link_1_2 = LinkSwitches(thr, 3, one, 3, BW, ((rand() % 3) + 1)));
  _connlist.append( link_1_2 );

  if ((n/4) != 1) {
    b1 = thr;
    b2 = one;
  } else {
    b1 = one;
    b2 = thr;
  }
}


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

  DiagLevel("sim.leadership",       DIAG_DEBUG);
  DiagLevel("fsm.visitors.dumplog", DIAG_ERROR);
  DiagLevel("fsm.hello.rcc",        DIAG_DEBUG);
  DiagLevel("fsm.hello.lgn",        DIAG_DEBUG);
  DiagLevel("fsm.election",         DIAG_DEBUG);
  DiagLevel("fsm.nodepeer",         DIAG_INFO);
  DiagLevel("sim.acac",             DIAG_DEBUG);
  DiagLevel("sim.logos",            DIAG_DEBUG);
  DiagLevel("sim.routecontrol",     DIAG_DEBUG);

  // Make Switch 1
  NodeID * node = new NodeID(addresses[0]),
         * pref = new NodeID(addresses[0]),
         * pgl  = new NodeID(addresses[3]);
  Conduit * term = 0;
  Conduit * sw1 = MakeMeASwitch("SwitchOne", node, pgl, pref, term);

  // Make Switch 2
  NodeID * node2 = new NodeID(addresses[4]),
         * pref2 = new NodeID(addresses[4]),
         * pgl2  = new NodeID(addresses[7]);
  term = 0;
  Conduit * sw2 = MakeMeASwitch("SwitchTwo", node2, pgl2, pref2, term);

  // Make Switch 3
  NodeID * node3 = new NodeID(addresses[8]),
         * pref3 = new NodeID(addresses[8]),
         * pgl3  = new NodeID(addresses[11]);
  term = 0;
  Conduit * sw3 = MakeMeASwitch("SwitchThree", node3, pgl3, pref3, term);

  // Link PG1 and PG2 twice
  LinkStateWrapper * link_1_2 = 0;
  assert(link_1_2 = LinkSwitches(sw1, 1, sw2, 1, BW, ((rand() % 3) + 1)));
  // Link PG2 and PG3 twice
  LinkStateWrapper * link_2_3 = 0;
  assert(link_2_3 = LinkSwitches(sw2, 2, sw3, 2, BW, ((rand() % 3) + 1)));
  // Link PG1 and PG3 twice
  LinkStateWrapper * link_3_1 = 0;
  assert(link_3_1 = LinkSwitches(sw3, 1, sw1, 2, BW, ((rand() % 3) + 1)));

  if (argc > 2)
    stop_time = atof(argv[2]);

  // Get the stop time then run the Kernel
  theKernel().StopIn(stop_time);
  theKernel().Run();

  // Unlink the switches
  UnlinkSwitches(link_1_2);
  delete link_1_2;
  UnlinkSwitches(link_2_3);
  delete link_2_3;
  UnlinkSwitches(link_3_1);
  delete link_3_1;

  char filename[64];
  sprintf(filename, "registry.vis");
  cout << "Saving the Interface/Shareable Registry to '" << filename << "'." << endl;
  theShareableRegistry().save(filename);

  delete sw1;
  delete sw2;
  delete sw3;
}
