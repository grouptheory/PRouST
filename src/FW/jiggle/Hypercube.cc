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
static char const _Hypercube_cc_rcsid_[] =
"$Id: Hypercube.cc,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif
#include <common/cprototypes.h>

extern "C" {
#include <math.h>
};

#include "Hypercube.h"
#include "Node.h"
#include "Edge.h"

Hypercube::Hypercube(int n, bool dir) 
  : Graph()
{ 
  int twoToTheN = (int)pow(2, n), i, j;
  Node ** N = new Node * [ twoToTheN ];
  for (i = 0; i < twoToTheN; i++)
    insertNode(N[i] = new Node());
  for (i = 0; i < twoToTheN; i++) {
    for (j = 1; j < twoToTheN; j *= 2) {
      if ((i & j) == 0) 
	insertEdge(new Edge(N[i], N[i + j], dir));
    }
  }
  delete [] N;
}

Hypercube::~Hypercube() { }
