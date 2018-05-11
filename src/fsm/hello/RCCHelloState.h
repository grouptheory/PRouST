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
#ifndef __RCC_HELLO_STATE_H__
#define __RCC_HELLO_STATE_H__

#ifndef LINT
static char const _RCCHelloState_h_rcsid_[] =
"$Id: RCCHelloState.h,v 1.23 1999/02/10 18:37:37 mountcas Exp $";
#endif

#include <FW/actors/State.h> 
#include <iostream.h>

class NodeID;
class ig_lgn_horizontal_link_ext;
class FastUNIVisitor;
class PeerID;
class Addr;

class RCCHelloTimer;
class RCCInactivityTimer;
class RCCIntegrityTimer;
class RCCInactivityTimer;
class RCCInitTimer;
class RetryLGNSVCTimer;

/** RCCHelloState

    The protocol used to verify the communications link between two
    LGNs is very close to the protocol between lowest-level neighbors,
    and uses the same packet type.  However, unlike lowest-level
    neighbor nodes, LGN neighbors will have a singal PNNI routing
    control channel between them.  This SVCC-based RCC is used for the
    exchange of all PNNI routing packets between the LGN neighbors,
    including PTSPs and other packets used to maintain database
    synchronization as well as Hellos.  The Hello protocol used to
    monitor the status of the SVCC triggers the AddPort and DropPort
    events in the neighboring peer state machine that control database
    synchronization between the LGNs.  This is similar to the
    relationship between the Hello protocol and the neighboring peer
    state machines run between lowest-level neighbors.  The event
    AddPort in the neighboring peer state machine is triggered when
    the Hello state machine for the SVCC reaches the state TwoWay
    Inside.  The even DropPort in the neighboring peer state machine
    is triggered when the Hello state machine for the SVCC falls out
    of the TwoWay Inside state.
 */
class RCCHelloState : public State {
  friend class InternalRCCHelloState;
  friend void  AllocTheStates(void);
  friend void  DeallocTheStates(void);
public:

  static const int RCCHelloInterval        = 15;
  static const int RCCInactivityFactor     = 4;
  static const int RCCCalledIntegrityTime  = 50;
  static const int RCCCallingIntegrityTime = 35;
  static const int RCCHLinkInactivityTime  = 120;
  static const int RCCRetryLgnSVCTime      = 30;
  static const int RCCInitLgnSVCTime       = 10;

  /**@name Enumeration of different states.
   */
  //@{
  enum StateID {
    /// Down State (Initial State)
    LinkDownState      = 0,
    /// Attempt
    AttemptState       = 1,
    /// OneWay Inside
    Way1InsideState    = 2,
    /// TwoWay Inside
    Way2InsideState    = 3
  };
  //@}

  /**@name Enumeration of timers.
   */
  //@{
  enum RCCTimers {
    RCCInactivityTIMER = 0,
    RCCHelloTIMER,
    RCCIntegrityTIMER,
    HLInactivityTIMER,
    InitLGNSVCTIMER,
    RetryLGNSVCTIMER 
  };
  //@}

  /**@name Enumeration of transition events.
   */
  //@{
  enum RCCHelloTransitions {
    KillVisitor        = 0,
    Invalid               ,
    passthru              ,
    WayInside1            ,
    WayInside2            ,
    HelloMisMatch         ,
    SVCSetup              ,
    SVCRelease            ,
    SVCConnect            ,
    SVCFailure            ,
    NewUpLink             ,
    RemotePGLChanged      ,
    PGLChanged            ,
    BadNeighbor           ,
    AnUpLinkDown          ,
    NPToFullState
  };
  //@}

  /// Constructor
  RCCHelloState( const NodeID * nid, 
		 const NodeID * unid = 0, 
		 int vpi = -1, int vci = -1,
		 u_short nver = 1, 
		 u_short over = 1, 
		 int cref = -1);
  /// Destructor
  virtual ~RCCHelloState();

  /// Dump contents of data members to standard out (for debugging purposes).
  void                           Dump(void);
  /// Dump contents of data members to specified output stream.
  void                           Dump(ostream & os);
  /// Returns the Current State of the FSM.
  const InternalRCCHelloState  * GetCS(void) const;
  /// Handles all received SimEvent, currently unused.
  void                           Interrupt(SimEvent * ev);
  /// Main entry point for all Visitors.
  State                        * Handle(Visitor * v);
  /// Registers the specified timer.
  void                           RegisterTimer(RCCTimers type);
  /// Deregisters the specified timer.
  void                           CancelTimer(RCCTimers type);

  /// Requests LgnHLinkExt Information from each of the Lgn Hellos
  void  SendLgnHLinkExtRequest(void);
  /// Adds an LGN Horizontal Link Extension IG to its list
  void  AddLgnHLinkExtInfo(int agg, int lport, int rport);

protected:

  /// Returns the Local Node ID
  const NodeID      * GetLocNodeID(void) const;
  /// Returns the Remote Node ID (if known).
  const NodeID      * GetRemNodeID(void) const;
  /// Returns the Upnode ID of the remote node.
  const NodeID      * GetUpnodeID(void) const;
  /// Sets the Local Node ID (unused).
  void                SetLocNodeID(const NodeID *);
  /// Sets the Remote Node ID, as declared in the received Hello Pkt.
  void                SetRemNodeID(const NodeID *);
  /// Sets the Upnode ID, as declared in the received Hello Pkt.
  void                SetUpNodeID(const NodeID *);
  /// Sets the current version supported.
  void                SetCver(u_short cver);
  /// Sets the newest version supported.
  void                SetNver(u_short nver);
  /// Sets the oldest version supported.
  void                SetOver(u_short over);
  /// Sends a Hello Pkt to the remote peer.
  void                SendHello(void);
  /// Parses the Visitors and returns the appropriate event
  RCCHelloTransitions GetEventType(Visitor * v);
  /// Performs the appropriate actions based on the code, as declared in the specification.
  void                do_Hp(Visitor *v, int code);
  /// Starts the SVCC Integrity Timer
  void                StartSVCIntegrityTimer(void);
  /// Stops the SVCC Integrity Timer
  void                StopSVCIntegrityTimer(void);
  /// Sets the VPI that it is talking over.
  void                SetVPI(int vpi);
  /// Sets the VCI that it is talking over.
  void                SetVCI(int vci);
  /// Sends a LinkUp message down to the Lgn Hellos.
  void                SendLinkUp(void);
  /// Sends a LinkDown message to the Lgn Hellos.
  void                SendLinkDown(void);

  /// Returns the LGN Horizontal Link Extensions IG.
  const ig_lgn_horizontal_link_ext * GetLGNHLE(void) const; 
  /// Keeps track of whether or not the SVCC was initiated yet.
  void           SetSVCInitiated(bool val);
  /// Keeps track of whether or not the setup for the SVCC has been received yet.
  void           SetSetupReceived(bool val);
  /// Sets the Call Refernece for this SVCC
  void           SetCREF(int cref); 
  ///
  void           SetPort(u_int port);
  /// Keeps track of whether or not the SVCC is currently up.
  void           SetSVCUp(bool val);
  /// hunh?
  void           SetCause53(bool val);
  /// Parses the LGN Horizontal Link Extensions IGs received from the peer.
  void           ParseLgnExtLinks(const ig_lgn_horizontal_link_ext * lgn);
  /// Notifies of an Induced Uplink.
  void           SendInducedUpLink(FastUNIVisitor * v);
  /// Notifies of an Induced Uplink that has gone down.
  void           SendDropInducedUpLink(FastUNIVisitor * v);
  /// Broadcasts the Link Down to the Lgn Hello FSMs.
  void           BroadCastLinkDownToHLinks(void);
  /// Broadcasts the Hello Mismatch to the Lgn Hello FSMs.
  void           BroadCastMismatchToHLinks(void);
  /// Broadcasts the BadNeighbor to the Lgn Hello FSMs.
  void           BroadCastBadNeighborToHLinks(void);
  /// Sends the Setup Request down towards ACAC to reinitiate the SVCC.
  void           SendSetupRequest(void);
  /// Sends the Release Request down towards ACAC to release the SVCC.
  void           SendReleaseRequest(void); 

private:

  /// Do not define, declared to prevent the compiler from doing it.
  RCCHelloState(const RCCHelloState & rhs);
  /// Do not define, declared to prevent the compiler from doing it.
  const RCCHelloState & operator = (const RCCHelloState & rhs);

  /// Pointer to the Current State
  InternalRCCHelloState * _cs;
  /// true if we are in the process of setting up the SVCC, false otherwise.
  bool                    _booting;

  /**@name Conditional Flags
   */
  //@{
  bool   _CalledParty;
  bool   _SVCUp;
  bool   _SVCInitiated;
  bool   _SetupReceived;
  bool   _UplinkReceived;
  bool   _NPToFullState;
  //@}

  /// The VPI that the SVCC is currently setup over.
  int    _vpi;
  /// The VCI that the SVCC is currently setup over.
  int    _vci;
  /// The Call Reference for the SVCC call.
  u_long _cref;
  ///
  u_int  _port;

  /// Our Logical Node ID.
  NodeID                     * _LocNodeID;
  /// Our Logical Peer ID.
  PeerID                     * _LocPeerGroupID;
  /// Our Local Port ID (unused).
  u_int                        _LocPortID;
  /// LGN Horizontal Link Extensions IG, which must be included in the Hello Pkts.
  ig_lgn_horizontal_link_ext * _HLE; 

  /**@name Versions 
   */
  //@{
    /// Current Version support.
  u_short  _cver;
    /// Oldest Version support.
  u_short  _over;
    /// Newest Version support.
  u_short  _nver;
  //@}

  /// Remote Logical Node ID (if known, zero otherwise).
  NodeID   * _RemNodeID;
  /// Remote Logical Peer Group ID (if known, zero otherwise).
  PeerID   * _RemPeerGroupID;
  /// Remote Port ID (if known).
  u_int      _RemPortID;

  /// UpNodeID as advertised by the uplinks
  NodeID    * _UpNodeID;

  /**@name Timers and Values
   */
  //@{
    /// HelloInterval: interval between successive Hello pkts sent
  RCCHelloTimer   * _HelloTimer;

    /// Inactivity Factor: The maximum time before which you expect
    ///                    a  Hello packet from the remote node
  RCCInactivityTimer  * _InactivityTimer;

  RCCIntegrityTimer   * _IntegrityTimer; 

    /// The amount of time this node continues to advertise
    /// hlink for it has not received LGN Hlink extn in Hello Pkt.
  RCCInactivityTimer  * _HLInactivityTimer;

    /// Timer for Initial timeout in which Setup is attempted
  RCCInitTimer        * _InitLGNSVCTimer; 

    /// Timer for timeout to Retry Setup when attempt before fails
  RetryLGNSVCTimer    * _RetryLGNSVCTimer;
  //@}

  /// What the hell is this for?
  bool _Cause53;

  /// Important functions 
  void                         ChangeState(StateID type);
  void                         ClearDS(void);
  void                         ClearRemotePortInHLE();

  static VisitorType * _hello_vistype;
  static VisitorType * _horlink_vistype;
  static VisitorType * _fastuni_vistype;
  static VisitorType * _npstate_vistype;

  // Different States
  static InternalRCCHelloState * _StateLinkDown;
  static InternalRCCHelloState * _StateAttempt;
  static InternalRCCHelloState * _State1WayInside;
  static InternalRCCHelloState * _State2WayInside;
  static int                     _ref_count;
};


// ---------------------------------------------------------
class InternalRCCHelloState {
public:

  enum primal_type { primal_state };

  enum HelloError {
    fsm_error = 0,
    fsm_ok    = 1
  };

  InternalRCCHelloState(void);
  virtual ~InternalRCCHelloState(void);

  void                 Handle(RCCHelloState *e, Visitor *v);

  virtual int       ExpHelloTimer(RCCHelloState *s);
  virtual int       ExpInactivityTimer(RCCHelloState *s);
  virtual int       ExpIntegrityTimer(RCCHelloState *s);
  // virtual int       ExpHlInactivityTimer(RCCHelloState *s);
  virtual int       ExpInitLGNSVCTimer(RCCHelloState *s); 
  virtual int       ExpRetryLGNSVCTimer(RCCHelloState *s);

  virtual const char * const CurrentStateName(void) const = 0;

protected:

  void            do_Hp(RCCHelloState *s, Visitor *v, int code);
  bool            IsSVCUp(RCCHelloState *s); 
  bool            IsSVCInitiated(RCCHelloState *s);
  bool            IsSetupReceived(RCCHelloState *s);
  bool            IsCalledParty(RCCHelloState *s);
  void            SetCallingParty(RCCHelloState * s);
  bool            IsUplinksReceived(RCCHelloState *s);
  const NodeID  * GetLocNodeID(RCCHelloState * s) const;
  const NodeID  * GetRemNodeID(RCCHelloState * s) const;
  const NodeID  * GetUpNodeID(RCCHelloState * s) const;
  void            SetSVCInitiated(RCCHelloState *s, bool);
  void            SetSetupReceived(RCCHelloState *s, bool);
  void            SetCREF(RCCHelloState *s, int CREF);
  void            SetPort(RCCHelloState *s, u_int port);
  void            SetSVCUp(RCCHelloState *s, bool val);
  void            SendInducedUpLink(RCCHelloState *s, FastUNIVisitor *);
  void            SendDropInducedUpLink(RCCHelloState *s, 
                                               FastUNIVisitor  *); 
  void            StartSVCIntegrityTimer(RCCHelloState *s);
  const char *    OwnerName(RCCHelloState *s);                    
  void            SetVPVC(RCCHelloState *s, int vpi, int vci);
  void            SetCause53(RCCHelloState *s, bool val);
  bool            GetCause53(RCCHelloState *);

  virtual int     RcvLinkUp(RCCHelloState *s, Visitor *v);
  virtual int     RcvWayInside1(RCCHelloState *s, Visitor *v);
  virtual int     RcvWayInside2(RCCHelloState *s, Visitor *v);
  virtual int     RcvHelloMisMatch(RCCHelloState *s, Visitor *v);
  virtual int     RcvLinkDown(RCCHelloState *s,  Visitor *v);

  // FastUNI event handling
  virtual int     RcvSVCSetup(RCCHelloState *s, Visitor *v);
  virtual int     RcvSVCFailure(RCCHelloState *s, Visitor *v);
  virtual int     RcvCallingSVCRelease(RCCHelloState *s, Visitor *v); 
  virtual int     RcvCalledSVCRelease(RCCHelloState *s, Visitor *v);
  virtual int     RcvSVCConnect(RCCHelloState *s, Visitor *v);
  // Uplink event handling
  virtual int     RcvNewUpLink(RCCHelloState *s, Visitor *v);
  virtual int     RcvRemotePGLChange(RCCHelloState *s, Visitor *v);
  virtual int     RcvPGLChange(RCCHelloState *s, Visitor *v);
  virtual int     RcvAnUpLinkDown(RCCHelloState *s, Visitor *v);
  // NP event handling
  virtual int     RcvBadNeighbor(RCCHelloState *s, Visitor *v);

  // Timer handling
  virtual void    RegisterTimer(RCCHelloState *s, RCCHelloState::RCCTimers type);
  // misc
  void            ChangeState(RCCHelloState *s, RCCHelloState::StateID type);
  void            PassThru(RCCHelloState *s, Visitor *v);
  void            ClearDS(RCCHelloState *s);
  void            SendSetupRequest(RCCHelloState *s);
  void            SendReleaseRequest(RCCHelloState *s); 
  void            BroadCastLinkDownToHLinks(RCCHelloState *s);
  void            BroadCastMismatchToHLinks(RCCHelloState *s); 
  void            BroadCastBadNeighborToHLinks(RCCHelloState *s);
  void            HandleCause53(RCCHelloState *s, bool LocalPGLDown); 
  void            PassVisitorToA(RCCHelloState *s, Visitor *v);
};

class SVCStateLinkDown : public InternalRCCHelloState {
public:

  SVCStateLinkDown(void);
  virtual ~SVCStateLinkDown();

  int ExpHelloTimer(RCCHelloState *s);

  virtual const char * const CurrentStateName(void) const { return "SVCStateLinkDown"; }

protected:

  int RcvLinkUp(RCCHelloState *s, Visitor *v);
  int RcvLinkDown(RCCHelloState *s, Visitor *v); 
};

class SVCStateAttempt : public InternalRCCHelloState {
public:

  SVCStateAttempt(void);
  virtual ~SVCStateAttempt();

  virtual const char * const CurrentStateName(void) const { return "SVCStateAttempt"; }

protected:

  int RcvWayInside1(RCCHelloState *s, Visitor *v);
  int RcvWayInside2(RCCHelloState *s, Visitor *v);
  int RcvHelloMisMatch(RCCHelloState *s, Visitor *v);
  int RcvLinkDown(RCCHelloState *s, Visitor *v);
};

class SVCState1WayInside : public InternalRCCHelloState {
public:

  SVCState1WayInside(void);
  virtual ~SVCState1WayInside();

  int     ExpInactivityTimer(RCCHelloState *s);

  virtual const char * const CurrentStateName(void) const { return "SVCState1WayInside"; }

protected:

  int     RcvWayInside1(RCCHelloState *s, Visitor *v);
  int     RcvWayInside2(RCCHelloState *s, Visitor *v);
  int     RcvHelloMisMatch(RCCHelloState *s, Visitor *v);
  int     RcvLinkDown(RCCHelloState *s, Visitor *v);
};


class SVCState2WayInside : public InternalRCCHelloState {
public:

  SVCState2WayInside(void);
  virtual ~SVCState2WayInside();

  int     ExpInactivityTimer(RCCHelloState *s);

  virtual const char * const CurrentStateName(void) const { return "SVCState2WayInside"; }

protected:

  int     RcvWayInside1(RCCHelloState *s, Visitor *v);
  int     RcvWayInside2(RCCHelloState *s, Visitor *v);
  int     RcvHelloMisMatch(RCCHelloState *s, Visitor *v);
  int     RcvLinkDown(RCCHelloState *s, Visitor *v);
  int     RcvBadNeighbor(RCCHelloState *s, Visitor *v);
};

#endif // __RCC_HELLOSTATE_H__
