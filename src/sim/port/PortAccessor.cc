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
 * File: PortAccessor.cc
 * Author: talmage
 * Version: $Id: PortAccessor.cc,v 1.22 1998/09/28 19:36:19 mountcas Exp $
 * Purpose: Maps PortVisitors and PortInstallerVisitors into their 
 * corresponding Conduits.
 * BUGS:
 */

#ifndef LINT
static char const rcsid[] =
"$Id: PortAccessor.cc,v 1.22 1998/09/28 19:36:19 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <sim/port/PortAccessor.h>
#include <sim/switch/PortInstallerVisitor.h>
#include <fsm/visitors/PortVisitor.h>
#include <fsm/nni/Q93bVisitors.h>

// ------------------ PortAccessor ----------------------
const VisitorType * PortAccessor::_port_installer_type = 0;
const VisitorType * PortAccessor::_port_visitor_type = 0;
const VisitorType * PortAccessor::_q93b_visitor_type = 0;

PortAccessor::PortAccessor(void) 
{
  if (_port_installer_type == 0)
    _port_installer_type = QueryRegistry(PORT_INSTALLER_VISITOR_NAME);

  if (_port_visitor_type == 0)
    _port_visitor_type = QueryRegistry(PORT_VISITOR_NAME);

  if (_q93b_visitor_type == 0)
    _q93b_visitor_type = QueryRegistry(Q93B_VISITOR_NAME);
}

PortAccessor::~PortAccessor()
{
  dic_item ditem = 0;
  Conduit *c = 0;

  //
  // Clear _access_map.
  // PortAccessor doesn't own the Conduit *'s in _access_map, so
  // the best it can do is zero them.
  //
  forall_items(ditem, _access_map) {
    _access_map.change_inf(ditem, (Conduit *)0);
  }

  _access_map.clear();

}

Conduit * PortAccessor::GetNextConduit(Visitor * v)
{
  int i = -1;

  if (v->GetType().Is_A(_port_installer_type))
    i = ((PortInstallerVisitor *)v)->GetOutPort();
  else if (v->GetType().Is_A(_q93b_visitor_type))
    i = ((Q93bVisitor *)v)->get_port();
  else if (v->GetType().Is_A(_port_visitor_type))
    i = ((PortVisitor *)v)->GetOutPort();

  dic_item di;
  if (di = _access_map.lookup(i))
    return (_access_map.inf(di));
  return 0;
}

bool PortAccessor::Broadcast(Visitor * v)
{
  int count = _access_map.size();

  dic_item it;
  forall_items(it,_access_map) {
    count--;

    Visitor * ToSend = v;
    if (count)
      ToSend = v->duplicate();

    Conduit * c = _access_map.inf(it);
    c->Accept(ToSend);
  }
  return true;
}

bool PortAccessor::Add(Conduit * c, Visitor * v)
{
  PortInstallerVisitor * piv = (PortInstallerVisitor*)v;
  PortVisitor * pv = (PortVisitor *)v;
  Q93bVisitor * qv = (Q93bVisitor *)v;
  VisitorType vt = v->GetType();
  int i;

  if (vt.Is_A(_port_installer_type))
    i = piv->GetOutPort();
  else if (vt.Is_A(_port_visitor_type))
    i = pv->GetOutPort();
  else if (vt.Is_A(_q93b_visitor_type))
    i = qv->get_port();
  else return 0;

  if (_access_map.lookup(i))
    return false;
  
  _access_map.insert(i,c);
  return true;
}

bool PortAccessor::Del(Conduit * c)
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

bool PortAccessor::Del(Visitor * v)
{
  PortInstallerVisitor * piv = (PortInstallerVisitor*)v;
  PortVisitor * pv = (PortVisitor *)v;
  Q93bVisitor * qv = (Q93bVisitor *)v;
  VisitorType vt = v->GetType();
  int i;

  if (vt.Is_A(_port_installer_type))
    i = piv->GetOutPort();
  else if (vt.Is_A(_port_visitor_type))
    i = pv->GetOutPort();
  else if (vt.Is_A(_q93b_visitor_type))
    i = qv->get_port();
  else return 0;

  if (!_access_map.lookup(i))
    return false;
  _access_map.del(i);
  return true;
}
