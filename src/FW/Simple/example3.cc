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
static char const _example3_cc_rcsid_[] =
"$Id: example3.cc,v 1.12 1998/08/06 04:02:31 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/kernel/Kernel.h>
#include <FW/kernel/KDB.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include "Simple.h"
#include <iostream.h>

extern "C" {
#include <unistd.h>
#include <stdlib.h>
};

// This example instantiates a timer and an I/O handler.
//  The SimpleTimer goes off every 2 seconds and prints
//  something.  The I/O handler checks to see if the specified
//  descriptor is ready for input and if so reads and then
//  prints what it read.  Very simple stuff.  Trying running
//  it in real time.
void main(int argc, char ** argv)
{
  if (argc < 3) {
    cout << "usage: " << argv[0] << " <simulation speed, 0 is realtime> <stop_time> <filename>" << endl;
    exit(1);
  }
  // Time for the simulation to stop
  double speedvalue = atof(argv[1]);
  double stop_time  = atof(argv[2]);

  if (argc > 3)
    VisPipe(argv[3]);

  Kernel & controller = theKernel();
  SimEntity * kdb = controller.KDB();

  // 2.0 is the repeat interval, feel free to change it.
  SimpleTimer * st = new SimpleTimer(kdb, 2.0);
  SimpleInput * si = new SimpleInput(kdb, STDIN_FILENO);

  controller.SetSpeed( (Kernel::Speed) speedvalue);
  controller.StopIn(stop_time);
  cout << "Set stop time for simulation to " << stop_time << " seconds from now." << endl;

  // theKernel().Debug();
  theKernel().Run();
  // Done.
}
