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
#ifndef _GRAPHFIELD_H_
#define _GRAPHFIELD_H_

#ifndef LINT
static char const _graphfield_h_rcsid_[] =
"$Id: graphfield.h,v 1.2 1999/02/24 21:50:47 mountcas Exp $";
#endif

#include <qapp.h>
#include <qobject.h>
#include <qpushbt.h>
#include <qscrbar.h>
#include <qlcdnum.h>
#include <qfont.h>
#include <qpopmenu.h>
#include <qkeycode.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qevent.h>
#include <qpoint.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qmsgbox.h>
#include <qgrpbox.h>
#include <qstring.h>
#include <qwidget.h>
#include <qslider.h>
#include <qlistbox.h>
#include <qfiledlg.h>
#include <qkeycode.h>
#include <qrect.h>
#include <qpopmenu.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qmainwindow.h>

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
#include <FW/jiggle/Graph.h>

class NV_conduit;
class NV_visitor;

double dr_log(double x, double base);
int quadrant(QPoint & p, QPoint & center);
int quadrant(double x, double y, double x_cen, double y_cen);

// Add whatever you want to the layout container ...
class LayoutContainer {
  friend int operator == (const LayoutContainer & rhs, const LayoutContainer & lhs);
public:

  LayoutContainer(int x, int y, bool vis) :
    _x(x), _y(y), _vis(vis) { }
  LayoutContainer(int x = -1, int y = -1, char vis = 'y') :
    _x(x), _y(y), _vis(true) 
  {
      if (vis == 'y') _vis = true;
      else _vis = false;
  }
  LayoutContainer(const LayoutContainer & rhs) :
    _x(rhs._x), _y(rhs._y), _vis(rhs._vis) { }
  ~LayoutContainer() { }

  QPoint GetPos(void) const { return QPoint(_x, _y); }
  bool   IsVisible(void) const { return _vis; }

  int  _x;
  int  _y;
  bool _vis;
};

class MainWin;

// Global Windows/Widgets
class GraphField : public QMainWindow { // QWidget {
  Q_OBJECT
  friend int main(int argc, char ** argv);
  friend class MainWin;
public:

  GraphField(QWidget * parent = 0, const char * name = 0);
  virtual ~GraphField();
  void	   draw_frame(QPainter & p);
  void     draw_graph(QPainter & p);
  void     draw_node(QPainter & p, NV_conduit * c);
  void     draw_edge(QPainter & p, NV_conduit * start, NV_conduit * end);
  void     draw_visitors(QPainter & p);
  void     draw_visitor_colors(QPainter & p);
  void     draw_highlighting_box(QPainter & p, NV_conduit * v);
  void     emit_time(double t);
  void     set_active_visitor(NV_visitor * v);
  bool     continuing(void) const;
  LayoutContainer  find_layout(NV_conduit * c) const;
  void     SetDirty(void);
  void     screen_dump(bool d = false);
  // Added for trotta
#ifdef TROTTA
  void     window_to_kdb(void);
  void     reset_geometry(void);
  void     store_data(char * name, char * command, char * type);
  char *   itoa(int num);
  void     read_from_file(FILE * fileptr);
  void     build_timer_list(void); 
  void     build_simentity_list(void);
  void     build_conduit_list(void);
#endif
public slots:

  // Print menu
  void  print( );
  void  screen_cap( );

  // File menu
  void  open_data( );
  void  load_layout( );
  void  save_layout( );
  void  restart( );

  // Edit menu
  void  cut( );
  void  copy( );
  void  paste( );

  // Control menu
  void  step( );
  void  cont( );
  void  stop( );
  void  layout( );
  void 	zoom( );
  void 	unzoom( );
  void  toggle( );
  void  test( );

  void toggleStatusBar( );
  void toggleToolBar( );

  // Help menu
  void  show_help( );
  void  show_about( );

  void	scroll_h_handler(int i);
  void	scroll_v_handler(int i);
  void  highlight_visitor(int i);
  void  display_time(double t);
  void  compute_force( ); // const list<NV_conduit *> & rhs );

  // This section needs to be commented out 
  //  because 'moc' doesn't understand preprocessor directives
  //   void  breakpoints_handler();                           
  //   void  quit_list_objects_handler();                     
  //   void  list_timers_handler();                           
  //   void  list_conduit_handler();                          
  //   void  list_simentities_handler();                      
  //   void  list_visitors_handler();                         
  
  //   void  Breakpoint_clicked();                            
  //   void  quit_clicked();                                  
  
  //   void  clear_all_bpts_handler();                        
  //   void  delete_bpt_handler();                            
  //   void  quit_bptlist_handler();                          
  
  //   void  quit_list_timers_handler();                      
  //   void  break_timers_callback_handler();                 
  //   void  break_timers_destruction_handler();              
  
  //   void  quit_list_simentities_handler();                 
  //   void  break_simentities_handler();
  // the above section needs to be commented out

signals:

  void  time_event(double);
  void  compute_event( );
  
protected:

  void  paintEvent( QPaintEvent * );
  void  timerEvent( QTimerEvent * );
  void  mousePressEvent(QMouseEvent * );
  void  mouseReleaseEvent(QMouseEvent * );
  void  mouseMoveEvent(QMouseEvent * );
  void  keyPressEvent(QKeyEvent * );

  void  zoomer(double);
  QPoint find_center(void);
  void   zoom(QPoint &, double, double);
  QPoint adjust(double zf, double yzf, QPoint & old_center, 
		QPoint & new_center, QPoint & p);
  NV_conduit * nearest_node(QPoint & pnt, double & d_min);
  NV_conduit * find_node(QPoint & pnt);
  void  display_node_data(NV_conduit * v);
  void  load_layout_file(const char * filename);
  void  clear_layouts(void);
  void  initialize_visitor_table(void);
  void  draw_visitor_tracking(QPainter & p, QColor color, NV_visitor * vis, int loc);
  void  track_all_visitors(QPainter & p);
  void  draw_hbox(QPainter & p);
  void  CalculateNodePos(NV_conduit * c);
  void  draw_to_pixmap(QPixmap & pm, const char * filename = 0);
  void  restore_nodes(const list<NV_conduit *> & cons, Graph & g);
  double GetX(NV_conduit * nc);
  double GetY(NV_conduit * nc);
  void draw_event_list(list<char *> & events);

private:

#ifdef TROTTA
  // JT - to determine following process, command to kdb, depending on button
  enum eDebugType {
    Unknown = 0,
    Quit,
    Breakpoint,
    // ... add more here
  };
  void        display_data(eDebugType dt);

  //  list<QPushButton *> _kdb_win_buttons; // JT 
  QGroupBox         * _group; //JT
  QListBox          * _visitorlist; // JT
  QPushButton       * _Quit; // JT
  QPushButton       * _Breakpoint; // JT

  QGroupBox         * _list_objects; // JT
  QPushButton       * _quit_list_objects;
  QPushButton       * _breakpoint;
  QPushButton       * _timers;
  QPushButton       * _list_conduit;
  QPushButton       * _simentities;
  QPushButton       * _visitors;
  
  QGroupBox         * _group_breakpts; //JT
  QListBox          * _breakptlist; //JT
  QPushButton       * _quit_bptlist; //JT
  QPushButton       * _delete_bpt; //JT
  QPushButton       * _clear_all_bpts; //JT
 
  QGroupBox         * _list_timers; // JT
  QListBox          * _timerlist; // JT
  QPushButton       * _quit_list_timers; // JT
  QPushButton       * _break_timers_callback; // JT
  QPushButton       * _break_timers_destruction; // JT

  QGroupBox         * _list_simentities; // JT
  QListBox          * _simentitylist; // JT
  QPushButton       * _quit_list_simentities; // JT
  QPushButton       * _break_simentities; // JT
 
  bool                _am_I_finished; // JT

  static const int    _str_size = 50; // JT
  static const int    _max_words = 5; // JT
  char                _bunch[_max_words][_str_size]; // JT
  char                _scan_s[_str_size]; // JT
#endif

  QToolBar          * _tools;
  QMenuBar          * _menu;
  QPopupMenu        * _options;
  int                 _toolBar, _statusBar;
  list<NV_conduit *>  _hbox_nodes;
  QScrollBar        * _hBar;
  QScrollBar        * _vBar;
  QRect	            * _hbox;
  QPoint              _hbox_pt;
  NV_conduit        * _moved_node;
  NV_conduit        * _clicked_conduit;
  NV_visitor        * _active_visitor;
  int                 _X_BP;
  int                 _Y_BP;
  double              _Jig_minX;
  double              _Jig_minY;
  double              _Jig_maxX;
  double              _Jig_maxY;
  dictionary<char *, LayoutContainer> _layouts;

  // ---------- control flags ----------
  // True when drawing the Visitor window
  bool                _Vist_flag;
  // True if we are in a continue loop.
  bool                _Cont_flag;
  // True when shift key is pressed (to ungroup expanders).
  bool                _Ungroup_flag;
  // True when alt key is pressed (to click on conduits for debugging).
  bool                _Debugging_mode;
  // True if left MB is pressed (to drag around Conduits).
  bool                _Move_flag;
  // True when zooming in and out (perhaps we can rescale the pixmaps as well?)
  bool                _Zoom_flag;
  // True if we do not want to move around nodes.
  bool                _No_change_flag;
  // True if 'a' is pressed, used to group nodes.
  bool                _Hbox_flag;
  bool                _Hbox_ready;
  // True if left MB is pressed when group is selected.
  bool                _Hbox_move;
  // True if something visible has changed
  bool                _dirty;
  bool                _screen_dump;

  int                 _Cont_timer;
  int                 _Compute_timer;
  int                 _zoom_depth;
  // ----------------------------------
  static const int    _node_width = 16;
  static const int    _offset = 55;
};

#endif  // _GRAPHFIELD_H_
