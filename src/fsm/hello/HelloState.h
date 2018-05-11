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
#ifndef __HELLO_STATE_H__
#define __HELLO_STATE_H__

#ifndef LINT
static char const _HelloState_h_rcsid_[] =
"$Id: HelloState.h,v 1.99 1999/02/10 19:02:41 mountcas Exp $";
#endif

#include <iostream.h>
#include <common/cprototypes.h>
#include <FW/actors/State.h> 
#include <DS/containers/list.h>

class NodeID;
class PeerID;
class Addr;
class ig_nodal_hierarchy_list;
class ig_resrc_avail_info;
class HelloPkt;
class HelloTimer;
class InactivityTimer;
class InternalHelloState;
class HelloVisitor;
class LinkUpVisitor;

class HelloState : public State {
  friend class InternalHelloState;
  friend class StateLinkDown;
  friend class StateAttempt;
  friend class State1WayInside;
  friend class State2WayInside;
  friend class State1WayOutside;
  friend class State2WayOutside;
  friend class StateCommon;
public:

  static const int TimerInterval    = 15;
  static const int InactivityFactor = 5;
  static const int NewestVersion    = 1;
  static const int OldestVersion    = 1;

  HelloState( u_char nid[22], u_int pid,
	      u_short cver = 1, u_short nver = 1, u_short over = 1,
	      int vpi = 0, int vci = 18);
  HelloState( const NodeID * nid, u_int pid,
	      u_short cver=1,u_short nver=1, u_short over=1,
	      int vpi = 0, int vci = 18);
  virtual ~HelloState();

  virtual State * Handle(Visitor * v);
  virtual void    Interrupt(SimEvent * ev);

  void Dump(void);
  void Dump(ostream & os);

  // timers
  void SetHelloTimer(void);
  void SetInactivityTimer(void);
  void StopHelloTimer(void);
  void StopInactivityTimer(void);

  const char * GetName(void);

  const int      GetVP(void) const;
  const int      GetVC(void) const;
  const u_short  GetVersion(void) const; // current version
  const u_short  GetNewVersion(void) const;
  const NodeID * GetRemoteNode(void) const;
  const NodeID * GetLocalNode(void) const;
  const int      GetRemotePort(void) const;
  const int      GetLocalPort(void) const;
  const PeerID * GetRemotePeer(void) const;
  const PeerID * GetLocalPeer(void) const;
  const PeerID * GetCommonPeer(void) const;
  const NodeID * GetUpNode(void) const;
  const Addr   * GetUpNodeATM(void) const;
  const u_int    GetRcvNHLSeqNum(void) const;
  const int    GetConfLinkAggToken(void) const;
  const int    GetDerLinkAggToken(void) const;
  const int    GetRemLinkAggToken(void) const;
  const u_int    GetRcvULIASeqNum(void) const;

  void  SetConfLinkAggToken(int a);
  void  SetDerLinkAggToken(int a);
  void  SetRemLinkAggToken(int a);

  const ig_nodal_hierarchy_list     * GetNHL(const NodeID * n) const;
  const list<ig_resrc_avail_info *> * GetTheRAIGs(void) const;

  void SetRcvNHLSeqNum(u_int i);
  void SetRcvULIASeqNum(u_int i);
  void SetTheRAIGs(list<ig_resrc_avail_info *> * r);
  void SetRemotePort(int port);
  void SetRemoteNode(NodeID * n);
  void SetRemotePeer(PeerID * p);
  void SetCommonPeer(PeerID * p);
  void SetUpNode(NodeID * n);
  void SetUpNodeATM(Addr * a);
  void SetVersion(u_short v);
  
  bool PortIsUp(void) const;
  void PortIsUp(bool);

  void PassThruVis(Visitor * v);
  void PassVisToA(Visitor * v);
  void PassVisToB(Visitor * v);
  Visitor::which_side VisFrom(Visitor * v);

  void SendHello(void);
  void SendHelloWithNHL(void);

  void Clear(void);
  void ChangeState(InternalHelloState * ns);

  InternalHelloState * GetCS(void) const;

private:

  // Do not define the copy ctor (this also prevents the compiler from doing it for us)
  HelloState(const HelloState & rhs);

  bool              _PortIsUP;
  int               _vpi;
  int               _vci;
  int               _vpvc;
  int               _seqn;
  u_short           _cver;
  u_short           _nver;
  u_short           _over;
  Addr            * _LocAddr;
  HelloPkt        * _hp;

  // local Info
  PeerID          * _LocPeerGroupID;
  NodeID          * _LocNodeID;
  u_int             _LocPortID;
  list<ig_resrc_avail_info *> * _theRAIGs;	// Outgoing RAIGs for this port

  // Remote Info
  PeerID          * _RemPeerGroupID;
  PeerID          * _RemChildPeerGroupID;
  NodeID          * _RemNodeID;
  u_int             _RemPortID;

  // timers
  int               _HelloInterval;
  u_short           _InactivityFactor;
  HelloTimer      * _HelloTimer;
  InactivityTimer * _InactivityTimer;

  int               _Hcnt;
  int               _Icnt;

  // outside peer group links
  PeerID          * _CommonPeerGroupID;
  NodeID          * _UpNodeID;

  Addr            * _UpNodeATMaddr;
  u_int             _TrxULIASeqNum;
  u_int             _RcvULIASeqNum;
  u_int             _RcvNHLSeqNum;

  // Derived Aggregation Token (see page 133 of the spec)
  int             _DerLinkAggToken;
  // Configured Aggregation Token (obtained from the LinkUpVisitor).
  int             _ConfLinkAggToken;
  // Remote Aggregation Token (received in a Hello Pkt from the remote node).
  int             _RemLinkAggToken;

  static const VisitorType * _hello_type;
  static const VisitorType * _npflood_type;
  static const VisitorType * _linkup_type;
  static const VisitorType * _linkdown_type;
  static const VisitorType * _npstate_type;
  static const VisitorType * _vpvc_type;

  InternalHelloState * _cs;

  static InternalHelloState * _StateLinkDown;
  static InternalHelloState * _StateAttempt;
  static InternalHelloState * _StateOneWayInside;
  static InternalHelloState * _StateTwoWayInside;
  static InternalHelloState * _StateOneWayOutside;
  static InternalHelloState * _StateTwoWayOutside;
  static InternalHelloState * _StateCommon;
  static int                  _ref_count;

  void AllocateStates(void);
  void DeallocateStates(void);
};

// -----------------------------------------------------------------------
class InternalHelloState {
public:

  enum HelloTransitions {
    Invalid               = -1,
    passthru              =  0,
    LinkIsUp              =  1,
    WayInside1            =  2,
    WayInside2            =  3,
    WayOutside1           =  4,
    WayOutside2           =  5,
    CommHier              =  6,
    HelloMisMatch         =  7,
    HierMisMatch          =  8,
    HelloTimerExpiry      =  9,
    InactivityTimerExpiry = 10,
    LinkIsDown            = 12
  };

  enum NodeType {
    PhysicalNode         = 0,
    LogicalNode          = 1
  };

  enum HelloMask {
    DHM = 0
  };

  enum HelloError {
    fsm_ok    =  0,
    fsm_error = -1
  };

  InternalHelloState(void);
  virtual ~InternalHelloState(void);

  virtual const char * CurrentStateName(void) const = 0;

  void Handle(HelloState * s, Visitor * v);

  HelloTransitions GetEventType(HelloState * s, Visitor * v) const;

  const char * const GetTN(HelloTransitions code); // gets transition name
  
  void        do_Hp(HelloState * s, Visitor * v, int code);

  const char * OwnerName(HelloState * s) const;

  virtual int LinkUp(HelloState *, Visitor * v);
  virtual int RcvWayInside1(HelloState *, HelloVisitor * v);
  virtual int RcvWayInside2(HelloState *, HelloVisitor * v);
  virtual int RcvWayOutside1(HelloState *, HelloVisitor * v);
  virtual int RcvWayOutside2(HelloState *, HelloVisitor * v);
  virtual int RcvCommHier(HelloState *, HelloVisitor * v);
  virtual int RcvHelloMisMatch(HelloState *, HelloVisitor * v);
  virtual int RcvHierMisMatch(HelloState *, HelloVisitor * v);
  virtual int LinkDown(HelloState *, HelloVisitor * v);
  virtual int ExpHelloTimer(HelloState *);
  virtual int ExpInactivityTimer(HelloState *);

  void PassTHRU(HelloState * s, Visitor * v);
  void PassVisitorTOB(HelloState * s, Visitor * v);
  void PassVisitorTOA(HelloState * s, Visitor * v);
};

class StateLinkDown : public InternalHelloState {
public:

  StateLinkDown(void);
  virtual ~StateLinkDown();

  int LinkUp(HelloState *, Visitor *v);
  int LinkDown(HelloState *, HelloVisitor *v);
  int RcvWayInside1(HelloState *, HelloVisitor *v);
  int RcvWayOutside1(HelloState *, HelloVisitor *v);
  int RcvCommHier(HelloState *, HelloVisitor *v);
  int RcvHelloMisMatch(HelloState *, HelloVisitor *v);
  int ExpHelloTimer(HelloState *);
  int ExpInactivityTimer(HelloState *);

  virtual const char * CurrentStateName(void) const;
};

class StateAttempt : public InternalHelloState {
public:

  StateAttempt(void);
  virtual ~StateAttempt();

  int LinkUp(HelloState *, Visitor *v);
  int RcvWayInside1(HelloState *, HelloVisitor *v);
  int RcvWayInside2(HelloState *, HelloVisitor *v);
  int RcvWayOutside1(HelloState *, HelloVisitor *v);
  int RcvWayOutside2(HelloState *, HelloVisitor *v);
  int RcvCommHier(HelloState *, HelloVisitor *v);
  int RcvHelloMisMatch(HelloState *, HelloVisitor *v);
  int ExpHelloTimer(HelloState *);
  int ExpInactivityTimer(HelloState *);
  int RcvHierMisMatch(HelloState *, HelloVisitor *v);
  int LinkDown(HelloState *, HelloVisitor *v);

  virtual const char * CurrentStateName(void) const;
};

class State1WayInside : public InternalHelloState {
public:

  State1WayInside(void);
  virtual ~State1WayInside();

  int LinkUp(HelloState *, Visitor *v);
  int RcvWayInside1(HelloState *, HelloVisitor *v);
  int RcvWayInside2(HelloState *, HelloVisitor *v);
  int RcvWayOutside1(HelloState *, HelloVisitor *v);
  int RcvWayOutside2(HelloState *, HelloVisitor *v);
  int RcvCommHier(HelloState *, HelloVisitor *v);
  int RcvHelloMisMatch(HelloState *, HelloVisitor *v);
  int RcvHierMisMatch(HelloState *, HelloVisitor *v);
  int ExpHelloTimer(HelloState *);
  int ExpInactivityTimer(HelloState *);
  int LinkDown(HelloState *, HelloVisitor *v);

  virtual const char * CurrentStateName(void) const;
};


class State2WayInside : public InternalHelloState {
public:

  State2WayInside(void);
  virtual ~State2WayInside();

  int LinkUp(HelloState *, Visitor *v);
  int RcvWayInside1(HelloState *, HelloVisitor *v);
  int RcvWayInside2(HelloState *, HelloVisitor *v);
  int RcvWayOutside1(HelloState *, HelloVisitor *v);
  int RcvWayOutside2(HelloState *, HelloVisitor *v);
  int RcvCommHier(HelloState *, HelloVisitor *v);
  int RcvHelloMisMatch(HelloState *, HelloVisitor *v);
  int RcvHierMisMatch(HelloState *, HelloVisitor *v);
  int ExpHelloTimer(HelloState *);
  int ExpInactivityTimer(HelloState *);
  int LinkDown(HelloState *, HelloVisitor *v);

  virtual const char * CurrentStateName(void) const;
};


class State1WayOutside : public InternalHelloState {
public:

  State1WayOutside(void);
  virtual ~State1WayOutside();

  int LinkUp(HelloState *, Visitor *v);
  int RcvWayInside1(HelloState *, HelloVisitor *v);
  int RcvWayInside2(HelloState *, HelloVisitor *v);
  int RcvWayOutside1(HelloState *, HelloVisitor *v);
  int RcvWayOutside2(HelloState *, HelloVisitor *v);
  int RcvCommHier(HelloState *, HelloVisitor *v);
  int RcvHelloMisMatch(HelloState *, HelloVisitor *v);
  int RcvHierMisMatch(HelloState *, HelloVisitor *v);
  int ExpHelloTimer(HelloState *);
  int ExpInactivityTimer(HelloState *);
  int LinkDown(HelloState *, HelloVisitor *v);

  virtual const char * CurrentStateName(void) const;
};

class State2WayOutside : public InternalHelloState {
public:

  State2WayOutside(void);
  virtual ~State2WayOutside();

  int LinkUp(HelloState *, Visitor *v); 
  int RcvWayInside1(HelloState *, HelloVisitor *v);
  int RcvWayInside2(HelloState *, HelloVisitor *v);
  int RcvWayOutside1(HelloState *, HelloVisitor *v);
  int RcvWayOutside2(HelloState *, HelloVisitor *v);
  int RcvCommHier(HelloState *, HelloVisitor *v);
  int RcvHelloMisMatch(HelloState *, HelloVisitor *v);
  int RcvHierMisMatch(HelloState *, HelloVisitor *v);
  int ExpHelloTimer(HelloState *);
  int ExpInactivityTimer(HelloState *);
  int LinkDown(HelloState *, HelloVisitor *v);

  virtual const char * CurrentStateName(void) const;
};

class StateCommon : public InternalHelloState {
public:

  StateCommon(void);
  virtual ~StateCommon();

  int LinkUp(HelloState *, Visitor *v); 
  int RcvWayInside1(HelloState *, HelloVisitor *v);
  int RcvWayInside2(HelloState *, HelloVisitor *v);
  int RcvWayOutside1(HelloState *, HelloVisitor *v);
  int RcvWayOutside2(HelloState *, HelloVisitor *v);
  int RcvCommHier(HelloState *, HelloVisitor *v);
  int RcvHelloMisMatch(HelloState *, HelloVisitor *v);
  int RcvHierMisMatch(HelloState *, HelloVisitor *v);
  int ExpHelloTimer(HelloState *);
  int ExpInactivityTimer(HelloState *);
  int LinkDown(HelloState *, HelloVisitor *v);

  virtual const char * CurrentStateName(void) const;
};

#endif // #define __HELLO_STATE_H__
