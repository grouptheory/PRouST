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
static char const _Handlers_cc_rcsid_[] =
"$Id: Handlers.cc,v 1.4 1999/03/04 19:29:04 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/kernel/Handlers.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include <FW/actors/Creator.h>
#include <FW/kernel/SimEntity.h>
#include <FW/kernel/SimEvent.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/KDB.h>
#include <iostream.h>

// ----------------------- Handler -------------------------

Handler::Handler(void) : _owner(0) {  }

Handler::Handler(SimEntity * owner) : _owner(owner)  {  }

Handler::~Handler()
{ 
  if (IsRegistered()) Deregister(); 
}

bool Handler::Deregister(void)
{
  bool res = false;

  if ((IsRegistered()) && (! (res = theKernel().Deregister(_token)))) {
    DIAG("FW.kernel", DIAG_ERROR, cerr << "ERROR: Unable to deregister Handler with Kernel." << endl;);
  }
  return res;
}

void Handler::KernelStop(void) { theKernel().Stop(); }
void Handler::KernelRun(void)  { theKernel().Run(); }
			    
bool Handler::IsRegistered(void) const { return theKernel().IsRegistered(_token); }

void Handler::SetToken(KernelToken kt) { _token = kt; }

void Handler::PassVisitorToA(Visitor * v)
{
  assert(_owner);

  switch (_owner->GetType()) {
    case SimEntity::STATE:
      ((State *)_owner)->PassVisitorToA(v);
      break;
    case SimEntity::TERMINAL:
      ((Terminal *)_owner)->Inject(v);
      break;
    case SimEntity::CREATOR:
      ((Creator *)_owner)->PassVisitorToA(v);
      break;
    default:
      v->Suicide();
      break;
  }
}

void Handler::PassVisitorToB(Visitor * v)
{
  assert(_owner);


  switch (_owner->GetType()) {
    case SimEntity::STATE:
      ((State *)_owner)->PassVisitorToB(v);
      break;
    case SimEntity::CREATOR:
      ((Creator *)_owner)->PassVisitorToB(v);
      break;
    default:
      v->Suicide();
      break;
  }
}

void Handler::PassVisitorToSelf(Visitor * v)
{
  assert(_owner);

  switch (_owner->GetType()) {
    case SimEntity::STATE:
      ((State *)_owner)->Handle(v);
      break;
    case SimEntity::TERMINAL:
      ((Terminal *)_owner)->Absorb(v);
      break;
    case SimEntity::CREATOR:
      ((Creator *)_owner)->Create(v);
      break;
    default:
      v->Suicide();
      break;
  }
}

Visitor::which_side Handler::VisitorFrom(Visitor * v)
{
  assert(_owner);
  Visitor::which_side rval = Visitor::OTHER;

  switch (_owner->GetType()) {
    case SimEntity::STATE:
      rval = ((State *)_owner)->VisitorFrom(v);
      break;
    case SimEntity::TERMINAL:
      rval = Visitor::A_SIDE;
      break;
    case SimEntity::CREATOR:
      rval = ((Creator *)_owner)->VisitorFrom(v);
      break;
    default:
      rval = Visitor::OTHER;
      break;
  }
  return rval;
}

bool Handler::Register(TimerHandler * t)
{ 
  if (!t) return false;
  return t->_Register(); 
}

bool Handler::Register(InputHandler * i)
{
  if (!i) return false;
  if (i->IsRegistered()) i->Deregister();
  return theKernel().Register(i);
}

bool Handler::Register(OutputHandler * o)
{
  if (!o) return false;
  if (o->IsRegistered()) o->Deregister();
  return theKernel().Register(o);
}

bool Handler::Register(SimEvent* e)
{
  if (!e) return false;
  if (e->IsRegistered()) e->Deregister();
  return theKernel().Register(e);
}

bool Handler::Cancel(Handler * h)
{
  if (!h) return false;
  if (h->IsRegistered())
    return h->Deregister();
  return true;
}

bool Handler::Cancel(SimEvent * e)
{
  if (!e) return false;
  if (e->IsRegistered())
    return e->Deregister();
  return true;
}

#define TIMED_DELIVERY_OVERRIDE -1

// ----------------------- TimerHandler -------------------------
TimerHandler::TimerHandler(double timefromnow, int override) 
  : Handler() , _timefromcreation(timefromnow), _expiry(0), _kdb_timer_id(override)
{ 
  if (_kdb_timer_id != TIMED_DELIVERY_OVERRIDE)
    _kdb_timer_id = theKernel().KDB()->Report(this); 
}

TimerHandler::TimerHandler(SimEntity * o, double timefromnow) 
  : Handler(o) , _timefromcreation(timefromnow), _expiry(0) { 
    _kdb_timer_id = theKernel().KDB()->Report(this); }

TimerHandler::~TimerHandler() 
{ 
  if (_kdb_timer_id != TIMED_DELIVERY_OVERRIDE) {
    if (theKernel().KDB()->Resign(_kdb_timer_id, this) == 0) {
      BPT * b = new BPT(this,BPT::TimerDestruction);
      int is_breakpoint = (theKernel().KDB()->Notify(b));
      delete b;
      if (is_breakpoint) {
	ALERT_DEBUGGER;
      }
    }
  }
}

KernelTime TimerHandler::GetExpirationTime(void) const { return _expiry; }

void TimerHandler::SetExpirationTime(KernelTime kt) { _expiry = kt; }

void TimerHandler::SetExpirationTime(void)
{ _expiry = theKernel().Future(_timefromcreation); }

void TimerHandler::ExpiresIn(double timefromnow)
{
  if (IsRegistered()) Deregister();
  _timefromcreation = timefromnow;  // Changed 4/2/98 - mountcas
}

bool TimerHandler::_Register(void)
{
  if (IsRegistered()) Deregister();
  SetExpirationTime();  // Changed 4/2/98 - mountcas
  return theKernel().Register(this);
}

void TimerHandler::Print(ostream & os)
{ os << "TimerHandler " << _token << " Expires at " << _expiry << endl; }

bool TimerHandler::IsInterruptMessenger(void) { return false; }

// ---------------------- InputHandler -----------------------------
InputHandler::InputHandler(SimEntity * o, int fd) 
  : Handler(o), _fd(fd) {  _kdb_input_id = theKernel().KDB()->Report(this); }

InputHandler::~InputHandler() 
{  
  if (theKernel().KDB()->Resign(_kdb_input_id, this) == 0) {
    BPT * b = new BPT(this,BPT::InputDestruction);
    int is_breakpoint = (theKernel().KDB()->Notify(b));
    delete b;
    if (is_breakpoint) {
      ALERT_DEBUGGER;
    }
  }
}

int InputHandler::GetFD(void) const
{ return _fd; }

bool InputHandler::_Register(void)
{
  if (IsRegistered()) Deregister();
  return theKernel().Register(this);
}

void InputHandler::Print(ostream & os)
{ os << "InputHandler " << _token << " on descriptor " << _fd << endl; }

// -------------------------- OutputHandler ---------------------
OutputHandler::OutputHandler(SimEntity * o, int fd) 
  : Handler(o), _fd(fd) { _kdb_output_id = theKernel().KDB()->Report(this); }

OutputHandler::~OutputHandler() 
{ 
  if (theKernel().KDB()->Resign(_kdb_output_id, this) == 0) {
    BPT * b = new BPT(this,BPT::OutputDestruction);
    int is_breakpoint = (theKernel().KDB()->Notify(b));
    delete b;
    if (is_breakpoint) {
      ALERT_DEBUGGER;
    }
  }
}

int OutputHandler::GetFD(void) const {  return _fd; }

bool OutputHandler::_Register(void) 
{  
  if (IsRegistered()) Deregister();
  return theKernel().Register(this); 
}

void OutputHandler::Print(ostream & os)
{ os << "OutputHandler " << _token << " on descriptor " << _fd << endl; }

// ----------------------- TimedDeliveryHandler -------------------------
TimedDeliveryHandler::TimedDeliveryHandler(SimEntity * owner, 
					   SimEvent * se, 
					   double timefromnow) 
  : TimerHandler(timefromnow, TIMED_DELIVERY_OVERRIDE), _se(se), _own(owner) 
{ _kdb_interrupt_id = theKernel().KDB()->Report(this); }
  
TimedDeliveryHandler::~TimedDeliveryHandler() 
{  
  if (theKernel().KDB()->Resign(_kdb_interrupt_id,this) == 0) {
    BPT * b = new BPT(this,BPT::InterruptDestruction);
    int is_breakpoint = (theKernel().KDB()->Notify(b));
    delete b;
    if (is_breakpoint) {
      ALERT_DEBUGGER;
    }
  }
}

void TimedDeliveryHandler::Callback(void)
{
  assert(_own);

  BPT * b = new BPT(_se->_to,BPT::SimEntityInterruption, this);
  int is_breakpoint=theKernel().KDB()->Notify(b);
  delete b;

  if (is_breakpoint) {
    ALERT_DEBUGGER;
  }
  _own->_Deliver(_se);

  _own->UnhookMe(this);
  delete this;
}

void TimedDeliveryHandler::Print(ostream & os)
{ os << "TimedDeliveryHandler with event " << *(_se) << " Expires at " << GetExpirationTime() << endl; }

bool TimedDeliveryHandler::IsInterruptMessenger(void) { return true; }

// --------------- StopTimer -------------------
StopTimer::StopTimer(double time) : TimerHandler(time) { }

StopTimer::~StopTimer() { }

void StopTimer::Callback(void) { KernelStop(); }

// ------------------ XXXTimer -----------------------------
XXXTimer::XXXTimer(double time) : TimerHandler(time) { theKernel().DisableXXX(); }

XXXTimer::~XXXTimer() { }

void XXXTimer::Callback(void) 
{ 
  theKernel().AppendXXXToRecord();
}

// -------------------- QueueTimer ------------------------
QueueTimer::QueueTimer(State * s, double time) :
  TimerHandler(s, time), _s(s) 
{ 
  assert(_s); 
}

QueueTimer::~QueueTimer() {  }

void QueueTimer::Callback(void)
{
  bool aside = _s->_toA.empty(),
       bside = _s->_toB.empty();

  if (!aside) {
    Visitor * v = _s->_toA.pop();
    _s->QPassVisitorToA(v);
  }

  if (!bside) {
    Visitor * v = _s->_toB.pop();
    _s->QPassVisitorToB(v);
  }

  if ( _s->_toA.empty() && _s->_toB.empty() )
    Cancel(this);
  else
    Register(this);
}

