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
static char const _NetStatsCollector_cc_rcsid_[] =
"$Id: NetStatsCollector.cc,v 1.7 1999/02/17 22:02:04 mountcas Exp $";
#endif

#include "NetStatsCollector.h"
#include "StatRecord.h"
#include "Investigator.h"
#include "InvestigatorTimer.h"
#include <codec/pnni_ig/id.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/SimEvent.h>
#include <FW/basics/diag.h>
#include <fsm/omni/Omni.h>
#include <fsm/omni/OmniProxy.h>
#include <iostream.h>
#include <fstream.h>
#include <FW/basics/Log.h>

NetStatsCollector & theNetStatsCollector(void)
{
  if (!NetStatsCollector::_singleton)
    NetStatsCollector::_singleton = new NetStatsCollector( );

  return *(NetStatsCollector::_singleton);
}

NetStatsCollector * NetStatsCollector::_singleton = 0;

NetStatsCollector::NetStatsCollector(void) 
  : _maxInterval( 1 ) { Initialize(); }

NetStatsCollector::~NetStatsCollector() 
{ 
  dic_item di;
  forall_items(di, _events)
    delete _events.inf(di);
  _events.clear();

  list_item li_inv;
  forall_items(li_inv, _investigators) {
    Investigator* inv = _investigators.inf( li_inv );
    delete inv;
  }
  _investigators.clear();

  cout << "HelloTraffic: " << TrafficSize( NetStatsCollector::Hello ) << endl;
  cout << "NodePeerTraffic: " << TrafficSize( NetStatsCollector::NodePeer ) << endl;
  cout << "UNITraffic: " << TrafficSize( NetStatsCollector::UNI ) << endl;

  forall_items(di, _traffic)
    free( _traffic.key( di ) );
  _traffic.clear();
}

void NetStatsCollector::Initialize(void)
{
  SetCollectionOn("Call_Submission");
  SetCollectionOn("Call_Arrival");
  SetCollectionOn("Call_Admission");
  SetCollectionOn("Call_Rejection");
  SetCollectionOn("DTL_Expansion");
  SetCollectionOn("Crankback");
  SetCollectionOn("Start_Elections");
  SetCollectionOn("I_am_PGL");
  SetCollectionOn("Voted_Out");
  SetCollectionOn("Lost_Election");
  SetCollectionOn("Hlink_Aggr");
  SetCollectionOn("NSP_Aggr");
  SetCollectionOn("Hello_Up");
  SetCollectionOn("Hello_Down");
  SetCollectionOn("NP_Full");
  SetCollectionOn("NP_Down");
  SetCollectionOn("NP_Exchanging");
  SetCollectionOn("Switch_Up");
  SetCollectionOn("Switch_Down");
  SetCollectionOn("Local_Synchrony");
  SetCollectionOn("Local_Discord");
  SetCollectionOn("Global_Synchrony");
  SetCollectionOn("Global_Discord");
  SetCollectionOn("End_Of_Simulation");

  _traffic_filename = 0;
  _traffic_nni = 0;
  _traffic_hello = 0;
  _traffic_nodepeer = 0;
}

void NetStatsCollector::SetCollectionOn(const char * type)
{
  dic_item di;
  if (! (di = _logging.lookup((char *)type)) )
    _logging.insert((char *)type, true);
  else
    _logging.change_inf(di, true);
}

void NetStatsCollector::SetCollectionOff(const char * type)
{
  dic_item di;
  if (! (di = _logging.lookup((char *)type)) )
    _logging.insert((char *)type, false);
  else
    _logging.change_inf(di, false);
}

void NetStatsCollector::ReportNetEvent(const char * type, 
				       const char * location,
				       const char * misc, 
				       const NodeID * node)
{
  if (strcmp(type, "Global_Synchrony") &&
      strcmp(type, "Global_Discord"))
    assert( type != 0 && location != 0 );

  double time = theKernel().CurrentElapsedTime();

  // Notify of any add/delete events
  NotifyOmni( time, type, misc, node );

  // Notify OmniProxy ---->
  char buf[256];
  sprintf( buf, "%lf\t%s\t%s\t%s\t%s", 
	   time, type, // time and type are req'd for ALL events
	   location ? location : "(null)", 
	   misc ? misc : "(null)",
	   (node ? node->Print() : "(null)") );
  theOmniProxy().notify( buf );
  // <---- Notify OmniProxy

  // interleave OMNI comments into VisLog
  AppendCommentToLog( buf );

  if (isLoggingOn(type)) {
    StatRecord * sr = new StatRecord(type, location, misc, node, time);
    _events.insert( time, sr );
    
    list_item li_inv;
    forall_items(li_inv, _investigators) {
      Investigator * inv = _investigators.inf( li_inv );
      if ( inv->timedInvestigator() == false )
	inv->NSC_Notification();
    }

    diag("fsm.netstats", DIAG_INFO, 
	 "NetStatsCollector received event '%s' from %s %s at time %lf.\n",
	 type, location, (node ? node->Print() : ""), time);
  }

  if ( _invTimers.empty() == true )
    CleanUp();
}

void NetStatsCollector::NotifyOmni( double time, const char * type, 
				    const char * misc, const NodeID * node )
{
  char buf[256]; buf[0] = '\0';

  // Special Events that trigger OmniProxy notification
  if (!strcmp(type, "Switch_Up")) {
    // add node
    assert( node != 0 );
    sprintf( buf, "%lf add node %s", time,
	     node->Print() );
  } else if (!strcmp(type, "Voted_Out") ||
	     !strcmp(type, "Switch_Down")) {
    assert( node != 0 );
    // delete node  -- for Voted_Out do we want delete leader instead?
    sprintf( buf, "%lf delete node %s", time,
	     node->Print() );
  } else if (!strcmp(type, "Hello_Up") ||
	     !strcmp(type, "Hlink_Aggr")) {
    // add edge
    assert( node && misc );
    sprintf( buf, "%lf add edge %s %s", time,
	     node->Print(), misc );
  } else if (!strcmp(type, "Hello_Down") ||
	     !strcmp(type, "Hlink_Deaggr")) {
    // delete edge
    assert( node && misc );
    sprintf( buf, "%lf delete edge %s %s", time,
	     node->Print(), misc );
  } else if (!strcmp(type, "Lost_Election") ||
	     !strcmp(type, "I_am_PGL")) {
    // add leader
    assert( node && misc );
    sprintf( buf, "%lf add leader %s %s", time,
	     node->Print(), misc );
  }
  if (*buf)
    theOmniProxy().notify( buf );
}

list<const StatRecord * > * NetStatsCollector::ExtractNetEvents(double horizon, 
								int items_limit,
								const char * type, 
								const char * location) const
{
  // If you want to see everything use "*", not 0
  assert( type != 0 && location != 0 );

  list<const StatRecord * > * events = new list<const StatRecord * >;
  dic_item di;
  if (di = _events.last()) {
    StatRecord * sr = _events.inf(di);

    KernelTime   event_time   = _events.key(di);
    KernelTime   current_time = theKernel().CurrentElapsedTime();

    int ct = 0;
    while (di != 0 && 
	   ((double)(current_time - event_time) <= horizon) &&
	   (ct <= items_limit)) {
      if ((!strcmp(type, "*") || !strcmp(sr->getType(), type)) &&
	  (!strcmp(location, "*") || !strcmp(sr->getLoc(), location))){
	events->append(sr);
	ct++;
      }
      
      if (di = _events.pred( di )) {
	sr = _events.inf(di);
	event_time = _events.key(di);
      }
    }
  }

  return events;
}

void NetStatsCollector::DumpNetEvents(const char * type, 
				      const char * location,
				      const char * filename)
{
  // If you want to see everything use "*", not 0
  assert( type != 0 && location != 0 );

  ostream * os;
  if (filename)
    os = new ofstream(filename);
  else
    os = &cout;

  // Optional comment so people can figure out what's in the file
  *os << "# Time\tType\t\tLocation\tMisc Info.\tNodeID" << endl;

  dic_item di;
  forall_items(di, _events) {
    StatRecord * sr = _events.inf(di);

    if ((!strcmp(type, "*") || !strcmp(sr->getType(), type)) &&
	(!strcmp(location, "*") || !strcmp(sr->getLoc(), location))) {
      *os << _events.key(di) << "\t" << sr->getType() << "\t"
	  << sr->getLoc() << "\t" << sr->getMisc() << "\t";
      if (sr->getNode())
	*os << *(sr->getNode());
      *os << endl;
    }
  }
}

bool NetStatsCollector::isLoggingOn(const char * type) const
{
  dic_item di;
  if (di = _logging.lookup((char *)type))
    return _logging.inf(di);
  return false;
}


void NetStatsCollector::RegisterInvestigator( Investigator * inv ) 
{
  assert( inv != 0 && ! _investigators.lookup( inv ) );
  // inv is a valid ptr AND it hasn't been registered already
  _investigators.append(inv);

  if ( !(inv->timedInvestigator()) && inv->getIntensity() > _maxInterval )
    _maxInterval = (int)inv->getIntensity();
  else if ( inv->timedInvestigator() ) {
    InvestigatorTimer * th = new InvestigatorTimer( this, inv->getIntensity(), inv );
    _invTimers.append( th );
    Register( th );
  }
}

void NetStatsCollector::changeIntensity( Investigator * inv, double intensity )
{
  list_item li = _investigators.lookup( inv );
  assert( inv != 0 && li != 0 );

  if ( !(inv->timedInvestigator()) && inv->getIntensity() > _maxInterval )
    _maxInterval = (int)inv->getIntensity();
  else if ( inv->timedInvestigator() ) {
    list_item li;
    forall_items( li, _invTimers ) {
      InvestigatorTimer * th = _invTimers.inf( li );
      if ( th->equals( inv ) ) {
	delete th;
	_invTimers.del_item( li );
	break;
      }
    }
    InvestigatorTimer * th = new InvestigatorTimer( this, inv->getIntensity(), inv );
    _invTimers.append( th );
    Register( th );
  }
}

void NetStatsCollector::TrafficNotification( const char * location, 
					     double bytes,
					     NetStatsCollector::TrafficTypes t )
{
  dic_item di, ddi;

  if ( di = _traffic.lookup( (char *)location ) ) {
    dictionary<NetStatsCollector::TrafficTypes, double> * tt2t =
      _traffic.inf( di );

    if ( ddi = tt2t->lookup( t ) ) {
      double prev = tt2t->inf( ddi );
      tt2t->change_inf( ddi, prev + bytes );
    } else
      tt2t->insert( t, bytes );
  } else { // haven't seen it before
    dictionary<NetStatsCollector::TrafficTypes, double> * tt2t =
      new dictionary<NetStatsCollector::TrafficTypes, double> ;
    tt2t->insert( t, bytes );
    _traffic.insert( strdup( location ), tt2t );
  }

  if (_traffic_filename) {
    double time = (double)( theKernel().CurrentElapsedTime() );

    switch(t) {
    case Hello:
      *_traffic_hello << time << " " << bytes << endl << flush;
      break;
    case NodePeer:
      *_traffic_nodepeer << time << " " << bytes << endl << flush;
      break;
    case UNI:
      *_traffic_nni << time << " " << bytes << endl << flush;
      break;
    default:
      break;
    }
  }
}

double NetStatsCollector::TrafficSize( const char * location, 
				       NetStatsCollector::TrafficTypes t )
{
  if ( ! location || ! *location )
    return TrafficSize( t );

  double rval = 0.0;
  dic_item di, ddi;

  if ( di = _traffic.lookup( (char *)location ) ) {
    dictionary<NetStatsCollector::TrafficTypes, double> * tt2t =
      _traffic.inf( di );

    if ( t != NetStatsCollector::Unknown ) {
      if ( ddi = tt2t->lookup( t ) )
	rval = tt2t->inf( ddi );
    } else {
      // add them all up
      forall_items( ddi, *tt2t ) {
	rval += tt2t->inf( ddi );
      }
    }
  } else {
    diag("fsm.netstats", DIAG_FATAL, 
	 "NetStatsCollector: No traffic information found for '%s'!",
	 location);
  }
  return rval;
}

double NetStatsCollector::TrafficSize( NetStatsCollector::TrafficTypes t )
{
  double rval = 0.0;
  dic_item di, ddi;

  forall_items( di, _traffic ) {
    dictionary<NetStatsCollector::TrafficTypes, double> * tt2t =
      _traffic.inf( di );
    assert( tt2t != 0 );
    
    if ( t != NetStatsCollector::Unknown ) {
      // Only return specific type of traffic at all locations
      if ( ddi = tt2t->lookup( t ) )
	rval += tt2t->inf( ddi );
    } else {
      // all traffic at all locations
      forall_items( ddi, *tt2t ) {
	rval += tt2t->inf( ddi );
      }
    }
  }
  return rval;
}

void NetStatsCollector::DumpTraffic( NetStatsCollector::TrafficTypes t )
{
  char buf[256];
  double time = theKernel().CurrentElapsedTime();

  if ( t == Unknown )
    sprintf( buf, "# NSC %lf: TotalTraffic=%lf (H:%lf, N:%lf, U:%lf)", 
	     time, TrafficSize(), TrafficSize( Hello ),
	     TrafficSize( NodePeer ), TrafficSize( UNI ) );
  else
    sprintf( buf, "# NSC %lf: %sTraffic=%lf",
	     time, (t == Hello ? "Hello" : 
		    (t == NodePeer ? "NodePeer" : 
		     ( t == UNI ? "UNI" : "!ERROR!" ))),
             TrafficSize( t ) );
  theOmniProxy().notify( buf );
}

void NetStatsCollector::Interrupt( SimEvent * se )
{
  diag( "fsm.netstats", DIAG_FATAL,
	"NetStatsCollector received SimEvent %d which it cannot handle.",
	se->GetCode() );
}

void NetStatsCollector::Notify( Investigator * inv )
{
  dic_item di;
  if (! (di = _seenEvents.lookup( inv ) ) )
    di = _seenEvents.insert( inv, 0.0 );

  KernelTime kt = ( _events.last() ? (double)(_events.key( _events.last() ))
		    : 0.0 );
  _seenEvents.change_inf( di, kt );

  CleanUp();
}

void NetStatsCollector::Remove( Investigator * inv )
{
  dic_item di;
  assert( di = _seenEvents.lookup( inv ) );
  _seenEvents.del_item( di );
}

void NetStatsCollector::CleanUp( void )
{
  if ( _invTimers.empty() == false ) {
    // we have Timer based Investigators ...
    KernelTime kt = _seenEvents.inf( _seenEvents.first() );
    dic_item di;
    forall_items(di, _seenEvents) {
      if ( _seenEvents.inf( di ) < kt )
	kt = _seenEvents.inf( di );
    }
    if ( (double)kt > 0.0 ) {
      while (( kt < _events.key( _events.first() ) ) && 
	     _events.size() > _maxInterval ) {
	delete _events.inf( _events.first() );
	_events.del_item( _events.first() );
      }
    }
  } else if ( _events.size() > _maxInterval ) {
    // time to clean-up the events
    dic_item di = _events.first();
    delete _events.inf( di );
    _events.del_item( di );
  }
}


void NetStatsCollector::TrafficLoggingOn(const char* filename) {

  TrafficLoggingOff();

  if (filename) {
    _traffic_filename = new char [ strlen(filename)+1 ];
    strcpy( _traffic_filename, filename );

    char tmp[strlen(filename)+50];
    sprintf(tmp,"%s.nni",filename);
    _traffic_nni = new ofstream( tmp );
    sprintf(tmp,"%s.hello",filename);
    _traffic_hello = new ofstream( tmp );
    sprintf(tmp,"%s.nodepeer",filename);
    _traffic_nodepeer = new ofstream( tmp );
  }
}

void NetStatsCollector::TrafficLoggingOff(void) {
  delete [] _traffic_filename;
  _traffic_filename = 0;

  delete _traffic_nni;
  _traffic_nni = 0;
  delete _traffic_hello;
  _traffic_hello = 0;
  delete _traffic_nodepeer;
  _traffic_nodepeer = 0;
}
