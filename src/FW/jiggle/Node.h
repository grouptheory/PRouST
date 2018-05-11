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
#ifndef __NODE_H__
#define __NODE_H__

#ifndef LINT
static char const _Node_h_rcsid_[] =
"$Id: Node.h,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif

#include "Object.h"
#include "Vector.h"

class Edge;
class QuadTree;

// This class is the base class for nodes in graph structures.  It does
// not behave well with multiple edges connected to the same adjacent node.
class Node : public Object {
public:

  Node(void);
  Node(const char * name);
  virtual ~Node();

  void insertNeighbor(Edge * e);
  void deleteNeighbor(Edge * e);
  
  void sortDirectedNeighborsByX(void);

  const char * toString(void) const;

  char * _name;

  int degree;
  int inDegree;
  int outDegree;
  int intField;

  // Vectors will make this much easier ...
  Vector incidentEdges; // Edge ** incidentEdges;
  Vector inEdges;       // Edge ** inEdges;
  Vector outEdges;      // Edge ** outEdges;
  Vector neighbors;     // Node ** neighbors;
  Vector inNeighbors;   // Node ** inNeighbors;
  Vector outNeighbors;  // Node ** outNeighbors;

  bool fixed;
  bool selected;
  bool flag;

  QuadTree * QT;  // For Barnes-Hut

  double x(void) const;
  double y(void) const;
  double z(void) const;
  
  void   x(double x);
  void   y(double y);
  void   z(double z);

private: // Need to keep track of where these are set

  double _x;
  double _y;
  double _z;
};

#endif
