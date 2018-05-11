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
 * File: SwitchExpander.cc
 * Author: 
 * Version: $Id: SwitchExpander.cc,v 1.76 1999/02/19 21:22:52 marsh Exp $
 * Purpose: Creates Switches with an arbitrary number of Ports.
 * BUGS:
 */
#ifndef LINT
static char const rcsid[] =
"$Id: SwitchExpander.cc,v 1.76 1999/02/19 21:22:52 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include <DS/util/String.h>
#include <FW/basics/FW.h>
#include <codec/uni_ie/addr.h>
#include <codec/pnni_ig/id.h>
#include <fsm/config/Configurator.h>
#include <fsm/forwarder/Forwarder.h>
#include <fsm/forwarder/VCAllocator.h>
#include <fsm/visitors/PortVisitor.h>
#include <fsm/netstats/NetStatsCollector.h>
#include <sim/ctrl/Control.h>
#include <sim/port/NullState.h>
#include <sim/port/PortAccessor.h>
#include <sim/port/PortCreator.h>
#include <sim/switch/SwitchExpander.h>

// ------------------------- SwitchExpander ----------------------------
SwitchExpander::SwitchExpander(ds_String *key, 
			       ds_String *name, 
			       NodeID *& myNode,
			       Creator *portCreator,
			       Conduit *& controlTerminal) : Expander(),
  _control(0), _data_forwarder(0), _top_mux(0), _bot_mux(0), _port_factory(0)
{
  const Configurator & globalConfig = theConfigurator();
  Accessor *ac;
  Adapter  *adapt;
  Cluster  *clust;
  Conduit  *last;
  Creator  *cr = portCreator;
  Expander *exp;
  Factory  *fb;
  Mux      *mb;
  Terminal *dft;
  
  // Expander that performs the switch control
  //
  // Create this first so that its Database has the
  // right to speak its name before all other switch elements.
  //
  assert(key != 0);
  _cptr = new Control(key, myNode, controlTerminal);

  // Create top adapter -- DataForwarder
  dft   = new Forwarder( myNode );
  adapt = new Adapter(dft);
  _data_forwarder = new Conduit("DataForwarder", adapt);

  //
  // Put the Control Expander into its Cluster and Conduit.
  //
  clust = new Cluster(_cptr);
  _control = new Conduit("ControlPort", clust);

#if 0
  //
  // Add a terminal to the B side of the Control Expander.
  // Use the one supplied as controlTerminal if it's not zero.
  // Build a SwitchTerminal if controlTerminal is zero.
  //
  _term = controlTerminal;

  if (controlTerminal == 0) {
    // It is important that this be a SwitchTerminal since the 
    // capping Terminal must generate FastUNIVisitor::Connect messages 
    // upon receiving a Setup
    SwitchTerminal * term = new SwitchTerminal();
    adapt = new Adapter(term);
    _term = new Conduit("ControlTerminal", adapt);
    controlTerminal = _term;
  }
  Join(B_half(_control), A_half(_term));
#endif

  // Top Mux
  ac = new PortAccessor;
  mb = new Mux(ac);
  _top_mux = new Conduit("UpperPortMux", mb);

  PortVisitor * vis = new PortVisitor(0);
  vis->SetOutPort(0);
  vis->SetInPort(0);

  // Join Control to Top Mux.
  Join(B_half(_top_mux), A_half(_control), vis, 0);
  // Link bottom of Data Forwarder with Top of TopMux
  Join(A_half(_data_forwarder), A_half(_top_mux));

  // Create the PortFactory
  if (cr == 0) 
    cr = new PortCreator( );
  fb = new Factory(cr);
  _port_factory = new Conduit("PortFactory", fb);

  // Join def of Top Mux to sideA of Factory.
  Join(B_half(_top_mux), A_half(_port_factory));

  // Create the bottom Mux
  ac = new PortAccessor;
  mb = new Mux(ac);
  _bot_mux = new Conduit("LowerPortMux", mb);
  
  // Join def of Bottom Mux to sideB of Factory.
  Join(B_half(_bot_mux), B_half(_port_factory));

  vis->Suicide();

  SetBlocking();
  DefinitionComplete();
}

SwitchExpander::~SwitchExpander()
{
  theNetStatsCollector().ReportNetEvent( "Switch_Down", 
					 GetAddress()->Print(), 0, 
					 GetAddress() );
  delete _control;
  delete _data_forwarder;
  delete _port_factory;
  delete _top_mux;
  delete _bot_mux;
}

Conduit * SwitchExpander::GetAHalf(void) const
{
  return A_half(_data_forwarder);
}


Conduit * SwitchExpander::GetBHalf(void) const
{
  return A_half(_bot_mux);    // Inverted Mux.
}




