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
static char const _DynamicArray_cc_rcsid_[] =
"$Id: DynamicArray.cc,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "DynamicArray.h"

Node   ** DynamicArray::add(Node   ** arr, int size, Node * elem)
{
  Node ** tmp_arr = new Node * [ size + 1 ];

  for (int i = 0; i < size; i++)
    tmp_arr[i] = arr[i];
  tmp_arr[ size ] = elem;

  return tmp_arr;
}

Edge   ** DynamicArray::add(Edge   ** arr, int size, Edge * elem)
{
  Edge ** tmp_arr = new Edge * [ size + 1 ];

  for (int i = 0; i < size; i++)
    tmp_arr[i] = arr[i];
  tmp_arr[ size ] = elem;

  return tmp_arr;
}

double ** DynamicArray::add(double ** arr, int size, double * elem)
{
  double ** tmp_arr = new double * [ size + 1 ];
  
  for (int i = 0; i < size; i++)
    tmp_arr[i] = arr[i];
  tmp_arr[ size ] = elem;

  return tmp_arr;
}

void DynamicArray::remove(Node ** arr, int size, Node * elem)
{
  int i = 0;
  while (arr[i] != elem)
    i++;

  while (i + 1 < size) {
    arr[i] = arr[i + 1];
    i++;
  }

  arr [ size - 1 ] = 0;
}

void DynamicArray::remove(Edge ** arr, int size, Edge * elem)
{
  int i = 0;
  while (arr[i] != elem)
    i++;

  while (i + 1 < size) {
    arr[i] = arr[i + 1];
    i++;
  }

  arr [ size - 1 ] = 0;
}

void DynamicArray::remove(double ** arr, int size, double * elem)
{
  int i = 0;
  while (arr[i] != elem)
    i++;

  while (i + 1 < size)
    arr[i] = arr[i + 1];

  arr [ size - 1 ] = 0;
}
