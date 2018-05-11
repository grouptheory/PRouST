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

/* -*- C++ -*-
 * File: PortInstallerVisitor.h
 * Author: 
 * Version: $Id: PortInstallerVisitor.h,v 1.22 1999/02/05 17:29:56 mountcas Exp $
 * Purpose: Instructs a Switch conduit to add a new port to itself.
 * BUGS:
 */
#ifndef __PORT_INSTALLER_VISITOR_H__
#define __PORT_INSTALLER_VISITOR_H__

#ifndef LINT
static char const _PortInstallerVisitor_h_rcsid_[] =
"$Id: PortInstallerVisitor.h,v 1.22 1999/02/05 17:29:56 mountcas Exp $";
#endif

#include <FW/basics/VisitorType.h>
#include <fsm/visitors/PortVisitor.h>

class NodeID;

#define PORT_INSTALLER_VISITOR_NAME "PortInstallerVisitor"
class PortInstallerVisitor : public PortVisitor {
public:

  PortInstallerVisitor(int p, NodeID * node);

  virtual const VisitorType GetType(void) const;

  const NodeID * GetNode(void);
  void SetNode(NodeID * node);

  NodeID * StealNode(void);	// Returns _node then sets _node to 0

protected:

  PortInstallerVisitor(vistype & child_vtype, int p, NodeID *node);

  virtual ~PortInstallerVisitor();

private:

  NodeID *_node;

  virtual const vistype& GetClassType(void) const;

  static vistype _my_type;
};

#endif  // __PORT_INSTALLER_VISITOR_H__






