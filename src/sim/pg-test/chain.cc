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
static char const _chain_cc_rcsid_[] =
"$Id: chain.cc,v 1.15 1999/02/19 21:22:54 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include <DS/containers/list.h>
#include <DS/util/String.h>
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
#include <fsm/config/Configurator.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <sim/ctrl/ResourceManager.h>
#include <sim/switch/SwitchFunctions.h>
#include <sim/switch/SwitchTerminal.h>

u_char base_addr[22] =
{ 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,     
  0x11, 0x11, 0x10, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void main(int argc, char ** argv)
{
  struct timeval tp;
  gettimeofday(&tp, 0);
  srand(tp.tv_sec);

  theConfigurator( "Configuration.nfo" );

  list<Conduit *> switches;

  double stop_time = 1000.0;
  int num_switches = 5;

  if (argc < 2)
    cout << "usage: " << argv[0] << " [num-switches [stop-time [outfile]]]" << endl;

  if (argc > 1)
    num_switches = atoi(argv[1]);

  if (argc > 2)
    stop_time = atof(argv[2]);

  if (argc > 3)
    VisPipe(argv[3]);
  else
    VisPipe("/dev/null");

  cout << "Building " << num_switches << " switches:" << endl;

  Conduit * current = 0, * last = 0, * term = 0;
  for (int i = 0; i < num_switches; i++) {
    cout << "." << flush;

    term = 0;
    u_char * addr = base_addr;
    addr[0] = 96; 

    if (i > 0xFFFFFF)
      addr[11] = (i >> 24) & 0xFF;
    if (i > 0xFFFF)
      addr[12] = (i >> 16) & 0xFF;
    if (i > 0xFF)
      addr[13] = (i >>  8) & 0xFF;

    addr[14] = (i & 0xFF);

    NodeID * nid  = new NodeID(addr);
    addr[14] = ((num_switches/2) & 0xFF);
    NodeID * pref = new NodeID(addr);
    addr[0] = 88;
    NodeID * pgl = new NodeID(addr);
    
    char name_ar[64];
    sprintf(name_ar, "Switch-%d", i);
    ds_String * name = new ds_String(name_ar);
    ds_String * rec  = new ds_String("PGOne");
    current = MakeSwitch(rec, name, nid, term);

    if (last) {
      LinkStateWrapper * link = 0;
      assert(link = LinkSwitches(last, 2, current, 1, OC3, ((rand() % 3) + 1)));
    }
    last = current;
  }
  cout << endl;

  DiagLevel("sim.leadership", DIAG_DEBUG);
  DiagLevel("fsm.nodepeer",   DIAG_DEBUG);

  // Get the stop time then run the Kernel
  theKernel().StopIn(stop_time);
  theKernel().Run();

  char filename[64];
  sprintf(filename, "registry.vis");
  cout << "Saving the Interface/Shareable Registry to '" << filename << "'." << endl;
  theShareableRegistry().save(filename);
}
