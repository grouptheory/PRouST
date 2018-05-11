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

// -*-C++-*-
#ifndef LINT
static char const _Q93bCall_cc_rcsid_[] =
"$Id: Q93bCall.cc,v 1.55 1999/02/26 15:32:46 talmage Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/diag.h>
#include <FW/kernel/SimEvent.h>
#include <iostream.h>

#include "Q93bVisitors.h"
#include "Q93bTimers.h"
#include "Q93bState.h"
#include "Q93bCall.h"
#include <codec/q93b_msg/q93b_msg.h>
#include "Q93bParty.h"

#include <fsm/forwarder/VCAllocator.h>
#include <fsm/nni/Q93bCreator.h>

#define Q93B_DEBUGGING "fsm.nni.Q93B_debugging"

// -----------------------------------------
Call::Call(int port, VCAllocator * vcpool) : 
  _port(port), _vc_oracle(vcpool), _me(0),
  _call_type(ie_bbc::p2p), 
  _t303_max_retries(1), _t308_max_retries(1), 
  _t309_max_retries(1), _t310_max_retries(1), 
  _t313_max_retries(1), _t316_max_retries(1), 
  _t317_max_retries(1), _t322_max_retries(1), 
  _call_state(nn0_null), _active_timer(0L), 
  _un(ie_cause::local_private_network),
  _vpi(0), _vci(-1)
{
  for (int i = 0; i < num_ie; i++)
    _ie_array[i] = 0L;
  // create the timers
  _t301 = new Q93b_t301(this,0L);
  _t303 = new Q93b_t303(this,0L);
  _t308 = new Q93b_t308(this,0L);
  _t309 = new Q93b_t309(this,0L);
  _t310 = new Q93b_t310(this,0L);
  _t313 = new Q93b_t313(this,0L);
  _t316 = new Q93b_t316(this,0L);
  _t317 = new Q93b_t317(this,0L);
  _t322 = new Q93b_t322(this,0L);
  _cs = NN0_Null::Instance();
}

Call::~Call()
{
  register int i;
  for (i = 0; i < num_ie; i++) {
    if (_ie_array[i]) {
      delete _ie_array[i];
      _ie_array[i] = 0L;
    }
  }
  if (_t301) 
    { _t301->StopTimer();delete _t301; _t301=0L; }
  if (_t303)
    {_t303->StopTimer();delete _t303; _t303=0L;}
  if (_t308)
    {_t308->StopTimer();delete _t308; _t308=0L;}
  if (_t309)
    {_t309->StopTimer();delete _t309; _t309=0L;}
  if (_t310)
    {_t310->StopTimer();delete _t310; _t310=0L;}
  if (_t313)
    {_t313->StopTimer();delete _t313; _t313=0L;}
  if (_t316)
    {_t316->StopTimer();delete _t316; _t316=0L;}
  if (_t317)
    {_t317->StopTimer();delete _t317; _t317=0L;}
  if (_t322)
    {_t322->StopTimer();delete _t322; _t322=0L;}

  DIAG("fsm.nni.Q93bCall", DIAG_DEBUG, cout <<
       OwnerName() << " is dying." << endl);
}

inline void Call::ChangeState(Q93bState *s)
{  
  _cs = s;  
  _call_state = s->CurrentCallState();
}

State * Call::Handle(Visitor * v)
{
  bool killme = false;
  const VisitorType * vt2 = QueryRegistry(Q93B_VISITOR_NAME);
  VisitorType vt1 = v->GetType();

  DIAG("fsm.nni.Q93bCall", DIAG_DEBUG, cout <<
       "*** " << OwnerName() << "(" << this << "): Got a " << v->GetType() << endl);

  // is it a Q93bVisitor?
  if (vt2 && vt1.Is_A(vt2)) {
    Q93bVisitor *qv = (Q93bVisitor *)v;
    killme = qv->CallProtocol(this);
  } else
    PassThru(v);

  delete vt2;
  return (killme ? 0 : this);
}


void  Call::Interrupt(SimEvent *event)
{
}

Party * Call::GetParty(int epr)
{
  if (_party_map.defined(epr))
    return _party_map[epr];
  return 0;
}

Call::CallState Call::GetCallState() {return _call_state;}

void Call::SetCallState(Call::CallState call_state) {_call_state = call_state;}

int Call::IsP2P(void)
{
  if (_call_type == ie_bbc::p2p)
    return 1;
  return 0;
}

int Call::IsP2MP(void)
{
  if (_call_type == ie_bbc::p2mp)
    return 1;
  return 0;
}

int Call::IsRootLIJ(void)
{
  return 0;
}

int Call::IsNetLIJ(void)
{
  return 0;
}

inline Q93bState *Call::GetCS(void) const { return _cs; }

Party * Call::Init(Q93bVisitor * qv)
{
  assert(qv != 0);

  generic_q93b_msg * msg = qv->get_msg();
  assert(msg != 0);

  Party    * party = 0;
  InfoElem * an_ie = 0;

  for (int i = 0; i < num_ie; i++) {
    if (an_ie = msg->ie((InfoElem::ie_ix)i))
      _ie_array[i] = an_ie->copy();
  }
  // clean up since qv->get_msg made a copy
  delete msg;

  ie_bbc * bbc_ie = 
    (ie_bbc *)_ie_array[InfoElem::ie_broadband_bearer_cap_ix];

  if (bbc_ie && ((_call_type = bbc_ie->get_conn_type()) == ie_bbc::p2mp)) {
    InfoElem * epr_ie = _ie_array[InfoElem::ie_end_pt_ref_ix];
    int epr = ((ie_end_pt_ref  *)epr_ie)->get_epr_value();
    party =  new Party(this,epr);
    assert(party != 0);
    party->SetOwner(this); 
    // ie_end_pt_state does not have same variable names for UNI and PNNI
    // so we need to keep both
    party->set_eps(ie_end_pt_state::p_null);
    party->SetPartyState(Party::p0_null);
    _party_map[epr] = party;
    ie_called_party_subaddr *subaddr = NULL;
    if (subaddr = (ie_called_party_subaddr *)_ie_array[InfoElem::ie_called_party_subaddr_ix])
      party->set_called_subaddr(subaddr);
    ie_called_party_num *number = (ie_called_party_num *)_ie_array[InfoElem::ie_called_party_num_ix];
    party->set_called_number(number);
  }
  _crv = qv->get_crv();
  return (party);
}



// top of call is A while bottom of call is B
inline void  Call::Send2Peer(Q93bVisitor *qv)
{
  PassVisitorToB(qv);
}

inline void  Call::Send2CCD(Visitor *qv)
{
  PassVisitorToA(qv);
}

int  Call::Send2Peer(action_type a)
{
  cout << "Ha Haa: " << a << endl;
  return(-1);
}

int Call::Send2CCD(action_type a)
{
  cout << "Ha Haa: " << a << endl;
  return(-1);
}


int Call::Send2Peer(action_type a,
		    ie_cause::CauseValues cv,
		    Party *party,
		    CallState cs)
{
  generic_q93b_msg *msg = 0L;
  Q93bVisitor *qv = 0L;
  InfoElem *ie_array[num_ie];
  for (int i=0;i < num_ie;i++)
    ie_array[i] = NULL;

  // for status message we need to inlude a call state IE
  if (cs != nn_none) {
    // make a PNNI call state IE (need to modify the IEs)
  }
  switch(a) {
    case setup_req:
      // Our SetupReq does not include an ie_conn_id IE. We will
      // take what's proposed to us in the CallProcInd if no conflict.
      msg = new q93b_setup_message(_ie_array, _crv, 0);
      qv = new setupVisitor(msg,Q93bVisitor::setup_req);
      qv->set_msg_type(header_parser::setup_msg);
      break;

    case call_proc_req:
      if (!_ie_array[InfoElem::ie_conn_identifier_ix]) {
	if(_vci == -1)
	  _vci = GetNewVCI();
	_ie_array[InfoElem::ie_conn_identifier_ix] = new ie_conn_id(_vpi,_vci);
      }
      ie_array[InfoElem::ie_conn_identifier_ix] = _ie_array[InfoElem::ie_conn_identifier_ix];
      if(_call_type == ie_bbc::p2mp)
	ie_array[InfoElem::ie_end_pt_ref_ix] = _ie_array[InfoElem::ie_end_pt_ref_ix];
      msg = new q93b_call_proceeding_message(ie_array, _crv, 0);
      qv = new call_procVisitor(msg, Q93bVisitor::call_proc_req);
      qv->set_msg_type(header_parser::call_proceeding_msg);
      break;

    case release_req:
      if (_ie_array[InfoElem::ie_cause_ix])
	((ie_cause *)_ie_array[InfoElem::ie_cause_ix])->set_cause_value(cv);
      else
	_ie_array[InfoElem::ie_cause_ix] = new ie_cause(cv,ie_cause::local_private_network);
      ie_array[InfoElem::ie_cause_ix] = _ie_array[InfoElem::ie_cause_ix];
      msg = new q93b_release_message(ie_array, _crv, 0);
      qv = new releaseVisitor(msg, Q93bVisitor::release_req);
      qv->set_msg_type(header_parser::release_msg);
      break;

    case release_comp_req:
      if (_ie_array[InfoElem::ie_cause_ix])
	((ie_cause *)_ie_array[InfoElem::ie_cause_ix])->set_cause_value(cv);
      else
	_ie_array[InfoElem::ie_cause_ix] = new ie_cause(cv,_un);
      ie_array[InfoElem::ie_cause_ix] = _ie_array[InfoElem::ie_cause_ix];
      msg =  new q93b_release_comp_message(ie_array, _crv, 0);
      qv = new release_compVisitor(msg, Q93bVisitor::release_comp_req);
      qv->set_msg_type(header_parser::release_comp_msg);
      break;

    case status_req:
      ie_array[InfoElem::ie_call_state_ix] = _ie_array[InfoElem::ie_call_state_ix];
      if (_ie_array[InfoElem::ie_cause_ix])
	((ie_cause *)_ie_array[InfoElem::ie_cause_ix])->set_cause_value(cv);
      else
	_ie_array[InfoElem::ie_cause_ix] = new ie_cause(cv,_un);
      ie_array[InfoElem::ie_cause_ix] = _ie_array[InfoElem::ie_cause_ix];
      if (party) {
	ie_array[InfoElem::ie_end_pt_ref_ix] = party->get_epr_ie();
	ie_array[InfoElem::ie_end_pt_state_ix] = party->get_eps_ie();
      }
      msg = new q93b_status_message(ie_array, _crv, 0);
      qv = new statusVisitor(msg, Q93bVisitor::status_req);
      qv->set_msg_type(header_parser::status_msg);
      break;

    case status_enq_req:
      if (party)
	ie_array[InfoElem::ie_end_pt_ref_ix] = party->get_epr_ie();
      msg = new q93b_status_enq_message(ie_array, _crv, 0);
      qv = new status_enqVisitor(msg, Q93bVisitor::status_enq_req);
      qv->set_msg_type(header_parser::status_enq_msg);
      break;

    case connect_req:
      ie_array[InfoElem::ie_aal_param_ix] = _ie_array[InfoElem::ie_aal_param_ix];
      ie_array[InfoElem::ie_blli_ix] = _ie_array[InfoElem::ie_blli_ix];
      ie_array[InfoElem::ie_conn_identifier_ix] = _ie_array[InfoElem::ie_conn_identifier_ix];
      ie_array[InfoElem::ie_end_pt_ref_ix] = _ie_array[InfoElem::ie_end_pt_ref_ix];
      msg = new q93b_connect_message(ie_array, _crv, 0);
      qv = new connectVisitor(msg, Q93bVisitor::connect_req);
      qv->set_msg_type(header_parser::connect_msg);
      break;
      
    case connect_ack_req:
      msg = new q93b_connect_ack_message(ie_array, _crv, 0);
      qv = new connect_ackVisitor(msg, Q93bVisitor::connect_ack_req);
      qv->set_msg_type(header_parser::connect_ack_msg);
      break;

    case add_party_req:
      assert(party != 0);
      // make the add_party_message  -- IEs from the root
      ie_array[InfoElem::ie_aal_param_ix]               = _ie_array[InfoElem::ie_aal_param_ix];
      ie_array[InfoElem::ie_bhli_ix]                    = _ie_array[InfoElem::ie_bhli_ix];
      ie_array[InfoElem::ie_blli_ix]                    = _ie_array[InfoElem::ie_blli_ix];
      ie_array[InfoElem::ie_calling_party_num_ix]       = _ie_array[InfoElem::ie_calling_party_num_ix];
      ie_array[InfoElem::ie_calling_party_subaddr_ix]   = _ie_array[InfoElem::ie_calling_party_subaddr_ix];
      ie_array[InfoElem::ie_broadband_send_comp_ind_ix] = _ie_array[InfoElem::ie_broadband_send_comp_ind_ix];
      ie_array[InfoElem::ie_transit_net_sel_ix]         = _ie_array[InfoElem::ie_transit_net_sel_ix];
      // custom IEs from the party
      ie_array[InfoElem::ie_called_party_num_ix] = party->get_called_number();
      ie_array[InfoElem::ie_called_party_subaddr_ix] = party->get_called_subaddr();
      ie_array[InfoElem::ie_end_pt_ref_ix] = party->get_epr_ie();
      msg = new q93b_add_party_message(ie_array, _crv, 0);
      // make a Q93bVisitor
      qv = new add_partyVisitor(msg, Q93bVisitor::add_party_req);
      qv->set_msg_type(header_parser::add_party_msg);
      qv->set_pid(party->get_epr());
      break;

    case add_party_ack_req:
      assert(party != 0);
      ie_array[InfoElem::ie_end_pt_ref_ix] = party->get_epr_ie();
      msg = new q93b_add_party_ack_message(ie_array, _crv, 0);
      qv = new add_party_ackVisitor(msg, Q93bVisitor::add_party_ack_req);
      qv->set_msg_type(header_parser::add_party_ack_msg);
      qv->set_pid(party->get_epr());
      break;

    case add_party_rej_req:
      assert(party != 0);
      if (_ie_array[InfoElem::ie_cause_ix])
	((ie_cause *)_ie_array[InfoElem::ie_cause_ix])->set_cause_value(cv);
      else
	_ie_array[InfoElem::ie_cause_ix] = new ie_cause(cv,_un);
      ie_array[InfoElem::ie_cause_ix] = _ie_array[InfoElem::ie_cause_ix];
      ie_array[InfoElem::ie_end_pt_ref_ix] = party->get_epr_ie();
      msg = new q93b_add_party_rej_message(ie_array, _crv, 0);
      qv  = new add_party_rejVisitor(msg, Q93bVisitor::add_party_rej_req);
      qv->set_msg_type(header_parser::add_party_rej_msg);
      qv->set_pid(party->get_epr());
      break;

    case drop_party_req:
      if (_ie_array[InfoElem::ie_cause_ix])
	((ie_cause *)_ie_array[InfoElem::ie_cause_ix])->set_cause_value(cv);
      else
	_ie_array[InfoElem::ie_cause_ix] = new ie_cause(cv,_un);
      ie_array[InfoElem::ie_cause_ix] = _ie_array[InfoElem::ie_cause_ix];
      ie_array[InfoElem::ie_end_pt_ref_ix] = party->get_epr_ie();
      msg = new q93b_drop_party_message(ie_array, _crv, 0);
      qv = new drop_partyVisitor(msg, Q93bVisitor::drop_party_req);
      qv->set_msg_type(header_parser::drop_party_msg);
      qv->set_pid(party->get_epr());
      break;

    case drop_party_ack_req:
      assert(party != 0);
      if (_ie_array[InfoElem::ie_cause_ix])
	((ie_cause *)_ie_array[InfoElem::ie_cause_ix])->set_cause_value(cv);
      else
	_ie_array[InfoElem::ie_cause_ix] = new ie_cause(cv,_un);
      // cause is already there, no need to set it
      ie_array[InfoElem::ie_cause_ix] = _ie_array[InfoElem::ie_cause_ix];
      ie_array[InfoElem::ie_end_pt_ref_ix] = party->get_epr_ie();
      msg = new q93b_drop_party_ack_message(ie_array, _crv, 0);
      // make a Q93bVisitor
      qv = new drop_partyVisitor(msg, Q93bVisitor::drop_party_req);
      qv->set_msg_type(header_parser::drop_party_ack_msg);
      qv->set_pid(party->get_epr());
      break;

    case restart_req:
      ie_array[InfoElem::ie_conn_identifier_ix] = _ie_array[InfoElem::ie_conn_identifier_ix];
      ie_array[InfoElem::ie_restart_ind_ix] = _ie_array[InfoElem::ie_restart_ind_ix];
      msg = new q93b_restart_message(ie_array, _crv, 0);
      qv = new restartVisitor(msg, Q93bVisitor::restart_req);
      qv->set_msg_type(header_parser::restart_msg);
      break;

    case restart_ack_req:
      ie_array[InfoElem::ie_conn_identifier_ix] = _ie_array[InfoElem::ie_conn_identifier_ix];
      ie_array[InfoElem::ie_restart_ind_ix] = _ie_array[InfoElem::ie_restart_ind_ix];
      msg = new q93b_restart_ack_message(ie_array, _crv, 0);
      qv = new restart_ackVisitor(msg, Q93bVisitor::restart_ack_req);
      qv->set_msg_type(header_parser::restart_ack_msg);
      break;
  }
  // set the parameters
  qv->set_crv(_crv);
  PassVisitorToB(qv);
  return 0;
}



int Call::Send2CCD(action_type a,
		   ie_cause::CauseValues cv,
		   Party *party,
		   CallState cs)
{
  generic_q93b_msg *msg = 0L;
  Q93bVisitor* qv = 0L;
  int epr = -1;
  if (party)
    epr = party->get_epr();
  InfoElem *ie_array[num_ie]; 
  for(int i = 0;i < num_ie;i++)
    ie_array[i] = NULL;
  // for status message we need to inlude a call state IE
  if (cs != nn_none) {
      // make a PNNI call state IE (need to modify the IEs)
  }
  switch (a) {
    case call_proc_ind:
      // TODO 
      break;

    case release_ind:
      if (_ie_array[InfoElem::ie_cause_ix])
	((ie_cause *)_ie_array[InfoElem::ie_cause_ix])->set_cause_value(cv);
      else
	_ie_array[InfoElem::ie_cause_ix] = new ie_cause(cv,_un);
      ie_array[InfoElem::ie_cause_ix] = _ie_array[InfoElem::ie_cause_ix];
      msg = new q93b_release_message(ie_array, _crv, 0);
      qv = new releaseVisitor(msg, Q93bVisitor::release_ind);
      qv->set_msg_type(header_parser::release_msg);
      break;

    case add_party_ack_ind:
      ie_array[InfoElem::ie_end_pt_ref_ix] = _ie_array[InfoElem::ie_end_pt_ref_ix];
      msg = new q93b_add_party_ack_message(ie_array, _crv, 0);
      qv = new add_party_ackVisitor(msg, Q93bVisitor::add_party_ack_ind);
      qv->set_msg_type(header_parser::add_party_ack_msg);
      break;

    case add_party_rej_ind:
      ie_array[InfoElem::ie_cause_ix] = _ie_array[InfoElem::ie_cause_ix];
      ie_array[InfoElem::ie_end_pt_ref_ix] = _ie_array[InfoElem::ie_end_pt_ref_ix];
      msg = new q93b_add_party_rej_message(ie_array, _crv, 0);
      qv = new add_party_rejVisitor(msg, Q93bVisitor::add_party_rej_ind);
      qv->set_msg_type(header_parser::add_party_rej_msg);
      break;

    case drop_party_ind:
      if (!party)
	return -1;
      if (_ie_array[InfoElem::ie_cause_ix])
	((ie_cause *)_ie_array[InfoElem::ie_cause_ix])->set_cause_value(cv);
      else
	_ie_array[InfoElem::ie_cause_ix] = new ie_cause(cv,_un);
      if (_ie_array[InfoElem::ie_end_pt_ref_ix])
	((ie_end_pt_ref *)_ie_array[InfoElem::ie_end_pt_ref_ix])->set_epr_value(epr);
      else
	_ie_array[InfoElem::ie_end_pt_ref_ix] = new ie_end_pt_ref(epr);
      ie_array[InfoElem::ie_cause_ix] = _ie_array[InfoElem::ie_cause_ix];
      ie_array[InfoElem::ie_end_pt_ref_ix] = _ie_array[InfoElem::ie_end_pt_ref_ix];
      msg = new q93b_drop_party_message(ie_array, _crv, 0);
      qv = new drop_partyVisitor(msg, Q93bVisitor::drop_party_ind);
      qv->set_msg_type(header_parser::drop_party_msg);
      break;

    case drop_party_ack_ind:
      ie_array[InfoElem::ie_cause_ix] = _ie_array[InfoElem::ie_cause_ix];
      ie_array[InfoElem::ie_end_pt_ref_ix] = _ie_array[InfoElem::ie_end_pt_ref_ix];
      msg = new q93b_drop_party_ack_message(ie_array, _crv, 0);
      qv = new drop_party_ackVisitor(msg, Q93bVisitor::drop_party_ack_ind);
      qv->set_msg_type(header_parser::drop_party_ack_msg);
      break;

    case restart_ind:
      ie_array[InfoElem::ie_conn_identifier_ix] = _ie_array[InfoElem::ie_conn_identifier_ix];
      ie_array[InfoElem::ie_restart_ind_ix] = _ie_array[InfoElem::ie_restart_ind_ix];
      msg = new q93b_restart_message(ie_array, _crv, 0);
      qv = new restartVisitor(msg, Q93bVisitor::restart_ind);
      qv->set_msg_type(header_parser::restart_msg);
      break;

    case restart_ack_ind:
      ie_array[InfoElem::ie_conn_identifier_ix] = _ie_array[InfoElem::ie_conn_identifier_ix];
      ie_array[InfoElem::ie_restart_ind_ix] = _ie_array[InfoElem::ie_restart_ind_ix];
      msg = new q93b_restart_ack_message(ie_array, _crv, 0);
      qv = new restart_ackVisitor(msg, Q93bVisitor::restart_ack_ind);
      qv->set_msg_type(header_parser::restart_ack_msg);
      break;
  }
  // set the parameters
  qv->set_crv(_crv);
  PassVisitorToA(qv);
  return 0;
}

inline void Call::SetT301(){_active_timer =_t301; Register(_t301);}
inline void Call::SetT303(){_active_timer =_t303; Register(_t303);}
inline void Call::SetT308(){_active_timer =_t308; Register(_t308);}
inline void Call::SetT309(){_active_timer =_t309; Register(_t309);}
inline void Call::SetT310(){_active_timer =_t310; Register(_t310);}
inline void Call::SetT313(){_active_timer =_t313; Register(_t313);}
inline void Call::SetT316(){_active_timer =_t316; Register(_t316);}
inline void Call::SetT317(){_active_timer =_t317; Register(_t317);}
inline void Call::SetT322(){_active_timer =_t322; Register(_t322);}

inline void Call::StopTimer(void)
{
  if (_active_timer){
    Cancel(_active_timer);
    _active_timer = 0L;
  }
}

inline void Call::StopT301(){_active_timer = 0L; Cancel(_t301);}
inline void Call::StopT303(){_active_timer = 0L; Cancel(_t303);}
inline void Call::StopT308(){_active_timer = 0L; Cancel(_t308);}
inline void Call::StopT309(){_active_timer = 0L; Cancel(_t309);}
inline void Call::StopT310(){_active_timer = 0L; Cancel(_t310);}
inline void Call::StopT313(){_active_timer = 0L; Cancel(_t313);}
inline void Call::StopT316(){_active_timer = 0L; Cancel(_t316);}
inline void Call::StopT317(){_active_timer = 0L; Cancel(_t317);}
inline void Call::StopT322(){_active_timer = 0L; Cancel(_t322);}

int Call::q93b_t301_timeout()
{
  u_char timer_diag[3] = {'3','0','1'};
  _t301_retries++;
  return(-1);
}

inline void Call::SetT397(Party *p){p->_active_timer = p->_t397; Register(p->_t397);}
inline void Call::SetT398(Party *p){p->_active_timer = p->_t398; Register(p->_t398);}
inline void Call::SetT399(Party *p){p->_active_timer = p->_t399; Register(p->_t399);}

inline void Call::StopTimer(Party *p)
{
  if(p->_active_timer)
    {
      Cancel(p->_active_timer);
      p->_active_timer = 0L;
    }
}

inline void Call::StopT397(Party *p){p->_active_timer = 0L; Cancel(p->_t397);}
inline void Call::StopT398(Party *p){p->_active_timer = 0L; Cancel(p->_t398);}
inline void Call::StopT399(Party *p){p->_active_timer = 0L; Cancel(p->_t399);}

int Call::q93b_t303_timeout()
{
  u_char timer_diag[3] = {'3','0','3'};
  _t303_retries++;
  if (_call_state == nn6_call_present) {
    if (_t303_retries < _t303_max_retries) {
      Send2Peer(setup_req);
      Register(_t303);
      return(0);
    }
    _call_state = nn0_null;
    _t303_retries = 0;
    // send a release cpmplete to the succeding side
    Send2Peer(release_comp_req,ie_cause::recovery_on_timer_expiry);
    // initiate clearing (without crankback) toward calling party
    Send2CCD(release_ind,ie_cause::recovery_on_timer_expiry);
    return(0);
  }
  // incompatible state 
  return(-1);
}



// see 5.5.4.3
int Call::q93b_t308_timeout()
{
  return(-1);
}

// See 5.5.6.10: sent when SAAL is disconnected: we got the AAL_RELEASE_INDICATION from SAAL
// apply last 2 paragraphs
// notice that we have distributed this global timer to each call.
// normally the global state accessible by both downmux and upmux should keep this state.

int Call::q93b_t309_timeout()
{
  u_char timer_diag[3] = {'3','0','9'};
  Send2CCD(release_ind,ie_cause::destination_out_of_order);
  return(0);
}

int Call::q93b_t310_timeout()
{
  u_char timer_diag[3] = {'3','1','0'};
  if (_call_state == nn9_call_proc_received) {
    _call_state = nn0_null;
    // send a release complete to the succeding side
    Send2Peer(release_comp_req,ie_cause::recovery_on_timer_expiry);
    // initiate clearing (without crankback) toward calling party
    Send2CCD(release_ind,ie_cause::recovery_on_timer_expiry);
    return(0);
  }
  // incompatible state 
  return(-1);
}


// apply 5.5.2.7 
int Call::q93b_t313_timeout()
{
  u_char timer_diag[3] = {'3','1','3'};

  if ((ie_call_state::call_state_values)_call_state == 
      ie_call_state::u8_connect_request) {
    Send2Peer(release_req,ie_cause::recovery_on_timer_expiry);
    Send2CCD(release_ind,ie_cause::recovery_on_timer_expiry);
    return(0);
  }
  // incompatible state
  cout << "q93b:Call::q93b_t313_timeout(): incompatible state = " << _call_state << endl;
  return(-1);
}

// this timer is set when RESTART was sent
// apply 5.5.5.1
int Call::q93b_t316_timeout()
{
  u_char timer_diag[3] = {'3','1','6'};

  _t316_retries++;
  if ((ie_call_state::call_state_values)_call_state == 
      ie_call_state::rest1_restart_request) {
    if (_t316_retries < _t316_max_retries) {
      Send2Peer(restart_req);
      return(0);
    }
    // no calls should be accepted or sent notify management here
    _call_state = rest2_restart;
    Send2CCD(restart_ind);
    return(0);
  }
  // incompatible state
  cout << "q93b:Call::q93b_t316_timeout(): incompatible state = " << _call_state << endl;
  return(-1);
}


// See 5.5.5.2
// T317 expired before we are done with internal clearing
int Call::q93b_t317_timeout()
{
  u_char timer_diag[3] = {'3','1','7'};
  _t317_retries++;

  cout << "T317 expired before clearing of call" << endl;
  return(-1);
}


// see 5.5.6.11 last paragraph 
int Call::q93b_t322_timeout()
{
  u_char timer_diag[3] = {'3','2','2'};
  _t322_retries++;
  if (_t322_retries < _t322_max_retries) {
    Send2Peer(status_enq_req);
      return(0);
  }
  Send2Peer(release_req,ie_cause::temporary_failure);
  Send2CCD(release_ind,ie_cause::temporary_failure);
  return(0);
}


void Call::SetupReq(generic_q93b_msg *msg)
{
  Q93bVisitor* qv = new setupVisitor(msg,Q93bVisitor::setup_req);
  qv->set_msg_type(header_parser::setup_msg);
  qv->set_crv(_crv);
  qv->CallProtocol(this); 
}

void Call::CallProcReq(generic_q93b_msg *msg)
{
  Q93bVisitor* qv = new call_procVisitor(msg, Q93bVisitor::call_proc_req);
  qv->set_msg_type(header_parser::call_proceeding_msg);
  qv->set_crv(_crv);
  qv->CallProtocol(this); 
}

void Call::AlertReq(generic_q93b_msg *msg)
{
  cout << " TODO " << endl;
}

// same as ConnectReq
void Call::SetupResp(generic_q93b_msg *msg)
{
  ConnectReq(msg);
}

void Call::ConnectReq(generic_q93b_msg *msg)
{
  Q93bVisitor* qv = new connectVisitor(msg, Q93bVisitor::connect_req);
  qv->set_msg_type(header_parser::connect_msg);
  qv->set_crv(_crv);
  qv->CallProtocol(this); 
}

// same as ConnectAckReq
void Call::SetupCompResp(generic_q93b_msg *msg)
{
  ConnectAckReq(msg);
}

void Call::ConnectAckReq(generic_q93b_msg *msg)
{
  Q93bVisitor* qv = new connect_ackVisitor(msg, Q93bVisitor::connect_ack_req);
  qv->set_msg_type(header_parser::connect_ack_msg);
  qv->set_crv(_crv);
  qv->CallProtocol(this); 
}

void Call::ReleaseReq(generic_q93b_msg *msg)
{
  Q93bVisitor* qv = new releaseVisitor(msg, Q93bVisitor::release_req);
  qv->set_msg_type(header_parser::release_msg);
  qv->set_crv(_crv);
  qv->CallProtocol(this); 
}

// same as ReleaseCompReq
void Call::ReleaseResp(generic_q93b_msg *msg)
{
  ReleaseCompReq(msg);
}

void Call::ReleaseCompReq(generic_q93b_msg *msg)
{
  Q93bVisitor* qv = new release_compVisitor(msg, Q93bVisitor::release_comp_req);
  qv->set_msg_type(header_parser::release_comp_msg);
  qv->set_crv(_crv);
  qv->CallProtocol(this); 
}

void Call::SetIdentity(Conduit* c){
  _me = c;
}

bool Call::RequestVCI(int vci){
  assert(_vc_oracle);
  return _vc_oracle->RequestVCI(vci);
}

int Call::GetNewVCI()
{
  assert(_vc_oracle);
  return _vc_oracle->GetNewVCI();
}

Conduit* Call::get_me(void)
{
  return _me;
}

// return the old one to the pool 
inline void Call::SetVCI(int vci)
{
  if (_vci != -1) {
    assert(_vc_oracle);
    _vc_oracle->ReturnVCI((unsigned int)_vci);
  }
  _vci = vci;
}

inline int  Call::GetVCI(void) { return _vci; }
inline int  Call::GetVPI(void) { return _vpi; }
