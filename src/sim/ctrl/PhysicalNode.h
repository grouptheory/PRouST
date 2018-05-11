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
#ifndef __PHYSICALNODE_H__
#define __PHYSICALNODE_H__

#ifndef LINT
static char const _PhysicalNode_h_rcsid_[] =
"$Id: PhysicalNode.h,v 1.17 1999/02/10 18:55:46 mountcas Exp $";
#endif

#include <FW/actors/Expander.h>
#include <FW/actors/Creator.h>

class NodeID;
class ElectionState;

/** The Physical Node Expander represents the lowest level physical node.
 */
class PhysicalNodeExp : public Expander {
public:

  /// Constructor, Database is used by the NodePeerFSM Creator
  PhysicalNodeExp(const NodeID * nid);
  /// Destructor
  virtual ~PhysicalNodeExp();

  /// Returns the logical A-half of the Physical Node 
  Conduit * GetAHalf(void) const;
  /// Returns the logical B-half of the Physical Node 
  Conduit * GetBHalf(void) const;

private:

  /// Pointer to the upper .... Accessor Mux
  Conduit * _topmux;
  /// Pointer to the lower .... Accessor Mux
  Conduit * _botmux;
  /// Pointer to the NodePeerState Factory
  Conduit * _factory;
  /// Pointer to the ElectionState FSM
  Conduit * _election;

  ElectionState * _elefsm;
};

/** Allocates the NodePeerFSMs inside the Physical Node
 */
class NodePeerFSMCreator : public Creator {
public:

  /// Constructor, Database is passed to all allocated NodePeerFSMs
  NodePeerFSMCreator( void );

  /// Returns the newly allocated NodePeerState FSM
  Conduit * Create(Visitor * v);
  /// Handles interrupts
  void Interrupt(SimEvent* e);

  /// Destructor
  virtual ~NodePeerFSMCreator();

private:

  ///
  int              _counter;
};

#endif // __LOGICALNODE_H__
