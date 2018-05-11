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
#ifndef __LEADERSHIPNOTIFYVISITOR_H__
#define __LEADERSHIPNOTIFYVISITOR_H__

#ifndef LINT
static char const _LeadershipNotifyVisitor_h_rcsid_[] =
"$Id: LeadershipNotifyVisitor.h,v 1.1 1998/06/11 19:12:02 mountcas Exp $";
#endif

// Used between the Database and Leadership to notify 
// Leadership when the DB receives a nodal info group
// for a parent node, so Leadership can update the 
// nodal hierarchy list for this node.
class LeadershipNotifyVisitor {
public:

  LeadershipNotifyVisitor(ig_nodal_info_group * nodal_ig);
  ~LeadershipNotifyVisitor();

  virtual const VisitorType GetType(void) const;

  const ig_nodal_info_group * GetNodalIG(void) const;

protected:

  LeadershipNotifyVisitor(vistype & child_type);
  LeadershipNotifyVisitor(const LeadershipNotifyVisitor & rhs);

  virtual const vistype & GetClassType(void) const;

private:
  
  ig_nodal_info_group * _nodal_ig;
  static vistype        _mytype;
};

#endif // __LEADERSHIPNOTIFYVISITOR_H__
