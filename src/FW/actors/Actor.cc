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

// -*- C++ -*_
#ifndef LINT
static char const _Actor_cc_rcsid_[] =
"$Id: Actor.cc,v 1.1 1999/01/13 19:12:11 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/basics/VisitorType.h>
#include <FW/basics/Registry.h>
#include <FW/behaviors/Behavior.h>
#include <FW/actors/Actor.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/Log.h>
#include <FW/interface/ShareableRegistry.h>

Registry * Actor::_registry = 0;

Actor::Actor(void) : 
  _behavior(0), _name(0), _deletion_violation(true),
  _parent_expander(0)
{ 
  if (!_registry) 
    InitFW(); 
}

Actor::~Actor() 
{ 
  if (_deletion_violation) {
    cerr << "**************************************************************\n";
    cerr << "***   You may NOT call the delete on a derived actor.      ***\n";
    cerr << "***   Keep all destructors of derived actors private.      ***\n";
    cerr << "***   The protected method Free() may be used to indicate  ***\n";
    cerr << "***   that an actor is no longer needed.                   ***\n";
    cerr << "**************************************************************\n";
    abort();
  }
}

void Actor::Authorize_Deletion(void) 
{
  _deletion_violation = false;
}

void Actor::Free(void) 
{ 
  _behavior->GetOwner()->Set_Free();
}

const char * const Actor::OwnerName(void)
{ 
  if (!_name && _behavior)
    _name = _behavior->GetOwnerName();

  return _name;
}

void Actor::ExpanderUpdate(const char * const name) { }

void Actor::SetOwnerName(const char * const name) 
{ 
  _name = name; 

  if (_behavior->GetType() == Behavior::CLUSTER)
    ExpanderUpdate(name);
}

void Actor::SetBehavior(Behavior * b) 
{
  _behavior = b;
}

Behavior * Actor::GetBehavior(void) const
{
  return _behavior;
}

const VisitorType * const QueryRegistry(const char * const name)
{
  char * stripws(const char *);

  char * tmp = (char *)name; // stripws(name);
  const vistype * vt = Actor::_registry->Lookup(tmp);
  // delete [] tmp;

  if (vt) 
    return new VisitorType(*vt);
  return 0;
}

void Actor::RegisterConduit(Conduit * c)
{
  char buf[Conduit::MAX_LOG_SIZE];

  assert(_behavior != 0);

  if (_parent_expander != 0 &&
      _behavior->_owner->GetParent() == 0)
    _behavior->_owner->SetParent(_parent_expander->_behavior->_owner);

  if ((_behavior->GetType() == Behavior::FACTORY) &&
      (_behavior->_owner->GetParent() == 0)) {
    // This is for adding Conduits to a top level Factory
    //  the Conduit will be situated right below the root node
    sprintf(buf, "!add %s !to %s", c->GetName(), OwnerName());
    theLog().AppendToRecord(buf);
  } else {
    // the conduit to add must exist AND
    // this conduit must be part of an expander AND
    // the conduit to add must not have a parent already
    assert(c && _behavior->_owner->GetParent() &&
	   (!c->GetParent()));
    c->SetParent( _behavior->_owner->GetParent() );
    sprintf(buf, "!add %s !to %s", c->GetName(), 
	    _behavior->_owner->GetParent()->GetName());
    theLog().AppendToRecord(buf);
    
    sprintf(buf, "%s %s", _parent_expander->OwnerName(), c->GetName());
    c->SetName( buf );
    // Now situate it within the hierarchy
    theShareableRegistry().update(c->GetName(), c->_behavior->_actor);
    // It's children should already be situated correctly beneath it
  }
}

void Actor::SetParent(const Conduit * c) { }
