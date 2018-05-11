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
static char const _4switchChain_cc_rcsid_[] =
"$Id: 4SingleSWPG-Linear.cc,v 1.12 1998/08/20 15:19:44 mountcas Exp $";
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
#include <sim/port/LinkStateWrapper.h>
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
  // PG 1 - Phys
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,            
    0x11, 0x11, 0x10, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x00 },    
  // PG 1 - Pref PGL
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,             
    0x11, 0x11, 0x10, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x00 },     

  // PG 2 - Phys
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,            
    0x11, 0x11, 0x20, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x00 },    
  // PG 2 - Pref PGL
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,             
    0x11, 0x11, 0x20, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x00 },

  // PG 3 - Phys
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,            
    0x11, 0x12, 0x30, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x00 },    
  // PG 3 - Pref PGL
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,             
    0x11, 0x12, 0x30, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x00 },

  // PG 4 - Phys
  { 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,              
    0x10, 0x10, 0x40, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x00 },    
  // PG 4 - Pref PGL
  { 88, 96, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,              
    0x10, 0x10, 0x40, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x00 }
};

void main(int argc, char ** argv)
{
  double stop_time = 1000.0;

  if (argc > 1)
    VisPipe(argv[1]);

  DiagLevel("fsm.hello.rcc", DIAG_DEBUG);
  DiagLevel("sim.leadership", DIAG_DEBUG);
  DiagLevel("fsm.visitors.dumplog", DIAG_ERROR);

  NodeID * n1  = new NodeID(addresses[0]),
         * pn1 = new NodeID(addresses[1]),
         * n2 = new NodeID(addresses[2]),
         * pn2 = new NodeID(addresses[3]),
         * n3 = new NodeID(addresses[4]),
         * pn3 = new NodeID(addresses[5]),
         * n4 = new NodeID(addresses[6]),
         * pn4 = new NodeID(addresses[7]);

  SwitchTerminal * term = 0;
  Conduit * switch1 = MakeMeASwitch("SwitchOne", n1, pn1, n1, term);
  Conduit * switch2 = MakeMeASwitch("SwitchTwo", n2, pn2, n2, term);
  LinkStateWrapper *link_1_2 = 0;	// switch 1 linked to switch2

  Conduit * switch3 = MakeMeASwitch("SwitchThree", n3, pn3, n3, term);
  Conduit * switch4 = MakeMeASwitch("SwitchFour", n4, pn4, n4, term);
  LinkStateWrapper *link_3_4 = 0;	// switch 3 linked to switch 4
  LinkStateWrapper *link_2_3 = 0;	// switch 2 linked to switch 3

  cout << "Linking " << switch1->GetName() 
       << " <---> "  << switch2->GetName() 
       << " <---> "  << switch3->GetName() 
       << " <---> "  << switch4->GetName() << endl;

  assert(link_1_2 = LinkSwitches(switch1, 1, switch2, 1));
  assert(link_3_4 = LinkSwitches(switch3, 1, switch4, 1));
  // Make the final link in the chain
  assert(link_2_3 = LinkSwitches(switch2, 2, switch3, 2));

  // Get the stop time then run the Kernel
  if (argc > 2)
    stop_time = atof(argv[2]);

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
