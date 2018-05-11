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
static char const _UsageEvent_cc_rcsid_[] =
"$Id: UsageEvent.cc,v 1.23 1998/09/23 03:15:10 bilal Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/Conduit.h>
#include <fsm/events/UsageEvent.h>

extern "C" {
#include <unistd.h>
#include <sys/resource.h>
#ifndef __Linux__
  int getpagesize(void);  /* Linux has this in unistd.h */
#endif
};

void UsageEvent::Callback(void)
{
  struct rusage rus;
  getrusage(RUSAGE_SELF, &rus);
  int pages  = rus.ru_maxrss;

  if ((pages != _prev_pages) ) { // || (nptses != _prev_ptses)) {
    char buf[180];

#ifdef PRINT_NICELY
    unsigned long kbytes = (pages * getpagesize()) / 1000;
    // Ignore this quick hack for readability
    buf[0] = '\0';
    while (kbytes > 0) {
      char tmp[180];
      unsigned long n = kbytes % 1000;
      sprintf(tmp, "%s%d%s%s", (n < 100 && (bytes / 1000) ? (n < 10 ? "00" : "0") : ""), (int)n, 
	      (*buf ? "," : ""), buf);
      kbytes = kbytes / 1000;
      strcpy(buf, tmp);
    }
#else
    sprintf(buf, "%.2lf", (double)((pages * getpagesize())/1000));
#endif
    // Resume reading
    cout << "Usage at time " << _my_time << " is " << pages 
	 << " pages (" << buf << " kb)." << endl;

    _prev_pages = pages;
  }
  _my_time += _t;
  ExpiresIn(_t);
  Register((TimerHandler *)this);
}

void PrintUsage(ostream & os, bool clean)
{
  struct rusage rus;
  getrusage(RUSAGE_SELF, &rus);
  int pages  = rus.ru_maxrss;

  char buf[180];

#ifdef PRINT_NICELY
  unsigned long kbytes = (pages * getpagesize()) / 1000;
  // Ignore this quick hack for readability
  buf[0] = '\0';
  while (kbytes > 0) {
    char tmp[180];
    unsigned long n = kbytes % 1000;
    sprintf(tmp, "%s%d%s%s", (n < 100 && (bytes / 1000) ? (n < 10 ? "00" : "0") : ""), (int)n, 
	    (*buf ? "," : ""), buf);
    kbytes = kbytes / 1000;
    strcpy(buf, tmp);
  }
#else
  sprintf(buf, "%.2lf", (double)((pages * getpagesize())/1000));
#endif
  // Resume reading
  os << "Usage is " << pages << " pages (" << buf << " kb)." << endl;
}
