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
static char const _SpringChargeModel_cc_rcsid_[] =
"$Id: SpringChargeModel.cc,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>

#include "SpringChargeModel.h"
#include "Graph.h"
#include "Node.h"
#include "Edge.h"
#include "QuadTree.h"
#include "DynamicArray.h"

extern "C" {
#include <math.h>
#include <assert.h>
};

// #include <sys/ddi.h> causes parse errors ...
int max(int a, int b)
{
  return (a > b ? a : b);
}

int min(int a, int b)
{
  return (a < b ? a : b);
}


SpringChargeModel::SpringChargeModel(double k, int sprLaw, int stRepulsionLaw,
		  int ltRepulsionLaw, bool BH)
  : _k(0), _kSquared(0), _kCubed(0), _twok(0), _springLaw(Model::NONE),
    _shortRangeRepulsionLaw(Model::NONE), _longRangeRepulsionLaw(Model::NONE),
    _BarnesHut(false), _grid(0), _gridX(0), _gridY(0), _size(0)
{
  _k = k; _springLaw = sprLaw;
  _shortRangeRepulsionLaw = stRepulsionLaw;
  _longRangeRepulsionLaw = ltRepulsionLaw;
  _BarnesHut = BH;
}

SpringChargeModel::SpringChargeModel(double k, int sprLaw, int repulsionLaw,
		  bool BH)
  : _k(0), _kSquared(0), _kCubed(0), _twok(0), _springLaw(Model::NONE),
    _shortRangeRepulsionLaw(Model::NONE), _longRangeRepulsionLaw(Model::NONE),
    _BarnesHut(false), _grid(0), _gridX(0), _gridY(0), _size(0)
{
  _k = k; _springLaw = sprLaw;
  _shortRangeRepulsionLaw = repulsionLaw;
  _longRangeRepulsionLaw = repulsionLaw;
  _BarnesHut = BH;
}

SpringChargeModel::SpringChargeModel(double k, int sprLaw, int stRepulsionLaw,
		  int ltRepulsionLaw)
  : _k(0), _kSquared(0), _kCubed(0), _twok(0), _springLaw(Model::NONE),
    _shortRangeRepulsionLaw(Model::NONE), _longRangeRepulsionLaw(Model::NONE),
    _BarnesHut(false), _grid(0), _gridX(0), _gridY(0), _size(0)
{
  _k = k; _springLaw = sprLaw;
  _shortRangeRepulsionLaw = stRepulsionLaw;
  _longRangeRepulsionLaw = ltRepulsionLaw;
  _BarnesHut = false;
}

SpringChargeModel::SpringChargeModel(double k, int sprLaw, int repulsionLaw)
  : _k(0), _kSquared(0), _kCubed(0), _twok(0), _springLaw(Model::NONE),
    _shortRangeRepulsionLaw(Model::NONE), _longRangeRepulsionLaw(Model::NONE),
    _BarnesHut(false), _grid(0), _gridX(0), _gridY(0), _size(0)
{
  _k = k; _springLaw = sprLaw;
  _shortRangeRepulsionLaw = repulsionLaw;
  _longRangeRepulsionLaw = repulsionLaw;
  _BarnesHut = false;
}

SpringChargeModel::~SpringChargeModel() { }

double * SpringChargeModel::computeGradient(Graph & g, double * gradient, int & gradientLength)
{
  int n = g.numberOfNodes, i = 0;
  _kSquared = _k * _k; 
  _kCubed = _k * _kSquared; 
  _twok = 2 * _k;
  
  if ((gradient == 0) || (gradientLength < 2 * n)) {
    gradientLength = 2 * n;
    gradient = new double [ gradientLength ];
  }
    
  for (i = 0; i < 2 * n; i++) 
    gradient[i] = 0;
  computeSprings(g, gradient);
  computeRepulsion(g, gradient);
  for (i = 0; i < n; i++) {
    if (g.nodes(i)->fixed) 
      gradient[2 * i] = gradient[2 * i + 1] = 0;
  }
  return gradient;
}

void SpringChargeModel::computeSprings(Graph & g, double * gradient)
{
  int n = g.numberOfNodes;
  for (int i = 0; i < n; i++) {
    Node * v1 = g.nodes(i);
    for (int j = 0; j < v1->degree; j++) {
      Node * v2 = (Node *)v1->neighbors[j];
      double x = v2->x() - v1->x(), 
	     y = v2->y() - v1->y();
      Edge * e = (Edge *)v1->incidentEdges[j];
      if (e->isDirected) {
	y += ((v1 == e->from) != e->reversed) ? - _k : _k;
	y *= 2;
	if ((y < 0) == (v1 == e->from)) y *= 4;
      }
      double w = 0;
      switch (_springLaw) {
        case LOG: {
	  double d = sqrt(x * x + y * y);
	  w = (log(d) / d) / (log(_k) / _k);
	  }
	  break;
        case LINEAR: 
	  w = 1; 
	  break;
        case QUADRATIC:
	  w = sqrt(x * x + y * y) / _k;
	  break;
      }
      gradient[2 * i    ] += w * x;
      gradient[2 * i + 1] += w * y;
    }
  }
}

void SpringChargeModel::computeRepulsion(Graph & g, double * gradient)
{
  if (_BarnesHut) 
    computeRepulsionWithBH(g, gradient);
  else if (_longRangeRepulsionLaw == NONE)
    computeRepulsionWithGridVariant(g, gradient);
  else computeRepulsionNaively(g, gradient);
}

void SpringChargeModel::computeRepulsionNaively(Graph & g, double * gradient)
{
  int n = g.numberOfNodes;
  for (int i = 0; i < n - 1; i++) {
    Node * v1 = g.nodes(i);
    for (int j = i + 1; j < n; j++) {
      Node * v2 = g.nodes(j);
      double x = v1->x() - v2->x(), 
	     y = v1->y() - v2->y(), 
	     d2 = x * x + y * y;
      if (d2 == 0) {
	// throw new Error ("Coinicident Nodes");
	cerr << "ERROR:  Coinicident Nodes." << endl;
	exit(1);
      }
      double w = 0;
      if (d2 <= 4 * _kSquared) {
	switch (_shortRangeRepulsionLaw) {
	  case INVERSE: 
	    w = _kSquared / d2; break;
	  case INVERSESQUARE: 
	    w = _kCubed / (d2 * sqrt(d2)); break;
	}
      }	else {
	switch (_longRangeRepulsionLaw) {
	  case INVERSE: 
	    w = _kSquared / d2; break;
	  case INVERSESQUARE: 
	    w = _kCubed / (d2 * sqrt(d2)); break;
	}
      }
      gradient[2 * i] += w * x; gradient[2 * i + 1] += w * y;
      gradient[2 * j] -= w * x; gradient[2 * j + 1] -= w * y;
    }
  }
}

void SpringChargeModel::computeRepulsionWithGridVariant(Graph & g, double * gradient)
{
  int i;
  int n = g.numberOfNodes; if (n < 2) return;
  int xMin = 0, xMax = 0, yMin = 0, yMax = 0;
  for (i = 0; i < n; i++) {
    Node * v = g.nodes(i);
    int x = (int) (v->x() / (_twok)), 
        y = (int) (v->y() / (_twok));
    if (x < xMin) xMin = x; if (x > xMax) xMax = x;
    if (y < yMin) yMin = y; if (y > yMax) yMax = y;
  }
  if ((_gridX < xMax - xMin + 1) || (_gridY < yMax - yMin + 1)) {
    _gridX = xMax - xMin + 1; _gridY = yMax - yMin + 1;
    // allocate the Node * arrays
    _grid = new Node *** [_gridX]; // _grid = new Node * [_gridX][_gridY][1]; 
    for (i = 0; i < _gridX; i++) {
      _grid[i] = new Node ** [_gridY];
      for (int j = 0; j < _gridY; j++)
	_grid[i][j] = new Node * [ 1 ];
    }
    // Allocate the size arrays
    _size = new int * [_gridX];
    for (i = 0; i < _gridX; i++)
      _size[i] = new int [_gridY];
  }
  for (int x = 0; x < xMax - xMin; x++)
    for (int y = 0; y < yMax - yMin; y++) 
      _size [x] [y] = 0;
  for (i = 0; i < n; i++) {
    Node * v = g.nodes(i);
    int x = (int) (v->x() / (_twok)) - xMin, 
        y = (int) (v->y() / (_twok)) - yMin;
    _grid [x] [y] = DynamicArray::add(_grid[x][y], (_size[x][y])++, v); 
  }
  for (i = 0; i < n; i++) {
    Node * v1 = g.nodes(i);
    int gx = (int) (v1->x() / (_twok)) - xMin, 
        gy = (int) (v1->y() / (_twok)) - yMin;
    for (int a = max(0, gx - 1); a <= min(gx + 1, xMax - xMin); a++) {
      for (int b = max(0, gy-1); b <= min(gy+1,yMax-yMin); b++) {
	for (int c = 0; c < _size[a][b]; c++) {
	  Node * v2 = _grid[a][b][c];
	  if (v1 == v2) continue;
	  double x = v1->x() - v2->x(), 
	         y = v1->y() - v2->y();
	  double d2 = x * x + y * y;
	  if (d2 == 0) {
	    // throw new Error ("Coinicident Nodes");
	    cerr << "ERROR:  Coinicident Nodes." << endl;
	    exit(1);
	  }
	  if (d2 > 4 * _kSquared) continue;
	  double w = 0;
	  switch (_shortRangeRepulsionLaw) {
	    case INVERSE: 
	      w = _kSquared / d2; break;
	    case INVERSESQUARE: 
	      w = _kCubed / (d2 * sqrt(d2)); break;
	  }
	  gradient [2 * i] += w * x; gradient [2 * i + 1] += w * y;
	}
      }
    }
  }
}

void SpringChargeModel::computeRepulsionWithBH(Graph & g, double * gradient)
{
  int n = g.numberOfNodes, i = 0;
  if (n <= 1) return;
  double xMin = 0, xMax = 0, yMin = 0, yMax = 0;
  for (i = 0; i < n; i++) {
    Node * v = g.nodes(i); 
    double x = v->x(), y = v->y();
    if (x < xMin) xMin = x; if (x > xMax) xMax = x;
    if (y < yMin) yMin = y; if (y > yMax) yMax = y;
  }
  QuadTree * QT = new QuadTree(xMin-1, yMin-1, xMax+1, yMax+1);
  for (i = 0; i < n; i++)
    QT->insert(g.nodes(i)->x(), g.nodes(i)->y());
  for (i = 0; i < n; i++)
    g.nodes(i)->QT = QT->lookUp(g.nodes(i)->x(), g.nodes(i)->y());
  for (i = 0; i < n; i++) {
    Node * v = g.nodes(i);
    QuadTree * qt = v->QT, * current = qt;
    assert(current);
    while (current->_parent != 0) {
      if (current != current->_parent->_NW)
	computeQuadTreeRepulsion(qt, current->_parent->_NW);
      if (current != current->_parent->_NE)
	computeQuadTreeRepulsion(qt, current->_parent->_NE);
      if (current != current->_parent->_SW)
	computeQuadTreeRepulsion(qt, current->_parent->_SW);
      if (current != current->_parent->_SE)
	computeQuadTreeRepulsion(qt, current->_parent->_SE);
      current = current->_parent;
    }
  }
  computeQuadTreeForces(QT);
  for (i = 0; i < n; i++) {
    Node * v = g.nodes(i);
    gradient [2 * i    ] += v->QT->_forceX;
    gradient [2 * i + 1] += v->QT->_forceY;
  }

  delete QT;
}

void SpringChargeModel::computeQuadTreeRepulsion(QuadTree * QT1, QuadTree * QT2)
{
  if ((QT1 == QT2) || (QT2->_size == 0)) return;
  else if ((QT2->_size == 1) || wellSeparated(QT1, QT2)) {
    double x = QT1->_x - QT2->_x, 
           y = QT1->_y - QT2->_y, 
          d2 = x * x + y * y;
    if (d2 == 0) {
      // throw new Error ("Coinicident Nodes");
      cerr << "ERROR:  Coinicident Nodes." << endl;
      exit(1);
    }
    double w = 0;
    if (d2 <= 4 * _kSquared) {
      switch (_shortRangeRepulsionLaw) {
        case INVERSE: 
	  w = _kSquared / d2; break;
        case INVERSESQUARE: 
	  w = _kCubed / (d2 * sqrt(d2)); break;
      }
    } else {
      switch (_longRangeRepulsionLaw) {
        case INVERSE: 
	  w = _kSquared / d2; break;
        case INVERSESQUARE: 
	  w = _kCubed / (d2 * sqrt(d2)); break;
      }
    }
    QT1->_forceX += w * QT2->_size * x; QT1->_forceY += w * QT2->_size * y;
    QT2->_forceX -= w * QT1->_size * x; QT2->_forceY -= w * QT1->_size * y;
  } else {
    computeQuadTreeRepulsion(QT1, QT2->_NW);
    computeQuadTreeRepulsion(QT1, QT2->_NE);
    computeQuadTreeRepulsion(QT1, QT2->_SW);
    computeQuadTreeRepulsion(QT1, QT2->_SE);
  }
}

bool SpringChargeModel::wellSeparated(QuadTree * QT1, QuadTree * QT2)
{
  return ((QT1->_xMax != QT2->_xMin) && (QT2->_xMax != QT1->_xMin) &&
	  (QT1->_yMax != QT2->_yMin) && (QT2->_yMax != QT1->_yMin));
}

void SpringChargeModel::computeQuadTreeForces(QuadTree * qt)
{
  if ((qt != 0) && (qt->_size >= 2)) {
    qt->_NW->_forceX += qt->_forceX; qt->_NW->_forceY += qt->_forceY;
    qt->_NE->_forceX += qt->_forceX; qt->_NE->_forceY += qt->_forceY;
    qt->_SW->_forceX += qt->_forceX; qt->_SW->_forceY += qt->_forceY;
    qt->_SE->_forceX += qt->_forceX; qt->_SE->_forceY += qt->_forceY;
    computeQuadTreeForces (qt->_NW); computeQuadTreeForces (qt->_NE);
    computeQuadTreeForces (qt->_SW); computeQuadTreeForces (qt->_SE);
  }
}
