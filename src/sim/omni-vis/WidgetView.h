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
#ifndef __WIDGET_H__
#define __WIDGET_H__

#ifndef LINT
static char const _WidgetView_h_rcsid_[] =
"$Id: WidgetView.h,v 1.16 1999/02/04 17:38:36 mountcas Exp $";
#endif

class QVBoxLayout;
class QGridLayout;
class QListBox;
class QLabel;
class QScrollView;
class QSlider;
class canvas3D;
class ov_main;
class ov_data;
class ov_event;
class ov_ec;
class Event;
class NodeID;

#include "eventTypeRegistry.h"
#include <qwidget.h>
#include <DS/containers/dictionary.h>

class WidgetView : public QWidget {
  friend class ov_main;

  Q_OBJECT
public:

  WidgetView( ov_main *parent, const char *name, ov_data *data );

  void draw( QPainter & p );

  void moveCameraTo( double phi, double theta );
  void drawAxes( bool state );
  void drawLeaderEdges( bool state );
  void drawPerspective( bool p );
  void setFont( const char * filename );

  void addEvent( Event * e );
  void filterEvents(void);
  void updateTime( int t );
  void sliderMax( int t );
  void selectEvent( double, eventTypeRegistry::event_type );

protected:

  void keyPressEvent( QKeyEvent  * ke );

private slots:

  void itemEventSelected( int );
  void itemEventHighlighted( int index );
  void eventSliderChanged( int );

private:

  void setTime_Widget(double time);
  void setEvent_Widget(const char * event);

  ov_event    * _lbEvents;
  ov_main     * _parent;
  ov_ec       * _eventCanvas;

  QVBoxLayout * _topLayout;
  QGridLayout * _grid;
  QLabel      * _time;
  QLabel      * _event;
  QLabel      * _loc;
  QScrollView * _eventViewer;
  QSlider     * _eventSlider;
  canvas3D    * _viewer;
  int           _currentTime;

  dictionary< char *, NodeID * > _abbrev2node;
};

#endif // __WIDGET_H__
