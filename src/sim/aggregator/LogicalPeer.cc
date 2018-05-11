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
#ifndef LINT
static char const _LogicalPeer_cc_rcsid_[] =
"$Id: LogicalPeer.cc,v 1.1 1999/02/16 00:29:28 bilal Exp $";
#endif

#include <common/cprototypes.h>
#include "LogicalPeer.h"
#include "LogicalLink.h"
#include "Aggregator.h"
#include "AggregationPolicy.h"
#include <FW/basics/diag.h>
#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/uplinks.h>
#include <fsm/hello/HelloVisitor.h>

LogicalPeer::LogicalPeer(NodeID * remNode, Aggregator* a, AggregationPolicy * aggr)
  : _aggr(aggr), _agg_state(a), _remNode(remNode)
{ }

LogicalPeer::~LogicalPeer()
{
  dic_item di;
  forall_items(di, _links) {
    delete _links.inf(di);
  }
  _links.clear();
  _HLinkUp.clear();
}

int LogicalPeer::Update(int id_of_dying_ptse, NodeID* origin) {
  int affected = 0;
  dic_item di;
  forall_items(di, _links) {
    affected += _links.inf(di)->Update( id_of_dying_ptse, origin );
  }
  return affected;
}

void LogicalPeer::Update(int id_of_new_ptse, NodeID* origin, ig_uplinks * ulig )
{
  int agg = ulig->GetAggTok();

  dic_item di = _links.lookup(agg);

  // We cannot use the aggregation token as an identifier, 
  // it almost always conflicts with previously chosen 
  // identifier values.
  if (!di)
    di = _links.insert(agg, new LogicalLink(GetNextID(), _aggr, this));

  if (di) {
    LogicalLink * ll = _links.inf(di);
    ll->Update(id_of_new_ptse, origin, ulig);
  }
}

int LogicalPeer::Aggregate(HorLinkVisitor * hv)
{
  int agg  = hv->GetAgg();

  dic_item di;
  if (!(di = _links.lookup(agg))) {
    // perhaps this is the first time we're seeing it.
    di = _links.insert(agg, new LogicalLink(GetNextID(), _aggr, this));
  }
  DIAG("sim.aggregator", DIAG_DEBUG, cout
       << "Forcing reaggregation of LogicalLink to " 
       << *_remNode << " for aggregation token " << agg << endl);
  LogicalLink * llcoolj = _links.inf(di);
  llcoolj->Aggregate(hv);

  return llcoolj->localPort();
}

int LogicalPeer::DeAggregate(HorLinkVisitor * hv)
{
  // force the origination of null body PTSEs
  dic_item di;

  assert( di = _links.lookup(hv->GetAgg()) );
  LogicalLink * ll = _links.inf( di );

  DIAG("sim.aggregator", DIAG_ERROR, cout
	 << "Forcing deaggregation of LogicalLink to " 
	 << *_remNode << " for aggregation token " << hv->GetAgg() 
         << " IS NOT SUPPORTED YET!!" << endl);

  return ll->localPort();
}

int LogicalPeer::GetNextID(void)
{ // make next PTSE Identifier
  return _agg_state->NextID();
}

void LogicalPeer::HLinkUp(int token, bool h)
{
  dic_item di=_HLinkUp.lookup(token);
  if (h) {
    if (di) 
      _HLinkUp.change_inf(di,1);
    else
      _HLinkUp.insert(token,1);
  }
  else {
    if (di) 
      _HLinkUp.change_inf(di,0);
    else
      _HLinkUp.insert(token,0);
  }
}

bool LogicalPeer::HLinkUp(int token)
{
  dic_item di=_HLinkUp.lookup(token);
  if (di) {
    if ( _HLinkUp.inf(di) == 1)
      return true;
    return false;
  }

  return false;
}

const NodeID * LogicalPeer::GetRemoteNID(void) const
{
  return _remNode;
}
