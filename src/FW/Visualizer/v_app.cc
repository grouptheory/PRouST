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
static char const _v_app_cc_rcsid_[] =
"$Id: v_app.cc,v 1.3 1999/02/09 17:19:03 mountcas Exp $";
#endif

#include "v_app.h"
#include "v_main.h"
#include "v_data.h"
#include "v_parser.h"
#include "v_conduit.h"
#include "v_visitor.h"
#include "HelpSearchDlg.h"

#include <qmenubar.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>
#include <qkeycode.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>
#include <qpixmap.h>
#include <qfiledialog.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qslider.h>

// -----
#include "pixmaps/open_icon.xpm"
#include "pixmaps/save_icon.xpm"
#include "pixmaps/print_icon.xpm"
#include "pixmaps/zoom_in_icon.xpm"
#include "pixmaps/zoom_out_icon.xpm"
#include "pixmaps/layout_icon.xpm"
#include "pixmaps/stop_icon.xpm"
#include "pixmaps/step_icon.xpm"
#include "pixmaps/cont_icon.xpm"
#include "pixmaps/restart_icon.xpm"
// -----

#include <common/cprototypes.h>
#include <iostream.h>
#include <fstream.h>
#include <sys/stat.h>
#include <fcntl.h>

// -----------------------------
int main(int argc, char ** argv)
{
  struct timeval tp;
  gettimeofday(&tp, 0);
  srand( tp.tv_usec );

  QApplication a( argc, argv );
  v_app * mw = new v_app( argc, argv, a );
  a.setMainWidget(mw);
  mw->show();
  a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
  return a.exec();
}

// ---------------------------------------------------------------
char * first_arg(char * argument, char * first_arg);
char * whereami(char * prog);

v_app & theApplication(void)
{
  // this may not be called until the ctor has been called in main
  assert( v_app::_instance != 0 );
  return *(v_app::_instance);
}

v_app   * v_app::_instance = 0;
const int v_app::PARSE_INTERVAL = 250;

v_app::v_app(int argc, char ** argv, QApplication & app) 
  : QMainWindow( 0, argv[0] ), 
    _menuBar(0), _toolBar(0), _statusBar(0), 
    _centralWidget(0), _fileMenu(0), _controlMenu(0),
    _optionsMenu(0), _helpMenu(0), _tb(-1), _sb(-1), 
    _ifd(-1), _where_am_i(0), _app(app), _data(0),
    _parseTimer(0)
{ 
  assert( _instance == 0 );

  _toolBar   = new QToolBar( this, "" );
  assert( _toolBar != 0 );
  _menuBar   = menuBar();
  assert( _menuBar != 0 );
  _statusBar = statusBar();
  assert( _statusBar != 0 );
  
  bzero(_path, 256);
  _where_am_i = whereami( argv[0] );

  resize(1280, 1024);

  _instance = this;

  // ----- Allocate Icons for Menu/Tool Bars -----
  QPixmap openIcon   = QPixmap( open_icon ),
          saveIcon   = QPixmap( save_icon ),
          printIcon  = QPixmap( print_icon ),
          zoomIcon   = QPixmap( zoom_in_icon ),
          unzoomIcon = QPixmap( zoom_out_icon ),
          layoutIcon = QPixmap( layout_icon ),
          stopIcon   = QPixmap( stop_icon ),
          stepIcon   = QPixmap( step_icon ),
          contIcon   = QPixmap( cont_icon ),
          restIcon   = QPixmap( restart_icon );

  // ----- Allocate Menu Bar Buttons -----

  // File Menu
  _fileMenu = new QPopupMenu( );
  assert( _fileMenu != 0);
  _fileMenu->insertItem( openIcon,  "Open",     this, SLOT(open()),     CTRL + Key_O );
  _fileMenu->insertItem( saveIcon,  "Save",     this, SLOT(save()),     CTRL + Key_S );
  _fileMenu->insertSeparator( );
  _fileMenu->insertItem( printIcon, "Print",    this, SLOT(print()),    CTRL + Key_P );
  _fileMenu->insertSeparator( );
  _fileMenu->insertItem(            "Quit",     this, SLOT(quit()),     CTRL + Key_Q );
  // Notify the Menu Bar of the File Menu
  _menuBar->insertItem( "&File", _fileMenu );

  // Control Menu
  _controlMenu = new QPopupMenu( );
  assert( _controlMenu != 0);
  _controlMenu->insertItem( stepIcon,   "Step",     this, SLOT(step( )),     Key_Space     );
  _controlMenu->insertItem( contIcon,   "Continue", this, SLOT(cont( )),     Key_Return    );
  _controlMenu->insertItem( stopIcon,   "Stop",     this, SLOT(stop( )),     Key_Pause     );
  _controlMenu->insertItem( restIcon,   "Restart",  this, SLOT(restart( )),  Key_Backspace );
  _controlMenu->insertSeparator( );
  _controlMenu->insertItem( layoutIcon, "Layout",   this, SLOT(layout( )),   CTRL + Key_L );
  _controlMenu->insertSeparator( );
  _controlMenu->insertItem( zoomIcon,   "Zoom In",  this, SLOT(zoom_in( )),  CTRL + Key_Z );
  _controlMenu->insertItem( unzoomIcon, "Zoom Out", this, SLOT(zoom_out( )), CTRL + Key_U );
  // Notify the Menu Bar of the Control Menu
  _menuBar->insertItem( "&Control", _controlMenu );

  // Options Menu
  _optionsMenu = new QPopupMenu( );
  assert( _optionsMenu != 0);
  _tb = _optionsMenu->insertItem( "Tool Bar",   this, SLOT(toggleToolBar()) );
  _sb = _optionsMenu->insertItem( "Status Bar", this, SLOT(toggleStatusBar()) );
  _optionsMenu->setCheckable( TRUE );
  _optionsMenu->setItemChecked( _tb, TRUE );
  _optionsMenu->setItemChecked( _sb, TRUE );
  // Notify the Menu Bar of the Options Menu
  _menuBar->insertItem( "&Options", _optionsMenu );

  _helpMenu = new QPopupMenu( );
  assert( _helpMenu != 0);
  _helpMenu->insertItem( "Index",  this, SLOT(help_index()), CTRL + Key_H );
  _helpMenu->insertItem( "Search", this, SLOT(help_search()));
  _helpMenu->insertItem( "About",  this, SLOT(about()),      CTRL + Key_A );
  // Notify the Menu Bar of the Help Menu
  _menuBar->insertItem( "&Help", _helpMenu );

  // ----- Allocate Tool Bar Buttons -----
  QToolButton * opent = new QToolButton(openIcon, "Open", 0, this, 
                                        SLOT(open()), _toolBar, "open");
  QToolButton * savet = new QToolButton(saveIcon, "Save", 0, this,
                                        SLOT(save()), _toolBar, "save");
  QToolButton * printt = new QToolButton(printIcon, "Print", 0, this,
                                         SLOT(print()), _toolBar, "print");
  _toolBar->addSeparator( );
  QToolButton * restt = new QToolButton(restIcon, "Restart", 0, this, 
					SLOT(restart( )), _toolBar, "restart" );
  QToolButton * stept = new QToolButton(stepIcon, "Step", 0, this, 
					SLOT(step()), _toolBar, "step");
  QToolButton * stopt = new QToolButton(stopIcon, "Stop", 0, this, 
					SLOT(stop()), _toolBar, "stop");
  QToolButton * contt = new QToolButton(contIcon, "Continue", 0, this, 
					SLOT(cont()), _toolBar, "continue");
  _toolBar->addSeparator( );
  QToolButton * layoutt = new QToolButton(layoutIcon, "Layout", 0, this,
                                          SLOT(layout()), _toolBar,
                                          "rearrange switches");
  QToolButton * zoomt = new QToolButton(zoomIcon, "Zoom", 0, this,
                                        SLOT(zoom_in()), _toolBar, "zoom");
  QToolButton * unzoomt = new QToolButton(unzoomIcon, "UnZoom", 0, this,
                                          SLOT(zoom_out()), _toolBar, "unzoom");

  // Add the tool tips for the above
  QWhatsThis::add( opent,   "Click this button to open a simulation.", FALSE );
  QWhatsThis::add( savet,   "Click this button to save the layout.", FALSE );
  QWhatsThis::add( printt,  "Click this button to print the simulation.", FALSE );

  QWhatsThis::add( stept,   "Click this button to parse one line of the simulation.", FALSE );
  QWhatsThis::add( stopt,   "Click this button to stop a running simulation.", FALSE );
  QWhatsThis::add( contt,   "Click this button to continue a simulation.", FALSE );

  QWhatsThis::add( layoutt, "Click this button to rearrange the nodes in the simulation.", FALSE );
  QWhatsThis::add( zoomt,   "Click this button to zoom in on the simulation.", FALSE );
  QWhatsThis::add( unzoomt, "Click this button to zoom out of the simulation.", FALSE );

  QWhatsThis::whatsThisButton( _toolBar );

  // ----- Allocate the Data Container -----
  _data = new v_data( );

  // ----- Allocate Central Widget -----
  _centralWidget = new v_main( this, width(), height() );
  _centralWidget->setFocus();
  setCentralWidget( _centralWidget );
  _statusBar->message( "Ready", 2000 );

  // ----- Allocate the Parser -----
  _parser = new v_parser( _data, argc, argv );

  // ----- Allocate the Slider -----
#if 0
  _slider = new QSlider( this );
  // need to much with it's geometry
  _slider->setOrientation( QSlider::Horizontal );
  _slider->setTickmarks( QSlider::Below );
  _slider->setRange( 0, 99 );
  _slider->setSteps( 1, 10 );
  _slider->setValue( 0 );
  _slider->setGeometry( 20, height() - 50,
			width() - 50, 25 );
  connect( _slider, SIGNAL(valueChanged(int)), SLOT(sliderChanged(int)) );
#endif

  loadResources( );
}

v_app::~v_app() 
{
  delete _data;
  delete _centralWidget;
  delete _parser;
  delete [] _where_am_i;
}

v_data * v_app::getData(void) const
{
  return _data;
}

v_main * v_app::centralWidget(void) const
{
  return _centralWidget;
}

const char * v_app::path(void) const
{
  return _path;
}

const char * v_app::input(void) const
{
  return _parser->input();
}

void v_app::notify(double t)
{
  int time = (int)t + 1;
  //  _slider->setRange( 0, time );
}

void v_app::updateSlider(double t)
{
  //  _slider->setValue( (int)t );  
}

void v_app::searchHelp(const char * keyword) const
{
  QMessageBox mb( (QWidget *)this, (const char *)"Search Results" );
  QString text;

  text += "\nNo help found on '";
  text += keyword;
  text += "'.\n";

  mb.setText( text );
  mb.setButtonText( "OK" );
  mb.show( );
}

void v_app::loadResources(void)
{
  char filename[256];
  // try to locate a file like .gtrc/.nvrc/etc.
  sprintf(filename, "%s/.nvrc", getenv("HOME"));
  ifstream gtrc(filename);

  sprintf(_path, "%s/pixmaps", _where_am_i);

  if (!gtrc || gtrc.bad())
    cerr << "ERROR: Unable to open '" << filename << "' -- Using defaults." << endl;
  else {
    while (!gtrc.eof()) {
      char line[1024];
      gtrc.getline(line, 1024);
      if (!*line)
	continue;
      // parse the line
      switch (line[0]) {
        case '#': // comment
	  break;
        case 'p': // pixmap path
	  sscanf(line, "path %s", _path);
	  break;
        case 'b':
	  {
	    char keyword[256];
	    first_arg(line, keyword);
	    if (!strcmp(keyword, "bitvec")) {  // bitvec
	      char tmp[256];
	      sscanf(line, "bitvec %s", tmp);
	      
	      if (ConduitRegistry()->SetFlag( tmp ) == false)
		cerr << "ERROR:  Unknown bit vector name '" << tmp << "'." << endl;
	    }
	  }
	  break;
        case 'x': // x
        case 'w': // width
	  {
	    char op[255];
	    int width = -1;
	    sscanf(line, "%s %d", op, &width);
	    resize( width, height() );
	    break;
	  }
        case 'y': // y
        case 'h': // height
	  {
	    char op[255];
	    int height = -1;
	    sscanf(line, "%s %d", op, &height);
	    resize( width(), height );
	    break;
	  }
        case 'c': // visitor color
	  {
	    char visname[255], col[255];
	    sscanf(line, "color %s %s", visname, col);
	    // col is either a one word color description or a hexadecimal value
	    QColor color(col);  // Assume it's a word desc
	    if (*col == '#') { // hex value
	      int hx = 0;
	      sscanf(col, "#%lx", &hx);
	      color.setRgb((hx & 0xFF0000) >> 16,
			   (hx & 0x00FF00) >> 8,
			   (hx & 0x0000FF));
	    }
	    VisitorColorsTable()->register_visitor_color(visname, color);
	    break;
	  }
        case 'i':
	  {
	    char keyword[256];
	    first_arg(line, keyword);
	    if (!strcmp(keyword, "image")) {  // specific pixmap for conduit type 
	      char condname[255], pixname[255];
	      sscanf(line, "image %s %s", condname, pixname);
	      ConduitPixmapsTable()->register_conduit_pixmap(condname, pixname);
	    } else if (!strcmp(keyword, "icon")) // icon for NV
	      sscanf(line, "icon %s", _iconFile);
	    else if (!strcmp(keyword, "iconText"))
	      sscanf(line, "iconText %s", _iconText);
	    break;
	  }
        default:
	  cerr << "ERROR: Unable to parse '" << line << "'" << endl;
	  break;
      }
    }
  }
}

void v_app::timerEvent( QTimerEvent * te )
{
  if ( te->timerId() == _parseTimer ) {
    killTimer( _parseTimer );    
    if ( _parser->input() == 0 ) {
      QMessageBox mb( this, "Error" );
      QString text = "\nThere is no data to be parsed!\nTry opening a data file.\n";
      mb.setText( text );
      mb.setButtonText( "OK" );
      mb.show( );
    } else {
      int rval = -1;
      if ((rval = _parser->parse()) == 0)
	_centralWidget->repaint( false );
      else if (rval == PARSE_XXX_LINE)
	_statusBar->message( "Read initial kernel time stamp, stopping", 5000 );
      
      if (_parser->continuing())
	_parseTimer = startTimer( PARSE_INTERVAL );
    }
  }
}

void v_app::resizeEvent( QResizeEvent * re )
{
  const QSize & sz = re->size();
  _centralWidget->resize( sz.width(), sz.height() );
}

void v_app::keyPressEvent( QKeyEvent * ke )
{
  _centralWidget->keyPressEvent( ke );
}

void v_app::sliderChanged(int i)
{
  _centralWidget->setTime( i );
}

void v_app::open( )
{
  QString fn = QFileDialog::getOpenFileName( 0, "*.vis", this );

  if ( !fn.isEmpty() ) {
    _data->clear( );
    _parser->restart( fn );
    _centralWidget->repaint( false );
  } else
    _statusBar->message( "Loading aborted", 2000 );
}

void v_app::save( )
{
  if (_data->empty())
    _statusBar->message( "There is no layout information to save!", 2000 );
  else {
    QString fn = QFileDialog::getSaveFileName( 0, "*.lay", this );

    if ( !fn.isEmpty() )
      _data->save( fn );
    else
      _statusBar->message( "Saving aborted", 2000 );
  }
}

void v_app::print( )
{
  if (_data->empty())
    _statusBar->message( "There is nothing to print!", 2000 );
  else {
    QPrinter * printer = new QPrinter( );
    printer->setPageSize( QPrinter::Letter );
    // How the hell do I get this to list printers?
    //   The documentation isn't clear at all!
    if (printer->setup(this)) {
      QPainter p;
      p.begin( printer );
      p.setFont( _centralWidget->font() );
      p.setViewport( 0, 0, 350, 650 );
      // CRAP!
      p.end();             // Send the job off to the printer
      _statusBar->message( "Printing completed", 2000 );
    } else
      _statusBar->message( "Printing aborted", 2000 );
  }
}

void v_app::toggleStatusBar( )
{
  if ( _statusBar->isVisible() ) {
    _statusBar->hide();
    _optionsMenu->setItemChecked( _sb, FALSE );
  } else {
    _statusBar->show();
    _optionsMenu->setItemChecked( _sb, TRUE );
  }
}

void v_app::toggleToolBar( )
{
  if ( _toolBar->isVisible() ) {
    _toolBar->hide();
    _optionsMenu->setItemChecked( _tb, FALSE );
  } else {
    _toolBar->show();
    _optionsMenu->setItemChecked( _tb, TRUE );
  }
}

void v_app::help_index( )
{
  QString text;
  // Print the help text here ... perhaps in the future we can use
  //   the HTML browser widget (see www.troll.no) and store our help
  //   in HTML format.

  // for now we will just load the 'help.txt' file and show that ...
  char filename[256];
  sprintf(filename, "%s/help.txt", _path);
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

void v_app::help_search( )
{
  HelpSearchDlg * hsd = 
    new HelpSearchDlg( this, "Help - Search", this );
  hsd->show();
}

void v_app::about( )
{
  QString text;
  
  text += " FVT (Framework Visualization Tool) v0.01 \n\n";
  text += " Signaling Group, NRL \n\n";

  QMessageBox mb( this, "About" );
  mb.setText( text );
  mb.setButtonText( "Dismiss" );
  mb.show( );
}

void v_app::zoom_in( )
{
  _centralWidget->zoom_in( );
  _statusBar->message( "Zooming In", 2000 );
}

void v_app::zoom_out( )
{
  _centralWidget->zoom_out( );
  _statusBar->message( "Zooming Out", 2000 );
}

void v_app::layout( )
{
  _data->layout( );
  _statusBar->message( "Rearranging the nodes", 10000 );
}

void v_app::stop( )
{
  _parser->continuing(false);
  _centralWidget->stopTimers( );
  _statusBar->message( "Stopping", 2000 );
}

void v_app::cont( )
{
  if ( _parseTimer != 0 )
    killTimer( _parseTimer );
  _parseTimer = startTimer( PARSE_INTERVAL );
  _parser->continuing(true);
  _statusBar->message( "Continuing", 10000 );
}

void v_app::step( )
{
  _parseTimer = startTimer( PARSE_INTERVAL );
  _statusBar->message( "Stepping", 2000 );
}

void v_app::restart( )
{
  _parser->restart();
  _statusBar->message( "Restarting the simulation", 2000 );
}

void v_app::quit( )
{
  _app.quit();
}

// ------------------------------------------------
#define LOWER(c)  (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))

void skip_spaces(char **string)
{
  for (; **string && isspace(**string); (*string)++);
}

char * first_arg(char * argument, char * first_arg)
{
  // clear all whitespace
  skip_spaces(&argument);

  while (*argument && !isspace(*argument)) {
    *(first_arg++) = LOWER(*argument);
    argument++;
  }

  *first_arg = '\0';

  return argument;
}

char * whereami(char * prog) 
{
  char * dirname = 0;

  // if prog contains / then it is just the longest dir portion
  char * dir = strrchr(prog,'/');
  if (dir) {
    int size = 1 + (dir-prog); // ? 1 + or NOT ?
    dirname = new char[1+dir-prog];
    strncpy(dirname,prog,size);
    dirname[size-1]='\0';
  } else {
    // if there are no / in prog then we have to search PATH
    char * path = getenv("PATH");
    char * PATH = new char [ strlen(path) + 1 ];
    strcpy(PATH, path);
    
    if (PATH) {
      char *dir = strtok(PATH,":");
      while (dir) {
        // construct the file as it would appear in each diriectory in path
        char * filename = new char[ strlen(dir) + strlen(prog) + 2]; // /+null
        strcpy(filename, dir);
        strcat(filename, "/");
        strcat(filename, prog);
        
        // check if there is such a file and is it executable
        struct stat buf;
        int err = stat(filename, &buf);
        if (err == 0 && S_ISDIR(buf.st_mode) == 0 ) {
          if (buf.st_mode & S_IEXEC) // executable for owner
            return whereami(filename);
        }
        delete [] filename;
        
        // advance to the next directory
        dir = strtok(0,":");
      }
    }
    delete [] PATH;
  }
  return dirname;
}
