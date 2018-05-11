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
#ifndef __AGGREGATOR_H__
#define __AGGREGATOR_H__

#ifndef LINT
static char const _Aggregator_h_rcsid_[] =
"$Id: Aggregator.h,v 1.3 1999/02/19 22:42:17 marsh Exp $";
#endif

#include <FW/actors/State.h>

class AggregationPolicy;
class FastUNIVisitor;
class NPFloodVisitor;
class HorLinkVisitor;
class LogosGraphVisitor;

class NodeID;
class LogicalPeer;
class VisitorType;
class ComplexRep;

#include <DS/containers/dictionary.h>
#include <DS/containers/queue.h>
#include <DS/containers/list.h>
#include <codec/pnni_ig/id.h>

#define QUEUE_SERVICE_SIM_EVENT_CODE 0xAFFA
#define QUEUE_SVC_INTERVAL           0.001

// Aggregation
//    This is the master state machine that snoops on visitors
//    in the Ctrl expander and takes appropriate re-aggregation 
//    actions as necessary.
class Aggregator : public State {
  friend class AggregationPolicy;
  friend class LogicalPeer;
  friend class LogicalLink;
  friend class ComplexRep;
public:

  Aggregator(const NodeID & myNode, const PeerID & myPeer, 
	     const char * agg_pol = 0);
  virtual ~Aggregator();

  State * Handle(Visitor * v);
  void Interrupt(SimEvent * e);

protected:
  // Aggregator provides DB Advertisement services for its client Plugins
  void Advertise(NPFloodVisitor * npv);  

private:

  int GetNextLevelUp( int level );
  int GetNextLevelDown( int level );
  int GetPhysicalLevel( void );

  // Signalling and PNNI event handling helpers
  bool HandleNPFlood(NPFloodVisitor * v);
  bool HandleHLink(HorLinkVisitor * v);

  bool HandleSVCFromOutside(FastUNIVisitor * v);
  bool HandleSVCFromInside(FastUNIVisitor * v);

  bool HandleLogosGraphFromOutside(LogosGraphVisitor * v);

  void ServiceLHIQueues(void);

  // SVCs currently active 
  dictionary<const NodeID *, LogicalPeer *> _peers;

  // SVC Setup/Teardown
  void SVCSetup(const NodeID * nid);
  void SVCTeardown(const NodeID * nid);
  bool IsSVCSetup(const NodeID * nid) const;

  int  ObtainLogicalPort(const NodeID * remNode, int aggTok);
  void ObtainLogicalPort(HorLinkVisitor * hv);
  int  ObtainNextAvailLogicalPort(int level);

  const char * PrintName(void) const;

  bool AmILeader( int level ) const;

  // PTSE Identifier generator
  enum { LOGICAL_LINK_BASE_ID = 1000 };
  int  NextID(void);
  int                           _nextid;
  list<NodeID *>                _svcs;

  struct RACont {
    RACont(const NodeID *, int);
    ~RACont();

    RACont(const RACont & rhs);
    RACont & operator = (const RACont & rhs);

    NodeID * _rnid;
    int      _aggtok;
  };
  friend ostream & operator << (ostream & os, const RACont & rhs);
  friend int operator == (const RACont & lhs, const RACont & rhs);
  friend int compare(RACont * const & lhs, RACont * const & rhs);

  // level -> dictionary<RACont, LGport>
  dictionary<int, dictionary<RACont *, int> *> _LGport_table;

  // Dictionary of Queues for Logical Hello Instantiators
  dictionary<const NodeID *, ds_queue<FastUNIVisitor *> *> _lhi_map;
  // SimEvent to service the queues
  SimEvent * _qse;

  // Link aggregator Plugin
  AggregationPolicy * _aggr;

  NodeID             * _me;

  // Types
  static const VisitorType * _fastuni_type;
  static const VisitorType * _npflood_type;
  static const VisitorType * _horlink_type;
  static const VisitorType * _logosgraph_type;
};

#endif
