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

/* -*- C++ -*-
 * File: NNIExpander.cc
 * Author: 
 * Version: $Id: NNIExpander.cc,v 1.18 1999/02/10 19:10:21 mountcas Exp $
 * Purpose: 
 * BUGS:
 */

#ifndef LINT
static char const rcsid[] =
"$Id: NNIExpander.cc,v 1.18 1999/02/10 19:10:21 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include <FW/behaviors/Factory.h>
#include <FW/behaviors/Mux.h>
#include <FW/behaviors/Protocol.h>
#include <codec/pnni_ig/id.h>
#include <fsm/nni/NNIExpander.h>
#include <fsm/nni/Translator.h>
#include <fsm/nni/CREFAccessor.h>
#include <fsm/nni/Q93bCreator.h>
#include <fsm/nni/Coordinator.h>

extern "C" {
#include <stdio.h>
};

NNIExpander::NNIExpander(int port, const NodeID * node, VCAllocator * vcpool)
  : _translator(0), _vpvc_mux(0), _cref_mux(0), _q93b_factory(0),
    _lower_coord(0), _upper_coord(0), _node(0)
{
  assert( node != 0 );
  _node = new NodeID(*node);

  Translator * fsm = new Translator();
  Protocol * pro = new Protocol(fsm);
  _translator = new Conduit("Translator", pro);

  Coordinator* coord2 = new Coordinator();
  Protocol * p_coord2 = new Protocol(coord2);
  _upper_coord = new Conduit("UpperCoordinator", p_coord2);

  Join(B_half(_translator), A_half(_upper_coord));

  CREFAccessor * accA = new CREFAccessor( );
  Mux * mA = new Mux(accA);
  _vpvc_mux = new Conduit("UpperCREFMux", mA);

  Join(B_half(_upper_coord), A_half(_vpvc_mux));

  CREFAccessor * accB = new CREFAccessor();
  Mux * mB = new Mux(accB);
  _cref_mux = new Conduit ("LowerCREFMux", mB);

  Q93bCreator * cr = new Q93bCreator (port, node, vcpool);
  Factory* f =new Factory (cr);
  _q93b_factory=new Conduit ("CallFactory", f);

  Join(A_half(_q93b_factory), B_half(_vpvc_mux));
  Join(B_half(_q93b_factory), B_half(_cref_mux));

  Coordinator* coord1 = new Coordinator(Coordinator::ToggleCREF_Flag);
  Protocol * p_coord1 = new Protocol(coord1);
  _lower_coord = new Conduit("LowerCoordinator", p_coord1);

  Join(A_half(_cref_mux), A_half(_lower_coord));

  // NNI
  // 
  //     Translator
  //         |
  //   Upper Coordinator
  //         |
  //     VPVC Mux
  //    /        \
  //  Calls ... Q93BCreator
  //    \        /
  //     CREF Mux
  //         |
  //   Lower Coordinator
  
  DefinitionComplete();
}


//
// Delete all of the conduits that we created in the constructor.

NNIExpander::~NNIExpander(void)
{
  delete _translator;
  delete _upper_coord;
  delete _q93b_factory;
  delete _vpvc_mux;
  delete _cref_mux;
  delete _lower_coord;
  delete _node;
}


Conduit * NNIExpander::GetAHalf(void) const
{
  return A_half(_translator);
}


Conduit * NNIExpander::GetBHalf(void) const
{
  return B_half(_lower_coord);
}
