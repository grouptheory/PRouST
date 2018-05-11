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
#ifndef __v_MAIN_H__
#define __v_MAIN_H__

#ifndef LINT
static char const _v_main_h_rcsid_[] =
"$Id: v_main.h,v 1.1 1999/01/20 22:58:05 mountcas Exp $";
#endif

#include <qwidget.h>
#include <DS/containers/list.h>

#define SEP     25
#define COLUMNS 8
#define ROWS    5

class v_app;
class v_graph;
class v_list;
class v_table;
class v_time;
class v_visitor;

class v_main : public QWidget {
  Q_OBJECT

  friend class v_app;
public:

  v_main( v_app * parent, int w, int h);
  virtual ~v_main();

  void draw( QPainter & p );

  void zoom_in(void);
  void zoom_out(void);

  int   width(void)  const;
  int   height(void) const;
  int   columnWidth(void) const;
  int   rowHeight(void) const;
  int   canvasWidth(void) const;
  int   canvasHeight(void) const;

  void  resize(int w, int h);
  void  setTime(double t);
  void  jiggleTimer(void);
  void  stopTimers(void);

  void  notify(v_visitor * v);
  void  remove(v_visitor * v);

  const list<v_visitor *> * visitors(void) const;

protected:

  void paintEvent( QPaintEvent * pe );
  void keyPressEvent( QKeyEvent * ke );

private:

  v_app   * _parent;

  v_graph * _graph;
  v_list  * _list;
  v_table * _table;
  v_time  * _time;

  int       _width;
  int       _height;
};

#endif
