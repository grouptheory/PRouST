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
 * File: StrikeVisitor.h
 * @author talmage
 * @version $Id: StrikeVisitor.h,v 1.10 1998/08/11 20:29:34 mountcas Exp $
 *
 * Some Creator objects recognize the concept of "on strike".  When 
 * those Creators are visited by a StrikeVisitor, they stop producing.
 * A visit by a StrikeBreakerVisitor brings the Creator back "on line".
 *
 * StrikeVisitor assumes that any Creator::Create() that returns zero
 * obeys the Strike/StrikeBreaker protocol.  It Suicide()s when Create()
 * returns zero.  It allows itself to be sent on when Create() does not
 * return zero.
 *
 * BUGS:
 */

#ifndef __STRIKE_VISITOR_H__
#define __STRIKE_VISITOR_H__
#ifndef LINT
static char const _StrikeVisitor_h_rcsid_[] =
"$Id: StrikeVisitor.h,v 1.10 1998/08/11 20:29:34 mountcas Exp $";
#endif

#include <FW/basics/Visitor.h>

class Creator;
class Factory;

#define STRIKE_VISITOR_NAME "StrikeVisitor"
class StrikeVisitor : public Visitor {
public:

  StrikeVisitor(void);

  virtual const VisitorType GetType(void) const { 
    return VisitorType(GetClassType());
  }

protected:

  virtual Visitor * dup(void) const { return new StrikeVisitor(); }

  StrikeVisitor(vistype &child_type);
  virtual ~StrikeVisitor();
  virtual const vistype &GetClassType(void) const;

private:

  static vistype _mytype;
};


#endif
