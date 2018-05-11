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
static char const _v_graph_cc_rcsid_[] =
"$Id: v_graph.cc,v 1.1 1999/01/20 22:58:13 mountcas Exp $";
#endif

#include "v_graph.h"
#include "v_main.h"
#include "v_data.h"
#include "v_canvas.h"

#include <qpixmap.h>
#include <common/cprototypes.h>
#include <DS/containers/list.h>

#include <math.h>

v_graph::v_graph(v_main * p, v_data * d) 
  : QScrollView(p), _parent(p), _data(d), 
    _zoomLevel(0), _jTimer(-1)
{ 
  setBackgroundColor( black );
  setVScrollBarMode( QScrollView::AlwaysOn );
  setHScrollBarMode( QScrollView::AlwaysOn );
  setFrameStyle( Box | Raised );

  _canvas = new v_canvas( this, d );
  addChild( _canvas );
  _canvas->setFocus( );
}

v_graph::~v_graph() { }

void  v_graph::paintEvent( QPaintEvent * pe )
{
  //  cout << "v_graph::paintEvent" << endl;
  _canvas->repaint( false );
  //  drawContentsOffset( &p, 0, 0, 0, 0, width(), height() );
}

void v_graph::zoom_in(void)
{
  _zoomLevel++;
}

void v_graph::zoom_out(void)
{
  _zoomLevel--;
}

void  v_graph::restartTimer(v_graph::timers t)
{
  if (t == jTimer)
    _jTimer = startTimer( 50 );
}

void  v_graph::stopTimer(v_graph::timers t)
{
  if (t == jTimer)
    killTimer( _jTimer );
}

void  v_graph::timerEvent( QTimerEvent * te )
{
  if (te->timerId() == _jTimer) {
     killTimer( _jTimer );
    _data->layout();
  }
}

void  v_graph::keyPressEvent( QKeyEvent * ke )
{
  _canvas->keyPressEvent( ke );
}
