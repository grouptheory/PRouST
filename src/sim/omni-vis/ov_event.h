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
#ifndef __OV_EVENT_H__
#define __OV_EVENT_H__
#ifndef LINT
static char const _ov_event_h_rcsid_[] =
"$Id: ov_event.h,v 1.5 1999/02/04 17:35:58 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
#include <qlistbox.h>
#include "eventTypeRegistry.h"

class ov_main;
class ov_data;
class Event;
class WidgetView;

//---------------------------------------------------------------
class ov_event : public QListBox {
public:

  ov_event( WidgetView * p, ov_data * d );

  void notify( int min, int max );
  void addEvent( Event * e );
  void remEvent( Event * e );
  void clearEvents( void );

  void filterEvents(void);

  void sprintfEvent(char * text, Event * e);
  void sprintfEventVerbose(char * text, Event * e);
  const Event * locateEvent(char * text) const;

  void selectEvent( double time, eventTypeRegistry::event_type );

private:
  
  WidgetView  * _parent;
  ov_data     * _data;

  list<Event *> _activeEvents;
  dictionary< char *, Event * > _txt2Event;
};

#endif // __OV_EVENT_H__
