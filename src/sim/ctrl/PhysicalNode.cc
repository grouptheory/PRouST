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
static char const _PhysicalNode_cc_rcsid_[] =
"$Id: PhysicalNode.cc,v 1.47 1999/02/10 18:57:06 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <sim/ctrl/PhysicalNode.h>
#include <FW/basics/Conduit.h>
#include <FW/behaviors/Factory.h>
#include <FW/behaviors/Protocol.h>
#include <FW/behaviors/Mux.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <sim/ctrl/NodeAccessor.h>
#include <fsm/visitors/LinkVisitor.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/nodepeer/NodePeerState.h>
#include <fsm/election/ElectionState.h>
#include <fsm/visitors/PortUpVisitor.h>

PhysicalNodeExp::PhysicalNodeExp( const NodeID * nid )
  : _topmux(0), _botmux(0), _factory(0), _election(0)
{
  Accessor * acc = new NodeIDAccessor(NodeIDAccessor::SOURCE);
  Mux      * mux = new Mux(acc);
  _topmux = new Conduit("UpperNodeIDMux", mux);

  acc = new NodeIDAccessor(NodeIDAccessor::DESTINATION, true);
  mux = new Mux(acc);
  _botmux = new Conduit("LowerNodeIDMux", mux);

  Creator * cre = new NodePeerFSMCreator( );
  Factory * fac = new Factory(cre);
  _factory = new Conduit("NodePeerStateFactory", fac);

  Join(B_half(_topmux), A_half(_factory));
  Join(B_half(_botmux), B_half(_factory));

  _elefsm = new ElectionState( nid );
  Protocol * pro = new Protocol(_elefsm);
  _election = new Conduit("ElectionState", pro);

  Join(A_half(_botmux), A_half(_election));

  DefinitionComplete();
}


PhysicalNodeExp::~PhysicalNodeExp()
{
  if (_factory)
    delete _factory;
  if (_topmux)
    delete _topmux;
  if (_botmux)
    delete _botmux;
  if (_election)
    delete _election;
}


Conduit * PhysicalNodeExp::GetAHalf(void) const
{
  return A_half(_topmux);
}


Conduit * PhysicalNodeExp::GetBHalf(void) const
{
  return B_half(_election);
}


/// -------------------- NodePeerFSMCreator --------------------
NodePeerFSMCreator::NodePeerFSMCreator( void ) : _counter(0) { }

NodePeerFSMCreator::~NodePeerFSMCreator()  { }

Conduit * NodePeerFSMCreator::Create(Visitor * v)
{
  Conduit * rval = 0;
  VisitorType vt = v->GetType(), * vt2 = (VisitorType *)QueryRegistry(PORT_UP_VISITOR_NAME);

  if (vt.Is_A(vt2)) {
    PortUpVisitor * vis = (PortUpVisitor *)v;

    if ( vis->GetSourceNID() ) {
      DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
      assert( dbi != 0 && dbi->good() );
      dbi->Reference();
      State * st = new NodePeerState( dbi->PhysicalNodeID(), vis->GetSourceNID() );
      dbi->Unreference();
      char buf[256];
      sprintf(buf, "NodePeerState-%d", _counter++);
      Creator::Register( rval = new Conduit(buf, st) );
    }
  }
  delete vt2;

  return rval;
}

void NodePeerFSMCreator::Interrupt(SimEvent * e)
{
  // Ack I've been interrupted and don't know what to do!
}
