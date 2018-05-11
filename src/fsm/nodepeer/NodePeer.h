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
//
// NOTICE:  This is the beginning of an attempt to reimplement
//    the NodePeer FSM originally implemented by Sandeep Bhat.
//    Unfortunately since he was not located at NRL with us, 
//    there were difficulties in communication which resulted
//    in an implementation of the NodePeer FSM that is difficult
//    for us to maintain and for new developers to understand.
//
//    As of 10/1/98 this implementation has not been completed
//    due to time constraints.  Please use the existing code,
//    located in NodePeerState.{h,cc}
#ifndef __NodePeer_H__
#define __NodePeer_H__

#ifndef LINT
static char const _NodePeer_h_rcsid_[] =
"$Id: NodePeer.h,v 1.7 1998/10/19 20:21:15 mountcas Exp $";
#endif

class InternalNodePeerState;

/**  NodePeerState

     When a node first learns about the existence of a neighboring
     peer node (residing in the same peer group), it initiates a
     database exchange process in order to synchronize the topology
     databases of the neighboring peers.  The database exchange
     process involves the exchange of a sequence of Database Summary
     packets, which contains the indentifying information of all PTSEs
     in a node's topology database.  Database Summary packets are
     exchanged using a lock-step mechanism, whereby one side sends a
     Database Summary packet and other side responds (implicitly
     acknowledging the received packet) with its own Database Summary
     packet.  At most one outstanding packet between the two
     neighboring peers is allowed at any one time.

     When a node receives a Database Summary packet from a neighboring
     peer, it examines its topology database for the presence of each
     PTSE described in the packet.  If the PTSE is not found in the
     topology database or if the neighboring peer has a more recent
     version of the PTSE, then the node must request the PTSE from
     this neigboring peer, or optionally from another neighboring peer
     whose database summary indicates that it has the most recent
     version of the PTSE.  
*/
class NodePeerState : public State {
  friend class InternalNodePeerState;
public:

  enum NodePeerEvents {
    Invalid,
    AddPort,
    NegotiationDone,
    ExchangeDone,
    SynchDone,
    LoadingDone,
    DSMismatch,
    BadPTSERequest,
    DropPort,
    DropPortLast
  };

  enum Mode {
    Master,
    Slave
  };

  enum NodePeerTimers {
    PeerDelayedAckTimer,
    DSRxmtTimer,
    RequestRxmtTimer
  };

  NodePeerState(const NodeID * LocNodeID, const NodeID * RemNodeID, 
		int vp = -1, int vc = -1, u_long cref = 0xffffffff);
  
  State * Handle(Visitor * v);
  void    Interrupt(SimEvent * e);

protected:

  virtual ~NodePeerState();

  NodePeerEvents VisitorEvent(Visitor * v);

  void do_ds(Visitor *v, int code);
  void StartTimer(NodePeerTimers t);
  void StopTimer(NodePeerTimers t);
  void SendPacket(DatabaseSumPkt * send);
  void ResendPacket(void);

private:

  const int               PTSERetransmissionInterval = 5;
  const int               PeerDelayedAckInterval = 1;
  const int               DSRxmtInterval = 5;
  const int               RequestRxmtInterval = 5;
  // This class is for physical node peer only
  const int               Level = 96;

  // Provides the state of the FSM.
  InternalNodePeerState * _current_state;

  const NodeID          * _LocNodeID;
  const NodeID          * _RemNodeID;

  int                     _DSSequenceNumber;
  list<ig_ptse *>         _PeerRetransmissionList;
  list<PTSEAck *>         _PeerDelayedAcksList;
  list<ig_ptse *>         _PTSERequestList;

  PeerDelayedAckTimer   * _PeerDelayedAckTimer;
  DSRxmtTimer           * _DSRxmtTimer;
  RequestRxmtTimer      * _RequestRxmtTimer;

  DatabaseSumPkt        * _storedDSPkt;
  PTSEReqPkt            * _storedReqPkt;

  dictionary<int, u_int>  _PortID;

  Mode                    _Mode;  // Master or Slave?

  int                     _vci;
  int                     _vpi;
  u_long                  _crv;
};

class InternalNodePeerState {
public:

  InternalNodePeerState * Handle(NodePeerState * s, Visitor * v);

protected:

  InternalNodePeerState(void);
  virtual ~InternalNodePeerState( );

  virtual int RcvAddPort(NodePeerState * s, PortUpVisitor * v);
  virtual int RcvNegotiationDone(NodePeerState * s,  );
  virtual int RcvExchangeDone(NodePeerState * s,  );
  virtual int RcvSynchDone(NodePeerState * s,  );
  virtual int RcvLoadingDone(NodePeerState * s,  );
  virtual int RcvDSMismatch(NodePeerState * s,  );
  virtual int RcvBadPTSEReq(NodePeerState * s,  );
  virtual int RcvDropPort(NodePeerState * s,  );
  virtual int RcvDropPortLast(NodePeerState * s,  );

  void do_ds(NodePeerState * s, Visitor * v, int code);

  int      getVC(NodePeerState * s);
  int      getVP(NodePeerState * s);
  u_long   getCRV(NodePeerState * s);
  NodeID * getLocalNode(NodePeerState * s);
  NodeID * getRemoteNode(NodePeerState * s);
  void     StartTimer(NodePeerState::Timers t);
  void     StopTimer(NodePeerState::Timers t);

  const int FSM_OK  =  0;
  const int FSM_ERR = -1;
};

// The initial state of a neighboring peer FSM.  This state indicates
// that there are no active links (i.e. in Hello state 2-WayInside) to
// the neighboring peer.  In this state, there are no adjacencies associated
// with the neighboring peer.
class NodePeerDown : public InternalNodePeerState {
protected:

  NodePeerDown(void);
  virtual ~NodePeerDown( );

  int RcvAddPort(NodePeerState * s, PortUpVisitor * v);
};

// The first step in creating an adjacency between the two neighboring
// peers.  The goal of this step is to decide which node is the master,
// and to decide upon the initial DS sequence number.
class NodePeerNegotiating : public InternalNodePeerState {
protected:

  NodePeerNegotiating(void);
  virtual ~NodePeerNegotiating( );

  int RcvAddPort(NodePeerState * s, PortUpVisitor * v);
  int RcvNegotiationDone(NodePeerState * s,  );
  int RcvDropPort(NodePeerState * s,  );
  int RcvDropPortLast(NodePeerState * s,  );
};

// In this state the node describes its topology database by sending
// Database Summary packets to the neighboring peer.  Following as a
// result of processing Database Summary packets, requred PTSEs can be
// requested.
class NodePeerExchanging : public InternalNodePeerState {
protected:

  NodePeerExchanging(void);
  virtual ~NodePeerExchanging( );

  int RcvAddPort(NodePeerState * s, PortUpVisitor * v);
  int RcvExchangeDone(NodePeerState * s,  );
  int RcvSynchDone(NodePeerState * s,  );
  int RcvDSMismatch(NodePeerState * s,  );
  int RcvBadPTSEReq(NodePeerState * s,  );
  int RcvDropPort(NodePeerState * s,  );
  int RcvDropPortLast(NodePeerState * s,  );
};

// In this state, a full sequence of Database Summary packets has
// been exchanged with the neigboring peer, and the required PTSEs
// are requested and at least one has not yet been received.
class NodePeerLoading : public InternalNodePeerState {
protected:

  NodePeerLoading(void);
  virtual ~NodePeerLoading( );

  int RcvAddPort(NodePeerState * s, PortUpVisitor * v);
  int RcvLoadingDone(NodePeerState * s,  );
  int RcvDSMismatch(NodePeerState * s,  );
  int RcvBadPTSEReq(NodePeerState * s,  );
  int RcvDropPort(NodePeerState * s,  );
  int RcvDropPortLast(NodePeerState * s,  );
};

// In this state, this node has received all PTSEs known to be
// available from the neighboring peer.  Links to the neighboring
// peer can now be advertised in PTSEs.
class NodePeerFull : public InternalNodePeerState {
protected:

  NodePeerFull(void);
  virtual ~NodePeerFull( );

  int RcvAddPort(NodePeerState * s, PortUpVisitor * v);
  int RcvDSMismatch(NodePeerState * s,  );
  int RcvBadPTSEReq(NodePeerState * s,  );
  int RcvDropPort(NodePeerState * s,  );
  int RcvDropPortLast(NodePeerState * s,  );
};

#endif // __NodePeer_H__
