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
#ifndef __DBVISITORS_H__
#define __DBVISITORS_H__

#ifndef LINT
static char const _DBVisitors_h_rcsid_[] =
"$Id: DBVisitors.h,v 1.7 1998/08/11 18:31:07 mountcas Exp $";
#endif

#include <FW/basics/Visitor.h>

class Database;

extern "C" {
#include <assert.h>
};

#define DB_INTRO_VISITOR_NAME "DBIntroVisitor"

class DBIntroVisitor : public Visitor {
public:
  
  DBIntroVisitor(Database * db);
  DBIntroVisitor(const DBIntroVisitor & rhs);

  virtual const VisitorType GetType(void) const 
    { return VisitorType(GetClassType()); }

  Database * GetDatabase(void);

protected:

  virtual Visitor * dup(void) const;

  virtual void at(Mux * m, Accessor * a);

  DBIntroVisitor(vistype& child_type, Database * db);
  virtual ~DBIntroVisitor( );
  virtual void on_death(void) const;

  virtual const vistype& GetClassType(void) const { return _my_type; }

  Database    * _db;
  static vistype  _my_type;
};

#endif // __DBVISITORS_H__
