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
#ifndef __NV_APPLICATION_H__
#define __NV_APPLICATION_H__

#ifndef LINT
static char const _NV_application_h_rcsid_[] =
"$Id: NV_application.h,v 1.2 1999/02/24 21:47:28 mountcas Exp $";
#endif

#include <fstream.h>
#include <qapp.h>
#include <qcolor.h>
#include <qlistbox.h>
#include <qwidget.h>
#include "NV_fw_state.h"

class MainWin;
class GraphField;

// This class controls the entire application, 
//  there should only be one of these instantiated.
class NV_application {
  friend class GraphField;
  friend NV_application & GetApp(int argc = 1, char ** argv = 0);
public:

  bool ProcessInput(bool continuous);
  void ProcessEvents(const char * filename);
  bool RelinquishToken(void);
  bool IsLive(void) const;

  QApplication * GetQApplication(void) const;
  QListBox     * GetVisitorWin(void) const;
  QWidget      * GetClockWin(void) const;
  QWidget      * GetVisitorTab(void) const;
  GraphField   * GetMainWin(void) const;
  const char   * GetPath(void) const;
  const char   * GetInputName(void) const;
  
  int GetPixXMin(void) const;
  int GetPixXMax(void) const;
  int GetPixYMin(void) const;
  int GetPixYMax(void) const;

  void Restart(void);

private:

  NV_application(void);
  NV_application(int argc, char **argv);
  virtual ~NV_application();

  bool parseResource(const char * filename);
  void SetInputFile(const char * filename = 0);
  
  QApplication * _qApp;
  QListBox     * _visitorWin;
  QWidget      * _clockWin;
  QWidget      * _visitorTab;
  MainWin      * _mainWin;
  GraphField   * _graphWin;

  int            _BASEY;
  int            _HCLCK;
  int            _SB_WIDTH;

  int            _Pix_x_min;
  int            _Pix_x_max;
  int            _Pix_y_min;
  int            _Pix_y_max;
  int            _X_min;
  int            _Y_min;
  int            _X_max;
  int            _Y_max;
  int            _Offset;
  int            _input_fd;
  int            _output_fd;
  ifstream     * _input;
  const char   * _layout;
  char         * _where_am_i;
  static const int FNAME_SIZE = 255;
  char _path[FNAME_SIZE];
  char _input_name[FNAME_SIZE];
  char _iconFile[FNAME_SIZE];
  char _iconText[FNAME_SIZE];
  char _background[FNAME_SIZE];

  NV_fw_state  * _fws;

  bool _live;

  static NV_application * _singleton;
};

#endif // __NV_APPLICATION_H__
