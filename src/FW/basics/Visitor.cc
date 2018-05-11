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
static char const _Visitor_cc_rcsid_[] =
"$Id: Visitor.cc,v 1.1 1999/01/21 14:56:25 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/basics/Visitor.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/Log.h>
#include <FW/basics/diag.h>

#include <FW/behaviors/Behavior.h>
#include <FW/behaviors/Mux.h>
#include <FW/behaviors/Factory.h>
#include <FW/behaviors/Adapter.h>
#include <FW/behaviors/Protocol.h>
#include <FW/behaviors/Cluster.h>

#include <FW/actors/Accessor.h>
#include <FW/actors/Creator.h>
#include <FW/actors/Terminal.h>
#include <FW/actors/State.h>
#include <FW/actors/Expander.h>
#include <FW/basics/Registry.h>

#include <FW/kernel/Kernel.h>
#include <FW/kernel/SimEvent.h>
#include <FW/kernel/KDB.h>

extern "C" {
#include <assert.h>
};

// --------------------- Visitor ------------------------------
Registry * Visitor::_registry = 0;

Visitor::Visitor(void) 
  : _ref_count(1), _current_conduit(0), _last(0), _current_behavior(0), 
    _current_actor(0), _origin(0), _ret_event(0), _logging_on(false), _log(0), 
    // Added below 5-28-98 - mountcas
    _kissofdeath(false), _conduit_type(UNKNOWN), _good(false)
{ 
  SetLoggingOn(); 
  _kdb_visitor_id = theKernel().KDB()->Report(this);
}

Visitor::Visitor(const Visitor & rhs) 
  : _ref_count(1), _current_conduit(rhs._current_conduit), _last(0), 
    _current_behavior(rhs._current_behavior), _current_actor(rhs._current_actor), 
    _origin(0), _ret_event(0), _logging_on(rhs._logging_on), _log(0),
    // Added below 5-28-98 - mountcas
    _kissofdeath(false), _conduit_type(UNKNOWN), _good(false)
{ 
  _kdb_visitor_id = theKernel().KDB()->Report(this);
  if (_current_conduit)
    SetLast( _current_conduit );
}

Visitor::Visitor(vistype & child_type) 
  : _ref_count(1), _current_conduit(0), _last(0), _current_behavior(0), 
    _current_actor(0), _origin(0), _ret_event(0), _logging_on(false), _log(0),
    // Added below 5-28-98 - mountcas
    _kissofdeath(false), _conduit_type(UNKNOWN), _good(true)
{ 
  child_type.derived_from(_my_type); 
  _kdb_visitor_id = theKernel().KDB()->Report(this);
}

void Visitor::Suicide(void) 
{
  if (_conduit_type == FACTORY) {
    _kissofdeath = true;
    return;
  }
    
  // Log the Visitor's death.
  char buf[Conduit::MAX_LOG_SIZE];
  VisitorType vt = GetType();

  if (CurrentConduit()) {
    char subtype[50];
    Fill_Subtype_Name(subtype);
    if (strcmp(subtype,"")==0)
      sprintf(buf, "!death %s:%d !Visitor !in %s", vt.Name(),
	      _kdb_visitor_id, 
	      (CurrentConduit() ? CurrentConduit()->GetName() : "Unknown"));
    else
      sprintf(buf, "!death %s:%d !Visitor !in %s", subtype,
	      _kdb_visitor_id, 
	      (CurrentConduit() ? CurrentConduit()->GetName() : "Unknown"));
    theLog().AppendToRecord(buf);
  }
  
  on_death();

  if (--_ref_count == 0)
    delete this;
}

Visitor::~Visitor( )
{
  // Clean out the Visitor's log (of its path)
  if (_log) {
    if (!_log->empty()) {
      list_item li;
      forall_items(li, * _log)
	delete [] _log->inf(li);
    }
    _log->clear();
    delete _log;
    _log = 0;
  }
  // Free the return event if present
  if (_ret_event)
    delete _ret_event;

  // Notify KDB that this Visitor is dying
  if (theKernel().KDB()->Resign(_kdb_visitor_id, this) == 0) {
    BPT* b = new BPT(this,BPT::VisitorDestruction);
    int is_breakpoint = (theKernel().KDB()->Notify(b));
    delete b;
    if (is_breakpoint) {
      ALERT_DEBUGGER;
    }
  }
}

Visitor::which_side Visitor::EnteredFrom(void)
{
  if (_last == _current_behavior->OwnerSideA())
    return Visitor::A_SIDE;
  if (_last == _current_behavior->OwnerSideB())
    return Visitor::B_SIDE;
  return Visitor::OTHER;
}

Visitor::behavior_t Visitor::ConduitType(void) const
{
  return _conduit_type;
}

Conduit * Visitor::GetLast(void) const  { return _last; }

void Visitor::SetLast(Conduit * c) 
{
  if (!_last && c) {
    // Log birth of Visitor.
    char buf[Conduit::MAX_LOG_SIZE];

    CurrentConduit(c);

    VisitorType vt = GetType();
    char subtype[50];
    Fill_Subtype_Name(subtype);
    if (!*subtype)
      sprintf(buf, "!birth %s:%d !Visitor !in %s", vt.Name(), 
	      _kdb_visitor_id, c->GetName());
    else
      sprintf(buf, "!birth %s:%d !Visitor !in %s", subtype,
	      _kdb_visitor_id, c->GetName());
    theLog().AppendToRecord(buf);
  }
  _last = c; 
}

Conduit * Visitor::CurrentConduit(void) const 
{ return _current_conduit; }

void Visitor::CurrentConduit(Conduit * c) 
{ 
  _current_conduit  = c; 
  _current_behavior = (c ? c->_behavior : 0); 
}

Conduit * Visitor::SideA(void) 
{ 
  return (_current_behavior ? _current_behavior->OwnerSideA() : 0); 
}

Conduit * Visitor::SideB(void) 
{ 
  return (_current_behavior ? _current_behavior->OwnerSideB() : 0); 
}

void UpdateProtocol(Protocol * p, State * s)
{
  assert(p);
  p->UpdateState(s);
}

void Visitor::at(Adapter * a, Terminal * t)
{
  assert(a && t);

  switch (EnteredFrom()) {
    case A_SIDE:
      t->Absorb(this);
      break;
    case B_SIDE:
    case OTHER:  // foreigner
      t->Inject(this);
      break;
  }
}

void Visitor::at(Behavior * b)
{
  // Added 5-28-98 - mountcas
  if (_kissofdeath) {
    _conduit_type = UNKNOWN;
    Suicide();
    return;
  }

  Conduit * dest;

  assert(b);

  switch (EnteredFrom()) {
    case A_SIDE:
      SetLast( CurrentConduit() );
      dest = SideB();
      break;
    case B_SIDE:
    case OTHER:  // foreigner
      SetLast( CurrentConduit() );
      dest = SideA();
      break;
  }

  if (dest)
    dest->Accept(this);
  else
    Suicide();
}

void Visitor::at(Mux * m, Accessor *a)
{
  Conduit * dest;

  assert(m && a);

  switch (EnteredFrom()) {
    case A_SIDE:
      SetLast( CurrentConduit() );
      if (dest = a->GetNextConduit(this))
	dest->Accept(this);
      else {
	if (dest = SideB()) 
	  dest->Accept(this);
	else 
	  Suicide();
      }
      break;
    case B_SIDE:
    case OTHER:
      at(m);
      break;
  }
}

Conduit * Visitor::at(Factory * f, Creator * c)
{
  Conduit * a = SideA(), * b = SideB();
  
  assert(f && c);

  Conduit * newc = 0;
  if (a && b) {
    if ((newc = c->Create(this)) == 0)      
      at(f);  // call at(Behavior) since no Conduit was created.
    else {
      if (Join(B_half(a), A_half(newc), this, this)) {
	if (Join(B_half(b), B_half(newc), this, this)) {
	  if (_kissofdeath) {  
	    // If the creator tried to kill us get depressed and commit suicide
	    _conduit_type = UNKNOWN;
	    Suicide();
	  } else {
	    switch (EnteredFrom()) {
	      case A_SIDE:
		A_half(newc)->Accept(this);
		break;
	      case B_SIDE:
		B_half(newc)->Accept(this);
		break;
	      case OTHER:
		break;
	    }
	  }
	} else {
	  diag("FW", DIAG_WARNING, "Join failed for Conduit %s.\n", newc->GetName());
	  delete newc; newc = 0;
	  Suicide();
	}
      } else {
	diag("FW", DIAG_WARNING, "Join failed for Conduit %s.\n", newc->GetName());
	delete newc; newc = 0;
	Suicide();
      } // newc is all joined up and ready to go ...
    }
  } else 
    Suicide();

  return newc;
}

void Visitor::at(Protocol * p, State * s)
{
  assert(p && s);

  if (s) // Protocol now points to new state
    UpdateProtocol(p, s->Handle(this));
  else 
    at(p);
}

void Visitor::LogMyLocation(void)
{
  list_item li;

  if (!_logging_on)
    return;

  if (!_log)
    _log = new list<char *>;

  if (li = _log->last())
    if (!strcmp(_log->contents(li), _current_conduit->GetName())) 
      return;

  char * temp = new char [ strlen(_current_conduit->GetName()) + 1 ];
  strcpy(temp, _current_conduit->GetName());
  _log->append(temp);
}

inline void Visitor::SaveLocation(Behavior* b,Actor* a) 
{ 
  _current_behavior = b; _current_actor = a; 
  _current_conduit = b->GetOwner();
}

void Visitor::_at(Mux * m, Accessor * a) 
{ 
  assert(_good == true);
  // Added 5-28-98 - mountcas
  _conduit_type = MUX;
  SaveLocation(m, a); 
  if (_logging_on) 
    LogMyLocation();
  at(m,a); 
}

void Visitor::_at(Factory * f, Creator * c) 
{ 
  assert(_good == true);
  // Added 5-28-98 - mountcas
  _conduit_type = FACTORY;
  SaveLocation(f, c); 
  if (_logging_on) 
    LogMyLocation();

  Conduit * newc = at(f,c); 
  if (newc && newc->_registered == false) {
    cerr << "ERROR:  You must call Creator::Register on Conduits make in Creator::Create" << endl;
    assert(newc->_registered);
  }
}

void Visitor::_at(Protocol * p, State * s) 
{ 
  assert(_good == true);
  // Added 5-28-98 - mountcas
  _conduit_type = PROTOCOL;
  SaveLocation(p,s); 
  if (_logging_on) 
    LogMyLocation();
  at(p, s); 
}

void Visitor::_at(Adapter * a, Terminal * t) 
{ 
  assert(_good == true);
  // Added 5-28-98 - mountcas
  _conduit_type = ADAPTER;
  SaveLocation(a,t); 
  if (_logging_on) 
    LogMyLocation();
  at(a,t); 
}

inline void Visitor::_at(Behavior * b) 
{ 
  assert(_good == true);
  // Added 5-28-98 - mountcas
  _conduit_type = UNKNOWN;
  SaveLocation(b,0); 
  if (_logging_on) 
    LogMyLocation();
  at(b);   
}

void Visitor::CleanLog(void)
{
  if (_log && (_log->empty() == false)) {
    list_item li;
    forall_items(li, * _log) {
      delete [] _log->inf(li);
      _log->assign(li, 0);
    }
    _log->clear();
  }
}

bool Visitor::DumpLog(ostream & os) const
{
  if (!_log || _log->size() == 0) 
    return false;

  VisitorType vt = GetType();
  char subtype[50];
  Fill_Subtype_Name(subtype);

  if (!*subtype)
    os << vt.Name() << " Log ";
  else
    os << subtype << " Log ";

  bool first = true;
  list_item li;
  forall_items(li, * _log) {
    char * str = _log->inf(li);
    if (first) { 
      os << "{ " << endl << "  "; first = false; 
    } else { 
      os << "  " << flush; 
    }
    os << str << "," << endl;
  }
  if (!first) 
    os << " }" << flush;

  return true;
}

Visitor & Visitor::operator = (const Visitor & rhs)
{
  _last = rhs._last;

  _logging_on = rhs._logging_on;

  _current_conduit  = rhs._current_conduit;
  _current_behavior = rhs._current_behavior;
  _current_actor    = rhs._current_actor;
  
  _origin = 0;
  _ret_event = 0;

  return *this;
}

void Visitor::SetOrigin(SimEntity * e)
{
  _origin = e;
}

SimEntity* Visitor::GetOrigin(void)
{
  return _origin;
}

void Visitor::SetReturnEvent(SimEvent * e)
{
  _ret_event = e;
}

SimEvent * Visitor::GetReturnEvent(void)
{
  SimEvent * se = _ret_event;
  _ret_event = 0;
  return se;
}

void Visitor::SetLoggingOn(void)
{
  _logging_on = true;
}

void Visitor::SetLoggingOff(void)
{
  _logging_on = false;
}

const VisitorType Visitor::GetType(void) const 
{ 
  return VisitorType(GetClassType());
}

Visitor * Visitor::duplicate(void) const
{
  Visitor * rval = dup();
  VisitorType vt = GetType();
  if (!(rval->GetType().Is_Exactly( &vt ))) {
    rval->Suicide();
    rval = 0;
  } else
    rval->_good = true;
  return rval;
}

void Visitor::on_death(void) const 
{ 
  // cout << "Aaaiiiieeeeeeyyyyyoooooohhhhh!" << endl;  // Blood-curdling death cry
}

const vistype & Visitor::GetClassType(void) const { return _my_type; }

bool Visitor::EmptyLog(void) const 
{ if (!_log->size()) return (bool)true; return (bool)false; }

void Visitor::Fill_Subtype_Name(char* buf) const {
  sprintf(buf,"");
}
