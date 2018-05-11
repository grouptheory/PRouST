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
static char const _ctors_sim_cc_rcsid_[] =
"$Id: sim_ctors.cc,v 1.22 1999/02/26 14:43:30 mountcas Exp $";
#endif
#include <common/cprototypes.h>

// --------------------- PortDataVisitor ---------------------------
#include <sim/port/PortDataVisitor.h>
vistype PortDataVisitor::_mytype(PORT_DATA_VISITOR_NAME);

#include <sim/switch/PortInstallerVisitor.h>
// --------------------------- PortInstallerVisitor -------------------
vistype PortInstallerVisitor::_my_type(PORT_INSTALLER_VISITOR_NAME);

// --------------------- StrikeVisitor ---------------------------
#include <sim/switch/StrikeVisitor.h>
vistype StrikeVisitor::_mytype(STRIKE_VISITOR_NAME);

// --------------------- StrikeBreakerVisitor ---------------------------
#include <sim/switch/StrikeBreakerVisitor.h>
vistype StrikeBreakerVisitor::_mytype(STRIKE_BREAKER_VISITOR_NAME);

#include <sim/aggregator/LogosGraphVisitor.h>
vistype LogosGraphVisitor::_my_type(LOGOS_GRAPH_VISITOR_NAME);

#include <sim/switch/FateVisitor.h>
vistype FateVisitor::_my_type( FATE_VISITOR_NAME );

// ----------------------------------------------------------------------
extern bool fsm_ctors_inited;
extern bool init_ctors_fsm(void);
extern bool init_ctors_sim(void);

bool sim_ctors_inited = init_ctors_sim();

// ----------------------------------------------------------------------
bool init_ctors_sim(void)
{
  if (!fsm_ctors_inited)
    fsm_ctors_inited = init_ctors_fsm();

  if (sim_ctors_inited)
    return sim_ctors_inited;

  /*
  PortInstallerVisitor* global_PortInstallerVisitor = new PortInstallerVisitor(0,0.0,0);

  PortProtocolVisitor* global_PortProtocolVisitor = new PortProtocolVisitor(0,0,0,0);

  LinkUpVisitor* global_LinkUpVisitor = new LinkUpVisitor(0,0);
  LinkDownVisitor* global_LinkDownVisitor = new LinkDownVisitor(0,0);

  FastUNIVisitor* global_FastUNIVisitor = new FastUNIVisitor(0,0,0,0,0,0,0, FastUNIVisitor::FastUNISetup);
  PortDataVisitor* global_PortDataVisitor = new PortDataVisitor(0,0,0);

  StrikeVisitor* global_StrikeVisitor = new StrikeVisitor;
  StrikeBreakerVisitor* global_StrikeBreakerVisitor = new StrikeBreakerVisitor;
  */
  return true;
}


#include "sim_atexit.h"
#include "sim_atexit.cc"

#ifdef __ATEXIT_CLEANUP_H__
sim_atexitCleanup sim_clean_now;
#endif

