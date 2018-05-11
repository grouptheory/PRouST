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
 * File: PortExpander.cc
 * Author: 
 * Version: $Id: PortExpander.cc,v 1.47 1999/02/10 19:09:41 mountcas Exp $
 * Purpose: Creates Conduits that behave like the port of a switch.
 * BUGS:
 */

#ifndef LINT
static char const rcsid[] =
"$Id: PortExpander.cc,v 1.47 1999/02/10 19:09:41 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include <FW/behaviors/Factory.h>
#include <FW/behaviors/Mux.h>
#include <FW/behaviors/Protocol.h>
#include <codec/pnni_ig/id.h>
#include <fsm/queue/QueueState.h>
#include <fsm/hello/HelloState.h>
#include <sim/port/PortExpander.h>
#include <fsm/nni/NNIExpander.h>
#include <fsm/forwarder/VCAllocator.h>

PortExpander::PortExpander( int port, const NodeID * node )
  : _port(port), _port_hello(0), _port_nni(0)
{
  // Hello FSM
  HelloState * fsm = new HelloState( node, _port );
  _port_hello = new Conduit("HelloState", fsm);

  // This is the NNI at the very bottom of the Port.
  VCAllocator * vca = new VCAllocator();
  NNIExpander * nni = new NNIExpander( port, node, vca );
  _port_nni = new Conduit("NNI", nni);  // only one NNI per port
  
  if (!Join(B_half(_port_hello), A_half(_port_nni))) {
    DIAG(SIM, DIAG_DEBUG, cout << "Unable connected " << _port_hello->GetName() 
	 << " (" << _port_hello << ") to " << _port_nni->GetName() << " (" 
	 << _port_nni << ")." << endl;);
  }

  DefinitionComplete();
}


//
// Delete all of the conduits that we created in the constructor.
// Deleting a conduit deletes its behavior and its actor.
//
PortExpander::~PortExpander(void)
{
  delete _port_hello;
  delete _port_nni;
}


Conduit * PortExpander::GetAHalf(void) const
{
  return A_half(_port_hello);
}


Conduit * PortExpander::GetBHalf(void) const
{
  return B_half(_port_nni);
}

int PortExpander::GetPort(void)
{
  return _port;
}

void PortExpander::SetPort(int port)
{
  _port = port;
}
