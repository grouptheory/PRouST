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
static char const _v_table_cc_rcsid_[] =
"$Id: v_table.cc,v 1.1 1999/01/20 22:58:13 mountcas Exp $";
#endif

#include "v_table.h"
#include "v_main.h"
#include "v_visitor.h"
#include "v_conduit.h"

#include <qlabel.h>
#include <qpixmap.h>
#include <common/cprototypes.h>

v_table::v_table(v_main * p, v_data * d) 
  : QWidget(p), _parent(p), _data(d)
{ 
  //  setFocusPolicy( QWidget::NoFocus );
  //  setFrameStyle( Box | Sunken );
}

v_table::~v_table() { }

void v_table::paintEvent( QPaintEvent * pe )
{
  QPixmap window(size());
  
  QPainter thePainter;
  thePainter.begin(this);
  
  QPainter paintersAssistant;
  window.fill(white);
  paintersAssistant.begin(&window);
  draw(paintersAssistant);
  paintersAssistant.end();
  
  thePainter.drawPixmap( QPoint(0, 0), window);
  thePainter.end();
}

void v_table::draw(QPainter & p)
{
  int y = 25, x = QWidget::width() / 2 - ( p.fontMetrics().width("Visitor Legend") / 2 );
  int width = 15, height = 15;

  p.drawText( x, y, "Visitor Legend", strlen("Visitor Legend") );
  y += (p.fontMetrics().height() + 5) * 2;
  x = 10;

  const dictionary<char *, QColor> * ctable = VisitorColorsTable()->color_table();
  dic_item di;
  forall_items(di, *ctable) {
    const char * name = ctable->key(di);
    QColor color = ctable->inf(di);

    // If we want to view all the visitor colors, don't make this check
    if (! (ConduitRegistry()->GetFlags() & v_conduit_registry::ViewAllVisitorColors)) {
      if (! VisitorRegistry()->type_is_active(name))
	continue;
    }

    // draw a filled, colored rect and a string per Visitor
    p.fillRect( x, y - height, width, height, color );
    p.drawRect( x - 1, y - (height + 1), width + 1, height + 1);
    x += (width * 2);
    p.drawText( x, y, name, strlen(name) );

    y += (height + 2);
    x -= (width * 2);
  }
}
