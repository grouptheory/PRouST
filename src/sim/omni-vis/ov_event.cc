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
static char const _ov_event_cc_rcsid_[] =
"$Id: ov_event.cc,v 1.15 1999/02/04 17:37:32 mountcas Exp $";
#endif

#include "ov_event.h"
#include "ov_main.h"
#include "WidgetView.h"
#include "ov_data.h"
#include "Event.h"
#include "eventTypeRegistry.h"

#include <codec/pnni_ig/id.h>

//----------------------------------------
// Methods of class 'ov_event'
// ---------------------------------------

ov_event::ov_event( WidgetView * p, ov_data * d ) 
  : QListBox( p ), _parent(p), _data(d) 
{ 
  setFrameStyle( WinPanel | Sunken );
}

void ov_event::addEvent( Event * e )
{
  if (! _activeEvents.lookup( e ) ) {
    _activeEvents.insert( e );

    eventTypeRegistry::event_type t = 
      theEventTypeRegistry().str2type( e->getType() );
    if (theEventTypeRegistry().isSelected( t )) {

      char text[256];
      sprintfEvent(text, e);
      
      insertItem( text, 0 );
      _txt2Event.insert( strdup(text), e );
      setCurrentItem( 0 );
      centerCurrentItem();
      repaint(false);
    }
  }
}

void ov_event::selectEvent( double time, eventTypeRegistry::event_type type )
{
  // selects the first event of the specified type at the specified time
  for ( int index = 0; index < count(); index++ ) {
    const char * txt = text( index );
    const Event * e = locateEvent( (char *)txt );
    if ( type == theEventTypeRegistry().str2type( e->getType() ) &&
	 (int)time == (int)e->getTime() ) {
      setCurrentItem( index );
      centerCurrentItem( );
      repaint( false );
      break;
    }
  }
}

void ov_event::filterEvents(void) 
{
  clear();
  list_item li, nli;

  setAutoUpdate( FALSE );
  for ( li = _activeEvents.last(); li; li = nli ) {
    nli = _activeEvents.pred( li );

    Event * e = _activeEvents.inf( li );
    eventTypeRegistry::event_type t = 
      theEventTypeRegistry().str2type( e->getType() );
    
    if (theEventTypeRegistry().isSelected( t )) {

      char text[256];
      sprintfEvent(text, e);

      insertItem( text );
    }
  }
  setAutoUpdate( TRUE );
  repaint(false);
}

void ov_event::remEvent( Event * e )
{
  list_item li;

  if ( li = _activeEvents.lookup( e ) ) {
    char txt[256];
    sprintfEvent(txt, e);

    for (int i = 0; i < count(); i++) {
      const char * t = text( i );
      if ( !strcmp(txt, t) ) {
	removeItem( i );
	dic_item di;
	if ( di = _txt2Event.lookup( txt ) ) {
	  delete _txt2Event.key( di );
	  _txt2Event.del_item( di );
	}
	break;
      }
    }
    _activeEvents.del_item( li );
  }
}

void ov_event::clearEvents( void )
{
  clear();
  _activeEvents.clear();
}

void ov_event::notify( int minTime, int maxTime )
{
  list_item li, nli;

  for ( li = _activeEvents.first(); li; li = nli ) {
    nli = _activeEvents.next( li );

    Event * e = _activeEvents.inf( li );
    if ( e->getTime() < minTime ||
	 e->getTime() > maxTime )
      remEvent( e );
  }
}

void ov_event::sprintfEvent(char * text, Event * e) 
{
  char timestr[50];
  abbreviatedTime(timestr,e->getTime());

  char nodestr[80]; nodestr[0] = '\0';
  if ( e->getNode() )
    abbreviatedPrint(nodestr, e->getNode());
  sprintf(text, "%s : %s %s%s",
	  timestr, e->getType(), 
	  *nodestr ? "at " : "", nodestr);
}

void ov_event::sprintfEventVerbose(char * text, Event * e) 
{
  char timestr[50];
  abbreviatedTime(timestr,e->getTime());

  sprintf(text, "%s: %s at %s %s %s",
	  timestr, e->getType(), e->getLoc(),
	  e->getMisc(), e->getNode()->Print());
}

const Event * ov_event::locateEvent(char * text) const
{
  dic_item di;
  if ( di = _txt2Event.lookup( text )  )
    return _txt2Event.inf( di );
  return 0;
}
