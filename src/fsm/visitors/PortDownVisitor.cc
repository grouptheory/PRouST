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
 * File: PortDownVisitor.cc
 * @author talmage
 * @version $Id: PortDownVisitor.cc,v 1.11 1998/08/10 20:54:36 mountcas Exp $
 *
 * Indicates that a port is down.
 *
 * BUGS: Too many to count.
 */
#ifndef LINT
static char const rcsid[] =
"$Id: PortDownVisitor.cc,v 1.11 1998/08/10 20:54:36 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/addr.h>
#include "PortDownVisitor.h"

// --------------------- PortDownVisitor ---------------------------
PortDownVisitor::PortDownVisitor(int port, NodeID *local_node, int local_port, 
				 NodeID *remote_node, int remote_port,
				 int aggregation_token) :
  LinkVisitor(_mytype,port, aggregation_token, local_node, remote_node)
{
  SetLocalPort(local_port);
  SetRemotePort(remote_port);
}

PortDownVisitor::PortDownVisitor(const PortDownVisitor & rhs) :
  LinkVisitor(rhs) { }

PortDownVisitor::PortDownVisitor(vistype &child_type, int port,
			     NodeID *local_node, int local_port, 
			     NodeID *remote_node, int remote_port,
			     int aggregation_token) :
  LinkVisitor(child_type.derived_from(_mytype), port, aggregation_token,
	      local_node, remote_node)
{
  SetLocalPort(local_port);
  SetRemotePort(remote_port);
}

PortDownVisitor::~PortDownVisitor() { }

const vistype &PortDownVisitor::GetClassType(void) const
{
  return _mytype;
}

const VisitorType PortDownVisitor::GetType(void) const 
{  return VisitorType(GetClassType());  }

Visitor * PortDownVisitor::dup(void) const { return new PortDownVisitor(*this); }
