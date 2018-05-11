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
 * File: LinkVisitor.h
 * @author talmage
 * @version $Id: LinkVisitor.h,v 1.28 1998/10/20 13:05:48 mountcas Exp $
 *
 * Carries Link messages around in a switch.
 *
 * BUGS:
 */
#ifndef __LINK_VISITOR_H__
#define __LINK_VISITOR_H__

#ifndef LINT
static char const _LinkVisitor_h_rcsid_[] =
"$Id: LinkVisitor.h,v 1.28 1998/10/20 13:05:48 mountcas Exp $";
#endif

#include <fsm/visitors/NodeIDVisitor.h>
#include <DS/containers/list.h>

class NodeID;
class ig_resrc_avail_info;

#define LINK_VISITOR_NAME "LinkVisitor"

extern const u_long NO_BW;
extern const u_long OC3;
extern const u_long OC12;
extern const u_long OC48;
extern const u_long OC192;

class LinkVisitor : public NodeIDVisitor {
  friend ig_resrc_avail_info * CreateRAIG(long bandwidth = 0);
public:
  // local == source, remote == destination
  LinkVisitor(int port = -1, int agg = -1, 
	      const NodeID * local = 0, 
	      const NodeID * remote = 0);

  virtual const VisitorType GetType(void) const;

  const int GetLocalPort(void) const;
  const int GetRemotePort(void) const;

  void      SetLocalPort(int port);
  void      SetRemotePort(int port);

  // You call this, you own it -- don't screw up.
  list<ig_resrc_avail_info *> * GetRAIGs(void);
  void                          SetRAIGs(const list<ig_resrc_avail_info *> * raigs);
  void                          AddRAIG(ig_resrc_avail_info * raig);

protected:

  virtual Visitor * dup(void) const;

  virtual ~LinkVisitor();

  virtual void on_death(void) const;

  LinkVisitor(const LinkVisitor & rhs);
  LinkVisitor(vistype & child_type, int port = -1, int agg = -1, 
	      const NodeID * local = 0, const NodeID * remote = 0);

  virtual const vistype & GetClassType(void) const;

  int                   _local_port;
  int                   _remote_port;
  list<ig_resrc_avail_info *> * _raigs;

private:

  static vistype _mytype;
};

#endif // __LINKVISITOR_H__
