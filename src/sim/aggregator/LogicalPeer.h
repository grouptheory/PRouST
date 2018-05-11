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
#ifndef __LOGICALPEER_H__
#define __LOGICALPEER_H__
#ifndef LINT
static char const _LogicalPeer_h_rcsid_[] =
"$Id: LogicalPeer.h,v 1.1 1999/02/16 00:29:28 bilal Exp $";
#endif

class LogicalLink;
class AggregationPolicy;
class Aggregator;
class ig_uplinks;
class HorLinkVisitor;
class NodeID;

#include <DS/containers/dictionary.h>

// This class represents an SVC between us and another LGN
// It maintains a dictionary of Logical Links, each of
// which is a bundle of lower-level links with the same aggregation
// token.

class LogicalPeer {
public:

  LogicalPeer(NodeID * remNode, Aggregator * a, AggregationPolicy * aggr);
  ~LogicalPeer();

  // Possible Logical Hello phenomena--
  // discovery of new supporting uplink
  void Update(int id_of_new_ptse, NodeID* origin, ig_uplinks * ulig);
  // death of supporting uplink
  int Update(int id_of_dying_ptse, NodeID* origin);

  int  Aggregate(HorLinkVisitor * v);
  // Responsible for removing the specified info from a 
  // logical Hlink or removing the logical Hlink altogether.
  int  DeAggregate(HorLinkVisitor * v);

  void HLinkUp(int token, bool h);
  bool HLinkUp(int token);

  const NodeID * GetRemoteNID(void) const;

private:

  int GetNextID(void);                   // Next PTSE id (for new logical links)

  AggregationPolicy  * _aggr;            // Plugin
  Aggregator         * _agg_state;       // Owning state machine

  dictionary<int, LogicalLink *> _links; // agg token to logical link map
                                         // These are the logical links being
                                         // maintained over this SVC, indexed
                                         // by their aggregation token.

  dictionary <int, int>  _HLinkUp;       // Token to 0/1 if LGN is up or down

  NodeID             * _remNode;
};

#endif // __LOGICAL_PEER_H__
