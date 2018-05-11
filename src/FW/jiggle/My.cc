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
static char const _My_cc_rcsid_[] =
"$Id: My.cc,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "My.h"
#include "CGwithRestarts.h"

const int My::maxInt = MAXINT;

My::My(void) { }

// MATH METHODS
double My::square(double x) { return x * x; }

double My::random(double lo, double hi)
{
  double rval = lo;

  srand(currentTimeMillis());
  if (lo != hi)
    rval = lo + ((hi - lo) * (rand() / maxInt));

  return rval;
}

int My::randomInt(int lo, int hi)
{
  int rval = lo;
  srand(currentTimeMillis());
  if (lo != hi)
    rval =  (rand() % (hi - lo)) + lo;
  return rval;
}

double My::angle(double x, double y)
{
  if ((x == 0) && (y == 0)) return 0;
  else if ((x == 0) && (y > 0)) return M_PI / 2;
  else if (x == 0) return - M_PI / 2;
  else if (x > 0) return atan(y / x); 
  else return atan(y / x) + M_PI;
}

// STRING METHODS
int My::stringToInt(const char * s)
{
  int rval = 0;
  sscanf(s, "%d", &rval);
  return rval;
}

double My::stringToDouble(const char * s)
{
  double rval = 0.0;
  sscanf(s, "%lf", &rval);
  return rval;
}
