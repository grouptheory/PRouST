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

#ifndef LINT
static char const _Buffer_cc_rcsid_[] =
"$Id: Buffer.cc,v 1.3 1998/08/06 04:02:57 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include "Buffer.h"

Buffer::Buffer(int length) {
  _storage_begin = new unsigned char [length + _max_trailer + _max_header];
  _storage_end = _storage_begin + length + _max_trailer + _max_header - 1;
  _current = _storage_begin + _max_header;
  _length = length;
}

Buffer::~Buffer() {
}
  
unsigned char* Buffer::data(void) const {
  return _current;
}

bool Buffer::GrowHeader(int bytes) {
  if (_current-bytes<_storage_begin) return false;
  _current-=bytes;
  _length+=bytes;
  return true;
}

bool Buffer::GrowTrailer(int bytes) {
  if (_current+_length+bytes>_storage_end) return false;
  _length+=bytes;
  return true;
}

bool Buffer::ShrinkHeader(int bytes) {
  if (_current+bytes>_storage_end) return false;
  _current+=bytes;
  _length-=bytes;
  return true;
}

bool Buffer::ShrinkTrailer(int bytes) {
  if (_current+_length-bytes<_storage_begin) return false;
  _length-=bytes;
  return true;
}

int Buffer::length(void) const {
  return _length;
}

bool Buffer::set_length(int newlength) {
  if (_current+newlength>_storage_end) return false;
  _length=newlength;
  return true;
}
