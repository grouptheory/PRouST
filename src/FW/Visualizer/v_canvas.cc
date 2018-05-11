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
static char const _v_canvas_cc_rcsid_[] =
"$Id: v_canvas.cc,v 1.1 1999/01/20 22:58:13 mountcas Exp $";
#endif

#include <math.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qkeycode.h>
#include <qmessagebox.h>
#include <common/cprototypes.h>

#include "v_canvas.h"
#include "v_app.h"
#include "v_main.h"
#include "v_graph.h"
#include "v_data.h"
#include "v_conduit.h"
#include "v_visitor.h"

const int OFFSET = 10; // 55;
const double MIN_DIST = 30.0;

v_canvas::v_canvas(v_graph * p, v_data * d)
  : QWidget(p), _data(d), _parent(p),
    _ungroupFlag(false), _selectionFlag(false),
    _moving_node(0)
{
  setGeometry( 0, 0, 1000, 1000 );
  setBackgroundColor( white );
  setFocusPolicy( QWidget::StrongFocus );

  _selectionBox.setRect(0, 0, 0, 0);
  _selectionStart.setX(0);
  _selectionStart.setY(0);
}

v_canvas::~v_canvas( ) { }

// -----------------------------------------------
void v_canvas::mousePressEvent( QMouseEvent * me )
{
  double x = me->pos().x() - OFFSET;
  double y = me->pos().y() - OFFSET;
  QPoint pntoff = QPoint((int)x, (int)y);
  double d_min = MIN_DIST;

  switch (me->button()) {
    case LeftButton:
      {
	// find the nearest node to move
	if (!(_moving_node = _data->nearest_node(pntoff, d_min))) {
	  if ( _selectedNodes.empty() == true ) {
	    // We are starting to draw a box
	    _selectionFlag = true;
	    _selectionBox.setRect(0, 0, 0, 0);
	    _selectionStart = pntoff;
	  } else
	    // We didn't selected a node, or start a box, deselect all selected nodes
	    _selectedNodes.clear();
	}
      }
      break;
    case MidButton: 
      {
	v_conduit * c = _data->nearest_node(pntoff, d_min);
	if (c != 0)
	  display_node_data( c );	
      }
      break;
    case RightButton:
      {
	// collapse a node, if possible
	v_conduit * c = _data->nearest_node(pntoff, d_min);
	bool changed = false;
	if (c != 0 && _ungroupFlag) 
	  changed = expand_cluster( c );
	else if (c != 0)
	  changed = collapse_containing_cluster( c );
	//	if (changed)
	repaint( false );
      }
      break;
  }
}

void v_canvas::mouseReleaseEvent( QMouseEvent * me )
{
  _moving_node = 0;
  _selectionFlag = false;
  repaint( false );
}

void v_canvas::mouseMoveEvent( QMouseEvent * me )
{
  if (_moving_node) {
    if ( _selectedNodes.lookup( _moving_node ) ) {
      int xdiff = (me->pos().x() - _moving_node->GetX());
      int ydiff = (me->pos().y() - _moving_node->GetY());

      list_item li;
      forall_items(li, _selectedNodes) {
	v_conduit * c = _selectedNodes.inf(li);
	c->SetX( c->GetX() + xdiff );
	c->SetY( c->GetY() + ydiff );
      }
    } else {
      _moving_node->SetX( me->pos().x() );
      _moving_node->SetY( me->pos().y() );
    }
    repaint( false );
  } else if ( _selectionFlag == true ) {
    int width  = me->pos().x() - _selectionStart.x();
    int height = me->pos().y() - _selectionStart.y();

    if (width <= 0)  width = 1;
    if (height <= 0) height = 1;

    _selectionBox.setRect( _selectionStart.x(), _selectionStart.y(), width, height );

    list_item li, nli;
    // add any nodes inside the box
    forall_items(li, *(_data->getNodes())) {
      v_conduit * c = _data->getNodes()->inf(li);
      QPoint p(c->GetX(), c->GetY());
      if ( _selectionBox.contains( p ) && ! _selectedNodes.lookup( c ) ) {
#if 0
	cout << c->GetName() << " " << c->GetX() << "," << c->GetY() << " is located within "
	     << _selectionStart.x() << "," << _selectionStart.y() << " and " 
	     << _selectionStart.x() + width << "," << _selectionStart.y() + height << endl;
#endif
	_selectedNodes.append( c );
      }
    }
    // remove any nodes outside of the box
    for (li = _selectedNodes.first(); li; li = nli) {
      nli = _selectedNodes.next(li);
      v_conduit * c = _selectedNodes.inf(li);
      QPoint p(c->GetX(), c->GetY());
      if (! _selectionBox.contains( p ) )
	_selectedNodes.del_item(li);
    }
    repaint( false );
  }
}

void v_canvas::keyPressEvent( QKeyEvent * ke )
{
  switch (ke->key()) {
    case Key_U:
    case Key_Shift: // for backwards compatibility
      _ungroupFlag = true;
      break;
    case Key_Escape:
    default:
      // remove all flags
      _ungroupFlag = _selectionFlag = false;
      break;
  }
}

void v_canvas::paintEvent( QPaintEvent * pe )
{
  //  cout << "v_canvas::paintEvent" << endl;
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

void v_canvas::draw( QPainter & p )
{
  // Obtain the list of nodes from _data and draw them
  list_item li;
  const list<v_conduit *> * conduits = _data->getNodes();
  if (conduits && conduits->empty() == false) {

    // First fix up the Conduits
    forall_items(li, *conduits) {
      v_conduit * c = conduits->inf(li);
      calculateNodePos(c);
    }
    // Then draw all the edges
    forall_items(li, *conduits) {
      v_conduit * c = conduits->inf(li);
      const list<Adjacency *> * adjs = c->GetNeighbors();
      if (adjs && adjs->empty() == false) {
	list_item nli;
	forall_items(nli, *adjs) {
	  const v_conduit * n = adjs->inf(nli)->GetNeighbor();
	  draw_edge(p, c, n);
	}
      }
    }
    // Then draw all the conduits
    forall_items(li, *conduits) {
      v_conduit * c = conduits->inf(li);
      draw_node(p, c);
    }
    draw_visitors(p);
  
    // Then draw the selection box if applicable
    if ( _selectionFlag == true )
      p.drawRect( _selectionBox );
  }
}

void v_canvas::draw_node( QPainter & p, v_conduit * c )
{
  if (c->IsVisible()) {
    // Just to be safe ...
    calculateNodePos(c);    

    const char * name = c->GetName();
    if (strchr(name, '.')) {
      name = name + strlen(name) - 1;
      while (*(name - 1) != '.')
	name--;
    }

    QPixmap * pix = c->GetPixmap();
    if ( _selectedNodes.lookup( c ) )
      pix = c->GetSelectedPixmap();

    assert( pix != 0 );
    int x = c->GetX() + OFFSET;
    int y = c->GetY() + OFFSET;
    int width  = p.fontMetrics().width(name);
    int height = p.fontMetrics().height() + 3;
    int pixwidth  = pix->width();
    int pixheight = pix->height();

    int angle = 0;
    if (!strcmp(c->GetType(), "Mux"))
      angle = c->orientation();
    
    if (angle) {
      p.translate( x, y );
      p.rotate(angle);
      p.translate(-pixwidth / 2, -pixheight / 2);
    } else
      p.translate( x  - pixwidth / 2, y - pixheight / 2); 

    p.drawPixmap(0, 0, *pix);

    if (angle) { // Rotate back for the text
      p.translate(pixwidth / 2, pixheight / 2);
      p.rotate(-angle);
      p.translate(-x, -y);
    } else
      p.translate(-(x - pixwidth / 2), -(y - pixheight / 2));

    p.drawText( x - width / 2, y + height + pixheight / 2, name, strlen(name) );
  }
}

void v_canvas::draw_edge(QPainter & p, const v_conduit * s, const v_conduit * e)
{
  if (!(s == e || !s->IsVisible() || !e->IsVisible())) {
    int x1 = s->GetX() + OFFSET;
    int y1 = s->GetY() + OFFSET;
    int x2 = e->GetX() + OFFSET;
    int y2 = e->GetY() + OFFSET;
    p.drawLine( x1, y1, x2, y2 );
  }
}

void v_canvas::draw_visitors(QPainter & p)
{
  // Obtain the list of visitors from _data and draw them
  const list<v_visitor *> * visitors = theApplication().centralWidget()->visitors();
  if (visitors && visitors->empty() == false) {
    list_item li;
    forall_items(li, *visitors) {
      v_visitor * v = visitors->inf(li);
      v_conduit * c = v->CurrentLocation();
      if (c && c->IsVisible()) {
	// draw the Visitor at the conduit;
	int x = c->GetX() + OFFSET;
	int y = c->GetY() + OFFSET;
	int rank = c->GetRank(v);
	
	int PenWidth = 7;
	int box_width = 27 + (rank * (2 * PenWidth));
	if (!strcmp("Mux", c->GetType()))
	  box_width = 46 + (rank * (2 * PenWidth));

	QPen oldPen = p.pen();
	QPen newPen( VisitorColorsTable()->lookup_visitor_color(v->GetType()) );
	newPen.setWidth( PenWidth );
	p.setPen(newPen);

	p.drawEllipse( x - box_width / 2, y - box_width / 2,
		       box_width, box_width );

	p.setPen(oldPen);

	p.drawEllipse( (x - box_width / 2) - PenWidth / 2 + 1,
		       (y - box_width / 2) - PenWidth / 2 + 1,
		       box_width + PenWidth - 2, box_width + PenWidth - 2);
      }
    }
  }
}

void v_canvas::calculateNodePos(v_conduit * c) const
{
  int x = c->GetX(), y = c->GetY();

  // The position will only be set if the X or Y values are -1
  if ((x == -1) || (y == -1)) {
    double side = sqrt( (1 + ConduitRegistry()->GetConduits()->size()) ),
             Sx = width() / side,
             Sy = height() / side;
    int rank = ConduitRegistry()->GetConduits()->rank((char *)c->GetName());
    
    x = (int)((rank % (int)side) * Sx);
    y = (int)((rank / (int)side) * Sy);

    if ( x > width() - BORDER_WIDTH )
      x = width() - BORDER_WIDTH;
    else if ( x < BORDER_WIDTH )
      x += BORDER_WIDTH;
    if ( y > height() - BORDER_WIDTH )
      y = height() - BORDER_WIDTH;
    else if ( y < BORDER_WIDTH )
      y += BORDER_WIDTH;

    c->SetX(x);
    c->SetY(y);
  }
}

void v_canvas::display_node_data(v_conduit * c) const
{
  assert( c != 0 );

  double x = c->GetX();
  double y = c->GetY();
  
  QString text;
  text += c->GetName();
  text += "\n";
  text += "position (";
  QString xps;
  xps.setNum(x, 'f', 2);
  QString yps;
  yps.setNum(y, 'f', 2);
  text += xps;
  text += ",";
  text += yps;
  text += ")\n";

  if (!strcmp(c->GetType(), "Mux")) {
    text += "\nOrientation is ";
    int angle = c->orientation();
    QString ori;
    ori.setNum(angle);
    text += ori;
    text += ".\n";
  }
  // List the neighbors
  const list<Adjacency *> * neighs = c->GetNeighbors();
  list_item li;
  forall_items(li, *neighs) {
    Adjacency  * adj = neighs->inf(li);
    const char * con_name = adj->GetNeighbor()->GetName();

    text += "Connected to ";
    text += con_name;
    text += " on the ";
    if (adj->GetSide() == v_conduit::A_SIDE)
      text += "A Side.\n";
    else if (adj->GetSide() == v_conduit::B_SIDE)
      text += "B Side.\n";
    else
      text += "Other Side.\n";
  }

  // List the visitors
  const list<v_visitor *> * inhabs = c->GetVisitors();
  if (!(inhabs->empty())) {
    text += "\nContains the following visitors:\n";
    int vf = 1;
    forall_items(li, *inhabs) {
      const char * vis_name = inhabs->inf(li)->GetName();
      QString vnum;
      vnum.setNum(vf++);
      text += vnum;
      text += " ";
      text += vis_name;
      text += "\n";  
    }
  } else
    text += "\nContains no visitors.\n";
  
  QMessageBox mb((QWidget *)this, "Conduit Information") ;
  mb.setText(text);
  mb.setButtonText("Dismiss");
  mb.show();
}
