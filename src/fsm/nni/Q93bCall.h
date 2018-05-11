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
#ifndef _Q93BCALL_H_
#define _Q93BCALL_H_

#ifndef LINT
static char const _Q93bCall_h_rcsid_[] =
"$Id: Q93bCall.h,v 1.17 1999/01/25 14:59:06 battou Exp $";
#endif

#include <iostream.h>
#include <sys/types.h>
#include <sys/time.h>

#include<DS/containers/list.h>
#include<DS/containers/h_array.h>

#include <FW/kernel/Kernel.h>
#include <FW/actors/State.h>
#include <codec/uni_ie/ie.h>

class Q93bState;
class Party;
class Q93bVisitor;
class ie_cause;
class Q93b_timer;
class generic_q93b_msg;
class VCAllocator;

class Call : public State {
  friend class Party;
  friend class Q93b_timer;
  friend class Q93bState;
public:

  enum CallState {
    nn_none = -1,
    nn0_null = ie_call_state::u0_null,
    nn1_call_initiated = ie_call_state::u1_call_initiated,
    nn3_call_proc_sent = ie_call_state::u3_out_going_call_proceeding,
    nn4_alert_delivered = ie_call_state::u4_call_delivered,
    nn6_call_present = ie_call_state::u6_call_present,
    nn7_alert_received = ie_call_state::u7_call_received,
    nn9_call_proc_received = ie_call_state::u9_incoming_call_proceeding,
    nn10_active = ie_call_state::u10_active,
    nn11_release_req = ie_call_state::u11_release_request,
    nn12_release_ind = ie_call_state::u12_release_indication,
    rest1_restart = ie_call_state::rest1_restart_request,
    rest2_restart = ie_call_state::rest2_restart
  };

  enum action_type {
    release_ind = 1,
    add_party_ind,
    add_party_ack_ind,
    add_party_rej_ind,
    drop_party_ack_ind,
    drop_party_ind,
    restart_ind,
    restart_ack_ind,
    setup_req,
    call_proc_req,
    call_proc_ind,
    release_req,
    release_comp_req,
    status_resp_req,
    status_req,
    status_enq_req,
    connect_req,
    connect_ack_req,
    add_party_req,
    add_party_ack_req,
    add_party_rej_req,
    drop_party_req,
    drop_party_ack_req,
    restart_req,
    restart_ack_req
  };

  Call(int port, VCAllocator* vcpool);
  virtual ~Call();

  virtual State * Handle(Visitor *v);
  virtual void    Interrupt(SimEvent *e);

  Party * GetParty(int epr);

  void Send2Peer(Q93bVisitor *qv);
  void Send2CCD(Visitor *qv);
  int  Send2Peer(action_type a,ie_cause::CauseValues cv,Party *p=0,
		 CallState cs = nn_none);
  int Send2CCD(action_type a,ie_cause::CauseValues cv,Party *p=0,
	       CallState cs = nn_none);
  int Send2Peer(action_type a);
  int Send2CCD(action_type a);
  Party * Init(Q93bVisitor *qv);
  int IsP2P(void);
  int IsP2MP(void);
  int IsNetLIJ(void);
  int IsRootLIJ(void);

  // timers
  int q93b_t301_timeout();
  int q93b_t303_timeout();
  int q93b_t308_timeout();
  int q93b_t309_timeout();
  int q93b_t310_timeout();
  int q93b_t313_timeout();
  int q93b_t316_timeout();
  int q93b_t317_timeout();
  int q93b_t322_timeout();

  void SetT301();
  void SetT303();
  void SetT308();
  void SetT309();
  void SetT310();
  void SetT313();
  void SetT316();
  void SetT317();
  void SetT322();
  void SetT397(Party *p);
  void SetT398(Party *p);
  void SetT399(Party *p);

  void StopTimer(void); // stop the active timer
  void StopTimer(Party *p); // stop the active timer

  void StopT301();
  void StopT303();
  void StopT308();
  void StopT309();
  void StopT310();
  void StopT313();
  void StopT316();
  void StopT317();
  void StopT322();
  void StopT397(Party *p);
  void StopT398(Party *p);
  void StopT399(Party *p);

  void               SetCallType(ie_bbc::conn_type ct) { _call_type = ct; }
  ie_bbc::conn_type  GetCallType(void) const { return _call_type; }
  Q93bState * GetCS(void) const;

  // Non Visitor Interface
  void SetupReq(generic_q93b_msg *msg);
  void CallProcReq(generic_q93b_msg *msg);
  void AlertReq(generic_q93b_msg *msg);
  void SetupResp(generic_q93b_msg *msg); // same as ConnectReq
  void ConnectReq(generic_q93b_msg *msg);
  void SetupCompResp(generic_q93b_msg *msg); // same as ConnectAckReq
  void ConnectAckReq(generic_q93b_msg *msg);
  void ReleaseReq(generic_q93b_msg *msg);
  void ReleaseResp(generic_q93b_msg *msg); // same as ReleaseCompReq
  void ReleaseCompReq(generic_q93b_msg *msg);

  void SetIdentity(Conduit* c);
  bool RequestVCI(int vci);
  int GetNewVCI(void);
  Conduit* get_me(void);
  void SetVCI(int vci);
  int GetVCI(void);
  int GetVPI(void);
  CallState GetCallState();
  void SetCallState(CallState call_state);

private:

  void ChangeState(Q93bState  *s);

  Q93bState  * _cs;
  int _crv;
  int _vpi;
  int _vci;
  int _port;

  ie_cause::Location                 _un; // set to local_private_network
  ie_bbc::conn_type                  _call_type;
  CallState                          _call_state;
  // this is where we keep the states of the different parties of PMP call
  h_array<int, Party *> _party_map;
  // IE's
  InfoElem * _ie_array[num_ie];
  // the current active timer (depends on state)
  Q93b_timer  * _active_timer;
  Q93b_timer  * _t301;
  Q93b_timer  * _t303;
  Q93b_timer  * _t308;
  Q93b_timer  * _t309;
  Q93b_timer  * _t310;
  Q93b_timer  * _t313;
  Q93b_timer  * _t316;
  Q93b_timer  * _t317;
  Q93b_timer  * _t322;
  u_int _t301_retries;
  u_int _t303_retries;
  u_int _t308_retries;
  u_int _t309_retries;
  u_int _t310_retries;
  u_int _t313_retries;
  u_int _t316_retries;
  u_int _t317_retries;
  u_int _t322_retries;
  u_int _t303_max_retries;
  u_int _t308_max_retries;
  u_int _t309_max_retries;
  u_int _t310_max_retries;
  u_int _t313_max_retries;
  u_int _t316_max_retries;
  u_int _t317_max_retries;
  u_int _t322_max_retries;

  VCAllocator* _vc_oracle;
  Conduit*     _me;
};

#endif

