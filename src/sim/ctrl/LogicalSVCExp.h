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
#ifndef __LOGICALSVCEXP_H__
#define __LOGICALSVCEXP_H__

#ifndef LINT
static char const _LogicalSVCExp_h_rcsid_[] =
"$Id: LogicalSVCExp.h,v 1.24 1999/02/10 18:35:32 mountcas Exp $";
#endif

#include <DS/containers/dictionary.h>
#include <FW/actors/Expander.h>
#include <FW/actors/Creator.h>

class NodeID;
class Database;
class AggAccessor;

/** The LogicalSVC Expander is the major component of a Logical Node.
    It contains, the SVCHelloFSM, a NodePeerFSM, two muxes, and a HelloFSM Factory.
 */
class LogicalSVCExp : public Expander {
public:

  /// Constructor, passes the remote NodeID to the NodePeerFSM, 
  ///   the Database is used by both the HelloFSM and NodePeerFSM.
  LogicalSVCExp( const NodeID * logical, const NodeID * remote,
		 int vpi = 0, int vci = -1, u_long cref = 0xffffffff );
  /// Destructor
  virtual ~LogicalSVCExp();

  /// Returns the logical A-half of the LogicalSVC (in this case the SVC Hello FSM)
  Conduit * GetAHalf(void) const;
  /// Returns the logical B-half of the LogicalSVC (in this case the NodePeer FSM)
  Conduit * GetBHalf(void) const;

  /// Tell me about my conduit
  void InformOfConduit(Conduit* conduit);

private:

  /// HelloFSM which keeps the SVC alive
  Conduit * _rcc_hello;
  /// Upper mux keyed on Aggregation Token
  Conduit * _topmux;
  /// Lower mux keyed on Aggregation Token
  Conduit * _botmux;
  /// HelloFSMCreator Factory
  Conduit * _factory;
  /// NodePeerFSM
  Conduit * _nodepeer;
  
  // The accessor in _botmux;
  AggAccessor* _lower_aggtok_accessor;
};

class LgnHelloState;

/** The HelloFSMCreator is used in the LogicalSVCExp to create new Hello FSMs.
 */
class LgnHelloFSMCreator : public Creator {
  friend class LogicalSVCExp;
  friend class LgnHelloState;
public:

  /// Constructor, Database is passed to all allocated HelloFSMs.
  LgnHelloFSMCreator( const NodeID * localNode, 
		      const NodeID * remoteNode );
  /// Destructor
  virtual ~LgnHelloFSMCreator();

  /// Method which allocates new Hello FSMs
  Conduit * Create(Visitor * v);
  /// Method to handle events.
  void Interrupt(SimEvent* e);

private:

  bool KillLgnHello(int aggTok);
  bool LgnHelloDeparting(int aggTok);

  dictionary<int, LgnHelloState *> _agg2lgn;

  ///
  const NodeID   * _logical_nid;
  const NodeID   * _logical_rnid;

  static VisitorType * _fastuni_vistype;
  static VisitorType * _horlink_vistype;
  static VisitorType * _lgninst_vistype;
};

#endif // __LOGICALSVCEXP_H__
