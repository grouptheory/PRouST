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
static char const _Q93bVisitors_cc_rcsid_[] =
"$Id: Q93bVisitors.cc,v 1.72 1999/01/28 15:49:23 mountcas Exp battou $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/diag.h>
#include <FW/basics/Conduit.h>
#include "Q93bVisitors.h"
#include "Q93bCall.h"
#include "Q93bParty.h"
#include "Q93bState.h"
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/ie.h>
#include <codec/q93b_msg/q93b_msg.h>


Q93bVisitor::Q93bVisitor(generic_q93b_msg * msg, Q93bVisitorType vt)
  : Visitor(_my_type), PacketVisitor(_my_type), _msg(msg), _vt(vt), 
    _vpi(0), _vci(-1), _port(-1) 
{  
  strcpy(_label, "");
}

Q93bVisitor::Q93bVisitor(const Q93bVisitor & rhs) 
  : Visitor(_my_type), PacketVisitor(rhs), _msg(0), _vt(rhs._vt), _vpi(rhs._vpi), 
    _vci(rhs._vci), _port(rhs._port)
{ 
  if (rhs._msg) _msg = rhs._msg->copy(); 
  strcpy(_label, rhs._label);
}

Q93bVisitor::Q93bVisitor(vistype & child_type, 
			 generic_q93b_msg * msg, 
			 Q93bVisitorType vt) 
  : Visitor(_my_type), PacketVisitor(child_type.derived_from(_my_type)), 
    _msg(msg), _vt(vt), _vpi(0), _vci(-1), _port(-1) 
{  
  strcpy(_label, "");
}

Q93bVisitor::~Q93bVisitor() { }

inline char * Q93bVisitor::label(void) { return _label; }

const vistype & Q93bVisitor::GetClassType(void) const
{ return _my_type;  }

const VisitorType Q93bVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

inline Q93bVisitor::Q93bVisitorType Q93bVisitor::GetVT(void) { return _vt; }

generic_q93b_msg * Q93bVisitor::get_msg(void)
{ 
  generic_q93b_msg * ret = _msg->copy();
  return ret;
}

generic_q93b_msg * Q93bVisitor::take_msg(void) 
{
  generic_q93b_msg * ret = _msg;
  _msg = 0;
  return ret;
}

inline generic_q93b_msg* Q93bVisitor::share_msg(void) 
{  return _msg;  }

inline void Q93bVisitor::set_msg(generic_q93b_msg * msg)
{
  // Don't clobber _msg if it's the same as msg.
  if (_msg && _msg != msg)
    delete _msg;
  _msg = msg;
}

inline void Q93bVisitor::set_msg_type(header_parser::msg_type mt)
{  
  diag("fsm.nni", DIAG_FATAL, 
       "Stay seated.  The code-master is going to give you a spanking.\n");
}

inline header_parser::msg_type Q93bVisitor::get_msg_type(void)
{
  if (_msg) return _msg->type();
  return header_parser::UNASSIGNED_MSG_TYPE;
}

inline void Q93bVisitor::set_crv(int crv)
{  if (_msg) _msg->set_crv(crv);  }

inline int Q93bVisitor::get_crv(void)
{
  int rval = -1;

  if (_msg) rval = _msg->get_crv();
  return  rval;
}

inline void Q93bVisitor::set_pid(int epr)
{  _epr = epr; }

inline int Q93bVisitor::get_pid(void)
{  return  _epr; }

inline void Q93bVisitor::set_port(int port)
{  
  _port = port; 
}

inline int Q93bVisitor::get_port(void)
{  
  return  _port; 
}

inline int Q93bVisitor::get_vpi(void) 
{  return _vpi; }

inline int Q93bVisitor::get_vci(void)
{  return _vci; }

inline void Q93bVisitor::set_vpi(int vpi)
{  _vpi = vpi; }

inline void Q93bVisitor::set_vci(int vci)
{  _vci = vci; }

inline void Q93bVisitor::TurnIntoReq(void)
{
  if (_vt < 0)
    _vt = (Q93bVisitorType) (- _vt);
}

inline void Q93bVisitor::TurnIntoInd(void)
{
  if (_vt > 0)
    _vt = (Q93bVisitorType) (- _vt);
}

inline bool Q93bVisitor::IsReq(void)
{
  if (_vt > 0)
    return true;
  return false;
}

inline bool Q93bVisitor::IsInd(void)
{
  if (_vt < 0)
    return true;
  return false;
}

Bomb * Q93bVisitor::SetExplosives(void)
{
  Conduit * haplessVictim = CurrentConduit();
  return new Bomb( haplessVictim );
}

ostream & operator << (ostream & os, const Q93bVisitor & rhs)
{
  os << rhs.GetType() << ":" << rhs._vt << " " << rhs._label << endl;
  // if (rhs._msg) os << *rhs._msg << endl;
  os << (int)rhs._vpi << ":" << (int)rhs._vci << ":" 
     << (int)rhs._port 
     << ":" << rhs._epr << endl;
  return os;
}

bool Q93bVisitor::encode(unsigned char *& buffer, int & len) const
{
  bool rval = false;
  if (_msg) {
    assert( (len = _msg->encoded_bufferlen( )) > 0 );
    memcpy( buffer, _msg->encoded_buffer( ), len );
    rval = true;
  }
  return rval;
}

bool Q93bVisitor::decode(const unsigned char *& buffer, int & len)
{
  bool rval = false;
  if ( _msg = Parse( buffer, len ) )
    rval = true;
  return rval;
}

// ----------------------------------------------------------
setupVisitor::setupVisitor(generic_q93b_msg *msg, Q93bVisitorType vt) 
  : Q93bVisitor(_my_type, msg,vt) { }

setupVisitor::setupVisitor(const setupVisitor & rhs) 
  : Q93bVisitor(rhs) { }

setupVisitor::~setupVisitor() { }

Visitor * setupVisitor::dup(void) const
{ return new setupVisitor(*this); }

const vistype & setupVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType setupVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

bool setupVisitor::CallProtocol(Call *c)
{
  bool rval = false;

  if (GetVT() == setup_req)
    rval = c->GetCS()->SetupReq(this, c);
  else
    rval = c->GetCS()->SetupInd(this, c);
  return rval;
}

// ----------------------------------------------------------
bool setup_compVisitor::CallProtocol(Call *c) 
{
  bool rval = false;

  if (GetVT() == setup_comp_req)
    rval = c->GetCS()->SetupCompReq(this,c);
  else
    rval = c->GetCS()->SetupCompInd(this,c);
  return rval;
}

setup_compVisitor::setup_compVisitor(generic_q93b_msg *msg, 
				     Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

setup_compVisitor::setup_compVisitor(const setup_compVisitor & rhs) :
  Q93bVisitor(rhs) { }

setup_compVisitor::~setup_compVisitor() { }

Visitor * setup_compVisitor::dup(void) const
{ return new setup_compVisitor(*this); }

const vistype & setup_compVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType setup_compVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// ----------------------------------------------------------
bool call_procVisitor::CallProtocol(Call *c) 
{
  bool rval = false;

  if (GetVT() == call_proc_req)
    rval = c->GetCS()->CallProcReq(this,c);
  else
    rval = c->GetCS()->CallProcInd(this,c);
  return rval;
}

call_procVisitor::call_procVisitor(generic_q93b_msg *msg,
				   Q93bVisitorType vt) : 
  Q93bVisitor(_my_type, msg,vt) { }

call_procVisitor::call_procVisitor(const call_procVisitor & rhs) :
  Q93bVisitor(rhs) { }

call_procVisitor::~call_procVisitor() { }

Visitor * call_procVisitor::dup(void) const
{ return new call_procVisitor(*this); }

const vistype & call_procVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType call_procVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// -----------------------------------------------------------
bool connectVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == connect_req)
    rval = c->GetCS()->ConnectReq(this,c);  
  else
    rval = c->GetCS()->ConnectInd(this,c);  
  return rval;
}

connectVisitor::connectVisitor(generic_q93b_msg *msg, Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

connectVisitor::connectVisitor(const connectVisitor & rhs) :
  Q93bVisitor(rhs) { }

connectVisitor::~connectVisitor() { }

Visitor * connectVisitor::dup(void) const
{ return new connectVisitor(*this); }

const vistype & connectVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType connectVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// ------------------------------------------------------------
bool connect_ackVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == connect_ack_req)
    rval = c->GetCS()->ConnectAckReq(this,c);  
  else
    rval = c->GetCS()->ConnectAckInd(this,c);  
  return rval;
}

connect_ackVisitor::connect_ackVisitor(generic_q93b_msg *msg, 
				       Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

connect_ackVisitor::connect_ackVisitor(const connect_ackVisitor & rhs) :
  Q93bVisitor(rhs) { }

connect_ackVisitor::~connect_ackVisitor() { }

Visitor * connect_ackVisitor::dup(void) const
{ return new connect_ackVisitor(*this); }

const vistype & connect_ackVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType connect_ackVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// -------------------------------------------------------------
bool  releaseVisitor::CallProtocol(Call *c) 
{
  bool rval = false;

  if (GetVT() == release_req)
    rval = c->GetCS()->ReleaseReq(this,c);
  else
    rval = c->GetCS()->ReleaseInd(this,c);
  return rval;
}

releaseVisitor:: releaseVisitor(generic_q93b_msg *msg, Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

releaseVisitor::releaseVisitor(const releaseVisitor & rhs) :
  Q93bVisitor(rhs) { }

releaseVisitor::~ releaseVisitor() { }

Visitor * releaseVisitor::dup(void) const
{ return new releaseVisitor(*this); }

const vistype & releaseVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType releaseVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// -------------------------------------------------------------
bool release_compVisitor::CallProtocol(Call *c) 
{
  bool rval = false;

  if (GetVT() == release_comp_req)
    rval = c->GetCS()->ReleaseCompReq(this,c);
  else
    rval = c->GetCS()->ReleaseCompInd(this,c);
  return rval;
}

release_compVisitor::release_compVisitor(generic_q93b_msg *msg, 
					 Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

release_compVisitor::release_compVisitor(const release_compVisitor & rhs) :
  Q93bVisitor(rhs) { }

release_compVisitor::~release_compVisitor() { }

Visitor * release_compVisitor::dup(void) const
{ return new release_compVisitor(*this); }

const vistype & release_compVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType release_compVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// --------------------------------------------------------------
bool status_enqVisitor::CallProtocol(Call *c) 
{
  bool rval = false;

  if (GetVT() == status_enq_req)
    rval = c->GetCS()->StatusEnqReq(this,c);
  else
    rval = c->GetCS()->StatusEnqInd(this,c);
  return rval;
}

status_enqVisitor::status_enqVisitor(generic_q93b_msg *msg, 
				     Q93bVisitorType vt) : 
  Q93bVisitor(_my_type, msg,vt) { }

status_enqVisitor::status_enqVisitor(const status_enqVisitor & rhs) :
  Q93bVisitor(rhs) { }

status_enqVisitor::~status_enqVisitor() { }

Visitor * status_enqVisitor::dup(void) const
{ return new status_enqVisitor(*this); }

const vistype & status_enqVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType status_enqVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// ---------------------------------------------------------------
bool statusVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == status_req)
    rval = c->GetCS()->StatusRespReq(this,c);  
  else
    rval = c->GetCS()->StatusRespInd(this,c);  
  return rval;
}

statusVisitor::statusVisitor(generic_q93b_msg *msg, Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

statusVisitor::statusVisitor(const statusVisitor & rhs) :
  Q93bVisitor(rhs) { }

statusVisitor::~statusVisitor() { }

Visitor * statusVisitor::dup(void) const
{ return new statusVisitor(*this); }

const vistype & statusVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType statusVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// ----------------------------------------------------------------
bool add_partyVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == add_party_req)
    rval = c->GetCS()->AddPartyReq(this,c);  
  else
    rval = c->GetCS()->AddPartyInd(this,c);  
  return rval;
}

add_partyVisitor::add_partyVisitor(generic_q93b_msg *msg, Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

add_partyVisitor::add_partyVisitor(const add_partyVisitor & rhs) :
  Q93bVisitor(rhs) { }

add_partyVisitor::~add_partyVisitor() { }

Visitor * add_partyVisitor::dup(void) const
{ return new add_partyVisitor(*this); }

const vistype & add_partyVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType add_partyVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// -----------------------------------------------------------------
bool add_party_compVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == add_party_comp_req)
    rval = c->GetCS()->AddPartyCompReq(this,c);  
  else
    rval = c->GetCS()->AddPartyCompInd(this,c);  
  return rval;
}

add_party_compVisitor::add_party_compVisitor(generic_q93b_msg *msg, 
					     Q93bVisitorType vt) : 
  Q93bVisitor(_my_type, msg,vt) { }

add_party_compVisitor::add_party_compVisitor(const add_party_compVisitor & rhs) :
  Q93bVisitor(rhs) { }

add_party_compVisitor::~add_party_compVisitor() { }

Visitor * add_party_compVisitor::dup(void) const
{ return new add_party_compVisitor(*this); }

const vistype & add_party_compVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType add_party_compVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// ------------------------------------------------------------------
bool add_party_ackVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == add_party_ack_req)
    rval = c->GetCS()->AddPartyAckReq(this,c);  
  else
    rval = c->GetCS()->AddPartyAckInd(this,c);  
  return rval;
}

add_party_ackVisitor::add_party_ackVisitor(generic_q93b_msg *msg, 
					   Q93bVisitorType vt) : 
  Q93bVisitor(_my_type, msg,vt) { }

add_party_ackVisitor::add_party_ackVisitor(const add_party_ackVisitor & rhs) :
  Q93bVisitor(rhs) { }

add_party_ackVisitor::~add_party_ackVisitor() { }

Visitor * add_party_ackVisitor::dup(void) const
{ return new add_party_ackVisitor(*this); }

const vistype & add_party_ackVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType add_party_ackVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// -------------------------------------------------------------------
bool add_party_rejVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == add_party_rej_req)
    rval = c->GetCS()->AddPartyRejReq(this,c);  
  else
    rval = c->GetCS()->AddPartyRejInd(this,c);  
  return rval;
}

add_party_rejVisitor::add_party_rejVisitor(generic_q93b_msg *msg, 
					   Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

add_party_rejVisitor::add_party_rejVisitor(const add_party_rejVisitor & rhs) :
  Q93bVisitor(rhs) { }

add_party_rejVisitor::~add_party_rejVisitor() { }

Visitor * add_party_rejVisitor::dup(void) const
{ return new add_party_rejVisitor(*this); }

const vistype & add_party_rejVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType add_party_rejVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// --------------------------------------------------------------------
bool drop_partyVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == drop_party_req)
    rval = c->GetCS()->DropPartyReq(this,c);  
  else
    rval = c->GetCS()->DropPartyInd(this,c);  
  return rval;
}

drop_partyVisitor::drop_partyVisitor(generic_q93b_msg *msg, 
				     Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

drop_partyVisitor::drop_partyVisitor(const drop_partyVisitor & rhs) :
  Q93bVisitor(rhs) { }

drop_partyVisitor::~drop_partyVisitor() { }

Visitor * drop_partyVisitor::dup(void) const
{ return new drop_partyVisitor(*this); }

const vistype & drop_partyVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType drop_partyVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// ---------------------------------------------------------------------
bool drop_party_compVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == drop_party_comp_req)
    rval = c->GetCS()->DropPartyCompReq(this,c);  
  else
    rval = c->GetCS()->DropPartyCompInd(this,c);  
  return rval;
}

drop_party_compVisitor::drop_party_compVisitor(generic_q93b_msg *msg, 
					       Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

drop_party_compVisitor::drop_party_compVisitor(const drop_party_compVisitor & rhs) :
  Q93bVisitor(rhs) { }

drop_party_compVisitor::~drop_party_compVisitor() { }

Visitor * drop_party_compVisitor::dup(void) const
{ return new drop_party_compVisitor(*this); }

const vistype & drop_party_compVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType drop_party_compVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// ----------------------------------------------------------------------
bool drop_party_ackVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == drop_party_ack_req)
    rval = c->GetCS()->DropPartyAckReq(this,c);  
  else
    rval = c->GetCS()->DropPartyAckInd(this,c);  
  return rval;
}

drop_party_ackVisitor::drop_party_ackVisitor(generic_q93b_msg *msg, 
					     Q93bVisitorType vt) : 
  Q93bVisitor(_my_type, msg,vt) { }

drop_party_ackVisitor::drop_party_ackVisitor(const drop_party_ackVisitor & rhs) :
  Q93bVisitor(rhs) { }

drop_party_ackVisitor::~drop_party_ackVisitor() { }

Visitor * drop_party_ackVisitor::dup(void) const
{ return new drop_party_ackVisitor(*this); }

const vistype & drop_party_ackVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType drop_party_ackVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// -----------------------------------------------------------------------
bool leaf_setupVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == leaf_setup_req)
    rval = c->GetCS()->LeafSetupReq(this,c);  
  else
    rval = c->GetCS()->LeafSetupInd(this,c);  
  return rval;
}

leaf_setupVisitor::leaf_setupVisitor(generic_q93b_msg *msg, 
				     Q93bVisitorType vt) : 
  Q93bVisitor(_my_type, msg,vt) { }

leaf_setupVisitor::leaf_setupVisitor(const leaf_setupVisitor & rhs) :
  Q93bVisitor(rhs) { }

leaf_setupVisitor::~leaf_setupVisitor() { }

Visitor * leaf_setupVisitor::dup(void) const
{ return new leaf_setupVisitor(*this); }

const vistype & leaf_setupVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType leaf_setupVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// -----------------------------------------------------------------------
bool restartVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == restart_req)
    rval = c->GetCS()->RestartReq(this,c);  
  else
    rval = c->GetCS()->RestartInd(this,c);  
  return rval;
}

restartVisitor::restartVisitor(generic_q93b_msg *msg, Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

restartVisitor::restartVisitor(const restartVisitor & rhs) :
  Q93bVisitor(rhs) { }

restartVisitor::~restartVisitor() { }

Visitor * restartVisitor::dup(void) const
{ return new restartVisitor(*this); }

const vistype & restartVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType restartVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// -----------------------------------------------------------------------
bool restart_respVisitor::CallProtocol(Call *c) 
{  
  return c->GetCS()->RestartResp(this,c);  
}

restart_respVisitor::restart_respVisitor(generic_q93b_msg *msg, 
					 Q93bVisitorType vt) : 
  Q93bVisitor(_my_type, msg,vt) { }

restart_respVisitor::restart_respVisitor(const restart_respVisitor & rhs) :
  Q93bVisitor(rhs) { }

restart_respVisitor::~restart_respVisitor() { }

Visitor * restart_respVisitor::dup(void) const
{ return new restart_respVisitor(*this); }

const vistype & restart_respVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType restart_respVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

// -----------------------------------------------------------------------
bool restart_ackVisitor::CallProtocol(Call *c) 
{  
  bool rval = false;

  if (GetVT() == restart_ack_req)
    rval = c->GetCS()->RestartAckReq(this,c);  
  else
    rval = c->GetCS()->RestartAckInd(this,c);  
  return rval;
}

restart_ackVisitor::restart_ackVisitor(generic_q93b_msg *msg, 
				       Q93bVisitorType vt) :
  Q93bVisitor(_my_type, msg,vt) { }

restart_ackVisitor::restart_ackVisitor(const restart_ackVisitor & rhs) :
  Q93bVisitor(rhs) { }

restart_ackVisitor::~restart_ackVisitor() { }

Visitor * restart_ackVisitor::dup(void) const
{ return new restart_ackVisitor(*this); }

const vistype & restart_ackVisitor::GetClassType(void) const
{  return _my_type;  }

const VisitorType restart_ackVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

