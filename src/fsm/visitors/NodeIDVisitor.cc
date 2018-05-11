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
static char const _NodeIDVisitor_cc_rcsid_[] =
"$Id: NodeIDVisitor.cc,v 1.9 1998/11/25 17:25:21 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "NodeIDVisitor.h"
#include <codec/pnni_ig/id.h>

NodeIDVisitor::NodeIDVisitor(int oport, int aggtok, 
		const NodeID * source, const NodeID * dest)
  : PortVisitor(_myType, oport, aggtok), _source(0), _destination(0)
{
  if (source)
    _source = new NodeID(*source);
  if (dest)
    _destination = new NodeID(*dest);
}

NodeIDVisitor::NodeIDVisitor(const NodeIDVisitor & rhs)
  : PortVisitor(rhs), _source(0), _destination(0)
{
  if (rhs._source)
    _source = new NodeID(*(rhs._source));
  if (rhs._destination)
    _destination = new NodeID(*(rhs._destination));
}

NodeIDVisitor::NodeIDVisitor(vistype & child_type, int oport, int aggtok,
			     const NodeID * source, const NodeID * destination)
  : PortVisitor(child_type.derived_from(_myType), oport, aggtok), 
    _source(0), _destination(0)
{
  if (source)
    _source = new NodeID(*source);
  if (destination)
    _destination = new NodeID(*destination);
}

NodeIDVisitor & NodeIDVisitor::operator = (const NodeIDVisitor & rhs)
{
  delete _source; _source = 0;
  if (rhs._source)
    _source = new NodeID(*(rhs._source));

  delete _destination; _destination = 0;
  if (rhs._destination)
    _destination = new NodeID(*(rhs._destination));

  return (NodeIDVisitor &)(*(PortVisitor *)this = (PortVisitor &)rhs);
}

const NodeID * NodeIDVisitor::GetSourceNID(void) const
{
  return _source;
}

NodeID * NodeIDVisitor::TakeSourceNID(void)
{
  NodeID * rval = (NodeID *)_source;  // we cannot return const because the caller may delete it
  _source = 0;
  return rval;
}

void NodeIDVisitor::SetSourceNID(const NodeID * nid)
{
  delete _source;  // If you think this should be uncommented speak with Bilal
  _source = 0;
  if (nid)
    _source = nid->copy();
}

const NodeID * NodeIDVisitor::GetDestNID(void) const
{
  return _destination;
}

NodeID * NodeIDVisitor::TakeDestNID(void)
{
  NodeID * rval = (NodeID *)_destination;  // we cannot return const because the caller may delete it
  _destination = 0;
  return rval;
}

void NodeIDVisitor::SetDestNID(const NodeID * nid)
{
  delete _destination;  // If you think this should be uncommented talk to Bilal
  _destination = 0;
  if (nid)
    _destination = nid->copy();
}

const VisitorType NodeIDVisitor::GetType(void) const
{
  return VisitorType(_myType);
}

Visitor * NodeIDVisitor::dup(void) const
{
  return new NodeIDVisitor(*this);
}
  
NodeIDVisitor::~NodeIDVisitor()
{
  delete _source;
  delete _destination;
}

const vistype & NodeIDVisitor::GetClassType(void) const
{
  return _myType;
}
