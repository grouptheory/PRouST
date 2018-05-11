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
 * File: Q93BCreator.h
 * Author: battou
 * Version: $Id: Q93bCreator.h,v 1.7 1999/02/10 19:10:59 mountcas Exp $
 * Purpose:
 * BUGS:
 */
#ifndef __Q93B_CREATOR_H__
#define __Q93B_CREATOR_H__
#ifndef LINT
static char const _Q93bCreator_h_rcsid_[] =
"$Id: Q93bCreator.h,v 1.7 1999/02/10 19:10:59 mountcas Exp $";
#endif

#include <FW/actors/Creator.h>
#include <codec/pnni_ig/id.h>

class VCAllocator;
class VisitorType;
class LinkUpVisitor;

// Allocates only Q93Bs
class Q93bCreator : public Creator {
public:

  Q93bCreator(int port, const NodeID * node, VCAllocator * vcpool = 0);

  virtual Conduit * Create(Visitor * v);
  void Interrupt(SimEvent* e) { }

protected:

  virtual ~Q93bCreator(void);

  VCAllocator * _vcpool;
  NodeID      * _node;
  int           _port;
  int           _counter;

  static const VisitorType* _uni_visitor_type;
  static const VisitorType* _linkup_visitor_type;
};

#endif // __Q93B_CREATOR_H__
