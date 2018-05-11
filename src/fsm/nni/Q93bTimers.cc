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

#ifndef LINT
static char const _Q93bTimers_cc_rcsid_[] =
"$Id: Q93bTimers.cc,v 1.8 1998/08/06 04:04:59 bilal Exp $";
#endif
#include <common/cprototypes.h>

#ifndef _UNI_TIMERS_CC_
#define _UNI_TIMERS_CC_

extern "C" {
#include <assert.h>
};

#include <fsm/nni/Q93bTimers.h>
#include <fsm/nni/Q93bCall.h>
#include <fsm/nni/Q93bParty.h>

Q93b_timer::Q93b_timer(q93b_timers t, Call *ca, Party *party):TimerHandler((double )t),
		       _call(ca),_party(party)
{
  _timer_diag[0] = '0';
  _timer_diag[1] = '0';
  _timer_diag[2] = '0';
}

Q93b_timer::~Q93b_timer() { }

void Q93b_t301::Callback()
{
  cout << "Base class, can't call me" << endl;
}


Call * Q93b_timer::get_call()
{
  return _call;
}
Party  * Q93b_timer::get_party()
{
  return _party;
}

int Q93b_timer::SetTimer()
{
  Register(this);
  return 1;
}

int Q93b_timer::StopTimer()
{
  Cancel(this);
  return 1;

}

//------------- T301: Alerting message  ----------------
// started after sending an Alerting  message

Q93b_t301::Q93b_t301(Call *ca,Party *party): Q93b_timer(t301,ca,party) {
  _timer_diag[0] = '3';
  _timer_diag[1] = '0';
  _timer_diag[2] = '1';
}

Q93b_timer::q93b_timer_type Q93b_t301::Timer_type()
{
  return timer_t301;
}

void Q93b_timer::Callback()
{
  assert(_call!=0);
  _call->q93b_t301_timeout();
}


//------------- T303: setup message  ----------------
// started after sending a setup  message

Q93b_t303::Q93b_t303(Call *ca,Party *party): Q93b_timer(t303,ca,party) {
  _timer_diag[0] = '3';
  _timer_diag[1] = '0';
  _timer_diag[2] = '3';
}

Q93b_timer::q93b_timer_type Q93b_t303::Timer_type()
{
  return timer_t303;
}

void Q93b_t303::Callback()
{
  assert(_call!=0);
  _call->q93b_t303_timeout();
}


//------------- T308: release message  ----------------
// started after sending a RELEASE  message

Q93b_t308::Q93b_t308(Call *ca,Party *party): Q93b_timer(t308,ca,party) {
  _timer_diag[0] = '3';
  _timer_diag[1] = '0';
  _timer_diag[2] = '8';
}
Q93b_timer::q93b_timer_type Q93b_t308::Timer_type()
{
  return timer_t308;
}

void Q93b_t308::Callback()
{
  assert(_call!=0);
  _call->q93b_t308_timeout();
}

//------------- T309 ----------------
// started on receipt of: AAL_RELEASE_INDICATION from SSCF

Q93b_t309::Q93b_t309(Call *ca,Party *party): Q93b_timer(t309,ca,party) {
  _timer_diag[0] = '3';
  _timer_diag[1] = '0';
  _timer_diag[2] = '9';
}
Q93b_timer::q93b_timer_type Q93b_t309::Timer_type()
{
  return timer_t309;
}

void Q93b_t309::Callback()
{
  assert(_call!=0);
  _call->q93b_t309_timeout();
}

//------------- T310 : call_proceeding  ----------------
// started after sending a CALL_PROCEEDING  message

Q93b_t310::Q93b_t310(Call *ca,Party *party): Q93b_timer(t310,ca,party) {
  _timer_diag[0] = '3';
  _timer_diag[1] = '1';
  _timer_diag[2] = '0';
}
Q93b_timer::q93b_timer_type Q93b_t310::Timer_type(){return timer_t310;}

void Q93b_t310::Callback()
{
  assert(_call!=0);
  _call->q93b_t310_timeout();
}

//------------- T313 : connect message  ----------------
// started after sending a CONNECT  message

Q93b_t313::Q93b_t313(Call *ca,Party *party): Q93b_timer(t313,ca,party) {
  _timer_diag[0] = '3';
  _timer_diag[1] = '1';
  _timer_diag[2] = '3';
}
Q93b_timer::q93b_timer_type Q93b_t313::Timer_type(){return timer_t313;}


void Q93b_t313::Callback()
{
  assert(_call!=0);
  _call->q93b_t313_timeout();
}

//------------- T316 ----------------
// started after sending a RESTART message

Q93b_t316::Q93b_t316(Call *ca,Party *party): Q93b_timer(t316,ca,party) {
  _timer_diag[0] = '3';
  _timer_diag[1] = '1';
  _timer_diag[2] = '6';
}
Q93b_timer::q93b_timer_type Q93b_t316::Timer_type(){return timer_t316;}

void Q93b_t316::Callback()
{
  _call->q93b_t316_timeout();
}

//------------- T317 ----------------
// started on receipt of a RESTART message

Q93b_t317::Q93b_t317(Call *ca,Party *party): Q93b_timer(t317,ca,party) {
  _timer_diag[0] = '3';
  _timer_diag[1] = '1';
  _timer_diag[2] = '7';
}
Q93b_timer::q93b_timer_type Q93b_t317::Timer_type(){return timer_t317;}

void Q93b_t317::Callback()
{
  _call->q93b_t317_timeout();
}

//------------- T322: status_enq message  ----------------
// started after sending a STATUS_ENQ  message

Q93b_t322::Q93b_t322(Call *ca,Party *party): Q93b_timer(t322,ca,party) {
  _timer_diag[0] = '3';
  _timer_diag[1] = '2';
  _timer_diag[2] = '2';
}
Q93b_timer::q93b_timer_type Q93b_t322::Timer_type(){return timer_t322;}

void Q93b_t322::Callback()
{
  _call->q93b_t322_timeout();
}

//------------- T398: drop_party message  ----------------
// started after sending a DROP_PARTY  message

Q93b_t398::Q93b_t398(Call *ca,Party *party): Q93b_timer(t398,ca,party) {
  _timer_diag[0] = '3';
  _timer_diag[1] = '9';
  _timer_diag[2] = '8';
}
Q93b_timer::q93b_timer_type Q93b_t398::Timer_type(){return timer_t398;}

void Q93b_t398::Callback()
{
  assert(_party != 0);
  _party->q93b_t398_timeout();
}

//------------- T399: add_party  message  ----------------
// started after sending an ADD_PARTY  message

Q93b_t399::Q93b_t399(Call *ca,Party *party): Q93b_timer(t399,ca,party) {
  _timer_diag[0] = '3';
  _timer_diag[1] = '9';
  _timer_diag[2] = '9';
}
Q93b_timer::q93b_timer_type Q93b_t399::Timer_type(){return timer_t399;}

void Q93b_t399::Callback()
{
  assert(_party != 0);
  _party->q93b_t399_timeout();
}
#endif
