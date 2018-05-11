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
static char const _State_cc_rcsid_[] =
"$Id: State.cc,v 1.5 1999/01/25 13:26:45 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/actors/State.h>
#include <FW/basics/Visitor.h>
#include <FW/basics/Conduit.h>
#include <FW/kernel/Handlers.h>

// ----------------------- State -----------------------
State::State(bool queuing) : 
  Actor(), SimEntity(SimEntity::STATE), _queuing(queuing)
{ 
  if ( _queuing ) {
    _serviceTimer = new QueueTimer(this, 0.0);
    assert( _serviceTimer != 0 );
  }
  // Do not register it until we receive a Visitor
}

State::~State() 
{ 
  if ( _queuing ) {
    assert( _serviceTimer != 0 );
    Cancel(_serviceTimer);
    delete _serviceTimer;

    while (! _toA.empty()) {
      Visitor* v = _toA.pop();
      v->Suicide();
    }
    while (! _toB.empty()) {
      Visitor* v = _toB.pop();
      v->Suicide();
    }
    _toA.clear();
    _toB.clear();
  } else
    assert( _toA.empty() && _toB.empty() );
}

enum Visitor::which_side State::VisitorFrom(Visitor* v)
{
  assert(v);
  return v->EnteredFrom();
}

void State::PassThru(Visitor * v)
{
  switch (VisitorFrom(v)) {
    case Visitor::A_SIDE:
      PassVisitorToB(v);
      break;
    case Visitor::B_SIDE:
      PassVisitorToA(v);
      break;
    case Visitor::OTHER:
    default:
      PassVisitorToA(v);
      break;
  }
}

void State::PassVisitorToA(Visitor * v)
{
  if (_queuing) {
    Register(_serviceTimer);
    _toA.append(v);
  } else
    QPassVisitorToA(v);
}

void State::PassVisitorToB(Visitor * v)
{
  if (_queuing) {
    Register(_serviceTimer);
    _toB.append(v);
  } else
    QPassVisitorToB(v);
}

// ------------------ Added for Queuing State -------------------
void State::QPassVisitorToA(Visitor * v)
{
  Conduit * c = 0;
  Behavior * b = GetBehavior();

  assert(v && b);

  v->SetLast(b->GetOwner()); // this also puts birth in the log

  if (c = b->OwnerSideA())
    c->Accept(v);
  else
    v->Suicide();
}

void State::QPassVisitorToB(Visitor * v)
{
  Conduit * c = 0;
  Behavior * b = GetBehavior();

  assert(v && b);

  v->SetLast(b->GetOwner()); // this also puts birth in the log

  if (c = b->OwnerSideB())
    c->Accept(v);
  else
    v->Suicide();
}
