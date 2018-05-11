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
static char const _Coordinator_cc_rcsid_[] =
"$Id: Coordinator.cc,v 1.29 1998/10/05 14:27:58 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <fsm/nni/Coordinator.h>
#include <FW/basics/diag.h>

#include <fsm/visitors/VPVCVisitor.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/visitors/PortDownVisitor.h>
#include <fsm/visitors/PortVisitor.h>
#include <fsm/nni/Q93bVisitors.h>
#include <fsm/nni/SSCFVisitor.h>
#include <codec/q93b_msg/q93b_msg.h>

//
// 'The call reference flag can take the values "0" or "1".  The call
// reference flag is used to identify which end of the signalling
// virtual channel originated a call reference.  The origination side
// always sets the call reference flag to "0".  The destination side
// always sets the call reference flag to a "1".
//
// 'Hence the call reference flag identifies who allocated the call
// reference value and the only purpose of the call reference flag is
// to resolve simultaneous attempts to alloate the same call reference
// value.  The call reference flag also applies to functions which
// use the global call reference (e.g. restart procedures).'
//
// UNI 3.0, p. 184

#define  CREF_FLAG  0x800000

// -------------------- Static Members ---------------------
const VisitorType * Coordinator::_linkstatus_type  = 0;
const VisitorType * Coordinator::_q93b_type  = 0;
const VisitorType * Coordinator::_vpvc_type  = 0;

// -------------------- Constructor --------------------
// When the Coordinator is constructed with the coordinator_function
// argument set to ToggleCREF_Flag, the Coordinator toggles the call
// reference flag on incoming Q93b messages.  This permits the
// NNIExpander's CREF Mux to "route" the message to the right instance
// of the NNI.  E.g. When an incoming call reference is 8001 (i.e. has
// the cref flag set), the Coordinator toggles the cref flag, making
// the cref 0001.
//
// When the Coordinator is constructed with the coordinator_function
// argument set to DoNotToggleCREF_Flag, it does not modify the
// incoming call reference.
//
Coordinator::Coordinator(CoordinatorFunction coordinator_function) : 
  State(true), _port_active(true), _coordinator_function(coordinator_function)
{
  // These are the visitor types we need to be sensitive to
  if (_linkstatus_type  == 0)
    _linkstatus_type = QueryRegistry(SSCF_LINKSTATUS_VISITOR_NAME);
  if (_q93b_type == 0)
    _q93b_type = QueryRegistry(Q93B_VISITOR_NAME);
  if (_vpvc_type == 0)
    _vpvc_type = QueryRegistry(VPVC_VISITOR_NAME);
}

//----------------------------------------------------
Coordinator::~Coordinator(void)
{ _q_for_uni.clear();  }

//----------------------------------------------------
State * Coordinator::Handle(Visitor* v)
{
  DIAG("fsm.nni.coordinator", DIAG_DEBUG, cout << 
       "*** " << OwnerName() << " (" << this << 
       ") *** received " << v->GetType() << endl);

  if (v->GetType().Is_A(_linkstatus_type)) {
    SSCFLinkStatusVisitor* lsv = (SSCFLinkStatusVisitor*)v;
    SSCFLinkStatusVisitor::status st = lsv->get_status();
    switch (st) {
      case SSCFLinkStatusVisitor::up:
	list_item li;
	while (_q_for_uni.empty() == false) {
	  Visitor * vis = _q_for_uni.pop();
	  PassThru(vis);            // flush _q_to_uni
	}
	_q_for_uni.clear();
	_port_active = true;
	break;
      case SSCFLinkStatusVisitor::down:
	_port_active = false;
	break;
      default:
	break;
    }
    if (lsv->get_ttl() == 0)
      lsv->Suicide();
    else
      PassThru(v);
  } else {
    switch (VisitorFrom(v)) {
      case Visitor::B_SIDE: {
	if (v->GetType().Is_A(_q93b_type)) {
	  Q93bVisitor * qv = (Q93bVisitor *)v;
	  assert( qv->share_msg() != 0 );
	  // If _coordinator_function permits it, flip the CREF bit
	  // once so that externally-originated calls will have the
	  // bit set and internally-originated calls will not have the
	  // bit set.  NO OTHER CODE MAY SET THIS BIT UNDER ANY
	  // CIRCUMSTANCES.
	  int crv = qv->get_crv();
	  if (_coordinator_function == ToggleCREF_Flag) {
	    crv ^= CREF_FLAG;
	    qv->set_crv(crv);
	  }
	  qv->TurnIntoInd();
	  PassVisitorToA(qv);
	  break;
	} else if (v->GetType().Is_A(_vpvc_type)) {
	  VPVCVisitor * vv = (VPVCVisitor *)v;
	  if (_coordinator_function == ToggleCREF_Flag) {
	    u_long crv = vv->GetCREF();
	    crv ^= CREF_FLAG;
	    vv->SetCREF(crv);
	  }
	  PassVisitorToA(vv);
	} else
	  PassThru(v);  // not a Q93bVisitor
      }
      break;
      
      case Visitor::A_SIDE:
	if (v->GetType().Is_A(_q93b_type)) {
	  Q93bVisitor * qv = (Q93bVisitor *)v;
	  assert( qv->share_msg() != 0 );
	}
	if (_port_active) 
	  PassThru(v);
	else
	  _q_for_uni.append(v);
	break;

      default:
	v->Suicide();
	break;
    }
  }
  return this;
}

//----------------------------------------------------
void Coordinator::Interrupt(SimEvent *event)
{
  // does nothing
}

//----------------------------------------------------
bool Coordinator::IsPortActive(void)
{
  return _port_active;
}
