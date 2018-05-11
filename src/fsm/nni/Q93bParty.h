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

#ifndef _PARTY_H_
#define _PARTY_H_

#include <sys/types.h>

#include <codec/uni_ie/ie.h>
#include <fsm/nni/Q93bTimers.h>

#ifndef LINT
static char const _Q93bParty_h_rcsid_[] =
"$Id: Q93bParty.h,v 1.14 1998/07/14 15:08:38 marsh Exp battou $";
#endif

class Call;
class Q93bState;

class Q93b_timer;
class Q93b_t398;
class Q93b_t399;

class Party {
friend class Call;
public:
  enum PartyStates {
    p0_null = 0,
    p1_add_party_initiated,
    p2_add_party_received,
    p3_party_alert_delivered,
    p4_party_alert_received,
    p5_drop_party_initiated,
    p6_drop_party_received,
    p7_party_active
  };
  Party(Call *ca, int epr);
  ~Party();
  ie_end_pt_state::party_state get_eps();
  void set_eps(ie_end_pt_state::party_state state);
  int get_epr();
  void set_epr(int epr);
  void set_epr_ie(InfoElem *epr_ie);
  void set_eps_ie(InfoElem *eps_ie);
  void set_called_number(ie_called_party_num *cn_ie);
  void set_called_subaddr(ie_called_party_subaddr *cs_ie);
  InfoElem *get_epr_ie();
  InfoElem *get_eps_ie();
  InfoElem* get_called_number(void);
  InfoElem* get_called_subaddr(void);
  // timers
  void  q93b_t397_timeout();
  void  q93b_t398_timeout();
  void  q93b_t399_timeout();
  PartyStates GetPartyState();
  void SetPartyState(PartyStates ps);
  Call *GetOwner(void);
  void SetOwner(Call *c);
protected:
  void ChangeState(PartyStates s);
  PartyStates _party_state;
  Call * _ca;
  int _epr;
  Q93b_timer * _t397;
  Q93b_timer * _t398;
  Q93b_timer * _t399;
  Q93b_timer * _active_timer;
  u_int _t397_retries;
  u_int _t398_retries;
  u_int _t399_retries;

  u_int _t397_max_retries;
  u_int _t398_max_retries;
  u_int _t399_max_retries;

  ie_called_party_num *_dest;
  ie_called_party_subaddr *_dest_subaddr;
  ie_end_pt_ref   *_epr_ie;
  ie_end_pt_state *_eps_ie;
};
#endif
