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
static char const _ExperimentalInvestigator_cc_rcsid_[] =
"$Id: ExperimentalInvestigator.cc,v 1.21 1999/02/25 19:50:23 mountcas Exp $";
#endif

#include "ExperimentalInvestigator.h"
#include <FW/basics/diag.h>
#include <FW/kernel/Kernel.h>
#include <fsm/netstats/StatRecord.h>
#include <fsm/omni/OmniProxy.h>

ExperimentalInvestigator::ExperimentalInvestigator( const char * filename, 
						    double initialTime,
						    void (*func_ptr)(void),
						    double finalTime, 
						    int maxSetups ) 
  : Investigator(filename, initialTime, true), _lastTime( 0.0 ), _finalTime( finalTime ),
    _seenGS( false ), _maxSetups( maxSetups ), _seenSetups( 0 ), _func_ptr( func_ptr ) 
{ 
  if ( _func_ptr == 0 ) {
    diag("sim.investigators", DIAG_FATAL, "If you wish to use ExperimentalInvestigators "
	 "you MUST specify a funct to be called upon Global_Synchrony, otherwise use "
	 "GSynchronyInvestigators!\n");
  }
  assert( _func_ptr != 0 );
}

ExperimentalInvestigator::~ExperimentalInvestigator() 
{ 
  if ( ! _seenGS ) {
    char str[80];
    sprintf(str, "Simulation has NOT reached Global Synchrony by time %lf!\n",
	     (double)(theKernel().CurrentElapsedTime()) );
    finalObservation( str );
  }
}

void ExperimentalInvestigator::makeObservation(char * const notebook_entry, bool & quit_now)
{
  quit_now = false;

  double time = theKernel().CurrentElapsedTime();
  double t    = time - _lastTime + 1.0;

  list< const StatRecord * > * events =
    obtainRecentEvents( t, getIntensity(), "Global_Synchrony", "*" );

  if ( events != 0 && events->empty() == false ) {
    // Debugging
    if ( events->size() > 1 )
      cout << "WARNING:  events contains " << events->size() << " Global_Synchronies!!" << endl;

    const StatRecord * sr = events->head();
    
    if ( sr->getTime() < events->tail()->getTime() )
      sr = events->tail();

    // _lastTime corresponds to the call submission time, and MUST be strictly
    // less than the Global_Synchrony time.
    if ( _lastTime < sr->getTime() ) {
      int   netsize = -1;
      sscanf( sr->getMisc(), "n%d", &netsize );
      
      sprintf( notebook_entry, "%s %lf, traffic=%lf (%lf, %lf, %lf), netsize=%d", 
	       sr->getType(), sr->getTime(), trafficSize(), 
	       trafficSize(0, NetStatsCollector::Hello ),
	       trafficSize(0, NetStatsCollector::NodePeer ),
	       trafficSize(0, NetStatsCollector::UNI ),
	       netsize );
      diag( "investigator", DIAG_INFO, "%s\n", notebook_entry );
      
      // Get this stuff in the OMNI file as comments ...
      char buf[512];
      sprintf( buf, "# ExperimentalInvestigator: %s", notebook_entry );
      theOmniProxy().notify( buf );
      
      if ( ! _seenGS && _finalTime > 0.0 )
	changeIntensity( _finalTime );
      
      _seenGS = true;
      
      if ( _maxSetups > _seenSetups ) {
	
	sprintf( buf, "# ExperimentalInvestigator: Launching_Setup %lf, "
		 "traffic=%lf (%lf, %lf, %lf), netsize=%d",
		 time, theNetStatsCollector().TrafficSize(), 
		 theNetStatsCollector().TrafficSize( NetStatsCollector::Hello ),
		 theNetStatsCollector().TrafficSize( NetStatsCollector::NodePeer ),
		 theNetStatsCollector().TrafficSize( NetStatsCollector::UNI ),
		 netsize );
	theOmniProxy().notify( buf );
	// Call the functions
	(*_func_ptr)( );
	
	_seenSetups += 1;
	
      } else
	quit_now = true;
      
      strcat( notebook_entry, "\n" );
    }
  }
  _lastTime = theKernel().CurrentElapsedTime();
}

