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
static char const _WidgetView_cc_rcsid_[] =
"$Id: WidgetView.cc,v 1.33 1999/02/04 17:45:02 mountcas Exp $";
#endif

#include <common/cprototypes.h>

#include "WidgetView.h"
#include "canvas3D.h"
#include "ov_main.h"
#include "ov_data.h"
#include "ov_event.h"
#include "ov_ec.h"
#include "Event.h"

#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qslider.h>
#include <qlistbox.h>
#include <qtooltip.h>
#include <qscrollview.h>
#include <qmessagebox.h>

#include <codec/pnni_ig/id.h>


//----------------------------------------
// Methods of class 'WidgetView'
// ---------------------------------------

WidgetView::WidgetView( ov_main *parent, const char * name, ov_data * data )
  : QWidget( parent, name ), _parent(parent), _currentTime(0)
{
  QHBoxLayout * hbox;
  QVBoxLayout * vbox;
  QLabel      * msgLabel;
  QLabel      * msg;
  QColor        col;

  // Create a layout to position the widgets
  _topLayout = new QVBoxLayout( this, 10 );
  
  // Create a grid layout to hold most of the widgets
  _grid = new QGridLayout( 2, 2 );

  // This layout will get all of the stretch
  _topLayout->addLayout( _grid, 10 );
  
  // Create a 3D canvas for the network
  _viewer = new canvas3D( this, "canvas3D", data );
  if ( ! _viewer->isValid() )
    fatal("Failed to create OpenGL rendering context on this display");
  _viewer->setFocus( );
  _grid->addWidget( _viewer, 0, 0 );
  drawAxes( true );

  // ---------- Events ListBox ----------
  _lbEvents = new ov_event( this, data );
  _grid->addWidget( _lbEvents, 0, 1 );
  connect( _lbEvents, SIGNAL(selected(int)), 
	   SLOT(itemEventSelected(int)) );
  connect( _lbEvents, SIGNAL(highlighted(int)), 
	   SLOT(itemEventHighlighted(int)) );

  QToolTip::add( _lbEvents, "Small-scale view of simulation events" );
  
  // ---------- Events Slider -----------
  _eventViewer = new QScrollView( this , "largeScaleView" );
  _eventViewer->setHScrollBarMode( QScrollView::AlwaysOn );
  _eventCanvas = new ov_ec( _eventViewer, this, data, _parent->endTime() );
  _eventViewer->addChild( _eventCanvas );
  _grid->addWidget( _eventViewer, 1,0 );
  QToolTip::add( _eventViewer, "Large-scale view of simulation events" );

  // ---------- Location Label ----------
  vbox = new QVBoxLayout();
  _grid->addLayout( vbox , 1 , 1 );

  hbox = new QHBoxLayout();
  vbox->addLayout( hbox );

  msgLabel = new QLabel( this, "currentLocLabel" );
  msgLabel->setText( "Loc:  " );
  msgLabel->setAlignment( AlignRight|AlignVCenter );
  QSize labelsize = msgLabel->sizeHint();
  msgLabel->setFixedSize( labelsize );
  hbox->addWidget( msgLabel );
  
  _loc = new QLabel( this, "currentLoc" );
  _loc->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  _loc->setAlignment( AlignCenter );
  _loc->setFont( QFont("times",12,QFont::Bold) );
  _loc->setText( "???" );
  hbox->addWidget( _loc );
  QToolTip::add( _loc, "Current simulation location" );

  // ---------- Time Label ----------
  hbox = new QHBoxLayout();
  vbox->addLayout( hbox );

  msgLabel = new QLabel( this, "currentTimeLabel" );
  msgLabel->setText( "Time: " );
  msgLabel->setAlignment( AlignRight|AlignVCenter );
  labelsize = msgLabel->sizeHint();
  msgLabel->setFixedSize( labelsize );
  hbox->addWidget( msgLabel );
  
  _time = new QLabel( this, "currentTime" );
  _time->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  _time->setAlignment( AlignCenter );
  _time->setFont( QFont("times",12,QFont::Bold) );
  _time->setText( "???" );
  hbox->addWidget( _time , 10 );

  hbox = new QHBoxLayout();
  vbox->addLayout( hbox );

  // ---------- Event Label ----------
  msgLabel = new QLabel( this, "eventTypeLabel" );
  msgLabel->setText( "Event:" );
  msgLabel->setAlignment( AlignRight|AlignVCenter );
  labelsize = msgLabel->sizeHint();
  msgLabel->setFixedSize( labelsize );
  hbox->addWidget( msgLabel );
  
  _event = new QLabel( this, "eventType" );
  _event->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  _event->setAlignment( AlignCenter );
  _event->setFont( QFont("times",12,QFont::Bold) );
  _event->setText( "???" );
  hbox->addWidget( _event , 10 );
  QToolTip::add( _event, "Current simulation event" );

  // Adjust spacing and stretch attributes
  _grid->addColSpacing(0, 400);
  _grid->addColSpacing(1, 200);

  _grid->addRowSpacing(0, 400);
  _grid->addRowSpacing(1, 50);
  
  _grid->setColStretch(0, 10);
  _grid->setColStretch(1, 0);

  _grid->setRowStretch(0, 10);
  _grid->setRowStretch(1, 0);
  
  _topLayout->activate();
}

//-----------------------------------------------
void WidgetView::itemEventSelected( int index )
{
  //  cout << "List box item " << index << " selected." << endl;
  const Event * e = _lbEvents->locateEvent( (char *)_lbEvents->text( index ) );
  assert( e != 0 );
  // build a QMessageBox out of the Event
  char nodestr[80]; nodestr[0] = '\0';
  if ( e->getNode() )
    abbreviatedPrint(nodestr, e->getNode());

  QMessageBox mb( this, "Event Info" );
  QString text;
  text.sprintf( " %lf : %s at %s %s %s ",
		e->getTime(), e->getType(),
		e->getLoc(), e->getMisc(),
		nodestr );
  mb.setText( text );
  mb.setButtonText( "Dismiss" );
  mb.show( );
}

//-----------------------------------------------
void WidgetView::itemEventHighlighted( int index )
{    
  //  cout << "List box item " << index << " highlighted." << endl;
  const char * txt = _lbEvents->text( index );
  // Now that we have the text attempt to convert it into an OVNode
  // double : EventType at NodeID
  char type[128], temp[128]; double t;
  sscanf( txt, "%lf : %s at %s", &t, type, temp );
  // type and temp should enable us to highlight a specific node ...
  dic_item di = _abbrev2node.lookup(temp);
  NodeID * tmp = di != 0 ? _abbrev2node.inf( di ) : 0;
  Event dummyEvent( t, type, 0, 0, tmp ? tmp->copy() : 0 );
  _viewer->noteEvent( &dummyEvent );
}

//-----------------------------------------------
void WidgetView::eventSliderChanged( int v )
{
  int oldTime = _currentTime;
  _currentTime = v;

  if ( _currentTime > oldTime )
    _parent->parse( _currentTime );

  if ( _eventSlider != 0 )
    _eventSlider->setValue( v );

  // here we must somehow notify the event list
  //  repaint( false );
}

//-----------------------------------------------
void WidgetView::updateTime( int t )
{
  eventSliderChanged( t );
}

//-----------------------------------------------
void WidgetView::sliderMax( int t )
{
  if ( _eventSlider != 0 )
    _eventSlider->setRange( 0, t );
}

//-----------------------------------------------
void WidgetView::selectEvent( double t, eventTypeRegistry::event_type evt )
{
  _lbEvents->selectEvent( t, evt );
}

//-----------------------------------------------
void WidgetView::setEvent_Widget(const char * event)
{
  assert(event);
  _event->setText( event );
}

//-----------------------------------------------
void WidgetView::setTime_Widget(double time)
{
  assert(time >= 0);

  char time_str[50];
  sprintf( time_str, "%.5lf",time);
  _time->setText( time_str );
}

//-----------------------------------------------
void WidgetView::draw( QPainter & p ) 
{
  _viewer->repaint( false );
  _lbEvents->repaint( false );
  _eventCanvas->repaint( false );
}

//-----------------------------------------------
void WidgetView::moveCameraTo( double phi, double theta ) 
{
  _viewer->moveCameraTo(phi,theta);
}

//-----------------------------------------------
void WidgetView::drawAxes( bool state ) 
{
  _viewer->drawAxes( state );
}

//-----------------------------------------------
void WidgetView::setFont( const char * filename )
{
  _viewer->setFont( filename );
}

//-----------------------------------------------
void WidgetView::drawLeaderEdges( bool state ) 
{
  _viewer->drawLeaderEdges( state );
}

//-----------------------------------------------
void WidgetView::addEvent( Event * e )
{
  _lbEvents->addEvent( e );

  char text[256];
  sprintf(text, "%s", e->getType());
  _event->setText( text );

  _viewer->noteEvent( e );

  char abbrev[256];
  if ( e->getNode() ) {
    abbreviatedPrint( abbrev, e->getNode() );
    _abbrev2node.insert( strdup(abbrev), e->getNode()->copy() );
    _loc->setText( abbrev );
  }
  char timestr[50];
  abbreviatedTime(timestr, e->getTime());
  _time->setText( timestr );

  // notify the event canvas as well
  _eventCanvas->notify( e );
}

//-----------------------------------------------
void WidgetView::filterEvents(void) 
{
  _lbEvents->filterEvents();
}

void WidgetView::keyPressEvent( QKeyEvent * ke )
{
  //  cout << "WidgetView::keyPressEvent" << endl;
  _viewer->keyPressEvent( ke );
}

void WidgetView::drawPerspective( bool p )
{
  _viewer->drawPerspective( p );
}
