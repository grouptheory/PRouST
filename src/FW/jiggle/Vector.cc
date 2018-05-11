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
static char const _Vector_cc_rcsid_[] =
"$Id: Vector.cc,v 1.1 1999/01/21 14:54:39 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "Vector.h"

extern "C" {
#include <strings.h>
};

Vector::Vector(void) 
  : _maxSize(0), _currentSize(0), _objects(0) { }

Vector::~Vector() 
{ 
  clear();
}

void Vector::addElement(Object * n)
{
  if (_currentSize == _maxSize)
    increaseSize();

  _objects [ _currentSize++ ] = n;
}

Object * Vector::remElement(Object * n)
{
  int i = 0, j = 0;
  while (_objects[i] != n)
    i++;

  j = i++;
  while (i < _currentSize)
    _objects[j] = _objects[i];

  _currentSize--;
  return n;
}

Object *& Vector::element(int index)
{
  static Object * rval = 0;

  if (index < _currentSize && index >= 0)
    return _objects[index];
  
  return rval;
}

Object *& Vector::operator [] (int index)
{
  return element(index);
}

int  Vector::size(void) 
{ return _currentSize; }

void Vector::clear(void) 
{ 
  _currentSize = _maxSize = 0; 
  delete [] _objects; _objects = 0;
}

void Vector::increaseSize(void)
{
  if (!_objects)
    _maxSize = 32;
  _maxSize *= 2;

  Object ** new_obj = new Object * [ _maxSize ];
  bzero(new_obj, sizeof(Object *) * _maxSize);

  for (int i = 0; i < _currentSize; i++)
    new_obj[i] = _objects[i];

  delete [] _objects;
  _objects = new_obj;
}
