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
 * File: PortDownVisitor.h
 * @author talmage
 * @version $Id: PortDownVisitor.h,v 1.10 1998/08/11 18:33:22 mountcas Exp $
 *
 * Indicates that some port is down.
 *
 * BUGS:
 */

#ifndef __PORT_DOWN_VISITOR_H__
#define __PORT_DOWN_VISITOR_H__
#ifndef LINT
static char const _PortDownVisitor_h_rcsid_[] =
"$Id: PortDownVisitor.h,v 1.10 1998/08/11 18:33:22 mountcas Exp $";
#endif

#include <fsm/visitors/LinkVisitor.h>

class Addr;
class NodeID;

#define PORT_DOWN_VISITOR_NAME "PortDownVisitor"
class PortDownVisitor : public LinkVisitor {
public:

  PortDownVisitor(int port = 0, NodeID *local_node  = 0, int local_port = 1,
		  NodeID *remote_node = 0, int remote_port = 1,
		  int agg = 0);

  virtual const VisitorType GetType(void) const;

protected:

  virtual Visitor * dup(void) const;

  virtual ~PortDownVisitor();

  PortDownVisitor(const PortDownVisitor & rhs);

  PortDownVisitor(vistype &child_type, int port = 0,
		  NodeID *local_node  = 0, int local_port = 1,
		  NodeID *remote_node = 0, int remote_port = 1,
		  int agg = 0);

  virtual const vistype &GetClassType(void) const;

private:

  static vistype _mytype;
};

#endif
