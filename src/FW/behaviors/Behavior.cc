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
static char const _Behavior_cc_rcsid_[] =
"$Id: Behavior.cc,v 1.1 1999/01/13 19:12:50 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/behaviors/Behavior.h>

#include <FW/basics/Conduit.h>
#include <FW/basics/Visitor.h>
#include <FW/actors/Actor.h>
#include <FW/kernel/KDB.h>

// Default Behavior.
Behavior::Behavior(Actor * a, types type) : _actor(a), _type(type) 
{ 
  if (a) a->SetBehavior(this); 
}

Behavior::~Behavior() { delete _actor; _actor = 0; }

void Behavior::Accept(Visitor * v, int is_breakpoint)
{
  if (is_breakpoint){
    ALERT_DEBUGGER;
  }
  v->at(this);
  v = 0;
}

void Behavior::Suicide(void)
{
  Conduit* a=OwnerSideA();
  Conduit* b=OwnerSideB();
  Conduit* myowner=GetOwner();

  if (a) {
    DisconnectVisitor* dv_a=new DisconnectVisitor;
    dv_a->SetLast(myowner);
    a->Accept(dv_a);
  }
  if (b) {
    DisconnectVisitor* dv_b=new DisconnectVisitor;
    dv_b->SetLast(myowner);
    b->Accept(dv_b);
  }
  delete this;
}

void Behavior::SetOwner(Conduit * c) { _owner = c; }

void Behavior::SetOwnerName(const char * const name) 
{ 
  if (_actor)  {
    _actor->SetOwnerName(name); 
    if (IsComposite()) 
      _actor->SetParent( GetOwner() );
  }
}

Behavior::types Behavior::GetType(void) const { return _type; }

Conduit * Behavior::GetOwner(void) const { return _owner; }

Conduit * Behavior::OwnerSideA(void) const { return _owner->_sideA; }

Conduit * Behavior::OwnerSideB(void) const { return _owner->_sideB; }


bool Behavior::ConnectA(Conduit *a, Visitor *v){
  if (OwnerSideA()) return false;
  _owner->set_sideA(a);
  return true;
}

bool Behavior::DisconnectA(Conduit *a, Visitor *v){
  if (!OwnerSideA()) return false;

  if (a){
    if (OwnerSideA()==a) {
      _owner->set_sideA(0);
      return true;
    }
    return false;
  }

  _owner->set_sideA(0);
  return true;
}

bool Behavior::ConnectB(Conduit *b, Visitor *v){
  if (OwnerSideB()) return false;
  _owner->set_sideB(b);
  return true;
}

bool Behavior::DisconnectB(Conduit *b, Visitor *v){
  if (!OwnerSideB()) return false;

  if (b){
    if (OwnerSideB()==b) {
      _owner->set_sideB(0);
      return true;
    }
    return false;
  }

  _owner->set_sideB(0);
  return true;
}

const char * Behavior::GetOwnerName(void) const { return _owner->GetName(); }

const Conduit * Behavior::GetA(Visitor * v) { return GetSideA(_owner); }
const Conduit * Behavior::GetB(Visitor * v) { return GetSideB(_owner); }

bool     Behavior::IsComposite(void) const { return (bool)false; }

void Behavior::Authorize_Deletion(void) const { 
  _actor->Authorize_Deletion(); 
}

const Actor * Behavior::GetActor(void) const { return _actor; }
