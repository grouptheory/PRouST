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
static char const _GSynchronyInvestigator_cc_rcsid_[] =
"$Id: GSynchronyInvestigator.cc,v 1.5 1999/02/24 15:23:44 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "GSynchronyInvestigator.h"
#include <fsm/netstats/StatRecord.h>
#include <FW/basics/diag.h>
#include <FW/kernel/Kernel.h>

GSynchronyInvestigator::GSynchronyInvestigator(const char * filename, int time, bool die) 
  : Investigator(filename, time), _lastTime(0.0), _dieOnFirstGS(die), _seenGS( false )
{ 
}

GSynchronyInvestigator::~GSynchronyInvestigator() 
{ 
  if ( ! _seenGS ) {
    char str[80];
    sprintf(str, "Simulation has NOT reached Global Synchrony by time %lf!\n",
	     (double)(theKernel().CurrentElapsedTime()) );
    finalObservation( str );
  }
}

void GSynchronyInvestigator::makeObservation(char * const notebook_entry, bool & quit_now)
{
  quit_now = false;
  double t = (double)(theKernel().CurrentElapsedTime()) - _lastTime + 1.0;

  list< const StatRecord * > * events =
    obtainRecentEvents( t, getIntensity(), "Global_Synchrony", "*" );

  if ( events != 0 && events->empty() == false ) {
    const StatRecord * sr = events->head();
    int   netsize = -1;
    sscanf( sr->getMisc(), "n%d", &netsize );

    sprintf( notebook_entry, "%s %lf, traffic=%lf (%lf, %lf, %lf), netsize=%d\n", 
	     sr->getType(), sr->getTime(), trafficSize(), 
	     trafficSize(0, NetStatsCollector::Hello ),
	     trafficSize(0, NetStatsCollector::NodePeer ),
	     trafficSize(0, NetStatsCollector::UNI ),
	     netsize );
    diag( "investigator", DIAG_INFO, "%s", notebook_entry );
    if ( _dieOnFirstGS )
      quit_now = true;
  }

  _lastTime = theKernel().CurrentElapsedTime();
}
