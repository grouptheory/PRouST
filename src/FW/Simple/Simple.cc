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
static char const _Simple_cc_rcsid_[] =
"$Id: Simple.cc,v 1.29 1998/08/13 20:11:57 mountcas Exp $";
#endif
#include <common/cprototypes.h>
// -*- C++ -*-
#include "Simple.h"

//
#include <FW/basics/Conduit.h>
#include <FW/behaviors/Mux.h>
#include <FW/behaviors/Factory.h>
#include <FW/behaviors/Protocol.h>
#include <FW/behaviors/Adapter.h>
#include <FW/kernel/SimEvent.h>
//

#include <iostream.h>

// ---------------------------- SimpleTerm ---------------------------------
SimpleTerm::SimpleTerm(void) : Terminal() { }
SimpleTerm::~SimpleTerm() { }

void SimpleTerm::Absorb(Visitor * v)
{
  cout << OwnerName() << " has absorbed a " << v->GetType() << " (" << v << ")." << endl;
  // Kill it
  v->Suicide();
}

void SimpleTerm::Interrupt(SimEvent * s)
{
  cout << "Gah!  " << OwnerName() << " was interrupted by a SimEvent with code " << s->GetCode() << endl;
}

// Registers a TimerHandler (see Handlers.h)
void SimpleTerm::RegTimer(TimerHandler * t) { Register(t); }
// Registers an InputHandler (see Handlers.h)
void SimpleTerm::RegInput(InputHandler * i) { Register(i); }
// Registers an OutputHandler (see Handlers.h)
void SimpleTerm::RegOutput(OutputHandler * o) { Register(o); }

// ---------------------------- SimpleAccessor ----------------------------------
VisitorType * SimpleAccessor::_simple_vistype = 0;

SimpleAccessor::SimpleAccessor(void) : Accessor() 
{ 
  if (!_simple_vistype)
    _simple_vistype = (VisitorType *)QueryRegistry(SIMPLE_VISITOR_NAME);
}

SimpleAccessor::~SimpleAccessor() { }

Conduit * SimpleAccessor::GetNextConduit(Visitor * v)
{
  Conduit * rval = 0;

  if (v->GetType().Is_A(_simple_vistype)) {
    SimpleVisitor * sv = (SimpleVisitor *)v;
    int i = sv->GetKey();

    dic_item di;
    if (di = _int2Conduit.lookup(i))
      rval = _int2Conduit.inf(di);
  }
  return rval;
}

// This will broadcast the Visitor to all the 
// Conduits attached to the 'wide' side of the mux.
bool SimpleAccessor::Broadcast(Visitor * v)
{
  dic_item di;
  forall_items(di, _int2Conduit) {
    Conduit * c = _int2Conduit.inf(di);
    c->Accept(v->duplicate());
  }
  // Kill the original since copies went out
  v->Suicide();
  return true;
}

// Adds the specified Conduit to its dictionary.
bool SimpleAccessor::Add(Conduit * c, Visitor * v)
{
  if (v->GetType().Is_A(_simple_vistype)) {
    SimpleVisitor * sv = (SimpleVisitor *)v;
    int i = sv->GetKey();

    dic_item di;
    if (!(di = _int2Conduit.lookup(i))) {
      _int2Conduit.insert(i, c);
      return true;
    }
  }
  return false;
}

// Removes the specified Conduit from its dictionary.
bool SimpleAccessor::Del(Conduit * c)
{
  dic_item di;
  forall_items(di, _int2Conduit) {
    if (c == _int2Conduit.inf(di)) {
      _int2Conduit.del_item(di);
      return true;
    }
  }
  return false;
}

// Removes a specific Conduit by using the Vistor's key.
bool SimpleAccessor::Del(Visitor * v)
{ 
  if (v->GetType().Is_A(_simple_vistype)) {
    SimpleVisitor * sv = (SimpleVisitor *)v;
    dic_item di;
    int key = sv->GetKey();
    if (di = _int2Conduit.lookup(key)) {
      _int2Conduit.del_item(di);
      return true;
    }
  }
  return false;
}

// -------------------------- SimpleCreator ------------------------------
VisitorType * SimpleCreator::_simple_vistype = 0;
VisitorType * SimpleCreator::_not_so_simple_vistype = 0;

// Constructor
SimpleCreator::SimpleCreator(int mc) : Creator(), _makeClusters(mc)
{ 
  if (!_simple_vistype)
    _simple_vistype = (VisitorType *)QueryRegistry(SIMPLE_VISITOR_NAME);
  if (!_not_so_simple_vistype)
    _not_so_simple_vistype = (VisitorType *)QueryRegistry(NOT_SO_SIMPLE_VISITOR_NAME);
}

// Destructor
SimpleCreator::~SimpleCreator() { }

// Allocates either a SimpleExpander or SimpleState
Conduit * SimpleCreator::Create(Visitor * v)
{
  char buf[128];
  Conduit * rval = 0;

  if (v->GetType().Is_A(_simple_vistype)) {
    SimpleVisitor * sv = (SimpleVisitor *)v;
    if (_makeClusters > 0) {
      SimpleExpander * se = new SimpleExpander(--_makeClusters);
      Cluster * c = new Cluster(se);
      sprintf(buf, "SimpleExpander-%d", sv->GetKey());
      rval = new Conduit(buf, c);
      cout << OwnerName() << " created " << rval->GetName() << endl;
    } else {
      SimpleState * sp = new SimpleState();
      Protocol * p = new Protocol(sp);
      sprintf(buf, "SimpleState-%d", sv->GetKey());
      rval = new Conduit(buf, p);
    }
    //    cout << OwnerName() << " building a new Conduit (" << buf << ")." << endl;
    // This enables the parent Creator destructor to clean up after it
    Register(rval);
  } else if (v->GetType().Is_A(_not_so_simple_vistype)) {
    v->Suicide();
  }
  return rval;
}

void SimpleCreator::Interrupt(SimEvent * s)
{
  cout << "Ack!  " << OwnerName() << " was interrupted by a SimEvent with code " << s->GetCode() << endl;
}

// ---------------------------- SimpleState --------------------------------
SimpleState::SimpleState(void) : State(true) { _ct=0; }
SimpleState::~SimpleState() { }

// this is usually where the main fsm logic resides.
// the SimpleState currently just passes through all Visitors
State * SimpleState::Handle(Visitor * v)
{
  cout << OwnerName() << " received a " << v->GetType() << " (" << v << ") passing it on." << endl;
  _ct++;
  if (_ct>2) 
    Free();
  PassThru(v);
  return this;
}

void SimpleState::Interrupt(SimEvent * s)
{
  cout << "Argh!  " << OwnerName() << " was interrupted by a SimEvent with code " << s->GetCode() << endl;
}

// ----------------------------- SimpleExpander -------------------------------
SimpleExpander::SimpleExpander(int i) : Expander(), _recursive(i)
{ 
  // This Expander basically contains the whole example1 (see example1.fig)
  SimpleState * sp = new SimpleState();
  Protocol * p = new Protocol(sp);
  _upper_proto = new Conduit("UpperState", p);

  SimpleAccessor * sa = new SimpleAccessor();
  Mux * m = new Mux(sa);
  _upper_mux = new Conduit("UpperMux", m);

  SimpleCreator * sc = new SimpleCreator(--_recursive);
  Factory * f = new Factory(sc);
  _factory = new Conduit("Factory", f);

  sa = new SimpleAccessor();
  m  = new Mux(sa);
  _lower_mux = new Conduit("LowerMux", m);

  sp = new SimpleState();
  p  = new Protocol(sp);
  _lower_proto = new Conduit("LowerState", p);

  // Now join them all together
  if (!Join(B_half(_upper_proto), A_half(_upper_mux))) {
    cout << "The Joining of the " << _upper_proto->GetName() << " and the "
         << _upper_mux->GetName() << " has failed." << endl;
    abort();
  }
  if (!Join(B_half(_upper_mux), A_half(_factory))) {
    cout << "The Joining of the " << _upper_mux->GetName() << " and the "
         << _factory->GetName() << " has failed." << endl;
    abort();
  }
  if (!Join(B_half(_factory), B_half(_lower_mux))) {
    cout << "The Joining of the " << _lower_mux->GetName() << " and the "
         << _factory->GetName() << " has failed." << endl;
    abort();
  }
  if (!Join(A_half(_lower_mux), A_half(_lower_proto))) {
    cout << "The Joining of the " << _lower_mux->GetName() << " and the "
         << _lower_proto->GetName() << " has failed." << endl;
    abort();
  }

  DefinitionComplete();
}

// Destructor
SimpleExpander::~SimpleExpander() 
{ 
  delete _upper_proto;
  delete _upper_mux;
  delete _factory;
  delete _lower_mux;
  delete _lower_proto;
}

// returns Conduit that is the A_SIDE of the Expander (in this case the UpperState)
Conduit * SimpleExpander::GetAHalf(void) const
{
  return A_half(_upper_proto);
}

// returns Conduit that is the B_SIDE of the Expander (in this case the LowerState)
Conduit * SimpleExpander::GetBHalf(void) const
{
  return B_half(_lower_proto);
}

// ------------------------------- SimpleVisitor ----------------------------------
// this sets up the vistype properly.
vistype SimpleVisitor::_my_type(SIMPLE_VISITOR_NAME);

// Constructor, passes in vistype, so the hierarchy can be established via ctor calls.
SimpleVisitor::SimpleVisitor(int i) : Visitor(_my_type), _key(i) { }

// Derived ctor
SimpleVisitor::SimpleVisitor(vistype & child_type) : 
  Visitor(child_type.derived_from(_my_type)), _key(-1) { }

// Destructor
SimpleVisitor::~SimpleVisitor( ) { }

// returns the key
const int SimpleVisitor::GetKey(void) const { return _key; }

// sets the key
void SimpleVisitor::SetKey(int k) { _key = k; }

// returns the vistype for SimpleVisitor encapsulated in a VisitorType
const VisitorType SimpleVisitor::GetType(void) const { return VisitorType(_my_type); }

// returns the vistype for the SimpleVisitor (private or protected);
const vistype & SimpleVisitor::GetClassType(void) const { return _my_type; }

// -------------------------------------------------------------------
// this sets up the vistype properly.
vistype NotSoSimpleVisitor::_my_type(NOT_SO_SIMPLE_VISITOR_NAME);

// Constructor, passes in vistype, so the hierarchy can be established via ctor calls.
NotSoSimpleVisitor::NotSoSimpleVisitor(int i) : Visitor(_my_type), _key(i) { }

// Derived ctor
NotSoSimpleVisitor::NotSoSimpleVisitor(vistype & child_type) : 
  Visitor(child_type.derived_from(_my_type)), _key(-1) { }

// Destructor
NotSoSimpleVisitor::~NotSoSimpleVisitor( ) 
{ 
  cout << "Ogh! Ack!  Choking on my own playchair." << endl; // - Moe the Bartender, Simpsons 
}

// returns the key
const int NotSoSimpleVisitor::GetKey(void) const { return _key; }

// sets the key
void NotSoSimpleVisitor::SetKey(int k) { _key = k; }

// returns the vistype for SimpleVisitor encapsulated in a VisitorType
const VisitorType NotSoSimpleVisitor::GetType(void) const { return VisitorType(_my_type); }

// returns the vistype for the SimpleVisitor (private or protected);
const vistype & NotSoSimpleVisitor::GetClassType(void) const { return _my_type; }


// ------------------------- example3.cc -----------------------------
SimpleTimer::SimpleTimer(SimEntity * o, double interval) :
  TimerHandler(o, interval), _reregister_interval(interval) 
{ Register(this); }

SimpleTimer::~SimpleTimer() { }

// This method is called when the timer expires (i.e. goes off).
void SimpleTimer::Callback(void)
{
  // Right now all the SimpleTimer does is a print a message.
  cout << "SimpleTimer " << this << " going off at time " << theKernel().CurrentTime() << endl;
  // Reregister this particular instance
  Register(this);
}

// SimpleInput handler
SimpleInput::SimpleInput(SimEntity * o, int fd) :
  InputHandler(o, fd), _istr(fd) 
{ Register(this); }

SimpleInput::~SimpleInput() { }

// This is called when the specified descriptor is ready for reading
void SimpleInput::Callback(void)
{
  char buf[1024];
  _istr.getline(buf, 1024);
  // All it does it print out what it read
  cout << "IHandler " << this << " read: '" << buf << "'" << endl;
}
