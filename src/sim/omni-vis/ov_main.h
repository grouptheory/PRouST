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
#ifndef __OV_MAIN_H__
#define __OV_MAIN_H__
#ifndef LINT
static char const _ov_main_h_rcsid_[] =
"$Id: ov_main.h,v 1.14 1999/02/24 21:56:54 mountcas Exp $";
#endif

#include <qmainwindow.h>
#include "ov_event.h"
#include "eventTypeRegistry.h"

class QApplication;
class QBoxLayout;
class QMenuBar;
class QMenuBar;
class QToolBar;
class QStatusBar;
class QPopupMenu;
class WidgetView;
class ov_main;
class ov_parser;
class ov_data;
class Event;
class levelRegistry;

//-----------------------------------------------
class ov_main : public QMainWindow {
  Q_OBJECT

  friend ov_main & theVisualizer(void);
public:

  ov_main(int argc, char ** argv, QApplication & app);
  virtual ~ov_main();
  
  void addEvent(Event * e);
  void updateTime(int time);
  void sliderMax(int time);
  void parse( int time );
  int  endTime(void) const;

protected:

  void draw( QPainter & p );

  void timerEvent(  QTimerEvent  * te );
  void keyPressEvent( QKeyEvent  * ke );

  void initializeMenuBar(QBoxLayout * geomMgr);
  void initializeToolBar(QBoxLayout * geomMgr);
  void initializeStatusBar(QBoxLayout * geomMgr);

private slots:

  void menuFile_Open( );
  void menuFile_Load( );
  void menuFile_Save( );
  void menuFile_Font( );
  void menuFile_Print( );
  void quit( );
  
  void menuControl_Step( );
  void menuControl_Cont( );
  void menuControl_Stop( );
  void menuControl_Restart( );
  
  void menuNetwork_All_Levels( );
  void menuNetwork_Level_96( );
  void menuNetwork_Level_88( );
  void menuNetwork_Level_80( );
  void menuNetwork_Level_72( );
  void menuNetwork_Other_Levels( );
  void menuNetwork_Layout( );
  
  void menuView_Above( );
  void menuView_North( );
  void menuView_NorthEast( );
  void menuView_East( );
  void menuView_SouthEast( );
  void menuView_South( );
  void menuView_SouthWest( );
  void menuView_West( );
  void menuView_NorthWest( );
  void menuView_Other( );
  void menuView_Toggle_Axes( );
  void menuView_Toggle_Edges( );
  void menuView_Toggle_Perspective( );
  
  void doEventMenu( int );
  
  void menuHelp_Index( );
  void menuHelp_About( );
  
private:
  
  int _ID_File_Open;
  int _ID_File_Save;
  int _ID_File_Load;
  int _ID_File_Font;
  int _ID_File_Print;
  int _ID_File_Quit;
  
  int _ID_Control_Step;
  int _ID_Control_Cont;
  int _ID_Control_Stop;
  int _ID_Control_Restart;
  
  static const int _MAX_LEVELS = 108;
  int              _num_lvl;

  void setNetworkMenu( bool value );
  int _ID_Network_All_Levels;
  int _ID_Network_Level_96;
  int _ID_Network_Level_88;
  int _ID_Network_Level_80;
  int _ID_Network_Level_72;
  int _ID_Network_Other_Levels;
  int _ID_Network_Layout;
  
  int _ID_View_Above;
  int _ID_View_North;
  int _ID_View_NorthEast;
  int _ID_View_East;
  int _ID_View_SouthEast;
  int _ID_View_South;
  int _ID_View_SouthWest;
  int _ID_View_West;
  int _ID_View_NorthWest;
  int _ID_View_Other;
  int _ID_View_Toggle_Axes;
  int _ID_View_Toggle_Edges;
  int _ID_View_Toggle_Perspective;
  
  static const int _MAX_EVENT_TYPES = 50;
  int              _num_ev;
  
  void setEventsMenu( bool value );
  int _ID_Event[eventTypeRegistry::_NUM_EVENT_TYPES];
    
  int _ID_Help_Index;
  int _ID_Help_About;
  
  QMenuBar * _menuBar;
  
  QPopupMenu     * _fileMenu;
  QPopupMenu     * _controlMenu;
  QPopupMenu     * _networkMenu;
  QPopupMenu     * _viewMenu;
  QPopupMenu     * _eventsMenu;
  QPopupMenu     * _helpMenu;

  QToolBar       * _toolBar;
  QStatusBar     * _statusBar;

  QApplication   & _app;

  WidgetView     * _central_widget;

  static const int PARSE_INTERVAL  = 1; // 200;
  static const int LAYOUT_INTERVAL = 50;
  ov_parser      * _parser;
  int              _parseTimer;

  ov_data           * _data;
  eventTypeRegistry * _etypes;

  levelRegistry     * _levels;
  int                 _layoutTimer;

  static ov_main    * _instance;
};

class NodeID;
void abbreviatedPrint(char* str, const NodeID* node, bool dots = false);
void abbreviatedTime(char* str, const double time);

#endif // __OV_MAIN_H__
