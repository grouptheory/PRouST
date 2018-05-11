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
#ifndef __NODEPEERSTATE_H__
#define __NODEPEERSTATE_H__

#ifndef LINT
static char const _NpeerState_h_rcsid_[] =
"$Id: NodePeerState.h,v 1.54 1999/02/10 18:43:00 mountcas Exp $";
#endif

#include <iostream.h>
#include <sys/types.h>

#include <DS/containers/dictionary.h>
#include <DS/containers/list.h>
#include <FW/actors/State.h>

class PTSPPkt;
class PTSPRxTimer;
class NodeID;
class SimEvent;
class Visitor;
class PTSPVisitor;
class AckVisitor;
class AckContainer;
class AckTimer;
class DSVisitor;
class DSRxTimer;
class ReqVisitor;
class ReqRxTimer;
class PortUpVisitor;
class PortDownVisitor;
class SVCCPortInfoVisitor;
class HorLinkVisitor;
class NPFloodVisitor;
class DatabaseSumPkt;
class PTSEReqPkt;
class PTSEAckPkt;
class ig_ptse;
class ig_nodal_ptse_ack;
class RXEntry;

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
public:

  /// Constructor
  NodePeerState(u_char * nid, u_char * rnid = 0);
  /// Constructor
  NodePeerState(const NodeID * LocNodeID, const NodeID * RemNodeID, 
		int vp = -1, int vc = -1, u_long cref = 0xffffffff);
  
  /**@name Enumeration of possible actions to take based on Visitors received.
   */
  //@{
  enum ThisVisitor {
    Invalid,
    DSPkt,
    ReqPkt,
    PtspPkt,
    AckPkt,
    AddPort,
    DropPort,
    Flood,
    SvccPortInfo,
    passthru,
  };
  //@}

  /**@name Type of NodePeerState, either Physical or Logical.
   */
  //@{
  enum NodeType {
    PhysicalNode,
    LogicalNode
  };
  //@}

  /**@name Error return codes 
   */
  //@{
  enum NodePeerError {
    fsm_ok    = 0,
    fsm_error = 1
  };
  //@}

  /**@name NodePeerFsm States
   */
  //@{
  enum NodePeerStates {
    NpDown,
    Negotiating,
    Exchanging,
    Loading,
    Full
  };
  //@}

  /**@name Mode of transmitting database summary packets.
   */
  //@{
  enum Mode{
    Unknown = 0,
    Master,
    Slave
  };
  //@}

  /// Point of entry to a pkt in NP FSM
  virtual State * Handle(Visitor *v);
  /// Not used right now
  virtual void    Interrupt(SimEvent *e);
  /// Returns type of visitor  received
  virtual ThisVisitor GetVisitorType(Visitor *v);
  /// Process Database Summary packets
  virtual int RcvDSPkt(DSVisitor *v);
  /// Process ptse request packets.
  virtual int RcvPTSEReqPkt(ReqVisitor *v);
  /// Process ptsp packets
  virtual int RcvPTSPPkt(PTSPVisitor *v);
  /// Process ptsp acknowledgement packets
  virtual int RcvPTSEAckPkt(AckVisitor *v);
  /// Act upon when LinkUp is reported
  virtual int RcvAddPort(PortUpVisitor *v);
  /// Act upon when  LinkDown is reported
  virtual int RcvDropPort(PortDownVisitor *v);
  /// Process a ptsp packet to be flooded 
  virtual int RcvFloodPkt(NPFloodVisitor *v);
  ///  Logical Node's port status information
  virtual int RcvSvccPortInfo(SVCCPortInfoVisitor *v);
  
  /// Do what's said in Table 5-12 page 90 PNNI specs
  void do_ds(Visitor *v, int code);
  /// Process Database summary packets like a Master
  void MasterProcessDS(DatabaseSumPkt *rDpkt);
  /// Process Database summary packets like a Slave
  void SlaveProcessDS(DatabaseSumPkt *rDpkt);

  /// Act when Database Summary packet response time expired
  virtual void ExpireDSTimer(void);
  /// Act when ptsp request packet timer expired
  virtual void ExpireReqTimer(void);
  /// Act when you the PTSP timer has expired.
  virtual void ExpirePTSPTimer(PTSPPkt *pkt, PTSPRxTimer *timer) ;
  /// Act when the Acknowledgement timer expires before receiving an ack from your peer.
  virtual void ExpireAckTimer(void);

  /// Send a Database Summary Packet to the peer
  virtual void SendDSPkt(void);
  /// Send a Request packet to the peer
  virtual void SendReqPkt(void);
  //      a ptsp packet to the peer
  virtual void SendPTSPPkt(PTSPPkt * pkt);
  //      an Acknowledgement packet to the peer
  virtual void SendAckPkt(void);

  // Utility functions
  // Tell me if the ptsp requested was a one in our database
  virtual bool IsBadRequest(const u_char *onid, ig_ptse * rPtse);
  // Do what's said in 5.8.3.3 under (3) on page 108, specs 
  virtual void CheckPTSPRxlistForCase3(ig_ptse * rPtse, ig_ptse * lPtse, PTSPPkt * sPtsp);
  // Do what's said in 5.8.3.3 under (4) on page 108, specs
  virtual void CheckPTSPRxlistForCase4(ig_ptse * rPtse, ig_ptse * lPtse, PTSPPkt * sPtsp);
  // Delete an entry of ptsp in _death_row in NP fsm datastructure
  virtual void DelInDeathList(const u_char * onid, int pid);
  //  Delete an entry of ptsp in retransmission list
  virtual void DelPTSEInPTSPRxList(const u_char * onid,  int pid);
  // Delete an entry in Ack list
  virtual void DelInAckList(const u_char * onid, int pid);
  // Stop all ptse retransmission timers and clear the ptse's
  virtual void StopAllPTSETimers(void);
  // Remove ptse's which have been acknowledged
  virtual void RemoveAckedRxPtses(AckContainer * rAcont, ig_nodal_ptse_ack * rAhr);
  // Clear the ptse list which has expired ptse's
  virtual void ClearDeathRow(void);

  // A few repetative one's
  // Set the Database Summary packet sequence number
  inline void SetDSSequenceNum(u_int num); 
  // Set the summary pkt ptr to be sent
  inline void SetDSPkt(DatabaseSumPkt *pkt);
  // Set request pkt ptr to be sent
  inline void SetReqPkt(PTSEReqPkt * lRpkt);

protected:

  void ChangeState( NodePeerState::NodePeerStates newS );
  const char * StateName(void) const;

  // The NodePeerFsm DataStructure.

  // Provides the state of the FSM.
  NodePeerStates   _State;
  // Is it a physical or logical node?
  NodeType         _NodeType;
  // Level of hierarchy
  int              _level;
  // This node's NodeID's pointer
  NodeID          *_LocNodeID;
  // Peer node's NodeID's pointer
  NodeID          *_RemNodeID;
  // An array storing all the ports connected to peer. 
  //  dictionary<int, u_int> _PortID;
  list<u_int> _PortID;
  // Index to next empty place in _PortID array to store a port
  //  u_short          _NextPort;

  // Mode of  operation of this node when Database
  // Summary packet exchange
  Mode             _Mode;
  // The Current Database Summarypkt sequence number
  int              _DSSequenceNum;
  // list of PTSP pkts which have been transmitted to peer
  // Will be retransmitted if pkt is not acknowledged
  list<RXEntry *>  _PTSPRxList;
  // A list of pkt's which need flused from the database
  list<PTSPPkt *>  _death_row;
  // The retransmission interval for the ptsp pkts sent by this node
  double           _PTSPRxInterval;
  // Acknowledgement packt for the ptsp's received
  PTSEAckPkt     * _PTSEAckpkt;
  // Ack Interval
  double           _PTSEAckInterval;
  // Ack timer whose expiry initiates sending Ack pkt.
  AckTimer       * _AckTimer;
  // A ptse request pkt to be sent to peer
  PTSEReqPkt     * _PTSEReqRxpkt;
  // Time interval to resend the request packets
  double           _PTSEReqRxInterval;
  // Request Timer whose expiry initiates sending Request packet
  ReqRxTimer     * _ReqRxTimer;
  // The databsase summary pkt sent which needs response
  DatabaseSumPkt * _DSPktOutStanding;
  // Time interval to resend the Database Summary pkts
  u_short          _DSRxInterval;
  // Timer when expires, initiates to resend the Database Summary packet
  DSRxTimer      * _DSRxTimer;
  // A flag to see if Ack packet needs to be sent 
  // immediately for a ptsp received
  bool             _SendAck;

  int              _vci;
  int              _vpi;
  u_long           _cref;

  virtual ~NodePeerState();

  static const VisitorType * _port_up_type;
  static const VisitorType * _port_down_type;
  static const VisitorType * _ds_type;
  static const VisitorType * _req_type;
  static const VisitorType * _ptsp_type;
  static const VisitorType * _ack_type;
  static const VisitorType * _npflood_type;
  static const VisitorType * _link_up_type;
  static const VisitorType * _svcc_type;
  static const VisitorType * _hlink_type;
  static const VisitorType * _fast_uni_type;
};

#endif // __NPEERSTATE_H__
