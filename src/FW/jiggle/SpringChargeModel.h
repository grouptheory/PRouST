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
#ifndef __SPRINGCHARGEMODEL_H__
#define __SPRINGCHARGEMODEL_H__

#ifndef LINT
static char const _SpringChargeModel_h_rcsid_[] =
"$Id: SpringChargeModel.h,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif

#include "Model.h"

class Graph;
class QuadTree;
class Node;

// This model generalizes the physically based models that
// represent nodes as mutually repelling point charges and
// edges as springs.

// The constructor requires that five parameters be specified:

// 1) The optimal edge length k--that is, the optimal length
// of an edge in the absence of other forces acting on the
// endpoints.

// 2) The force law for springs, which can be logarithmic,
// linear, or quadratic in the edge length.

// 3) The force law for short-range repulsion, which can be
// inverse or inverse square in the distance between nodes.

// 4) The force law for long-range repulsion, which can be
// inverse or inverse square in the distance between nodes.
// It is also possible to specify that there be no long-range
// repulsion.

// 5) Whether or not the Barnes-Hut tree-code is to be used
// to approximate long-range repulsion.

// The short-range repulsion affects nodes that are at most 2k
// apart; the long-range repuslion law is used for nodes that
// are further apart.  The constants are chosen to make the
// repulsion laws agree at 2k unless the long-range repulsion
// is set to be zero.

// If an edge is undirected, then its associated spring is at
// rest when the distance between the endpoints is zero--unless
// its force law is logarithmic, in which case the rest length
// depends on k.

// If an edge is directed, then its associated spring is at rest
// when the "from" node and "to" nodes have the same x coordinate
// but the "to" node's y coordinate exceeds the "from" node's y
// coordinate by exactly k--or vice versa if the edge is reversed.
// The spring force for directed edge is amplified, especially in
// the y direction, in order to place a greater emphasis on
// preserving flow in the drawing.

// Performace is unpredictable in the presence of a directed
// cycle that has not been broken through edge reversal.
class SpringChargeModel : public Model {
public:

  SpringChargeModel(double k, int sprLaw, int stRepulsionLaw,
		    int ltRepulsionLaw, bool BH);
  SpringChargeModel(double k, int sprLaw, int repulsionLaw,
		     bool BH);
  SpringChargeModel(double k, int sprLaw, int stRepulsionLaw,
		     int ltRepulsionLaw);
  SpringChargeModel(double k, int sprLaw, int repulsionLaw);

  virtual ~SpringChargeModel();

  double * computeGradient(Graph & g, double * gradient, int & gradientLength);


  double _k, _kSquared, _kCubed, _twok;
  int    _springLaw;
  int    _shortRangeRepulsionLaw;
  int    _longRangeRepulsionLaw;
  bool   _BarnesHut;

private:

  void computeSprings(Graph & g, double * gradient);
  void computeRepulsion(Graph & g, double * gradient);
  void computeRepulsionNaively(Graph & g, double * gradient);
  void computeRepulsionWithGridVariant(Graph & g, double * gradient);
  void computeRepulsionWithBH(Graph & g, double * gradient);
  void computeQuadTreeRepulsion(QuadTree * QT1, QuadTree * QT2);
  bool wellSeparated(QuadTree * QT1, QuadTree * QT2);
  void computeQuadTreeForces(QuadTree * qt);

  Node **** _grid; // This looks really wierd, NOTE: try single deref w/ arthimetic
  int _gridX, _gridY, ** _size;
};

#endif
