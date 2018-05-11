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
static char const _LogicalNode_cc_rcsid_[] =
"$Id: LogicalNode.cc,v 1.73 1999/02/10 18:34:02 mountcas Exp $";
#endif

#define DEFAULT_PRIORITY 1

#include <common/cprototypes.h>
#include <FW/basics/FW.h>
#include <FW/basics/diag.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <fsm/database/Database.h>
#include <fsm/election/ElectionState.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/LGNVisitors.h>
#include <sim/ctrl/LogicalNode.h>
#include <sim/ctrl/LogicalSVCExp.h>
#include <sim/ctrl/NodeAccessor.h>
#include <sim/ctrl/VPVCAccessor.h>
#include <sim/ctrl/NID2VCMapper.h>

LogicalNodeExp::LogicalNodeExp( const NodeID * logical_nid ) :
  _nid2vc(0), _topmux(0), _botmux(0), _election(0), _factory(0)
{
  // This needs to be changed to a VCAccessor
  // OUTDATED: Accessor * acc = new NodeIDAccessor(NodeIDAccessor::SOURCE);
  // OUTDATED: Mux      * mux = new Mux(acc);
  // OUTDATED: _topmux = new Conduit("TopMux", mux);
  Accessor * acc = new VCAccessor(VCAccessor::OUT, true);
  Mux      * mux = new Mux(acc);
  _topmux = new Conduit("UpperVPVCMux", mux);

  // This may need to be changed to a VCAccessor
  // OUTDATED: acc = new NodeIDAccessor(NodeIDAccessor::DESTINATION, true);
  // OUTDATED: mux = new Mux(acc);
  // OUTDATED: _botmux = new Conduit("BotMux", mux);
  acc = new VCAccessor(VCAccessor::IN, true);
  mux = new Mux(acc);
  _botmux = new Conduit("LowerVPVCMux", mux);

  Creator * cre = new LogicalSVCExpCreator( logical_nid );
  Factory * fac = new Factory(cre);
  _factory = new Conduit("LogicalSVCFactory", fac);

  Join(B_half(_topmux), A_half(_factory));
  Join(B_half(_botmux), B_half(_factory));

  State * s = new NID2VCMapper( );
  Protocol * pro = new Protocol(s);
  _nid2vc = new Conduit("NodeID2VCMapper", pro);

  Join(A_half(_botmux), A_half(_nid2vc));
  
  int priority = DEFAULT_PRIORITY;
  _elefsm = new ElectionState( (NodeID *)(logical_nid), priority );
  pro = new Protocol(_elefsm);
  _election = new Conduit("ElectionState", pro);

  Join(B_half(_nid2vc), A_half(_election));
  
  // OUTDATED: State    * vcr = new VCMapper();
  // OUTDATED:            pro = new Protocol(vcr);
  // OUTDATED: _vc2rnid = new Conduit("VCtoRNIDMapper", pro);
  // OUTDATED: Join(A_half(_topmux), B_half(_vc2rnid));
  DefinitionComplete();
}

LogicalNodeExp::~LogicalNodeExp()
{
  // OUTDATED: delete _vc2rnid;
  delete _factory;
  delete _topmux;
  delete _botmux;
  delete _election;
}

Conduit * LogicalNodeExp::GetAHalf(void) const
{
  // OUTDATED: A_half(_vc2rnid);
  return A_half(_topmux);
}

Conduit * LogicalNodeExp::GetBHalf(void) const
{
  return B_half(_election);
}

void LogicalNodeExp::SetConduit( Conduit * c )
{
  _elefsm->SetParent( c );
}

/// -------------------- LogicalSVCExpCreator -------------------
const VisitorType * LogicalSVCExpCreator::_fast_uni_type = 0;
const VisitorType * LogicalSVCExpCreator::_lgn_inst_type = 0;

LogicalSVCExpCreator::LogicalSVCExpCreator( const NodeID * logical_nid ) 
  : _logical_nid(0), _counter(0)
{  
  if (logical_nid)
    _logical_nid = new NodeID(*logical_nid);
  if (!_fast_uni_type)
    _fast_uni_type = QueryRegistry(FAST_UNI_VISITOR_NAME);
  if (!_lgn_inst_type)
    _lgn_inst_type = QueryRegistry(LGN_INSTANTIATION_VISITOR);
}

LogicalSVCExpCreator::~LogicalSVCExpCreator() { delete _logical_nid; }

Conduit * LogicalSVCExpCreator::Create(Visitor * v)
{
  Conduit * rvl = 0;

  if (! (v->GetType().Is_A( _lgn_inst_type ))) {
    // Is it a FastUNIVisitor?
    if (v->GetType().Is_A( _fast_uni_type )) {
      FastUNIVisitor * fuv = (FastUNIVisitor *)v;

      if ( fuv->GetMSGType() != FastUNIVisitor::FastUNISetup &&
	   fuv->GetMSGType() != FastUNIVisitor::FastUNILHI ) {
	DIAG("sim.ctrl", DIAG_FATAL, cout << 
	     OwnerName() << " received unknown type of FastUNIVisitor " << 
	     fuv->PrintMSGType() << endl);
      } else {
	// THIS CODE IS TEMPORARILY BROKEN ---->
	const NodeID * remote = (VisitorFrom(v) == Visitor::A_SIDE ? 
				 fuv->GetSourceNID() : fuv->GetDestNID());
	int vpi = fuv->GetInVP(), vci = fuv->GetInVC();
	u_long cref = fuv->GetMSGType() == FastUNIVisitor::FastUNISetup ? fuv->GetCREF() : (u_long)-1;
	// <---- THIS CODE IS TEMPORARILY BROKEN

	// Now create the LogicalSVCExp
	LogicalSVCExp * exp = new LogicalSVCExp( _logical_nid, remote, vpi, vci, cref );
	char buf[256];
	sprintf(buf, "LogicalSVC-%d", _counter++);
	rvl = new Conduit(buf, exp);
	exp->InformOfConduit(rvl);

	Creator::Register(rvl);
      }
    }
  }
  return rvl;
}

void LogicalSVCExpCreator::Interrupt(SimEvent * e) { }
