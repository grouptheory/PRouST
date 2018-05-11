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
static char const _Node_cc_rcsid_[] =
"$Id: Node.cc,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>
#include "Node.h"
#include "Edge.h"

Node::Node(void) 
  : _name(0), _x(0), _y(0), _z(0), 
    degree(0), inDegree(0), outDegree(0),
    intField(0), fixed(false), selected(false),
    flag(false), QT(0) { }

Node::Node(const char * name)
  : _name(0), _x(0), _y(0), _z(0), 
    degree(0), inDegree(0), outDegree(0),
    intField(0), fixed(false), selected(false),
    flag(false), QT(0)
{
  if (name) {
    _name = new char [ strlen(name) + 1 ];
    strcpy(_name, name);
  }
}

Node::~Node() 
{ 
  delete [] _name;
}

void Node::insertNeighbor(Edge * e)
{
  incidentEdges.addElement(e);
  if (this == e->from)
    neighbors.addElement(e->to);
  else
    neighbors.addElement(e->from);
  degree++;

  if (e->isDirected && (this == e->from)) {
    outEdges.addElement(e);
    outNeighbors.addElement(e->to);
    outDegree++;
  }
  if (e->isDirected && (this == e->to)) {
    inEdges.addElement(e);
    inNeighbors.addElement(e->from);
    inDegree++;
  }
}

void Node::deleteNeighbor(Edge * e)
{
  incidentEdges.remElement(e);
  if (this == e->from)
    neighbors.remElement(e->to);
  else if (this == e->to)
    neighbors.remElement(e->from);
  else // throw new Error ("Not Found");
    cerr << "ERROR:  Neighbor not found!" << endl;
  degree--;

  if (e->isDirected && (this == e->from)) {
    outEdges.remElement(e);
    outNeighbors.remElement(e->to);
    outDegree--;
  }
  if (e->isDirected && (this == e->to)) {
    inEdges.remElement(e);
    inNeighbors.remElement(e->from);
    inDegree--;
  }
}

void Node::sortDirectedNeighborsByX(void)
{
  if (inDegree > 1) {
    double max = ((Node *)(inNeighbors[0]))->_x;
    for (int i = 1; i < inDegree; i++) {
      Node * n = (Node *)inNeighbors [i]; 
      Edge * e = (Edge *)inEdges [i];
      if (n->_x >= max) {
	max = n->_x; 
	continue;
      }
      int j = i - 2; 
      while ((j >= 0) && (((Node *)inNeighbors[j])->_x > n->_x)) 
	--j;
      for (int k = i - 1; k > j; k--) {
	inNeighbors[k + 1] = inNeighbors[k];
	inEdges[k + 1] = inEdges[k];
      }
      inNeighbors [j + 1] = n; inEdges [j + 1] = e;
    }
  }
  if (outDegree > 1) {
    double max = ((Node *)outNeighbors[0])->_x;
    for (int i = 1; i < outDegree; i++) {
      Node * n = (Node *)outNeighbors[i]; 
      Edge * e = (Edge *)outEdges[i];
      if (n->_x >= max) {
	max = n->_x; 
	continue;
      }
      int j = i - 2; 
      while ((j >= 0) && (((Node *)outNeighbors[j])->_x > n->_x)) 
	--j;
      for (int k = i - 1; k > j; k--) {
	outNeighbors[k + 1] = outNeighbors[k];
	outEdges[k + 1] = outEdges[k];
      }
      outNeighbors[j + 1] = n; outEdges[j + 1] = e;
    }
  }
}

const char * Node::toString(void) const
{
  return _name;
}

double Node::x(void) const { return _x; }
double Node::y(void) const { return _y; }
double Node::z(void) const { return _z; }

void   Node::x(double x) { _x = x; }
void   Node::y(double y) { _y = y; }
void   Node::z(double z) { _z = z; }
