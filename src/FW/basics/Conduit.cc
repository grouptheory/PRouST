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
static char const _Conduit_cc_rcsid_[] =
"$Id: Conduit.cc,v 1.2 1999/01/28 15:50:20 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/basics/Conduit.h>

#include <FW/basics/Registry.h>
#include <FW/basics/Visitor.h>
#include <FW/basics/diag.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/KDB.h>

// for the new ctors
#include <FW/behaviors/Mux.h>
#include <FW/behaviors/Factory.h>
#include <FW/behaviors/Cluster.h>
#include <FW/behaviors/Protocol.h>
#include <FW/behaviors/Adapter.h>
#include <FW/actors/Expander.h>
#include <FW/interface/ShareableRegistry.h>

#include <iostream.h>

extern "C"{
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
};

// ------------------------------- Conduit -----------------------------------
const VisitorType * Conduit::_disconnect_type    = 0;
const VisitorType * Conduit::_clusterkiller_type = 0;
Registry *          Conduit::_registry = 0;
int                 Conduit::_population = 0;


// ----------------- deemed obsolete ------------------
Conduit::Conduit(const char * name, Behavior* b) : 
  _behavior(b),  _sideA(0), _sideB(0), _name(0),
  _parent(0), _creator(0), _composite(0),
  _sideAhalf(HALF_NOT_SET), _sideBhalf(HALF_NOT_SET),
  _freed(false), _registered(false)
{
  assert(_behavior);

  if (_behavior->GetType() == Behavior::CLUSTER) {
    Expander * e = (Expander *)_behavior->_actor;
    if (!e->ProperlyConstructed()) {
      cerr << "Conduit " << name << " is being instantiated without having "
	   << "it's Expander call DefinitionComplete!" << endl;
      exit(2);
    }
  }
  _composite = _behavior->IsComposite();
  _behavior->SetOwner(this);
  _kdb_conduit_id = theKernel().KDB()->Report(this);
  // Construct the name
  SetName(name);
  // Build everything that needs building, and record birth
  global_conduit_ctors();
  // Increase the global Conduit population.
  _population++;
}

// ---- Accessor version ----
Conduit::Conduit(const char * name, Accessor * a) :
  _behavior(0),  _sideA(0), _sideB(0), _name(0),
  _parent(0), _creator(0), _composite(0),
  _sideAhalf(HALF_NOT_SET), _sideBhalf(HALF_NOT_SET),
  _freed(false), _registered(false)
{
  assert(a);
  // Allocate a Mux for the Accessor
  _behavior  = new Mux(a);
  assert(_behavior);
  _behavior->SetOwner(this);
  _composite = _behavior->IsComposite();
  _kdb_conduit_id = theKernel().KDB()->Report(this);

  // Construct our name
  SetName(name);
  // Build everything that needs building, and record birth
  global_conduit_ctors();

  // Increase the global Conduit population.
  _population++;
}

// ---- Creator Version ----
Conduit::Conduit(const char * name, Creator * c) :
  _behavior(0),  _sideA(0), _sideB(0), _name(0),
  _parent(0), _creator(0), _composite(0),
  _sideAhalf(HALF_NOT_SET), _sideBhalf(HALF_NOT_SET),
  _freed(false), _registered(false)
{
  assert(c);
  // Allocate a Mux for the Accessor
  _behavior  = new Factory(c);
  assert(_behavior);
  _behavior->SetOwner(this);
  _composite = _behavior->IsComposite();
  _kdb_conduit_id = theKernel().KDB()->Report(this);

  // Construct our name
  SetName(name);
  // Build everything that needs building, and record birth
  global_conduit_ctors();

  // Increase the global Conduit population.
  _population++;
}

// --- Expander Version ---
Conduit::Conduit(const char * name, Expander * e) :
  _behavior(0),  _sideA(0), _sideB(0), _name(0),
  _parent(0), _creator(0), _composite(0),
  _sideAhalf(HALF_NOT_SET), _sideBhalf(HALF_NOT_SET),
  _freed(false), _registered(false)
{
  assert(e);

  if (!e->ProperlyConstructed()) {
    cerr << "Conduit " << name << " is being instantiated without having "
	 << "it's Expander didn't call DefinitionComplete!" << endl;
    exit(2);
  }
  // Allocate a Mux for the Accessor
  _behavior  = new Cluster(e);
  assert(_behavior);
  _behavior->SetOwner(this);
  _composite = _behavior->IsComposite();
  _kdb_conduit_id = theKernel().KDB()->Report(this);

  // Construct our name
  SetName(name);
  // Build everything that needs building, and record birth
  global_conduit_ctors();

  // Increase the global Conduit population.
  _population++;
}

// ---- State Version ----
Conduit::Conduit(const char * name, State * s) :
  _behavior(0),  _sideA(0), _sideB(0), _name(0),
  _parent(0), _creator(0), _composite(0),
  _sideAhalf(HALF_NOT_SET), _sideBhalf(HALF_NOT_SET),
  _freed(false), _registered(false)
{
  assert(s);
  // Allocate a Mux for the Accessor
  _behavior  = new Protocol(s);
  assert(_behavior);
  _behavior->SetOwner(this);
  _composite = _behavior->IsComposite();
  _kdb_conduit_id = theKernel().KDB()->Report(this);

  // Construct our name
  SetName(name);
  // Build everything that needs building, and record birth
  global_conduit_ctors();

  // Increase the global Conduit population.
  _population++;
}

// ---- Terminal Version ----
Conduit::Conduit(const char * name, Terminal * t) :
  _behavior(0),  _sideA(0), _sideB(0), _name(0),
  _parent(0), _creator(0), _composite(0),
  _sideAhalf(HALF_NOT_SET), _sideBhalf(HALF_NOT_SET),
  _freed(false), _registered(false)
{
  assert(t);
  // Allocate a Mux for the Accessor
  _behavior  = new Adapter(t);
  assert(_behavior);
  _behavior->SetOwner(this);
  _composite = _behavior->IsComposite();
  _kdb_conduit_id = theKernel().KDB()->Report(this);

  // Construct our name
  SetName(name);
  // Build everything that needs building, and record birth
  global_conduit_ctors();

  // Increase the global Conduit population.
  _population++;
}

// ---- dtor ----
Conduit::~Conduit() 
{
  --_population;
	
  theKernel().RemConduit(this);

  Behavior::types t = _behavior->GetType();

  if (_behavior) { 
    _behavior->Authorize_Deletion();
    _behavior->Suicide(); 
    _behavior = 0; 
  }
  if (_creator) { _creator->Deregister(this); _creator = 0; }

  // Record death if not composite.
  if (!_composite) {
    char buf[MAX_LOG_SIZE];
    sprintf(buf, "!death %s !%s", GetName(), 
	    (t == Behavior::ADAPTER ? "Adapter" :
	     (t == Behavior::CLUSTER ? "Cluster" :
	      (t == Behavior::FACTORY ? "Factory" :
	       (t == Behavior::MUX ? "Mux" :
		(t == Behavior::PROTOCOL ? "Protocol" : "Unknown"))))));
    theLog().AppendToRecord(buf);
  }

  // Now cleanup memory used by name
  if (_name) { delete [] _name; _name = 0; }

  _h = HALF_NOT_SET; _sideA = 0; _sideAhalf = HALF_NOT_SET;
  _sideB = 0; _sideBhalf = HALF_NOT_SET; _composite = 0;
  _parent = 0;

#ifndef OPTIMIZE
  if (theKernel().KDB()->Resign(_kdb_conduit_id, this) == 0){
    BPT* b=new BPT(this,BPT::ConduitDestruction);
    int is_breakpoint = (theKernel().KDB()->Notify(b));
    delete b;
    if (is_breakpoint){
      ALERT_DEBUGGER;
    }
  }
#endif
}

void Conduit::global_conduit_ctors(void)
{
  if (!_registry) 
    InitFW();
      
  if (!_disconnect_type) {
    const vistype * dv = _registry->Lookup(DISCONNECT_VISITOR_TYPE);
    if (!dv) {
      diag("FW", DIAG_FATAL, "Framework cannot boot without the %s registered.\n",
	   DISCONNECT_VISITOR_TYPE);
    }
    _disconnect_type = new VisitorType(*dv);
  }
  
  if (!_clusterkiller_type) {
    const vistype * dv = _registry->Lookup(CLUSTERKILLER_VISITOR_TYPE);
    if (!dv) {
      diag("FW", DIAG_FATAL, "Framework cannot boot without the %s registered.\n",
	   CLUSTERKILLER_VISITOR_TYPE);
    }
    _clusterkiller_type = new VisitorType(*dv);
  }
  
  // Record birth.
  if (!_composite) {
    char buf[MAX_LOG_SIZE];
    Behavior::types t = _behavior->GetType();
    sprintf(buf, "!birth %s !%s", GetName(), 
	    (t == Behavior::ADAPTER ? "Adapter" :
	     (t == Behavior::CLUSTER ? "Cluster" :  // This can never be true cause of the if statement ...
	      (t == Behavior::FACTORY ? "Factory" :
	       (t == Behavior::MUX ? "Mux" :
		(t == Behavior::PROTOCOL ? "Protocol" : "Unknown"))))));
    theLog().AppendToRecord(buf);
  } 

  BuildRegistry();
}

void Conduit::Accept(Visitor * v) 
{
  if (!_behavior) {
    diag("FW.basics", DIAG_FATAL, "Conduit '%s' does not have a valid Behavior!", GetName());
  }
  assert(_behavior != 0);

#ifndef OPTIMIZE
  BPT * b = new BPT(this,BPT::ConduitIntrusion,v);
  int is_breakpoint = theKernel().KDB()->Notify(b);
  delete b;

  b = new BPT(v,BPT::VisitorArrival,this);
  is_breakpoint |= (theKernel().KDB()->Notify(b));
  delete b;
#endif

  const VisitorType t = v->GetType();
  Conduit * c = v->GetLast();

  // Record the receipt of a Visitor.
  if (!_composite && c) {
    char buf[MAX_LOG_SIZE];
    char subtype[50];
    v->Fill_Subtype_Name(subtype);
    if (strcmp(subtype,"")==0)
      sprintf(buf, "!received %s:%d !in %s", v->GetType().Name(), 
	      v->_kdb_visitor_id, GetName());
    else
      sprintf(buf, "!received %s:%d !in %s", subtype, 
	      v->_kdb_visitor_id, GetName());
    theLog().AppendToRecord(buf);
    theKernel().Lockstep_Notification();
  }

  if (t.Is_A(_disconnect_type)) {
    if (c == _sideB) 
      _behavior->DisconnectB(c,0);
    else if (c == _sideA)
      _behavior->DisconnectA(c,0);
    else  {
      _behavior->DisconnectB(c,v);
    }
    // DisconnectVisitors should be freed here ...
    v->CurrentConduit(this);
    v->Suicide();
    return;
  } else if (t.Is_A(_clusterkiller_type)) {
    if (_parent) {
      Conduit * dst = (Conduit *)_parent;
      v->CurrentConduit(this);
      dst->Accept(v);
      return;
    } else if (_composite) {
      // ClusterKillers only destroy composites
      v->Suicide();
      delete this;
      return;
    }
  } 
  if (_behavior) {
    v->CurrentConduit(this);
    //_freed = false;
#ifndef OPTIMIZE
    _behavior->Accept(v, is_breakpoint);       // Don't use v after this line!
#else
    _behavior->Accept(v, 0);
#endif
    // if (_freed) delete this;
  }
}

bool Conduit::set_sideA(Conduit * a) 
{ 
  _sideA = a; 
  if (a) 
    _sideAhalf = a->_h;
  else
    _sideAhalf = HALF_NOT_SET;
  return true; 
}

bool Conduit::set_sideB(Conduit * b) 
{ 
  _sideB = b; 
  if (b) 
    _sideBhalf = b->_h;
  else
    _sideBhalf = HALF_NOT_SET;
  return true; 
}

Conduit * A_half(Conduit* c) 
{ 
  if (! c->_composite) {
    c->_h = Conduit::A_HALF; 
    return c; 
  } else {
    switch (c->_sideAhalf) {
      case Conduit::A_HALF:
	return A_half(c->_sideA);
	break;
      case Conduit::B_HALF:
	return B_half(c->_sideA);
	break;
    default: // This is a fatal error, so be sure to use the generated core.
	cerr << "Bad A_half" << flush;
	abort();
	break;
    }
  }
}

Conduit * B_half(Conduit* c) 
{ 
  if (! c->_composite) {
    c->_h = Conduit::B_HALF; 
    return c; 
  } else {
    switch (c->_sideBhalf) {
      case Conduit::A_HALF:
	return A_half(c->_sideB);
	break;
      case Conduit::B_HALF:
	return B_half(c->_sideB);
	break;
      default:
	cerr << "Bad A_half" << flush;
	abort();
	break;
    }
  }
}


bool Join(Conduit * c1, Conduit * c2)
{
  return Join(c1, c2, 0, 0);
}

bool Join(Conduit* c1, Conduit* c2, Visitor * v1, Visitor * v2 = 0)
{
  bool      result;
  Conduit * saved;

  if ((!c1->_behavior) ||
      (!c2->_behavior) ||
      (c1->_h==Conduit::HALF_NOT_SET) ||
      (c2->_h==Conduit::HALF_NOT_SET)) {
    result=false;
  } else {
    switch(c1->_h) {
      case Conduit::A_HALF:
	saved = (Conduit*) (c1->GetA(v1));
	switch (c2->_h) {
	  case Conduit::A_HALF:
	    if (result = c1->_behavior->ConnectA(c2, v1))     // true
	      if (!(result = c2->_behavior->ConnectA(c1,v2))) // 2nd Connect failed, so restore.
		c1->_behavior->ConnectA(saved, v1);
	    break;
	  case Conduit::B_HALF:
	    if (result = c1->_behavior->ConnectA(c2,v1))
	      if (!(result = c2->_behavior->ConnectB(c1,v2)))
		c1->_behavior->ConnectA(saved, v1);
	    break;
	  default:
	    result = false;
	}
	break;
      case Conduit::B_HALF:
	saved = (Conduit*) (c1->GetB(v1));
	switch (c2->_h) {
	  case Conduit::A_HALF:
	    if (result = c1->_behavior->ConnectB(c2,v1))
	      if (!(result = c2->_behavior->ConnectA(c1,v2)))
		c1->_behavior->ConnectB(saved, v1);
	    break;
	  case Conduit::B_HALF:
	    if (result = c1->_behavior->ConnectB(c2,v1))
	      if (!(result = c2->_behavior->ConnectB(c1,v2)))
		c1->_behavior->ConnectB(saved, v1);
	    break;
	  default:
	    result = false;
	}
	break;
      default:
	break;
    }
  }
  // Record connection if made properly.
  if (result == true) {
    char buf[Conduit::MAX_LOG_SIZE];
    Behavior::types b1 = c1->_behavior->GetType(), b2 = c2->_behavior->GetType();
    // 0 = Not set, 1 = A, 2 = B, 3 = Other
    sprintf(buf, "!connected %s %d !to %s %d", c1->GetName(), 
	    ((v1 && b1 == Behavior::MUX) ? 3 : c1->_h), c2->GetName(), 
	    ((v2 && b2 == Behavior::MUX) ? 3 : c2->_h));
    theLog().AppendToRecord(buf);
  }
  c1->_h=Conduit::HALF_NOT_SET;
  c2->_h=Conduit::HALF_NOT_SET;

  return result;
}


bool Sever(Conduit * c1, Conduit * c2)
{
  return Sever(c1, c2, 0, 0);
}

bool Sever(Conduit* c1, Conduit* c2, Visitor * v1, Visitor * v2 = 0)
{  
  bool result;

  if ((!c1->_behavior)||
      (c1->_h==Conduit::HALF_NOT_SET) ||
      (c2->_h==Conduit::HALF_NOT_SET)) {
    c1->_h = Conduit::HALF_NOT_SET;
    c2->_h = Conduit::HALF_NOT_SET;
    result = false;
  } else {
    switch (c1->_h) {
      case Conduit::A_HALF:
	switch (c2->_h) {
	  case Conduit::A_HALF:
	    if (result = c1->_behavior->DisconnectA(c2,v1))
	      if (!(result = c2->_behavior->DisconnectA(c1,v2)))
		c1->_behavior->ConnectA(c2, v1);
	    break;
	  case Conduit::B_HALF:
	    if (result = c1->_behavior->DisconnectA(c2,v1))
	      if (!(result = c2->_behavior->DisconnectB(c1,v2)))
		c1->_behavior->ConnectA(c2, v1);
	    break;
	  default:
	    result = false;
	}
	break;
      case Conduit::B_HALF:
	switch (c2->_h) {
	  case Conduit::A_HALF:
	    if (result = c1->_behavior->DisconnectB(c2,v1))
	      if (!(result = c2->_behavior->DisconnectA(c1,v2)))
		c1->_behavior->ConnectB(c2,v1);
	    break;
	  case Conduit::B_HALF:
	    if (result = c1->_behavior->DisconnectB(c2,v1))
	      if (!(result = c2->_behavior->DisconnectB(c1,v2)))
		c1->_behavior->ConnectB(c2, v1);
	    break;
	  default:
	    result = false;
	}
	break;
      default:
	result = false;
	break;
    }
  }
  c1->_h = Conduit::HALF_NOT_SET;
  // Record disconnection if made properly.
  if (result == true) {
    char buf[Conduit::MAX_LOG_SIZE];
    sprintf(buf, "!disconnected %s !from %s", c1->GetName(), c2->GetName());
    theLog().AppendToRecord(buf);
  }
  return result;
}


void Conduit::SetName(const char* name)
{
  char buf[MAX_LOG_SIZE], buf2[MAX_CONDUIT_NAMELENGTH];
  char * stripws(const char *);
  char * stripaddr(const char *);
  
  buf[0] = '\0';

  if (_name) { //  && !_composite) {
    sprintf(buf, "!namechange %s !to ", _name);
    delete [] _name;
    _name=0;
  }
  // Strip the white space and '!' out of name.
  char * tmp = stripws(name);
  // _name = new char[MAX_CONDUIT_NAMELENGTH];
  
  if (!name) { // use Conduit<address> as name
    sprintf(buf2, "Conduit:%d+", _kdb_conduit_id);
    _name = new char [strlen(buf2)+1];
    strcpy(_name, buf2);
  } else {
    char ptr[MAX_CONDUIT_NAMELENGTH];
    char * oldtmp = tmp;
    tmp = stripaddr(oldtmp);
    delete [] oldtmp;
    sprintf(ptr, "%s:%d+", tmp, _kdb_conduit_id);
    int len = strlen(ptr);
    if (len >= MAX_CONDUIT_NAMELENGTH) 
      len = MAX_CONDUIT_NAMELENGTH-1;
    _name = new char [len+1];
    strncpy(_name, ptr, len);
    _name[len] = 0;
  }
  // Free memory alloc'ed by stripws
  delete [] tmp;

  // name change is not printed when being born.
  if (*buf) {
    strcat(buf, _name);
    // Add it to the global conduit record.
    theLog().AppendToRecord(buf);
  }

  // Notify behavior of new name.
  if (_behavior) _behavior->SetOwnerName(_name);
}

const Conduit * GetSideA(Conduit * c) { return c->_sideA; }

const Conduit * GetSideB(Conduit * c) { return c->_sideB; }

char * stripws(const char * name)
{
  char * rval = new char [strlen(name)+1];
  int i, j = 0;

  for (i = 0; name[i]; i++) {
    if (isspace(name[i]) || name[i] == '!')
      rval[j++] = '.'; // New way replaces all spaces with '.'
      // continue;   // Old way removes all spaces
    else
      rval[j++] = name[i];
  }
  rval[j] = 0;
  return rval;
}

char * stripaddr(const char * name)
{
  if (!name)
    return 0;

  char * rval = new char [strlen(name)+1];  
  int i, j;

  for (i = 0, j = 0; name[i] != '\0' && j < 1024; i++) {
    if (name[i] != ':')
      rval[j++] = name[i];
    else {
      while ((name[i] != '\0') && (name[i] != '+'))
	i++;
    }
  }
  rval[j] = '\0';
  return rval;
}

const char * const Identifier(const char * const n)
{
  const char * rval = n + strlen(n);
  int i = 0;

  while ( rval ) {
    if (*(--rval) == '.')
      break;
    i++;
    // This is necessary for names that do not contain periods
    if (i == strlen(n))
      break;
  }

  if (rval && rval[0] == '.')
    rval++;
  else
    rval = n;

  return rval;
}

void Conduit::Set_Free(void) 
{
  theKernel().AddConduit(this);
  _freed = true;
}

void Conduit::BuildRegistry(void)
{
  // if there is something on the expanders stack, add myself to it
  if (_registry->_expanders.empty())
    AddToRegistry();
  else {
    Expander * exp = _registry->_expanders.pop();
    exp->_children.append(this);
    _behavior->_actor->_parent_expander = exp;
    _registry->_expanders.push(exp);
  }
}

void Conduit::AddToRegistry(void)
{
  // Add myself first
  theShareableRegistry().update(GetName(), _behavior->_actor);

  // then any children if necessary
  if (_behavior->GetType() == Behavior::CLUSTER) {
    Expander * exp = (Expander *)_behavior->_actor;

    list_item li;
    forall_items(li, exp->_children) {
      Conduit * c = exp->_children.inf(li);
      c->AddToRegistry();
    }
  }
}

void Conduit::SetParent(const Conduit * c)  
{ 
  _parent = c; 
}

const Conduit * Conduit::GetParent(void) const 
{ 
  return _parent; 
}

// -------- BOOM --------
Bomb::Bomb(Conduit * victim) : _victim(victim) { }

Bomb::~Bomb( ) { delete _victim; _victim = 0; }

void Bomb::Detonate(void) { delete this; }
