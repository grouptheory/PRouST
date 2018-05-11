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
#ifndef __GRAPH_CC__
#define __GRAPH_CC__

#ifndef LINT
static char const _graph_cc_rcsid_[] =
"$Id: graph.cc,v 1.48 1999/01/07 17:40:15 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <fstream.h>
#include <DS/containers/graph.h>

// -------------------------------------------------
node_struct::node_struct(void) { }

node_struct::~node_struct( ) { }

edge node_struct::first_adj_edge(void) const 
{ 
  if (_out_edges.first())
    return _out_edges.head(); 
  return 0;
}

edge node_struct::first_in_edge(void) const 
{ 
  if (_in_edges.first())
    return _in_edges.head(); 
  return 0;
}

edge node_struct::first_edge(void) const 
{ 
  if (_out_edges.first())
    return _out_edges.head(); 
  return 0;
}

edge node_struct::first_out_edge(void) const 
{ 
  if (_out_edges.first())
    return _out_edges.head(); 
  return 0;
}

edge node_struct::last_adj_edge(void) const 
{ 
  if (_out_edges.last())
    return _out_edges.tail(); 
  return 0;
}

edge node_struct::last_in_edge(void) const 
{ 
  if (_in_edges.last())
    return _in_edges.tail(); 
  return 0;
}

edge node_struct::last_out_edge(void) const 
{ 
  if (_out_edges.last())
    return _out_edges.tail(); 
  return 0;
}

edge node_struct::last_edge(void) const 
{ 
  if (_in_edges.last())
    return _in_edges.tail(); 
  return 0;
}

// ---- This now will continue on to in edges after iterating over all
//      out edges ----
edge node_struct::succ_edge(edge e) const 
{
  edge rval = 0;
  list_item li;
  if (li = _out_edges.lookup(e)) {
    if (li = _out_edges.next(li))
      rval = _out_edges.inf(li);
    else if (li = _in_edges.first())
      rval = _in_edges.inf(li);
  } else if (li = _in_edges.lookup(e)) {
    if (li = _in_edges.next(li))
      rval = _in_edges.inf(li);
  }
  return rval;
}

edge node_struct::pred_edge(edge e) const 
{
  edge rval = 0;
  list_item li;
  if (li = _in_edges.lookup(e)) {
    if (li = _in_edges.pred(li))
      rval = _in_edges.inf(li);
    else if (li = _out_edges.last())
      rval = _out_edges.inf(li);
  } else if (li = _out_edges.lookup(e)) {
    if (li = _out_edges.pred(li))
      rval = _out_edges.inf(li);
  }
  return rval;
}

edge node_struct::succ_out_edge(edge e) const 
{
  edge rval = 0;
  list_item li;
  if (li = _out_edges.lookup(e))
    if (li = _out_edges.next(li))
      rval = _out_edges.inf(li);
  return rval;
}

edge node_struct::pred_out_edge(edge e) const 
{
  edge rval = 0;
  list_item li;
  if (li = _out_edges.lookup(e))
    if (li = _out_edges.pred(li))
      rval = _out_edges.inf(li);
  return rval;
}

edge node_struct::succ_in_edge(edge e) const 
{
  edge rval = 0;
  list_item li;
  if (li = _in_edges.lookup(e))
    if (li = _in_edges.next(li))
      rval = _in_edges.inf(li);
  return rval;
}

edge node_struct::pred_in_edge(edge e) const 
{
  edge rval = 0;
  list_item li;
  if (li = _in_edges.lookup(e))
    if (li = _in_edges.pred(li))
      rval = _in_edges.inf(li);
  return rval;
}

void node_struct::del_edge(edge e)
{
  list_item li;
  if (li = _out_edges.lookup(e))
    _out_edges.del_item(li);
  if (li = _in_edges.lookup(e))
    _in_edges.del_item(li);
}

void node_struct::del_adj_edge(edge e)
{
  list_item li;
  if (li = _out_edges.lookup(e))
    _out_edges.del_item(li);
}

void node_struct::del_in_edge(edge e)
{
  list_item li;
  if (li = _in_edges.lookup(e))
    _in_edges.del_item(li);
}

void node_struct::append_adj_edge(edge e)
{
  if (!_out_edges.lookup(e))
    _out_edges.append(e);
}

void node_struct::append_in_edge(edge e)
{
  if (!_in_edges.lookup(e))
    _in_edges.append(e);
}

void node_struct::insert_adj_edge(edge e, edge e1, dir direction)
{
  list_item li;
  if (!(li = _out_edges.lookup(e1)))
    li = _out_edges.last();
  if (!_out_edges.lookup(e))
    _out_edges.insert(e, li, direction);
}

void node_struct::insert_in_edge(edge e, edge e1, dir direction)
{
  list_item li;
  if (!(li = _in_edges.lookup(e1)))
    li = _in_edges.last();
  if (!_in_edges.lookup(e))
    _in_edges.insert(e, li, direction);
} 

int indeg(node  n) { return n->_in_edges.length(); }
int outdeg(node n) { return n->_out_edges.length(); }
int degree(node n) { return indeg(n) + outdeg(n); }

// -------------------------------------------------
edge_struct::edge_struct(node s, node t) :
  _source(s), _target(t) 
{ 
  _source->append_adj_edge(this);
  _target->append_in_edge(this);
}

edge_struct::~edge_struct( ) { }

edge edge_struct::adj_edge(void) const 
{ return _source->first_adj_edge(); }

edge edge_struct::succ_edge(void) const 
{ return _source->succ_edge((edge_struct *)this); }

edge edge_struct::pred_edge(void) const 
{ return _source->pred_edge((edge_struct *)this); }

node source(edge e) { return e->_source; }
node target(edge e) { return e->_target; }
node opposite(node n, edge e)
{
  node rval = 0;

  if (e->_source == n)
    rval = e->_target;
  else if (e->_target == n)
    rval = e->_source;
  return rval;
}

// -------------------------------------------------
graph::graph(void) { }
graph::~graph() { }

int graph::number_of_nodes(void) { return _nodes.length(); }
int graph::number_of_edges(void) { return _edges.length(); }

list<node> graph::all_nodes(void) const { return _nodes; }
list<edge> graph::all_edges(void) const { return _edges; }

list<edge> graph::adj_edges(node v) const 
{ 
  return v->_out_edges;
}
list<edge> graph::out_edges(node v) const 
{ 
  return v->_out_edges;
}

list<edge> graph::in_edges(node v)  const 
{ 
  return v->_in_edges;
}

list<node> graph::adj_nodes(node v) const 
{ 
  return v->_adj_nodes;;
}

node graph::first_node(void)  const 
{ 
  if (_nodes.first())
    return _nodes.head(); 
  return 0;
}

node graph::last_node(void)   const 
{ 
  if (_nodes.last())
    return _nodes.tail(); 
  return 0;
}
node graph::choose_node(void) const { return first_node(); }
node graph::succ_node(node v) const 
{ 
  node rval = 0;
  list_item li;
  if (li = _nodes.lookup(v))
    if (li = _nodes.next(li))
      rval = _nodes.inf(li);
  return rval;
}

node graph::pred_node(node v) const 
{ 
  node rval = 0;
  list_item li;
  if (li = _nodes.lookup(v))
    if (li = _nodes.pred(li))
      rval = _nodes.inf(li);
  return rval;
}

edge graph::first_edge(void)  const 
{ 
  if (_edges.first())
    return _edges.head(); 
  return 0;
}
edge graph::last_edge(void)   const 
{ 
  if (_edges.last())
    return _edges.tail(); 
  return 0;
}
edge graph::choose_edge(void) const { return first_edge(); }
edge graph::succ_edge(edge e) const 
{
  edge rval = 0;
  list_item li;
  if (li = _edges.lookup(e))
    if (li = _edges.next(li))
      rval = _edges.inf(li);
  return rval;
}
edge graph::pred_edge(edge e) const 
{
  edge rval = 0;
  list_item li;
  if (li = _edges.lookup(e))
    if (li = _edges.pred(li))
      rval = _edges.inf(li);
  return rval;
}

edge graph::first_adj_edge(node v) const { return v->first_adj_edge(); }
edge graph::last_adj_edge(node v)  const { return v->last_adj_edge(); }

edge graph::first_in_edge(node v) const { return v->first_in_edge(); }
edge graph::last_in_edge(node v) const { return v->last_in_edge(); }

node graph::source(edge e) { return ::source(e); }
node graph::target(edge e) { return ::target(e); }

node graph::new_node(void) 
{ 
  node n = new node_struct;
  _nodes.append( n );
  return n;
}

edge graph::new_edge(node v, node w) 
{ 
  edge e = new edge_struct(v, w);
  _edges.append( e );
  return e;
}

edge graph::new_edge(edge e, node w, dir direction) 
{
  edge rval = new edge_struct(source(e), w);
  list_item li = _edges.lookup(e);
  _edges.insert(rval, li, direction);
  return rval;
}

edge graph::new_edge(node v, edge e, dir direction) 
{ 
  edge rval = new edge_struct(v, target(e));
  list_item li = _edges.lookup(e);
  _edges.insert(rval, li, direction);
  return rval;
}

edge graph::new_edge(edge e1, edge e2, dir direction)
{  
  edge rval = new edge_struct(source(e1), target(e2));
  list_item li = _edges.lookup(e1);
  _edges.insert(rval, li, direction);
  return rval;
}

// hunh?
void graph::hide_edge(edge e) { }
// hunh?
void graph::restore_edge(edge e) { }

void graph::del_node(node v) 
{ 
  // remove it from the graph only ...
  list_item li;
  if (li = _nodes.lookup(v))
    _nodes.del_item(li);

}

void graph::del_edge(edge e) 
{ 
  // remove it from the graph only ...
  list_item li;
  if (li = _edges.lookup(e))
    _edges.del_item(li);

}

void graph::move_edge(edge e, node start, node end)
{
  source(e)->del_edge(e);
  target(e)->del_edge(e);

  start->_out_edges.append(e);
  // start->_in_edges.append(e);
  end->_in_edges.append(e);
  // end->_out_edges.append(e);
  e->_source = start;
  e->_target = end;
}

void graph::clear(void) 
{ 
  _nodes.clear();
  _edges.clear();
}

void graph::sort_edges(void) { _edges.sort(); }
void graph::sort_nodes(void) { _nodes.sort(); }

void graph::write(const char * filename) 
{ 
  ofstream ofs(filename);
  write(ofs);
}

void graph::write(ostream & os)
{
  list_item li;
  os << "Nodes " << _nodes.length() << endl;
  forall_items(li, _nodes) {
    os << _nodes.inf(li) << endl;
  }
  os << "Edges " << _edges.length() << endl;
  forall_items(li, _edges) {
    edge e = _edges.inf(li);
    os << source(e) << " -> " << target(e) << endl;
  }
}

int  graph::read(const char * filename) 
{ 
  ifstream ifs(filename);
  return read(ifs);
}

int  graph::read(istream & is)
{
  return -1;
}

// -------------------------------------------------
template < class VTYPE, class ETYPE >
VTYPE GRAPH< VTYPE, ETYPE >::get_node(node_struct * n) const
{ 
  VTYPE rval;
  dic_item di;
  if (di = _n2v.lookup(n))
    rval = _n2v.inf(di);
  return rval;
}

template < class VTYPE, class ETYPE >
VTYPE GRAPH< VTYPE, ETYPE >::inf(node_struct * n) const
{ 
  return GRAPH< VTYPE, ETYPE >::get_node(n);
}

template < class VTYPE, class ETYPE >
ETYPE GRAPH< VTYPE, ETYPE >::get_edge(edge_struct * e) const
{ 
  ETYPE rval;
  dic_item di;
  if (di = _e2e.lookup(e))
    rval = _e2e.inf(di);
  return rval;
}

template < class VTYPE, class ETYPE >
ETYPE GRAPH< VTYPE, ETYPE >::inf(edge_struct * e) const
{ 
  return GRAPH< VTYPE, ETYPE >::get_edge(e);
}

template < class VTYPE, class ETYPE >
VTYPE & GRAPH< VTYPE, ETYPE >::operator [] (node n) const
{ 
  static VTYPE v;
  dic_item di;
  if (di = _n2v.lookup(n))
    v = _n2v.inf(di);
  return v;
}

template < class VTYPE, class ETYPE >
ETYPE & GRAPH< VTYPE, ETYPE >::operator [] (edge e) const
{
  static ETYPE ev = 0;
  dic_item di;
  if (di = _e2e.lookup(e))
    ev = _e2e.inf(di);
  return ev;
}

template < class VTYPE, class ETYPE >
void GRAPH< VTYPE, ETYPE >::assign(node n, VTYPE vn) 
{ 
  dic_item di;
  if (di = _n2v.lookup(n))
    _n2v.change_inf(di, vn);
}

template < class VTYPE, class ETYPE >
void GRAPH< VTYPE, ETYPE >::assign(edge e, ETYPE ve) 
{ 
  dic_item di;
  if (di = _e2e.lookup(e))
    _e2e.change_inf(di, ve);
}

template < class VTYPE, class ETYPE >
edge GRAPH< VTYPE, ETYPE >::new_edge(node n1, node n2, ETYPE e) 
{
  edge rval = graph::new_edge(n1, n2);

  _e2e.insert(rval, e);
  return rval;
}

template < class VTYPE, class ETYPE >
void GRAPH< VTYPE, ETYPE >::del_edge(edge e)
{
  dic_item di;
  if (di = _e2e.lookup(e))
    _e2e.del_item(di);
  graph::del_edge(e);
}
template < class VTYPE, class ETYPE >
node GRAPH< VTYPE, ETYPE >::new_node(VTYPE vn) 
{
  node rval = graph::new_node();
  _n2v.insert(rval, vn);
  return rval;
}

template < class VTYPE, class ETYPE >
void GRAPH< VTYPE, ETYPE >::del_node(node n) 
{
  dic_item di;
  if (di = _n2v.lookup(n))
    _n2v.del_item(di);

  graph::del_node(n);
}

template < class VTYPE, class ETYPE >
void GRAPH< VTYPE, ETYPE >::write(const char * filename) 
{ 
  ofstream ofs(filename);
  write(ofs);
}

template < class VTYPE, class ETYPE >
void GRAPH< VTYPE, ETYPE >::write(ostream & os)
{
  list_item li;
  os << "Nodes " << _nodes.length() << endl;
  forall_items(li, _nodes) {
    node n = _nodes.inf(li);
    VTYPE v = inf(n);
    os << v << endl;
  }
  os << "Edges " << _edges.length() << endl;
  forall_items(li, _edges) {
    edge e = _edges.inf(li);
    ETYPE d = inf(e);
    os << d << endl;
  }
}

template < class VTYPE, class ETYPE >
int GRAPH< VTYPE, ETYPE >::read(const char * filename)
{
  ifstream ifs(filename);
  return read(ifs);
}

template < class VTYPE, class ETYPE >
int GRAPH< VTYPE, ETYPE >::read(istream & is)
{
  return -1;
}

// -------------------------------------------------
template < class T >
edge_array< T >::edge_array(graph & g) 
{ 
  list_item li;
  list<edge> eds = g.all_edges();
  forall_items(li, eds) {
    edge e = eds.inf(li);
    T foo;  // lets hope it has a default ctor ...
    _e2t.insert(e, foo);
  }
}

template < class T >
edge_array< T >::edge_array(graph & g, T val) 
{ 
  list_item li;
  list<edge> eds = g.all_edges();
  forall_items(li, eds) {
    edge e = eds.inf(li);
    _e2t.insert(e, val);
  }
}

template < class T >
edge_array< T >::~edge_array( ) 
{ 
  dic_item di;
  while (_e2t.empty() == false)
    _e2t.del_item( _e2t.first() );
}

template < class T >
T & edge_array< T >::operator [] (edge e) const 
{ 
  dic_item di;
  if (di = _e2t.lookup(e))
    return _e2t.inf(di);
}

// -------------------------------------------------
template < class T >
node_array< T >::node_array(graph & g) 
{ 
  list_item li;
  list<node> nods = g.all_nodes();
  forall_items(li, nods) {
    node n = nods.inf(li);
    T foo;  // lets hope it has a default ctor ...
    _n2t.insert(n, foo);
  }
}

template < class T >
node_array< T >::node_array(graph & g, T val) 
{ 
  list_item li;
  list<node> nods = g.all_nodes();
  forall_items(li, nods) {
    node n = nods.inf(li);
    _n2t.insert(n, val);
  }
}

template < class T >
node_array< T >::~node_array( ) 
{ 
  dic_item di;
  while (_n2t.empty() == false)
    _n2t.del_item( _n2t.first() );
}

template < class T >
T & node_array< T >::operator [] (node n) const 
{ 
  dic_item di;
  if (di = _n2t.lookup(n))
    return _n2t.inf(di);
}

#endif // __GRAPH_CC__
