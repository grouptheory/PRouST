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
static char const _NV_application_cc_rcsid_[] =
"$Id: NV_application.cc,v 1.1 1999/01/21 14:55:00 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>
#include "NV_application.h"
#include "NV_conduit.h"
#include "NV_visitor.h"
#include "NV_fw_state.h"
#include "MainWin.h"
#include "graphfield.h"
#include <FW/kernel/Kernel_NV_Synch.h>

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
};

char * first_arg(char * argument, char * first_arg);
char * whereami(char * prog);

#define RESOURCE_FILENAME  ".nvrc"
#define OLD_RESRC_FILENAME ".gtrc"

// --------------------------------------------
NV_application & GetApp(int argc, char ** argv)
{ 
  if (!NV_application::_singleton)
    NV_application::_singleton = new NV_application(argc, argv);
  return *(NV_application::_singleton);
}

NV_application * NV_application::_singleton = 0;
//--------------------------------------------------------------
NV_application::NV_application(int argc, char **argv) 
  : _BASEY(10),
    _HCLCK(103),
    _SB_WIDTH(16), 
    _Pix_x_min(0), 
    _Pix_x_max(650),
    _Pix_y_min(0),
    _Pix_y_max(650),
    _X_max(650), _X_min(0),
    _Y_max(650), _Y_min(0),
    _Offset(57),
    _input_fd(0),
    _layout(0),
    _fws(0)
{
  char tmp[256];
  struct timeval tp;
  gettimeofday(&tp, 0);
  srand(tp.tv_sec);

  bzero(_iconFile,   FNAME_SIZE);
  bzero(_iconText,   FNAME_SIZE);
  bzero(_background, FNAME_SIZE);

  _where_am_i = whereami(argv[0]);

  int ind = 1;  bool dump = false;
  if (!strcmp(argv[1], "-d") ||
      !strcmp(argv[1], "-dump")) {
    dump = true;
    ind++;
  }

  if (!strcmp(argv[1], "-p") ||
      !strcmp(argv[1], "-pipe")) {
    _live = true;
    cout << "NV: operating in live pipe mode.\n";
    ind++;
  } else {
    _live = false;
    cout << "NV: operating in file mode.\n";
  }

  if ((_input_fd = open(argv[ind], O_RDONLY|O_NONBLOCK)) < 0) { 
    perror("NV: unable to open reading pipe");
    exit(1);
  } else {
    cout << "NV: opened reading pipe " << argv[ind] << endl;
  }

  char rev[255];
  sprintf(rev, "%s.in", argv[ind]);
  if ((_output_fd = open(rev, O_WRONLY)) < 0) { 
    if (_live) {
      cout << "NV: unable to open writing pipe " << rev << endl;
      exit(1);
    }
  } else  { 
    cout << "NV: opened writing pipe " << rev << endl;
  }

  strcpy(_input_name, argv[ind]);
  _input = new ifstream(_input_fd);
  if (! _input->good()) {
    cerr << argv[ind] << " is bad ... unable to read from file." << endl;
    exit(1);
  }

  _singleton  = this;
  _fws = FWState();
    
  if (argc >= 3)
    _layout = argv[++ind];

  _qApp       = new QApplication(argc, argv);

  // try to locate the .gtrc file
  char * username = getenv("USER");
  char rc_filename[255];
  // CWD
  sprintf(rc_filename, "./%s", RESOURCE_FILENAME);
  // HOME
  if (open(rc_filename, O_RDONLY) < 0)
    sprintf(rc_filename, "%s/%s", getenv("HOME"), RESOURCE_FILENAME);
  // Anywhere else I should look?
  if (!parseResource(rc_filename)) {
    // give it one more try
    sprintf(rc_filename, "./%s", OLD_RESRC_FILENAME);
    if (open(rc_filename, O_RDONLY) < 0)
      sprintf(rc_filename, "%s/%s", getenv("HOME"), OLD_RESRC_FILENAME); 
    if (!parseResource(rc_filename)) {
      cerr << "Unable to locate resource file (.nvrc)." << endl;
      // exit(1);
    }
  }

  // This is a top-level window containing all the NV windows
  _mainWin    = new MainWin(); // QWidget();
  // These are the child windows where the action takes place
  _visitorWin = new QListBox(_mainWin,   "VisitorList");
  _clockWin   = new QWidget(_mainWin,    "SimulationTime");
  _visitorTab = new QWidget(_mainWin,    "VisitorColors");
  _graphWin   = new GraphField(_mainWin);
  _mainWin->SetGraphField(_graphWin);

  if (*_iconFile) {
    QPixmap * icon = new QPixmap();
    char buf[256];
    sprintf(buf, "%s/%s", _path, _iconFile);
    if (icon->load(buf)) // , "XPM"))
      _mainWin->setIcon(*icon);
    else
      cerr << "ERROR: Unable to set icon to '" << buf << "'." << endl;
    delete icon;
  }
  if (*_iconText)
    _mainWin->setIconText(_iconText);
  if (*_background) {
    // first assume it's a pixmap name ... 
    QPixmap * bg = new QPixmap();
    char buf[256];
    sprintf(buf, "%s/%s", _path, _background);
    if (bg->load(buf))
      _graphWin->setBackgroundPixmap(*bg);
    else { // oops, it must've been a color
      QColor color(_background);
      _graphWin->setBackgroundColor(color);
    }
  }

  // make it start at the origin (upper left corner)
  int OFFSET = 0;

  // This should make the mainWin large enough to hold everything
  _mainWin->setGeometry(OFFSET, OFFSET,  _X_max + 2 * _Offset + 290, _Y_max + 2 * _Offset + 15);
  
  _graphWin->setGeometry(OFFSET, OFFSET, _X_max + 2 * _Offset, _Y_max + 2 * _Offset);
  _visitorWin->setGeometry(OFFSET + 10 + 2 * _Offset + _X_max, _BASEY, 0, 0 );
  _visitorTab->setGeometry(OFFSET + 10 + 2 * _Offset + _X_max, _BASEY + 8 +
			   (int)((_Y_max + 2 * _Offset - _HCLCK > 300 ) ?
				 ((_Y_max + 2 * _Offset - _HCLCK) / 3.0) : 100), 
			   0, 0 );
  _clockWin->setGeometry(OFFSET + 10 + 2 * _Offset + _X_max, _BASEY +
			 (int)((_Y_max + 2 * _Offset - _HCLCK > 300) ?
			       ((_Y_max + 2 * _Offset - _HCLCK) / 3.0) : 100) +
			 (int)((_Y_max + 2 * _Offset - _HCLCK > 300) ?
			       (2.0 * (_Y_max + 2 * _Offset - _HCLCK) / 3.0) : 100), 
			 0, 0 );
  // if this doesn't work try _graphWin
  _qApp->setMainWidget( _mainWin );

  _mainWin->show();
  _visitorWin->show();
  _visitorTab->show();
  _clockWin->show();
  _graphWin->show();

  if (dump)
    _graphWin->screen_dump(dump);
  _graphWin->SetDirty();

  // Read through the entire file looking for !event lines
  // perhaps we should just use a system call to grep and store all the events in a temp file
  char cmdbuf[256];
  sprintf(cmdbuf, "grep \"\\!event\" %s > /tmp/nv-eventlog.%d", _input_name, getpid());
  system(cmdbuf);
  // now read from /tmp/nv-eventlog.%d
  sprintf(cmdbuf, "/tmp/nv-eventlog.%d", getpid());
  ProcessEvents(cmdbuf);

  if (argc >= 4) {
    if (isdigit( argv[++ind][0] ))
      _fws->searchForEvent( atoi(argv[ind]) );
    else
      _fws->searchForEvent( eventNumber( argv[ind] ) );
  }
  ProcessInput(false);
  _qApp->exec();
}

NV_application::~NV_application( )
{
  delete _qApp;
  delete _visitorWin;
  delete _clockWin;
  delete _visitorTab;
  delete _mainWin;
  delete [] _where_am_i;
}

bool NV_application::parseResource(const char * filename)
{
  bool rval = false;
  ifstream gtrc(filename);

  sprintf(_path, "%s/pixmaps", _where_am_i);

  if (!gtrc || gtrc.bad())
    cerr << "ERROR: Unable to open '" << filename << "'" << endl;
  else {
    rval = true;
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
	    } else if (!strcmp(keyword, "background")) {
	      sscanf(line, "background %s", _background);
	    }
	  }
	  break;
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
        case 'x': // x
        case 'w': // width
	  {
	    char op[255];
	    sscanf(line, "%s %d", op, &_Pix_x_max);
	    _X_max = _Pix_x_max;
	    break;
	  }
        case 'y': // y
        case 'h': // height
	  {
	    char op[255];
	    sscanf(line, "%s %d", op, &_Pix_y_max);
	    _Y_max = _Pix_y_max;
	    break;
	  }
        default:
	  cerr << "ERROR: Unable to parse '" << line << "'" << endl;
	  break;
      }
    }
  }
  return rval;
}

void NV_application::ProcessEvents(const char * filename)
{
  ifstream infile(filename);

  if (infile.bad())
    return;

  while (infile.eof() == false) {
    char line[512];
    infile.getline(line, 512);
    if (!*line)
      continue;
    _fws->addEvent(line);
  }

  if (unlink(filename))
    cerr << "ERROR:  Unable to remove '" << filename << "'." << endl;
}

bool NV_application::ProcessInput(bool continuous)
{
  bool verdict = true;
  int rval = PARSE_NOERR, i = 1;
  char data[2048];
  int bytes = 0;

  if ( !_live ) {
    // this keeps us in a tight loop until we hit the XXX, stop searching or i reaches 0
    while (!_fws->readXXX() || (i-- > 0) || _fws->searchingForEvent()) {
      if (_input->eof() && !_fws->readXXX()) {
	cerr << "There is no 'xxxxx' line present in your data file!" << endl;
	cerr << "Your data file is most likely corrupt, please rerun " << endl
	     << "your program to obtain a new data file." << endl;
	exit(2);
      }
      _input->getline(data, 2048);
      bytes = strlen(data);
      rval = _fws->parse_and_update(data);
    }
    if ((rval == PARSE_XXX_LINE) ||
	(bytes == 0))
      verdict = false;
  } else {
    fd_set RD_SET;

    struct timeval timeout = { 0, 500000 };
    int numfds = 0;

    do {
      strcpy(data,"");
      _input->getline(data, 2048);
      bytes=strlen(data);
      rval = _fws->parse_and_update(data);

      if ((rval==PARSE_TOKEN_FOUND) || 
	  (rval==PARSE_XXX_LINE)) {
	bytes = -1;
      }

      if ((rval == PARSE_TOKEN_FOUND) && (!continuous)) 
	verdict = false;
      if (rval == PARSE_XXX_LINE)
	verdict = false;
    } while (bytes > 0);	

    if (rval == PARSE_TOKEN_FOUND) {
      GetApp().RelinquishToken();
    }
  }

  if (_input->eof());
  _mainWin->repaint(FALSE);

  return verdict;
}

void NV_application::Restart(void)
{
  if (_input)
    _input->seekg(0, ios::beg);

  VisitorRegistry()->clear();
  ConduitRegistry()->clear();
  _mainWin->repaint(FALSE);
}

QApplication * NV_application::GetQApplication(void) const
{ return _qApp; }

QListBox     * NV_application::GetVisitorWin(void) const
{ return _visitorWin; }

QWidget      * NV_application::GetClockWin(void) const
{ return _clockWin; }

QWidget      * NV_application::GetVisitorTab(void) const
{ return _visitorTab; }

GraphField   * NV_application::GetMainWin(void) const
{ return _graphWin; }

const char * NV_application::GetPath(void) const
{ return _path; }

const char * NV_application::GetInputName(void) const
{ return _input_name; }

int NV_application::GetPixXMin(void) const
{ return _Pix_x_min; }

int NV_application::GetPixXMax(void) const
{ return _Pix_x_max; }

int NV_application::GetPixYMin(void) const
{ return _Pix_y_min; }

int NV_application::GetPixYMax(void) const
{ return _Pix_y_max; }

void NV_application::SetInputFile(const char * filename)
{
  if (!filename)
    filename = _input_name;

  if ((_input_fd = open(filename, O_RDONLY )) < 0) { // | O_NDELAY | O_NONBLOCK)) < 0) {
    perror("Error, unable to open pipe: ");
    exit(1);
  }
  if (strcmp(_input_name, filename))
    strcpy(_input_name, filename);
  if (_input) delete _input;
  _input = new ifstream(_input_fd);
  if (! _input->good()) {
    cerr << _input_name << " is bad ... unable to read from file." << endl;
    exit(1);
  }
}

// Shouldn't we hold on to the token in file mode?
bool NV_application::RelinquishToken(void) 
{
  bool rval = false;
  if (_live) {
    char patched[255];
    sprintf(patched,"%s\n",TOKEN_NV_TO_KERNEL);
    
    if (_output_fd > 0) {
      cout << "NV: relinquishing token <" TOKEN_NV_TO_KERNEL  ">\n";
      cout << "NV:-------------------------------------------------\n";
      
      write( _output_fd, 
	     patched,
	     strlen(patched) );
      rval = true;
    }
  }
  return rval;
}

bool NV_application::IsLive(void) const
{
  return _live;
}

// ---------------- helper funcs -----------------------
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

// find where this program is
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
	char *filename = new char[strlen(dir) + strlen(prog) + 2]; // /+null
	strcpy(filename,dir);
	strcat(filename,"/");
	strcat(filename,prog);
	
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
