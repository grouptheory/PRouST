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
#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#ifndef LINT
static char const _QuadTree_h_rcsid_[] =
"$Id: QuadTree.h,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif

// This class represents a quad-tree as used in the Barnes-Hut
// algorithm for approximating many-body interaction.

// Note that, out of consistency with Java's coordinate system,
// y increases southwards rather than northwards.
class QuadTree {
public:

  QuadTree(double xMin, double yMin, double xMax, double yMax);
  QuadTree(double xMin, double yMin, double xMax, double yMax, QuadTree * parent);
  virtual ~QuadTree();

  QuadTree * lookUp(double nX, double nY);

  // insert (nX, nY) adds a node to the (sub)tree rooted at this
  // whose coordinates are (nX, nY)..
  void insert (double nX, double nY);

  int _size;                            // number of nodes in cell
  double _x, _y;                        // center of mass of nodes in cell
  double _forceX, _forceY;              // used for Barnes-Hut algorithm
  QuadTree * _parent;                   // parent of cell, null if cell is root
  QuadTree * _NW, * _NE, * _SW, * _SE;  // children of cell

  int _numberOfOperations;

  // The following variables define the bounding box of the cell.
  double _xMin, _yMin, _xMax, _yMax;

private:

  void splitCell (void);
};

#endif
