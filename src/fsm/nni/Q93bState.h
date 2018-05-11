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

//-*-C++-*-
#ifndef _Q93BSTATE_H_
#define _Q93BSTATE_H_

#ifndef LINT
static char const _Q93bState_h_rcsid_[] =
"$Id: Q93bState.h,v 1.18 1998/02/17 21:02:29 mountcas Exp $";
#endif

#include <FW/kernel/Kernel.h>
#include <FW/actors/State.h>
#include <codec/uni_ie/ie.h>

#include "Q93bCall.h"

class generic_q93b_msg;
class Party;
class ie_cause;
class Call;

const int OK  = 1;
const int RAP = 2;
const int CLR = 3;
const int RAI = 4;
const int I   = 5;

const int KillVisitor = -1;
const int PassVisitor =  0;

class Q93bState {
public:

  enum CallState {
    nn0_null               = 0,
    nn1_call_initiated,
    nn3_call_proc_sent,
    nn4_alert_delivered,
    nn6_call_present,
    nn7_alert_received,
    nn9_call_proc_received,
    nn10_active,
    nn11_release_req,
    nn12_release_ind
  };

  Q93bState();
  virtual ~Q93bState();
  // P2P methods
  virtual bool AlertReq(Q93bVisitor *qv, Call *c);
  virtual bool CallProcReq(Q93bVisitor *qv, Call *c);
  virtual bool ConnectReq(Q93bVisitor *qv, Call *c);
  virtual bool ConnectAckReq(Q93bVisitor *qv, Call *c);
  virtual bool ReleaseReq(Q93bVisitor *qv, Call *c);
  virtual bool ReleaseCompReq(Q93bVisitor *qv, Call *c);
  virtual bool SetupReq(Q93bVisitor *qv, Call *c);
  virtual bool SetupCompReq(Q93bVisitor *qv, Call *c);
  virtual bool StatusReq(Q93bVisitor *qv, Call *c);
  virtual bool StatusEnqReq(Q93bVisitor *qv, Call *c);
  virtual bool StatusRespReq(Q93bVisitor *qv, Call *c);
  virtual bool NotifyReq(Q93bVisitor *qv, Call *c);

  virtual bool AlertInd(Q93bVisitor *qv, Call *c);
  virtual bool CallProcInd(Q93bVisitor *qv, Call *c);
  virtual bool ConnectInd(Q93bVisitor *qv, Call *c);
  virtual bool ConnectAckInd(Q93bVisitor *qv, Call *c);
  
  // redefined in N0 and N12 see page 83 of Q.2971
  virtual bool ReleaseInd(Q93bVisitor *qv, Call *c);
  // redefined in N0 see page 83 of Q.2971
  virtual bool ReleaseCompInd(Q93bVisitor *qv, Call *c);

  virtual bool SetupInd(Q93bVisitor *qv, Call *c);
  virtual bool SetupCompInd(Q93bVisitor *qv, Call *c);
  virtual bool StatusInd(Q93bVisitor *qv, Call *c);
  virtual bool StatusEnqInd(Q93bVisitor *qv, Call *c);
  virtual bool StatusRespInd(Q93bVisitor *qv, Call *c);
  virtual bool NotifyInd(Q93bVisitor *qv, Call *c);

  // global state methods
  bool RestartReq(Q93bVisitor *qv, Call *c);
  bool RestartResp(Q93bVisitor *qv, Call *c);
  bool RestartAckReq(Q93bVisitor *qv, Call *c);

  bool RestartInd(Q93bVisitor *qv, Call *c);
  bool RestartCompInd(Q93bVisitor *qv, Call *c);
  bool RestartAckInd(Q93bVisitor *qv, Call *c);
  // P2MP methods
  virtual bool LeafSetupReq(Q93bVisitor *qv, Call *c);
  virtual bool AddPartyReq(Q93bVisitor *qv, Call *c);
  virtual bool AddPartyCompReq(Q93bVisitor *qv, Call *c);
  virtual bool AddPartyAckReq(Q93bVisitor *qv, Call *c);
  virtual bool PartyAlertReq(Q93bVisitor *qv, Call *c);
  virtual bool AddPartyRejReq(Q93bVisitor *qv, Call *c);
  virtual bool DropPartyReq(Q93bVisitor *qv, Call *c);
  virtual bool DropPartyCompReq(Q93bVisitor *qv, Call *c);
  virtual bool DropPartyAckReq(Q93bVisitor *qv, Call *c);
  
  virtual bool LeafSetupInd(Q93bVisitor *qv, Call *c);
  virtual bool AddPartyInd(Q93bVisitor *qv, Call *c);
  virtual bool AddPartyCompInd(Q93bVisitor *qv, Call *c);
  virtual bool AddPartyAckInd(Q93bVisitor *qv, Call *c);
  virtual bool PartyAlertInd(Q93bVisitor *qv, Call *c);
  virtual bool AddPartyRejInd(Q93bVisitor *qv, Call *c);
  virtual bool DropPartyInd(Q93bVisitor *qv, Call *c);
  virtual bool DropPartyCompInd(Q93bVisitor *qv, Call *c);
  virtual bool DropPartyAckInd(Q93bVisitor *qv, Call *c);
  // Message manipulations
  virtual int VerifyMessage(Q93bVisitor *qv, Call *c);
  int UnexpectedMSG(Q93bVisitor *qv, Call *c);
  // timers
  virtual int q93b_t301_timeout(Call *c);
  virtual int q93b_t303_timeout(Call *c);
  virtual int q93b_t308_timeout(Call *c);
  virtual int q93b_t309_timeout(Call *c);
  virtual int q93b_t310_timeout(Call *c);
  virtual int q93b_t313_timeout(Call *c);
  virtual int q93b_t316_timeout(Call *c);
  virtual int q93b_t317_timeout(Call *c);
  virtual int q93b_t322_timeout(Call *c);

  virtual const char *       CurrentStateName(void) = 0;
  virtual Call::CallState    CurrentCallState(void) = 0;

  const char * OwnerName(Call * c) const;

protected:

  void ChangeState(Call *c, Q93bState *s);
  int GetNewVCI(Call *c);
};

// redefinition of state-dependent behaviors below
class NN0_Null : public Q93bState {
public:

  virtual ~NN0_Null();
  static Q93bState* Instance(void);
  // P2P methods
  bool SetupReq(Q93bVisitor *qv, Call *c);
  bool SetupInd(Q93bVisitor *qv, Call *c);
  bool ReleaseInd(Q93bVisitor *qv, Call *c);
  bool ReleaseCompInd(Q93bVisitor *qv, Call *c);
  bool StatusInd(Q93bVisitor *qv, Call *c);
  bool StatusEnqInd(Q93bVisitor *qv, Call *c);
  bool NotifyInd(Q93bVisitor *qv, Call *c);
  bool DropPartyAckInd(Q93bVisitor *qv, Call *c);
  bool ReleaseReq(Q93bVisitor *qv, Call *c);

  virtual const char *    CurrentStateName(void) { return "NN0_Null"; }
  virtual Call::CallState CurrentCallState(void) { return Call::nn0_null; }

private:

  NN0_Null();
  static Q93bState* _instance;
};

class NN1_CallInitiated : public Q93bState {
public:

  virtual ~NN1_CallInitiated();
  static Q93bState* Instance(void);
  // P2P methods
  bool CallProcReq(Q93bVisitor *qv, Call *c);
  bool ReleaseCompReq(Q93bVisitor *qv, Call *c);

  virtual const char * CurrentStateName(void) { return "NN1_CallInitiated"; }
  virtual Call::CallState CurrentCallState(void) { return Call::nn1_call_initiated; }

private:

  NN1_CallInitiated();
  static Q93bState* _instance;
};

class NN3_CallProcSent : public Q93bState {
public:

  virtual ~NN3_CallProcSent();
  static Q93bState* Instance(void);
  // P2P methods
  bool ConnectReq(Q93bVisitor *qv, Call *c);
  bool AlertReq(Q93bVisitor *qv, Call *c);

  virtual const char * CurrentStateName(void) { return "NN3_CallProcSent"; }
  virtual Call::CallState CurrentCallState(void) { return Call::nn3_call_proc_sent; }

private:

  NN3_CallProcSent();
  static Q93bState* _instance;
};

class NN4_AlertDelivered : public Q93bState {
public:

  virtual ~NN4_AlertDelivered();
  static Q93bState* Instance(void);
  // P2P methods
  bool ConnectReq(Q93bVisitor *qv, Call *c);

  virtual const char * CurrentStateName(void) { return "NN4_AlertDelivered"; }
  virtual Call::CallState CurrentCallState(void) { return Call::nn4_alert_delivered; }

private:

  NN4_AlertDelivered();
  static Q93bState* _instance;
};

class NN6_CallPresent : public Q93bState {
public:

  virtual ~NN6_CallPresent();
  static Q93bState* Instance(void);
  // P2P methods
  bool CallProcInd(Q93bVisitor *qv, Call *c);
  int q93b_t303_timeout(Call *c);

  virtual const char * CurrentStateName(void) { return "NN6_CallPresent"; }
  virtual Call::CallState CurrentCallState(void) { return Call::nn6_call_present; }

private:

  NN6_CallPresent();
  static Q93bState* _instance;
};

class NN7_AlertReceived : public Q93bState {
public:

  virtual ~NN7_AlertReceived();
  static Q93bState* Instance(void);
  // P2P methods
  bool ConnectInd(Q93bVisitor *qv, Call *c);
  int q93b_t301_timeout(Call *c);

  virtual const char * CurrentStateName(void) { return "NN7_AlertReceived"; }
  virtual Call::CallState CurrentCallState(void) { return Call::nn7_alert_received; }

private:

  NN7_AlertReceived();
  static Q93bState* _instance;
};

class NN9_CallProcReceived : public Q93bState {
public:

  virtual ~NN9_CallProcReceived();
  static Q93bState* Instance(void);
  // P2P methods
  bool AlertInd(Q93bVisitor *qv, Call *c);
  bool ConnectInd(Q93bVisitor *qv, Call *c);
  int q93b_t310_timeout(Call *c);

  virtual const char * CurrentStateName(void) { return "NN9_CallProcReceived"; }
  virtual Call::CallState CurrentCallState(void) { return Call::nn9_call_proc_received; }

private:

  NN9_CallProcReceived();
  static Q93bState* _instance;
};

class NN10_Active : public Q93bState {
public:

  virtual ~NN10_Active();
  static Q93bState* Instance(void);
  // P2MP methods
  bool AddPartyReq(class Q93bVisitor *, class Call *);
  bool AddPartyAckReq(class Q93bVisitor *, class Call *);
  bool AddPartyRejReq(class Q93bVisitor *, class Call *);
  bool PartyAlertReq(class Q93bVisitor *, class Call *);
  bool DropPartyReq(class Q93bVisitor *, class Call *);
  bool DropPartyAckReq(class Q93bVisitor *, class Call *);

  virtual const char * CurrentStateName(void) { return "NN10_Active"; }
  virtual Call::CallState CurrentCallState(void) { return Call::nn10_active; }

private:

  NN10_Active();
  static Q93bState* _instance;
};

class NN11_ReleaseReq : public Q93bState {
public:

  virtual ~NN11_ReleaseReq();
  static Q93bState* Instance(void);
  // P2P methods
  bool StatusInd(Q93bVisitor *qv, Call *c);
  bool ReleaseReq(Q93bVisitor * qv, Call * c);

  virtual const char * CurrentStateName(void) { return "NN11_ReleaseReq"; }
  virtual Call::CallState CurrentCallState(void) { return Call::nn11_release_req; }

private:

  NN11_ReleaseReq();
  static Q93bState* _instance;
};

class NN12_ReleaseInd : public Q93bState {
public:

  virtual ~NN12_ReleaseInd();
  static Q93bState* Instance(void);
  // P2P methods
  bool ReleaseCompReq(Q93bVisitor *qv, Call *c);
  bool ReleaseInd(Q93bVisitor *qv, Call *c);
  bool StatusInd(Q93bVisitor *qv, Call *c);
  bool ReleaseReq(Q93bVisitor *qv, Call *c);

  virtual const char * CurrentStateName(void) { return "NN12_ReleaseInd"; }
  virtual Call::CallState CurrentCallState(void) { return Call::nn12_release_ind; }

private:

  NN12_ReleaseInd();
  static Q93bState* _instance;
};

#endif // __Q93BSTATE_H__
