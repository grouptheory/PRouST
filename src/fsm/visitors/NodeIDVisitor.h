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
#ifndef __NODEIDVISITOR_H__
#define __NODEIDVISITOR_H__

#ifndef LINT
static char const _NodeIDVisitor_h_rcsid_[] =
"$Id: NodeIDVisitor.h,v 1.8 1998/10/19 19:36:14 mountcas Exp $";
#endif

#include <fsm/visitors/PortVisitor.h>

#define NODEID_VISITOR_NAME "NodeIDVisitor"

class NodeID;

class NodeIDVisitor : public PortVisitor {
public:

  NodeIDVisitor(int oport = -1, int aggtok = -1, 
		const NodeID * source = 0, const NodeID * dest = 0);
  NodeIDVisitor(const NodeIDVisitor & rhs);

  NodeIDVisitor & operator = (const NodeIDVisitor & rhs);

  // Share the NodeID;
  const NodeID * GetSourceNID(void) const;
        NodeID * TakeSourceNID(void);
  // This method makes a copy
  void           SetSourceNID(const NodeID * nid);

  const NodeID * GetDestNID(void) const;
        NodeID * TakeDestNID(void);
  // This method makes a copy
  void           SetDestNID(const NodeID * nid);

  virtual const VisitorType GetType(void) const;

protected:

  NodeIDVisitor(vistype & child_type, int oport = -1, int aggtok = -1,
		const NodeID * source = 0, const NodeID * destination = 0);

  virtual Visitor * dup(void) const;
  
  virtual ~NodeIDVisitor();

  virtual const vistype & GetClassType(void) const;

private:

  const NodeID * _source;
  const NodeID * _destination;

  static vistype _myType;
};

#endif
