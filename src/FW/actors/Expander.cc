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
static char const _Expander_cc_rcsid_[] =
"$Id: Expander.cc,v 1.2 1999/01/25 13:27:12 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/actors/Expander.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/Log.h>
#include <FW/basics/Registry.h>

extern "C" {
#include <stdio.h>
};

const char * const Identifier(const char * const n);

// ------------------------ Base Expander -------------------------
Expander::Expander(void) : Actor(), _properly_constructed(false)
{ 
  char buf[1024];
  sprintf(_prevname, "%lx", this);
  sprintf(buf, "!start %s", _prevname);
  theLog().AppendToRecord(buf);

  Actor::_registry->_expanders.push(this);
}

Expander::~Expander() { }

Conduit * A_half(const Expander * e)
{
  return (e->GetAHalf());
}

Conduit * B_half(const Expander * e)
{
  return (e->GetBHalf());
}

void Expander::SetParent(const Conduit * c)  
{
  Conduit * a = GetAHalf();
  Conduit * b = GetBHalf();
  if (a) 
    a->SetParent(c);
  if (b) 
    b->SetParent(c);

  if (*_prevname) {
    // This only needs to be done the very first time since subsequent name changes
    //  will be facilitated by the parent Conduit itself.
    char buf[1024];
    sprintf(buf, "!namechange %s !to %s", _prevname, c->GetName());
    _prevname[0] = 0x0;
    theLog().AppendToRecord(buf);
  }
}

const Conduit * Expander::GetParent(void) const
{
  return (GetAHalf()->GetParent());
}

void Expander::ExpanderUpdate(const char * const name)
{
  char buf[Conduit::MAX_CONDUIT_NAMELENGTH];

  list_item li;
  forall_items(li, _children) {
    Conduit * c = _children.inf(li);
    sprintf(buf, "%s %s", name, Identifier(c->GetName()));
    c->SetName(buf);
  }
}

void Expander::DefinitionComplete(void)
{
  char buf[1024];   
  sprintf(buf, "!end %lx", this);
  theLog().AppendToRecord(buf);

  Expander * e = Actor::_registry->_expanders.pop();
  assert( this == e );

  _properly_constructed = true;
}

bool Expander::ProperlyConstructed(void) const
{
  return _properly_constructed;
}
