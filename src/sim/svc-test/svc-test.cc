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

#ifndef LINT
static char const _svc_test_cc_rcsid_[] =
"$Id: svc-test.cc,v 1.30 1998/08/06 04:05:45 bilal Exp $";
#endif
#include <common/cprototypes.h>
// -*- C++ -*-
#include "SwitchTimers.h"

#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include <FW/behaviors/Adapter.h>
#include <FW/behaviors/Protocol.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/Handlers.h>
#include <FW/actors/Terminal.h>
#include <FW/actors/State.h>
#include <codec/pnni_ig/id.h>
#include <fsm/hello/Hello_DIAG.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <sim/acac/ACAC_DIAG.h>
#include <sim/ctrl/ResourceManager.h>
#include <sim/switch/SwitchFunctions.h>
#include <sim/switch/SwitchTerminal.h>
#include <sim/port/LinkStateWrapper.h>

const double SVC_INT   = 0.01;
const int    QDEPTH    = 100;
const int    NUM_LINKS = 2;
const int    PORTS     = NUM_LINKS + 1;

Conduit * MakeMeASwitch(const char * name, NodeID * nid, 
			NodeID * pnid, NodeID * prefPGL, 
			SwitchTerminal *& tptr,
			const char * filename = 0)
{
  // Create a switch
  tptr   = new SwitchTerminal();
  Adapter  * adapt  = new Adapter((Terminal *)tptr);
  Conduit  * swterm = new Conduit("Terminal", adapt);

  cout << "Creating switch '" << name << "'" << endl;
  Conduit  * sw = MakeSwitch(name, PORTS, SVC_INT, QDEPTH,
				  nid, pnid, prefPGL, swterm, 
				  filename);
  assert(sw != 0);
  // Wham! You're a switch.
  return sw;
}

u_char addresses[][22] = {
  // PG ...0x10 0x10
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x10, 0x10, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x00 },    
  // PG ...0x20 0x20
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x20, 0x20, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x00 },
  // PG ...0x10 0x10
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x10, 0x10, 0x01a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x00 },     
  // PG ...0x20 0x20
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x20, 0x20, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x00 },
  // PG ...0x00 0x00
  { 56, 88, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
};

// Test SVC Setups with a 2 or 4 switch example.
// NOTE: this example is dependent upon the 
// Switch Expander and all of its components.
void main(int argc, char ** argv)
{
  LinkSwitchesTimer *linkers[NUM_LINKS];
  double stop_time = 250.0;

  if (argc > 1)
    VisPipe(argv[1]);

  DiagLevel("fsm.election", DIAG_DEBUG);
  DiagLevel(FSM_HELLO, DIAG_DEBUG);
  DiagLevel(SIM_ACAC_BORDER_DOWN, DIAG_INFO);
  DiagLevel(SIM_ACAC_BORDER_UP, DIAG_INFO);
  DiagLevel(SIM_ACAC_CALL, DIAG_DEBUG);
  DiagLevel(SIM_ACAC_NODE_PEER, DIAG_INFO);
  DiagLevel(SIM_ACAC_PORT_DOWN, DIAG_INFO);
  DiagLevel(SIM_ACAC_PORT_UP, DIAG_INFO);
  DiagLevel("sim.aggregator", DIAG_DEBUG);
  DiagLevel("sim.leadership", DIAG_DEBUG);
  DiagLevel("sim.routecontrol", DIAG_DEBUG);
  DiagLevel("sim.switch.link", DIAG_DEBUG);
  DiagLevel("sim.switch.unlink", DIAG_DEBUG);

  // DiagLevel("sim", DIAG_DEBUG);
  // DiagLevel("fsm.visitors.dumplog", DIAG_ERROR);

  NodeID * n1 = new NodeID(addresses[0]),
         * n2 = new NodeID(addresses[1]),
         * pn1 = new NodeID(addresses[2]),
         * pn2 = new NodeID(addresses[3]);

  SwitchTerminal * term = 0;
  Conduit * switch1 = MakeMeASwitch("SwitchOne", n1, pn1, n1, term);
  Conduit * switch2 = MakeMeASwitch("SwitchTwo", n2, pn2, n2, term);

  //
  // Link the switches together at time i seconds.  Unlink the
  // switches in 20 seconds.  Link and unlink them at 20-second
  // intervals.
  //
  for (int i = 1; i < (NUM_LINKS + 1); i++) {

    //
    // Link two switches in i seconds.  Unlink them after 20 seconds.
    // Keep ulinking and relinking them until the program stops.
    //
    linkers[i-1] = new LinkSwitchesTimer((double)i, 80.0, 
					 switch1, i, switch2, i, 
					 80.0);
    term->Register(linkers[i-1]);
  }

  XXXTimer * xxxt = new XXXTimer(1.0);
  term->Register(xxxt);

  // Get the stop time then run the Kernel
  theKernel().StopIn(stop_time);
  theKernel().Run();

  for (int i = 1; i < (NUM_LINKS + 1); i++) {
    delete linkers[i-1];
  }

  delete switch1;
  delete switch2;
}
