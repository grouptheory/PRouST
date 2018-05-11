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
static char const _KernelTime_cc_rcsid_[] =
"$Id: KernelTime.cc,v 1.1 1999/01/13 19:12:37 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/kernel/KernelTime.h>
#include <iomanip.h>

extern "C" {
#include <sys/time.h>
#include <math.h>
#include <malloc.h>
};

KernelTime operator + (KernelTime x, KernelTime y)
{
  return KernelTime(x._time + y._time);
}

KernelTime operator - (KernelTime x, KernelTime y)
{
  if (x._time < y._time) 
    return KernelTime(0.0);
  else 
    return KernelTime(x._time - y._time);
}

KernelTime operator * (KernelTime x, double sc)
{
  return KernelTime(x._time * sc);
}

KernelTime operator * (double sc, KernelTime x)
{
  return KernelTime(x._time * sc);
}

ostream & operator << (ostream & os, const KernelTime & kt)
{
  os << setiosflags(ios::fixed) << setprecision(5) << kt._time;
  return os;
}

bool operator < (KernelTime x, KernelTime y)
{
  int ans = compare(x,y);
  return (ans==-1);
}

bool operator <= (KernelTime x, KernelTime y)
{
  int ans = compare(x,y);
  return ((ans==0)||(ans==-1));
}

bool operator > (KernelTime x, KernelTime y)
{
  int ans = compare(x,y);
  return (ans==1);
}

bool operator >= (KernelTime x, KernelTime y)
{
  int ans = compare(x,y);
  return ((ans==0)||(ans==1));
}

bool operator == (KernelTime x, KernelTime y)
{
  int ans = compare(x,y);
  return (ans==0);
}

bool operator != (KernelTime x, KernelTime y)
{
  int ans = compare(x,y);
  return ((ans==-1)||(ans==1));
}

KernelTime::KernelTime(double t) : _time(t) { assert(t >= 0.0); }

KernelTime::~KernelTime() { }

struct timeval * KernelTime::timeval(void)
{
  struct timeval * tsp = (struct timeval*) malloc(sizeof (struct timeval));
  tsp->tv_sec  = (long)(floor(_time));
  tsp->tv_usec = (long)((_time - (double)(tsp->tv_sec))*1000000.0);
  return tsp;
}

KernelTime::operator double () { return _time; }
 
