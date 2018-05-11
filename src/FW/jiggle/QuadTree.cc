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
static char const _QuadTree_cc_rcsid_[] =
"$Id: QuadTree.cc,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "QuadTree.h"

QuadTree::QuadTree(double xMin, double yMin, double xMax, double yMax)
  : _size(0), _x(0), _y(0), _forceX(0), _forceY(0),
    _parent(0), _NW(0), _NE(0), _SW(0), _SE(0), 
    _numberOfOperations(0), _xMin(xMin), _yMin(yMin), 
    _xMax(xMax), _yMax(yMax) { }

QuadTree::QuadTree(double xMin, double yMin, double xMax, double yMax, QuadTree * parent)
  : _size(0), _x(0), _y(0), _forceX(0), _forceY(0),
    _parent(parent), _NW(0), _NE(0), _SW(0), _SE(0), 
    _numberOfOperations(0), _xMin(xMin), _yMin(yMin), 
    _xMax(xMax), _yMax(yMax) { }

QuadTree::~QuadTree() 
{ 
  // There should be no back links ...
  delete _NW;
  delete _NE;
  delete _SW;
  delete _SE;
}

QuadTree * QuadTree::lookUp(double nX, double nY)
{
  _numberOfOperations += 1;
  if ((_x == nX) && (_y == nY)) return this;
  else if (_size <= 1) return 0;
  else {
    QuadTree * child = 0;
    double cx = (_xMin + _xMax) / 2, 
      cy = (_yMin + _yMax) / 2;
    if (nX < cx) { if (nY < cy) child = _NW; else child = _SW; } 
    else { if (nY < cy) child = _NE; else child = _SE; }
    return child->lookUp(nX, nY);
  }
}

void QuadTree::insert(double nX, double nY)
{
  _numberOfOperations += 1;
  if (_size == 0) { _x = nX; _y = nY; _size = 1; return; }
  if (_size == 1) splitCell();
  _x = (_x * _size + nX) / (_size + 1); 
  _y = (_y * _size + nY) / (_size + 1);
  ++_size;
  QuadTree * child = 0;
  double cx = (_xMin + _xMax) / 2, 
         cy = (_yMin + _yMax) / 2;
  if (nX < cx) { if (nY < cy) child = _NW; else child = _SW; }
  else { if (nY < cy) child = _NE; else child = _SE; }
  child->insert(nX, nY);
}

void QuadTree::splitCell(void)
{
  _numberOfOperations += 1;
  double cx = (_xMin + _xMax) / 2, cy = (_yMin + _yMax) / 2;

  // This should prevent it from continuing when two nodes share the same coords
  assert( (cx != _xMin && cy != _yMin) );
  // If you are dying here, you have two vertices at the same x, y coordinate!

  _NW = new QuadTree(_xMin, _yMin,    cx,    cy, this);
  _NE = new QuadTree(   cx, _yMin, _xMax,    cy, this);
  _SW = new QuadTree(_xMin,    cy,    cx, _yMax, this);
  _SE = new QuadTree(   cx,    cy, _xMax, _yMax, this);
  QuadTree * child = 0;
  if (_x < cx) { if (_y < cy) child = _NW; else child = _SW; }
  else { if (_y < cy) child = _NE; else child = _SE; }
  child->insert(_x, _y);
}
