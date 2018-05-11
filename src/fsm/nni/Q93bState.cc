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
static char const _Q93bState_cc_rcsid_[] =
"$Id: Q93bState.cc,v 1.62 1999/01/28 15:57:44 mountcas Exp battou $";
#endif
#include <common/cprototypes.h>

extern "C" {
#include <assert.h>
};

#include "Q93bVisitors.h"
#include "Q93bTimers.h"
#include "Q93bParty.h"
#include "Q93bCall.h"
#include "Q93bState.h"
#include <FW/basics/diag.h>
#include <FW/basics/Conduit.h>
#include <codec/q93b_msg/q93b_msg.h>

Q93bState *NN0_Null::_instance = 0;
Q93bState *NN1_CallInitiated::_instance = 0;
Q93bState *NN3_CallProcSent::_instance = 0;
Q93bState *NN4_AlertDelivered::_instance = 0;
Q93bState *NN6_CallPresent::_instance = 0;
Q93bState *NN7_AlertReceived::_instance = 0;
Q93bState *NN9_CallProcReceived::_instance = 0;
Q93bState *NN10_Active::_instance = 0;
Q93bState *NN11_ReleaseReq::_instance = 0;
Q93bState *NN12_ReleaseInd::_instance = 0;


// DEFAULT: this is what you get if you do not redefine these methods
Q93bState::Q93bState() { }

Q93bState::~Q93bState() { }

const char * Q93bState::OwnerName(Call * c) const
{ return c->OwnerName(); }

bool Q93bState::AlertReq(Q93bVisitor * qv, Call * c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined AlertReq to handle " 
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::CallProcReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined CallProcReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::ConnectAckReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ConnectAckReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::SetupReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined SetupReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::StatusReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined StatusReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::StatusEnqReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined StatusEnqReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::RestartReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined RestartReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::RestartResp(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined RestartResp to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::RestartAckReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined RestartAckReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::AddPartyReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined AddPartyReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::AddPartyRejReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined AddPartyRejReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::AddPartyAckReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined AddPartyAckReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::DropPartyReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined DropPartyReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::DropPartyAckReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined DropPartyAckReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::AlertInd(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined AlertInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::CallProcInd(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined CallProcInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::ConnectAckInd(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ConnectAckInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::SetupInd(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined SetupInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::RestartInd(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined RestartInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::RestartCompInd(Q93bVisitor *qv, Call *c) 
{
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined RestartCompInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::RestartAckInd(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined RestartAckInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::AddPartyInd(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined AddPartyInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::AddPartyRejInd(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined AddPartyRejInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::AddPartyAckInd(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined AddPartyAckInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::DropPartyCompInd(Q93bVisitor *qv, Call *c) 
{
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined DropPartyCompInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::PartyAlertInd(Q93bVisitor *qv, Call *c) 
{
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined PartyAlertInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::AddPartyCompInd(Q93bVisitor *qv, Call *c) 
{
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined AddPartyCompInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::LeafSetupInd(Q93bVisitor *qv, Call *c) 
{
   DIAG("fsm.nni", DIAG_DEBUG, cout
	<< OwnerName(c) << CurrentStateName() << " has not redefined LeafSetupInd to handle "
	<< qv->GetType() << endl);
  qv->Suicide();
   return false;
}

bool Q93bState::DropPartyCompReq(Q93bVisitor *qv, Call * c) 
{
   DIAG("fsm.nni", DIAG_DEBUG, cout
	<< OwnerName(c) << CurrentStateName() << " has not redefined DropPartyCompReq to handle "
	<< qv->GetType() << endl);
  qv->Suicide();
   return false;
}

bool Q93bState::PartyAlertReq(Q93bVisitor *qv, Call * c) 
{
   DIAG("fsm.nni", DIAG_DEBUG, cout
	<< OwnerName(c) << CurrentStateName() << " has not redefined PartyAlertReq to handle "
	<< qv->GetType() << endl);
  qv->Suicide();
   return false;
}

bool Q93bState::AddPartyCompReq(Q93bVisitor *qv, Call * c) 
{
   DIAG("fsm.nni", DIAG_DEBUG, cout
	<< OwnerName(c) << CurrentStateName() << " has not redefined AddPartyCompReq to handle "
	<< qv->GetType() << endl);
  qv->Suicide();
   return false;
}

bool Q93bState::LeafSetupReq(Q93bVisitor *qv, Call * c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined LeafSetupReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::StatusRespInd(Q93bVisitor *qv, Call * c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined StatusRespInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::SetupCompInd(Q93bVisitor *qv, Call * c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined SetupCompInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::ConnectInd(Q93bVisitor *qv, Call * c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ConnectInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::StatusRespReq(Q93bVisitor *qv, Call * c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined StatusRespReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::SetupCompReq(Q93bVisitor *qv, Call * c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined SetupCompReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool Q93bState::ConnectReq(Q93bVisitor *qv, Call * c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ConnectReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

int Q93bState::q93b_t301_timeout(Call * c) { return c->q93b_t301_timeout(); }
int Q93bState::q93b_t303_timeout(Call * c) { return c->q93b_t303_timeout(); }
int Q93bState::q93b_t308_timeout(Call * c) { return c->q93b_t308_timeout(); }
int Q93bState::q93b_t309_timeout(Call * c) { return c->q93b_t309_timeout(); }
int Q93bState::q93b_t310_timeout(Call * c) { return c->q93b_t310_timeout(); }
int Q93bState::q93b_t313_timeout(Call * c) { return c->q93b_t313_timeout(); }
int Q93bState::q93b_t316_timeout(Call * c) { return c->q93b_t316_timeout(); }
int Q93bState::q93b_t317_timeout(Call * c) { return c->q93b_t317_timeout(); }

// must be redefined in NN0, NN10, NN11, and NN12 p.84 in Q.2971
bool Q93bState::DropPartyInd(Q93bVisitor * qv, Call * c)
{
 if (c->IsP2P()) {
    UnexpectedMSG(qv,c);
    return true;
  }
  return false;
}

// must be redefined in NN0, NN10, NN11, and NN12 p.84 in Q.2971
bool Q93bState::DropPartyAckInd(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** " << OwnerName(c) << CurrentStateName() << " received " << qv->GetType() << endl);

  if (c->IsP2P()) {
    UnexpectedMSG(qv,c);
    return true;
  }
  return false;
}

bool Q93bState::ReleaseCompReq(Q93bVisitor *qv, Call *c) 
{ 
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ReleaseCompReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

// must be redefined only in N0 and N12 (Q.2931 p.146)
bool Q93bState::ReleaseCompInd(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** " << OwnerName(c) << CurrentStateName() 
       << " received " << qv->GetType() << ":" 
       << hex << (int)qv->get_crv() << dec << endl);
  // reset all timers
  c->StopTimer();
  Bomb * TriNitroToluene = qv->SetExplosives();
  c->Send2CCD(qv);
  TriNitroToluene->Detonate();
  return true;
}

/*
 * N0, N11, N12 must redefine this.  See Q.2971, p. 85.
 */
bool Q93bState::ReleaseReq(Q93bVisitor *qv, Call *c) 
{ 
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** " << OwnerName(c) << CurrentStateName() 
       << " received " << qv->GetType() << ":" 
       << hex << (int)qv->get_crv() << dec << endl);
  c->SetT308();
  // ChangeState(c, NN11_ReleaseReq::Instance());
  ChangeState(c, NN12_ReleaseInd::Instance());
  c->Send2Peer(qv);
  return false;
}

// must be redefined only in N0 and N12 (Q.2971 p.83)
// NN1, NN3, NN4, NN6, NN7, NN8, NN9, and NN10 we go to state ReleaseReq
// we will move to NN0_Null once we get the ReleaseRespReq (ReleaseCompReq)
// from CCD
bool Q93bState::ReleaseInd(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** " << OwnerName(c) << CurrentStateName() 
       << " received " << qv->GetType() << ":" << hex 
       << (int)qv->get_crv() << dec << endl);
  ChangeState(c, NN11_ReleaseReq::Instance());
  c->Send2CCD(qv);
  return false;
}


// must be redefined only in N0, N11, and N12 (Q.2931 p.150)
// coded from Q.2971 page 90
bool Q93bState::StatusInd(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** " << OwnerName(c) << CurrentStateName() 
       << " received " << qv->GetType() << ":" << hex 
       << (int)qv->get_crv() << dec << endl);
  qv->Suicide();
  return false;
}

// must be redefined only in N0 (Q.2931 p.150)
bool Q93bState::StatusEnqInd(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** " << OwnerName(c) << CurrentStateName() 
       << " received " << qv->GetType() << ":" << hex 
       << (int)qv->get_crv() << dec << endl);
  qv->Suicide();
  return false;
}

// must be redefined only in N0 (Q.2931 p.144)
bool Q93bState::NotifyInd(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** " << OwnerName(c) << CurrentStateName() 
       << " received " << qv->GetType() << ":" << hex 
       << (int)qv->get_crv() << dec << endl);
  qv->Suicide();
  return false;
}

// must be redefined only in N0 (Q.2931 p.144)
bool Q93bState::NotifyReq(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** " << OwnerName(c) << CurrentStateName()
       << " received " << qv->GetType() << ":" << hex 
       << (int)qv->get_crv() << dec << endl);
  qv->Suicide();
  return false;
}

// must be redefined only in N0 (Q.2931 p.151)
int Q93bState::q93b_t322_timeout(Call * c) 
{
  return c->q93b_t322_timeout(); 
}

/* Make a decision based on the parsing results and coarse it
 * to (OK, RAP, RAI, I, CLR) and provide the cause when necessary
 * return -1 to inform the caller to check for cause 
 */
int Q93bState::VerifyMessage(Q93bVisitor *qv, Call *c)
{
  assert(qv && c);
  generic_q93b_msg * msg = qv->get_msg();
  assert(msg);
  // check results of parsing in msg
  return OK; // always OK for now
}

int Q93bState::UnexpectedMSG(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_ERROR, cout << 
       "Q93bState::UnexpectedMSG() ==> Unexpected messages" << endl);
  return 0;
}

void Q93bState::ChangeState(Call *c, Q93bState *s) { c->ChangeState(s); }

int Q93bState::GetNewVCI(Call *c) { return c->GetNewVCI(); }


// ----------------------------------------------------------------------
// --------- SPECIALIZED IMPLEMENTATIONS DEPENDING ON STATE -------------
// ----------------------------------------------------------------------

Q93bState* NN0_Null::Instance(void)
{
  if (!_instance)
    _instance = new NN0_Null;
  return(_instance);
}

NN0_Null::NN0_Null()  { }
NN0_Null::~NN0_Null() { }

bool NN0_Null::SetupReq(Q93bVisitor *qv, Call *c)
{
  assert(qv && c);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN0_Null received " << qv->GetType() << endl);
  Party *p = c->Init(qv);
  ChangeState(c, NN6_CallPresent::Instance());
  c->Send2Peer(qv);
  c->SetT303();
  return false;
}

bool NN0_Null::SetupInd(Q93bVisitor *qv, Call *c)
{
  bool rval = false;
  assert(qv && c);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** " << OwnerName(c) << CurrentStateName() 
       << " received " << qv->GetType() << ":" << hex 
       << (int)qv->get_crv() << dec << endl);

  int ret = VerifyMessage(qv,c);
  switch (ret) {
    case RAP:
    case OK: {
	Party *p = c->Init(qv);
	ChangeState(c, NN1_CallInitiated::Instance());
	c->Send2CCD(qv);
	if (ret == RAP) {
	    // send a STATUS message
	}
      }
      break;
    case CLR: {
        // send a ReleaseComp message and terminate the call
        // we may need to inform management
        Bomb * trinitrotoluene = qv->SetExplosives();
	// send a ReleaseCompReq before she blows
	// c->Send2Peer(new_qv);
	// bye cruel world ...
	trinitrotoluene->Detonate();
	rval = true;
      }
      break;

    case RAI:
      // send a STATUS message
      break;
  }
  return rval;
}

bool NN0_Null::ReleaseReq(Q93bVisitor * qv, Call * c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ReleaseReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}
  
bool NN0_Null::ReleaseInd(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ReleaseInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool NN0_Null::ReleaseCompInd(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ReleaseReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool NN0_Null::StatusInd(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ReleaseReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool NN0_Null::StatusEnqInd(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ReleaseReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool NN0_Null::NotifyInd(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ReleaseReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool NN0_Null::DropPartyAckInd(Q93bVisitor *qv, Call *c)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined DropPartyAckInd to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

/* NN1_CallInitiated:
 * this state exists in a succeeding side after it has 
 * received  a call establishment request but has not yet responded.
 */
NN1_CallInitiated::NN1_CallInitiated(void)  { }
NN1_CallInitiated::~NN1_CallInitiated(void) { }

Q93bState* NN1_CallInitiated::Instance(void)
{
  if (!_instance)
    _instance = new NN1_CallInitiated;
  return(_instance);
}

bool NN1_CallInitiated::CallProcReq(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN1_CallInitiated received " << qv->GetType() << endl);
  ChangeState(c, NN3_CallProcSent::Instance());
  c->Send2Peer(qv);
  return false;
}

bool NN1_CallInitiated::ReleaseCompReq(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** " << OwnerName(c) << CurrentStateName() 
       << " received " << qv->GetType() << ":" << hex 
       << (int)qv->get_crv() << dec << endl);
  // c->Send2Peer(qv);
  // B-A-N-G-!
  Bomb * trinitrotoluene = qv->SetExplosives();
  // get me the hell out of here before she blows
  c->Send2Peer(qv);
  // bye cruel world ...
  trinitrotoluene->Detonate();
  return true;
}


// NN3_CallProcSent 
NN3_CallProcSent::NN3_CallProcSent()  { }
NN3_CallProcSent::~NN3_CallProcSent() { }

Q93bState* NN3_CallProcSent::Instance(void)
{
  if (!_instance)
    _instance = new NN3_CallProcSent;
  return(_instance);
}

// SetupResp or ConnectReq
bool NN3_CallProcSent::ConnectReq(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN3_CallProcSent received " << qv->GetType() << endl);
  ChangeState(c, NN10_Active::Instance());
  c->Send2Peer(qv);
  return false;
}

bool NN3_CallProcSent::AlertReq(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN3_CallProcSent received " << qv->GetType() << endl);
  ChangeState(c, NN4_AlertDelivered::Instance());
  c->Send2Peer(qv);
  return false;
}

// NN4_AlertDelivered
NN4_AlertDelivered::NN4_AlertDelivered()  { }
NN4_AlertDelivered::~NN4_AlertDelivered() { }

Q93bState* NN4_AlertDelivered::Instance(void)
{
  if (!_instance)
    _instance = new NN4_AlertDelivered;
  return(_instance);
}


bool NN4_AlertDelivered::ConnectReq(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN4_AlertDelivered received " << qv->GetType() << endl);
  ChangeState(c, NN10_Active::Instance());
  c->Send2Peer(qv);
  return false;
}

// NN6_CallPresent
NN6_CallPresent::NN6_CallPresent(void)  { }
NN6_CallPresent::~NN6_CallPresent(void) { }

Q93bState* NN6_CallPresent::Instance(void)
{
  if (!_instance)
    _instance = new NN6_CallPresent;
  return(_instance);
}

int NN6_CallPresent::q93b_t303_timeout(Call *c)
{
  assert(c != 0);
  return c->q93b_t303_timeout();
}

bool NN6_CallPresent::CallProcInd(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN6_CallPresent received " << qv->GetType() << endl);
  c->StopT303();
  c->SetT310();
  ChangeState(c, NN9_CallProcReceived::Instance());
  // forward this CallProcInd so that DF can do the SOLID binding
  c->Send2CCD(qv);
  return false;
}


/* NN7_AlertReceived:
 * this state exists when a preceding side has recieved an ALERTING
 * message from the succeeding side of the PNNI interface.
 *
 */
NN7_AlertReceived::NN7_AlertReceived()  { }
NN7_AlertReceived::~NN7_AlertReceived() { }

Q93bState* NN7_AlertReceived::Instance(void)
{
  if (!_instance)
    _instance = new NN7_AlertReceived;
  return(_instance);
}

int NN7_AlertReceived::q93b_t301_timeout(Call *c)
{
  assert(c != 0);
  return c->q93b_t301_timeout();
}


bool NN7_AlertReceived::ConnectInd(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN7_AlertReceived received " << qv->GetType() << endl);
  c->StopTimer();
  ChangeState(c, NN10_Active::Instance());
  c->Send2CCD(qv);
  return false;
}



/* NN9_CallProcReceived:
 * when a prededing side has received acknowledgement
 * that the succeeding side has received the call establishment request.
 *
 */
NN9_CallProcReceived::NN9_CallProcReceived(void)  { }
NN9_CallProcReceived::~NN9_CallProcReceived(void) { }

Q93bState* NN9_CallProcReceived::Instance(void)
{
  if (!_instance)
    _instance = new NN9_CallProcReceived;
  return(_instance);
}

int NN9_CallProcReceived::q93b_t310_timeout(Call *c)
{
  assert(c != 0);
  return c->q93b_t310_timeout();
}

bool NN9_CallProcReceived::ConnectInd(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN9_CallProcReceived received " << qv->GetType() << endl);
  c->StopT310();
  ChangeState(c, NN10_Active::Instance());
  c->Send2CCD(qv);
  return false;
}

bool NN9_CallProcReceived::AlertInd(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN9_CallProcReceived received " << qv->GetType() << endl);
  c->StopT310();
  c->SetT301();
  c->Send2Peer(qv);
  return false;
}

// NN10_Active
NN10_Active::NN10_Active()  { }
NN10_Active::~NN10_Active() { }

Q93bState* NN10_Active::Instance(void)
{
  if (!_instance)
    _instance = new NN10_Active;
  return(_instance);
}

bool NN10_Active::AddPartyReq(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN10_Active received " << qv->GetType() << endl);
  assert(c->GetCallState() == Call::nn10_active);
  Party *p = c->Init(qv);
  assert(p != 0);
  p->set_eps(ie_end_pt_state::add_party_initiated);
  p->GetOwner()->SetT399(p);
  return false;
}


bool NN10_Active::AddPartyAckReq(Q93bVisitor *qv, Call *c)
{
  qv->Suicide();
  return false;
}

bool NN10_Active::AddPartyRejReq(Q93bVisitor *qv, Call *c)
{
  qv->Suicide();
  return false;
}

bool NN10_Active::PartyAlertReq(Q93bVisitor *qv, Call *c)
{
  qv->Suicide();
  return false;
}

bool NN10_Active::DropPartyReq(Q93bVisitor *qv, Call *c)
{
  qv->Suicide();
  return false;
}

bool NN10_Active::DropPartyAckReq(Q93bVisitor *qv, Call *c)
{
  qv->Suicide();
  return false;
}


/* ReleaseReq: This state exists when a network node has sent a request to
 * the network node at the other side of the PNNI interface to release the
 * ATM connection and has not responded yet.
 */
NN11_ReleaseReq::NN11_ReleaseReq(void)  { }
NN11_ReleaseReq::~NN11_ReleaseReq(void) { }

Q93bState * NN11_ReleaseReq::Instance(void)
{
  if (!_instance)
    _instance = new NN11_ReleaseReq;
  return(_instance);
}

bool NN11_ReleaseReq::StatusInd(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN11_ReleaseInd received " << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool NN11_ReleaseReq::ReleaseReq(Q93bVisitor * qv, Call * c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ReleaseReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}


/* ReleaseInd:
 * This state exists when a network node has received a request from 
 * the network node at the other side of the PNNI interface to release
 * the ATM connection and has not responded yet.
 */

NN12_ReleaseInd::NN12_ReleaseInd(void) { }

NN12_ReleaseInd::~NN12_ReleaseInd(void) { }

Q93bState* NN12_ReleaseInd::Instance(void)
{
  if (!_instance)
    _instance = new NN12_ReleaseInd;
  return(_instance);
}

// Also called ReleaseResp
bool NN12_ReleaseInd::ReleaseCompReq(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** " << OwnerName(c) << CurrentStateName() 
       << " received " << qv->GetType() << ":" << hex 
       << (int)qv->get_crv() << dec << " dying now." << endl);
  // reset all timers
  c->StopTimer();
  Bomb * trinitrotoluene = qv->SetExplosives();
  c->Send2Peer(qv);
  trinitrotoluene->Detonate();
  return true;
}

bool NN12_ReleaseInd::ReleaseInd(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN12_ReleaseInd received " << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool NN12_ReleaseInd::StatusInd(Q93bVisitor *qv, Call *c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout <<
       "*** NN12_ReleaseInd received " << qv->GetType() << endl);
  qv->Suicide();
  return false;
}

bool NN12_ReleaseInd::ReleaseReq(Q93bVisitor * qv, Call * c)
{
  assert(qv != 0 && c != 0);
  DIAG("fsm.nni", DIAG_DEBUG, cout
       << OwnerName(c) << CurrentStateName() << " has not redefined ReleaseReq to handle "
       << qv->GetType() << endl);
  qv->Suicide();
  return false;
}
