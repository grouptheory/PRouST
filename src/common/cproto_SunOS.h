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

//-*- C++ -*-
#ifndef __CPROTOS_SUNOS__
#define __CPROTOS_SUNOS__

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/filio.h>
#include <ctype.h>
#include <assert.h>
#include <values.h>
#include <memory.h>

  char * sprintf(char * s, const char * format, ...);
  void bzero(void *, int);
  void bcopy(const void *, void *, int);
  int  unlink(const char *);
  int  getpid(void);
  int  gettimeofday(struct timeval *, struct timezone *);
  int  select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
  int  strcasecmp(const char *, const char *);
  int  getpagesize(void);
  int  getrusage(int, struct rusage *);
  void abort(void);

  int socket(int domain, int type, int protocol);
  int ioctl(int fd, int request, caddr_t arg);
  int bind(int s, struct sockaddr * name, int namelen);
  int listen(int s, int backlog);
  int accept(int s, struct sockaddr *addr, int *addrlen);
  int send(int s, const char * msg, int len, int flags);
  int recv(int s, char * buf, int len, int flags);
  int connect(int s, struct sockaddr * name, int namelen);

  int setitimer(int which, struct itimerval *value, struct itimerval *ovalue);
};

#endif
