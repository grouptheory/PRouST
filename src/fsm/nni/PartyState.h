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

#ifndef _PARTY_STATE_H_
#define _PARTY_STATE_H_

#ifndef LINT
static char const _Q93bPartyState_h_rcsid_[] =
"$Id: PartyState.h,v 1.6 1998/07/14 15:09:59 marsh Exp $";
#endif

#include <sys/types.h>

#include <codec/uni_ie/ie.h>
#include <fsm/nni/Q93bTimers.h>

class Party;
class Q93bVisitor;

class PartyState {
public:
  virtual ~PartyState();
  // [SigList-7] Recommendation Q.2971 page 57
  // Signals related to primitives
  virtual int SetupReq(Q93bVisitor *qv, Party *p);
  virtual int AlertReq(Q93bVisitor *qv, Party *p);
  virtual int SetupRespReq(Q93bVisitor *qv, Party *p);
  virtual int SetupCompReq(Q93bVisitor *qv, Party *p); 
  virtual int ReleaseReq(Q93bVisitor *qv, Party *p); // redefine as -1 in PN0 and PN5 (Q.2971 p. 106)
  virtual int ReleaseRespReq(Q93bVisitor *qv, Party *p); // redefine as -1 in PN0 (Q.2971 p. 106)
  virtual int ReleaseCompReq(Q93bVisitor *qv, Party *p); // redefine as -1 in PN0 (Q.2971 p. 106)
  virtual int AddPartyReq(Q93bVisitor *qv, Party *p);
  virtual int AddPartyAckReq(Q93bVisitor *qv, Party *p);
  virtual int AddPArtyRejReq(Q93bVisitor *qv, Party *p);
  virtual int PartyAlertReq(Q93bVisitor *qv, Party *p);
  virtual int DropPartyReq(Q93bVisitor *qv, Party *p);
  virtual int DropPartyAckReq(Q93bVisitor *qv, Party *p);
  virtual int PartyStatusEnqReq(Q93bVisitor *qv, Party *p); // redefine as -1 in PN0
  // Messages
  virtual int SetupInd(Q93bVisitor *qv, Party *p);
  virtual int AlertInd(Q93bVisitor *qv, Party *p);
  virtual int ReleaseInd(Q93bVisitor *qv, Party *p); // redefine as -1 in PN0 and PN5 (Q.2971 p. 106)
  virtual int ReleaseCompInd(Q93bVisitor *qv, Party *p); // redefine as -1 in PN0 (Q.2971 p. 106)
  virtual int StatusEnqInd(Q93bVisitor *qv, Party *p); // redefine as -1 in PN0
  virtual int StatusInd(Q93bVisitor *qv, Party *p); // redefine as -1 in PN0 and PN5
  virtual int AddPartyInd(Q93bVisitor *qv, Party *p);
  virtual int AddPartyAckInd(Q93bVisitor *qv, Party *p);
  virtual int AddPartyRejInd(Q93bVisitor *qv, Party *p);
  virtual int PartyAlertInd(Q93bVisitor *qv, Party *p);
  virtual int DropPartyInd(Q93bVisitor *qv, Party *p);
  virtual int DropPartyAckInd(Q93bVisitor *qv, Party *p);

  virtual int UnexpectedMSG(Q93bVisitor *qv, Party *p); // redefine to -1 in PN0 and PN2 (Q.2971 p. 106)
  
  // timers
  virtual void party_t397_timeout(Party *p);
  virtual void party_t398_timeout(Party *p);
  virtual void party_t399_timeout(Party *p);
protected:
  PartyState();
  void ChangeState(Party *p, PartyState *s);
};





class P0_Null : public PartyState {
public:
  virtual ~P0_Null();
  static PartyState *Instance();
  int SetupReq(Q93bVisitor *qv, Party *p);
  int AddPartyReq(Q93bVisitor *qv, Party *p);
  int SetupInd(Q93bVisitor *qv, Party *p);
  int AddPartyInd(Q93bVisitor *qv, Party *p);
  // from page 108 Q.2971
  int StatusEnqInd(Q93bVisitor *qv, Party *p);
  int StatusInd(Q93bVisitor *qv, Party *p);

protected:
  P0_Null();
  static PartyState *_instance;
};


class P1_AddPartyInit : public PartyState {
public:
  virtual ~P1_AddPartyInit();
  static PartyState *Instance();
  int SetupCompReq(Q93bVisitor *qv, Party *p);
  int AddPartyAckInd(Q93bVisitor *qv, Party *p);
  int AddPartyRejInd(Q93bVisitor *qv, Party *p);
  void party_t399_timeout(Party *p);
  int PartyAlertInd(Q93bVisitor *qv, Party *p);
  int AlertInd(Q93bVisitor *qv, Party *p);
  int DropPartyReq(Q93bVisitor *qv, Party *p);
  // from page 103 Q.2971
  int DropPartyAckInd(Q93bVisitor *qv, Party *p);
  // from page 104 Q.2971
  int DropPartyInd(Q93bVisitor *qv, Party *p);
  // from page 106 Q.2971
  int ReleaseInd(Q93bVisitor *qv, Party *p);
  int ReleaseReq(Q93bVisitor *qv, Party *p);
  int ReleaseCompInd(Q93bVisitor *qv, Party *p);
  int ReleaseRespReq(Q93bVisitor *qv, Party *p);
  int PartyStatusEnqReq(Q93bVisitor *qv, Party *p);
  int StatusEnqInd(Q93bVisitor *qv, Party *p);
  // from page 107 Q.2971
  int StatusInd(Q93bVisitor *qv, Party *p);
protected:
  P1_AddPartyInit();
  static PartyState *_instance;
};


class P2_AddPartyRcv : public PartyState {
public:
  virtual ~P2_AddPartyRcv();
  static PartyState *Instance();
  int SetupRespReq(Q93bVisitor *qv, Party *p);
  int AddPartyReq(Q93bVisitor *qv, Party *p);
  int AddPartyRejReq(Q93bVisitor *qv, Party *p);
  int PartyAlertReq(Q93bVisitor *qv, Party *p);
  int AlertReq(Q93bVisitor *qv, Party *p);
  int AddPartyInd(Q93bVisitor *qv, Party *p);
  // from page 103 Q.2971
  int DropPartyAckInd(Q93bVisitor *qv, Party *p);
  // from page 104 Q.2971
  int DropPartyInd(Q93bVisitor *qv, Party *p);
  // from page 106 Q.2971
  int ReleaseInd(Q93bVisitor *qv, Party *p);
  int ReleaseReq(Q93bVisitor *qv, Party *p);
  int ReleaseCompInd(Q93bVisitor *qv, Party *p);
  int ReleaseRespReq(Q93bVisitor *qv, Party *p);
  int PartyStatusEnqReq(Q93bVisitor *qv, Party *p);
  int StatusEnqInd(Q93bVisitor *qv, Party *p);
  // from page 107 Q.2971
  int StatusInd(Q93bVisitor *qv, Party *p);
protected:
  P2_AddPartyRcv();
  static PartyState *_instance;
};


class P3_PartyAlertDel : public PartyState {
public:
  virtual ~P3_PartyAlertDel();
  static PartyState *Instance();
  int SetupRespReq(Q93bVisitor *qv, Party *p);
  int AddPartyAckReq(Q93bVisitor *qv, Party *p);
  int DropPartyReq(Q93bVisitor *qv, Party *p);
  // from page 103 Q.2971
  int DropPartyAckInd(Q93bVisitor *qv, Party *p);
  // from page 104 Q.2971
  int DropPartyInd(Q93bVisitor *qv, Party *p);
  // from page 106 Q.2971
  int ReleaseInd(Q93bVisitor *qv, Party *p);
  int ReleaseReq(Q93bVisitor *qv, Party *p);
  int ReleaseCompInd(Q93bVisitor *qv, Party *p);
  int ReleaseRespReq(Q93bVisitor *qv, Party *p);
  int PartyStatusEnqReq(Q93bVisitor *qv, Party *p);
  int StatusEnqInd(Q93bVisitor *qv, Party *p);
  // from page 107 Q.2971
  int StatusInd(Q93bVisitor *qv, Party *p);
protected:
  P3_PartyAlertDel();
  static PartyState *_instance;
};


class P4_PartyAlertRcv : public PartyState {
public:
  virtual ~P4_PartyAlertRcv();
  static PartyState *Instance();
  int SetupCompReq(Q93bVisitor *qv, Party *p);
  void party_t397_timeout(Party *p);
  int AddPartyAckInd(Q93bVisitor *qv, Party *p);
  int DropPartyReq(Q93bVisitor *qv, Party *p);
  // from page 103 Q.2971
  int DropPartyAckInd(Q93bVisitor *qv, Party *p);
  // from page 104 Q.2971
  int DropPartyInd(Q93bVisitor *qv, Party *p);
  // from page 106 Q.2971
  int ReleaseInd(Q93bVisitor *qv, Party *p);
  int ReleaseReq(Q93bVisitor *qv, Party *p);
  int ReleaseCompInd(Q93bVisitor *qv, Party *p);
  int ReleaseRespReq(Q93bVisitor *qv, Party *p);
  int PartyStatusEnqReq(Q93bVisitor *qv, Party *p);
  int StatusEnqInd(Q93bVisitor *qv, Party *p);
  // from page 107 Q.2971
  int StatusInd(Q93bVisitor *qv, Party *p);
protected:
  P4_PartyAlertRcv();
  static PartyState *_instance;
};


class P5_DropPartyInit : public PartyState {
public:
  virtual ~P5_DropPartyInit();
  static PartyState *Instance();

  // from top of page 105 Q.2971
  int DropPartyInd(Q93bVisitor *qv, Party *p);
  int AddPartyRejInd(Q93bVisitor *qv, Party *p);
  void party_t398_timeout(Party *p);
  // from bottom page 105 Q.2971
  int StatusInd(Q93bVisitor *qv, Party *p);
  // from page 106 Q.2971
  int ReleaseInd(Q93bVisitor *qv, Party *p);
  int ReleaseReq(Q93bVisitor *qv, Party *p);
  int ReleaseCompInd(Q93bVisitor *qv, Party *p);
  int ReleaseRespReq(Q93bVisitor *qv, Party *p);
  int PartyStatusEnqReq(Q93bVisitor *qv, Party *p);
  int StatusEnqInd(Q93bVisitor *qv, Party *p);
  // from page 103 Q.2971
  int DropPartyAckInd(Q93bVisitor *qv, Party *p);

protected:
  P5_DropPartyInit();
  static PartyState *_instance;
};


class P6_DropPartyRcv : public PartyState {
public:
  virtual ~P6_DropPartyRcv();
  static PartyState *Instance();
  int DropPartyReq(Q93bVisitor *qv, Party *p);
  // from page 103 Q.2971
  int DropPartyAckInd(Q93bVisitor *qv, Party *p);
  // from page 104 Q.2971
  int DropPartyInd(Q93bVisitor *qv, Party *p);
  // from page 106 Q.2971
  int ReleaseInd(Q93bVisitor *qv, Party *p);
  int ReleaseReq(Q93bVisitor *qv, Party *p);
  int ReleaseCompInd(Q93bVisitor *qv, Party *p);
  int ReleaseRespReq(Q93bVisitor *qv, Party *p);
  int PartyStatusEnqReq(Q93bVisitor *qv, Party *p);
  int StatusEnqInd(Q93bVisitor *qv, Party *p);
  // from page 107 Q.2971
  int StatusInd(Q93bVisitor *qv, Party *p);
protected:
  P6_DropPartyRcv();
  static PartyState *_instance;
};


class P7_PartyActive : public PartyState {
public:
  virtual ~P7_PartyActive();
  static PartyState *Instance();
  int DropPartyReq(Q93bVisitor *qv, Party *p);
  // from page 103 Q.2971
  int DropPartyAckInd(Q93bVisitor *qv, Party *p);
  // from page 104 Q.2971
  int DropPartyInd(Q93bVisitor *qv, Party *p);
  // from page 106 Q.2971
  int ReleaseInd(Q93bVisitor *qv, Party *p);
  int ReleaseReq(Q93bVisitor *qv, Party *p);
  int ReleaseCompInd(Q93bVisitor *qv, Party *p);
  int ReleaseRespReq(Q93bVisitor *qv, Party *p);
  int PartyStatusEnqReq(Q93bVisitor *qv, Party *p);
  int StatusEnqInd(Q93bVisitor *qv, Party *p);
  // from page 107 Q.2971
  int StatusInd(Q93bVisitor *qv, Party *p);
protected:
  P7_PartyActive();
  static PartyState *_instance;
};

#endif
 
 
