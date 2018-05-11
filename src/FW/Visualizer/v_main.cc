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
static char const _v_main_cc_rcsid_[] =
"$Id: v_main.cc,v 1.1 1999/01/20 22:58:13 mountcas Exp $";
#endif

#include "v_main.h"
#include "v_app.h"
#include "v_graph.h"
#include "v_list.h"
#include "v_table.h"
#include "v_time.h"

#include <qpainter.h>
#include <iostream.h>

v_main::v_main(v_app * p, int width, int height) 
  : QWidget(p), _graph(0), _list(0),
    _table(0), _time(0), _width(width), _height(height)
{ 
  setBackgroundColor( black );

  // calls data before redraw for nodes and edges
  _graph = new v_graph( this, p->getData() );
  // calls data before redraw for visitors in play
  _list  = new v_list(  this );
  // calls data before redraw for visitor types known
  _table = new v_table( this, p->getData() );
  // calls data before redraw for current time
  _time  = new v_time(  this, p->getData() );

  _graph->setFocus();
}

v_main::~v_main() 
{ 
#if 0 // This bothers Qt's memory manager somehow
  delete _graph;
  delete _list;
  delete _table;
  delete _time;
#endif
}

int v_main::width(void) const
{
  return _width;
}

int v_main::height(void) const
{
  return _height;
}

int v_main::columnWidth(void) const
{
  return (_width / ROWS);
}

int v_main::rowHeight(void) const
{
  return (_height / COLUMNS);
}

int v_main::canvasWidth(void) const
{
  return (_width - (columnWidth() + (2 * SEP)));
}

int v_main::canvasHeight(void) const
{
  return (_height - (5 * SEP));
}

void v_main::resize(int width, int height)
{
  _width = width;
  _height = height;

  int w = columnWidth() - SEP;
  // 3/8ths of canvasHeight
  int h = (int)( 0.375 * canvasHeight() ) - SEP;
  _list->setGeometry(  _width - columnWidth(), SEP, w, h );

  int prevH = h + (2 * SEP);
  // 1/2 of canvasHeight
  h = (int)( 0.5 * canvasHeight() ) - SEP;
  _table->setGeometry( _width - columnWidth(), prevH, w, h );

  prevH += h + SEP;
  // 1/8th of canvasHeight
  h = (int)( 0.125 * canvasHeight() );
  _time->setGeometry(  _width - columnWidth(), prevH, w, h );

  w = canvasWidth();
  h = canvasHeight();
  _graph->setGeometry( SEP, SEP, w, h );
}

void v_main::setTime(double t)
{
  _time->setTime(t);
}

void v_main::stopTimers(void)
{
  _graph->stopTimer(v_graph::jTimer);
}

void v_main::jiggleTimer(void) 
{
  _graph->restartTimer(v_graph::jTimer);
}

void v_main::notify(v_visitor * v)
{
  _list->notify(v);
}

void v_main::remove(v_visitor * v)
{
  _list->remove(v);
}

const list<v_visitor *> * v_main::visitors(void) const
{
  return _list->visitors();
}

void v_main::zoom_in(void)
{
  _graph->zoom_in();
  update( );
}

void v_main::zoom_out(void)
{
  _graph->zoom_out();
  update( );
}

void  v_main::paintEvent( QPaintEvent * pe )
{
  //  cout << "v_main::paintEvent" << endl;
  _graph->repaint( false );
  _list->repaint( false );
  _table->repaint( false );
  _time->repaint( false );
}

void  v_main::keyPressEvent( QKeyEvent * ke )
{
  _graph->keyPressEvent( ke );
}
