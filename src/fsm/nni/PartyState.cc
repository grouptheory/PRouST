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
#ifndef LINT
static char const _PartyState_cc_rcsid_[] =
"$Id: PartyState.cc,v 1.14 1998/08/06 04:04:57 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include "Q93bParty.h"
#include "PartyState.h"
#include "Q93bVisitors.h"
#include "Q93bCall.h"


PartyState *P0_Null::_instance = 0;
PartyState *P1_AddPartyInit::_instance = 0;
PartyState *P2_AddPartyRcv::_instance = 0;
PartyState *P3_PartyAlertDel::_instance = 0;
PartyState *P4_PartyAlertRcv::_instance = 0;
PartyState *P5_DropPartyInit::_instance = 0;
PartyState *P6_DropPartyRcv::_instance = 0;
PartyState *P7_PartyActive::_instance = 0;

PartyState::PartyState()
{
}

PartyState::~PartyState()
{
}

// DEFAULTS

// Signals related to primitives
int PartyState::SetupReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::AlertReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::SetupRespReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::SetupCompReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::ReleaseReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::ReleaseCompReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::ReleaseRespReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::AddPartyReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::AddPartyAckReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::AddPArtyRejReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::PartyAlertReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::DropPartyReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::DropPartyAckReq(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::PartyStatusEnqReq(Q93bVisitor *qv, Party *p){ return -1; }

  // Messages
int PartyState::SetupInd(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::AlertInd(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::ReleaseInd(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::ReleaseCompInd(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::StatusEnqInd(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::StatusInd(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::AddPartyInd(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::AddPartyAckInd(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::AddPartyRejInd(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::PartyAlertInd(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::DropPartyInd(Q93bVisitor *qv, Party *p){ return -1; }
int PartyState::DropPartyAckInd(Q93bVisitor *qv, Party *p){ return -1; }


int PartyState::UnexpectedMSG(Q93bVisitor *qv, Party *p){ return -1; }

// timers
void PartyState::party_t397_timeout(Party *p)
{
  cout << "T397 can't expire in this state" << endl;
}

void PartyState::party_t398_timeout(Party *p)
{
  cout << "T398 can't expire in this state" << endl;
}

void PartyState::party_t399_timeout(Party *p)
{
  cout << "T399 can't expire in this state" << endl;
}

void PartyState::ChangeState(Party *p, PartyState *s)
{
  p->ChangeState(s);
}

// SPECIALIZED (state-dependent)


P0_Null::P0_Null()
{
}

P0_Null::~P0_Null()
{
}

PartyState *P0_Null::Instance()
{
  if (!_instance)
    _instance = new P0_Null;
  return(_instance);
}

// this will only occur for the first party
int P0_Null::SetupReq(Q93bVisitor *qv, Party *p)
{
  p->set_eps(ie_end_pt_state::add_party_initiated);
  ChangeState(p, P1_AddPartyInit::Instance());
  return 0;
}

// the call (link state) should be nn10_active OR nn7_alert_received
// for the ADD PARTY to be forwarded
int P0_Null::AddPartyReq(Q93bVisitor *qv, Party *p)
{
  p->GetOwner()->SetT399(p);
  p->set_eps(ie_end_pt_state::add_party_initiated);
  ChangeState(p, P1_AddPartyInit::Instance());
  return 0;
}

int P0_Null::SetupInd(Q93bVisitor *qv, Party *p)
{ return -1; }

int P0_Null::AddPartyInd(Q93bVisitor *qv, Party *p)
{ return -1; }

int P0_Null::StatusInd(Q93bVisitor *qv, Party *p) 
{ return -1; }

int P0_Null::StatusEnqInd(Q93bVisitor *qv, Party *p) 
{ return -1; }

// P1_AddPartyInit

P1_AddPartyInit::P1_AddPartyInit()
{
}

P1_AddPartyInit::~P1_AddPartyInit()
{
}

PartyState *P1_AddPartyInit::Instance()
{
  if (!_instance)
    _instance = new P1_AddPartyInit;
  return(_instance);
}

int P1_AddPartyInit::SetupCompReq(Q93bVisitor *qv, Party *p)
{ return -1; }

int P1_AddPartyInit::AddPartyAckInd(Q93bVisitor *qv, Party *p)
{ return -1; }

int P1_AddPartyInit::AddPartyRejInd(Q93bVisitor *qv, Party *p)
{ return -1; }

void P1_AddPartyInit::party_t399_timeout(Party *p)
{ }

int P1_AddPartyInit::PartyAlertInd(Q93bVisitor *qv, Party *p)
{ return -1; }

int P1_AddPartyInit::AlertInd(Q93bVisitor *qv, Party *p)
{ return -1; }

// see page 103 Q.2971
int P1_AddPartyInit::DropPartyReq(Q93bVisitor *qv, Party *p)
{ return -1; }

int P1_AddPartyInit::DropPartyAckInd(Q93bVisitor *qv, Party *p)
{ return -1; }

// from page 104 Q.2971
int P1_AddPartyInit::DropPartyInd(Q93bVisitor *qv, Party *p)
{ return -1; }

int P1_AddPartyInit::StatusInd(Q93bVisitor *qv, Party *p)
{ return -1; }  

int P1_AddPartyInit::StatusEnqInd(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P1_AddPartyInit::ReleaseCompInd(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P1_AddPartyInit::ReleaseInd(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P1_AddPartyInit::PartyStatusEnqReq(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P1_AddPartyInit::ReleaseRespReq(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P1_AddPartyInit::ReleaseReq(Q93bVisitor *qv, Party *p)
{ return -1; } 

// P2_AddPartyRcv
P2_AddPartyRcv::P2_AddPartyRcv() { }

P2_AddPartyRcv::~P2_AddPartyRcv() { }

PartyState * P2_AddPartyRcv::Instance(void)
{
  if (!_instance)
    _instance = new P2_AddPartyRcv;
  return(_instance);
}

int P2_AddPartyRcv::SetupRespReq(Q93bVisitor *qv, Party *p)
{ return -1; }

int P2_AddPartyRcv::AddPartyReq(Q93bVisitor *qv, Party *p)
{ return -1; }

int P2_AddPartyRcv::AddPartyRejReq(Q93bVisitor *qv, Party *p)
{ return -1; }

int P2_AddPartyRcv::PartyAlertReq(Q93bVisitor *qv, Party *p)
{ return -1; }

int P2_AddPartyRcv::AlertReq(Q93bVisitor *qv, Party *p)
{ return -1; }

int P2_AddPartyRcv::AddPartyInd(Q93bVisitor *qv, Party *p)
{ return -1; }

// page 103
int P2_AddPartyRcv::DropPartyAckInd(Q93bVisitor *qv, Party *p)
{ return -1; }

// from page 104 Q.2971
int P2_AddPartyRcv::DropPartyInd(Q93bVisitor *qv, Party *p)
{ return -1; }

int P2_AddPartyRcv::StatusInd(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P2_AddPartyRcv::StatusEnqInd(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P2_AddPartyRcv::ReleaseCompInd(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P2_AddPartyRcv::ReleaseInd(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P2_AddPartyRcv::PartyStatusEnqReq(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P2_AddPartyRcv::ReleaseRespReq(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P2_AddPartyRcv::ReleaseReq(Q93bVisitor *qv, Party *p)
{ return -1; } 

// P3_PartyAlertDel
P3_PartyAlertDel::P3_PartyAlertDel() { }

P3_PartyAlertDel::~P3_PartyAlertDel() { }

PartyState *P3_PartyAlertDel::Instance(void)
{
  if (!_instance)
    _instance = new P3_PartyAlertDel;
  return(_instance);
}

int P3_PartyAlertDel::SetupRespReq(Q93bVisitor *qv, Party *p)
{ return -1; }

int P3_PartyAlertDel::AddPartyAckReq(Q93bVisitor *qv, Party *p)
{ return -1; }

// see page 103 Q.2971
int P3_PartyAlertDel::DropPartyReq(Q93bVisitor *qv, Party *p)
{ return -1; }

// see page 103 Q.2971
int P3_PartyAlertDel::DropPartyAckInd(Q93bVisitor *qv, Party *p)
{ return -1; }

// from page 104 Q.2971
int P3_PartyAlertDel::DropPartyInd(Q93bVisitor *qv, Party *p)
{ return -1; }

int P3_PartyAlertDel::StatusInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P3_PartyAlertDel::StatusEnqInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P3_PartyAlertDel::ReleaseCompInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P3_PartyAlertDel::ReleaseInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P3_PartyAlertDel::PartyStatusEnqReq(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P3_PartyAlertDel::ReleaseRespReq(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P3_PartyAlertDel::ReleaseReq(Q93bVisitor *qv, Party *p) 
{ return -1; } 

// P4_PartyAlertRcv
P4_PartyAlertRcv::P4_PartyAlertRcv() { }

P4_PartyAlertRcv::~P4_PartyAlertRcv() { }

PartyState *P4_PartyAlertRcv::Instance(void)
{
  if (!_instance)
    _instance = new P4_PartyAlertRcv;
  return(_instance);
}

int P4_PartyAlertRcv::SetupCompReq(Q93bVisitor *qv, Party *p)
{ return -1; }

void P4_PartyAlertRcv::party_t397_timeout(Party *p)
{ }

int P4_PartyAlertRcv::AddPartyAckInd(Q93bVisitor *qv, Party *p)
{ return -1; }

// see page 103 Q.2971
int P4_PartyAlertRcv::DropPartyReq(Q93bVisitor *qv, Party *p)
{ return -1; }

// see page 103 Q.2971
int P4_PartyAlertRcv::DropPartyAckInd(Q93bVisitor *qv, Party *p)
{ return -1; }

// from page 104 Q.2971
int P4_PartyAlertRcv::DropPartyInd(Q93bVisitor *qv, Party *p)
{ return -1; }

int P4_PartyAlertRcv::StatusInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P4_PartyAlertRcv::StatusEnqInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P4_PartyAlertRcv::ReleaseCompInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P4_PartyAlertRcv::ReleaseInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P4_PartyAlertRcv::PartyStatusEnqReq(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P4_PartyAlertRcv::ReleaseRespReq(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P4_PartyAlertRcv::ReleaseReq(Q93bVisitor *qv, Party *p) 
{ return -1; } 

// P5_DropPartyInit
P5_DropPartyInit::P5_DropPartyInit() { }

P5_DropPartyInit::~P5_DropPartyInit() { }

PartyState *P5_DropPartyInit::Instance(void)
{
  if (!_instance)
    _instance = new P5_DropPartyInit;
  return(_instance);
}

int P5_DropPartyInit::DropPartyInd(Q93bVisitor *qv, Party *p)
{ return -1; }

int P5_DropPartyInit::AddPartyRejInd(Q93bVisitor *qv, Party *p)
{ return -1; }

void P5_DropPartyInit::party_t398_timeout(Party *p)
{ }

// see page 103 Q.2971
int P5_DropPartyInit::DropPartyAckInd(Q93bVisitor *qv, Party *p)
{ return -1; }

int P5_DropPartyInit::StatusInd(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P5_DropPartyInit::StatusEnqInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P5_DropPartyInit::ReleaseCompInd(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P5_DropPartyInit::ReleaseInd(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P5_DropPartyInit::PartyStatusEnqReq(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P5_DropPartyInit::ReleaseRespReq(Q93bVisitor *qv, Party *p)
{ return -1; } 

int P5_DropPartyInit::ReleaseReq(Q93bVisitor *qv, Party *p)
{ return -1; } 

// P6_DropPartyRcv
P6_DropPartyRcv::P6_DropPartyRcv() { }

P6_DropPartyRcv::~P6_DropPartyRcv() { }

PartyState *P6_DropPartyRcv::Instance(void)
{
  if (!_instance)
    _instance = new P6_DropPartyRcv;
  return(_instance);
}

int P6_DropPartyRcv::DropPartyReq(Q93bVisitor *qv, Party *p)
{ return -1; }

int P6_DropPartyRcv::StatusInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P6_DropPartyRcv::StatusEnqInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P6_DropPartyRcv::ReleaseCompInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P6_DropPartyRcv::ReleaseInd(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P6_DropPartyRcv::PartyStatusEnqReq(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P6_DropPartyRcv::ReleaseRespReq(Q93bVisitor *qv, Party *p) 
{ return -1; } 

int P6_DropPartyRcv::ReleaseReq(Q93bVisitor *qv, Party *p) 
{ return -1; } 

// see page 103 Q.2971
int P6_DropPartyRcv::DropPartyAckInd(Q93bVisitor *qv, Party *p)
{ return -1; }

// from page 104 Q.2971
int P6_DropPartyRcv::DropPartyInd(Q93bVisitor *qv, Party *p)
{ return -1; }

// P7_PartyActive
P7_PartyActive::P7_PartyActive() { }

P7_PartyActive::~P7_PartyActive() { }

PartyState *P7_PartyActive::Instance(void)
{
  if (!_instance)
    _instance = new P7_PartyActive;
  return(_instance);
}

int P7_PartyActive::DropPartyReq(Q93bVisitor *qv, Party *p)
{ return -1; }

// see page 103 Q.2971
int P7_PartyActive::DropPartyAckInd(Q93bVisitor *qv, Party *p)
{ return -1; }

// from page 104 Q.2971
int P7_PartyActive::DropPartyInd(Q93bVisitor *qv, Party *p)
{ return -1; }

int P7_PartyActive::StatusInd(Q93bVisitor *qv, Party * p) 
{ return -1; } 

int P7_PartyActive::StatusEnqInd(Q93bVisitor *qv, Party * p) 
{ return -1; } 

int P7_PartyActive::ReleaseCompInd(Q93bVisitor *qv, Party * p) 
{ return -1; } 

int P7_PartyActive::ReleaseInd(Q93bVisitor *qv, Party * p) 
{ return -1; } 

int P7_PartyActive::PartyStatusEnqReq(Q93bVisitor *qv, Party * p) 
{ return -1; } 

int P7_PartyActive::ReleaseRespReq(Q93bVisitor *qv, Party * p) 
{ return -1; } 

int P7_PartyActive::ReleaseReq(Q93bVisitor *qv, Party * p) 
{ return -1; } 
