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
#ifndef __EDGE_H__
#define __EDGE_H__

#ifndef LINT
static char const _Edge_h_rcsid_[] =
"$Id: Edge.h,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif

#include "Object.h"

class Node;

// This class is the base class for edges in graph structures.
// The isDirected field determines if the edge is directed or
// undirected.
class Edge : public Object {
public:

  Edge(Node * f, Node * t);
  Edge(Node * f, Node * t, bool dir);
  virtual ~Edge();

  char * toString(void) const;

  Node * from;
  Node * to;
  bool isDirected;
  bool reversed;
};

#endif
