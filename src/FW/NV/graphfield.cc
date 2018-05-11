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
static char const _graphfield_cc_rcsid_[] =
"$Id: graphfield.cc,v 1.1 1999/01/21 14:55:00 bilal Exp $";
#endif

#include <common/cprototypes.h>

#include "graphfield.h"
#include "NV_application.h"
#include "NV_conduit.h"
#include "NV_visitor.h"

// XPMs
#include "pixmaps/fileopen.xpm"
#include "pixmaps/fileprint.xpm"
#include "pixmaps/filesave.xpm"
#include "pixmaps/restart.xpm"
#include "pixmaps/capture_icon.xpm"
#include "pixmaps/step_icon.xpm"
#include "pixmaps/cont_icon.xpm"
#include "pixmaps/stop_icon.xpm"
#include "pixmaps/zoom_icon.xpm"
#include "pixmaps/unzoom_icon.xpm"
#include "pixmaps/layout_icon.xpm"

#include <qwhatsthis.h>
#include <qtoolbutton.h>
#include <qstatusbar.h> 

#include <FW/jiggle/Graph.h>
#include <FW/jiggle/CGwithRestarts.h>
#include <FW/jiggle/SteepestDescent.h>
#include <FW/jiggle/SpringChargeModel.h>
#include <FW/jiggle/Node.h>
#include <FW/jiggle/Edge.h>

#define  X(i) (i + _offset - _X_BP)
#define  Y(i) (i + _offset - _Y_BP)

#define  MUX_MOD  2.7
#define  ADA_MOD  1.5

#define  CONTINUE_INTERVAL   200
#define  COMPUTE_INTERVAL    100
#define  FORCE_ITERATIONS_PER_CALLBACK 5

#define  OPTIMAL_EDGE_LENGTH 100.0
#define  FORCE_THRESHOLD     0.5
// --------------------------------------------------------
GraphField::GraphField(QWidget * parent, const char * name)
  : QMainWindow(parent, name), _X_BP(0), _Y_BP(0), _Vist_flag(false),
    _Cont_flag(false), _Ungroup_flag(false), _Move_flag(false),
    _Zoom_flag(false), _No_change_flag(true), _Hbox_flag(false),
    _Hbox_ready(false), _Hbox_move(false), _active_visitor(0),
    _dirty(false), _screen_dump(false), _Jig_minX(-1), _Jig_minY(-1),
    _Jig_maxX(-1), _Jig_maxY(-1), _Cont_timer(-1), _Compute_timer(-1),
    _Debugging_mode(false), _zoom_depth(0)
{
  QPixmap openIcon    = QPixmap( fileopen ), 
          saveIcon    = QPixmap( filesave ), 
          printIcon   = QPixmap( fileprint ),
          restartIcon = QPixmap( restart_icon ),
          captureIcon = QPixmap( capture_icon ),
          stepIcon    = QPixmap( step_icon ),
          contIcon    = QPixmap( cont_icon ),
          stopIcon    = QPixmap( stop_icon ),
          zoomIcon    = QPixmap( zoom_icon ),
          unzoomIcon  = QPixmap( unzoom_icon ),
          layoutIcon  = QPixmap( layout_icon );

  setMinimumSize( GetApp()._X_min + 2 * _offset, 
		  GetApp()._Y_min + 2 * _offset);
  setMaximumSize( GetApp()._X_max + 2 * _offset, 
		  GetApp()._Y_max + 2 * _offset);
  setBackgroundColor("white");

  // Allocate the Menu Bar
  //  _menu = new QMenuBar( this );
  _menu = menuBar();
  assert(_menu);
  
#ifdef USE_TOOLBARS
  // Allocate the Tool Bar
  _tools = new QToolBar( this, "" );
  QToolButton * stept = new QToolButton(stepIcon, "Step", 0, this, 
				       SLOT(step()), _tools, 
				       "step simulation");
  QToolButton * contt = new QToolButton(contIcon, "Continue", 0, this,
					SLOT(cont()), _tools,
					"continue simulation");
  QToolButton * stopt = new QToolButton(stopIcon, "Stop", 0, this,
				       SLOT(stop()), _tools,
				       "stop simulation");
  QToolButton * zoomt = new QToolButton(zoomIcon, "Zoom", 0, this,
				       SLOT(zoom()), _tools, "zoom");
  QToolButton * unzoomt = new QToolButton(unzoomIcon, "UnZoom", 0, this,
					 SLOT(unzoom()), _tools, "unzoom");
  QToolButton * layoutt = new QToolButton(layoutIcon, "Layout", 0, this,
					 SLOT(layout()), _tools,
					 "rearrange nodes");
  QWhatsThis::whatsThisButton( _tools );
  // Allocates Buttons and Tooltips for Tool Bar
  QWhatsThis::add( stept,   "Click this button to process one simulation event.\n", FALSE );
  QWhatsThis::add( contt,   "Click this button to continue running the simulation.\n", FALSE );
  QWhatsThis::add( stopt,   "Click this button to stop the simulation.\n", FALSE );     
  QWhatsThis::add( zoomt,   "Click this button to zoom in.\n",  FALSE );
  QWhatsThis::add( unzoomt, "Click this button to zoom out.\n", FALSE );
  QWhatsThis::add( layoutt, "Click this button to reorganize the nodes in the graph.\n", FALSE );
#else
  QPopupMenu * control = new QPopupMenu( );
  assert( control != 0 );
  control->insertItem(stepIcon,   "Step",     this, SLOT(step()) ,   Key_Space   );
  control->insertItem(contIcon,   "Continue", this, SLOT(cont()) ,   CTRL + Key_C);
  control->insertItem(stopIcon,   "Stop",     this, SLOT(stop()) ,   CTRL + Key_T);
  control->insertItem(restartIcon,"Restart",  this, SLOT(restart()), CTRL + Key_R);
  control->insertItem(zoomIcon,   "Zoom In",  this, SLOT(zoom()) ,   CTRL + Key_Z);
  control->insertItem(unzoomIcon, "Zoom Out", this, SLOT(unzoom()),  CTRL + Key_U);
  control->insertItem(layoutIcon, "Layout",   this, SLOT(layout()),  CTRL + Key_L);
#endif

  // Allocate the File Menu
  QPopupMenu * file = new QPopupMenu();
  assert(file);
  file->insertItem(openIcon, "Open",        this, SLOT(open_data()),   CTRL + Key_O);
  file->insertItem(openIcon, "Load layout", this, SLOT(load_layout()), CTRL + Key_L);
  file->insertItem(saveIcon, "Save layout", this, SLOT(save_layout()), CTRL + Key_S);
  file->insertSeparator();
  file->insertItem(printIcon, "Print",      this, SLOT(print()), CTRL+Key_P);
  file->insertSeparator();
  file->insertItem(captureIcon, "Screen capture",   this, SLOT(screen_cap()));
  file->insertSeparator();
  file->insertItem("Quit",        GetApp().GetQApplication(), SLOT(quit()), CTRL+Key_Q);
  // Notify the Menu Bar of the File Menu
  _menu->insertItem("&File",    file);
#if 0
  // Allocate the Edit Menu
  QPopupMenu * edit = new QPopupMenu();
  assert(edit);
  edit->setItemEnabled( edit->insertItem("Cut",   this, SLOT(cut())),   FALSE);
  edit->setItemEnabled( edit->insertItem("Copy",  this, SLOT(copy())),  FALSE);
  edit->setItemEnabled( edit->insertItem("Paste", this, SLOT(paste())), FALSE);
  // Notify the Menu Bar of the Edit Menu
  _menu->insertItem("&Edit",    edit);
#endif
#ifndef USE_TOOLBARS
  _menu->insertItem("&Control", control);
#endif

  _options = new QPopupMenu();
  assert(_options);
  _options->setCheckable( TRUE );
  _options->setItemChecked( _toolBar = _options->insertItem( "Tool Bar",   this, SLOT(toggleToolBar()) ),   
			   TRUE );
  _options->setItemChecked( _statusBar = _options->insertItem( "Status Bar", this, SLOT(toggleStatusBar()) ), 
			   TRUE );
  // Notify the Menu Bar of the Options Menu
  _menu->insertItem("&Options", _options);

  QPopupMenu * help = new QPopupMenu();
  assert(help);
  help->insertItem("Index", this, SLOT(show_help()), CTRL+Key_H);
  help->insertItem("About", this, SLOT(show_about()));
  // Notify the Menu Bar of the Help Menu
  _menu->insertItem("&Help",    help);

  _hbox = new QRect();
  
  connect( GetApp()._visitorWin, SIGNAL(selected(int)), SLOT(highlight_visitor(int)));
  connect( this, SIGNAL(time_event(double)), SLOT(display_time(double)));
  connect( this, SIGNAL(compute_event( )), SLOT(compute_force( )));
  
  _hBar = new QScrollBar(GetApp()._Pix_x_min,                    // min value
			 GetApp()._Pix_x_max - GetApp()._X_max,  // max value
			 10, 100,                                // line/page steps
			 0,                                      // inital value
			 QScrollBar::Horizontal,                 // orientation
			 this, "scrollbar_h" );                  // parent, name
  
  _hBar->setGeometry( _offset - GetApp()._SB_WIDTH,                     // x
		      GetApp()._Y_max + GetApp()._SB_WIDTH + _offset ,  // y
		      GetApp()._X_max + 2 * GetApp()._SB_WIDTH ,        // width
		      GetApp()._SB_WIDTH );                             // height
  
  
  _vBar = new QScrollBar(GetApp()._Pix_y_min,                   // min value
			 GetApp()._Pix_y_max - GetApp()._Y_max, // max value
			 10, 100,                               // line/page steps
			 0,                                     // inital value
			 QScrollBar::Vertical,                  // orientation
			 this, "scrollbar_v" );
  
  _vBar->setGeometry( GetApp()._X_max + GetApp()._SB_WIDTH + _offset, // x
		      _offset - GetApp()._SB_WIDTH,                   // y
		      GetApp()._SB_WIDTH,                             // width
		      GetApp()._X_max );                              // height

  // Scrolling
  connect( _vBar, SIGNAL(valueChanged(int)), SLOT(scroll_v_handler(int)));
  connect( _hBar, SIGNAL(valueChanged(int)), SLOT(scroll_h_handler(int)));

  QPainter p;
  QBrush brush(NoBrush);
  QPen   pen(black);
  p.begin(this);
  p.setBrush(brush);
  p.setPen(pen);
  draw_frame(p);
  p.end();
  
  GetApp()._visitorWin->setMaximumSize(260,(int)((GetApp()._Y_max + 2 * 
						  _offset -
						  GetApp()._HCLCK>300) ?
						 ((GetApp()._Y_max + 2 * 
						   _offset - 
						   GetApp()._HCLCK)/3.0):100));
  GetApp()._visitorWin->setMinimumSize(260,(int)((GetApp()._Y_max + 2 *
						  _offset - 
						  GetApp()._HCLCK>300) ?
						 ((GetApp()._Y_max + 2 * 
						   _offset -
						   GetApp()._HCLCK)/3.0):100));
  
  GetApp()._visitorTab->setMinimumSize(260,(int)((GetApp()._Y_max + 2 * 
						  _offset -
						  GetApp()._HCLCK > 300) ?
						 (2.0 * (GetApp()._Y_max + 2 *
						       _offset - 
						       GetApp()._HCLCK)/3.0):100));
  GetApp()._visitorTab->setMaximumSize(260,(int)((GetApp()._Y_max + 2 * 
						  _offset -
						  GetApp()._HCLCK>300) ?
						 (2.0 * (GetApp()._Y_max + 2 * 
							 _offset -
							 GetApp()._HCLCK)/3.0):100));

  GetApp()._visitorTab->setBackgroundColor(white);
  
  GetApp()._clockWin->setMaximumSize(260, GetApp()._HCLCK);
  GetApp()._clockWin->setMinimumSize(260, GetApp()._HCLCK);
  GetApp()._clockWin->setFont( QFont( "Times", 24, QFont::Bold ) );

  const char * layout = GetApp()._layout;
  if (layout && *layout)
    load_layout_file(GetApp()._layout);
}


GraphField::~GraphField()
{
  delete _menu;
  delete _hBar;
  delete _vBar;
  delete _hbox;
}

void GraphField::scroll_v_handler(int i)
{
  _Y_BP = i;
  if (!_Zoom_flag) 
    repaint(FALSE);
}

void GraphField::scroll_h_handler(int i)
{
  _X_BP = i;
  if (!_Zoom_flag) 
    repaint(FALSE);
}

// ------------------ Control Menu -------------------
void GraphField::cont( )
{
  if (!_Cont_flag) {
    _Cont_flag = true;
    _Cont_timer = startTimer( CONTINUE_INTERVAL );
  } 
}

void GraphField::stop( )
{
  if (_Cont_flag) {
    _Cont_flag = false;
    killTimers( ); // _Cont_timer ); // I guess when you hit stop you want everything to stop
  }
}

void GraphField::step( )
{
  bool continuous = false;
  // parse a line and redraw
  if (!GetApp().ProcessInput(continuous)) {
    _Cont_flag = false;
    killTimer( _Cont_timer );
  }
}

void GraphField::layout( )
{
  _Compute_timer = startTimer( COMPUTE_INTERVAL );
}

void GraphField::toggleToolBar( )
{
#ifdef USE_TOOLBARS
  if ( _tools->isVisible() ) {
    _tools->hide();
    _options->setItemChecked( _toolBar, FALSE );
  } else {
    _tools->show();
    _options->setItemChecked( _toolBar, TRUE );
  }
#endif
}

void GraphField::toggleStatusBar( )
{
  if ( statusBar()->isVisible() ) {
    statusBar()->hide();
    _options->setItemChecked( _statusBar, FALSE );
  } else {
    statusBar()->show();
    _options->setItemChecked( _statusBar, TRUE );
  }
}


#define ZOOM_IN_FACTOR  2.0
#define ZOOM_OUT_FACTOR 0.5
#define SMOOTH_FACTOR   10.0

void GraphField::zoom( )
{
  if (GetApp()._Pix_x_max > (GetApp()._X_max * 4)) {
    QString text;
    text += "\nYou have reached your zooming limit.\n";
    QMessageBox mb(this, "Unable to comply") ;
    mb.setText(text);
    mb.setButtonText("Dismiss");
    mb.show();
    return;  // we've reached the maximum extent
  }
  _Zoom_flag = true;
  zoomer(ZOOM_IN_FACTOR);
  _Zoom_flag = false;
}

void GraphField::unzoom( )
{
  if (GetApp()._Pix_x_max <= GetApp()._X_max) {
    QString text;
    text += "\nYou have reached your zooming limit.\n";
    QMessageBox mb(this, "Unable to comply") ;
    mb.setText(text);
    mb.setButtonText("Dismiss");
    mb.show();
    return; // we've reached the minimum extent
  }
  _Zoom_flag = true;
  zoomer(ZOOM_OUT_FACTOR);
  _Zoom_flag = false;
}

void GraphField::toggle( )
{
  _Ungroup_flag = true;
}

void GraphField::test( )
{
  QFileDialog fd(this, "Output Filename");
  QString filename = fd.getSaveFileName();
  if (!filename.isNull() && !filename.isEmpty()) {
    ofstream out(filename);

    const dictionary<char *, NV_conduit *> * cons = 
      ConduitRegistry()->GetConduits();
    
    dic_item di;
    forall_items(di, *cons) {
      NV_conduit * nc = cons->inf(di);
    
      if (nc->GetChildren()->empty() == false) {
	out << nc->GetName() << " " << nc->GetType() << " : Children" << endl;
	
	list_item li;
	forall_items(li, *(nc->GetChildren())) {
	  NV_conduit * child = nc->GetChildren()->inf(li);
	  out << " - " << child->GetName() << " " << child->GetType() << endl;
	}
	out << "-------------------------------------" << endl;
      }
    }
  }
}

// ------------------ Print Menu -------------------
void GraphField::print( )
{
  QPrinter * printer = new QPrinter;
  if (printer->setup(this)) {
    QPainter paint;
    paint.begin(printer);
    paint.setViewport( 0, 0, 350, 650 );
    draw_graph( paint );
    paint.end();
  }
}

void GraphField::screen_cap( )
{
  QPixmap pm(size());
  QFileDialog fd(this, "Output Filename");
  QString filename = fd.getSaveFileName();
  if (!filename.isNull() && !filename.isEmpty())
    draw_to_pixmap(pm, filename);
}

// ------------------ File Menu -------------------
void GraphField::open_data( )
{
  // Query user for new data file
  QString filename = QFileDialog::getOpenFileName( 0, "*.log", this );

  if (!filename.isNull() && !filename.isEmpty()) {
    // reset the simulation
    restart();
    clear_layouts();

    // notify NV_application of the new file
    GetApp().SetInputFile(filename);
  }
}

void GraphField::load_layout( )
{
  // query user for the file name, 
  QFileDialog fd(this, "Layout Filename");
  QString filename = fd.getOpenFileName();

  if (!filename.isNull() && !filename.isEmpty()) {
    // remove any old layout information
    clear_layouts();

    // load up the file
    load_layout_file(filename);
    
    // iterate over all currently instantiated NV_conduits and set their position.
    const dictionary<char *, NV_conduit *> * cons = 
      ConduitRegistry()->GetConduits();
    
    dic_item di;
    forall_items(di, *cons) {
      NV_conduit * nc = cons->inf(di);
      if (di = _layouts.lookup(ExtractName(nc->GetName()))) {
	LayoutContainer lc( _layouts.inf(di) );
	nc->SetX(lc._x);
	nc->SetY(lc._y);
      }
    }
  }
}

void GraphField::save_layout( )
{
  QFileDialog fd(this, "Layout Filename");
  QString filename = fd.getSaveFileName();

  // Save all the conduit locations to the file
  if (!filename.isNull() && !filename.isEmpty())
    ConduitRegistry()->SaveLayout(filename);
}

void GraphField::restart( )
{
  GetApp().Restart();
  clear_layouts();
  GetApp().SetInputFile();
  repaint(FALSE);
}

// --------------------- Edit Menu -----------------
void GraphField::cut( ) { }

void GraphField::copy( ) { }

void GraphField::paste( ) { }

// --------------------- Debug Menu -----------------
#ifndef TROTTA
#endif

// ------------------ File Menu -------------------
void GraphField::show_help( )
{
  QString text;
  text += "\nKeyboard Shortcuts\n\n";
  text += "Open new data file - Control+O\n";
  text += "Load new layout file - Control+L\n";
  text += "Save layout file - Control+S\n";
  text += "Quit - Control+Q\n";
  text += "\n";
  text += "Step - Control+T\n";
  text += "Continue - Control+C\n";
  text += "Stop - Control+P\n";
  text += "Zoom - Control+Z\n";
  text += "Un-Zoom - Control+U";
  text += "Layout - Control+A";
  text += "\n";
  text += "Help - Control+H\n";
  text += "\n\n";
  text += "Pressing the Shift key will toggle the ungrouping mode of the right mouse button.\n";

  QMessageBox mb(this, "Help") ;
  mb.setText(text);
  mb.setButtonText("Dismiss");
  mb.show();
}

void GraphField::show_about( )
{
  QString text;
  text += "\n\n";
  text += "The New Visualizer\n\n";
  text += "Designer:                   Bilal Khan\n";
  text += "Asst. Designer/Implementor: Sean Mountcastle\n";
  text += "Implementation Assistance:  Jack Marsh\n";

  QMessageBox mb(this, "About") ;
  mb.setText(text);
  mb.setButtonText("Dismiss");
  mb.show();
}

// -------------------------------------------------
void GraphField::draw_frame(QPainter & p)
{
  QBrush save_brush = p.brush();
  QBrush brush(white);
  QBrush bg(black);
  QPen   save_pen = p.pen();
  QPen   pen(black);
  
  p.setBrush(bg);
  p.setPen(pen);
  p.drawRect(GetApp()._X_min, GetApp()._Y_min, 
	     GetApp()._X_max + 2 * _offset, 
	     GetApp()._Y_max + 2 * _offset);
  p.setBrush(brush);
  p.drawRect(_offset - _node_width, _offset - _node_width, 
	     GetApp()._X_max + 2 * _node_width, 
	     GetApp()._Y_max + 2 * _node_width);
  p.setBrush(save_brush);
  p.setPen(save_pen);
}

void GraphField::timerEvent(QTimerEvent * e)
{
  // If we are continuing, then poll 'bilals_loop' at regular intervals
  bool continuous = true;

  if (e->timerId() == _Cont_timer) {
    if (_Cont_flag) {
      _Cont_flag = GetApp().ProcessInput(continuous);
      killTimer( _Cont_timer );   
      _Cont_timer = startTimer( CONTINUE_INTERVAL );
    }
  } else if (e->timerId() == _Compute_timer) {
    killTimer( _Compute_timer );
    emit compute_event( );
    // compute_force decides whether or not to register the timer again
  } else
    killTimers();    
}

void GraphField::paintEvent(QPaintEvent * e)
{
  if (!_dirty)
    return;

  QPixmap frame(size());
  QPixmap meat(size());
  
  QPainter thePainter;
  thePainter.begin(this);
  
  QPainter paintersAssistant;
  QBrush   brush(blue);
  QPen     pen(black);
  QPoint   pt(0, 0);
  meat.fill(this, pt);
  
  paintersAssistant.begin(&frame);
  paintersAssistant.setBrush(brush);
  paintersAssistant.setPen(pen);
  draw_frame(paintersAssistant);
  paintersAssistant.end();
  
  paintersAssistant.begin(&meat);
  paintersAssistant.setBrush(brush);
  paintersAssistant.setPen(pen);
  draw_graph(paintersAssistant);
  // this should draw the last time ..
  display_time(-1.0);
  paintersAssistant.end();
  
  bitBlt(&frame, _offset - _node_width, _offset - _node_width, 
         &meat, _offset - _node_width, _offset - _node_width, 
         GetApp()._X_max + 2 * _node_width, 
	 GetApp()._Y_max + 2 * _node_width, CopyROP);
  
  thePainter.drawPixmap(pt, frame);
  thePainter.end();
  
  // the block below is for capturing screens
  if (_screen_dump) {
    static int iter = 0;
    QPixmap pm(size());
    char buf[256];
    sprintf(buf, "vis/NV-%03d.xpm", iter++);
    draw_to_pixmap(pm, buf);
  }
  // This needs to be fixed ... for repainting when revealed
  //  _dirty = false;
}

void GraphField::draw_to_pixmap(QPixmap & pm, const char * filename)
{
  QPainter painter;
  QBrush   brush(blue);
  QPen     pen(black);
  QPoint   pt(0, 0);
  pm.fill(this, pt);
  
  painter.begin(&pm);
  painter.setBrush(brush);
  painter.setPen(pen);
  draw_frame(painter);
  painter.end();
  
  painter.begin(&pm);
  painter.setBrush(brush);
  painter.setPen(pen);
  draw_graph(painter);
  painter.end();

  if (filename)
    pm.save(filename, "XPM");
}

// This method is called when the mouse button is released.
void GraphField::mouseReleaseEvent(QMouseEvent * e)
{
  if (!_hbox_nodes.empty()) {
    _Hbox_ready = true;
  } else {
    _Hbox_move = false;
    _Hbox_flag = false;
    _Hbox_ready = false;
    _hbox->setRect(0,0,0,0);
    SetDirty();
  }
  repaint(FALSE);
}

// This method is called when the mouse is moved.
void GraphField::mouseMoveEvent(QMouseEvent * e)
{
  int MouseX = (e->pos().x() - _offset + _X_BP);
  int MouseY = (e->pos().y() - _offset + _Y_BP);
  
  if (MouseX > GetApp()._Pix_x_max)
    MouseX = GetApp()._Pix_x_max;
  if (MouseX < GetApp()._Pix_x_min)
    MouseX = GetApp()._Pix_x_min;
  
  if (MouseY > GetApp()._Pix_y_max)
    MouseY = GetApp()._Pix_y_max;
  if (MouseY < GetApp()._Pix_y_min)
    MouseY = GetApp()._Pix_y_min;
  
  if (_Move_flag) {
    int x_diff = (MouseX - _moved_node->GetX()),
        y_diff = (MouseY - _moved_node->GetY());
    _moved_node->SetX(MouseX); _moved_node->_secret_x = -1;
    _moved_node->SetY(MouseY); _moved_node->_secret_y = -1;
    if (ConduitRegistry()->GetFlags() & NV_conduit_registry::MoveFreezesConduit)
      _moved_node->SetTouched();
    SetDirty();

    if (ConduitRegistry()->GetFlags() & NV_conduit_registry::MoveAffectsChildren) {
      // Move the children by the same amount
      const list<NV_conduit *> * children = _moved_node->GetChildren();
      list_item li;
      forall_items(li, *children) {
	NV_conduit * child = children->inf(li);
	child->SetX( x_diff + child->GetX() ); child->_secret_x = -1;
	child->SetY( y_diff + child->GetY() ); child->_secret_y = -1;
      }
    }
  } else if (_Hbox_flag && !_Hbox_move) {
    MouseX += _offset - _X_BP;
    MouseY += _offset - _Y_BP;
    int width  = MouseX - _hbox_pt.x();
    int height = MouseY - _hbox_pt.y();
    
    if (width <= 0)
      width = 1;
    if (height <= 0)
      height = 1;

    _hbox->setRect(_hbox_pt.x(), _hbox_pt.y(), width, height);

    // Add all the nodes within the rectangle to the _hbox_nodes
    const dictionary<char *, NV_conduit *> * cons = 
      ConduitRegistry()->GetConduits();
    dic_item di;
    forall_items(di, *cons) {
      NV_conduit * nc = cons->inf(di);
      QPoint qpt(nc->GetX() + _offset - _X_BP,
		 nc->GetY() + _offset - _Y_BP);
      if (_hbox->contains(qpt)) {
	if (!_hbox_nodes.lookup(nc))
	  _hbox_nodes.append(nc);
      }
    }
    // Make to remove any nodes outside of the box
    list_item li, nli;
    for ( li = _hbox_nodes.first(); li != 0; li = nli ) {
      nli = _hbox_nodes.next(li);

      NV_conduit * nc = _hbox_nodes.inf(li);
      QPoint qpt(nc->GetX() + _offset - _X_BP,
		 nc->GetY() + _offset - _Y_BP);
      if (!_hbox->contains(qpt))
	_hbox_nodes.del_item(li);
    } 
    SetDirty();
  } else if (_Hbox_flag && _Hbox_move) {
    // Iterate over all the nodes within the box
    //   and move them the same distance as the moved 
    //   node.
    int x_diff = MouseX - _moved_node->GetX(),
        y_diff = MouseY - _moved_node->GetY();

    list_item li;
    forall_items(li, _hbox_nodes) {
      NV_conduit * nc = _hbox_nodes.inf(li);
      nc->SetX(nc->GetX() + x_diff); nc->_secret_x = -1;
      nc->SetY(nc->GetY() + y_diff); nc->_secret_y = -1;
    }
    _hbox_pt.setX( _hbox_pt.x() + x_diff );
    _hbox_pt.setY( _hbox_pt.y() + y_diff );
    _hbox->moveTopLeft(_hbox_pt);
    _hbox->setRect(_hbox_pt.x(), _hbox_pt.y(), 
                   _hbox->width(), _hbox->height());
    SetDirty();
  }
  repaint(FALSE);
}

// This method is called when the mouse button is pressed, 
//   e->button() returns the specific button pressed. 
void GraphField::mousePressEvent(QMouseEvent * e)
{
  double x = (e->pos().x() - _offset + _X_BP);
  double y = (e->pos().y() - _offset + _Y_BP);
  QPoint pntoff = QPoint((int)x, (int)y);

  // This needs to appear here
  SetDirty();

  switch (e->button()) {
    case MidButton: 
      {
        _Move_flag = _Hbox_flag = 
	  _Hbox_move = _Hbox_ready = false;
        _hbox->setRect(0, 0, 0, 0);
        _hbox_nodes.clear();
	
	_clicked_conduit = find_node(pntoff);
	if (_clicked_conduit) {
	  if (_Debugging_mode) {
#ifdef TROTTA
	    if (!_group)
	      window_to_kdb();
	    else
	      _group->show();
#endif
	    _Debugging_mode = false;
	  } else
	    display_node_data( _clicked_conduit );
	}
      }
      break;
    case LeftButton:
      {
	double min_dist = 20.0;
	if (!(_moved_node = nearest_node(pntoff, min_dist)) ||
	    _Hbox_flag) {
	  if (_hbox_nodes.empty() && !_Hbox_ready) {
	    _Hbox_flag = true;
	    //case where we first start the rectangle
	    _hbox->setRect(0, 0, 0, 0);
	    x += _offset - _X_BP;
	    y += _offset - _Y_BP;
	    _hbox_pt = QPoint((int)x,(int)y);
	  } else if (!_hbox_nodes.empty()) {
	    NV_conduit * v = find_node(pntoff);
	    if (_hbox_nodes.search(v)) {
	      // case where we want to move enclosed block of nodes
	      _Hbox_move = true;
	      _moved_node = v;
	    } else {
	      _Hbox_flag = false;
	      _Hbox_move = false;
	      _Hbox_ready = false;
	      _hbox->setRect(0, 0, 0, 0);
	      _hbox_nodes.clear();
	    }
	  }
	  _Move_flag = false;
        } else {
	  // move the node around
          _Hbox_flag = false;
          _Hbox_move = false;
          _Hbox_ready = false;
          _hbox->setRect(0, 0, 0, 0);
          _hbox_nodes.clear();
          // continue on moving individual nodes...
          _Move_flag = true;
	}
      }
      break;
    case RightButton:
      {
        _Move_flag = _Hbox_flag = 
	  _Hbox_move = _Hbox_ready = false;
        _hbox->setRect(0, 0, 0, 0);
        _hbox_nodes.clear();
	
	bool changed = false;
        NV_conduit * v = find_node(pntoff);
        if (v && ((_Ungroup_flag == true))) // || (! v->GetParent())))
	  changed = expand_cluster(v);
	else if (v)
	  changed = collapse_containing_cluster(v);
	
	if (changed) {
	  repaint(FALSE);
	}
      }
      break;
    default:
      // ERROR!
      break;
  }
  _Ungroup_flag = false;
}

// Called when a key is pressed (used in conjunction with mouse buttons)
void GraphField::keyPressEvent(QKeyEvent * e)
{
  switch (e->key()) {
    case Key_Shift:
      _Ungroup_flag = true;
      // control->insertItem("",         this, SLOT(toggle()), Key_Shift);
      break;
    case Key_Alt:
      _Debugging_mode = true; // works with middle mouse button to call Jen's code
      break;
    default:
      break;
  }
}

void GraphField::highlight_visitor(int i)
{
  const char * txt = GetApp()._visitorWin->text(i);
  char namepd[256];
  char dummy1[256];
  char dummy2[256];
  char dummy3[256];
  sscanf(txt, "%s %s %s %s", dummy1, dummy2, dummy3, namepd);

  char name[256];

  int strctr = 0;
  while (namepd[strctr] != '.') {
    name[strctr] = namepd[strctr];
    strctr++;
  }
  name[strctr] = '\0';
  // Find the conduit it resides in
  NV_conduit * v = ConduitRegistry()->query(name);
  v = v->HighestVisible();

  SetDirty();
  repaint(FALSE);
  
  QPainter p;
  // Draw the highlighting box
  p.begin(this);
  draw_highlighting_box(p, v);
  p.end(); 
}

void GraphField::display_time(double t)
{
  static double last_time = 0.0;

  if (t == -1.0)
    t = last_time;

  QPainter p;
  QPixmap  pix(GetApp()._clockWin->size());
  pix.fill(GetApp()._clockWin, 0, 0);

  char tme[255];
  sprintf(tme, "%lf", t);
  QPainter paintersAssistant;
  QPen     pen(black);
  QBrush   brush(white);
  paintersAssistant.begin(GetApp()._clockWin);
  paintersAssistant.setPen(pen);
  paintersAssistant.setBrush(brush);
  paintersAssistant.drawRect(15, 15, 230, 70);
  paintersAssistant.drawText(15, 15, 230, 70, AlignCenter, tme);
  paintersAssistant.end();
  last_time = t;
}

// draws all nodes and edges
void GraphField::draw_graph(QPainter & p)
{
  p.setRasterOp(CopyROP);  /* used to be OrROP */

  // Draw the Conduits
  const dictionary<char *, NV_conduit *> * cons = 
    ConduitRegistry()->GetConduits();

  dic_item di;
  // preprocess the conduits so the edges will be drawn correctly
  forall_items(di, *cons) {
    NV_conduit * nc = cons->inf(di);
    CalculateNodePos(nc);
  }
  // Draw all the edges
  forall_items(di, *cons) {
    NV_conduit * nc = cons->inf(di);
    const list<Adjacency> * neighs = nc->GetNeighbors();
    list_item li;
    forall_items(li, *neighs) {
      NV_conduit * nb = neighs->inf(li);
      draw_edge(p, nc, nb);
    }
  }
  // Draw all the conduits
  forall_items(di, *cons) {
    NV_conduit * nc = cons->inf(di);
    draw_node(p, nc);
  }

  // Draw the Visitor table
  draw_visitors(p);
  // Draw all of the Visitor tracking boxes
  track_all_visitors(p);
  // Draw the visitor color box
  draw_visitor_colors(p);

  // Draw any overlays
  if (_Hbox_flag) 
    draw_hbox(p);
}

void GraphField::draw_node(QPainter & p, NV_conduit * c)
{
  if (!c->IsVisible())
    return;  // not visible, so don't draw

  CalculateNodePos(c);
  int x = c->GetX(), y = c->GetY();

  QFontMetrics fm = p.fontMetrics();
  QPixmap * pix = c->GetPixmap();
  assert(pix);
  const char * name = c->GetName();
  if (strchr(name, '.')) {
    name = name + strlen(name) - 1;
    while (*(name - 1) != '.')
      name--;
  }

  int width = fm.width(name), height = fm.height() + 3,
      pxwidth = pix->width(), pxheight = pix->height();

  int angle = 0;
  if (!strcmp(c->GetType(), "Mux"))
    angle = c->orientation();

  if (angle) {
    p.translate(X(x), Y(y));
    p.rotate(angle);
    p.translate(-pxwidth/2, -pxheight/2);
  } else
    p.translate(X(x) - pxwidth/2, Y(y) - pxheight/2); 
  p.drawPixmap(0, 0, *(c->GetPixmap()));
  if (angle) { // Rotate back for the text
    p.translate(pxwidth/2, pxheight/2);
    p.rotate(-angle);
    p.translate(-X(x), -Y(y));
  } else
    p.translate(-(X(x) - pxwidth/2), -(Y(y) - pxheight/2)); 
  p.drawText(X(x) - width/2, Y(y) + height + pxheight/2, name, strlen(name));
}

void GraphField::CalculateNodePos(NV_conduit * c)
{
  int x = c->GetX(), y = c->GetY();

  // The position will only be set if the X or Y values are -1
  if ((x == -1) || (y == -1)) {
    dic_item di;
    if (di = _layouts.lookup(ExtractName(c->GetName()))) {
      LayoutContainer lc( _layouts.inf(di) );
      x = lc._x;
      y = lc._y;
      // Temporarily disable, causing SIGABORT - mountcas 8-4-98
      //      if (c->GetParent() || !(c->GetChildren()->empty()))
      //	c->SetVisible(lc._vis);
    } else {
      double side = sqrt( (1 + ConduitRegistry()->GetConduits()->size()) ),
	     Sx = (GetApp()._X_max - GetApp()._X_min) / side,
	     Sy = (GetApp()._Y_max - GetApp()._Y_min) / side;
      int rank = ConduitRegistry()->GetConduits()->rank((char *)c->GetName());

      x = (int)((rank % (int)side) * Sx);
      y = (int)((rank / (int)side) * Sy);
    }
    c->SetX(x);
    c->SetY(y);
  }
}

void GraphField::draw_edge(QPainter & p, NV_conduit * start, NV_conduit * end)
{
  if ((!start->IsVisible() || !end->IsVisible()) ||
      (start == end))
    return;

  // Save the current pen
  QPen save = p.pen();

  // Active pen crap
  //  QPen new_pen(red);
  //  new_pen.setWidth(15);
  //  p.setPen(new_pen);
  // End of active pen crap
  
  int x1 = start->GetX(), y1 = start->GetY();
  int x2 = end->GetX(), y2 = end->GetY();
  p.drawLine(X(x1), Y(y1), X(x2), Y(y2));
  // restore the original pen
  p.setPen(save);
}

void GraphField::draw_visitors(QPainter & p)
{
  // Initialize the Visitor location window
  if (!_Vist_flag)
    initialize_visitor_table();

  // Update the Visitor location window
  int active_index = -1;
  if (_active_visitor) {
    if (! VisitorRegistry()->visible(_active_visitor->GetType()))
      return;

    for (int i = 0; i < GetApp().GetVisitorWin()->count(); i++) {
      const char * txt = GetApp().GetVisitorWin()->text(i);
      char name[256], dummy[256];
      sscanf(txt, "%s %s is in %s.", dummy, name, dummy);
      if (!strcmp(_active_visitor->GetName(), name)) {
	active_index = i;
	break;
      }
    }
    QString text;
    if (_active_visitor->CurrentLocation() != 0) {
      text = "";
      text += _active_visitor->GetType();
      text += " ";
      text += _active_visitor->GetName();
      text += " is in ";
      text += _active_visitor->CurrentLocation()->GetName();
      text += ".";
    }

    // The active visitor is already in the table
    if (active_index > -1) {
      GetApp().GetVisitorWin()->removeItem(active_index);
      if (VisitorRegistry()->lookup(_active_visitor))
	GetApp().GetVisitorWin()->insertItem(text);
	// GetApp().GetVisitorWin()->changeItem(text, active_index);
    } else {
      // the active visitor has not yet entered the table
      GetApp().GetVisitorWin()->insertItem(text);
    }
    int loc = _active_visitor->CurrentLocation()->VisPresence(_active_visitor);
    QColor color = VisitorColorsTable()->lookup_visitor_color((char *)_active_visitor->GetType());
    if (loc)
      draw_visitor_tracking(p, color, _active_visitor, loc);
    _active_visitor = 0;
  }
}

void GraphField::initialize_visitor_table(void)
{
  QString text;

  const dictionary<char *, NV_visitor *> * viss =
    VisitorRegistry()->GetVisitors();
  dic_item di;
  forall_items(di, *viss) {
    NV_visitor * nv = viss->inf(di);

    if (nv->CurrentLocation() != 0) {
      text = "";
      text += nv->GetType();
      text += " ";
      text += nv->GetName();
      text += " is in ";
      text += nv->CurrentLocation()->GetName();
      text += ".";
      GetApp().GetVisitorWin()->insertItem(text, -1);
    }
  }
  _Vist_flag = true;
}

void GraphField::draw_visitor_colors(QPainter & p)
{
  int ycc = 70; int xcc = 50;

  QBrush savedbrush = p.brush();
  QPen   savedpen   = p.pen();
  
  QPainter p_vtab;
  QPainter p_vtab_pix;
  
  p_vtab.begin(GetApp().GetVisitorTab());
  QPixmap vtab_pix(GetApp().GetVisitorTab()->size());
  p_vtab_pix.begin(&vtab_pix);
  // Does this clear it?
  vtab_pix.fill(GetApp().GetVisitorTab(), 0, 0);
  
  int DX = 10, DY = 15;

  const dictionary<char *, QColor> * ctable = VisitorColorsTable()->color_table();
  dic_item di;
  forall_items(di, *ctable) {
    const char * name = ctable->key(di);

    // If we want to view all the visitor colors, don't make this check
    if (! (ConduitRegistry()->GetFlags() & NV_conduit_registry::ViewAllVisitorColors)) {
      if (! VisitorRegistry()->type_is_active(name))
	continue;
    }

    QColor color = ctable->inf(di);

    QBrush newbr(color);
    QPen   newpen(color);
    p_vtab_pix.setBrush(newbr);
    p_vtab_pix.setPen(newpen);
    p_vtab_pix.fillRect(xcc - DX, ycc - DY, 2 * DX, DY, color);
    p_vtab_pix.setBrush(NoBrush);
    QPen   outlinepen(black);
    p_vtab_pix.setPen(outlinepen);
    p_vtab_pix.drawText(xcc + 2 * DX, ycc, name, strlen(name));
    p_vtab_pix.drawRect(xcc - DX, ycc - DY, 2 * DX, DY);
    ycc += DY;
  }
  p_vtab_pix.end();
  p_vtab.drawPixmap(0, 0, vtab_pix);
  p_vtab.end();
  
  p.setBrush(savedbrush);
  p.setPen(savedpen);
}

void GraphField::draw_event_list(list<char *> & events)
{
  // draw the events list as a checkbox, so that someone can 
  // select an event and we can jump immediately to it. 
}

void GraphField::track_all_visitors(QPainter & p)
{
  const dictionary<char *, NV_visitor *> * viss =
    VisitorRegistry()->GetVisitors();
  dic_item di;
  forall_items(di, *viss) {
    NV_visitor * vis = viss->inf(di);

    if (! VisitorRegistry()->visible(vis->GetType()))
      return;

    NV_conduit * con = vis->CurrentLocation();

    if (con) {
      int    loc   = con->VisPresence(vis);
      QColor color = VisitorColorsTable()->lookup_visitor_color((char *)vis->GetType());
      draw_visitor_tracking(p, color, vis, loc);
    }
  }
}

void GraphField::draw_visitor_tracking(QPainter & p, QColor color, NV_visitor * vis, int loc)
{
  int PenWidth = 7;
  NV_conduit * nc = vis->CurrentLocation()->HighestVisible();

  if (!nc || !(nc->IsVisible())) 
    return;

  QPen   savedpen = p.pen();
  QBrush savedbrush = p.brush();
  QPen   newpen(color);
  newpen.setWidth( PenWidth );
  p.setPen(newpen);
  p.setBrush(NoBrush);

  double x = nc->GetX();
  double y = nc->GetY();

  int box_width = (int)((_node_width * ADA_MOD) + (loc - 1) * (2 * PenWidth)) + 3;
  if (!strcmp("Mux", nc->GetType()))
    box_width = (int)((_node_width * MUX_MOD) + (loc - 1) * (2 * PenWidth)) + 3;

  p.drawEllipse((int)(X(x) - box_width / 2), (int)(Y(y) - box_width / 2),
		box_width, box_width);

  QPen outlinepen(black);
  outlinepen.setWidth( 1 );
  p.setPen(outlinepen);
  p.setBrush(NoBrush);
  p.setRasterOp(CopyROP);

  p.drawEllipse((int)(X(x) - box_width / 2) - PenWidth / 2 + 1, 
		(int)(Y(y) - box_width / 2) - PenWidth / 2 + 1,
		box_width + PenWidth - 2, box_width + PenWidth - 2);
  
  p.setPen(savedpen);
  p.setBrush(savedbrush);
  p.setRasterOp(CopyROP);
}

void GraphField::draw_hbox(QPainter & p)
{
  if (!_Hbox_move) {
    QBrush savedbrush = p.brush();
    QPen   savedpen   = p.pen();
    QBrush newbrush(NoBrush);
    QPen   newpen(black);
    p.setBrush(newbrush);
    p.setPen(newpen);
    p.drawRect(*_hbox);
    p.setBrush(savedbrush);
    p.setPen(savedpen);
  }

  list_item li;
  forall_items(li, _hbox_nodes) {
    NV_conduit * nc = _hbox_nodes.inf(li);
    if (nc->IsVisible())
      draw_highlighting_box(p, nc);
  }
}

bool GraphField::continuing(void) const
{ return _Cont_flag; }

QPoint GraphField::find_center(void)
{
  int xcen = (GetApp()._Pix_x_max - GetApp()._Pix_x_min) / 2;
  int ycen = (GetApp()._Pix_y_max - GetApp()._Pix_y_min) / 2;
  QPoint cen(xcen, ycen);
  return (cen);
}

int quadrant(double x, double y, double x_cen, double y_cen) 
{
  //  cout << "(" << x << "," << y << ") (" << x_cen << "," << y_cen <<")." << endl;
  if (x >= x_cen && y >= y_cen) return (1);
  if (x <= x_cen && y >= y_cen) return (2);
  if (x <= x_cen && y <= y_cen) return (3);
  else return (4);
}

int quadrant(QPoint & p, QPoint & center) 
{
  return quadrant(p.x(), p.y(), center.x(), center.y());
}

double dr_log(double x, double base)
{
  return (log10(x)/log10(base));
}

NV_conduit * GraphField::nearest_node(QPoint & p, double & d_min)
{
  NV_conduit * rval = 0;
  if (d_min < 0)
    d_min = MAXINT;

  const dictionary<char *, NV_conduit *> * cons = 
    ConduitRegistry()->GetConduits();
  dic_item di;
  forall_items(di, *cons) {
    NV_conduit * tmp = cons->inf(di);
    if (!tmp->IsVisible()) 
      continue;
    QPoint q(tmp->GetX(), tmp->GetY());
    double dx = p.x() - q.x(), dy = p.y() - q.y();
    double dist = sqrt(dx * dx + dy * dy);
    if ((dist >= 0) && (dist < d_min)) {
      rval = tmp;
      d_min = dist;
    }
  }
  return rval;
}

NV_conduit * GraphField::find_node(QPoint & pnt)
{
  double d = -1.0;
  NV_conduit * rval = nearest_node(pnt, d);

  if (rval && d > (_node_width / 2))
    rval = 0;

  return rval;
}

void GraphField::display_node_data(NV_conduit * v)
{
  if (!v) return;

  double x = v->GetX();
  double y = v->GetY();
  
  QString text;
  text += v->GetName();
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

  if (!strcmp(v->GetType(), "Mux")) {
    text += "\nOrientation is ";
    int angle = v->orientation();
    QString ori;
    ori.setNum(angle);
    text += ori;
    text += ".\n";
  }

  // List the neighbors
  const list<Adjacency> * neighs = v->GetNeighbors();
  list_item li;
  forall_items(li, *neighs) {
    Adjacency adj = neighs->inf(li);
    const char * con_name = adj.GetNeighbor()->GetName();

    text += "Connected to ";
    text += con_name;
    text += " on the ";
    if (adj.GetSide() == NV_conduit::A_SIDE)
      text += "A Side.\n";
    else if (adj.GetSide() == NV_conduit::B_SIDE)
      text += "B Side.\n";
    else
      text += "Other Side.\n";
  }

  // List the visitors
  const list<NV_visitor *> * inhabs = v->GetVisitors();
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
  
  QMessageBox mb(this, "Conduit Information") ;
  mb.setText(text);
  mb.setButtonText("Dismiss");
  mb.show();
}

void  GraphField::draw_highlighting_box(QPainter & p, NV_conduit * v)
{
  QBrush   savedbrush = p.brush();
  QPen     savedpen   = p.pen();
  RasterOp savedrop   = p.rasterOp();
  
  QBrush   brush(NoBrush);
  QPen     pen(green);
  pen.setWidth(10);
  p.setBrush(brush);
  p.setPen(pen);
  p.setRasterOp(CopyROP);
  
  double x = v->GetX();
  double y = v->GetY();

  int box_width = (int)(_node_width * ADA_MOD);
  if (!strcmp(v->GetType(), "Mux"))
    box_width = (int)(_node_width * MUX_MOD);

  p.drawRect((int)(X(x) - box_width/2), (int)(Y(y) - box_width/2),
	     box_width, box_width);

  p.setPen(savedpen);
  p.setBrush(savedbrush);
  p.setRasterOp(savedrop);
}

void GraphField::emit_time(double t)
{
   emit time_event(t);
}

// load up the layout file
void GraphField::load_layout_file(const char * filename)
{
  ifstream input(filename);

  if (!input || input.bad())
    cerr << "ERROR: Unable to open '" << filename << "'" << endl;
  else {
    while (!input.eof()) {
      char line[1024];
      input.getline(line, 1024);
      if (!*line || *line == '#')
	continue;

      char name[255], vis = 'y';
      int x, y;
      
      // visibility is optional, that's why we check only if less than 3
      if (sscanf(line, "%s %d %d %c", name, &x, &y, &vis) < 3)
	cerr << "ERROR: Unable to parse '" << line << "'" << endl;
      else {
	if (x == -1 || y == -1)
	  continue;

	LayoutContainer lc(x, y, vis);
	if (strchr(name, '+'))
	  name[ strlen(name) - 1 ] = '\0';
	char * k = new char [ strlen(ExtractName(name)) + 1];
	strcpy(k, ExtractName(name));
	_layouts.insert(k, lc);
      }
    }
    // eof
  }
}

void GraphField::clear_layouts(void)
{
  dic_item di;
  forall_items(di, _layouts)
    delete _layouts.key(di);
  _layouts.clear();
}

LayoutContainer GraphField::find_layout(NV_conduit * c) const
{
  LayoutContainer rval;
  dic_item di;
  if (di = _layouts.lookup(ExtractName(c->GetName())))
    rval = _layouts.inf(di);
  
  return rval;
}

void GraphField::set_active_visitor(NV_visitor * v)
{
  _active_visitor = v;
}

double GraphField::GetX(NV_conduit * nc)
{
  double rval = nc->GetX();
  if (nc->_secret_x == -1 && _Jig_minX != -1 && _Jig_maxX != -1) {
    // scale the X accordingly
    double xdiff = _Jig_maxX - _Jig_minX;
    rval = ((rval - GetApp()._X_min) * xdiff/(GetApp()._X_max - GetApp()._X_min) + _Jig_minX);
  } else if (nc->_secret_x != -1)
    rval = nc->_secret_x;
  return rval;
}

double GraphField::GetY(NV_conduit * nc)
{
  double rval = nc->GetY();
  if (nc->_secret_y == -1 && _Jig_minY != -1 && _Jig_maxY != -1) {
    // scale the X accordingly
    double ydiff = _Jig_maxY - _Jig_minY;
    rval = ((rval - GetApp()._Y_min) * ydiff/(GetApp()._Y_max - GetApp()._Y_min) + _Jig_minY);
  } else if (nc->_secret_y != -1)
    rval = nc->_secret_y;
  return rval;
}

void GraphField::compute_force( void )
{
  list<NV_conduit *> cons;
  dic_item di;
  forall_items(di, *(ConduitRegistry()->GetConduits())) {
    NV_conduit * nc = (ConduitRegistry()->GetConduits())->inf(di);
    if (nc->IsVisible())
      cons.append(nc);
  }

  list_item li;

  // from JIGGLE
  Graph g;
  // build the graph
  forall_items(li, cons) {
    NV_conduit * nc = cons.inf(li);
    Node * v = g.insertNode(new Node(nc->GetName()));
    v->x( GetX(nc) );
    v->y( GetY(nc) );
    if (nc->Touched()) // Allow it to participate but don't move it
      v->fixed = true;
  }
  // insert all of the edges
  forall_items(li, cons) {
    NV_conduit * nc = cons.inf(li);

    int from = cons.rank(nc);
    const list<Adjacency> * neighbors = nc->GetNeighbors();
    list_item li2;
    forall_items(li2, *neighbors) {
      NV_conduit * neigh = neighbors->inf(li2).GetNeighbor();

      int to = -1;
      if (cons.lookup(neigh))
	to = cons.rank(neigh);

      if (from >= 0 && to >= 0)
	g.insertEdge(new Edge( g.nodes(from), g.nodes(to), false ));
    }
  }

  // the graph has been complete built now ...
  double optimalEdgeLen = OPTIMAL_EDGE_LENGTH;
  double initialStepSize = 0.1;
  CGwithRestarts opt(initialStepSize);
  // SteepestDescent opt(initialStepSize);  // Uncomment to try SteepestDescent
  SpringChargeModel model(optimalEdgeLen, Model::QUADRATIC, Model::INVERSE, true);

  double d = -1.0, last_d = 0.0;
  bool   done = false;
  int    i = FORCE_ITERATIONS_PER_CALLBACK;
  while (i-- > 0) {
    d = opt.improve(g, model);

    if (fabs(d - last_d) < FORCE_THRESHOLD) {
      done = true;
      break;
    }
    
    if (ConduitRegistry()->GetFlags() & NV_conduit_registry::ViewWhileContinue) {
      restore_nodes(cons, g);
      SetDirty();
      // view the result ...
      repaint(FALSE);
    }
    last_d = d;
  }

  if (!done)
    _Compute_timer = startTimer( COMPUTE_INTERVAL );

  restore_nodes(cons, g);
  SetDirty();
  // view the result ...
  repaint(FALSE);
}

void GraphField::restore_nodes(const list<NV_conduit *> & cons, Graph & g)
{
  _Jig_minX = g.nodes(0)->x();
  _Jig_minY = g.nodes(0)->y();
  _Jig_maxX = g.nodes(0)->x();
  _Jig_maxY = g.nodes(0)->y();

  // propagate the changes back into our data structure
  for (int i = 0; i < g.numberOfNodes; i++) {
    Node * v = g.nodes(i);
    
    if (v->x() < _Jig_minX)
      _Jig_minX = v->x();
    if (v->y() < _Jig_minY)
      _Jig_minY = v->y();

    if (v->x() > _Jig_maxX)
      _Jig_maxX = v->x();
    if (v->y() > _Jig_maxY)
      _Jig_maxY = v->y();
  }

  // now that we have the min and max values rescale them
  for (int i = 0; i < g.numberOfNodes; i++) {
    Node * v = g.nodes(i);

    int x = (int)v->x(), y = (int)v->y();
    // comment out the below block to try w/o scaling
    double xdiff = GetApp()._X_max - GetApp()._X_min;
    double ydiff = GetApp()._Y_max - GetApp()._Y_min;

    assert(xdiff && ydiff);

    x = (int)((v->x() - _Jig_minX) * xdiff/(_Jig_maxX - _Jig_minX) + GetApp()._X_min);
    y = (int)((v->y() - _Jig_minY) * ydiff/(_Jig_maxY - _Jig_minY) + GetApp()._Y_min);
    // comment out the above block to try w/o scaling
    cons[i]->SetX(x);
    cons[i]->SetY(y);
    cons[i]->_secret_x = v->x();
    cons[i]->_secret_y = v->y();
  }
}

void GraphField::zoomer(double factor)
{
  double S = GetApp()._Pix_x_max, E = (factor * GetApp()._Pix_x_max);
  double xzfact = exp( 1/SMOOTH_FACTOR * (log(E) - log(S)) ); // factor;
  S = GetApp()._Pix_y_max; E = (factor * S);
  double yzfact = exp( 1/SMOOTH_FACTOR * (log(E) - log(S)) ); // factor;
  int iterations = (int)SMOOTH_FACTOR;
  QPoint old_center = find_center();

  if (ConduitRegistry()->GetFlags() & NV_conduit_registry::ImmediateZoom) {
    iterations = 1;
    xzfact = yzfact = factor;
  }

  if (factor > 1)
    _zoom_depth++;
  else
    _zoom_depth--;

  for (int i = 0; i < iterations; i++) {
    old_center = find_center();

    // Min stays the same while max is halved or doubled
    GetApp()._Pix_x_max = (int)(GetApp()._Pix_x_max * xzfact);
    GetApp()._Pix_y_max = (int)(GetApp()._Pix_y_max * yzfact);
    
    zoom(old_center, xzfact, yzfact);

    // move this outside the loop if you want immediate zoom
    SetDirty();
    repaint(FALSE);
  }

  // correct for any trucation
  GetApp()._Pix_x_max = (int)(GetApp()._X_max * pow(2, _zoom_depth));
  GetApp()._Pix_y_max = (int)(GetApp()._Y_max * pow(2, _zoom_depth));

  QPoint new_cen = find_center();
  QPoint view_cen(_X_BP + (GetApp()._X_max - GetApp()._X_min) / 2,
		  _Y_BP + (GetApp()._Y_max - GetApp()._Y_min) / 2);

  QPoint new_view = adjust(xzfact, yzfact, old_center, new_cen, view_cen);
  
  int xbp = (new_view.x() - ((GetApp()._X_max - GetApp()._X_min) / 2));
  int ybp = (new_view.y() - ((GetApp()._Y_max - GetApp()._Y_min) / 2));

  // range is min to max, this should be fine ...
  _hBar->setRange( GetApp()._Pix_x_min, (GetApp()._Pix_x_max - GetApp()._X_max));
  _vBar->setRange( GetApp()._Pix_y_min, (GetApp()._Pix_y_max - GetApp()._Y_max));

  // This doesn't seem to matter since the values are always scaled between min and max
  _hBar->setValue( xbp );
  _vBar->setValue( ybp );
  
  _X_BP = xbp;
  _Y_BP = ybp;

  // This must be done outside as it takes much too long
  ConduitPixmapsTable()->resize_pixmaps( factor ); // , ConduitRegistry()->min_dist() / 3 * 2);

  SetDirty();
  repaint(FALSE);
}

void GraphField::zoom(QPoint & old_cen, double xzf, double yzf)
{
  QPoint new_cen = find_center();

  const dictionary<char *, NV_conduit *> * cons = 
    ConduitRegistry()->GetConduits();

  dic_item di;
  forall_items(di, *cons) {
    NV_conduit * nc = cons->inf(di);
    QPoint p_old(nc->GetX(), nc->GetY());
    QPoint p_new = adjust(xzf, yzf, old_cen, new_cen, p_old);
    nc->SetX(p_new.x());
    nc->SetY(p_new.y());
  }
}

QPoint GraphField::adjust(double xzf, double yzf, QPoint & old_center, 
			  QPoint & new_center, QPoint & p)
{
  double dx = fabs(old_center.x() - p.x()),
         dy = fabs(old_center.y() - p.y());

  if (dx < 1) dx = 0; else dx *= xzf;
  if (dy < 1) dy = 0; else dy *= yzf;

  switch (quadrant(p, old_center)) {
    case 1: break;
    case 2: dx = -dx; break;
    case 3: dx = -dx; dy = -dy; break;
    case 4: dy = -dy; break;
  }
  QPoint p_new((int)(new_center.x() + dx), (int)(new_center.y() + dy));
  return (p_new);
}

void GraphField::SetDirty(void) { _dirty = true; }
void GraphField::screen_dump(bool d) { _screen_dump = d; }

// ---------------------------------------------------
int compare(const QPoint & lhs, const QPoint & rhs)
{
  if (lhs.x() == rhs.x() &&
      lhs.y() == rhs.y())
    return 0;
  if (lhs.x() < rhs.x() &&
      lhs.y() < rhs.y())
    return -1;
  //  if (lhs.x() > rhs.x() &&
  //      lhs.y() > rhs.y())
  return 1;
}

// ---------------------------------------------------
int operator == (const LayoutContainer & lhs, const LayoutContainer & rhs)
{
  return (rhs._x == lhs._x && rhs._y == lhs._y && rhs._vis == lhs._vis);
}
