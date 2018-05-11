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
#ifndef __OV_EC_H__
#define __OV_EC_H__

#ifndef LINT
static char const _ov_ec_h_rcsid_[] =
"$Id: ov_ec.h,v 1.21 1999/02/04 17:47:00 mountcas Exp $";
#endif

#include <qwidget.h>
#include <qcolor.h>
#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>

class ov_data;
class Event;
class WidgetView;

class ov_ec : public QWidget {
public:

  ov_ec( QWidget * ip, WidgetView * p, ov_data * data, int width );
  virtual ~ov_ec();

  void notify( Event * e );

protected:

  void paintEvent( QPaintEvent * pe );
  void mousePressEvent(   QMouseEvent * me );

private:

  const QColor & event2color( Event * e ) const;
  int            event2row( Event * e ) const;

  WidgetView * _parent;
  ov_data    * _data;
  int          _width;
  int          _height;
  int          _evWidth;  
  const int    _numEvents;

  // time --> Event
  dictionary< double, list< Event * > * > _events;
};

#endif
