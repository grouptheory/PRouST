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
static char const _ov_ec_cc_rcsid_[] =
"$Id: ov_ec.cc,v 1.38 1999/02/09 22:16:33 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "ov_ec.h"
#include "WidgetView.h"
#include "eventTypeRegistry.h"
#include "Event.h"
#include <iostream.h>
#include <qpixmap.h>
#include <qpainter.h>

// This is based on a window size of 1024 x 768
#define NORMAL_WIDTH  795
#define NORMAL_HEIGHT 100

ov_ec::ov_ec( QWidget * ip, WidgetView * p, ov_data * data, int width ) 
  : QWidget( ip ), _parent( p ), _data(data), _evWidth( 5 ), 
    _numEvents(eventTypeRegistry::Last_type_sentinel - 
	       eventTypeRegistry::First_type_sentinel),
    _width( width * 5 ), _height( 42 )
{ 
  if ( _width < NORMAL_WIDTH ) {
    _width = NORMAL_WIDTH;
    _evWidth = width / 5;
  }
  // width * 5 since 4 pixels per event + 1 spacer
  setGeometry( 0, 0, _width, _height );
  setBackgroundColor( white );
}

ov_ec::~ov_ec() 
{ 
  dic_item di;
  forall_items( di, _events )
    delete _events.inf( di );
  _events.clear();
}

void ov_ec::paintEvent( QPaintEvent * pe )
{
  if ( _events.empty() == false ) {
    QPainter p(this);
    p.setClipRect(pe->rect());

    dic_item di;
    // obtain the list of events per time and draw vertical bands of color
    //   based on the types of events present
    
    int width = _evWidth - 1, height = _height / _numEvents;
    assert( height >= 1 );
    
    const QPen & oldPen = p.pen();
    
    int x = 0, y = 0;
    double time = 0.0;
    forall_items( di, _events ) {
      time = _events.key( di );
      x = (int)(_evWidth * time);

      list< Event * > * evlist = _events.inf( di );
      
      list_item li;
      forall_items( li, *evlist ) {
	Event * e = evlist->inf( li );
	p.setPen( event2color( e ) );
	y = event2row( e );

	// draw a rectangle of width by height in a particular color
	p.drawRect( x, y, width, height );
      }
    }
    
    p.setPen( oldPen );
  }
}

void ov_ec::mousePressEvent(   QMouseEvent * me )
{
  int x = me->pos().x();
  int y = me->pos().y();
  
  int time  = x / _evWidth;
  int event = y / ( _height / _numEvents );

  char evStr[64];
  eventTypeRegistry::event_type evt = (eventTypeRegistry::event_type)(event + 
    eventTypeRegistry::First_type_sentinel);
  theEventTypeRegistry().type2str( evt, evStr );
  //  cout << "ov_ec::mousePressEvent choose time " << time << " event " << evStr << endl;
  _parent->selectEvent( time, evt );
}

void ov_ec::notify( Event * e )
{
  dic_item di;

  if ( ! (di = _events.lookup( e->getTime() )) )
    di = _events.insert( e->getTime(), new list< Event * > );
  list< Event * > * evlist = _events.inf( di );

  list_item li;
  if ( ! (li = evlist->lookup( e )) ) {
    evlist->insert( e );
    repaint( false );
  }
}

const QColor & ov_ec::event2color( Event * e ) const
{
  switch ( theEventTypeRegistry().str2type( e->getType() ) ) {
    case eventTypeRegistry::Call_Submission_type:
      return darkYellow;
      break;
    case eventTypeRegistry::Call_Arrival_type:
      return darkGreen;
      break;
    case eventTypeRegistry::Call_Admission_type:
      return green;
      break;
    case eventTypeRegistry::Call_Rejection_type:
      return red;
      break;
    case eventTypeRegistry::DTL_Expansion_type:
      return blue;
      break;
    case eventTypeRegistry::Crankback_type:
      return magenta;
      break;
    case eventTypeRegistry::I_am_PGL_type:
      return darkMagenta;
      break;
    case eventTypeRegistry::Voted_Out_type:
      return darkCyan;
      break;
    case eventTypeRegistry::Lost_Election_type:
      return darkRed;
      break;
    case eventTypeRegistry::Start_Election_type:
      return blue;
      break;
    case eventTypeRegistry::Hlink_Aggr_type:
      return yellow;
      break;
    case eventTypeRegistry::Hlink_Deaggr_type:
      return yellow;
      break;
    case eventTypeRegistry::NSP_Aggr_type:
      return yellow;
      break;
    case eventTypeRegistry::Hello_Up_type:
      return green;
      break;
    case eventTypeRegistry::Hello_Down_type:
      return cyan;
      break;
    case eventTypeRegistry::NP_Exchanging_type:
      return lightGray;
      break;
    case eventTypeRegistry::NP_Full_type:
      return cyan;
      break;
    case eventTypeRegistry::NP_Down_type:
      return darkBlue;
      break;
    case eventTypeRegistry::Switch_Up_type:
      return black;
      break;
    case eventTypeRegistry::Switch_Down_type:
      return black;
      break;
    case eventTypeRegistry::Local_Synchrony_type:
      return blue;
      break;
    case eventTypeRegistry::Local_Discord_type:
      return yellow;
      break;
    case eventTypeRegistry::Global_Synchrony_type:
      return green;
      break;
    case eventTypeRegistry::Global_Discord_type:
      return red;
      break;
    case eventTypeRegistry::None_type:
    default:
      return white;
  }
  // we should NEVER get here
  abort();
}

int ov_ec::event2row( Event * e ) const
{
  eventTypeRegistry::event_type evt = theEventTypeRegistry().str2type( e->getType() );

  int row = evt - eventTypeRegistry::First_type_sentinel;
  return (row * ( _height / _numEvents ));
}
