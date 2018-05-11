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
#ifndef __v_APP_H__
#define __v_APP_H__

#ifndef LINT
static char const _v_app_h_rcsid_[] =
"$Id: v_app.h,v 1.2 1999/02/09 17:18:35 mountcas Exp $";
#endif

#include <qmainwindow.h> 
#include <qapplication.h>

class QMenuBar;
class QToolBar;
class QStatusBar;
class QPopupMenu;
class QSlider;
class v_main;
class v_parser;
class v_data;

class v_app : public QMainWindow {
  Q_OBJECT

  friend v_app & theApplication(void);
public:

  v_app(int argc, char ** argv, QApplication & app);
  virtual ~v_app();

  v_data * getData(void) const;
  v_main * centralWidget(void) const;
  const char * path(void) const;
  const char * input(void) const;

  void searchHelp(const char * keyword) const;

  void notify(double time);
  void updateSlider(double time);

protected:

  void timerEvent(  QTimerEvent  * te );
  void resizeEvent( QResizeEvent * re );
  void keyPressEvent( QKeyEvent * ke );

private slots:

  void open( );
  void save( );
  void print( );

  void sliderChanged( int );
  void toggleStatusBar( );
  void toggleToolBar( );

  void help_index( );
  void help_search( );
  void about( );

  void zoom_in( );
  void zoom_out( );
  void layout( );

  void stop( );
  void cont( );
  void step( );
  void restart( );

  void quit( );

private:
  
  void loadResources(void);

  // The main window contains a menu bar, tool bar, 
  //     status bar, and 'main widget'
  QMenuBar   * _menuBar;
  QToolBar   * _toolBar;
  QStatusBar * _statusBar;
  // This contains v_graph, v_list, v_table, and v_time
  v_main     * _centralWidget;
  v_parser   * _parser;
  v_data     * _data;

  // Menus
  QPopupMenu * _fileMenu;
  QPopupMenu * _controlMenu;
  QPopupMenu * _optionsMenu;
  QPopupMenu * _helpMenu;
  QSlider    * _slider;

  int            _tb, _sb, _ifd;
  
  static const int PARSE_INTERVAL;
  int            _parseTimer;

  QApplication & _app;

  char           _path[256];
  char           _iconFile[256];
  char           _iconText[64];
  char         * _where_am_i; 

  static v_app * _instance;
};

#endif
