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
static char const _BaseLogos_cc_rcsid_[] =
"$Id: BaseLogos.cc,v 1.63 1999/01/07 23:04:06 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include <sim/logos/Logos.h>
#include <sim/logos/atoms.h>
#include <sim/logos/BaseLogos.h>
#include <sim/logos/LogosGraph.h>
#include <fsm/visitors/FastUNIVisitor.h>

#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/uni_ie/called_party_num.h>
#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/PNNI_crankback.h>
#include <codec/uni_ie/PNNI_designated_transit_list.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/UNI40_td.h>
#include <codec/uni_ie/UNI40_xqos.h>
#include <codec/uni_ie/UNI40_bbc.h>
#include <codec/q93b_msg/q93b_msg.h>
#include <sim/acac/pluggable_port_info.h>

#include <FW/basics/diag.h>

#include <DS/containers/dictionary.h>
#include <DS/containers/p_queue.h>
class attr_metrics;

static void sssp(node s,                   // starting node
		 edge_array<int>&  cost,   // cost to traverse an edge
		 node_array<int>&  dist,   // distance from source
		 node_array<edge>& pred,   // edge leading to node
		 p_queue<int,node> * PQ,
		 LogosGraph *LG);

extern "C" BaseLogos * BaseLogosConstruct(Logos * logos)
{
  return new BaseLogos(logos);
}

BaseLogos::BaseLogos(Logos * logos) : _logos(logos), _nickname(0) { }

BaseLogos::~BaseLogos() { delete [] _nickname; }

bool BaseLogos::top_is_me(const NodeID *source,
			  FastUNIVisitor * fuv,
			  ie_cause * & cause,
			  PNNI_crankback * & crankback)
{
  list_item it;
  bool its_me = true;
  // find the level of source by comparing to all my levels
  list <NodeID *> *nodeid_levels = _logos->GetLevels();
  NodeID * level_nodeid = 0;

  forall_items(it, * nodeid_levels) {
    level_nodeid = nodeid_levels->inf(it);
    if ( same_as(level_nodeid, source)==0 ) break;
    else level_nodeid = 0;
  }

  if ( level_nodeid == 0 ) {
    DIAG("sim.logos", DIAG_INFO,
	 cout << "BaseLogos:("  << MyNickName() << "): " << endl
	      << "     ComputeRoute: 'source' " << *source
	      << "     is not me at any level " << endl;

	 NodeID *me = 0;
	 while (nodeid_levels->size() >0 ) {
	   me = nodeid_levels->pop();
	   cout << "    I am " << *me << endl;
	   delete me;
	 } );
    cause = new ie_cause(ie_cause::temporary_failure);
    crankback = new PNNI_crankback((u_char)source->GetLevel(),
				   PNNI_crankback::BlockedNode,
				   PNNI_crankback::DTLTransitNotMyNodeID,
				   source);
    fuv->SetRelease(cause, crankback);
    its_me = false;
  }
  while ( nodeid_levels->empty() == false )
    delete nodeid_levels->pop();

  return its_me;
}

// delete the contents of a route list
static void clear_route (list<transit *> * route)
{
  if (route != 0) {
    while (route->size() > 0) {
      transit *tr = route->pop();
      delete tr;
    }
  }
}
static void transfer_route (list<transit *> * dest, list<transit *> * src)
{
  assert( dest && src );

  while (src->size() > 0) {
    transit *tr = src->pop();
    dest->append(tr);
  }
}

// Using the fuv (the DTL and the internal recipient address)
// determine the source and destination for this setup
//
// Returns true:  when
//   FastUNIVisitor contains updated DTL
//
// Returns false: when
//   DTL computation failed. FastUNIVisitor *fun  should contain
//   an ie_cause, possibly a crankback ie, and will be
//   cranked back, or returned to the calling party.

bool BaseLogos::ComputeRoute(FastUNIVisitor * fuv)
{
  //
  // if the dtl is empty we are the source at the physical level
  //    and the destination address (NOT NodeID) is in the FastUNIVisitor
  //
  // else if the dtl is not empty, the the first element of the dtl
  //    must be us at some level higher than the physical level
  //
  //    if the first element of the dtl is us at a higher level then
  //       if the port of the dtl is non-zero, the destination is the
  //          list of all nodes represented by the aggregation for that
  //          logical border node --
  //          if one of these nodes is us at the physical level, then
  //             the second element of the dtl becomes the target
  //
  //       else the port is 0 then
  //          the destination is the second element of the dtl
  //    else if not us then error
  //
  //  if the destination derived from these actions is null then
  //     construct the destination form the destination address in the
  //     message.
  //
  list <transit *> *route = 0;
  const NodeID * source = 0, * destination = 0;
  ie_cause * cause= 0;
  PNNI_crankback * crankback = 0;
  bool return_value = true; // assume success
  list<NodeID *> * alternate_destinations = 0;

  // establish my own physical NodeID
  const NodeID * physical_nodeid = _logos->My_NodeID(); // a copy

  assert(physical_nodeid);
  int physical_level = physical_nodeid->GetLevel();

  //
  // Establish the source and destination
  //
  DTLContainer * top_dtc = fuv->Pop();
  const DTLContainer * next_dtc = 0;
  NodeID * top = 0;
  int top_port= 0;
  if (top_dtc) {
    top = (NodeID *)top_dtc->GetNID();
    top_port = top_dtc->GetPort();
  }

  // call the NetStatsCollector with type DTL_Expansion

#ifdef NEW_CODE
  if (top) {			// dtl exists top = sourace
    //    assert(0);
    source = top;
    if (top_is_me(top, fuv, cause, crankback)) {

      bool immediate_return=false, immediate_return_val=false;

      alternate_destinations =
	border_destination(fuv,
			   source, top_port,
			   destination, physical_nodeid,
			   immediate_return, immediate_return_val,
			   cause, crankback);
      if (immediate_return)
	return immediate_return_val; 
    } else {
      delete physical_nodeid;
      return false;
    }
  } else {			// dtl is empty
    source = physical_nodeid;
  }
#else
  if (top) {			// dtl exists top = sourace
    source = top;
    if (top_is_me(top, fuv, cause, crankback)) {

      if (source->GetLevel() != physical_nodeid->GetLevel()) {

	// border node
	if (top_port != 0) {
	  // there are two cases
	  // 1) I am not the border node in question - I have to route to it
	  // b) I am     the border node in question
	  next_dtc = fuv->Peek();
	  const NodeID * upnode = next_dtc->GetNID();
	  list<NodeID *> * borders =
	    _logos->LogicalPortToBorder(top, top_port, upnode);

	  if (borders) {
	    // if my own physical_nodeid is on the list of border nodes
	    // aggregated to top_dtc, then verify that the second element of
	    // dtc is adjacent to me -- difficult since we haven't got
	    //                          a graph to work with yet.
	    // If it is not remove ourselves from the list
	    // if it is then the route is trivial

	    while (borders->size()) {
	      NodeID *b = borders->pop();

	      if ( same_as(b, physical_nodeid) == 0 ) { // is me
		if ( _logos->I_am_adjacent(upnode) ) {
		  delete physical_nodeid;
		  while ( borders->size() > 0 ) borders->pop();
		  delete borders;
		  if (alternate_destinations != 0)
		    delete alternate_destinations;
		  return true;
		} else { // if I am not adjacent then leave me out - rare case
		  delete b;
		}
	      } else { // not me
		if (alternate_destinations == 0 )
		  	    alternate_destinations = new list<NodeID *>;
		alternate_destinations->append(b);
	      }
	    }
	    if (alternate_destinations) {
	      destination = alternate_destinations->pop();
	      if (alternate_destinations->empty()) {
		delete alternate_destinations;
		alternate_destinations = 0 ;
	      }
	    }
	    delete borders;
	  }
	  if (destination == 0) {
	    cout << "NEED TO ADD CRANKBACK to fuv at line 200 of BaseLogos"
		 << endl;
	    return false;
	  }
	} else if ((next_dtc = fuv->Pop())) // logical port == 0
	  destination = next_dtc->GetNID();
      } else if ((next_dtc = fuv->Pop()))
	destination = next_dtc->GetNID();
    } else {
      delete physical_nodeid;
      return false;
    }
  } else {			// dtl is empty
    source = physical_nodeid;
  }
#endif
  
  // the final entry border node has to reconstruct destination
  // using the contents of the q93b setup message
  if (!destination) {
    unsigned char id[22];
    q93b_setup_message * msg = fuv->GetSetup();
    InfoElem ** ie = msg->get_ie_array();
    ie_called_party_num * cpn =
      (ie_called_party_num *)ie[InfoElem::ie_called_party_num_ix];

    // this function is a setup
    Addr * called_party_addr = (cpn ? cpn->get_addr() : 0);
    if (called_party_addr)  called_party_addr->encode(id+2);

    const u_char *tochar = _logos->MatchingLevel(id);
    if (tochar) {
      id[0]  = tochar[0];
      id[1]  = tochar[1];
      id[21] = 0x0;
      destination = new NodeID(id);
    }
  }

  attr_metrics * am = 0;
  if (destination) {
    if ( same_as(physical_nodeid, destination) == 0){ // special case of TO ME
      fuv->Push(new NodeID(destination->GetNID()),0);
      return_value = true;

    } else {

      q93b_setup_message *setup = fuv->GetSetup();

      UNI40_traffic_desc * td =
	(UNI40_traffic_desc *)setup->ie(InfoElem::ie_traffic_desc_ix);

      ie_bbc * bbc = (ie_bbc *)setup->ie(InfoElem::ie_broadband_bearer_cap_ix);

      // these two are required for all setups ...
      assert(td && bbc);

      u_int service_class = ServiceCategory(bbc, td);
      // generate a attr_metrics(mcr, acr, ctd, cdv, clr0, clr01)
      // mcr = min acceptable cell rate,                  def=0
      // acr = available/average?? cell rate
      // ctd = max cumulative cell transit delay          def=MAXINT-1
      // cdv = cumulative cell delay variation            def=MAXINT-1
      //clr0 =clr01= min acceptable cell loss ratio (log) def=0
      // defaults of MAXINT - 1 are used to allow MAXINT return values
      //      as special value
      int mcr = td->get_FPCR_01();
      int bmcr = td->get_BPCR_01();
      int cdv, ctd, clr;
      UNI40_xqos_param * xqos;
      if ((xqos =
	   (UNI40_xqos_param *)setup->ie(InfoElem::UNI40_xqos_param_ix))) {
	cdv = xqos->get_forw_cdv();
	clr = xqos->get_forw_clr();
	ctd = MAXINT-1; // end to end transit delay ie code is empty
      } else {
	cdv = MAXINT-1;
	clr = 0;
	ctd = MAXINT-1;
      }
      // this is a hack -- use attr_metrics to hold path requirements request
      // use acr to hold backward cell rate requirement
      am = new attr_metrics (mcr, bmcr, ctd , cdv, clr, clr);
      list<PNNI_crankback *> * cb = fuv->GetCrankbackList();
      int routing_cost = MAXINT;

      route = RouteCompute(source, destination,  physical_nodeid,
			   am, service_class, routing_cost,
			   cb, cause, crankback);

      // for logical nodes with ports ther can be multiple occurrences
      // if there are do the rest
      while (alternate_destinations && alternate_destinations->size()) {
	NodeID *alternate = alternate_destinations->pop();
	list <transit *> *another_route = 0;
	int alt_cost= MAXINT;
	another_route = RouteCompute(source, alternate,  physical_nodeid,
				     am, service_class, alt_cost,
				     cb, cause, crankback);
	if (another_route && alt_cost < routing_cost) {
	  routing_cost = alt_cost;
	  clear_route(route);
	  transfer_route(route, another_route);
	} else {
	  clear_route(another_route);
	}
	// another route is now empty
	delete another_route;
	another_route = 0;
	delete alternate;
      }
      if (alternate_destinations) delete alternate_destinations;

      // we are done with the crankback list, empty it and delete it
      while (cb && cb->size() > 0 ) {
	PNNI_crankback *cb_e = cb->pop();
	delete cb_e;
      }
      delete cb;
      cb = 0;

      delete am;
      if (route) {
	// a delicate balancing act to show the route as computed
	DIAG("sim.logos", DIAG_DEBUG,
	     cout << "BaseLogos Route: "<< endl
	     << "  from "<< *source <<endl
	     << "    to "<< *destination<<endl<<"   is ";
	     {
	       NodeID *previous_nid = 0;
	       list_item li;

	       forall_items(li, *route) {
		 transit *tr = route->inf(li);
		 NodeID *next_nid = tr->ShareNID();

		 if ( previous_nid ) {
		   if (same_as(next_nid, previous_nid) == 0) {
		     cout << " NOT in DTL";
		   } else if ( next_nid->GetLevel() == physical_level) {
		     cout <<"->0  in DTL";
		   }
		 }
		 cout << endl;
		 cout <<"       " << *next_nid<<":"<< tr->GetPort();
		 previous_nid = next_nid;
	       }
	       cout << endl;
	     }
	     );

	DIAG("sim.logos", DIAG_INFO,
	     assert(source && destination);
	     cout << "BaseLogos DTL " << *(_logos->My_NodeID()) << ": " << endl
	          << "  from " << *source << endl
	          << "    to " << *destination << endl
	          << "    is " << endl;
	     );

	list <DTLContainer *> final_route;
	while (route->size()) {

	  transit *tr = route->pop();
	  NodeID *top_nid = tr->TakeNID();

	  // peek at next entry and discard tr if nodeid's are same
	  // discard all but last identical nodeid's
	  //   This is how we avoid having phony border nodes in DTL

	  list_item li = route->first();
	  transit *next_tr = (li?route->inf(li):0);
	  while (next_tr && (same_as(top_nid, next_tr->ShareNID()) == 0)) {
	    delete tr;
	    delete top_nid;
	    tr = route->pop();
	    top_nid = tr->TakeNID();
	    li = route->first();
	    next_tr = (li?route->inf(li):0);
	  }

	  // discard port for all physical nodes except border exit node
	  // this allows the local cac on each switch to pick the best port
	  //
	  // For uplinks, a specific port may go to a bypass or exception
	  int port = tr->GetPort();
	  if (top_nid->GetLevel() == physical_level) {
	    if (next_tr && next_tr->GetLevel() == physical_level)
	      port = 0;
	  }
	  DIAG("sim.logos", DIAG_INFO,
	       cout  << "       " << *top_nid << ":" << port <<endl;);
	  final_route.append (new DTLContainer(top_nid, port));
	  delete tr;
	}
	fuv->Push(&final_route);
	delete route;
	return_value = true;

      } else {			// route == 0

	diag("sim.logos", DIAG_WARNING, "BaseLogos %s:\n\t"
	     "%s has no route to\n\t%s\n",
	     _logos->My_NodeID()->Print(), source->Print(), destination->Print());
	fuv->SetRelease(cause, crankback);
	return_value = false;
      }
    }
  } else {			// no destination present.
				// this will happen if We don't have a level
				// high enough to match
    diag("sim.logos", DIAG_WARNING, "BaseLogos %s: on %s unable to route"
	 " - no destination\n", MyNickName(), physical_nodeid->Print());
    if (fuv->GetMSGType() == FastUNIVisitor::FastUNISetup) {
      cause=new ie_cause(ie_cause::destination_out_of_order);
      fuv->SetRelease(cause);
    }
    return_value = false;
  }

  // cleanup
  // delete destination;
  delete destination; // if border yes, else no???
  if (physical_nodeid != 0) delete physical_nodeid;
  return return_value;
}

#ifdef NEW_CODE
list <NodeID *>
*BaseLogos::border_destination(FastUNIVisitor * fuv,
			       const NodeID  * & source,
			       int top_port,
			       const NodeID  * & destination,
			       const NodeID * physical_nodeid,
			       bool & immediate_return,
			       bool & immediate_return_val,
			       ie_cause  * & cause,
			       PNNI_crankback  * & crankback)
{
  list<NodeID *> * alternate_destinations=0;
  const DTLContainer * next_dtc = 0;

  if (source->GetLevel() != physical_nodeid->GetLevel()) {

    // border node
    if (top_port != 0) {
      // there are two cases
      // 1) I am not the border node in question - I have to route to it
      // b) I am     the border node in question
      next_dtc = fuv->Peek();
      const NodeID * upnode = next_dtc->GetNID();
      list<NodeID *> * borders =
	_logos->LogicalPortToBorder(source, top_port, upnode);
      
      if (borders) {
	// if my own physical_nodeid is on the list of border nodes
	// aggregated to top_dtc, then verify that the second element of
	// dtc is adjacent to me -- difficult since we haven't got
	//                          a graph to work with yet.
	// If it is not remove ourselves from the list
	// if it is then the route is trivial
	
	while (borders->size()) {
	  NodeID *b = borders->pop();
	  
	  if ( same_as(b, physical_nodeid) == 0 ) { // is me
	    if ( _logos->I_am_adjacent(upnode) ) {
	      delete physical_nodeid;
	      while ( borders->size() > 0 ) borders->pop();
	      delete borders;
	      if (alternate_destinations != 0)
		delete alternate_destinations;
	      immediate_return = true;
	      immediate_return_val = true;
	      return 0;
	    } else { // if I am not adjacent then leave me out - rare case
	      delete b;
	    }
	  } else { // not me
	    if (alternate_destinations == 0 )
	      alternate_destinations = new list<NodeID *>;
	    alternate_destinations->append(b);
	  }
	}
	if (alternate_destinations) {
	  destination = alternate_destinations->pop();
	  if (alternate_destinations->empty()) {
	    delete alternate_destinations;
	    alternate_destinations = 0 ;
	  }
	}
	delete borders;
      }
      if (destination == 0) {
	cause = new ie_cause(ie_cause::temporary_failure);
	crankback =
	  new PNNI_crankback((u_char)source->GetLevel(),
			     PNNI_crankback::BlockedNode,
			     PNNI_crankback::DTLTransitNotMyNodeID,
			     source);
	immediate_return = true;
	immediate_return_val = false;
      }
    } else if ((next_dtc = fuv->Pop())) // logical port == 0
      destination = next_dtc->GetNID();
  } else if ((next_dtc = fuv->Pop()))
    destination = next_dtc->GetNID();
  return alternate_destinations;
} 
#endif

// lowest level virtual plugin
list<transit *> *BaseLogos::RouteCompute(const NodeID * from, // source
					 const NodeID * to,   // destination
					 const NodeID *physical_nodeid,
					 attr_metrics *reqmet,// requirements
					 int service_class,   // RAIG Flags
					 int &routing_cost,   // calulated cost
					 list<PNNI_crankback *> * cb,
					 ie_cause * & cause,
					 PNNI_crankback * & crankback)
{
  node nfrom, nto;
  bool border_node = !(*physical_nodeid == *from);
  bool ok_to_route = true;
  list<transit *> *route = 0;

  // destination is "to" at lowest hierarchical representation
  NodeID * destination = new NodeID(to->GetNID());
  const u_char *tochar = _logos->MatchingLevel(to);
  if (tochar) {
    destination->SetLevel(tochar[0]);
    destination->SetChildLevel(tochar[1]);
  }

  // get the routing graph
  //
  // for border entry nodes get from physical up to but not including
  // level of "from". The LogosGraph will contain the current level
  // plus anything it is connected to (upward)
  //
  // for originating nodes, highest level = 0 (the entire graph)
  // for border nodes highest level = level message arrived at.

  int target_level = (border_node ? from->GetLevel() :0);
  LogosGraph * LG = _logos->GetGraph (target_level, Logos::routing);

  // find our physical nodeid in the graph - it MUST be here
  nfrom = verify_from(LG, physical_nodeid, from, cause, crankback);
  if ((ok_to_route = (nfrom != 0))) {

    // find the destination nodeid
    nto =  verify_to(LG, destination, from, cause, crankback);
    if ((ok_to_route = (nto != 0))) {

      prune_edges(LG, reqmet, service_class_index(service_class));
      prune_nodes(LG, nfrom);

      // if there is crankback info in the message process that now
      if (cb) {	ok_to_route = process_crankback (cb,LG); }
      // find the route
      if (ok_to_route) {
	route = GetRoute(LG, nfrom, nto, physical_nodeid,
			 reqmet->_ctd, routing_cost);
      }
    }
  }
  delete LG;
  delete destination;
  return route;
}

// djikstra
// Single Source Shortest Path
static void sssp(node s,                   // starting node
		 edge_array<int>&  cost,   // cost to traverse an edge
		 node_array<int>&  dist,   // distance of node from source
		 node_array<edge>& pred,   // edge leading to node
		 p_queue<int,node> * PQ,
		 LogosGraph *LG)
{
  node_array<pq_item> I(LG->_graph);
  node v;

  // set distance (cost) to all nodes to MAXIMUM
  forall_nodes(v, LG->_graph) {
    pred[v] = 0x0;
    dist[v] = MAXINT;
  }

  // distance to starting point is 0, insert start into priority queue
  dist[s] = 0;
  I[s] = PQ->insert(0,s);

  // while there are items in the priority queue
  while (! PQ->empty()) {
    pq_item it = PQ->find_min(); // minimum cost node in priority queue
    node u = PQ->inf(it);
    int du = dist[u];
    edge e;

    forall_adj_edges(e,u) {	// loop on edges to the node
      logos_edge * le = LG->_graph[e];
      if (le->Getroute_state()) {
	v = LG->_graph.target(e);
	int c = du + cost[e];	// cost to next node is the min of current cost
	if (c < dist[v]) {	// and cost to current node + cost of edge
	  if (dist[v] == MAXINT)
	    I[v] = PQ->insert(c,v); // insert on first encounter
	  else
	    PQ->decrease_p(I[v],c); // reset position in priority queue
	  dist[v] = c;		// remember cost
	  pred[v] = e;		// remember edge with least cost
	}
      }
    }
    PQ->del_item(it);		// remove current node
  }
}

bool BaseLogos::process_crankback (list<PNNI_crankback *> * cb, LogosGraph *LG)
{
  PNNI_crankback *cbe=0;
  dic_item di;
  logos_edge * le;
  bool result = true; // ok to route

  list_item it = cb->first();
  if (it) cbe = cb->inf(it);
  DIAG("sim.logos", DIAG_DEBUG,
       if (cbe) cout << "BaseLogos:: processing crankback info" << endl);

  // loop over all crankback elements
  while (cbe) {
    const PNNI_crankback::BlockedTransitTypes btt =
      cbe->GetBlockedTransitType( );
    const PNNI_crankback::CrankbackCauses  cbc =
      cbe->GetCrankbackCause();

    NodeID *Blocked_nodeid = 0;
    NodeID *Blocked_linkid = 0;
    u_int blocked_port=0;

    if ( btt == PNNI_crankback::BlockedNode  ||
	 btt == PNNI_crankback::BlockedLink ) {
      Blocked_nodeid = cbe->GetBlockedNode();
    }

    if ( btt ==  PNNI_crankback::BlockedLink) {
      blocked_port = cbe->GetBlockedPort();
      Blocked_linkid = cbe->GetEndNode();
    }

    const u_int cbl = cbe->GetBlockedLevel();
    DIAG("sim.logos", DIAG_DEBUG,
	 cout << "        BlockedTransitTypes = " << btt << endl;
	 cout << "         crankback cause    = " << cbc << endl;
	 cout << "         blocked level      = " << cbl << endl;
	 if ( btt == PNNI_crankback::BlockedNode)
	 cout << "         blocked nodeid     = " << *Blocked_nodeid<<endl;
	 else if ( btt ==  PNNI_crankback::BlockedLink) {
	   cout << "         blocked start node = " << *Blocked_nodeid<<endl;
	   cout << "         blocked    port    = " << blocked_port   <<endl;
	   cout << "         blocked end   node = " << *Blocked_linkid<<endl;
	 }
	 );
    // processing continues
    if ( btt == PNNI_crankback::BlockedNode ||
	 btt == PNNI_crankback::BlockedLink ) {
      // find the blocked node
      if (di = LG->_nid_to_node.lookup(Blocked_nodeid)) {
	node rn = LG->_nid_to_node.inf(di);
	edge e;

	forall_in_edges(e,rn) {
	  le = LG->_graph[e];

	  if (Blocked_linkid) { // BlockedLink
	    // deal with non-zero port someday
	    const logos_node *ln = le->GetEnd();
	    const NodeID *end_of_link = ln->ShareNID();
	    if (end_of_link->equals(Blocked_linkid))
	      le->Setroute_state((bool)false);

	  } else {		  // BlockedNode
	    le->Setroute_state((bool)false);
	  }
	}
      }
    } else { // SuuceedingEndOfInterface
      DIAG("sim.logos", DIAG_DEBUG,
	   cout << "BaseLogos:(" << MyNickName()
	   <<") RouteCompute cannot crankback - "<< endl
	   <<"     SucceedingEndOfInterface encountered" << endl;);
      result = false; // need to clean up cb
    }

    // clean up
    if (Blocked_nodeid) delete Blocked_nodeid;
    if (Blocked_linkid) delete Blocked_linkid;
    it = cb->succ(it);
    if (it && result) cbe = cb->inf(it);
    else cbe = 0;
  }
  return result;
}

// verify that a NodeID (our physical nodeid in particular) is in LogosGraph
// if it is not issue error message and set crankback
node BaseLogos::verify_from (LogosGraph * LG,
			     const NodeID *physical,
			     const NodeID *from,
			     ie_cause * & cause,
			     PNNI_crankback * & crankback)
{
  node nfrom = LG->GetNode(physical);
  if (!nfrom) {
    DIAG("sim.logos", DIAG_ERROR,
	 cout << "BaseLogos:("<< MyNickName() << ")"
	 << " RouteCompute: "<< *physical <<" not found" << endl
	 << " THIS is SERIOUS!\n"
	 << "       this switch " << *physical << endl
	 << "       is not in its own graph\n" );
    cause = new ie_cause(ie_cause::temporary_failure);
    crankback = new PNNI_crankback((u_char)from->GetLevel(),
				   PNNI_crankback::BlockedNode,
				   PNNI_crankback::NetworkOutOfOrder,
				   from);
  }
    return nfrom;
}

// verify that a NodeID (our physical nodeid in particular) is in LogosGraph
// if it is not issue error message and set crankback
node BaseLogos::verify_to (LogosGraph * LG,
			   const NodeID *destination,
			   const NodeID *from,
			   ie_cause * & cause,
			   PNNI_crankback * & crankback)
{
  node nto = LG->GetNode(destination);

  if (!nto) {

    DIAG("sim.logos", DIAG_ERROR,
	 cout << "BaseLogos:("<< MyNickName()
	 <<") RouteCompute: \"to\"= "
	      << *destination << "  not in graph\n";)
    cause = new ie_cause(ie_cause::temporary_failure);
    crankback = new PNNI_crankback((u_char)from->GetLevel(),
				   PNNI_crankback::BlockedNode,
				   PNNI_crankback::DestinationUnreachable,
				   from);
  }
  return nto;
}



// loop through all edges
//     get metrics for routing service class
//         mark any non-compliant edge as no route
void BaseLogos::prune_edges(LogosGraph * LG,
			    attr_metrics * reqmet,
			    int class_index)
{
  edge e;
  logos_edge * le=0;
  forall_edges(e, LG->_graph) {
    le = LG->_graph[e];
    le->Setservice_class_index(class_index);
    int reverse_acr = le->Reverse() ? le->Reverse()->GetMetric()->_mcr :MAXINT;
    attr_metrics * m = le->GetMetric();
    if ( (reqmet->_mcr    > m->_acr)     ||
	 (reqmet->_acr    > reverse_acr) ||
	 (reqmet->_clr_0  > m->_clr_0)   ||
	 (reqmet->_clr_01 > m->_clr_01)  ||
	 (reqmet->_ctd    < m->_ctd)     ||
	 (reqmet->_cdv    < m->_cdv)
	 ) {
      le->Setroute_state((bool)false);
    } else {
      le->Setroute_state((bool)true);
    }
  }
}

// loop through all nodes in LG
//    if a node is marked as no transit and is not the source(nfrom),
//    then eliminate all edges leading away from this node
// this allows us to route "to" and "from" a non-transit node but not "through"
void BaseLogos::prune_nodes (LogosGraph *LG, node nfrom)
{
  node n;
  edge e;
  logos_node * ln;
  logos_edge * le;

  forall_nodes(n, LG->_graph) {
    if ( n != nfrom) {
      ln = LG->_graph[n];
      if (ln->NodalFlag() & ig_nodal_info_group::restrans_bit) {
	forall_out_edges(e,n) {
	  le = LG->_graph[e];
	  le->Setroute_state(false);
	}
      }
    }
  }
}

list<transit *> * BaseLogos::GetRoute(LogosGraph *LG,
				      node nfrom,
				      node nto,
				      const NodeID *from,
				      int max_cost,
				      int &routing_cost)
{
  //
  // use dijkstra alogorithm to find path based on _cdt
  //
  edge_array<int>  cost(LG->_graph);
  node_array<int>  dist(LG->_graph);
  node_array<edge> pred(LG->_graph);
  p_queue<int,node>* PQ = new p_queue<int,node>;
  edge e;
  logos_node *ln=0;
  logos_edge *le=0;

  // the cost shoud vary from service to service
  // it is WRONG to use _ctd
  forall_edges(e, LG->_graph) {
      logos_edge *le = LG->_graph[e];
      attr_metrics *m = le->GetMetric();
      cost[e] = m->_ctd;
  }
  sssp(nfrom, cost, dist, pred, PQ, LG);
  delete PQ;
  routing_cost = dist[nto];
  if ( routing_cost > max_cost ) {
    DIAG("sim.logos", DIAG_DEBUG,
	 cout << "BaseLogos:(" << MyNickName()
	 <<") RouteCompute cannot route, ctd cost " << routing_cost
	 << " exceeds budget of "<< max_cost <<endl);
    return 0;
  }

  // generate the DTL from the pred array
  list<transit *> *route = new list<transit *>;
  // begin at the target node nto
  node np = nto;
  ln = LG->_graph[np];
  const NodeID *nid = ln->ShareNID();
  edge edgep = 0;
  int port=0;
  // looping till we reach the source (nid==from)
  // note that "from" is not inserted
  while (nid != 0 && same_as(nid,from) != 0) {
    Transit *Tr = ln->ShareTransit();
    transit *tr = new transit(Tr);
    route->push(tr);
    edgep = pred[np];
    if (edgep != 0) {
      np = LG->_graph.source(edgep);
      ln = LG->_graph[np];
      nid = ln->ShareNID();
      le = LG->_graph[edgep];
      port = le->Getport();
    } else {
      DIAG("sim.logos", DIAG_DEBUG,
	   cout <<"BaseLogos:(" << MyNickName()
	   << ") RouteCompute coding ERROR. " << endl
	   << " sssp() returned an inconsistent pred[] array" << endl
	   << "      on Node =" << *from << endl
	   << "   No pred for " << *nid << endl);
      nid = 0;
      // need to ripout route here but since this is a "cannot happen" error
      // I'm not going to bother.
    }
  }
  return route;
}

// condensed version of OwnerName() - up to first . from last .
// a.b.c.d become a.d - assumes that OwnerName is fixed.
const char * BaseLogos::MyNickName() {
  if (_nickname == 0) {
    const char *owner = _logos->LogosOwnerName();
    if (owner) {
      _nickname = new char[1 + strlen(owner)];
      char *first_dot = (char *)strchr (owner,'.');
      char *last_dot  = (char *)strrchr(owner,'.');
      int len;

      if (first_dot) {
	if (last_dot == first_dot) { /* only one dot */
	  strcpy(_nickname,owner);
	} else {
	  len =  (int)(first_dot - owner);
	  strncpy(_nickname, owner, len);
	  strcpy (_nickname+len, last_dot);
	}

      } else {
	strcpy(_nickname,owner);
      }
    }
  }
  return _nickname;
}

// --------------------------------------------------
transit::transit (const NodeID *nid, int port)
  : _transit_nid(0), _port(port)
{
  if (nid) _transit_nid = new NodeID(*nid);
}

transit::transit(Transit * Tr)
  : _transit_nid(0), _port(Tr->GetPort())
{
  assert(Tr != 0);

  if (Tr->ShareNID())
    _transit_nid = new NodeID(*(Tr->ShareNID()));
}

transit::~transit() { delete  _transit_nid; }

// remove and return transit node id
NodeID *transit::TakeNID(void)
{
  NodeID *val = _transit_nid;
  _transit_nid = 0;
  return val;
}

NodeID *transit::ShareNID(void) { return _transit_nid; }

int transit::GetPort(void)    { return _port; }

int transit::GetLevel(void)
{
  return (_transit_nid != 0 ? _transit_nid->GetLevel() : 0);
}
