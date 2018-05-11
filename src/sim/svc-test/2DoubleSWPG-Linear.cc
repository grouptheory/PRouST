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
static char const _2switchPG_cc_rcsid_[] =
"$Id: 2DoubleSWPG-Linear.cc,v 1.17 1998/09/03 17:22:52 mountcas Exp $";
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

const int    PORTS   = 5;
const double SVC_INT = 0.01;
const int    QDEPTH  = 100;

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

  // PG 1
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x10, 0x10, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x00 },    
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x10, 0x20, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x00 },
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x10, 0x10, 0x01a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x00 },     

  // PG 2
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x20, 0x10, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x00 },    
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x20, 0x20, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x00 },
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x20, 0x10, 0x02a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x00 }

};

// Test SVC Setups with a 2 or 4 switch example.
// NOTE: this example is dependent upon the 
// Switch Expander and all of its components.
void main(int argc, char ** argv)
{
  double stop_time = 1000.0;

  if (argc > 1)
    VisPipe(argv[1]);

  NodeID * p1n1 = new NodeID(addresses[0]),
         * p1n2 = new NodeID(addresses[1]),
         * p1pg = new NodeID(addresses[2]),
         // PG 2
         * p2n1 = new NodeID(addresses[3]),
         * p2n2 = new NodeID(addresses[4]),
         * p2pg = new NodeID(addresses[5]);

  SwitchTerminal * term = 0;
  // First PG
  Conduit * switch1 = MakeMeASwitch("Switch-1A", p1n1, p1pg, p1n1, term);
  Conduit * switch2 = MakeMeASwitch("Switch-1B", p1n2, p1pg, p1n1, term);
  LinkStateWrapper * link_1_2 = 0;
  assert(link_1_2 = LinkSwitches(switch1, 1, switch2, 1));
  // Second PG
  Conduit * switch3 = MakeMeASwitch("Switch-2A", p2n1, p2pg, p2n2, term);
  Conduit * switch4 = MakeMeASwitch("Switch-2B", p2n2, p2pg, p2n2, term);

  cout << "Linking " << switch1->GetName() 
       << " <---> " << switch2->GetName() 
       << " <---> " << switch3->GetName()
       << " <---> " << switch4->GetName() << endl;

  LinkStateWrapper * link_3_4 = 0;
  assert(link_3_4 = LinkSwitches(switch3, 1, switch4, 1));
  // Link the two PGs together
  LinkStateWrapper * link_2_3 = 0;
  assert(link_2_3 = LinkSwitches(switch2, 2, switch3, 2));

  DiagLevel("sim.leadership", DIAG_INFO);
  DiagLevel("sim.aggregator", DIAG_DEBUG);
  DiagLevel("fsm.election",   DIAG_INFO);
  DiagLevel("fsm.nodepeer",   DIAG_INFO);

  XXXTimer * xxxt = new XXXTimer(1.0);
  term->Register(xxxt);

  if (argc > 2)
    stop_time = atof(argv[2]);

  // Get the stop time then run the Kernel
  theKernel().StopIn(stop_time);
  theKernel().Run();

  UnlinkSwitches(link_1_2);
  UnlinkSwitches(link_2_3);
  UnlinkSwitches(link_3_4);

  delete switch1;
  delete switch2;
  delete switch3;
  delete switch4;
}
