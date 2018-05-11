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
#ifndef __LOGOS_H__
#define __LOGOS_H__

#ifndef LINT
static char const _Logos_h_rcsid_[] =
"$Id: Logos.h,v 1.58 1999/03/05 17:30:38 marsh Exp $";
#endif

#include <iostream.h>
#include <FW/actors/State.h>
#include <sim/logos/atoms.h>
#include <sim/logos/transit.h>
#include <codec/pnni_ig/id.h>

#include <DS/containers/graph.h>
#include <DS/containers/dictionary.h>
#include <DS/containers/list.h>
#include <DS/containers/p_queue.h>
#include <DS/containers/sortseq.h>

class ds_String;
class FastUNIVisitor;
class NodeID;
class Nodalinfo;
class BaseLogos;
class LogosGraph;
class ig_nodal_state_params;
class Logos : public State {

  friend class Logos_Helper;	// define as needed (NOT for simulation)
				// see RouteOpt.cc or route_opt.cc
  friend logos_edge * process(Logos * gcac, const NodeID * nid,
			      ig_horizontal_links *horiz);
  friend logos_edge * processfwd(Logos * gcac, const NodeID * nid,
				 ig_uplinks * upl);
  friend logos_edge * processbwd(Logos * gcac, const NodeID * nid,
				 ig_uplinks * upl);

public:

  enum prune_type {
    routing,
    aggregation
  };

  Logos( const NodeID * nid, ds_String * plugin_prefix = 0, bool omni = false );

  virtual ~Logos (void);
  // set the routing plugin
  void router(BaseLogos *routing_plugin);
  // get the routing plugin
  BaseLogos * router(void);
  // Returns the node for the provided Node ID
  //  node     Access(const NodeID * nid) const;
  // Returns the Node ID for the provided ATM End Sys. Address
  NodeID * GetNodeID(const Addr * addr) const;
  // Returns the ATM ESA for the provided Node ID
  //  Addr   * GetATMAddr(const NodeID * nid) const;

  // Inserts the logos_edge into the dictionary ONLY
  edge     Insert_Edge(logos_edge * e);
  // Inserts the logos_node into the dictionary ONLY
  node     Insert_Node (logos_node * n);
  // Inserts the Node ID, node pair into the dictionary ONLY
  // void     Insert_Node (NodeID * nid, node n);
  // Inserts the Node ID, ATM ESA pair into the dictionary ONLY
  //void     Insert(NodeID * nid, Addr * a);

  // Returns the logos_node  corresponding to node -- See subscript operator
  //  logos_node *Access(node n);
  // Returns the logos_edge  corresponding to edge -- See subscript operator
  //  logos_edge *Access(edge e);
  // Changes the logos_node information relating to the node
  //  void       Assign(node n, logos_node * ln);
  // Changes the logos_edge information relating to the edge
  //  void       Assign(edge e, logos_edge * le);

  // Creates a new node in the graph, adds it to the dictionary and returns it.
  node       NewNode(logos_node * ln);
  // Creates a new node in the graph, adds it to the dictionary and returns it.
  node       NewNode(NodeID * nid);
  // Creates a new edge in the graph, adds it to the dictionary and returns it.
  edge       NewEdge(logos_node *start, logos_node *end, logos_edge *le);
  // Creates a new edge in the graph, adds it to the dictionary and returns it.
  edge       NewEdge(node start, node end, logos_edge * le);
  // Creates a new edge in the graph, adds it to the dictionary and returns it.
  edge       NewEdge(NodeID *source, NodeID *target,
		     attr_metrics *met=0, int port=-1);
  // return logos_node associated with a NodeID or Transit pointer
  logos_node * Getlogos_node(const NodeID * nid);
  logos_node * Getlogos_node(Transit * tr);

  // return node associated with a NodeID or Transit
  node Get_node(const NodeID *nid);
  node Get_node(Transit *tr);

  // returns pointer to list of Levels
  list<NodeID *> * GetLevels(void);

  // get a LogosGraph - target_level is lowest value to exclude (routing)
  LogosGraph * GetGraph(int target_level, prune_type trim);
  // get a LogosGraph for Aggregator
  LogosGraph * GetGraph(NodeID *LogicalNodeID);
  
  // is a node my immedaite neighbor?
  bool I_am_adjacent(const NodeID *some_node);
  
  virtual State * Handle(Visitor* v);

  // Does nothing 
  virtual void  Interrupt(SimEvent* e);

  // Removes all nodes and edges from _graph and builds a
  // new _graph representation of the network from Database
  void    Rebuild(prune_type trim = routing);
  // Consults the Database and updates _graph accordingly.
  void    Update(prune_type trim = routing);
  // removes contents of _graph
  void    Clear(void);

  // Writes the graph 
  void    SaveGraph(char * filename); // use "/dev/tty" to see in gdb
  int     ReadGraph(char * filename);
  void    ShowGraph(char * filename);  // for visualizer

  // set connectivity from node to a set of nodes -
  //     ignores bandwidth requirements
  void Election_Connectivity(sortseq<const NodeID *, Nodalinfo *> *NodeList);
  // return hierarchical NodeID data that best matches nid
  const u_char *MatchingLevel(const NodeID *nid);
  const u_char *MatchingLevel(const u_char *id);

  // get a copy of our physical nodeid
  const NodeID * My_NodeID(void);
  // do a logocal Node to physical Node lookup
  list<NodeID *> * LogicalPortToBorder(const NodeID *ln, int lport,
				      const NodeID *upnode);

  // the owner name - needed by BaseLogos classes for Output ID strings
  const char * LogosOwnerName(void);
  const char * MyNickName(void);

private:

  fw_Interface * QueryInterface( const char * ) const;

  // debugging diagnostic - in gdb you can: p help() when in a Logos method
  void help(void);

  void _Logos(void);	// set default values

  NodeID          * _LocNodeID;
  list<ig_ptse *> * CullHUPLinks(void);
  void              CullNodalInfo(void);
  void              CullNodalStateParameters(prune_type trim=routing);

  bool  MyFamily(const NodeID *GroupLeader); 
  
  // dijkstra single source shortest path 
  void sssp ( node s,
	      edge_array<int>&  cost,
	      node_array<int>&  dist,
	      node_array<edge>& pred,
	      p_queue<int,node>* PQ);

  void Add_Radial_Links(dictionary<const NodeID *, list<ig_resrc_avail_info *> *>
			* Default_Radius);

  void Add_ByPasses(dictionary<const NodeID *, list<ig_nodal_state_params *> *>
		    * Bypass_Exception);

  void Replace_UpLinks(void);
  void split_incoming_edge (edge e,
		      const list<ig_resrc_avail_info *> *RAIG);
  void split_outgoing_edge (edge e,
		      const list<ig_resrc_avail_info *> *RAIG);
  void add_central_radius(logos_edge *le,
			  logos_node *central,
			  logos_node *border,
			  const list<ig_resrc_avail_info *> *RAIG);
  node  validate_transit_node(const NodeID *n, int  port,
			      logos_edge *le, bool in,
			      const list<ig_resrc_avail_info *> *RAIG);
  node demand_node ( LogosGraph *lg, Transit *tr, bool insert_missing );
  
  GRAPH<logos_node *, logos_edge *> _graph;
  dictionary<const NodeID *, node>  _nid_to_node;
  dictionary<Transit *, node>       _Transit_to_node;
  dictionary<logos_node *, node>    _ln_to_node;
  dictionary<logos_edge *, edge>    _le_to_edge;
  // transient list of uplinks. filled and emptied while building _graph 
  list <logos_edge *>               _uplinks;
  // copy of _uplinks .. must be cleaned in Clear()
  list <logos_edge *>               _borderlinks;
  
  BaseLogos                       * _router;
  list <NodeID *>                   _levels;
  char                            * _nickname;
  // used to allow SaveGraph and ShowGraph while rebulding
  bool                              _rebuilding; 
  bool                              _omni;

  static const VisitorType * _fast_uni_type;
  static const VisitorType * _db_intro_type;
  static const VisitorType * _np_flood_type;
  static const VisitorType * _read_type;
  static const VisitorType * _save_type;
  static const VisitorType * _show_type;
  static const VisitorType * _election_type;
};

NodeID * Logos_NodeID(const NodeID *nid);
NodeID * Logos_NodeID(const unsigned char *id);

// lexical compare contents of two NodeID pointers
int same_as( const NodeID *lhs, const NodeID *rhs);

#endif // __LOGOS_H__

