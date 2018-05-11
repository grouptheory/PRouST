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
static char const _SteepestDescent_cc_rcsid_[] =
"$Id: SteepestDescent.cc,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "SteepestDescent.h"
#include "CGwithRestarts.h"
#include "Graph.h"
#include "Model.h"
#include "Node.h"

SteepestDescent::SteepestDescent(double initialStepSize)
  : _initialStepSize(initialStepSize), _stepSize(0),
    _gradient(0), _previousGradient(0), _tmp(0),
    _gradientLength(0), _previousGradientLength(0), _tmpLength(0)
{ }

SteepestDescent::~SteepestDescent() 
{ 
  delete [] _gradient; 
  delete [] _previousGradient;
}

double SteepestDescent::improve(Graph & g, Model & m)
{
  long startTime = currentTimeMillis();
  int n = g.numberOfNodes;
  if (resetFlag || (_previousGradient == 0) || (_previousGradientLength != 2 * n)) {
    resetFlag = false;
    _previousGradient = m.computeGradient(g, _previousGradient, _previousGradientLength);
    _stepSize = _initialStepSize;
  }
  double mag1 = magnitude(_previousGradient, 2 * n);
  if (mag1 / n < epsilon) return 0;
  step(g, _stepSize, n);
  _gradient = m.computeGradient(g, _gradient, _gradientLength);
  double mag2 = magnitude(_gradient, 2 * n), dp = dotProduct(n);
  double lo = 0, hi = _stepSize;
  while ((! resetFlag) && (dp / (mag1 * mag2) > Optimizer::maxCos)) {
    step(g, hi, n); lo = hi; hi *= 2; _stepSize = hi;
    _gradient = m.computeGradient(g, _gradient, _gradientLength);
    mag2 = magnitude(_gradient, 2 * n); dp = dotProduct (n);
  }
  int iter = 0;
  while ((! resetFlag) && ((dp < 0) || (dp / (mag1 * mag2) > Optimizer::maxCos))) {
    if (iter++ > MAX_ITER) {
      step(g, lo - _stepSize, n);
      _gradient = m.computeGradient(g, _gradient, _gradientLength);
      return magnitude(_gradient, 2 * n);
    }
    step(g, (lo + hi) / 2 - _stepSize, n);
    _stepSize = (lo + hi) / 2;
    _gradient = m.computeGradient (g, _gradient, _gradientLength);
    mag2 = magnitude(_gradient, 2 * n); dp = dotProduct(n);
    if (dp < 0) hi = _stepSize; else lo = _stepSize;
  }
  ++numberOfIterations;
  _tmp = _previousGradient; _tmpLength = _previousGradientLength;
  _previousGradient = _gradient; _previousGradientLength = _gradientLength;
  _gradient = _tmp; _gradientLength = _tmpLength;
  time += currentTimeMillis() - startTime;
  return mag2;
}

void   SteepestDescent::step(Graph & g, double d, int n)
{
  for (int i = 0; i < n; i++) {
    if (g.nodes(i)->fixed == false) {
      g.nodes(i)->x( g.nodes(i)->x() + d * _previousGradient[2 * i]);
      g.nodes(i)->y( g.nodes(i)->y() + d * _previousGradient[2 * i + 1]);
    }
  }
}

double SteepestDescent::magnitude(double * vector, int n)
{
  double sum = 0;
  for (int i = 0; i < n; i++) 
    sum += vector[i] * vector[i];
  return sqrt(sum);
}

double SteepestDescent::dotProduct(int n)
{
  double sum = 0;
  for (int i = 0; i < 2 * n; i++)
    sum += _previousGradient[i] * _gradient[i];
  return sum;
}
