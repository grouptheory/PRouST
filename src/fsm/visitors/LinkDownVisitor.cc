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

/** -*- C++ -*-
 * File: LinkDownVisitor.cc
 * @author talmage
 * @version $Id: LinkDownVisitor.cc,v 1.9 1998/08/10 20:52:57 mountcas Exp $
 *
 * Indicates that a link is down.
 *
 * BUGS:
 */

#ifndef LINT
static char const rcsid[] =
"$Id: LinkDownVisitor.cc,v 1.9 1998/08/10 20:52:57 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <fsm/visitors/QueueVisitor.h>
#include <fsm/visitors/LinkDownVisitor.h>

// --------------------- LinkDownVisitor ---------------------------

LinkDownVisitor::LinkDownVisitor(int port, int agg, NodeID * local, NodeID * remote) : 
  LinkVisitor(_mytype, port, agg, local, remote)
{
  // LinkDownVisitors have the override property!
  _mytype.derived_from(QueueOverrideType());
}

LinkDownVisitor::LinkDownVisitor(vistype &child_type, int port, int agg, NodeID * local, NodeID * remote):
  LinkVisitor(child_type.derived_from(_mytype), port, agg, local, remote)
{
  // Visitors derived from LinkDownVisitors are inherit the override property too!
  _mytype.derived_from(QueueOverrideType());
}

LinkDownVisitor::LinkDownVisitor(const LinkDownVisitor & rhs) :
  LinkVisitor(rhs) {  _mytype.derived_from(QueueOverrideType());  }

LinkDownVisitor::~LinkDownVisitor() { }

Visitor * LinkDownVisitor::dup(void) const { return new LinkDownVisitor(*this); }

const vistype & LinkDownVisitor::GetClassType(void) const
{  return _mytype;  }

const VisitorType LinkDownVisitor::GetType(void) const
{  return VisitorType(GetClassType()); }
