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
static char const _ArrivalRateInvestigator_cc_rcsid_[] =
"$Id: ArrivalRateInvestigator.cc,v 1.10 1999/02/26 19:48:12 mountcas Exp $";
#endif

#include "ArrivalRateInvestigator.h"
#include <FW/basics/diag.h>
#include <FW/kernel/Kernel.h>
#include <fsm/netstats/StatRecord.h>
#include <fsm/omni/OmniProxy.h>
#include <fsm/visitors/LinkVisitor.h>

ArrivalRateInvestigator::ArrivalRateInvestigator( const char * filename, double wakeUpInterval,
						  bool (*func_ptr)(double, double) ) 
  : Investigator( filename, wakeUpInterval, true ), _interval( wakeUpInterval ),
    _duration( 32 * wakeUpInterval ), _bw( OC3 / 2.0 - 12500 ), _func_ptr( func_ptr ),
    _counter( 0 )
{ 
  if ( _func_ptr == 0 ) {
    diag("sim.investigators", DIAG_FATAL, "If you wish to use ArrivalRateInvestigators "
	 "you MUST specify a funct to be called upon Global_Synchrony, otherwise use "
	 "GSynchronyInvestigators!\n");
  }
  assert( _func_ptr != 0 );
}

ArrivalRateInvestigator::~ArrivalRateInvestigator() { }

void ArrivalRateInvestigator::makeObservation(char * const notebook_entry, bool & quit_now)
{
  quit_now = false;

  list< const StatRecord * > * events =
    obtainRecentEvents( 5, getIntensity(), "Global_Synchrony", "*" );
  // Wait until first GSync to start launching setups
  if ( events != 0 && events->empty() == false ) {
    changeIntensity( _interval );
    
    char buf[128];
    sprintf( buf, "# ARI: BATCH SVC INTERVAL=%lf", _interval );
    theOmniProxy().notify( buf );
    bool loop = (*_func_ptr)( _duration, _bw );
    
    if ( loop ) {
      _duration /= 2.0;
      _interval /= 2.0;
      // sleep for the old duration to give old calls a chance to clear
      changeIntensity( _duration * 2.0 );
      _counter++;
    }
  }
  if ( _counter > 8 )
    quit_now = true;
}
