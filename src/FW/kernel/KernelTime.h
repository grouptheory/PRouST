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
#ifndef __KERNELTIME_H__
#define __KERNELTIME_H__
#if !defined(LINT)
static char const _KernelTime_h_rcsid_[] =
"$Id: KernelTime.h,v 1.1 1999/01/13 19:12:37 mountcas Exp $";
#endif

#include <iostream.h>

extern "C" {
#include <sys/time.h>
#include <assert.h>
};

class KernelTime {
  friend KernelTime operator + (KernelTime x, KernelTime y);
  friend KernelTime operator - (KernelTime x, KernelTime y);
  friend KernelTime operator * (KernelTime x, double sc);
  friend KernelTime operator * (double sc, KernelTime x);

  friend int compare(const KernelTime &, const KernelTime &);
  friend ostream & operator << (ostream & os, const KernelTime &);

  friend bool operator <  (KernelTime x, KernelTime y);
  friend bool operator <= (KernelTime x, KernelTime y);
  friend bool operator >  (KernelTime x, KernelTime y);
  friend bool operator >= (KernelTime x, KernelTime y);
  friend bool operator == (KernelTime x, KernelTime y);
  friend bool operator != (KernelTime x, KernelTime y);
public:

  KernelTime(double t = 0.0);
  ~KernelTime();

  struct timeval * timeval(void);

  operator double ();

private:

  double _time;
};

#endif // __KERNELTIME_H__
