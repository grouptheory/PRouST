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
#ifndef __LOGOSGRAPH_H__
#define __LOGOSGRAPH_H__

#ifndef LINT
static char const _LogosGraph_h_rcsid_[] =
"$Id: LogosGraph.h,v 1.13 1999/01/21 21:56:46 marsh Exp $";
#endif

#include <DS/containers/graph.h>
#include <DS/containers/dictionary.h>
#include <codec/pnni_ig/id.h>
#include <sim/logos/atoms.h>
#include <sim/logos/transit.h>

class LogosGraph {
public:

  LogosGraph(int lowest_level);
  LogosGraph(NodeID *creator = 0);
  ~LogosGraph();

  node insert_node(NodeID * nid, int logical_port = 0);
  node insert_logosnode_copy(logos_node * ln);
  node GetNode(const NodeID * nid);

  void SetReversePaths(void);	// set _reverse in logos_edges
  void FinNodes(void);		// set _node in logos_nodes (Finish)

  bool insert_logical_port(int logical_port, const NodeID *);
  list<NodeID *> * LookupPort_Shared(int logical_port);

  // Diagnostic Interface
  void ShowLogosGraph(char * file_name);
  void SaveLogosGraph(char * file_name);

  void PrintNodes(ostream & os=cout) const;
  void PrintEdges(ostream & os=cout) const;
  void PrintTransits(ostream & os=cout) const;

  void Print_nid_to_node(ostream & os=cout) const;
  void Print_port_to_NodeID(ostream & os=cout) const;

  // Data members
  NodeID *                          _creator;
  int                               _level;
  GRAPH<logos_node *, logos_edge *> _graph;
  dictionary<const NodeID *, node>  _nid_to_node;
  dictionary<Transit *, node>       _Transit_to_node;
  // maybe null (used by aggregation)
  dictionary<int, list<NodeID *> *> _port_to_NodeID;

};

#endif
