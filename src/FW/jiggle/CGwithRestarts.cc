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
static char const _CGwithRestarts_cc_rcsid_[] =
"$Id: CGwithRestarts.cc,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "CGwithRestarts.h"
#include "Graph.h"
#include "Model.h"
#include "Node.h"

extern "C" {
#include <math.h>
#include <sys/time.h>
};

CGwithRestarts::CGwithRestarts(double initialStepSize)
  : stepSize(initialStepSize), initialStepSize(initialStepSize),
    gradient(0), previousGradient(0), tmp(0), searchDir(0),
    previousSearchDir(0), gradient_length(0), 
    previousGradient_length(0), tmp_length(0), 
    searchDir_length(0), previousSearchDir_length(0) { }

CGwithRestarts::~CGwithRestarts() 
{ 
  delete [] gradient;
  delete [] previousGradient;
  delete [] searchDir;
  delete [] previousSearchDir;
}

double CGwithRestarts::improve(Graph & g, Model & m)
{
  long startTime = currentTimeMillis();
  int n = g.numberOfNodes;
  if (resetFlag || (!previousGradient) || (previousGradient_length != 2 * n)) {
    resetFlag = false;
    previousGradient = m.computeGradient(g, previousGradient, previousGradient_length);
    if ((!previousSearchDir) || (previousSearchDir_length != 2 * n)) {
      if (previousSearchDir) delete [] previousSearchDir;
      if (searchDir) delete [] searchDir;
      previousSearchDir = new double [2 * n]; previousSearchDir_length = (2 * n);
      searchDir = new double [2 * n]; searchDir_length = (2 * n);
    }
    for (int i = 0; i < 2 * n; i++) 
      previousSearchDir[i] = previousGradient[i];
    stepSize = initialStepSize;
  }
  double mag1 = magnitude(previousGradient, 2 * n);
  if (mag1 / n < epsilon) return 0;
  step(g, stepSize, n); gradient = m.computeGradient(g, gradient, gradient_length);
  double mag2 = magnitude(gradient, 2 * n), dp = dotProduct(n);
  double lo = 0, hi = stepSize;
  while ((! resetFlag) && (dp / (mag1 * mag2) > maxCos)) {
    step(g, hi, n); lo = hi; hi *= 2; stepSize = hi;
    gradient = m.computeGradient(g, gradient, gradient_length);
    mag2 = magnitude(gradient, 2 * n); dp = dotProduct(n);
  }
  int iter = 0, i = 0;
  while ((! resetFlag) && ((dp < 0) || (dp / (mag1 * mag2) > maxCos))) {
    if (iter++ > MAX_ITER) {
      step(g, lo - stepSize, n);
      gradient = m.computeGradient(g, gradient, gradient_length);
      return magnitude(gradient, 2 * n);
    }
    step(g, (lo + hi) / 2 - stepSize, n);
    stepSize = (lo + hi) / 2;
    gradient = m.computeGradient(g, gradient, gradient_length);
    mag2 = magnitude(gradient, 2 * n); dp = dotProduct(n);
    if (dp < 0) 
      hi = stepSize; 
    else 
      lo = stepSize;
  }
  ++numberOfIterations;
  double w = magnitudeSquared(gradient, 2 * n) /
    magnitudeSquared (previousGradient, 2 * n);
  for (i = 0; i < 2 * n; i++)
    searchDir[i] = gradient[i] + w * previousSearchDir[i];
  double sum = 0;
  for (i = 0; i < 2 * n; i++) 
    sum += searchDir[i] * gradient[i];
  sum /= (magnitude (searchDir, 2 * n) * magnitude (gradient, 2 * n));
  if (sum < 0.5) {
    reset(); 
    time += currentTimeMillis() - startTime; 
    return mag2;
  }

  tmp = previousGradient; tmp_length = previousGradient_length;
  previousGradient = gradient; previousGradient_length = gradient_length;
  gradient = tmp; gradient_length = tmp_length;
  tmp = previousSearchDir; tmp_length = previousSearchDir_length;
  previousSearchDir = searchDir; previousSearchDir_length = searchDir_length;
  searchDir = tmp; searchDir_length = tmp_length;

  time += currentTimeMillis() - startTime;
  return mag2;
}

void CGwithRestarts::step(Graph & g, double d, int n)
{
  for (int i = 0; i < n; i++) {
    if (g.nodes(i)->fixed == false) {
      g.nodes(i)->x( g.nodes(i)->x() + d * previousSearchDir[2 * i]);
      g.nodes(i)->y( g.nodes(i)->y() + d * previousSearchDir[2 * i + 1]);
    }
  }
}

double CGwithRestarts::magnitudeSquared(double * vector, int n)
{
  double sum = 0;
  for (int i = 0; i < n; i++) 
    sum += vector[i] * vector[i];
  return sum;
}

double CGwithRestarts::magnitude(double * vector, int n)
{
  return sqrt(magnitudeSquared (vector, n));
}

double CGwithRestarts::dotProduct(int n)
{
  double sum = 0;
  for (int i = 0; i < 2 * n; i++)
    sum += previousSearchDir[i] * gradient[i];
  return sum;
}

// This should correspond to Java's System.currentTimeMillis()
long currentTimeMillis(void)
{
  struct timeval tp;
  gettimeofday(&tp, 0);

  return ((tp.tv_sec / 1000) + (tp.tv_usec * 1000));
}
