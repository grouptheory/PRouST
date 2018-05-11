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
static char const _NodePeer_cc_rcsid_[] =
"$Id: NodePeer.cc,v 1.5 1998/09/04 18:25:38 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <fsm/nodepeer/NodePeer.h>
#include <fsm/nodepeer/NodePeerTimers.h>

NodePeerState::NodePeerState(const NodeID * LocNodeID, const NodeID * RemNodeID, 
			     int vp, int vc, u_long cref)
  : State(), _LocNodeID(0), _RemNodeID(0), _vp(vp), _vc(vc), _crv(cref),
    _DSSequenceNumber(0), _PeerDelayedAckTimer(0), _DSRxmtTimer(0), 
    _RequestRxmtTimer(0), _Mode(NodePeerState::Master)
{
  assert( LocNodeID && RemNodeID );
  // Allocate the NodeIDs
  _LocNodeID = new NodeID( *LocNodeID );
  _RemNodeID = new NodeID( *RemNodeID );
  // Allocate the Timers
  _PeerDelatedAckTimer = new PeerDelayedAckTimer(this, PeerDelayedAckInterval);
  _DSRxmtTimer         = new DSRxmtTimer(this, DSRxmtInterval);
  _RequestRxmtTimer    = new RequestRxmtTimer(this, RequestRxmtInterval);
}

NodePeerState::~NodePeerState() 
{ 
  delete _LocNodeID;
  delete _RemNodeID;

  delete _PeerDelatedAckTimer;
  delete _DSRxmtTimer;
  delete _RequestRxmtTimer;
}

State * NodePeerState::Handle(Visitor * v)
{
  assert(_current_state != 0);
  _current_state = _current_state->Handle(this, v);
  return this;
}

void    NodePeerState::Interrupt(SimEvent * e)
{
  // do nothing
}

NodePeerState::NodePeerEvents NodePeerState::VisitorEvent(Visitor * v)
{
  // switch on vistype and return the proper event
  NodePeerEvents rval = Invalid;

  return rval;
}

void NodePeerState::do_ds(Visitor *v, int code)
{
  switch (code) {
    case 0:
      break;
    case 1: 
      {
	PortUpVisitor * pv = (PortUpVisitor *)v;
	// the port ID is added to the PortID list
	_PortID.insert( _nextPort++, 
			(pv->GetInPort() > 0 ? pv->GetInPort() : pv->GetRemotePort()) );
	// the DSSequenceNumber is incremented
	_DSSequenceNumber++;
	// start sending DB Summaries with the Init, More, and Master bits set
	DatabaseSumPkt * sendDSPkt = new DatabaseSumPkt(_DSSequenceNumer);
	sendDSPkt->SetBit( DatabaseSumPkt::init_bit |
			   DatabaseSumPkt::more_bit |
			   DatabaseSumPkt::master_bit );
	SendPacket( sendDSPkt );
      }
      break;
    case 2:
      {
	DatabaseSumPkt * sendDSPkt = _Dbase->GetDatabaseSummary(Level);
	if (!sendDSPkt) {
	  // Case where there is nothing in our Database
	  sendDSPkt = new DatabaseSumPkt(_DSSequenceNumber);
	}
	// Assume that _DSSequenceNumber was set properly prior to calling this
	sendDSPkt->SetSequenceNum(_DSSequenceNumber);
	sendDSPkt->RemBit(DatabaseSumPkt::init_bit);
	if (_Mode == NodePeerState::Master) {
	  sendDSPkt->SetBit( DatabaseSumPkt::master_bit |
			     DatabaseSumPkt::more_bit );
	  if (_storedReqPkt)
	    SendRequestPacket( );
	} else {
	  sendDSPkt->RemBit( DatabaseSumPkt::master_bit |
			     DatabaseSumPkt::more_bit );
	}
	SendPacket( sendDSPkt );
      }
      break;
    case 3:
      {
	// Stop the DS Retransmit Timer if running
	StopTimer(NodePeerState::DSRxmtTimer);
	// Start, or continue sending PTSE Requests
	SendRequestPacket( );
      }
      break;
    case 4:
      {
	// Stop the DS Retransmit Timer if running
	StopTimer(NodePeerState::DSRxmtTimer);
	// The databases are now synchronized
      }
      break;
    case 5:
      {
	StopTimer(NodePeerState::DSRxmtTimer);
	StopTimer(NodePeerState::PeerDelayedAckTimer);
	StopTimer(NodePeerState::RequestRxmtTimer);
	// Clear the PeerRetransmission, PeerDelayedAcks, and PTSEReq lists

	// Exchange must start all over again
	_Mode = Master;
	// the DSSequenceNumber is incremented
	_DSSequenceNumber++;
	// start sending DB Summaries with the Init, More, and Master bits set
	DatabaseSumPkt * sendDSPkt = new DatabaseSumPkt(_DSSequenceNumer);
	sendDSPkt->SetBit( DatabaseSumPkt::init_bit |
			   DatabaseSumPkt::more_bit |
			   DatabaseSumPkt::master_bit );
	SendPacket( sendDSPkt );
      }
      break;
    case 6:
      {
	StopTimer(NodePeerState::DSRxmtTimer);
	StopTimer(NodePeerState::PeerDelayedAckTimer);
	StopTimer(NodePeerState::RequestRxmtTimer);
	// Clear the PeerRetransmission, PeerDelayedAcks, and PTSEReq lists

	// PTSEs must be modified to remove the links and re-originated or flushed

	// Exchange must start all over again
	_Mode = Master;
	// the DSSequenceNumber is incremented
	_DSSequenceNumber++;
	// start sending DB Summaries with the Init, More, and Master bits set
	DatabaseSumPkt * sendDSPkt = new DatabaseSumPkt(_DSSequenceNumer);
	sendDSPkt->SetBit( DatabaseSumPkt::init_bit |
			   DatabaseSumPkt::more_bit |
			   DatabaseSumPkt::master_bit );
	SendPacket( sendDSPkt );
      }
      break;
    case 7:
    case 8:
      {
	PortUpVisitor * pv = (PortUpVisitor *)v;
	// the port ID is added to the PortID list
	_PortID.insert( _nextPort++, 
			(pv->GetInPort() > 0 ? pv->GetInPort() : pv->GetRemotePort()) );
      }
      break;
    case 9:
      {
	// The link is removed from the PortID list
	PortDownVisitor * pv = (PortDownVisitor *)v;
	// the port ID is added to the PortID list
	int port = (pv->GetInPort() > 0 ? pv->GetInPort() : pv->GetRemotePort());
	dic_item di;
	forall_items(di, _PortID) {
	  if (_PortID.inf(di) == port) {
	    _PortID.del_item(di);
	    break;
	  }
	}
	// If this was the last active link to the neighbor generate the DropPortLast event
      }
      break;
    case 10:
      {
	StopTimer(NodePeerState::DSRxmtTimer);
	StopTimer(NodePeerState::PeerDelayedAckTimer);
	StopTimer(NodePeerState::RequestRxmtTimer);
	// Clear the PeerRetransmission, PeerDelayedAcks, and PTSEReq lists

      }
      break;
    default:
      break;
  }
}

void NodePeerState::StartTimer(NodePeerTimers t)
{
  switch (t) {
    case PeerDelayedAckTimer:
      Register(_PeerDelayedAckTimer);
      break;
    case DSRxmtTimer:
      Register(_DSRxmtTimer);
      break;
    case RequestRxmtTimer:
      Register(_RequestRxmtTimer);
      break;
  }
}

void NodePeerState::StopTimer(NodePeerTimers t)
{
  switch (t) {
    case PeerDelayedAckTimer:
      Cancel(_PeerDelayedAckTimer);
      break;
    case DSRxmtTimer:
      Cancel(_DSRxmtTimer);
      break;
    case RequestRxmtTimer:
      Cancel(_RequestRxmtTimer);
      break;
  }
}

void NodePeerState::SendPacket(DatabaseSumPkt * pkt)
{
  // Stop the DSRxmtTimer
  StopTimer(DSRxmtTimer);
  // Store a copy to send if/when the DSRxmtTimer expires
  _storedDSPkt = pkt->copy();
  // Send out the original
  int port = _PortID.head();
  DSVisitor * outgoingPkt = new DSVisitor(_LocNodeID, _RemNodeID, pkt, port);
  outgoingPkt->SetOutPort( port );
  outgoingPkt->SetOutVP(0); outgoingPkt->SetOutVC(18);
  outgoingPkt->SetInPort(0);
  outgoingPkt->SetInVP(0); outgoingPkt->SetInVC(18);
  PassVisitorToA( outgoingPkt );
  // Restart the DSRxmtTimer
  StartTimer(DSRxmtTimer);
}

void NodePeerState::ResendPacket(void)
{
  SendPacket(_storedDSPkt);
}

// --------------------- InternalNodePeerState -----------------------
InternalNodePeerState::InternalNodePeerState(void) { }

InternalNodePeerState::~InternalNodePeerState( ) { }

InternalNodePeerState * InternalNodePeerState::Handle(NodePeerState * s, Visitor * v)
{
  // The current state is the default rval
  InternalNodePeerState * rval = s->GetCS();

  switch ( s->VisitorEvent( v ) ) {
    case NodePeerState::AddPort:
      RcvAddPort(s, (PortUpVisitor *)v);
      break;
    case NodePeerState::NegotiationDone:
      RcvNegotiationDone(s, v);
      break;
    case NodePeerState::ExchangeDone:
      RcvExchangeDone(s, v);
      break;
    case NodePeerState::SynchDone:
      RcvSynchDone(s, v);
      break;
    case NodePeerState::LoadingDone:
      RcvLoadingDone(s, v);
      break;
    case NodePeerState::DSMismatch:
      RcvDSMismatch(s, v);
      break;
    case NodePeerState::BadPTSERequest:
      RcvBadPTSEReq(s, v);
      break;
    case NodePeerState::DropPort:
      RcvDropPort(s, v);
      break;
    case NodePeerState::DropPortLast:
      RcvDropPortLast(s, v);
      break;
    case NodePeerState::Invalid:
    default:
      v->Sucide();
      break;
  }

  return rval;
}

void InternalNodePeerState::do_ds(NodePeerState * s, Visitor * v, int code)
{
  s->do_ds(v, code);
}

int      InternalNodePeerState::getVC(NodePeerState * s)
{
  return s->_vci;
}

int      InternalNodePeerState::getVP(NodePeerState * s)
{
  return s->_vpi;
}

u_long   InternalNodePeerState::getCRV(NodePeerState * s)
{
  return s->_crv;
}

NodeID * InternalNodePeerState::getLocalNode(NodePeerState * s)
{
  return s->_LocNodeID;
}

NodeID * InternalNodePeerState::getRemoteNode(NodePeerState * s)
{
  return s->_RemNodeID;
}

void     InternalNodePeerState::StartTimer(NodePeerState::Timers t)
{
  s->StartTimer(t);
}

void     InternalNodePeerState::StopTimer(NodePeerState::Timers t)
{
  s->StopTimer(t);
}
