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
static char const _LogicalSVCExp_cc_rcsid_[] =
"$Id: LogicalSVCExp.cc,v 1.63 1999/02/16 18:26:29 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/diag.h>
#include <sim/ctrl/LogicalSVCExp.h>
#include <sim/ctrl/NodeAccessor.h>
#include <fsm/nodepeer/NodePeerState.h>
#include <fsm/hello/HelloVisitor.h>
#include <fsm/hello/HelloState.h>
#include <fsm/hello/RCCHelloState.h>
#include <fsm/hello/LgnHelloState.h>
#include <fsm/visitors/LGNVisitors.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <FW/basics/FW.h>

// Takes our DB, our Logical NodeID and the remote Logical NodeID
LogicalSVCExp::LogicalSVCExp( const NodeID * logical, const NodeID * remote,
			      int vpi, int vci, u_long cref) : 
  _rcc_hello(0), _topmux(0), _botmux(0), _factory(0), _nodepeer(0), _lower_aggtok_accessor(0)
{
  // RCC Hello
  State * sta = new RCCHelloState( logical, remote, vpi, vci,
				   HelloState::NewestVersion, 
				   HelloState::OldestVersion, 
				   cref);
  Protocol * pro = new Protocol(sta);
  _rcc_hello = new Conduit("RCCHelloState", pro);

  Accessor * acc = new AggAccessor();
  Mux      * mux = new Mux(acc);
  _topmux = new Conduit("UpperAggTokenMux", mux);

  Join(A_half(_topmux), B_half(_rcc_hello));

  _lower_aggtok_accessor = new AggAccessor();
  acc = _lower_aggtok_accessor;
  mux = new Mux(acc);
  _botmux = new Conduit("LowerAggTokenMux", mux);

  Creator * cre = new LgnHelloFSMCreator( logical, remote );
  Factory * fac = new Factory(cre);
  _factory = new Conduit("LgnHelloStateFactory", fac);

  Join(B_half(_topmux), A_half(_factory));
  Join(B_half(_botmux), B_half(_factory));

  sta = new NodePeerState( logical, remote, vpi, vci, cref );
  pro = new Protocol(sta);
  _nodepeer = new Conduit("NodePeerState", pro);

  Join(A_half(_botmux), A_half(_nodepeer));
  DefinitionComplete();
}

LogicalSVCExp::~LogicalSVCExp()
{
  delete _rcc_hello;
  delete _factory;
  delete _topmux;
  delete _botmux;
  delete _nodepeer;
}

Conduit * LogicalSVCExp::GetAHalf(void) const 
{
  return A_half(_rcc_hello);
}

Conduit * LogicalSVCExp::GetBHalf(void) const
{
  return B_half(_nodepeer);
}

void LogicalSVCExp::InformOfConduit(Conduit* my_conduit) {
  _lower_aggtok_accessor->InformOfSVCExp(my_conduit);
}

/// ----------------- HelloFSMCreator ---------------------
VisitorType * LgnHelloFSMCreator::_fastuni_vistype = 0;
VisitorType * LgnHelloFSMCreator::_horlink_vistype = 0;
VisitorType * LgnHelloFSMCreator::_lgninst_vistype = 0;

LgnHelloFSMCreator::LgnHelloFSMCreator( const NodeID * localNode,
					const NodeID * remoteNode ) 
  : _logical_nid(0)
{ 
  if (localNode) 
    _logical_nid = new NodeID (*localNode); 
  if (remoteNode)
    _logical_rnid = new NodeID(*remoteNode);

  if (!_fastuni_vistype)
    _fastuni_vistype = (VisitorType *)QueryRegistry(FAST_UNI_VISITOR_NAME);
  if (!_lgninst_vistype)
    _lgninst_vistype = (VisitorType *)QueryRegistry(LGN_INSTANTIATION_VISITOR);
  if (!_horlink_vistype)
    _horlink_vistype = (VisitorType *)QueryRegistry(HLINK_VISITOR_NAME);
}

LgnHelloFSMCreator::~LgnHelloFSMCreator() 
{ 
  delete (NodeID *)_logical_nid; 

  dic_item di;
  forall_items(di, _agg2lgn) {
    _agg2lgn.inf(di)->SetCreator( );
  }
  _agg2lgn.clear();
}

Conduit * LgnHelloFSMCreator::Create(Visitor * v)
{
  Conduit * rvl = 0;

  if (v->GetType().Is_A(_lgninst_vistype)) {
    v->Suicide();  // Suicide all LGN Instantiation Visitors
  } else if (v->GetType().Is_A(_horlink_vistype)) {
    HorLinkVisitor * hlv = (HorLinkVisitor *)v;

    // The LgnHello should only travel between RCCHelloState and LgnHelloState
    assert( hlv->GetVT() != HorLinkVisitor::LgnHello);
  } else if (v->GetType().Is_A(_fastuni_vistype)) {
    FastUNIVisitor * fv = (FastUNIVisitor *)v;

    LgnHelloState * sta = 0;
    switch ( VisitorFrom( v ) ) {
    case Visitor::A_SIDE:
      if (fv->GetMSGType() != FastUNIVisitor::FastUNISetup) {
	diag("sim.ctrl", DIAG_WARNING, "%s: Received %s from A Side!\n", 
	     OwnerName());
      } else {
	// it is a Setup so allow it to make a new LgnHelloState!
	sta = new LgnHelloState( fv->GetAgg(), _logical_nid, 
				 fv->GetLogicalPort(), _logical_rnid );
      }
      break;
    case Visitor::B_SIDE:
      if ((fv->GetMSGType() == FastUNIVisitor::FastUNILHI) ||
	  (fv->GetMSGType() == FastUNIVisitor::FastUNISetup &&
	   _logical_nid->equals( fv->GetSourceNID() ))) {
	sta = new LgnHelloState( fv->GetAgg(), _logical_nid,
				 fv->GetLogicalPort(), _logical_rnid );
      }
      break;
    default:
      diag("sim.ctrl", DIAG_FATAL, "%s: Received %s from unknown location!\n",
	   OwnerName(), v->GetType().Name());
      break;
    }

    if ( sta != 0 ) {
      sta->SetCreator( this );
      char buf[256];
      sprintf(buf, "LgnHelloState-%d", fv->GetAgg());
      rvl = new Conduit(buf, sta);
      Creator::Register(rvl);
      _agg2lgn.insert( fv->GetAgg(), sta );
    }
  }
  return rvl;
}

void LgnHelloFSMCreator::Interrupt(SimEvent * e) { }

bool LgnHelloFSMCreator::KillLgnHello(int aggTok)
{
  dic_item di = _agg2lgn.lookup(aggTok);
  if (di) {
    delete _agg2lgn.inf(di);
    // If we access this again we will die
    // _agg2lgn.change_inf(di, 0);
    return true;
  }
  return false;
}

bool LgnHelloFSMCreator::LgnHelloDeparting(int aggTok)
{
  dic_item di = _agg2lgn.lookup(aggTok);
  if (di) {
    _agg2lgn.del_item(di);
    return true;
  }
  return false;
}
