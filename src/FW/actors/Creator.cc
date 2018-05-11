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
static char const _Creator_cc_rcsid_[] =
"$Id: Creator.cc,v 1.1 1999/01/13 19:12:11 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/actors/Creator.h>
#include <FW/basics/Conduit.h>

Creator::Creator(void) : 
  Actor(), SimEntity(SimEntity::CREATOR)
{ 
  _creations = new list<Conduit *>; 
}

Creator::~Creator() 
{ 
  if (_creations->length() > 0) {
    list_item li;
    forall_items(li, *_creations) {
      Conduit * c = _creations->inf(li);
      if (c->_creator==this) {
	c->_creator=0;
	delete c;
      }
    }
    _creations->clear();
  }
  delete _creations;
  _creations=0;
}

void Creator::Deregister(Conduit * c)
{
  list_item li;
  if (li = _creations->search(c)) {
    _creations->del_item(li);
    c->_creator = 0;
  } else 
    cout << "Creation disowned by The Creator\n";
}

void Creator::Register(Conduit * c)
{
  assert(c != 0 && !(c->_registered));

  _creations->append(c);
  c->_creator    = this;
  c->_registered = true;

  RegisterConduit(c);
}

void Creator::PassThru(Visitor * v)
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

void Creator::PassVisitorToA(Visitor* v)
{
  Conduit * c = 0;
  assert(v);

  v->_last = GetBehavior()->GetOwner();

  if (c = GetBehavior()->OwnerSideA())
    c->Accept(v);
  else
    v->Suicide();
}

void Creator::PassVisitorToB(Visitor* v)
{
  Conduit * c = 0;
  assert(v);

  v->_last = GetBehavior()->GetOwner();

  if (c = GetBehavior()->OwnerSideB())
    c->Accept(v);
  else
    v->Suicide();
}

enum Visitor::which_side Creator::VisitorFrom(Visitor* v)
{
  return v->EnteredFrom();
}



