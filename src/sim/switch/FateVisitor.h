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
#ifndef __FATEVISITOR_H__
#define __FATEVISITOR_H__

#ifndef LINT
static char const _FateVisitor_h_rcsid_[] =
"$Id$";
#endif

#include <FW/basics/Visitor.h>

#define FATE_VISITOR_NAME "FateVisitor"

class FastUNIVisitor;

class FateVisitor : public Visitor {
public:

  FateVisitor( const FastUNIVisitor * vp, long cref );
  virtual ~FateVisitor( );

  virtual const VisitorType GetType(void) const;

  const FastUNIVisitor * getVP( void ) const;
  const long getCREF( void ) const;

protected:

  virtual const vistype & GetClassType(void) const;
  virtual Visitor * dup(void) const;

private:

  const FastUNIVisitor * _vp;
  long                   _cref;

  static vistype         _my_type;
};

#endif
