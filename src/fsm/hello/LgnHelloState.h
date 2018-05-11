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
#ifndef __LGNHELLOSTATE_H__
#define __LGNHELLOSTATE_H__

#ifndef LINT
static char const _LgnHelloState_h_rcsid_[] =
"$Id: LgnHelloState.h,v 1.4 1999/02/16 16:06:02 mountcas Exp $";
#endif

#include <FW/actors/State.h>
#include <DS/containers/list.h>

class InternalLgnHelloState;
class InducedUplink;
class HorLinkVisitor;
class NodeID;
class Visitor;
class SimEvent;
class LgnInactivityTimer;

/** LgnHelloState

    The protocol for determining the state of horizontal links between
    Logical Group Nodes is also based upon the Hello protocol.  The
    states of all horizontal links to an LGN neighbor are determined
    from the information in a single LGN Horizontal Link Extension
    information group included in the Hellos send over the SVCC-based
    RCC.  The LGNHorizontal Link Extension information group is
    present in all Hellos transmitted to the neighboring peer LGN.
    Each time a Hello is sent to the neighboring peer, the LGN
    Horizontal Link Extension information group shall contain an entry
    for each horizontal link to the neighboring peer node in any state
    other than Down.  For each horizontal link the Aggregation Token,
    Local Port ID, and Remote Port ID are included.  Each distinct
    aggregation token value represents a distinct horizontal link with
    its own independent state machine.  The information group is
    present in all Hellos transmitted.

    On receipt of Hellos received from the neighboring peer LGN, the
    LGN Horizontal Link Extension information group is only processed
    if the SVCC-based RCC Hello State Machine is in TwoWay Inside and
    the corresponding neighboring peer state machine is in Full state.  

    See page 78 of the PNNI 1.0 specification.
 */
class LgnHelloState : public State {
  friend class LgnHelloFSMCreator;
public:

  /**@name Enumerations of the 4 different states.
   */
  //@{
  enum StateNames {
    /// The Initial State.
    StateDown = 1,
    /// The Attempt State.
    StateAttempt,
    /// OneWay Inside, we know about our peer, but we do not know if he knows of us.
    StateOneWayInside,
    /// TwoWay Inside, we know about our peer and he knows about us.
    StateTwoWayInside
  };
  //@}

  /// Constructor - used by the LgnHello Creator
  LgnHelloState(int aggrToken, 
		const NodeID * localNode, 
		const NodeID * remoteNode = 0);

  /// Constructor - outdated, left in for compatibility
  LgnHelloState(int aggrToken, 
		const NodeID * localNode, 
		int localPort,
		const NodeID * remoteNode = 0);

  /// Destructor
  virtual ~LgnHelloState();

  /// Main entry point for dealing with all Visitors.
  State * Handle(Visitor * v);
  /// Used to obtain the logical port from the Aggregator.
  void    Interrupt(SimEvent * e);
  /// Used to facilitate the changing of state
  void ChangeState(StateNames n);
  /// 
  void DoLgnHelloProtocol(int num, HorLinkVisitor * v = 0);
  /// Helper method for InternalHelloState to allow Visitors to PassThrough
  void PassThroughVisitor(Visitor * v);
  /// Helper method for InternalHelloState to allow Visitors to be sent back the way they came.
  void BounceBackVisitor(Visitor * v);
  /// Helper method for InternalHelloState so it can print the name of the Conduit.
  const char * const GetName(void);
  /// Returns the Local Logical Port ID
  const int GetLocPortID(void) const;
  /// Returns the Remote Logical Port ID (if known).
  const int GetRemPortID(void) const;
  /// Returns the Aggregation Token
  const int GetAggrToken(void) const;
  /// Returns the Local Logical Node ID
  const NodeID * GetLocNodeID(void) const;
  /// Returns the Remote Logical Node ID (if known).
  const NodeID * GetRemNodeID(void) const;
  /// Sets the Local Logical Port ID (used when receiving the Logical Port from Aggregator).
  void      SetLocPortID(int p);
  /// Sets the Remote Logical Port ID (used when receiving the Logical Port in the Hello Pkt).
  void      SetRemPortID(int p);
  /// Sets the Aggregation Token (used to rebind the LgnHello when it is first instantiated).
  void      SetAggrToken(int t);
  /// Sets the Local Logical Node ID (presented for completeness).
  void      SetLocNodeID(const NodeID * n);
  /// Sets the Remote Logical Node ID (used when the RCC Hello discovers the remote peer).
  void      SetRemNodeID(const NodeID * n);
  /// Returns the current state of this FSM.
  InternalLgnHelloState * GetCS(void) const;
  /// Returns an enumeration of which side the Visitor arrived on.
  enum Visitor::which_side WhichSide(Visitor * v);
  /// Sends a HorLinkVisitor of type HLinkUp to the Aggregator.
  void SendLinkUp(void);
  /// Sends a HorLinkVisitor of type HLinkDown to the Aggregator.
  void SendLinkDown(void);
  /// Sends a HorLinkVisitor of type AddInducedLink to Aggregator.
  void SendAddInduced(void);
  /// Sends a HorLinkVisitor of type DropInducedLink to Aggregator.
  void SendDropInduced(void);
  /// Cancels the LgnHorizontalLinkInactivityTimer
  void CancelInactivityTimer(void);
  /// Starts the LgnHorizontalLinkInactivityTimer
  void RegisterInactivityTimer(void);

private:

  /// Do not define, declared to prevent the compiler from generating them.
  LgnHelloState(const LgnHelloState & rhs);
  /// Do not define, declared to prevent the compiler from generating them.
  LgnHelloState(void);

  /**@name Private helper methods
   */
  //@{
    /// Allocates the Internal Hello States.
  void AllocateStates(void);
    /// Deallocates the Internal Hello States.
  void FreeStates(void);
  //@}

  void AddInducingLink(const NodeID * border, int port);  
  void RemInducingLink(const NodeID * border, int port);
  void ClearInducingLinks(void);

  void SetCreator(LgnHelloFSMCreator * c = 0);
  LgnHelloFSMCreator * _creator;
  
  /// Local Logical NodeID
  NodeID * _LocNodeID;
  /// Remote Logical NodeID
  NodeID * _RemNodeID;
  /// Aggregation Token
  int      _AggToken;
  /// Local Logical Port ID
  int      _LocPortID;
  /// Remote Logical Port ID
  int      _RemPortID;
  /// Current State
  InternalLgnHelloState * _cs;
  /// List of Inducing Uplinks
  list<InducedUplink *> _induced_uplinks;
  /// Lgn Inactivity Timer
  LgnInactivityTimer * _LgnInactivityTimer;

  /// Period for the Inactivity Timer.
  static const u_int    _LgnLinkInactivityTime = 120;
  /// Reference count, to identify when all instance of this class drop to zero.
  static int                     _ref_count;
  /**@name Pointers to the four different states.
   */
  //@{
    /// Down
  static InternalLgnHelloState * _Down;
    /// Attempt
  static InternalLgnHelloState * _Attempt;
    /// OneWay
  static InternalLgnHelloState * _OneWay;
    /// TwoWay
  static InternalLgnHelloState * _TwoWay;
  //@}

  static VisitorType * _fastuni_vistype;
};

class InternalLgnHelloState {
public:

  enum EventTypes {
    Invalid = 0,
    PassThrough,
    AddInducingLink,
    OneWayReceived,
    TwoWayReceived,
    HelloMismatch,
    HLinkInactivityTimerExpired,
    BadNeighbor, 
    DropInducingLink,
    DropLastInducingLink,
    PassBack
  };

  InternalLgnHelloState(void);
  virtual ~InternalLgnHelloState();

  void Handle(LgnHelloState * lhs, Visitor * v);

  EventTypes GetEventType(LgnHelloState * l, Visitor * v);

  virtual void RcvAddInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvOneWayInside(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvTwoWayInside(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvHelloMismatch(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvBadNeighbor(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvDropInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvDropLastInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvHLinkInactivityExpiry(LgnHelloState * l);

  virtual const char * const CurrentStateName(void) const = 0;

protected:

  static VisitorType * _horlink_vistype;
  static VisitorType * _fastuni_vistype;
  static VisitorType * _npstate_vistype;
};

class LgnDown : public InternalLgnHelloState {
public:

  LgnDown(void);
  virtual ~LgnDown();

  virtual void RcvAddInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvOneWayInside(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvTwoWayInside(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvHelloMismatch(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvBadNeighbor(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvHLinkInactivityExpiry(LgnHelloState * l);

  virtual const char * const CurrentStateName(void) const;
};

class LgnAttempt : public InternalLgnHelloState {
public:

  LgnAttempt(void);
  virtual ~LgnAttempt();

  virtual void RcvAddInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvOneWayInside(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvTwoWayInside(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvHelloMismatch(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvBadNeighbor(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvDropInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvDropLastInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvHLinkInactivityExpiry(LgnHelloState * l);

  virtual const char * const CurrentStateName(void) const;
};

class LgnOneWayInside : public InternalLgnHelloState {
public:

  LgnOneWayInside(void);
  virtual ~LgnOneWayInside();

  virtual void RcvAddInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvOneWayInside(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvTwoWayInside(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvHelloMismatch(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvBadNeighbor(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvDropInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvDropLastInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvHLinkInactivityExpiry(LgnHelloState * l);

  virtual const char * const CurrentStateName(void) const;
};

class LgnTwoWayInside : public InternalLgnHelloState {
public:
  
  LgnTwoWayInside(void);
  virtual ~LgnTwoWayInside();

  virtual void RcvAddInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvOneWayInside(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvTwoWayInside(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvHelloMismatch(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvBadNeighbor(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvDropInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvDropLastInducingLink(LgnHelloState * l, HorLinkVisitor * v);
  virtual void RcvHLinkInactivityExpiry(LgnHelloState * l);

  virtual const char * const CurrentStateName(void) const;
};

// ------------------------------------------------------
class InducedUplink {
  friend int compare(InducedUplink *const &, InducedUplink *const &);
public:

  InducedUplink(const NodeID * borderNode, int portID);
  InducedUplink(const InducedUplink & rhs);
  ~InducedUplink();

  const NodeID * GetBorderNode(void) const;
  const int      GetPortID(void) const;

private:

  NodeID * _border;
  int      _portID;
};

#endif // __LGNHELLOSTATE_H__
