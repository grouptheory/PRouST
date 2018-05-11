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
#ifndef __INVESTIGATOR_H__
#define __INVESTIGATOR_H__

#ifndef LINT
static char const _Investigator_h_rcsid_[] =
"$Id: Investigator.h,v 1.3 1999/02/04 18:06:51 mountcas Exp $";
#endif

#include "NetStatsCollector.h"
#include <DS/containers/list.h>
#include <fstream.h>

class StatRecord;
class InvestigatorTimer;

class Investigator {
  friend class NetStatsCollector;
  friend class InvestigatorTimer;
public:

  // NOTICE:  The params have different meanings based on the final boolean value
  //  If true  investigation_intensity is simulation time in seconds
  //  If false investigation_intensity is number of events to be woken up at
  Investigator( const char * notebook_name, 
		double investigation_intensity,
		bool timer = false );

  double getIntensity(void) const;
  bool   timedInvestigator(void) const;

protected:

  void changeIntensity(double new_intensity);

  list< const StatRecord * > * 
      obtainRecentEvents(double history_horizon,
			 double number_of_events,
			 const char * type, 
			 const char * location) const;

  virtual void makeObservation(char * const notebook_entry, bool & quit_now) = 0;
  void finalObservation(const char * notebook_entry);

  double trafficSize( const char * location = 0, 
		      NetStatsCollector::TrafficTypes t = NetStatsCollector::Unknown );

  virtual ~Investigator();

private:

  void NSC_Notification(void);

  char _notebook_entry[1024];
  int  _event_ctr;
  bool _quit;

  char     * _notebook_name;
  ofstream * _ofs;

  double     _investigation_intensity;
  bool       _timer;
};

#endif // __INVESTIGATOR_H__
