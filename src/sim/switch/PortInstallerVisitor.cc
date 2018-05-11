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
 * File: PortInstallerVisitor.cc
 * Author: 
 * Version: $Id: PortInstallerVisitor.cc,v 1.21 1999/02/05 17:28:23 mountcas Exp $
 * Purpose: Instructs a Switch conduit to add a new port to itself.
 * BUGS:
 */
#ifndef LINT
static char const rcsid[] =
"$Id: PortInstallerVisitor.cc,v 1.21 1999/02/05 17:28:23 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "PortInstallerVisitor.h"
#include <FW/actors/Creator.h>
#include <FW/basics/Conduit.h>
#include <codec/pnni_ig/id.h>

// --------------------------- PortInstallerVisitor -----------------------

PortInstallerVisitor::PortInstallerVisitor(int p, NodeID *node) :
  PortVisitor(_my_type, p, -1), _node(node)
{ 
  SetLoggingOn();
}


PortInstallerVisitor::PortInstallerVisitor(vistype & child_vtype, int p, 
					   NodeID *node)
  : PortVisitor(child_vtype.derived_from(_my_type), p, -1), _node(node)
{
}


PortInstallerVisitor::~PortInstallerVisitor()
{
  delete _node;
}


const VisitorType PortInstallerVisitor::GetType(void) const
{
  return VisitorType(GetClassType());
}


const NodeID *PortInstallerVisitor::GetNode(void)
{
  return _node;
}


void PortInstallerVisitor::SetNode(NodeID *node)
{
  _node = node;
}


// Returns _node then sets _node to 0
NodeID *PortInstallerVisitor::StealNode(void)
{
  NodeID * answer = _node;
  _node = 0;
  return answer;
}

const vistype & PortInstallerVisitor::GetClassType(void) const 
{
  return _my_type; 
}
