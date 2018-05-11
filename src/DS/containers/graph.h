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
#ifndef __GRAPH_H__
#define __GRAPH_H__

#ifndef LINT
static char const _graph_h_rcsid_[] =
"$Id: graph.h,v 1.20 1999/01/07 17:40:13 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>

class edge_struct;
class node_struct;
class graph;

typedef node_struct * node;
typedef edge_struct * edge;

class node_struct {
  friend class graph;
  friend int indeg(node  n);
  friend int outdeg(node n);
  friend int degree(node n);
public:

  node_struct(void);
  ~node_struct( );

  edge first_adj_edge(void) const;
  edge last_adj_edge(void) const;
  edge first_in_edge(void) const;
  edge last_in_edge(void) const;
  edge first_out_edge(void) const;
  edge last_out_edge(void) const;
  edge first_edge(void) const;
  edge last_edge(void) const;

  edge succ_edge(edge e) const;
  edge pred_edge(edge e) const;
  edge succ_in_edge(edge e) const;
  edge pred_in_edge(edge e) const;
  edge succ_out_edge(edge e) const;
  edge pred_out_edge(edge e) const;

  void append_adj_edge(edge e);
  void insert_adj_edge(edge e, edge e1, dir direction);
  void del_adj_edge(edge e);

  void append_in_edge(edge e);
  void insert_in_edge(edge e, edge e1, dir direction);
  void del_in_edge(edge e);

  void del_edge(edge e);

private:

  // LEDA does it differently, there we have adj edges 
  //   for undirected, and in/out edges for directed.
  //   Here I maintain both in and out edges even in an
  //   undirected graph.
  list<edge> _in_edges;
  list<edge> _out_edges;

  list<node> _adj_nodes;
};

class edge_struct {
  friend class graph;
  friend node source(edge e);
  friend node target(edge e);
  friend node opposite(node n, edge e);
public:

  edge_struct(node s, node t);
  ~edge_struct( );

  edge adj_edge(void) const;
  edge succ_edge(void) const;
  edge pred_edge(void) const;

private:

  node _source;
  node _target;
};

class graph {
public:

  graph(void);
  virtual ~graph();

  // Accessors
  int  number_of_nodes(void);
  int  number_of_edges(void);

  list<node> all_nodes(void) const;
  list<edge> all_edges(void) const;

  list<edge> adj_edges(node v) const;
  list<edge> out_edges(node v) const;
  list<edge> in_edges(node v)  const;
  list<node> adj_nodes(node v) const;

  node first_node(void)  const;
  node last_node(void)   const;
  node choose_node(void) const;
  node succ_node(node v) const;
  node pred_node(node v) const;

  edge first_edge(void)  const;
  edge last_edge(void)   const;
  edge choose_edge(void) const;
  edge succ_edge(edge e) const;
  edge pred_edge(edge e) const;

  edge first_adj_edge(node v) const;
  edge last_adj_edge(node v)  const;
  edge first_in_edge(node v) const;
  edge last_in_edge(node v) const;

  node source(edge e);
  node target(edge e);

  // Mutators
  node new_node(void);
  edge new_edge(node v, node w);
  edge new_edge(edge e, node w, dir direc = after);
  edge new_edge(node v, edge e, dir direc = after);
  edge new_edge(edge e1, edge e2, dir direc = after);
  void hide_edge(edge e);
  void restore_edge(edge e);
  void del_node(node v);
  void del_edge(edge e);
  void move_edge(edge e, node start, node end);

  void clear(void);
  void sort_edges(void);
  void sort_nodes(void);

  void  write(const char * filename);
  int   read(const char * filename);

  virtual void write(ostream & os);
  virtual int  read(istream & is);

protected:

  //  dictionary< node, list< edge > * > _adjacencies;
  list< node > _nodes;
  list< edge > _edges;
};

// x is a node or edge, y is a graph
#define forall_nodes(node, graph) \
      for ( (node) = (graph).first_node(); (node); (node) = (graph).succ_node(node) )

#define forall_edges(edge, graph) \
      for ( (edge) = (graph).first_edge(); (edge); (edge) = (graph).succ_edge(edge) )

// x is an edge, y is a node
#define forall_out_edges(x, y) \
      for ( (x) = (y)->first_out_edge(); (x); (x) = (y)->succ_out_edge(x) )

#define forall_adj_edges(x, y) \
      forall_out_edges(x, y)

#define forall_in_edges(x, y) \
      for ( (x) = (y)->first_in_edge(); (x); (x) = (y)->succ_in_edge(x) )

template < class VTYPE, class ETYPE > 
class GRAPH : public graph {
public:

  VTYPE inf(node_struct * n) const;
  ETYPE inf(edge_struct * e) const;

  VTYPE & operator [] (node_struct * n) const;
  ETYPE & operator [] (edge_struct * e) const;

  void  assign(node n, VTYPE vn);
  void  assign(edge e, ETYPE ve);

  edge  new_edge(node n1, node n2, ETYPE e);
  void  del_edge(edge e);
  node  new_node(VTYPE vn);
  void  del_node(node n);

  void  write(const char * filename);
  int   read(const char * filename);

  void  write(ostream & os);
  int   read(istream & is);

  VTYPE get_node(node_struct * n) const;
  ETYPE get_edge(edge_struct * e) const;

private:

  dictionary<node, VTYPE>  _n2v;
  dictionary<edge, ETYPE>  _e2e;
};

template < class T > class edge_array {
public:

  edge_array(graph & g);
  edge_array(graph & g, T val);
  virtual ~edge_array();

  T & operator [] (edge e) const;

private:

  dictionary<edge, T> _e2t;
};

template < class T > class node_array {
public:

  node_array(graph & g);
  node_array(graph & g, T val);
  virtual ~node_array();

  T & operator [] (node n) const;

private:

  dictionary<node, T> _n2t;
};


#endif // __GRAPH_H__
