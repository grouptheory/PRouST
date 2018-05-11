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
#ifndef __LOGICALLINK_H__
#define __LOGICALLINK_H__
#ifndef LINT
static char const _LogicalLink_h_rcsid_[] =
"$Id: LogicalLink.h,v 1.1 1999/02/16 00:29:28 bilal Exp $";
#endif

class AggregationPolicy;
class ig_horizontal_links;
class ig_uplinks;
class NodeID;
class LogicalPeer;
class HorLinkVisitor;

#include <FW/kernel/Handlers.h>
#include <DS/containers/list.h>

class Anchor;

// A Logical Link is a bundle of lower level links
//     that have chosen to be aggregatd together
//     by selecting the same Aggregation token.
// 
// There are two flavors:
//     1. The logical HLINK
//     2. The induced UPLINK
//
class LogicalLink : public TimerHandler {
public:

  LogicalLink(int id, AggregationPolicy * agg, LogicalPeer * parent);
  ~LogicalLink();

  // discovery of new supporting uplink
  virtual void Update(int id_of_new_ptse, NodeID* origin, ig_uplinks * lnk);
  // death of supporting uplink
  int Update(int id_of_dying_ptse, NodeID* origin);

  virtual void Aggregate(HorLinkVisitor * hv = 0);

  virtual bool Disconnected(void);  // The bundle is empty?

  virtual void Callback(void);

  int localPort(void) const;
  int remotePort(void) const;

protected:

  AggregationPolicy  * _agg;             // The Plugin
  int                  _id;              // The PTSE ID for this Logical Link
  int                  _seqnum;          // The current sequence number

  bool HLinkUp( int token );

  void SetPorts(int lp, int rp);

private:

  NodeID * _rNID;
  int      _aggtok;

  int      _rport; // of the aggregated Hlink
  int      _lport;

  // search the bundle for a lower-level uplink
  dic_item Scan(int id_of_new_ptse, NodeID* origin);

  dictionary<Anchor*, ig_uplinks *>    _uplinks;        // supporting lower level uplinks
  dictionary<int, int>                 _induced_ids;    // local node level -> induced uplink ID
  dictionary<int, int>                 _induced_seqnum; // local node level -> induced uplink seqnum

  ig_horizontal_links * _saved_hlink;
  ig_uplinks          * _saved_uplink;

  LogicalPeer         * _parent;
  bool                  _dirty;
};

#endif



