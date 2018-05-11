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
#ifndef __CPROTOS__
#define __CPROTOS__

//*************************************************
//
//               D I S C L A I M E R 
//
//   Inclusion of an OS prototypes files does NOT
//   imply support, merely a passing interest.
//
//*************************************************


#ifdef __SunOS__
#include <common/cproto_SunOS.h>
#endif // __SunOS__

#ifdef __SunOS5__
#include <common/cproto_SunOS5.h>
#endif // __SunOS5__

#ifdef __IRIX64__
#include <common/cproto_IRIX64.h>
#endif // __IRIX64__

#ifdef __Linux__
#include <common/cproto_Linux.h>
#endif

#ifdef __NetBSD__
#include <common/cproto_NetBSD.h>
#endif

#ifdef __ProuST_OS__
#include <common/cproto_ProuST_OS.h>
#endif

#endif // __CPROTOS__
