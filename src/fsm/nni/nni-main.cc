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
static char const _linktst_cc_rcsid_[] =
"$Id: nni-main.cc,v 1.6 1998/10/15 19:09:41 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>

#include "NNIExpander.h"
#include "NNITester.h"
#include "Coordinator.h"
#include <fsm/forwarder/VCAllocator.h>
#include <FW/basics/diag.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/Visitor.h>
#include <FW/behaviors/Adapter.h>
#include <FW/behaviors/Cluster.h>
#include <FW/kernel/Kernel.h>

#define Q93B_DEBUGGING "fsm.nni.Q93B_debugging"


int main(int argc, char** argv)
{
  char filename[80];
  if (argc < 2)
    {
      cout << "Enter the name of the visualizer pipe: ";
      cin >> filename;
    }
  else
    strcpy(filename, argv[1]);
  VisPipe(filename);


  DiagLevel(Q93B_DEBUGGING,DIAG_DEBUG);
  
  VCAllocator* vcal1=new VCAllocator();
  VCAllocator* vcal2=new VCAllocator();

  NNIExpander* N1 = new NNIExpander (1,0,vcal1);
  NNIExpander* N2 = new NNIExpander (2,0,vcal2);
  Cluster* N1cluster = new Cluster(N1);
  Cluster* N2cluster = new Cluster(N2);

  Conduit * N1conduit = new Conduit("NNI-1",N1cluster);
  Conduit * N2conduit = new Conduit("NNI-2",N2cluster);

  Join(B_half(N1conduit), B_half(N2conduit));

  NNITester* T1 = new NNITester("bilal.1",vcal1,1);
  NNITester* T2 = new NNITester("bilal.2",vcal2,0);
  Adapter* T1adapter = new Adapter(T1);
  Adapter* T2adapter = new Adapter(T2);
  Conduit * T1conduit = new Conduit("API-1",T1adapter);
  Conduit * T2conduit = new Conduit("API-2",T2adapter);

  Join(A_half(T1conduit), A_half(N1conduit));
  Join(A_half(T2conduit), A_half(N2conduit));

  theKernel().Run();
  exit(0);
}
