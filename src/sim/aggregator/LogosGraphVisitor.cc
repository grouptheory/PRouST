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
static char const _LogosGraphVisitor_cc_rcsid_[] =
"$Id: LogosGraphVisitor.cc,v 1.1 1999/02/16 00:29:28 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include "LogosGraphVisitor.h"

LogosGraphVisitor::LogosGraphVisitor(int level) 
  : Visitor(_my_type), _graph(0), _level(level) { }

LogosGraphVisitor::~LogosGraphVisitor() 
{
  // May I delete the graph?
}

void         LogosGraphVisitor::GiveGraph(LogosGraph *& g)
{
  _graph = g;
  g = 0;
}

LogosGraph * LogosGraphVisitor::TakeGraph(void)
{
  LogosGraph * rval = _graph;
  _graph = 0;
  return rval;
}

const int LogosGraphVisitor::GetLevel(void) const
{
  return _level;
}

const VisitorType LogosGraphVisitor::GetType(void) const 
{ 
  return VisitorType(GetClassType());
}

const vistype & LogosGraphVisitor::GetClassType(void) const 
{ return _my_type; }

void LogosGraphVisitor::at(Mux * m, Accessor * a)
{
  // Goodbye cruel world!
  Suicide();
}

Conduit * LogosGraphVisitor::at(Factory * f, Creator * c)
{
  // Goodbye cruel world!
  Suicide();
  return 0;
}

void LogosGraphVisitor::at(Adapter * a, Terminal * t)
{
  // Goodbye cruel world!
  Suicide();
}