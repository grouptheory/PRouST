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
static char const _ov_main_cc_rcsid_[] =
"$Id: ov_main.cc,v 1.30 1999/02/03 17:06:03 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <iostream.h>
#include <fstream.h>
#include <qapplication.h>
#include <qpainter.h>

#include "ov_main.h"
#include "WidgetView.h"
#include "canvas3D.h"
#include "ov_parser.h"
#include "ov_data.h"
#include "levelRegistry.h"

#include <codec/pnni_ig/id.h>

#define TOOL_BAR_HEIGHT   25
#define TOOL_BAR_WIDTH    200
#define STATUS_BAR_HEIGHT 25

/* ------------------------------------------------
   The main() program for the PRouST OmniVisualizer
   ------------------------------------------------ */

int main( int argc, char **argv )
{
  QApplication::setColorSpec( QApplication::CustomColor );
  QApplication a( argc, argv );
  ov_main * mw = new ov_main( argc, argv, a );
  a.setMainWidget(mw);
  mw->show();
  a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
  return a.exec();
}

//------------------------------------------------------------------

#include "ov_main.h"

#include <qmenubar.h>
#include <qtoolbar.h>
#include <qstatusbar.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>
#include <qfiledialog.h>
#include <qprinter.h>
#include <qkeycode.h>
#include <qpixmap.h>

// ----- Pixmaps -----
#include "pixmaps/open_icon.xpm"
#include "pixmaps/save_icon.xpm"
#include "pixmaps/print_icon.xpm"
#include "pixmaps/layout_icon.xpm"
#include "pixmaps/stop_icon.xpm"
#include "pixmaps/step_icon.xpm"
#include "pixmaps/cont_icon.xpm"
#include "pixmaps/restart_icon.xpm"

ov_main * ov_main::_instance = 0;

//----------------------------------------
// Methods of class 'ov_main'
// ---------------------------------------

ov_main & theVisualizer(void)
{
  assert( ov_main::_instance != 0 );
  return *(ov_main::_instance);
}

// ------------------------------------------------------------------
ov_main::ov_main(int argc, char ** argv, QApplication & app) 
  : QMainWindow( 0, argv[0] ), 
    _menuBar(0), _toolBar(0), _statusBar(0), 
    _fileMenu(0), _controlMenu(0), 
    _networkMenu(0), _viewMenu(0),
    _eventsMenu(0), _helpMenu(0), 
    _app(app), 
    _parseTimer(-1)
{ 
  assert( _instance == 0 );
  _instance = this;

  _etypes = new eventTypeRegistry();
  _levels = new levelRegistry();

  setCaption( "The PRouST OmniVisualizer" );
  initializeMenuBar   ( 0 );
  initializeToolBar   ( 0 );

  // Allocate the data repository
  _data = new ov_data( );

  // Allocate the parser
  _parser = new ov_parser( this, _data, argc, argv );

  _central_widget = new WidgetView(this, "central widget", _data);
  setCentralWidget( _central_widget );

  if ( !strncmp(argv[1], "-f", 2) )  
    _central_widget->setFont( argv[2] );

  initializeStatusBar ( 0 );

  resize( 1024, 768 );
}

// ------------------------------------------------------------------
void ov_main::addEvent( Event * e )
{
  _central_widget->addEvent( e );
}

void ov_main::updateTime( int t )
{
  _central_widget->updateTime( t );
}

void ov_main::sliderMax(int i)
{
  _central_widget->sliderMax(i);
}

void ov_main::parse( int time )
{
  _parser->parse(time);
}

int  ov_main::endTime(void) const
{
  return (int)(_parser->finalTime()) + 1;
}

// ------------------------------------------------------------------
void ov_main::initializeToolBar(QBoxLayout* geomMgr) {
  assert( (_toolBar = new QToolBar( this, "" )) != 0 );

  // ----- Allocate Icons for Tool Bar -----
  QPixmap openIcon( open_icon ),
    saveIcon( save_icon ),
    printIcon( print_icon ),
    layoutIcon( layout_icon ),
    stopIcon( stop_icon ),
    stepIcon( step_icon ),
    contIcon( cont_icon ),
    restIcon( restart_icon );

  // ----- Allocate Tool Bar Buttons -----
  QToolButton * opent  = new QToolButton(openIcon, "Open", 0, this, 
                         SLOT(menuFile_Open()), _toolBar, "open");
  QToolButton * savet  = new QToolButton(saveIcon, "Save", 0, this,
                         SLOT(menuFile_Save()), _toolBar, "save");
  QToolButton * loadt  = new QToolButton(openIcon, "Load Layout", 0, this,
                         SLOT(menuFile_Load()), _toolBar, "load");
  QToolButton * printt = new QToolButton(printIcon, "Print", 0, this,
                         SLOT(menuFile_Print()), _toolBar, "print");
  _toolBar->addSeparator( );
  QToolButton * restt  = new QToolButton(restIcon, "Restart", 0, this, 
                         SLOT(menuControl_Restart( )), _toolBar, "restart" );
  QToolButton * stept  = new QToolButton(stepIcon, "Step", 0, this, 
                         SLOT(menuControl_Step()), _toolBar, "step");
  QToolButton * contt  = new QToolButton(contIcon, "Continue", 0, this, 
                         SLOT(menuControl_Cont()), _toolBar, "continue");
  QToolButton * stopt  = new QToolButton(stopIcon, "Stop", 0, this, 
                         SLOT(menuControl_Stop()), _toolBar, "stop");
  _toolBar->addSeparator( );
  QToolButton * layoutt = new QToolButton(layoutIcon, "Layout", 0, this,
                          SLOT(menuNetwork_Layout()), _toolBar,
                          "rearrange switches");

  // Add the tool tips for the above
  QWhatsThis::add( opent,   "open a simulation.",                FALSE );
  QWhatsThis::add( savet,   "save the layout.",                  FALSE );
  QWhatsThis::add( loadt,   "load the layout.",                  FALSE );
  QWhatsThis::add( printt,  "print the simulation.",             FALSE );

  QWhatsThis::add( restt,   "restart the simulation.",           FALSE );
  QWhatsThis::add( stept,   "parse one line of the simulation.", FALSE );
  QWhatsThis::add( stopt,   "stop a running simulation.",        FALSE );
  QWhatsThis::add( contt,   "continue a simulation.",            FALSE );

  QWhatsThis::add( layoutt, "rearrange the nodes",               FALSE );
  QWhatsThis::whatsThisButton( _toolBar );

  _toolBar->setFixedHeight(TOOL_BAR_HEIGHT);
  _toolBar->setFixedWidth(TOOL_BAR_WIDTH);
  _toolBar->show();
}

// ------------------------------------------------------------------
void ov_main::initializeStatusBar(QBoxLayout* geomMgr) {
  assert( (_statusBar = statusBar()) != 0 );

  _statusBar->setFixedHeight(STATUS_BAR_HEIGHT);
  _statusBar->message( "Ready", 2000 );
}

// ------------------------------------------------------------------
ov_main::~ov_main() 
{ 
  delete _data;
  delete _parser;
  
  delete _etypes;
  delete _levels;

  assert(_instance);
  _instance = 0;
}

// ------------------------------------------------------------------
void ov_main::timerEvent(  QTimerEvent  * te )
{
  if ( te->timerId() == _parseTimer ) {
    killTimer( _parseTimer );    

    if ( _parser->input() == 0 ) {
      QMessageBox mb( this, "Error" );
      QString text = "\nThere is no data to be parsed!\n";
      text += "Try opening a data file.\n";
      mb.setText( text );
      mb.setButtonText( "OK" );
      mb.show( );
    } else {
      int rval = -1;
      if ((rval = _parser->parse()) == 0) {
        repaint( false );
	if (_parser->continuing()) {
	  _parseTimer = startTimer( PARSE_INTERVAL );
	} else {
	  _parseTimer = -1;
	}
      } else {
        _statusBar->message( "Read initial time stamp, stopping", 5000 );
      }
    }
  } else if ( te->timerId() == _layoutTimer ) {
    killTimer( _layoutTimer );
    if (_data->layout( ) == false)
      _layoutTimer = startTimer( LAYOUT_INTERVAL );
    else
      _layoutTimer = -1;

    repaint( false );
  }
}

// ------------------------------------------------------------------
void ov_main::keyPressEvent( QKeyEvent  * ke ) 
{ 
  //  cout << "ov_main::keyPressEvent" << endl;
  _central_widget->keyPressEvent( ke );
}

// ------------------------------------------------------------------
void ov_main::draw( QPainter & p )
{
  _central_widget->draw( p );
}

// ------------------------------------------------------------------
// menu-related methods and slots
// ------------------------------------------------------------------
void ov_main::initializeMenuBar(QBoxLayout* geomMgr) 
{
  assert( (_menuBar = menuBar()) != 0 );

  // ----- Allocate Icons for Menu Bar -----
  QPixmap openIcon( open_icon ),
          saveIcon( save_icon ),
          printIcon( print_icon ),
          layoutIcon( layout_icon ),
          stopIcon( stop_icon ),
          stepIcon( step_icon ),
          contIcon( cont_icon ),
          restIcon( restart_icon );

  // ----- Allocate Menu Bar Buttons -----
  assert( (_fileMenu = new QPopupMenu( )) != 0);
  _ID_File_Open = 
    _fileMenu->insertItem( openIcon,"Open",this,
			   SLOT(menuFile_Open()), CTRL + Key_O );
  _ID_File_Save = 
    _fileMenu->insertItem( saveIcon,"Save",this,
			   SLOT(menuFile_Save()), CTRL + Key_S );
  _ID_File_Load = 
    _fileMenu->insertItem( openIcon,"Load",this,
			   SLOT(menuFile_Load()), CTRL + Key_L );
  _ID_File_Font = 
    _fileMenu->insertItem( "Font",this,
			   SLOT(menuFile_Font()), CTRL + Key_F );
  _fileMenu->insertSeparator( );
  _ID_File_Print = 
    _fileMenu->insertItem( printIcon,"Print",this,
			   SLOT(menuFile_Print()), CTRL + Key_P );
  _fileMenu->insertSeparator( );
  _ID_File_Quit = 
    _fileMenu->insertItem( "Quit",this,
			   SLOT(quit()), CTRL + Key_Q );

  // Notify the Menu Bar of the File Menu
  _menuBar->insertItem( "&File", _fileMenu );

  assert( (_controlMenu = new QPopupMenu( ) ) != 0);
  _ID_Control_Step = 
    _controlMenu->insertItem( stepIcon,"Step",this,
			      SLOT(menuControl_Step( )),Key_Space);
  _ID_Control_Cont = 
    _controlMenu->insertItem( contIcon,"Continue",this,
			      SLOT(menuControl_Cont( )),Key_Return);
  _ID_Control_Stop = 
    _controlMenu->insertItem( stopIcon,"Stop",this,
			      SLOT(menuControl_Stop( )),Key_Pause);
  _ID_Control_Restart = 
    _controlMenu->insertItem( restIcon,"Restart",this,
			      SLOT(menuControl_Restart( )),Key_Backspace);

  // Notify the Menu Bar of the Control Menu
  _menuBar->insertItem( "&Control", _controlMenu );

  assert( (_networkMenu = new QPopupMenu( )) != 0);
  int level_i = 0;
  _ID_Network_All_Levels = 
  _networkMenu->insertItem( "All Levels",this, 
			    SLOT(menuNetwork_All_Levels()));
  level_i++;
  _ID_Network_Level_96 =
    _networkMenu->insertItem( "Level 96",this, 
			      SLOT(menuNetwork_Level_96()));
  level_i++;
  _ID_Network_Level_88 = 
    _networkMenu->insertItem( "Level 88",this, 
			      SLOT(menuNetwork_Level_88()));
  level_i++;
  _ID_Network_Level_80 = 
    _networkMenu->insertItem( "Level 80",this, 
			      SLOT(menuNetwork_Level_80()));
  level_i++;
  _ID_Network_Level_72 = 
    _networkMenu->insertItem( "Level 72",this, 
			      SLOT(menuNetwork_Level_72()));
  level_i++;
  _ID_Network_Other_Levels = 
    _networkMenu->insertItem( "Other Levels...",this, 
			      SLOT(menuNetwork_Other_Levels()));
  level_i++; _num_lvl = level_i;

  // not yet implemented
  _networkMenu->setItemEnabled(_ID_Network_Other_Levels , false);

  _networkMenu->setCheckable( TRUE );
  _networkMenu->setItemChecked( _ID_Network_All_Levels, TRUE );
  _networkMenu->setItemChecked(_ID_Network_Level_96, false);
  _networkMenu->setItemChecked(_ID_Network_Level_88 , false);
  _networkMenu->setItemChecked(_ID_Network_Level_80 , false);
  _networkMenu->setItemChecked(_ID_Network_Level_72 , false);
  _networkMenu->setItemChecked(_ID_Network_Other_Levels , false);

  _networkMenu->insertSeparator( );
  _ID_Network_Layout = 
  _networkMenu->insertItem( layoutIcon, "Layout", this, 
			    SLOT(menuNetwork_Layout( )), CTRL + Key_L);

  // Notify the Menu Bar of the Network Menu
  _menuBar->insertItem( "&Network", _networkMenu );

  // ----------------View Menu-----------------
  assert( (_viewMenu = new QPopupMenu( )) != 0);
  _ID_View_Above     = _viewMenu->insertItem( "Above",  
		       this, SLOT(menuView_Above()));
  _ID_View_North     = _viewMenu->insertItem( "North",  
		       this, SLOT(menuView_North()));
  _ID_View_NorthEast = _viewMenu->insertItem( "NE", 
		       this, SLOT(menuView_NorthEast()));
  _ID_View_East      = _viewMenu->insertItem( "East",   
		       this, SLOT(menuView_East()));
  _ID_View_SouthEast = _viewMenu->insertItem( "SE",
		       this, SLOT(menuView_SouthEast()));
  _ID_View_South     = _viewMenu->insertItem( "South",  
		       this, SLOT(menuView_South()));
  _ID_View_SouthWest = _viewMenu->insertItem( "SW", 
		       this, SLOT(menuView_SouthWest()));
  _ID_View_West      = _viewMenu->insertItem( "West",  
		       this, SLOT(menuView_West()));
  _ID_View_NorthWest = _viewMenu->insertItem( "NW",
		       this, SLOT(menuView_NorthWest()));
  _ID_View_Other     = _viewMenu->insertItem( "Other...", 
		       this, SLOT(menuView_Other()));

  _viewMenu->setItemEnabled(_ID_View_Other, false); // not yet implemented

  _viewMenu->insertSeparator( );
  _viewMenu->setCheckable( TRUE );
  _ID_View_Toggle_Axes = 
    _viewMenu->insertItem( "Draw Axes",  this, SLOT(menuView_Toggle_Axes()) );
  _viewMenu->setItemChecked( _ID_View_Toggle_Axes, TRUE );
  _ID_View_Toggle_Edges = 
    _viewMenu->insertItem( "Draw Leader Edges",  this, SLOT(menuView_Toggle_Edges()) );
  _viewMenu->setItemChecked( _ID_View_Toggle_Edges, FALSE );
  _ID_View_Toggle_Perspective =
    _viewMenu->insertItem( "Perspective View", this, SLOT(menuView_Toggle_Perspective()) );
  _viewMenu->setItemChecked( _ID_View_Toggle_Perspective, TRUE );
  
  // Notify the Menu Bar of the View Menu
  _menuBar->insertItem( "&View", _viewMenu );

  // ----------------Events Menu-----------------
  assert( (_eventsMenu = new QPopupMenu( )) != 0);
  _eventsMenu->setCheckable( TRUE );
  for (int event_i = eventTypeRegistry::_FIRST_EVENT_TYPE; 
       event_i <= eventTypeRegistry::_LAST_EVENT_TYPE; 
       event_i++) {

    eventTypeRegistry::event_type ev = (eventTypeRegistry::event_type)event_i;
    int index = theEventTypeRegistry().type2index(ev);

    char menu_name[50];
    theEventTypeRegistry().type2str(ev, menu_name);

    _ID_Event[ index ] = _eventsMenu->insertItem( menu_name, event_i );

    assert( _ID_Event[ index ] == (int)event_i );
    _eventsMenu->setItemChecked( event_i, FALSE );
  }
  _eventsMenu->setItemChecked( eventTypeRegistry::All_type, true );

  connect( _eventsMenu, SIGNAL(activated(int)), 
	   this, SLOT(doEventMenu(int)) );

  // Notify the Menu Bar of the Events Menu
  _menuBar->insertItem( "&Events", _eventsMenu );

  // ----------------Help Menu-----------------
  assert( (_helpMenu = new QPopupMenu( )) != 0);
  _ID_Help_Index =  
  _helpMenu->insertItem( "Index",this, 
			 SLOT(menuHelp_Index()), CTRL + Key_H );
  _ID_Help_About = 
  _helpMenu->insertItem( "About",this, 
			 SLOT(menuHelp_About()), CTRL + Key_A );
  
  // Notify the Menu Bar of the Help Menu
  _menuBar->insertItem( "&Help", _helpMenu );
}

// --------------------------- SLOTS --------------------------------
void ov_main::menuFile_Font( )
{
  QString fn = QFileDialog::getOpenFileName( "./FONTS", "*.txf", this );

  if ( !fn.isEmpty() ) {
    _statusBar->message( "Loading...", 1000 );
    _central_widget->setFont( fn );
    _parser->restart( fn );
    _statusBar->message( "Loading complete.", 2000 );
    repaint( false );
  } else {
    _statusBar->message( "Loading aborted.", 2000 );
  }
}

void ov_main::menuFile_Open( )
{
  QString fn = QFileDialog::getOpenFileName( "./TEST", "*.OMNI", this );

  if ( !fn.isEmpty() ) {
    _statusBar->message( "Loading...", 1000 );
    _parser->restart( fn );
    _statusBar->message( "Loading complete.", 2000 );
    repaint( false );
  } else {
    _statusBar->message( "Loading aborted.", 2000 );
  }
}

void ov_main::menuFile_Save( )
{
  if (_data->empty())
    _statusBar->message( "There is no layout information to save!", 2000 );
  else {
    QString fn = QFileDialog::getSaveFileName( 0, "*.lay", this );

    if ( !fn.isEmpty() ) {
      _statusBar->message( "Saving...", 1000 );
      _data->save( fn );
      _statusBar->message( "Saving complete.", 2000 );
    }
    else {
      _statusBar->message( "Saving aborted.", 2000 );
    }
  }
}

void ov_main::menuFile_Load( )
{
  if (_data->empty())
    _statusBar->message( "There is no layout information to load!", 2000 );
  else {
    QString fn = QFileDialog::getOpenFileName( 0, "*.lay", this );

    if ( !fn.isEmpty() ) {
      _statusBar->message( "Loading...", 1000 );
      _data->load( fn );
      _statusBar->message( "Loading complete.", 2000 );
      repaint( );
    }
    else {
      _statusBar->message( "Loading aborted.", 2000 );
    }
  }
}

void ov_main::menuFile_Print( )
{
  if (_data->empty())
    _statusBar->message( "There is nothing to print!", 2000 );
  else {
    QPrinter * printer = new QPrinter( );
    printer->setPageSize( QPrinter::Letter );

    if (printer->setup(this)) {
      _statusBar->message( "Printing...", 1000 );
      QPainter p;
      p.begin( printer );
      p.setViewport( 0, 0, 350, 650 );
      draw( p );
      p.end();
      _statusBar->message( "Printing complete.", 2000 );
    } else {
      _statusBar->message( "Printing aborted.", 2000 );
    }
  }
}

void ov_main::quit( )
{
  _statusBar->message( "Goodbye...", 2000 );
  _app.quit();
}


// ------------------------------------------------------------------

void ov_main::menuControl_Step( )
{
  _statusBar->message( "Stepping.", 2000 );
  _parseTimer = startTimer( PARSE_INTERVAL );
}

void ov_main::menuControl_Cont( )
{
  if ( _parseTimer != 0 )
    killTimer( _parseTimer );
  _parseTimer = startTimer( PARSE_INTERVAL );
  _parser->continuing(true);
  _statusBar->message( "Continuing...", 10000 );
}

void ov_main::menuControl_Stop( )
{
  _parser->continuing(false);
  if ( _parseTimer != -1 )
    killTimer( _parseTimer );
  _statusBar->message( "Stopping.", 2000 );
}

void ov_main::menuControl_Restart( )
{
  _parser->restart();
  _statusBar->message( "Restarting the simulation...", 2000 );
}

// ------------------------------------------------------------------


void ov_main::doEventMenu( int code ) {
  int postflip;

  switch (code) {
  case eventTypeRegistry::All_type:
  case eventTypeRegistry::None_type:
    setEventsMenu(false);
    _eventsMenu->setItemChecked( code, true );
    postflip = 1;
    break;
  case eventTypeRegistry::Call_Submission_type:
  case eventTypeRegistry::Call_Arrival_type:
  case eventTypeRegistry::Call_Admission_type:
  case eventTypeRegistry::Call_Rejection_type:
  case eventTypeRegistry::DTL_Expansion_type:
  case eventTypeRegistry::Crankback_type:
  case eventTypeRegistry::I_am_PGL_type:
  case eventTypeRegistry::Voted_Out_type:
  case eventTypeRegistry::Hlink_Aggr_type:
  case eventTypeRegistry::NSP_Aggr_type:
  case eventTypeRegistry::Hello_Up_type:
  case eventTypeRegistry::Hello_Down_type:
  case eventTypeRegistry::NP_Full_type:
  case eventTypeRegistry::NP_Down_type: 
    {
      bool presently = _eventsMenu->isItemChecked( code );
      if (presently) postflip = 0;
      else postflip = 1;
      _eventsMenu->setItemChecked( code, postflip );
      _eventsMenu->setItemChecked( eventTypeRegistry::All_type, false );
      _eventsMenu->setItemChecked( eventTypeRegistry::None_type, false );
    }
  break;
  default:
    abort();
    break;
  }

  theEventTypeRegistry().
    setSelected ((eventTypeRegistry::event_type)code, postflip);

  _central_widget->filterEvents();
}

void ov_main::setEventsMenu( bool value ) {
  for (int event_i = eventTypeRegistry::_FIRST_EVENT_TYPE; 
           event_i <= eventTypeRegistry::_LAST_EVENT_TYPE; 
           event_i++) {
    _eventsMenu->setItemChecked( event_i, value );
  }
}

// ------------------------------------------------------------------

void ov_main::menuView_Above( ) {
  _central_widget->moveCameraTo( deg2rad(90),  deg2rad(0) );
  repaint( false );
}

void ov_main::menuView_North( ) {
  _central_widget->moveCameraTo( deg2rad(45),  deg2rad(0) );
  repaint( false );
}

void ov_main::menuView_NorthEast( ) {
  _central_widget->moveCameraTo( deg2rad(45),  deg2rad(45) );
  repaint( false );
}

void ov_main::menuView_East( ) {
  _central_widget->moveCameraTo( deg2rad(45),  deg2rad(90) );
  repaint( false );
}

void ov_main::menuView_SouthEast( ) {
  _central_widget->moveCameraTo( deg2rad(45),  deg2rad(135) );
  repaint( false );
}

void ov_main::menuView_South( ) {
  _central_widget->moveCameraTo( deg2rad(45),  deg2rad(180) );
  repaint( false );
}

void ov_main::menuView_SouthWest( ) {
  _central_widget->moveCameraTo( deg2rad(45),  deg2rad(225) );
  repaint( false );
}

void ov_main::menuView_West( ) {
  _central_widget->moveCameraTo( deg2rad(45),  deg2rad(270) );
  repaint( false );
}

void ov_main::menuView_NorthWest( ) {
  _central_widget->moveCameraTo( deg2rad(45),  deg2rad(-45) );
  repaint( false );
}

void ov_main::menuView_Other( ) {
}

void ov_main::menuView_Toggle_Axes( )
{
  bool state =  _viewMenu->isItemChecked( _ID_View_Toggle_Axes );
  _viewMenu->setItemChecked( _ID_View_Toggle_Axes, !state );
  _central_widget->drawAxes( !state );
  repaint( false );
}

void ov_main::menuView_Toggle_Edges( )
{
  bool state = _viewMenu->isItemChecked( _ID_View_Toggle_Edges );
  _viewMenu->setItemChecked( _ID_View_Toggle_Edges, !state );
  _central_widget->drawLeaderEdges( !state );
  repaint( false );
}

void ov_main::menuView_Toggle_Perspective( )
{
  bool state = _viewMenu->isItemChecked( _ID_View_Toggle_Perspective );
  _viewMenu->setItemChecked( _ID_View_Toggle_Perspective, !state );
  _central_widget->drawPerspective( !state );
}
// ------------------------------------------------------------------

void ov_main::setNetworkMenu( bool value ) {
  _networkMenu->setItemChecked( _ID_Network_All_Levels, value );
  _networkMenu->setItemChecked( _ID_Network_Level_96, value );
  _networkMenu->setItemChecked( _ID_Network_Level_88, value );
  _networkMenu->setItemChecked( _ID_Network_Level_80, value );
  _networkMenu->setItemChecked( _ID_Network_Level_72, value );
  _networkMenu->setItemChecked( _ID_Network_Other_Levels, value );
}

void ov_main::menuNetwork_All_Levels( ) {
  setNetworkMenu(false);
  _networkMenu->setItemChecked( _ID_Network_All_Levels, true );
  _levels->setSelectedAll();
  repaint(false);
}

void ov_main::menuNetwork_Level_96( ) {
  _networkMenu->setItemChecked( _ID_Network_All_Levels, false );
  bool newval;
  _networkMenu->setItemChecked( _ID_Network_Level_96,
      (newval = ! _networkMenu->isItemChecked( _ID_Network_Level_96 )) );
  _levels->setSelected(96, newval);
  repaint(false);
}

void ov_main::menuNetwork_Level_88( ) {
  _networkMenu->setItemChecked( _ID_Network_All_Levels, false );
  bool newval;
  _networkMenu->setItemChecked( _ID_Network_Level_88,
      (newval = ! _networkMenu->isItemChecked( _ID_Network_Level_88 )) );
  _levels->setSelected(88, newval);
  repaint(false);
}

void ov_main::menuNetwork_Level_80( ) {
  _networkMenu->setItemChecked( _ID_Network_All_Levels, false );
  bool newval;
  _networkMenu->setItemChecked( _ID_Network_Level_80,
      (newval = ! _networkMenu->isItemChecked( _ID_Network_Level_80 )) );
  _levels->setSelected(80, newval);
  repaint(false);
}

void ov_main::menuNetwork_Level_72( ) {
  _networkMenu->setItemChecked( _ID_Network_All_Levels, false );
  bool newval;
  _networkMenu->setItemChecked( _ID_Network_Level_72,
      (newval = ! _networkMenu->isItemChecked( _ID_Network_Level_72 )) );
  _levels->setSelected(72, newval);
  repaint(false);
}

void ov_main::menuNetwork_Other_Levels( ) {
  _networkMenu->setItemChecked( _ID_Network_All_Levels, false );
  _networkMenu->setItemChecked( _ID_Network_Other_Levels,
      ! _networkMenu->isItemChecked( _ID_Network_Other_Levels ));
  repaint(false);
}

void ov_main::menuNetwork_Layout( )
{
  if (_data->layout( ) == false)
    _layoutTimer = startTimer( LAYOUT_INTERVAL );

  _statusBar->message( "Rearranging the nodes", 10000 );
}

// ------------------------------------------------------------------
void ov_main::menuHelp_Index( )
{
  QString text;
  // Print the help text here ... perhaps in the future we can use
  //   the HTML browser widget (see www.troll.no) and store our help
  //   in HTML format.

  // for now we will just load the 'help.txt' file and show that ...
  char filename[256];
  sprintf(filename, "./help.txt");
  ifstream help(filename);
  
  while (!help.eof() && help.good()) {
    char line[256];
    help.getline(line, 256);
    text += line;
  }

  QMessageBox mb( this, "Help - Index" );
  mb.setText( text );
  mb.setButtonText( "OK" );
  mb.show( );
}

// ------------------------------------------------------------------
void ov_main::menuHelp_About( )
{
  QString text;
  
  text += " PRouST OmniVisualizer\n";
  text += " Signalling Group, NRL \n\n";

  QMessageBox mb( this, "About" );
  mb.setText( text );
  mb.setButtonText( "Dismiss" );
  mb.show( );
}

// ------------------------------------------------------------------
void abbreviatedTime(char* fixed, const double time) {
  char str[50];
  sprintf(str,"%lf",time);
  int len = strlen(str);
  bool trailingzeros=true;
  for (int i=len-1; i>=0; i--) {
    if ((str[i]=='0') && trailingzeros) {
      str[i]=0;
    }
    else if ((str[i]!='0') && trailingzeros) {
      trailingzeros = false;
      break;
    }
  }
  len = strlen(str);
  if (str[len-1]=='.') {
    str[len]   = '0';
    str[len+1] = 0;
  }
  strcpy(fixed,str);
}
