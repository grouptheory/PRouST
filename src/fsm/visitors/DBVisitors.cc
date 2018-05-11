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

#ifndef LINT
static char const _DBVisitors_cc_rcsid_[] =
"$Id: DBVisitors.cc,v 1.19 1998/08/11 16:54:58 mountcas Exp $";
#endif
#include <common/cprototypes.h>

extern "C" {
#include <stdio.h>
#include <assert.h>
};

#include <fsm/visitors/DBVisitors.h>
#include <FW/basics/Conduit.h>
#include <FW/behaviors/Mux.h>

#include <FW/basics/diag.h>

DBIntroVisitor::DBIntroVisitor(Database * db) : Visitor(_my_type), _db(db)
{
  SetLoggingOn();
}

DBIntroVisitor::DBIntroVisitor(const DBIntroVisitor & rhs) :
  Visitor(rhs), _db(rhs._db) {
    SetLoggingOn();
}

DBIntroVisitor::DBIntroVisitor(vistype & child_type, Database * db) : 
  Visitor(child_type.derived_from(_my_type)), _db(db)
{
  SetLoggingOn();
}

DBIntroVisitor::~DBIntroVisitor( ) { }

void DBIntroVisitor::on_death(void) const
{
  DIAG(FSM, DIAG_INFO, 
       if (DumpLog(cout)) cout << endl;);
}

// Everywhere else it acts as a Visitor.
void DBIntroVisitor::at(Mux * m, Accessor * a)
{
  Conduit * dest;

  assert(m && a);

  switch(EnteredFrom()) {
  case A_SIDE:
    SetLast( CurrentConduit() );
    
    if (a) {
      // Send the visitor to the factory
      SideB()->Accept(this->dup());

      // Send the visitor to the other Conduits in the Mux
      m->Broadcast(this);
    }
    else if (dest = SideB()) 
        dest->Accept(this);
    else Suicide();
    break;
  case B_SIDE:
  case OTHER:
    // Call at::(Behavior)
    Visitor::at(m);
    break;
  }
}

Database * DBIntroVisitor::GetDatabase(void)
{ 
  return _db; 
}

Visitor * DBIntroVisitor::dup(void) const
{
  return new DBIntroVisitor(*this);
}
