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
#ifndef __ELECTIONSTATE_H__
#define __ELECTIONSTATE_H__

#ifndef LINT
static char const _ElectionState_h_rcsid_[] =
"$Id: ElectionState.h,v 1.1.1.51 1999/02/10 18:57:31 mountcas Exp $";
#endif

#include <FW/actors/State.h>
#include <codec/pnni_ig/id.h>

#include <iostream.h>
#include <sys/types.h>

#include <DS/containers/list.h>
#include <DS/containers/sortseq.h>

#include <FW/basics/Visitor.h>

class ElectionVisitor;
class VPVCVisitor;
class NodeID;
class PeerID;
class Nodalinfo;
class NPFloodVisitor;

class LinkVisitor;
class OverrideUnanimityTimer;
class PGLInitTimer;
class ReelectionTimer;
class SearchPeerTimer;

class ig_nodal_info_group;

class InternalElectionState;

/**  ElectionState

     The PNNI hierarchy requires that a node be selected in each peer
     group to perform some functions of the LGN.  The node selected
     for this purpose is known ad the Peer Group Leader.  PReference
     for peer group leadership is established through configuration.
     This preference is indicated by the PGL priority advertised by
     each node.  The node ID is used as a tie breaker among nodes with
     equal PGL priorities.

     The PGL election algorithm is used to dynamically select an
     appropriate node to assume peer group leadership within a peer
     group or to replace an outgoing PGL.  Among all nodes to which a
     node has connectivity, it must vote for the one with the highest
     non-zero PGL priority subject to tie breaking using node IDs.
     The leadership priority is advertised by all nodes in the peer
     group in PTSEs.  If no node advertises a non-zero PGL priority,
     then no node is selected.  A node will consider a 2/3 majority
     vote sufficient for PGL election after it determines that a
     unanimous vote cannot be obtained within a sufficient time, so
     that errant implementation in a small number of nodes in the peer
     group are not likely to cause a hung election.  For a similar
     reason, all nodes in a peer group must participate in PGL
     election during normal operation.  However, a node that has the
     Non-transit for PGL Election flag set in its Nodal IG does not
     participate in PGL election, i.e., it does not run the PGL
     Election FSM and advertises zero for Leadership Priority and
     Preferred PGL.  Such nodes are also not considered by other nodes
     in the peer group when determining connectivity in the peer
     group, and the PGL priority and preferred PGL advertised by such
     overloaded nodes are ignored by all other nodes.
 */
class ElectionState : public State {
public:

  static const int ReElectionInterval = 15;
  static const int PGLInitTime        = 15;
  static const int OverrideDelay      = 30;

  /**@name Enumeration of possible states.
   */
  //@{
  enum StateID {
    Starting, 
    Awaiting, 
    AwaitingFull,
    InitialDelay,
    Calculating,
    OperNotPGL,
    OperPGL,
    AwaitUnanimity,
    HungElection,
    AwaitReelection
  };
  //@}

  /**@name Enumeration of timers.
   */
  //@{
  enum ElectionTimers {
    SearchPeerTIMER,
    OverrideUnanimityTIMER,
    ReelectionTIMER,
    PGLInitTIMER
  };
  //@}

  /**@name Enumeration of Election FSM type (Physical or Logical).
   */
  //@{
  enum NodeType {
    /// This is a physical level election.
    PhysicalNode = 0,  
    /// This is a logical level election.
    LogicalNode = 1
  };
  //@}

  /// Constructor
  ElectionState( const NodeID * nid, int priority = 0 );

  /// Destructor
  virtual ~ElectionState( );

  /// Main entry point for dealing with Visitors.
  State * Handle(Visitor * v);

  /// unused
  void Interrupt(SimEvent *ev);

  /// Returns the Current State of the Election FSM.
  InternalElectionState * GetCS(void) const;

  /// Allows the Election FSM to know of it's parent Conduit (so the Logical Node instance can be brought down).
  void      SetParent(const Conduit * c);

  /// Returns either Physical or Logical.
  NodeType  GetNodeType(void) const;

  const NodeID * GetLocalNID(void) const;
  const char   * GetName(void);

  void SetPreferredPGL(const NodeID * p);
  void SetOldPGL(const NodeID * p);

  const NodeID * GetPreferredPGL(void) const;
  const NodeID * GetPreferredPGL(const NodeID * nid) const;
  const NodeID * GetOldPGL(void) const;

  void RegisterTimer(ElectionState::ElectionTimers);
  void CancelTimer(ElectionState::ElectionTimers);

  void SendVisitor(Visitor * v, Visitor::which_side s = Visitor::OTHER);

  const Nodalinfo * GetNodalinfo(const NodeID * nid) const;

  const int GetPreferredPrio(void) const;
  const int GetOldPrio(void) const;
  const int GetLocalPrio(void) const;

  void      SetPreferredPrio(int p);
  void      SetOldPrio(int p);
  void      SetLocalPrio(int p);

  void ChangeState(ElectionState::StateID);
  void CalculateVotes(void);
  int  GetVotes(void) const;
  int  NumVoters(void) const;

  const sortseq<const NodeID *, Nodalinfo *> * GetVoters(void) const;
  const sortseq<const NodeID *, list<int> *> * GetPorts(void) const;

  void AddVoter(const NodeID * n, Nodalinfo * ni);

  void UpdatePorts(Visitor * v);
  bool ClearPorts(const NodeID * rnid, LinkVisitor * v);

  // Should be private but I don't want to make the entire InternalElectionState class a friend
  void DeleteParent(void);

private:

  NodeType                _NodeType;
  NodeID                * _LocNodeID;

  // According to the PGL datastrcuture  
  InternalElectionState * _cs;
  NodeID                * _PreferredPGL;
  NodeID                * _OldPGL;
  int                     _LocalPriority;
  int                     _OldPGLlp; // Required for OperPGL and Non Oper Pgl case
  int                     _PreferredPGLlp; // Priority expressed by PGL(highest)
  int                     _votecount;

  sortseq<const NodeID *, Nodalinfo *> _members;
  sortseq<const NodeID *, list<int> *> _ports;

  // Internal Use Only
  Conduit * _parent_expander;

  friend InternalElectionState * GetState(ElectionState::StateID sid);
  void   BuildStates(void);
  void   FreeStates(void);

  SearchPeerTimer        * _SearchPeerTimer;
  PGLInitTimer           * _PGLInitTimer;
  OverrideUnanimityTimer * _OverrideUnanimityTimer;
  ReelectionTimer        * _ReelectionTimer;

  char * _name;

  static int _ref_count;
  static InternalElectionState * _Starting;
  static InternalElectionState * _Awaiting;
  static InternalElectionState * _AwaitingFull;
  static InternalElectionState * _InitialDelay;
  static InternalElectionState * _Calculating;
  static InternalElectionState * _OperNotPGL;
  static InternalElectionState * _OperPGL;
  static InternalElectionState * _AwaitUnanimity;
  static InternalElectionState * _HungElection;
  static InternalElectionState * _AwaitReelection;

  static const VisitorType     * _link_type;
};


// Container
class Nodalinfo {
public:

  Nodalinfo(const NodeID * nid,  int priority, 
	    const NodeID * pref, int transit);
  ~Nodalinfo();

  int            GetPriority(void) const;
  void           SetPriority(int priority);
  void           SetElectionFlag(bool flag);
  void           SetElectionTransit(int flag);
  int            GetElectionTransit(void) const;
  bool           GetElectionFlag(void) const;
  void           SetPrefPGL(const NodeID * p);
  const NodeID * GetPrefPGL(void) const;
  // You may NOT set this
  const NodeID * GetNodeID(void) const;

protected:

  NodeID * _NodeID;
  int      _priority;
  NodeID * _PreferredPGL;
  int      _electiontransit;
  int      _ElectionFlag;
};

#endif //  __ELECTIONSTATE_H__
