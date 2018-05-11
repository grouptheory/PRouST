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
static char const _Investigator_cc_rcsid_[] =
"$Id: Investigator.cc,v 1.5 1999/02/04 18:12:31 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/kernel/Kernel.h>
#include "Investigator.h"
#include "NetStatsCollector.h"
#include "StatRecord.h"

int compare (Investigator * const & l, Investigator * const & r)
{
  long rhs = (long)r, lhs = (long)l;
  return (lhs - rhs);
}

//------------------------------------------------------------
Investigator::Investigator( const char * notebook_name, 
			    double investigation_intensity,
			    bool timer )
  : _investigation_intensity( investigation_intensity ), 
    _event_ctr( 0 ), _quit( false ), _timer( timer )
{
  _notebook_name = new char [ strlen( notebook_name ) + 1 ];
  strcpy( _notebook_name, notebook_name );
  _ofs = new ofstream( _notebook_name, ios::app );
  assert( _ofs != 0 && _ofs->good() );
  theNetStatsCollector().RegisterInvestigator( this );
}

double Investigator::getIntensity(void) const
{
  return _investigation_intensity;
}

bool   Investigator::timedInvestigator(void) const
{
  return _timer;
}

//------------------------------------------------------------
Investigator::~Investigator() 
{
  delete _ofs; // handles closing it
  delete _notebook_name;
}

//------------------------------------------------------------
void Investigator::changeIntensity(double new_intensity)
{
  _investigation_intensity = new_intensity;
  theNetStatsCollector().changeIntensity( this, _investigation_intensity );
}

//------------------------------------------------------------
list< const StatRecord * > * 
Investigator::obtainRecentEvents(double history_horizon,
				 double number_of_events,
				 const char * type, 
				 const char * location) const
{
  return theNetStatsCollector().ExtractNetEvents(history_horizon, 
						 (int)number_of_events, 
						 type, location);
}

//------------------------------------------------------------
void Investigator::NSC_Notification(void) 
{
  if ( _quit == false ) {
    _event_ctr++;
    if ( _timer == true || _event_ctr > _investigation_intensity) {
      _event_ctr = 0;
      
      strcpy( _notebook_entry, "" );
      makeObservation(_notebook_entry, _quit);

      assert( strlen( _notebook_entry ) <= 1024 );

      if (strcmp( _notebook_entry, "" )) {
	*(_ofs) << _notebook_entry << flush;
	strcpy( _notebook_entry, "" );
      }
      
      if (_quit) {
	theKernel().Stop();
	delete _ofs;
	exit(0);
      }
    }
  }
}

double Investigator::trafficSize( const char * location, NetStatsCollector::TrafficTypes t )
{
  return theNetStatsCollector().TrafficSize( location, t );
}

void Investigator::finalObservation(const char * notebook_entry)
{
  if ( notebook_entry && *notebook_entry )
    *(_ofs) << notebook_entry << flush;
}
