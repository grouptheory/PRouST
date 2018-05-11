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
#ifndef __NETSTATSCOLLECTOR_H__
#define __NETSTATSCOLLECTOR_H__

#ifndef LINT
static char const _NetStatsCollector_h_rcsid_[] =
"$Id: NetStatsCollector.h,v 1.21 1999/02/08 15:04:25 mountcas Exp $";
#endif

#include <FW/kernel/SimEntity.h>
#include <DS/containers/dictionary.h>
#include <FW/kernel/KernelTime.h>

class StatRecord;
class NodeID;
class Investigator;
class InvestigatorTimer;
class ofstream;

class NetStatsCollector : public SimEntity {
  friend NetStatsCollector & theNetStatsCollector(void);
  friend class fsm_atexitCleanup;
  friend class InvestigatorTimer;
public:

  enum TrafficTypes {
    Unknown = 0,
    Hello,
    NodePeer,
    UNI
  };

  void SetCollectionOn(const char * type);
  void SetCollectionOff(const char * type);

  void ReportNetEvent(const char * type, const char * location,
		      const char * misc = 0, const NodeID * node = 0);
  
  void DumpNetEvents(const char * type, const char * location,
		     const char * filename = 0);

  list<const StatRecord * > *  ExtractNetEvents(double horizon, 
						int items_limit,
						const char * type, 
						const char * location) const;
  
  void RegisterInvestigator( Investigator * inv );

  void   TrafficNotification( const char * location, 
			      double bytes, 
			      NetStatsCollector::TrafficTypes t );
  // returns traffic size by location (and possibly by type at location)
  double TrafficSize( const char * location = 0, NetStatsCollector::TrafficTypes t = Unknown );
  // returns traffic size by type at all locations
  double TrafficSize( NetStatsCollector::TrafficTypes t );
  void   DumpTraffic( NetStatsCollector::TrafficTypes t = Unknown );

  void Interrupt( SimEvent * se );

  void TrafficLoggingOn(const char* filename);
  void TrafficLoggingOff(void);

  void changeIntensity( Investigator * i, double intensity );

private:

  void Initialize(void);
  void CleanUp(void);
  void NotifyOmni( double time, const char * type, const char * misc, const NodeID * n );

  bool isLoggingOn(const char * type) const;

  NetStatsCollector(void);
  ~NetStatsCollector();

  void Notify( Investigator * i );
  void Remove( Investigator * i );

  dictionary<char *, bool>             _logging;
  dictionary<KernelTime, StatRecord *> _events;

  dictionary<char *, dictionary< NetStatsCollector::TrafficTypes, double > * > _traffic;

  list<Investigator *> _investigators;
  list<InvestigatorTimer *> _invTimers;
  dictionary<Investigator *, KernelTime> _seenEvents;

  int  _maxInterval;
  char     * _traffic_filename;
  ofstream * _traffic_nni;
  ofstream * _traffic_hello;
  ofstream * _traffic_nodepeer;

  static NetStatsCollector * _singleton;
};

#endif
