// -*- C++ -*-
//
//                       -----NOTICE----
//
// Permission to use, copy, and modify this software and its
// documentation is hereby granted, provided that this permission
// notice appear in all copies of the software, derivative works or
// modified versions, and any portions thereof, and that this notice
// appears in supporting documentation.
//
// (This file contains code that is a direct port of the Java software
// package: JIGGLE, which was developed by Daniel Tunkelang, as part
// of his Doctoral Thesis at the Department of Computer Science,
// Carnegie Mellon University.  The original Java code on which this
// port is based was obtained with Daniel Tunkelang's consent in
// August 1998.  For more current information on the status of the
// JIGGLE project, please refer to http://www.cs.cmu.edu/~quixote/)
//
// NRL AND DARPA ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS"
// CONDITION AND DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES
// WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
//
// NRL and DARPA request users of this software to return modifications,
// improvements or extensions that they make to:
//
//                 sean-dev@cmf.nrl.navy.mil
//                         -or-
//                Naval Research Laboratory, Code 5590
//                Center for Computation Science
//                Washington, D.C.  20375
//
// and grant NRL and DARPA the rights to redistribute these changes in
// future upgrades.
#ifndef LINT
static char const _Graph_cc_rcsid_[] =
"$Id: Graph.cc,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "Graph.h"
#include "Node.h"
#include "Edge.h"
#include "DynamicArray.h"
#include "My.h"

Graph::Graph(void) 
  : numberOfNodes(0), numberOfEdges(0),
    _nodes(0), _edges(0), numberMarked(0) { }

Graph::~Graph() 
{ 
  clear();
}

void Graph::clear(void)
{
  // Should I clean them out as well?
  if (_nodes) {
    for (int i = 0; i < numberOfNodes; i++)
      delete _nodes[i];
    delete [] _nodes; 
  }
  if (_edges) {
    for (int i = 0; i < numberOfEdges; i++)
      delete _edges[i];
    delete [] _edges;
  }
  numberOfNodes = 0;
  numberOfEdges = 0;
  _nodes = 0;
  _edges = 0;
  numberMarked = 0;
}

Node * Graph::insertNode(Node * n)
{
  Node ** tmp = DynamicArray::add(_nodes, numberOfNodes++, n);
  delete [] _nodes;
  _nodes = tmp;
  return n;
}

Edge * Graph::insertEdge(Edge * e)
{
  e->from->insertNeighbor(e);
  e->to->insertNeighbor(e);
  Edge ** tmp = DynamicArray::add(_edges, numberOfEdges++, e);
  delete [] _edges;
  _edges = tmp;
  return e;
}

Edge * Graph::insertUndirectedEdge(Node * f, Node * t)
{
  return insertEdge(new Edge(f, t));
}

Edge * Graph::insertDirectedEdge(Node * f, Node * t)
{
  return insertEdge(new Edge(f, t, true));
}

Node * Graph::deleteNode(Node * n)
{
  for (int i = 0; i < n->degree; i++) {
    Edge * e = (Edge *)n->incidentEdges[i];
    ((Node *)n->neighbors[i])->deleteNeighbor(e);
    DynamicArray::remove(_edges, numberOfEdges--, e);
  }
  DynamicArray::remove(_nodes, numberOfNodes--, n);
  return n;
}

Edge * Graph::deleteEdge(Edge * e)
{
  e->from->deleteNeighbor(e);
  e->to->deleteNeighbor(e);
  DynamicArray::remove(_edges, numberOfEdges--, e);
  return e;
}

bool Graph::isConnected (void)
{
  if (numberOfNodes == 0)
    return false;
  for (int i = 0; i < numberOfNodes; i++)
    _nodes[i]->flag = false;
  numberMarked = 0;
  dft(_nodes[0]);
  return (numberMarked == numberOfNodes);
}

void Graph::dft(Node * n)
{
  n->flag = true; ++numberMarked;
  for (int i = 0; i < n->degree; i++) {
    if (! ((Node *)n->neighbors[i])->flag)
      dft((Node *)n->neighbors[i]);
  }
}

void Graph::bufferInsertNode(Node * n)
{
  nodeInsertionBuffer.addElement(n);
}

void Graph::bufferInsertEdge(Edge * e)
{
  edgeInsertionBuffer.addElement(e);
}

void Graph::bufferDeleteNode(Node * n)
{
  nodeDeletionBuffer.addElement(n);
}

void Graph::bufferDeleteEdge(Edge * e)
{
  edgeDeletionBuffer.addElement(e);
}

void Graph::bufferMove(Node * n, double x, double y)
{
  bufferMove(n, x, y, 0);
}

void Graph::bufferMove(Node * n, double x, double y, double z)
{
  for (int i = 0; i < moveBuffer.size(); i++) {
    Move * move = (Move *)moveBuffer.element(i);  // returns the Element at the ith position
    if (move->node == n) {
      move->x = x;
      move->y = y;
      move->z = z;
      return;
    }
  }
  moveBuffer.addElement(new Move(n, x, y, z));
}

void Graph::flushBuffer(void)
{
  int i;
  for (i = 0; i < nodeInsertionBuffer.size(); i++)
    insertNode( (Node *)nodeInsertionBuffer.element(i) );
  if (nodeInsertionBuffer.size() > 0) 
    nodeInsertionBuffer.clear();

  for (i = 0; i < edgeInsertionBuffer.size(); i++)
    insertEdge( (Edge *)edgeInsertionBuffer.element(i) );
  if (edgeInsertionBuffer.size() > 0)
    edgeInsertionBuffer.clear();

  for (i = 0; i < nodeDeletionBuffer.size(); i++)
    deleteNode( (Node *)nodeDeletionBuffer.element(i) );
  if (nodeDeletionBuffer.size() > 0)
    nodeDeletionBuffer.clear();

  for (i = 0; i < edgeDeletionBuffer.size(); i++)
     deleteEdge( (Edge *)edgeDeletionBuffer.element(i) );
  if (edgeDeletionBuffer.size() > 0)
    edgeDeletionBuffer.clear();

  for (i = 0; i < moveBuffer.size(); i++) {
    Move * move = (Move *)moveBuffer.element(i);

    move->node->x( move->x ); 
    move->node->y( move->y ); 
    move->node->z( move->z );
    
    delete move;
  }
  if (moveBuffer.size() > 0) 
    moveBuffer.clear();
}

void Graph::loadGraph(istream & in) 
{
  int i = 0;
  // clear the graph first
  clear();

  char buf[256];
  in.getline(buf, 256);  // Number of nodes
  int numNodes = My::stringToInt(buf);
  
  for (i = 0; i < numNodes; i++) {
    in.getline(buf, 256); // Node name
    Node * v = insertNode(new Node(buf));
    in.getline(buf, 256); // Node x coord
    v->x( My::stringToInt(buf) );
    in.getline(buf, 256); // Node y coord
    v->y( My::stringToInt(buf) );
  }
  in.getline(buf, 256); // Number of edges
  int numEdges = My::stringToInt(buf);
  for (i = 0; i < numEdges; i++) {
    in.getline(buf, 256); // From index
    int from = My::stringToInt(buf);
    in.getline(buf, 256); // To index
    int to   = My::stringToInt(buf);
    in.getline(buf, 256); // Directed? 0/1
    bool dir = (My::stringToInt(buf) == 1);
    insertEdge(new Edge( _nodes[from], _nodes[to], dir ));
  }
}

void Graph::saveGraph (ostream & out) 
{
  out << numberOfNodes << endl;
  for (int i = 0; i < numberOfNodes; i++) {
    Node * v = _nodes[i];
    out << v->toString() << endl
	<< v->x() << endl
	<< v->y() << endl;
    v->intField = i;
  }
  out << numberOfEdges << endl;
  for (int j = 0; j < numberOfEdges; j++) {
    Edge * e = _edges[j];
    out << e->from->intField << endl
	<< e->to->intField << endl
	<< (e->isDirected ? "1" : "0") << endl;
  }
}

void Graph::scramble (void) 
{
  for (int i = 0; i < numberOfNodes; i++) {
    Node * n = _nodes[i];
    int width = 700, height = 700;
    int z = (width + height) / 2;
    bufferMove(n, My::randomInt(-width, width), My::randomInt(-height, height)); // , My::random(0, z));
  }
  flushBuffer();
}

Node * Graph::nodes( int index ) const
{
  assert( index >= 0 && index < numberOfNodes);
  return _nodes[index];
}

Edge * Graph::edges( int index ) const
{
  assert( index >= 0 && index < numberOfEdges);
  return _edges[index];
}

// ------------------------------------------
Move::Move(Node * n, double x, double y, double z)
  : node(n), x(x), y(y), z(z) { }

Move::~Move( ) { }
