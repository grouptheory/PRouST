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
static char const _ctors_FW_cc_rcsid_[] =
"$Id: FW_ctors.cc,v 1.1 1999/01/21 14:54:11 bilal Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/Visitor.h>
#include <FW/basics/VisitorType.h>
#include <FW/kernel/Kernel.h>

// ------------------- DisconnectVisitor ---------------------
vistype DisconnectVisitor::_my_type(DISCONNECT_VISITOR_TYPE);

// ------------------- ClusterKillerVisitor -----------------
vistype ClusterKillerVisitor::_my_type(CLUSTERKILLER_VISITOR_TYPE);

// ------------------- Visitor -----------------
vistype Visitor::_my_type(VISITOR_TYPE);

bool init_ctors_FW(void);

bool   FW_ctors_inited  = init_ctors_FW();
int    MEMORY_DEBUGGING = 0;
double DELAY_THRESHOLD  = 0.0;

bool init_ctors_FW(void)
{
  if (FW_ctors_inited) 
    return FW_ctors_inited;

  Kernel::_kernel_singleton = new Kernel();

  char * flg = getenv("MEMORY_DEBUGGING");
  if (flg) {
    if (sscanf(flg,"%d", &MEMORY_DEBUGGING) == 0) {
      cout << "Legal values for the environment variable MEMORY_DEBUGGING are:\n"
	   << "KERNEL_INVARIANTS 1\n"
	   << "TIMELINE_ORDER    2\n"
	   << "DELAY_WARNINGS    4    --   and DELAY_THRESHOLD contains the max timer callback delay\n"
	   << "VERBOSE_DEBUG     8\n"
	   << "Exiting... " << endl;
      // This exit is okay, don't freak
      exit(1);
    }
  } else 
    MEMORY_DEBUGGING = 0;

  if ( MEMORY_DEBUGGING != 0 ) {
    cout << "MEMORY_DEBUGGING is at level " << MEMORY_DEBUGGING << endl;
  }

  char * flg2 = getenv("DELAY_THRESHOLD");
  if (flg2) DELAY_THRESHOLD = atof(flg2);
  else DELAY_THRESHOLD = 0.0;

  return true;
}


// ------------------- Memory Clean-Up -----------------------
#include "FW_atexit.h"
#include "FW_atexit.cc"

#ifdef __ATEXIT_CLEANUP_H__
FW_atexitCleanup FW_clean_now;  // This calls DeleteFW() on exit
#endif

