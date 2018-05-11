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
static char const _LgnHelloState_cc_rcsid_[] =
"$Id: LgnHelloState.cc,v 1.14 1999/02/16 16:06:23 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "LgnHelloState.h"

#include <FW/basics/diag.h>
#include <FW/kernel/SimEvent.h>
#include <codec/pnni_ig/id.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/hello/HelloVisitor.h>
#include <fsm/hello/HelloTimers.h>
#include <fsm/nodepeer/NPVisitors.h>

// LgnHelloState
int                     LgnHelloState::_ref_count = 0;
InternalLgnHelloState * LgnHelloState::_Down      = 0;
InternalLgnHelloState * LgnHelloState::_Attempt   = 0;
InternalLgnHelloState * LgnHelloState::_OneWay    = 0;
InternalLgnHelloState * LgnHelloState::_TwoWay    = 0;

VisitorType * LgnHelloState::_fastuni_vistype = 0;

void LgnHelloState::AllocateStates(void)
{
  if (!_Down)    _Down    = new LgnDown();
  if (!_Attempt) _Attempt = new LgnAttempt();
  if (!_OneWay)  _OneWay  = new LgnOneWayInside();
  if (!_TwoWay)  _TwoWay  = new LgnTwoWayInside();
}

void LgnHelloState::FreeStates(void)
{
  delete _Down;    _Down    = 0;
  delete _Attempt; _Attempt = 0;
  delete _OneWay;  _OneWay  = 0;
  delete _TwoWay;  _TwoWay  = 0;
}

LgnHelloState::LgnHelloState(int aggrToken, 
			     const NodeID * localNode, 
			     const NodeID * remoteNode) :
  State(), _LocNodeID(0), _RemNodeID(0), _AggToken(aggrToken),
  _LocPortID(-1), _RemPortID(0), _cs(0), _creator(0)
{
  AllocateStates();
  _cs = _Down;
  _ref_count++;

  if (localNode)  assert(_LocNodeID = localNode->copy());
  if (remoteNode) assert(_RemNodeID = remoteNode->copy());

  assert(_LgnInactivityTimer = new LgnInactivityTimer(this, _LgnLinkInactivityTime));
  Register(_LgnInactivityTimer);

  if (!_fastuni_vistype)
    _fastuni_vistype = (VisitorType *)QueryRegistry(FAST_UNI_VISITOR_NAME);
}

LgnHelloState::LgnHelloState(int aggrToken, 
			     const NodeID * localNode, 
			     int localPort,
			     const NodeID * remoteNode) :
  State(), _LocNodeID(0), _RemNodeID(0), _AggToken(aggrToken),
  _LocPortID(localPort), _RemPortID(0), _cs(0), _creator(0)
{
  AllocateStates();
  _cs = _Down;
  _ref_count++;

  if (localNode)  assert(_LocNodeID = localNode->copy());
  if (remoteNode) assert(_RemNodeID = remoteNode->copy());

  assert(_LgnInactivityTimer = new LgnInactivityTimer(this, _LgnLinkInactivityTime));
  Register(_LgnInactivityTimer);

  if (!_fastuni_vistype)
    _fastuni_vistype = (VisitorType *)QueryRegistry(FAST_UNI_VISITOR_NAME);
}


LgnHelloState::~LgnHelloState()
{
  if (--_ref_count == 0)
    FreeStates();

  delete _LocNodeID;
  delete _RemNodeID;
  delete _LgnInactivityTimer;
  ClearInducingLinks();

  // Dependancy Violation with libsim!!  mountcas 11/04/98
  //  if (_creator)
  //    _creator->LgnHelloDeparting( _AggToken );
}

void LgnHelloState::SetCreator(LgnHelloFSMCreator * c)
{
  _creator = c;
  // if (!c) // creator is departing what do I do?
}

void LgnHelloState::ChangeState(LgnHelloState::StateNames n)
{
  switch (n) {
    case StateDown:
      _cs = _Down;
      diag("fsm.hello.lgn", DIAG_DEBUG, "%s:  Going into DOWN State at %lf\n",OwnerName(),
	   (double)(theKernel().CurrentElapsedTime()));
      break;
    case StateAttempt:
      _cs = _Attempt;
      diag("fsm.hello.lgn", DIAG_DEBUG, "%s:  Going into ATTEMPT State at %lf\n",OwnerName(),
	   (double)(theKernel().CurrentElapsedTime()));
      break;
    case StateOneWayInside:
      _cs = _OneWay;
      diag("fsm.hello.lgn", DIAG_DEBUG, "%s:  Going into 1-WAYINSIDE State at %lf\n",OwnerName(),
	   (double)(theKernel().CurrentElapsedTime()));
      break;
    case StateTwoWayInside:
      _cs = _TwoWay;
      diag("fsm.hello.lgn", DIAG_DEBUG, "%s:  Going into 2-WAYINSIDE State at %lf\n",OwnerName(),
	   (double)(theKernel().CurrentElapsedTime()));
      break;
    default:
      diag("fsm.hello.lgn", DIAG_FATAL, "%s:  Invalid State at %lf\n",OwnerName(),
	   (double)(theKernel().CurrentElapsedTime()));
      // abort();
      break;
  }
}

const int LgnHelloState::GetLocPortID(void) const
{ return _LocPortID; }

const int LgnHelloState::GetRemPortID(void) const
{ return _RemPortID; }

const int LgnHelloState::GetAggrToken(void) const
{ return _AggToken; }

const NodeID * LgnHelloState::GetLocNodeID(void) const
{ return _LocNodeID; }

const NodeID * LgnHelloState::GetRemNodeID(void) const
{ return _RemNodeID; }

void      LgnHelloState::SetLocPortID(int p)
{ _LocPortID = p; }

void      LgnHelloState::SetRemPortID(int p)
{ _RemPortID = p; }

void      LgnHelloState::SetAggrToken(int t)
{ _AggToken = t; }

void      LgnHelloState::SetLocNodeID(const NodeID * n)
{
  delete _LocNodeID;
  _LocNodeID = (n ? n->copy() : 0);
}

void      LgnHelloState::SetRemNodeID(const NodeID * n)
{
  delete _RemNodeID;
  _RemNodeID = (n ? n->copy() : 0);
}

void LgnHelloState::RegisterInactivityTimer(void)
{
  Cancel(_LgnInactivityTimer);
  Register(_LgnInactivityTimer);
}

void LgnHelloState::CancelInactivityTimer(void)
{
  Cancel(_LgnInactivityTimer);
}

void LgnHelloState::DoLgnHelloProtocol(int n, HorLinkVisitor * v)
{
  switch (n) {
    case 0:
      // Action: Do nothing.
      break;
    case 1:
      // Action: Set the RemPortID to the PortID listed in 
      // the entry for the Aggregation Token in the received 
      // LGN HLink Ext IG.
      _RemPortID = v->GetRemotePort();
      break;
    case 2:
      // Action: Set the RemPortID to the PortID listed in 
      // the entry for the Aggregation Token in the received 
      // LGN HLink Ext IG.  
      _RemPortID = v->GetRemotePort();
      // Trigger an advertisement of the HLink in a new 
      // instance of a HLink PTSE originated by this node, 
      // provided the corresponding Neighboring Peer State 
      // machine is in state Full.
      SendLinkUp();
      break;
    case 3:
      // Action: Trigger an advertisement of the horizontal 
      // link in the new instance of a horizontal link PTSE 
      // originated by this node, provided the corresponding 
      // Neighboring Peer State machine is in state Full.
      SendLinkUp();
      break;
    case 4:
      // Action: The RemPortID is cleared.
      _RemPortID = 0x0;
      break;
    case 5:
      // Action: The RemPortID is cleared.  
      _RemPortID = 0x0;
      // The HLink must be removed from the 
      // PTSE originated by this node in 
      // which it has been advertised.
      SendLinkDown();
      break;
    case 6:
      // Action: The HLink must be removed from the PTSE 
      // originated by this node in which it has been 
      // advertised.
      SendLinkDown();
      break;
    case 10:
    case 11:
      // Action: Add the inducing uplink (identified by 
      // the NodeID of the border node in the child peer 
      // group and the PortID for the inducing uplink) to 
      // the _induced_uplinks list.
      AddInducingLink(v->GetBorderNode(), v->GetBorderPort());
      break;
    case 12:
      // Action: Add the inducing uplink (identified by 
      // the NodeID of the border node in the child peer 
      // group and the PortID for the inducing uplink) to 
      // the _induced_uplinks list.  
      AddInducingLink(v->GetBorderNode(), v->GetBorderPort());
      // If the addition of the inducing uplink causes a 
      // significant change in the topology state parameters 
      // for the aggregated horizontal link, originate a new 
      // instance of the horizontal link PTSE.
      SendAddInduced();
      break;
    case 13:
      // Action: Delete the inducing uplink (identified by the 
      // NodeID of the border node in the child peer group and 
      // the PortID for the inducing uplink) from the 
      // _induced_uplinks list.
      RemInducingLink(v->GetBorderNode(), v->GetBorderPort());
      break;
    case 14:
      // Action: Delete the inducing uplink (identified by the
      // NodeID of the border node in the child peer group and 
      // the PortID for the inducing uplink) from the 
      // _induced_uplinks list.  
      RemInducingLink(v->GetBorderNode(), v->GetBorderPort());
      // if the deletion of the inducing uplink causes a 
      // significant change in the topology state paramters 
      // for the aggregated horizontal link, originate a new 
      // instance of the horizontal link PTSE.
      SendDropInduced();
      break;
    case 15:
      // Action: The RemPortID is cleared.
      _RemPortID = 0x0;
      // Delete the inducing uplink (identified by the NodeID of the
      // border node in the child peer group and the PortID for the
      // inducing uplink) from the _induced_uplinks list.
      RemInducingLink(v->GetBorderNode(), v->GetBorderPort());
      // Originate a new instance of the horizontal link PTSE that
      // does not include any entry for this horizontal link, or flush
      // the PTSE if there is nothing else in the PTSE.
      SendDropInduced();
      break;
    case 16:
      // Action: The RemPortID is cleared.  
      _RemPortID = 0x0;
      // Delete the inducing uplink (identified by the NodeID of the
      // border node in the child peer group and the PortID for the
      // inducing uplink) from the _induced_uplinks list.
      RemInducingLink(v->GetBorderNode(), v->GetBorderPort());
      break;
    default:
      diag("fsm.hello.lgn", DIAG_WARNING, "%s: Attempt to execute undefined LgnHelloProtocol %d\n", 
	   OwnerName(), n);
      break;
  }
}

void LgnHelloState::AddInducingLink(const NodeID * border, int port)
{
  list_item li;
  InducedUplink * iul = new InducedUplink(border, port);
  // First see if we have it in our list already
  if (!(li = _induced_uplinks.search(iul))) {
    // if it's not there already add it
    _induced_uplinks.append(iul);
  } else // Otherwise free the memory we alloc'ed
    delete iul;
}

void LgnHelloState::RemInducingLink(const NodeID * border, int port)
{
  list_item li;
  InducedUplink iul(border, port);

  if (li = _induced_uplinks.search(&iul)) {
    InducedUplink * rem = _induced_uplinks.inf(li);
    delete rem;
    _induced_uplinks.del_item(li);
  }
}

void LgnHelloState::ClearInducingLinks(void)
{
  list_item li;
  forall_items(li, _induced_uplinks) {
    InducedUplink * rem = _induced_uplinks.inf(li);
    delete rem;
  }
  _induced_uplinks.clear();
}

void LgnHelloState::PassThroughVisitor(Visitor * v)
{
  PassThru(v);
}

void LgnHelloState::BounceBackVisitor(Visitor * v)
{
  switch (VisitorFrom(v)) {
    case Visitor::A_SIDE:
      PassVisitorToA(v);
      break;
    case Visitor::B_SIDE:
      PassVisitorToB(v);
      break;
  }
}

const char * const LgnHelloState::GetName(void)
{
  return OwnerName();
}

State * LgnHelloState::Handle(Visitor * v)
{
  if (v->GetType().Is_A(_fastuni_vistype)) {
    FastUNIVisitor * fv = (FastUNIVisitor *)v;
    if (fv->GetMSGType() == FastUNIVisitor::FastUNILHD) {
      assert (VisitorFrom(v) == Visitor::B_SIDE);
#ifdef __REAL_THING__
      Free();
      v->Suicide();
      return 0;
#else
      cout << "###Bilal###: LGNH " << (long)this << " would be deleted.\n";
      return this;
#endif
    }
  }

  _cs->Handle(this, v);
  return this;
}

void LgnHelloState::Interrupt(SimEvent * se)
{
  // The only SimEvents this FSM is expecting come from 
  // Aggregator and contain the logical port number
  _LocPortID = se->GetCode();
  delete se;

  // if we are in TwoWayInside, we should send down the HLinkUp message.
  if (_cs == _TwoWay)
    SendLinkUp();
}

void LgnHelloState::SendLinkUp(void)
{
  HorLinkVisitor * hlv = new HorLinkVisitor(HorLinkVisitor::HLinkUp, 
					    _LocNodeID, _RemNodeID, 0, -1,
					    _AggToken, _LocPortID, _RemPortID);
  // This should cause the Aggregator to originate a logical HLink PTSE
  PassVisitorToB(hlv);
}

void LgnHelloState::SendLinkDown(void)
{
  HorLinkVisitor * hlv = new HorLinkVisitor(HorLinkVisitor::HLinkDown, 
					    _LocNodeID, _RemNodeID, 0, -1, 
					    _AggToken, _LocPortID, _RemPortID);
  // This should cause the Aggregator to expunge the logical HLink PTSE
  PassVisitorToB(hlv);
}

void LgnHelloState::SendAddInduced(void)
{
  HorLinkVisitor * hlv = new HorLinkVisitor(HorLinkVisitor::AddInducedLink, 
					    _LocNodeID, _RemNodeID, 0, -1,
					    _AggToken, _LocPortID, _RemPortID);
  // We let the Aggregator determine if this is a siginificant change or not.
  PassVisitorToB(hlv);
}

void LgnHelloState::SendDropInduced(void)
{
  HorLinkVisitor * hlv = new HorLinkVisitor(HorLinkVisitor::DropInducedLink, 
					    _LocNodeID, _RemNodeID, 0, -1,
					    _AggToken, _LocPortID, _RemPortID);
  // We let the Aggregator determine if this is a siginificant change or not.
  PassVisitorToB(hlv);
}

InternalLgnHelloState * LgnHelloState::GetCS(void) const
{ return _cs; }

enum Visitor::which_side LgnHelloState::WhichSide(Visitor * v)
{ 
  return VisitorFrom(v); 
}

// ------------------- InducedUplink -----------------
InducedUplink::InducedUplink(const NodeID * border, int port) :
  _portID(port), _border(0)
{
  if (border)
    _border = border->copy();
}

InducedUplink::InducedUplink(const InducedUplink & rhs) :
  _portID(rhs._portID), _border(0)
{
  if (rhs._border)
    _border = rhs._border->copy();
}


InducedUplink::~InducedUplink()
{
  delete _border;
}

const NodeID * InducedUplink::GetBorderNode(void) const
{
  return _border;
}

const int      InducedUplink::GetPortID(void) const
{
  return _portID;
}

// -------------------- InternalLgnHelloState -----------------------
VisitorType * InternalLgnHelloState::_horlink_vistype = 0;
VisitorType * InternalLgnHelloState::_fastuni_vistype = 0;
VisitorType * InternalLgnHelloState::_npstate_vistype = 0;

InternalLgnHelloState::InternalLgnHelloState(void) 
{ 
  if (!_horlink_vistype)
    _horlink_vistype = (VisitorType *)QueryRegistry(HLINK_VISITOR_NAME);
  if (!_fastuni_vistype)
    _fastuni_vistype = (VisitorType *)QueryRegistry(FAST_UNI_VISITOR_NAME);
  if (!_npstate_vistype)
    _npstate_vistype = (VisitorType *)QueryRegistry(NPSTATE_VISITOR_NAME);
}

InternalLgnHelloState::~InternalLgnHelloState() { }

void InternalLgnHelloState::Handle(LgnHelloState * l, Visitor * v)
{
  switch (GetEventType(l, v)) {
    case AddInducingLink:
      RcvAddInducingLink(l, (HorLinkVisitor *)v);
      break;
    case OneWayReceived:
      // Restart the InactivityTimer
      l->RegisterInactivityTimer();
      RcvOneWayInside(l, (HorLinkVisitor *)v);
      break;
    case TwoWayReceived:
      // Restart the InactivityTimer
      l->RegisterInactivityTimer();
      RcvTwoWayInside(l, (HorLinkVisitor *)v);
      break;
    case HelloMismatch:
      RcvHelloMismatch(l, (HorLinkVisitor *)v);
      break;
    case BadNeighbor:
      RcvBadNeighbor(l, (HorLinkVisitor *)v);
      break;
    case DropInducingLink:
      RcvDropInducingLink(l, (HorLinkVisitor *)v);
      break;
    case DropLastInducingLink:
      RcvDropLastInducingLink(l, (HorLinkVisitor *)v);
      break;
    case PassThrough:
      l->PassThroughVisitor(v);
      v = 0x0;
      break;
    case PassBack:
      l->BounceBackVisitor(v);
      v = 0x0;
      break;
    case Invalid:
      v->Suicide();
      v = 0x0;
      break;
    default:
      diag("fsm.hello.lgn", DIAG_WARNING, 
	   "Unable to handle %s, so the LgnHello is killing it.\n", v->GetType().Name());
      break;
  }

  if (v)
    v->Suicide();
}

InternalLgnHelloState::EventTypes 
InternalLgnHelloState::GetEventType(LgnHelloState * l, Visitor * v)
{
  EventTypes rval = PassThrough;

  if (v->GetType().Is_A(_horlink_vistype)) {
    HorLinkVisitor * hlv = (HorLinkVisitor *)v;
    switch (hlv->GetVT()) {
      case HorLinkVisitor::BadNeighbor:
        rval = BadNeighbor;
        break;
      case HorLinkVisitor::HelloMismatch:
        rval = HelloMismatch;
        break;
      case HorLinkVisitor::AddInducedLink:
        rval = AddInducingLink;
        break;
      case HorLinkVisitor::DropInducedLink:
        rval = DropInducingLink;
        break;
      case HorLinkVisitor::DropLastInducedLink:
        rval = DropLastInducingLink;
        break;
      case HorLinkVisitor::LgnHello:
	{
	  int agg = hlv->GetAgg(), 
	      lport = hlv->GetLocalPort(), 
	      rport = hlv->GetRemotePort();
	  
	  // Is this a request for information?
	  if (agg == -1 && lport == -1 && rport == -1) {
	    hlv->SetAgg( l->GetAggrToken() );
	    hlv->SetLocalPort( l->GetLocPortID() );
	    hlv->SetRemotePort( l->GetRemPortID() );
	    rval = PassBack;
	  } else {
	    diag("fsm.hello.lgn", DIAG_DEBUG, "hlv->port = %d link->localPort = %d at %lf\n",
		 lport,l->GetLocPortID(),(double)(theKernel().CurrentElapsedTime()));
	    // Steal the Logical Port out of the HorLinkVisitor
	    l->SetRemPortID(rport);
	    // We passed in his remote port here, which should either be 0 or our local port
	    // AB 01/21/99: -1 really means ZERO till I find out where it comes from
	    // we shouldn't be getting this anymore -- fixed in RCCHelloState::ParseLgnExtLinks(lgnextn)
	    if (lport == 0 || lport == -1)
	      {
		rval = OneWayReceived;  // He doesn't know our logical port yet
		if(lport == -1)
		  diag("fsm.hello.lgn", DIAG_DEBUG, "hlv->port = %d SHOULD BE ZERO at %lf\n",
		       lport,(double)(theKernel().CurrentElapsedTime()));
	      }
	    // our local port id is equal to his remote port id
	    else if (lport == l->GetLocPortID())
	      rval = TwoWayReceived;  // He knows our logical port and we should know his
	    else
	      {
		rval = HelloMismatch;
		diag("fsm.hello.lgn", DIAG_DEBUG, "HelloMismatch due to hlv->port = %d link->localPort = %d at %lf\n",
		     lport,l->GetLocPortID(),(double)(theKernel().CurrentElapsedTime()));
	      }
	  }
	}
        break;
      case HorLinkVisitor::HLinkUp: 
	{
	  // Received when the RCC Hello reaches TwoWayInside
	  if (hlv->GetDestNID()) // pull out the remote logical NodeID
	    l->SetRemNodeID(hlv->GetDestNID());  // This method makes a copy, don't worry

	  // This turns it into a Request Logical Port HorLinkVisitor
	  hlv->SetVT(HorLinkVisitor::ReqLogicalPort);
	  SimEvent * se = new SimEvent(l, l, 0);
	  hlv->SetReturnEvent(se);
	  hlv->SetAgg(l->GetAggrToken());
	  // Pass it on to Aggregator to obtain the Logical Port ID
	  rval = PassThrough;
	}
	break;
      case HorLinkVisitor::HLinkDown:
	// Received when our uplink goes down, so we should revert back to Down or Attempt
	rval = HelloMismatch;
	break;
      default:
        rval = Invalid;
        break;
    }
  } else if (v->GetType().Is_A(_fastuni_vistype)) {
    // Extract the Logical Port if present
    FastUNIVisitor * fv = (FastUNIVisitor *)v;
    if (fv->GetLogicalPort() != -1)
      l->SetLocPortID(fv->GetLogicalPort());

    if (fv->GetMSGType() == FastUNIVisitor::FastUNILHIRebinder) {
      l->SetAggrToken(fv->GetAgg());
      
      if (l->WhichSide(v) == Visitor::A_SIDE)
	return Invalid; // v->Suicide();
    }

    rval = PassThrough;
  }
  return rval;
}

void InternalLgnHelloState::RcvAddInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{ diag("fsm.hello.lgn", DIAG_WARNING, "%s in FSM Error.\n", l->GetName()); }

void InternalLgnHelloState::RcvOneWayInside(LgnHelloState * l, HorLinkVisitor * v)
{ diag("fsm.hello.lgn", DIAG_WARNING, "%s in FSM Error.\n", l->GetName()); }

void InternalLgnHelloState::RcvTwoWayInside(LgnHelloState * l, HorLinkVisitor * v)
{ diag("fsm.hello.lgn", DIAG_WARNING, "%s in FSM Error.\n", l->GetName()); }

void InternalLgnHelloState::RcvHelloMismatch(LgnHelloState * l, HorLinkVisitor * v)
{ diag("fsm.hello.lgn", DIAG_WARNING, "%s in FSM Error.\n", l->GetName()); }

void InternalLgnHelloState::RcvBadNeighbor(LgnHelloState * l, HorLinkVisitor * v)
{ diag("fsm.hello.lgn", DIAG_WARNING, "%s in FSM Error.\n", l->GetName()); }

void InternalLgnHelloState::RcvDropInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{ diag("fsm.hello.lgn", DIAG_WARNING, "%s in FSM Error.\n", l->GetName()); }

void InternalLgnHelloState::RcvDropLastInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{ diag("fsm.hello.lgn", DIAG_WARNING, "%s in FSM Error.\n", l->GetName()); }

void InternalLgnHelloState::RcvHLinkInactivityExpiry(LgnHelloState * l)
{ diag("fsm.hello.lgn", DIAG_WARNING, "%s in FSM Error.\n", l->GetName()); }

// ------------------------- LgnDownState ----------------------------------
LgnDown::LgnDown(void) : InternalLgnHelloState() { }
LgnDown::~LgnDown() { }

void LgnDown::RcvAddInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(10, v);
  l->ChangeState(LgnHelloState::StateAttempt);
}

void LgnDown::RcvOneWayInside(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(0, v);
  l->ChangeState(LgnHelloState::StateDown);
}  
void LgnDown::RcvTwoWayInside(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(0, v);
  l->ChangeState(LgnHelloState::StateDown);
}

void LgnDown::RcvHelloMismatch(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(0, v);
  l->ChangeState(LgnHelloState::StateDown);
}

void LgnDown::RcvBadNeighbor(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(0, v);
  l->ChangeState(LgnHelloState::StateDown);
}

void LgnDown::RcvHLinkInactivityExpiry(LgnHelloState * l)
{
  l->DoLgnHelloProtocol(0);
  l->ChangeState(LgnHelloState::StateDown);
}

const char * const LgnDown::CurrentStateName(void) const { return "LgnStateDown"; }

// ----------------------- LgnAttempt State ------------------------
LgnAttempt::LgnAttempt(void) : InternalLgnHelloState() { }
LgnAttempt::~LgnAttempt() { }

void LgnAttempt::RcvAddInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(11, v);
  l->ChangeState(LgnHelloState::StateAttempt);
}

void LgnAttempt::RcvOneWayInside(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(1, v);
  l->ChangeState(LgnHelloState::StateOneWayInside);
}

void LgnAttempt::RcvTwoWayInside(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(2, v);
  l->ChangeState(LgnHelloState::StateTwoWayInside);
}

void LgnAttempt::RcvHelloMismatch(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(0, v);
  l->ChangeState(LgnHelloState::StateAttempt);
}

void LgnAttempt::RcvBadNeighbor(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(0, v);
  l->ChangeState(LgnHelloState::StateAttempt);
}

void LgnAttempt::RcvDropInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(13, v);
  l->ChangeState(LgnHelloState::StateAttempt);
}

void LgnAttempt::RcvDropLastInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(13, v);
  l->ChangeState(LgnHelloState::StateDown);
}

void LgnAttempt::RcvHLinkInactivityExpiry(LgnHelloState * l)
{
  l->DoLgnHelloProtocol(0);
  l->ChangeState(LgnHelloState::StateAttempt);
}

const char * const LgnAttempt::CurrentStateName(void) const { return "LgnStateAttempt"; }

// ------------------------------- LgnOneWayInside -------------------
LgnOneWayInside::LgnOneWayInside(void) : InternalLgnHelloState() { }
LgnOneWayInside::~LgnOneWayInside() { }

void LgnOneWayInside::RcvAddInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(11, v);
  l->ChangeState(LgnHelloState::StateOneWayInside);
}

void LgnOneWayInside::RcvOneWayInside(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(0, v);
  l->ChangeState(LgnHelloState::StateOneWayInside);
}

void LgnOneWayInside::RcvTwoWayInside(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(3, v);
  l->ChangeState(LgnHelloState::StateTwoWayInside);
}

void LgnOneWayInside::RcvHelloMismatch(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(4, v);
  l->ChangeState(LgnHelloState::StateAttempt);
}

void LgnOneWayInside::RcvBadNeighbor(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(4, v);
  l->ChangeState(LgnHelloState::StateAttempt);
}

void LgnOneWayInside::RcvDropInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(13, v);
  l->ChangeState(LgnHelloState::StateOneWayInside);
}

void LgnOneWayInside::RcvDropLastInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(16, v);
  l->ChangeState(LgnHelloState::StateDown);
}

void LgnOneWayInside::RcvHLinkInactivityExpiry(LgnHelloState * l)
{
  l->DoLgnHelloProtocol(4);
  l->ChangeState(LgnHelloState::StateAttempt);
}

const char * const LgnOneWayInside::CurrentStateName(void) const { return "LgnStateOneWayInside"; }

// -------------------------------- LgnTwoWayInside ---------------------------
LgnTwoWayInside::LgnTwoWayInside(void) : InternalLgnHelloState() { }
LgnTwoWayInside::~LgnTwoWayInside() { }

void LgnTwoWayInside::RcvAddInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(12, v);
  l->ChangeState(LgnHelloState::StateTwoWayInside);
}

void LgnTwoWayInside::RcvOneWayInside(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(6, v);
  l->ChangeState(LgnHelloState::StateOneWayInside);
}

void LgnTwoWayInside::RcvTwoWayInside(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(0, v);
  l->ChangeState(LgnHelloState::StateTwoWayInside);
}

void LgnTwoWayInside::RcvHelloMismatch(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(5, v);
  l->ChangeState(LgnHelloState::StateAttempt);
}

void LgnTwoWayInside::RcvBadNeighbor(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(5, v);
  l->ChangeState(LgnHelloState::StateAttempt);
}

void LgnTwoWayInside::RcvDropInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(14, v);
  l->ChangeState(LgnHelloState::StateTwoWayInside);
}

void LgnTwoWayInside::RcvDropLastInducingLink(LgnHelloState * l, HorLinkVisitor * v)
{
  l->DoLgnHelloProtocol(15, v);
  l->ChangeState(LgnHelloState::StateDown);
}

void LgnTwoWayInside::RcvHLinkInactivityExpiry(LgnHelloState * l)
{
  l->DoLgnHelloProtocol(5);
  l->ChangeState(LgnHelloState::StateAttempt);
}

const char * const LgnTwoWayInside::CurrentStateName(void) const { return "LgnStateTwoWayInside"; }
