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
 * File: PortVisitor.cc
 * @author talmage
 * @version $Id: PortVisitor.cc,v 1.37 1998/12/01 13:39:59 mountcas Exp $
 * Base class for a hierarchy of classes that travel through Ports 
 * in a Switch and possibly over Links to other Ports in other Switches.
 * BUGS: none!
 */

#ifndef LINT
static char const rcsid[] =
"$Id: PortVisitor.cc,v 1.37 1998/12/01 13:39:59 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <fsm/visitors/PortVisitor.h>
#include <FW/basics/diag.h>

// --------------------- PortVisitor ---------------------------
PortVisitor::PortVisitor(int out_port, int agg_tok) :
  Visitor(_mytype), _out_port(out_port), 
  _in_port(-1), _aggregation_token(agg_tok)
{ }

PortVisitor::~PortVisitor() { }

PortVisitor::PortVisitor(vistype &child_type, int out_port, int agg_tok) :
  Visitor(child_type.derived_from(_mytype)), _out_port(out_port), 
  _in_port(-1), _aggregation_token(agg_tok)
{ }

PortVisitor::PortVisitor(const PortVisitor& rhs) 
  : Visitor(rhs), _in_port(rhs._in_port), 
    _out_port(rhs._out_port), _aggregation_token(rhs._aggregation_token) 
{ }

Visitor * PortVisitor::dup(void) const
{  return new PortVisitor(*this);  }

void PortVisitor::on_death(void) const
{
  DIAG("fsm.visitors.dumplog", DIAG_INFO, 
       if (DumpLog(cout)) cout << endl;);
}


const vistype &PortVisitor::GetClassType(void) const
{
  return _mytype;
}

int PortVisitor::GetOutPort() const { return _out_port; }

void PortVisitor::SetOutPort(int port) { _out_port = port; }

int PortVisitor::GetInPort() const { return _in_port; }

void PortVisitor::SetInPort(int port) { _in_port = port; }

void PortVisitor::SetAgg(int agg) { _aggregation_token = agg; }

int  PortVisitor::GetAgg(void) const { return _aggregation_token; }

const VisitorType PortVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }

PortVisitor & PortVisitor::operator = (const PortVisitor & rhs)
{
  _in_port  = rhs._in_port;
  _out_port = rhs._out_port;
  _aggregation_token = rhs._aggregation_token;
  (*(Visitor *)this = (Visitor &)rhs);
  return *this;
}
