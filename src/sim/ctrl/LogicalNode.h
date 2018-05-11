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
#ifndef __LOGICALNODE_H__
#define __LOGICALNODE_H__

#ifndef LINT
static char const _LogicalNode_h_rcsid_[] =
"$Id: LogicalNode.h,v 1.29 1999/02/10 18:24:51 mountcas Exp $";
#endif

#include <FW/actors/Expander.h>
#include <FW/actors/Creator.h>

class NodeID;
class Database;
class ElectionState;

/** The Logical Node Expander encapsulates the information necessary for an uplink.
    It is created by the LogicalNodeExpCreator - see Control.h
    It contains a VC-to-Remote Node ID mapper, two muxes, an ElectionFSM, and a
    LogicalSVCExpCreator Factory.
 */
class LogicalNodeExp : public Expander {
public:

  /// Constructor, the Database is stored so that it can
  ///  be used by the LogicalSVCExpCreator.
  LogicalNodeExp( const NodeID * logical_nid );
  /// Destructor
  virtual ~LogicalNodeExp();

  /// Returns the logical A-half of the LogicalNodeExp (the VC-to-Remote Node ID mapper)
  Conduit * GetAHalf(void) const;
  /// Returns the logical B-half of the LogicalNodeExp (the ElectionFSM)
  Conduit * GetBHalf(void) const;

  void SetConduit( Conduit * c );

private:

  /// VC to Remote NodeID mapper
  Conduit * _nid2vc;
  /// Pointer to Source Node ID Mux
  Conduit * _topmux;
  /// Pointer to Destination Node ID Mux
  Conduit * _botmux;
  /// Pointer to the LogicalSVCExpCreator Factory
  Conduit * _factory;
  /// Pointer to the Election FSM
  Conduit * _election;

  ElectionState * _elefsm;
};

/** Creator used by the Logical Node Expander to allocate LogicalSVCExp's.
 */
class LogicalSVCExpCreator : public Creator {
public:

  /// Constructor, Database is stored so that it can 
  ///  be passed to all allocated LogicalSVCExps.
  LogicalSVCExpCreator( const NodeID * logical_nid );

  /// Destructor
  virtual ~LogicalSVCExpCreator( );

  /// Method which allocates new LogicalSVCExp's
  Conduit * Create(Visitor * v);
  /// Method which handles incoming interrupts
  void Interrupt(SimEvent* e);

private:

  ///
  int              _counter;
  /// Our Logical NodeID
  NodeID         * _logical_nid;

  static const VisitorType * _lgn_inst_type;
  static const VisitorType * _fast_uni_type;
};

#endif // __LOGICALNODE_H__
