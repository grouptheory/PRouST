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
static char const _TrafficInvestigator_cc_rcsid_[] =
"$Id: TrafficInvestigator.cc,v 1.2 1999/02/24 15:23:33 mountcas Exp $";
#endif

#include "TrafficInvestigator.h"
#include <FW/basics/diag.h>
#include <FW/kernel/Kernel.h>

TrafficInvestigator::TrafficInvestigator(const char * notebook_name, 
					 double investigation_intensity,
					 int smoothing_factor) 
  : Investigator( notebook_name, investigation_intensity, true ),
    _smoothing_factor( smoothing_factor ) { }

TrafficInvestigator::~TrafficInvestigator() 
{ 
  char str[128];
  sprintf( str, "%lf TotalTraffic %lf\n", 
	   theKernel().CurrentElapsedTime(),
	   trafficSize( 0, NetStatsCollector::Unknown ) );
  finalObservation( str );

  dic_item di;
  forall_items(di, _traffic) 
    delete _traffic.inf( di );
  _traffic.clear();
  _lastTraffic.clear();
}

void TrafficInvestigator::makeObservation(char * const notebook_entry, bool & quit_now)
{
  double time = theKernel().CurrentElapsedTime();

  // ----- request traffic for Hello -----
  double diff_hello = calculateTraffic( NetStatsCollector::Hello );
  // ----- request traffic for NodePeer -----
  double diff_nodepeer = calculateTraffic( NetStatsCollector::NodePeer );
  // ----- request traffic for UNI -----
  double diff_uni = calculateTraffic( NetStatsCollector::UNI );

  if (( diff_hello > 0.0 ) ||
      ( diff_nodepeer > 0.0 ) ||
      ( diff_uni > 0.0 ))
    sprintf( notebook_entry, "%lf %lf %lf %lf\n", time, diff_hello, diff_nodepeer, diff_uni );

  quit_now = false;
}

double TrafficInvestigator::calculateTraffic( NetStatsCollector::TrafficTypes t )
{
  double newTraffic = 0, diff = 0;
  dic_item di, ddi;

  // ----- request traffic for Hello -----
  newTraffic = trafficSize( 0, t );

  // If the list isn't present we must allocate it and populate it with a zero
  list< double > * vals = 0;
  if ( ! ( di = _traffic.lookup( t ) ) ) {
    vals = new list< double > ;
    assert( vals != 0 );
    vals->append( 0.0 );
    di = _traffic.insert( t, vals );
    assert( di != 0 );
  } else
    vals = _traffic.inf( di );

  assert( vals != 0 );

  // -----------------------------------------
  if ( ! (ddi = _lastTraffic.lookup( t ) ) )
    ddi = _lastTraffic.insert( t, 0.0 );
  diff = newTraffic - _lastTraffic.inf( ddi ); 
  _lastTraffic.change_inf( ddi, newTraffic );
  // -----------------------------------------

  // record the new value
  vals->push( diff );
  // remove the oldest value if applicable
  if ( vals->size() > _smoothing_factor )
    vals->Pop();
  // the list must be smaller than the smoothing factor + 1
  assert( vals->size() <= _smoothing_factor );
  // reset diff
  diff = 0;
  list_item li;

  // compute sum 
  double damp = 1.0;
  forall_items( li, *vals ) {
    diff += damp * vals->inf( li );
  }

  // compute the average
  diff /= ((double)_smoothing_factor * getIntensity());
  return diff;
}
