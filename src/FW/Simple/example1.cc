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
static char const _example1_cc_rcsid_[] =
"$Id: example1.cc,v 1.14 1998/08/13 20:11:40 mountcas Exp $";
#endif
#include <common/cprototypes.h>
// -*- C++ -*-

// This example shows the interaction between 
// Terminals, States, Muxes, and Factories.  The
// only type of Conduit not included in this example
// is the Cluster.

// This header file contains the class definitions for
// the derived Conduits.
#include "Simple.h"

//
#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include <FW/behaviors/Mux.h>
#include <FW/behaviors/Factory.h>
#include <FW/behaviors/Protocol.h>
#include <FW/behaviors/Adapter.h>
//

#include <iostream.h>

// See the associate XFig for the view of the network (example1.fig)
SimpleTerm * BuildTheNetwork(void)
{
  cout << "Building the Network (please consult example1.fig)" << endl;

  // First allocate the upper terminal
  SimpleTerm * rval = new SimpleTerm();
  Adapter * a  = new Adapter(rval);
  Conduit * ut = new Conduit("UpperTerminal", a);

  // Next allocate the upper mux
  SimpleAccessor * sa = new SimpleAccessor();
  Mux * m = new Mux(sa);
  Conduit * um = new Conduit("UpperMux", m);

  // Join the Terminal and the Mux together
  if (!Join(A_half(ut), A_half(um))) {
    cout << "The Joining of the " << ut->GetName() << " and the "
         << um->GetName() << " has failed." << endl;
    abort();
  }
  
  // Next build the factory
  SimpleCreator * sc = new SimpleCreator();
  Factory * f = new Factory(sc);
  Conduit * fac = new Conduit("SimpleFactory", f);

  // Now join the A side of the factory to the Upper Mux
  if (!Join(A_half(fac), B_half(um))) {
    cout << "The Joining of the " << fac->GetName() << " and the "
         << um->GetName() << " has failed." << endl;
    abort();
  }

  // Next build the Lower Mux
  sa = new SimpleAccessor();
  m = new Mux(sa);
  Conduit * lm = new Conduit("LowerMux", m);

  // Now join the B side of the factory to the Lower Mux
  if (!Join(B_half(fac), B_half(lm))) {
    cout << "The Joining of the " << fac->GetName() << " and the "
         << lm->GetName() << " has failed." << endl;
    abort();
  }

  // Finally build the Lower Terminal (which is also our return value
  rval = new SimpleTerm();
  a  = new Adapter(rval);
  Conduit * lt = new Conduit("LowerTerminal", a);

  // Make the last join between the Terminal and the Lower Mux
  if (!Join(A_half(lt), A_half(lm))) {
    cout << "The Joining of the " << lt->GetName() << " and the "
         << lm->GetName() << " has failed." << endl;
    abort();
  }

  // This is returned so we have some way of Injecting Visitors
  return rval;
}


void main(int argc, char ** argv)
{
  if (argc < 2) {
    cout << "usage: " << argv[0] << " <# of visitors> [<filename>]" << endl;
    exit(1);
  }

  if (argc > 2)
    VisPipe(argv[2]);


  // Construct the network as shown in example1.fig
  SimpleTerm * st = BuildTheNetwork();
  // This enables GT to parse the entire network and show it, 
  // before continuing on and displaying Visitor flow.

  for (int i = 0; i < atoi(argv[1]); i++) {
    // Build a simple visitor with a key between 0 and 4
    SimpleVisitor * sv = new SimpleVisitor(i%5);
    // Inject it into the Lower Terminal
    st->Inject(sv);
  }

  // All done.
}
