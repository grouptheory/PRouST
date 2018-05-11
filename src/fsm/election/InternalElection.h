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
#ifndef __InternalElection_H__
#define __InternalElection_H__

#ifndef LINT
static char const _InternalElection_h_rcsid_[] =
"$Id: InternalElection.h,v 1.7 1999/01/11 22:25:41 mountcas Exp bilal $";
#endif

#include <fsm/election/ElectionState.h>

class Visitor;
class VisitorType;
class NodeID;
class Nodalinfo;
class LinkVisitor;
class VPVCVisitor;
class ig_nodal_info_group;
class ElectionState;
class ElectionVisitor;

class InternalElectionState {
public:

  enum ElectionTransitions {
    Invalid                = 0,
    _HelloFSMStarted       = 1,
    _PeerFound             = 2,
    _LostAllPeers          = 3,
    _SearchPeerTimerExpiry = 4,
    _DBReceived            = 5,
    _PGLInitTimerExpiry    = 6,
    _PreferredPGLNotMe     = 7,
    _Unanimity             = 8,
    _OverrideUnanimitySucc = 9,
    _OverrideUnanimityFail = 10,
    _TwoThirdReached       = 11,
    _PreferredPGLMe        = 12,
    _ChangePreferredPGL    = 13,
    _LoseConnToPGL         = 14,
    _GetConnToPGL          = 15,
    _ReelectiomTimerExpiry = 16,
    _NodalIGInserted       = 17,
    _HorzlinkInserted      = 18,
    PassThrough
  };

  enum InsertionFlags {
    Electiontransit,
    ElectionNontransit,
    HlinkUp,
    HlinkDown,
    all
  };

  InternalElectionState(void);
  virtual ~InternalElectionState();

  InternalElectionState * Handle(ElectionState * e, Visitor * v);

  virtual bool PeerFound(ElectionState * e, Visitor *& v);
  virtual bool LostAllPeers(ElectionState * e, Visitor * v);
  virtual bool DBReceived(ElectionState * e, Visitor *& v);
  virtual bool InsertNodeID(ElectionState * e, ElectionVisitor * v);
  virtual bool InsertHorzID(ElectionState * e, Visitor * v);

  // Timer callbacks
  virtual bool SearchPeerTimerExpiry(ElectionState * e);
  virtual bool ReelectionTimerExpiry(ElectionState * e);
  virtual bool PGLInitTimerExpiry(ElectionState * e);
  virtual bool OverrideUnanimityExpiry(ElectionState * e);

  virtual const char * StateName(void) const = 0;

protected:

  const char * OwnerName(ElectionState * e) const;
  bool  PreferMyself(ElectionState * e) const;
  bool  OldIsPref(ElectionState * e) const;
  int   GetPrefPrio(ElectionState * e) const;
  int   GetPrefPGLPrio(ElectionState * e) const; // priority by pgl.
  int   GetOldPGLPrio(ElectionState * e) const; 
  int   GetVotes(ElectionState * e) const;
  const NodeID * GetLocalNode(ElectionState * e) const;
  const NodeID * GetPreferredPGL(ElectionState * e) const;
  const NodeID * GetOldPGL(ElectionState * e) const;
  const Nodalinfo * GetNodalInfo(ElectionState * e, const NodeID *  ) const; 
  
  void RegisterTimer(ElectionState * e, ElectionState::ElectionTimers t);
  void CancelTimer(ElectionState * e, ElectionState::ElectionTimers t);
  void PrefPrioToOld(ElectionState * e);
  void ChangeState(ElectionState * e, ElectionState::StateID s);
  void SetPrefPrio(ElectionState * e, int prio);
  void SetPrefPGLPrio(ElectionState * e, int prio);
  void SetPreferredPGL(ElectionState * e, const NodeID * pgl);

  virtual enum ElectionTransitions GetEventType(ElectionState * e, Visitor * v);

  void do_PGLE(ElectionState * e, Visitor * v, int code, 
	       NodeID * pref = 0, int prefPrio = 0);

  bool Unanimity(ElectionState * e);
  void Election(ElectionState * e);

  virtual bool HelloFSMStarted(ElectionState *e, Visitor *v);
  virtual bool OverrideUnanimitySucc(ElectionState *e);
  virtual bool OverrideUnanimityFail(ElectionState *e);
  virtual bool LoseConnToPGL(ElectionState *e);
  virtual bool GetConnToPGL(ElectionState *e);
  virtual bool TwoThirdReached(ElectionState * e);
  virtual bool ChangePreferredPGL(ElectionState * e);

  // Utility Functions
  NodeID * EvalPreferredPGL(ElectionState * e, int & prio);
  void UpdatePorts(ElectionState * e, LinkVisitor * v);
  void UpdatePorts(ElectionState * e, VPVCVisitor * v);
  bool AllPeersLost(ElectionState * e, LinkVisitor *v);
  bool DetermineConnToNode(ElectionState * e, const NodeID * n, InsertionFlags flag);
  void DetermineConnToNode(ElectionState * e, InsertionFlags flag);
  bool RemoveEntry(ElectionState *, NodeID *);
  void StoreEntry(ElectionState *, NodeID *, ig_nodal_info_group *);
  void SetEntry(ElectionState * e, const NodeID * nid);
  Nodalinfo * GetEntry(ElectionState * e, const NodeID * nid) const;

  void PreferredPGLNotMe(Visitor *v);
  void PreferredPGLMe(Visitor *v);

  void PassVisitorToA(ElectionState * e, Visitor * v);
  void PassVisitorToB(ElectionState * e, Visitor * v);
  void PassThru(ElectionState * e, Visitor * v);

  char* _namestring;

  static VisitorType * _portup_vistype;
  static VisitorType * _svcc_vistype;
  static VisitorType * _portdown_vistype;
  static VisitorType * _npstate_vistype;
  static VisitorType * _election_vistype;
  static VisitorType * _lgndest_vistype;
};

class StateStarting : public InternalElectionState {
public:

  StateStarting(void);
  virtual ~StateStarting();

  virtual const char * StateName(void) const;
};


class StateAwaiting : public InternalElectionState {
public:

  StateAwaiting(void);
  virtual ~StateAwaiting();

  virtual enum ElectionTransitions GetEventType(ElectionState * e, Visitor * v);

  bool SearchPeerTimerExpiry(ElectionState * e);
  bool PeerFound(ElectionState * e, Visitor *& v);
  bool InsertNodeID(ElectionState * e, ElectionVisitor * v);

  virtual const char * StateName(void) const;
};

class StateAwaitingFull : public InternalElectionState {
public:

  StateAwaitingFull(void);
  virtual ~StateAwaitingFull();

  bool LostAllPeers(ElectionState * e, Visitor *v);
  bool DBReceived(ElectionState * e, Visitor *& v);
  bool InsertNodeID(ElectionState * e, ElectionVisitor *v);

  virtual const char * StateName(void) const;
};

class StateInitialDelay : public InternalElectionState {
public:

  StateInitialDelay(void);
  virtual ~StateInitialDelay();

  bool PGLInitTimerExpiry(ElectionState * e);
  bool InsertNodeID(ElectionState * e, ElectionVisitor *v);

  virtual const char * StateName(void) const;
};

class StateCalculating : public InternalElectionState {
public:

  StateCalculating(void);
  virtual ~StateCalculating();

  virtual const char * StateName(void) const;
};

class StateOperNotPGL : public InternalElectionState {
public:

  StateOperNotPGL(void);
  virtual ~StateOperNotPGL();

  bool LoseConnToPGL(ElectionState * e);
  bool InsertNodeID(ElectionState * e, ElectionVisitor * v);
  bool InsertHorzID(ElectionState * e, Visitor * v); 

  virtual const char * StateName(void) const;
};

class StateOperPGL : public InternalElectionState {
public:

  StateOperPGL(void);
  virtual ~StateOperPGL();

  bool ChangePreferredPGL(ElectionState * e);
  bool InsertNodeID(ElectionState * e, ElectionVisitor *v);
  bool InsertHorzID(ElectionState * e, Visitor *v);

  virtual const char * StateName(void) const;
};

class StateAwaitUnanimity : public InternalElectionState {
public:

  StateAwaitUnanimity(void);
  virtual ~StateAwaitUnanimity();

  bool OverrideUnanimityExpiry(ElectionState * e);
  bool OverrideUnanimitySucc(ElectionState * e);
  bool OverrideUnanimityFail(ElectionState * e);
  bool InsertNodeID(ElectionState *, ElectionVisitor *);
  bool InsertHorzID(ElectionState *, Visitor *);

  virtual const char * StateName(void) const;
};


class StateHungElection : public InternalElectionState {
public:

  StateHungElection(void);
  virtual ~StateHungElection();

  bool InsertNodeID(ElectionState * e, ElectionVisitor *v); 
  bool InsertHorzID(ElectionState * e, Visitor *v);

  virtual const char * StateName(void) const;
};

class StateAwaitReelection : public InternalElectionState {
public:

  StateAwaitReelection(void);
  virtual ~StateAwaitReelection();

  bool ReelectionTimerExpiry(ElectionState * e);
  bool GetConnToPGL(ElectionState * e);
  bool InsertNodeID(ElectionState *, ElectionVisitor *);
  bool InsertHorzID(ElectionState *, Visitor *);

  virtual const char * StateName(void) const;
};

#endif //  __InternalElection_H__
