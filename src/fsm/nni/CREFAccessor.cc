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

/* -*- C++ -*-
 * File: CREFAccessor.cc
 * Author: talmage
 * Version: $Id: CREFAccessor.cc,v 1.12 1998/08/11 18:35:24 mountcas Exp $
 * Purpose: 
 * BUGS:
 */
#ifndef LINT
static char const rcsid[] =
"$Id: CREFAccessor.cc,v 1.12 1998/08/11 18:35:24 mountcas Exp $";
#endif

#include <common/cprototypes.h>

#include <FW/basics/diag.h>
#include <fsm/nni/CREFAccessor.h>
#include <fsm/nni/Q93bVisitors.h>


// ------------------ CREFAccessor ----------------------
const VisitorType * CREFAccessor::_uni_visitor_type = 0;

CREFAccessor::CREFAccessor(void) 
{
  if (_uni_visitor_type == 0)
    _uni_visitor_type = QueryRegistry(Q93B_VISITOR_NAME);
}

CREFAccessor::~CREFAccessor()
{
  //
  // Clear _access_map.
  // CREFAccessor doesn't own the Conduit *'s in _access_map, so
  // the best it can do is zero them.
  //
  _access_map.clear();
}

Conduit * CREFAccessor::GetNextConduit(Visitor * v)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout << 
       "*** " << OwnerName() << "::GetNextConduit (" << 
       this << ") *** received " << v->GetType() << endl);

  VisitorType vt = v->GetType();

  if (vt.Is_A(_uni_visitor_type)){
    Q93bVisitor * uv = (Q93bVisitor*)v;
    dic_item d;
    int i = 0;
    i = uv->get_crv();

    if (d = _access_map.lookup(i))
      return (_access_map.inf(d));
  }
  return 0;
}

bool CREFAccessor::Broadcast(Visitor * v)
{
  dic_item it;
  Conduit * c;
  forall_items(it, _access_map) {
    c = _access_map.inf(it);
    c->Accept(v->duplicate());
  }
  v->Suicide();
  return true;
}

bool CREFAccessor::Add(Conduit * c, Visitor * v)
{
  DIAG("fsm.nni", DIAG_DEBUG, cout << 
       "*** " << OwnerName() << "::GetAdd (" << 
       this << ") *** received " << v->GetType() << endl);

  VisitorType vt = v->GetType();

  if (vt.Is_A(_uni_visitor_type)){
    Q93bVisitor * uv = (Q93bVisitor*)v;
    dic_item d;
    int i = 0;
    i = uv->get_crv();

    if (_access_map.lookup(i)) 
      return false;
    _access_map.insert(i,c);
    return true;
  }
  return false;
}

bool CREFAccessor::Del(Conduit * c)
{
  dic_item it;
  forall_items(it,_access_map) {
    if (_access_map.inf(it)==c) {
      _access_map.del_item(it);
      return true;
    }
  }
  return false;
}

bool CREFAccessor::Del(Visitor * v)
{
  int crv = 0;
  VisitorType vt = v->GetType();

  if (vt.Is_A(_uni_visitor_type)) {
    Q93bVisitor * uv = (Q93bVisitor*)v;
    crv = uv->get_crv();
  }
  dic_item di;
  if (!(di = _access_map.lookup(crv)))
    return false;

  _access_map.del_item(di);
  return true;
}
