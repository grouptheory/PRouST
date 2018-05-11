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
static char const _cache_cc_rcsid_[] =
"$Id: cache.cc,v 1.4 1997/09/26 14:04:52 mountcas Exp $";
#endif

#include <common/cprototypes.h>

extern "C" {
#include <stdlib.h>
};

#include <codec/uni_ie/cache.h>


Cache::Cache(void) : _status(0), _cache(0L), _length(0)
{
  // No further initialization needed
}

Cache::~Cache(void) 
{
  if (_cache != 0L) {
    delete [] _cache;
    _cache = 0L;
  }
}

u_char * Cache::LookupCache(void)
{
  return ((_status == 1) ? _cache : 0L);
}

void Cache::MakeStale(void)
{
  _status = 0;
}

int Cache::CacheLength(void) const
{
  return _length;
}

void Cache::FillCache(u_char *buf, int len)
{
  if (IsVariableLen() && _cache != 0L) {
    delete [] _cache;
    _cache = 0L;
  }
  // check to see if this is the first time we're caching it 
  //  or in the case of a variable length cache
  if (_cache == 0L)
    _cache = new u_char [len];

  bcopy((char *)buf, (char *)_cache, len);

  _status = 1;
  _length = len;
}


char Cache::PrintSpecific(ostream & os)
{
  os << endl;
  if (_status) {
    os << "cache::";
    for (int i = 0; i < _length; i++)
      os << _cache[i];
  } else
    os << "Cache is empty.";
  os << endl;
  return 0;
}

ostream & operator << (ostream & os, Cache & x)
{
  x.PrintSpecific(os);
  return (os);
}
















