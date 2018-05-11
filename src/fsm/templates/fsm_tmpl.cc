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
static char const _FSM_TMPL_CC_rcsid_[] =
"$Id: fsm_tmpl.cc,v 1.50 1999/02/19 22:42:18 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <DS/containers/list.cc>
#include <fsm/templates/fsm_tmpl.h>
class PTSPPkt;
template class list<PTSPPkt *> ;
class ig_uplinks;
template class list<ig_uplinks *> ;
class ig_horizontal_links;
template class list<ig_horizontal_links *> ;
class ig_nodal_state_params;
template class list<ig_nodal_state_params *> ;
class PTSEReqPkt;
template class list<PTSEReqPkt *> ;
class InducedUplink;
template class list<InducedUplink *> ;
class NNIReplayer;
template class list<NNIReplayer *> ;
class RXEntry;
template class list<RXEntry *> ;
class PNNI_crankback;
template class list<PNNI_crankback *> ;
class NodeID;
template class list<NodeID *> ;
class Investigator;
template class list<Investigator *>;
class StatRecord;
template class list<const StatRecord *>;
class HorLinkVisitor;
template class list<HorLinkVisitor *>;
class InvestigatorTimer;
template class list<InvestigatorTimer *> ;

#include <DS/containers/queue.cc>
class Visitor;
template class ds_queue<Visitor *> ;

#include <DS/containers/dictionary.cc>
class Fab;
template class list<Fab *> ;
template class dictionary <Fab *, list<Fab *> *> ;
class NodeID;
template class dictionary <const NodeID *, int> ;
template class dictionary <InducedUplink *, NodeID *> ;
class Conduit;
template class dictionary<int, Conduit *> ;
class Addr;
template class dictionary <Fab *, Addr *> ;
template class dictionary <unsigned int, Fab *> ;
template class dictionary <int , dictionary<unsigned int, Fab *> *> ;
class FastUNIVisitor;
template class dictionary <FastUNIVisitor *, Fab *> ;
template class dictionary <char *, bool> ;
template class dictionary <const NodeID *, bool> ;

#include <FW/kernel/KernelTime.h>
#include <fsm/netstats/StatRecord.h>
template class dictionary<KernelTime, StatRecord *> ;
template class dictionary<char *, double> ;
#include <fsm/netstats/NetStatsCollector.h>
template class dictionary< NetStatsCollector::TrafficTypes, double > ;
template class dictionary< NetStatsCollector::TrafficTypes, list< double > * > ;
template class dictionary< char *, dictionary< NetStatsCollector::TrafficTypes, double > * > ;
template class dictionary< Investigator *, KernelTime> ;

#include <DS/containers/h_array.cc>
template class dictionary<int, bool> ;
template class h_array<int, bool> ;
template class dictionary<int, const NodeID *> ;
template class h_array<int, const NodeID *> ;
class Party;
template class dictionary<int, Party *> ;
template class h_array<int, Party *> ;
template class dictionary<int, unsigned int> ;
template class h_array<int, unsigned int> ;

#include <DS/containers/p_queue.cc>
class ig_ptse;
template class dictionary<double, ig_ptse *> ;
template class p_queue<double, ig_ptse *> ;

#include <DS/containers/sortseq.cc>
#include <codec/pnni_ig/id.h>
template class dictionary<const NodeID *, list<int> *> ;
template class sortseq<const NodeID * , list<int> *> ;
class Nodalinfo;
template class dictionary<const NodeID *, Nodalinfo *> ;
template class sortseq<const NodeID *, Nodalinfo *> ;
template class dictionary<pq_item, seq_item> ;
template class sortseq<pq_item, seq_item> ;
class VCAllocator;
template class dictionary<int, VCAllocator *> ;
template class sortseq<int, VCAllocator *> ;
#include <codec/pnni_ig/DBKey.h>
template class dictionary<DBKey *, pq_item> ;
template class sortseq<DBKey *, pq_item> ;
#include <fsm/database/Database.h>
template class dictionary<Database::OriginKey *, list<ig_ptse *> *> ;
template class sortseq<Database::OriginKey *, list<ig_ptse *> *> ;
template class list<Database::FloodContainer *> ;

class DBKey;
template class dictionary<const NodeID *, list<NodeID *> *> ;
template class dictionary<DBKey *, int> ;
template class dictionary<const NodeID *, NodeID *> ;
template class dictionary<const NodeID *, dictionary<DBKey *, int> *> ;

#include <DS/util/String.h>
class ds_String;
class CallConfiguration;
template class dictionary <ds_String *, CallConfiguration *>;

class LinkConfiguration;
template class dictionary <ds_String *, LinkConfiguration *>;

class SwitchConfiguration;
template class dictionary <ds_String *, SwitchConfiguration *>;

class SwitchLevelInfo;
template class dictionary<int, SwitchLevelInfo *>;

int compare(ds_String *const & x, ds_String *const & y)
{ 
  int answer = 0;
  if ((long)x != (long)y) {
    answer = ((x->size() < y->size()) ? -1 : 
	      ((y->size() < x->size()) ? 1 : 0));

    if (answer == 0)
      answer = strcmp(x->chars(), y->chars());
  }

  return answer;
}

// --------------------------------

template class sortseq<int, SwitchLevelInfo *>;

// ----------------------------------------------------------------
int compare(NNIReplayer * const & x, NNIReplayer * const & y)
{ return (x > y ? 1 : y > x ? -1 : 0); }

int compare(FastUNIVisitor * const & lhs, FastUNIVisitor * const & rhs)
{
  return ((long)lhs > (long)rhs ? 1 : ((long)rhs > (long)lhs ? -1 : 0));
}

int compare(SimEvent * const & lhs, SimEvent * const & rhs)
{
  return ((long)lhs > (long)rhs ? 1 : ((long)rhs > (long)lhs ? -1 : 0));
}

#include <fsm/database/Database.h>
int compare(const Database::OriginKey & lhs, const Database::OriginKey & rhs)
{
  return (memcmp((unsigned char *)lhs._oid, (unsigned char *)rhs._oid, 22));
}

int compare(Database::OriginKey *const & lhs, Database::OriginKey *const & rhs)
{
  return (compare(*lhs, *rhs));
}

int compare(Database::FloodContainer * const & x, Database::FloodContainer * const & y)
{ 
  return (x > y ? 1 : y > x ? -1 : 0); 
}

#include <fsm/hello/LgnHelloState.h>
int compare(InducedUplink *const & lhs, InducedUplink *const & rhs)
{
  int m = lhs->_border->NIDcmp(rhs->_border);

  if (!m)
    return (lhs->_portID > rhs->_portID ? 1 : (lhs->_portID < rhs->_portID ? -1 : 0));
  return m;
}

#include <fsm/forwarder/Fab.h>
int compare(Fab * const & lhs, Fab * const & rhs)
{
  assert(lhs && rhs);

  if (lhs->equals(*rhs))
    return 0;
  else if (*lhs < *rhs)
    return -1;
  else if (*lhs > *rhs)
    return 1;
  // if we've gotten this far there is a problem with the equality operators
  abort();
}

int compare(HorLinkVisitor * const & lhs, HorLinkVisitor * const & rhs)
{
  return ((long)lhs > (long)rhs ? 1 : ((long)rhs > (long)lhs ? -1 : 0));  
}

int compare(InvestigatorTimer * const & l, InvestigatorTimer * const & r)
{
  long lhs = (long)l, rhs = (long)r;
  return (lhs - rhs);
}

int compare(const StatRecord * const & l, const StatRecord * const & r)
{
  int rval = 0;

  if (!(rval = strcmp(l->getType(), r->getType())) &&
      !(rval = strcmp(l->getLoc(), r->getLoc()))   &&
      !(rval = strcmp(l->getMisc(), r->getMisc())) &&
      !(rval = compare(l->getNode(), r->getNode())) &&
      !(rval = (long)l->getTime() - (long)r->getTime()))
    rval = 0;
  return rval;
}

