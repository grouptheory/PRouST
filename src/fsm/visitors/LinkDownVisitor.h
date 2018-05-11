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
 * File: LinkDownVisitor.h
 * @author talmage
 * @version $Id: LinkDownVisitor.h,v 1.10 1998/08/11 18:32:07 mountcas Exp $
 *
 * Indicates that some link is down.
 *
 * BUGS:
 */

#ifndef __LINK_DOWN_VISITOR_H__
#define __LINK_DOWN_VISITOR_H__
#ifndef LINT
static char const _LinkDownVisitor_h_rcsid_[] =
"$Id: LinkDownVisitor.h,v 1.10 1998/08/11 18:32:07 mountcas Exp $";
#endif

#include <fsm/visitors/LinkVisitor.h>

#define LINK_DOWN_VISITOR_NAME "LinkDownVisitor"

class LinkDownVisitor : public LinkVisitor {
public:

  LinkDownVisitor(int port = -1, int agg = -1,
		  NodeID * local = 0, NodeID * remote = 0);
  LinkDownVisitor(const LinkDownVisitor & rhs);

  virtual const VisitorType GetType(void) const;

protected:

  virtual Visitor * dup(void) const;

  virtual ~LinkDownVisitor();
  LinKDownVisitor(const LinkDownVisitor & rhs);
  LinkDownVisitor(vistype &child_type, int port = -1, int agg = -1, NodeID * local = 0, NodeID * remote = 0);
  virtual const vistype &GetClassType(void) const;

private:

  static vistype _mytype;
};

#endif // __LINK_DOWN_VISITOR_H__
