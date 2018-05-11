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
#ifndef __LOGOSGRAPH_VISITOR_H__
#define __LOGOSGRAPH_VISITOR_H__

#ifndef LINT
static char const _LogosGraphVisitor_h_rcsid_[] =
"$Id: LogosGraphVisitor.h,v 1.1 1999/02/16 00:29:28 bilal Exp $";
#endif

#include <FW/basics/Visitor.h>
#define LOGOS_GRAPH_VISITOR_NAME "LogosGraphVisitor"

class LogosGraph;

class LogosGraphVisitor : public Visitor {
public:

  LogosGraphVisitor(int level);
  ~LogosGraphVisitor();

  void         GiveGraph(LogosGraph *& g);
  LogosGraph * TakeGraph(void);

  const int GetLevel(void) const;

  virtual const VisitorType GetType(void) const;

protected:
  
  // This is for use only between the Aggregator and Logos Protocols,
  //  anywhere else it will phreak out and commit suicide.
  virtual void at(Mux * m, Accessor * a);
  virtual Conduit * at(Factory * f, Creator * c);
  virtual void at(Adapter * a, Terminal * t);

private:

  // Do not define (this keeps g++ from doing it too
  LogosGraphVisitor(const LogosGraphVisitor & );
  LogosGraphVisitor & operator = (const LogosGraphVisitor & );
  // end of 'Do not define'

  virtual const vistype & GetClassType(void) const;

  int          _level;
  LogosGraph * _graph;

  static vistype _my_type;
};

#endif
