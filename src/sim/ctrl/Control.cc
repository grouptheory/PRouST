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
static char const _Control_cc_rcsid_[] =
"$Id: Control.cc,v 1.86 1999/02/19 21:22:47 marsh Exp $";
#endif

#include <sim/ctrl/Control.h>
#include <DS/util/String.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/Log.h>
#include <FW/behaviors/Mux.h>
#include <FW/behaviors/Factory.h>
#include <FW/behaviors/Protocol.h>
#include <FW/behaviors/Adapter.h>
#include <sim/ctrl/NodeAccessor.h>
#include <sim/ctrl/PhysicalNode.h>
#include <sim/ctrl/LogicalNode.h>
#include <sim/ctrl/ResourceManager.h>
#include <sim/switch/SwitchTerminal.h>
#include <fsm/visitors/LGNVisitors.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <common/cprototypes.h>

/// ------------------ Control -------------------
Control::Control(ds_String *key, NodeID *& myNode, 
		 Conduit *& controlTerm) :
  _topmux(0), _botmux(0), _manage(0), _my_nid(0), _resman(0), _added(false)
{ 
  Accessor * acc;
  Creator  * cre;
  Expander * exp;

  // Do it first to give the Database the chance to announce
  // itself before any other switch component.
  _resman = new ResourceManager(key, myNode);

  if (myNode)
    _my_nid = new NodeID(*myNode);

  VPVCVisitor * vis = new VPVCVisitor(0, -1, 18, myNode, myNode);

  // Create top Mux -- Top is slave because FastUNISetups come from below.
  acc = new AddrAccessor(AddrAccessor::DESTINATION, true); 
     // new NodeIDAccessor(NodeIDAccessor::DESTINATION, true);
  _topmux = new Conduit("UpperNSAPMux", acc);

  // Create bottom Mux
  acc = new AddrAccessor(AddrAccessor::SOURCE);
     // new NodeIDAccessor(NodeIDAccessor::SOURCE);
  _botmux = new Conduit("LowerNSAPMux", acc);

  // Allocate the Physical Node Expander
  exp = new PhysicalNodeExp( myNode );
  _physical = new Conduit("Physical", exp);

  Join(B_half(_topmux), A_half(_physical), vis, 0);
  Join(B_half(_botmux), B_half(_physical), vis, 0);

  // Create the LogicalNodeExp Factory
  cre = new LogicalNodeExpCreator( );
  _logical_fact = new Conduit("LogicalFactory", cre);
  // Muxes default to factory
  Join(B_half(_topmux), A_half(_logical_fact));
  Join(B_half(_botmux), B_half(_logical_fact));
  
  //
  // Put the CAC together.
  //
  _manage = new Conduit("ResourceManager", _resman);
  Join(A_half(_botmux), A_half(_manage));

  if (controlTerm == 0) {
    // It is important that this be a SwitchTerminal since the 
    // capping Terminal must generate FastUNIVisitor::Connect messages 
    // upon receiving a Setup
    SwitchTerminal * term = new SwitchTerminal(myNode);
    _term = new Conduit("ControlTerminal", term);
    controlTerm = _term;
    // Since it was constructed inside here ...
    _added = true;
  } else {
    _term = controlTerm;
    char buf[Conduit::MAX_LOG_SIZE];
    sprintf(buf, "!add %s to %lx", _term->GetName(), this);
    theLog().AppendToRecord(buf);
  }

  Join(B_half(_manage), A_half(_term));

  vis->Suicide();

  DefinitionComplete();
}

Control::~Control(void)
{
  delete _logical_fact;
  delete _physical;
  delete _topmux;
  delete _botmux;
  delete _manage;
  delete _my_nid;
}

Conduit * Control::GetAHalf(void) const
{
  return A_half(_topmux);
}

/// Bhalf of CAC Expander
Conduit * Control::GetBHalf(void) const
{
  return B_half(_manage);
}

/// --------------------- LogicalNodeExpCreator ------------------------
const VisitorType * LogicalNodeExpCreator::_lgn_inst_type = 0;
const VisitorType * LogicalNodeExpCreator::_lgn_dest_type = 0;

LogicalNodeExpCreator::LogicalNodeExpCreator(void)
{ 
  if ( !_lgn_dest_type )
    _lgn_dest_type = QueryRegistry(LGN_DESTRUCTION_VISITOR);
  if ( !_lgn_inst_type )
    _lgn_inst_type = QueryRegistry(LGN_INSTANTIATION_VISITOR);
}

LogicalNodeExpCreator::~LogicalNodeExpCreator() { }

Conduit * LogicalNodeExpCreator::Create(Visitor * v)
{
  Conduit * rvl = 0;

  if (v->GetType().Is_A( _lgn_inst_type )) {
    LGNInstantiationVisitor * lgn = (LGNInstantiationVisitor *)v;
    LogicalNodeExp * exp = new LogicalNodeExp( lgn->GetSourceNID() );

    char buf[256]; // SourceNID had better be OUR Logical NodeID
    sprintf(buf, "Logical-%d", lgn->GetSourceNID()->GetLevel());
    rvl = new Conduit(buf, exp);
    Creator::Register(rvl);

    // notify the LNExp of its Conduit for Election State
    exp->SetConduit( rvl );
    v->Suicide();  // added 02/04/99 mountcas
  } else if ( v->GetType().Is_A( _lgn_dest_type ) )
    v->Suicide();

  return rvl;
}
  
void LogicalNodeExpCreator::Interrupt(SimEvent * e)
{
  // Ack! I've been interrupted.
}
