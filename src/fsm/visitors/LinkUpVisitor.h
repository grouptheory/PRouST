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
 * File: LinkUpVisitor.h
 * @author talmage
 * @version $Id: LinkUpVisitor.h,v 1.8 1998/08/11 18:32:18 mountcas Exp $
 *
 * Indicates that some link is up.
 *
 * BUGS:
 */

#ifndef __LINK_UP_VISITOR_H__
#define __LINK_UP_VISITOR_H__
#ifndef LINT
static char const _LinkUpVisitor_h_rcsid_[] =
"$Id: LinkUpVisitor.h,v 1.8 1998/08/11 18:32:18 mountcas Exp $";
#endif

#include <fsm/visitors/LinkVisitor.h>

#define LINK_UP_VISITOR_NAME "LinkUpVisitor"

class VCAllocator;

class LinkUpVisitor : public LinkVisitor {
public:

  LinkUpVisitor(int port = -1, int agg = -1, NodeID * local = 0, NodeID * remote = 0);

  virtual const VisitorType GetType(void) const;

  VCAllocator* GetVCAllocator(void);
  void SetVCAllocator(VCAllocator* vca);

protected:

  virtual Visitor * dup(void) const;

  virtual ~LinkUpVisitor();
  LinkUpVisitor(const LinkUpVisitor & rhs);
  LinkUpVisitor(vistype & child_type, int port = -1, int agg = -1, NodeID * local = 0, NodeID * remote = 0);
  virtual const vistype & GetClassType(void) const;

private:

  static vistype _mytype;  
  VCAllocator* _vca;
};

#endif // __LINKUPVISITOR_H__
