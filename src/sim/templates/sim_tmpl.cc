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
static char const _SIM_TMPL_CC_rcsid_[] =
"$Id: sim_tmpl.cc,v 1.56 1999/02/25 14:44:30 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <sim/templates/sim_tmpl.h>
#include <DS/containers/list.cc>

// Used to be in cronos_tmpl, but the TrafficInvestigator needs this
// BK 1/21/99
template class list<double> ;

class logos_edge;
template class list<logos_edge *> ;
#include <codec/pnni_ig/id.h>
template class list<const NodeID *> ;
class transit;
template class list<transit *> ;
class call_identifier;
template class list<call_identifier *> ;
class FastUNIVisitor;
template class list<FastUNIVisitor *> ;
class LinkStateWrapper;
template class list<LinkStateWrapper *> ;


// ------------------------------
#include <DS/containers/queue.cc>
class FastUNIVisitor;
template class ds_queue<FastUNIVisitor *> ;
// -----------------------------------
#include <DS/containers/dictionary.cc>
template class dictionary<const NodeID *, int> ;
class node_struct;
template class dictionary<const NodeID *, node_struct *> ;
class Addr;
template class dictionary<const NodeID *, Addr *> ; 
template class dictionary<Addr *, NodeID *> ;
class ig_resrc_avail_info;
template class dictionary<unsigned int, ig_resrc_avail_info *> ;
class AggregationPolicy;
template class dictionary<const char *, AggregationPolicy *(*)(void)> ;
class ACACPolicy;
template class dictionary<const char *, ACACPolicy *(*)(void)> ;
class BaseLogos;
class Logos;
template class dictionary<const char *, BaseLogos *(*)(Logos *)> ;
template class dictionary<int, list<NodeID *> *> ;
class logos_node;
template class dictionary<logos_node *, node_struct *> ;
class LogicalLink;
template class dictionary<int, LogicalLink *> ;
class LogicalPeer;
template class dictionary<const NodeID *, LogicalPeer *> ;
template class dictionary<int, node_struct *> ;
template class dictionary<const char *, NodeID *> ;
class Conduit;
template class dictionary<u_int, Conduit *> ; // added for emulation Dec23,98 (AB)
template class dictionary<const char *, Conduit *> ;
template class dictionary<const char *, list<LinkStateWrapper *> *> ;
#include <sim/ctrl/NID2VCMapper.h>
template class dictionary<const NodeID *, NID2VCMapper::VCcontainer *> ;
#include <sim/router/RouteControl.h>
template class dictionary<int, RouteControl::MessageContainer *> ;
#include <sim/aggregator/Aggregator.h>
template class dictionary<Aggregator::RACont *, int> ;
template class dictionary<int, dictionary<Aggregator::RACont *, int> *> ;
class Conduit;
template class dictionary<Addr *, Conduit *> ;
template class dictionary<const NodeID *, Conduit *> ;
#include <sim/leadership/Leadership.h>
template class dictionary<unsigned char, Leadership::LevelInfo *> ;
template class dictionary<const NodeID *, ds_queue<FastUNIVisitor *> *> ;
template class dictionary<FastUNIVisitor *, bool> ;
class edge_struct;
class node_struct;
template class dictionary<logos_edge *, edge_struct *> ;
class DFExpander;
template class dictionary<Conduit *, DFExpander *> ;
template class dictionary<int, list<ig_resrc_avail_info *> *> ;
template class dictionary<int, dictionary<int, list<ig_resrc_avail_info *> *> *> ;
class ComplexRep;
template class dictionary<const NodeID *, ComplexRep *> ;
template class list<list_item *> ;

class ig_uplinks;
class Anchor;
template class dictionary<Anchor*, ig_uplinks *> ;

int compare( const Visitor * x, const Visitor * y )
{
  return ((long)x > (long)y ? 1 : (long)y > (long)x ? -1 : 0);
}

template class dictionary< const Visitor * , long > ;

int compare(Anchor * x, Anchor * y)
{ return (x > y ? 1 : y > x ? -1 : 0); }

// --------------------------------
#include <DS/containers/sortseq.cc>
#include <sim/acac/pluggable_port_info.h>
template class dictionary<unsigned int, pluggable_port_info::iport_oport_td *> ;
template class sortseq<unsigned int, pluggable_port_info::iport_oport_td *> ;

// ------------------------------
#include <DS/containers/graph.cc>
template class node_array<int> ;
template class node_array<edge_struct *> ;
template class node_array<dic_item> ;
template class edge_array<int> ;
#include <sim/logos/atoms.h>
class Transit;
template class GRAPH<logos_node *, logos_edge *> ;
template class dictionary<node_struct *, logos_node *> ;
template class dictionary<edge_struct *, logos_edge *> ;
template class dictionary<node_struct *, edge_struct *> ;
template class dictionary<node_struct *, association *> ;
template class dictionary<edge_struct *, int> ;
template class dictionary<node_struct *, int> ;
template class dictionary<const char *, logos_node *> ;
template class dictionary<const char *, list<logos_edge *> *> ;
template class dictionary<Transit *, node_struct *> ;
class ig_nodal_state_params;
class ig_resrc_avail_info;
template class dictionary<const NodeID *, list<ig_nodal_state_params *> *> ;
template class dictionary<const NodeID *, list<ig_resrc_avail_info *> *> ;
class LgnHelloState;
template class dictionary<int, LgnHelloState *> ;
// --------------------------------
#include <DS/containers/p_queue.cc>
template class p_queue<int, node_struct *> ;

#include <codec/pnni_ig/resrc_avail_info.h>
template class list <const ig_resrc_avail_info * >;

int compare(const ig_resrc_avail_info * & x,
	    const ig_resrc_avail_info * & y) 
{ return (x > y ? 1 : y > x ? -1 : 0); }
int compare(const ig_resrc_avail_info * const & x,
	    const ig_resrc_avail_info * const & y) 
{ return (x > y ? 1 : y > x ? -1 : 0); }

// --------------------------------------------------
int compare(transit * const & x, transit * const & y)
  //int compare(const transit * & x, const transit * & y)
{ return (x > y ? 1 : y > x ? -1 : 0); }
int compare(call_identifier * const & x, call_identifier * const & y)
  //int compare(const call_identifier * & x, const call_identifier * & y)
{ return (x > y ? 1 : y > x ? -1 : 0); }

int compare(LinkStateWrapper *const & x, LinkStateWrapper *const & y)
{ return (x > y ? 1 : y > x ? -1 : 0); }

int compare(list_node **const & lhs, list_node **const & rhs)
{
  if (lhs < rhs)
    return -1;
  if (lhs > rhs)
    return 1;
  return 0;
}

int compare(char const *const & x, char const *const & y)
{
  return strcmp(x, y);
}

int compare(unsigned char const & x, unsigned char const & y)
{  
  return ((x < y) ? -1 : (x > y ? 1 : 0)); 
}

#include <sim/aggregator/Aggregator.h>
int compare(Aggregator::RACont  * const & lhs, Aggregator::RACont * const & rhs)
{
  int rval = compare((const NodeID * const &)lhs->_rnid, 
		     (const NodeID * const &)rhs->_rnid);
  if (!rval)
    return (lhs->_aggtok > rhs->_aggtok ? 1 : (lhs->_aggtok < rhs->_aggtok ? -1 : 0));
  return rval;
}

#include <sim/logos/atoms.h>
int compare (logos_edge * const & e1, logos_edge * const & e2)
{
  // Usually when we write compares for pointers it is a simply this
  if ((long)e1 > (long)e2)
    return 1;
  if ((long)e2 > (long)e1)
    return -1;
  return 0;
}

int compare (logos_node * const & lhs, logos_node * const & rhs)
{
  int result=
    compare((const NodeID *)lhs->_node_id, (const NodeID *)rhs->_node_id);

  if (result == 0) { // same nodeid
    result = compare(lhs->_logical_port, rhs->_logical_port);
  }

  return result;
}

