// -*- C++ -*-
// +++++++++++++++
//   P R o u S T     ---  PNNI Routing Simulation Toolkit  ---
// +++++++++++++++
// Version: 1.0 (Fri Mar  5 14:35:33 EST 1999)
// 
// 			  Copyright (c) 1998
// 		 Naval Research Laboratory (NRL/CCS)
// 			       and the
// 	  Defense Advanced Research Projects Agency (DARPA)
// 
// 			 All Rights Reserved.
// 
// Permission to use, copy, and modify this software and its
// documentation is hereby granted, provided that both the copyright notice and
// this permission notice appear in all copies of the software, derivative
// works or modified versions, and any portions thereof, and that both notices
// appear in supporting documentation.
// 
// NRL AND DARPA ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
// DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM
// THE USE OF THIS SOFTWARE.
// 
// NRL and DARPA request users of this software to return modifications,
// improvements or extensions that they make to:
// 
//                 proust-dev@cmf.nrl.navy.mil
//                         -or-
//                Naval Research Laboratory, Code 5590
//                Center for Computation Science
//                Washington, D.C.  20375
// 
// and grant NRL and DARPA the rights to redistribute these changes in
// future upgrades.
//

// -*- C++ -*-
#ifndef LINT
static char const _String_cc_rcsid_[] =
"$Id: String.cc,v 1.9 1999/02/19 21:22:59 marsh Exp $";
#endif

#include "String.h"
#include <common/cprototypes.h>
#include <fstream.h>
#include <iostream.h>

ds_String::ds_String() 
  : _size(1024), _occupied(1) 
{
  _buf = new char [_size];
  memset(_buf, 0, _size);
}

ds_String::ds_String(const char *str) : _size(0), _occupied(0), _buf(0)
{
  (*this)+= str;
}


ds_String::ds_String(const ds_String &him) : _size(him._size), _occupied(him._occupied)
{
  _buf = new char[ him._size ];
  memset(_buf, 0, him._size);	// Can't hurt!  Makes us slower.  Oh, well.
  memcpy((void *)_buf, (void *)him._buf, (size_t)him._size);
}


ds_String::~ds_String() 
{
  delete [] _buf;
  _buf = 0;
}

ds_String & ds_String::operator += (const char * str) 
{
  int to_be_added = strlen(str);
  int remaining = _size - (_occupied + to_be_added);
  if (remaining < 0)
    grow( 2* (_occupied + to_be_added) );
  strcat(_buf, str);
  _occupied = 1 + strlen(_buf);
  return *this;
}

ds_String & ds_String::operator += (char c) 
{
  int to_be_added = 1;
  int remaining = _size - (_occupied + to_be_added);
  if (remaining < 0)
    grow( 2* (_occupied + to_be_added) );
  _buf[_occupied - 1] = c;
  _buf[_occupied] = 0;

  _occupied = 1 + strlen(_buf);
  return *this;
}

ds_String & ds_String::operator += (const ds_String &str) 
{
  int to_be_added = str._size;
  int remaining = _size - (_occupied + to_be_added);
  if (remaining < 0)
    grow( 2* (_occupied + to_be_added) );
  strcat(_buf, str._buf);
  _occupied = 1 + strlen(_buf);
  return *this;
}

void ds_String::grow( int size ) 
{
  // What's the point of growing otherwise?
  assert( size > _size );

  char * newbuf = new char [ size ];
  _size = size;
  memset(newbuf, 0, size);

  if (_buf != 0) {
    strcpy(newbuf, _buf);
    delete [] _buf;
  }

  _buf = newbuf;
}

const char * ds_String::chars(void) 
{
  return _buf;
}


int ds_String::size(void) const
{
  return _occupied - 1;
}


ostream & operator << (ostream & os, const ds_String & him)
{
  int i;
  for (i = 0; i < him._occupied - 1; i++)
    os << him._buf[i];

  return os;
}

