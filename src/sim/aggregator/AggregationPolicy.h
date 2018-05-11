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
#ifndef __AGGREGATIONPOLICY_H__
#define __AGGREGATIONPOLICY_H__

#ifndef LINT
static char const _AggregationPolicy_h_rcsid_[] =
"$Id: AggregationPolicy.h,v 1.1 1999/02/16 00:29:28 bilal Exp $";
#endif

#include <codec/pnni_ig/id.h>
class ig_horizontal_links;
class ig_uplinks;
class Aggregator;
class ig_resrc_avail_info;
class LogosGraph;
class ComplexRep;
class Anchor;

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>

// AggregationPolicy is the base plugin class for link aggregation.
//     Derived classes can override these default methods
//
// The following are called when a bundle of lower-level hlinks needs to
// aggregated into a single logical link.  In addition a derived class can
// override these default methods:
//
//  virtual ig_resrc_avail_info * 
//  Compute_CBR(const list <const ig_resrc_avail_info *>& lst, 
//              ig_resrc_avail_info* current);
//  virtual ig_resrc_avail_info * 
//  Compute_RTVBR(const list <const ig_resrc_avail_info *>& lst, 
//                ig_resrc_avail_info* current);
//  virtual ig_resrc_avail_info * 
//  Compute_NRTVBR(const list <const ig_resrc_avail_info *>& lst, 
//                 ig_resrc_avail_info* current);
//  virtual ig_resrc_avail_info * 
//  Compute_ABR(const list <const ig_resrc_avail_info *>& lst, 
//              ig_resrc_avail_info* current);
//  virtual ig_resrc_avail_info * 
//  Compute_UBR(const list <const ig_resrc_avail_info *>& lst, 
//              ig_resrc_avail_info* current);
//
// The following is called when a complex node is to be reaggregated
//
//  virtual void Update_Complex_Node(LogosGraph & lg, ComplexRep & cr);

class AggregationPolicy {
  friend class LogicalLink;
  friend class Aggregator;
  friend class LogicalPeer;
public:
  AggregationPolicy(void);
  virtual ~AggregationPolicy();

protected:
  // These are the only functions that (presently)
  // you can override in the derived class
  virtual ig_resrc_avail_info * 
  Compute_CBR(const list <const ig_resrc_avail_info *>& lst, 
	      ig_resrc_avail_info* current);
  virtual ig_resrc_avail_info * 
  Compute_RTVBR(const list <const ig_resrc_avail_info *>& lst, 
		ig_resrc_avail_info* current);
  virtual ig_resrc_avail_info * 
  Compute_NRTVBR(const list <const ig_resrc_avail_info *>& lst, 
		 ig_resrc_avail_info* current);
  virtual ig_resrc_avail_info * 
  Compute_ABR(const list <const ig_resrc_avail_info *>& lst, 
	      ig_resrc_avail_info* current);
  virtual ig_resrc_avail_info * 
  Compute_UBR(const list <const ig_resrc_avail_info *>& lst, 
	      ig_resrc_avail_info* current);

  virtual void Update_Complex_Node(LogosGraph & lg, ComplexRep & cr);

  // You'll get nothing more from me, my heirs!

private:
  friend class ComplexRep;

  // called by leadership when we win and lose PGL status
  void Register_ComplexRep(ComplexRep * cr);
  void Deregister_ComplexRep(ComplexRep * cr);

  // called when we gain or lose logical ports 
  // i.e. the addition or removal of uplinks leaving the PG
  void Update_Complex_Rep(const NodeID * lgn, int lgport, bool add = true);

  // If you don't define your own (derived) link aggregation, this is what you'll get
  ig_resrc_avail_info * 
  Default_Aggregate_Links(const list<const ig_resrc_avail_info *>& lst,
			  int service_cat,
			  ig_resrc_avail_info* current );

  // how frequently we must refresh the PTSEs
  double PTSERefreshInterval(void) const;

  void SetDBLifetime(int life);
  void SetDBRefresh(double interval);
  void SetMaster(Aggregator * s);
  void SetIdentity(const NodeID & myNode, const PeerID & myPeer);

  // Called by Logical Link when something happens to affect the bundle
  // re-aggregates, then resends fresh PTSE to the DB
  list <ig_resrc_avail_info *> * 
  AggregateUpLinks(const dictionary<Anchor *,ig_uplinks *> * links, 
		   const ig_horizontal_links * currently_advertised);

  // Insert null-body PTSE to flush
  void FlushHLink(int id, int seqnum);

  // called in order to send the PTSE for flooding
  void InjectPTSE(ig_horizontal_links * x, int id, int seqnum);
  void InjectPTSE(ig_uplinks * uplink, int id, int seqnum, NodeID* orig);

  const char * GetName(void) const;

  NodeID     * _myNode;
  PeerID     * _myPeerGroup;

  dictionary<const NodeID *, ComplexRep *> _complexReps;

  int          _life;
  double       _refresh;
  Aggregator * _master;  // Actually does the PTSE flooding 
                         // requested by above InjectPTSE() method
};

#endif // __AGGREGATIONPOLICY_H__
