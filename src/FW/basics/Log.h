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
#ifndef __LOG_H__
#define __LOG_H__

#ifndef LINT
static char const _Log_h_rcsid_[] =
"$Id: Log.h,v 1.1 1999/01/21 14:56:25 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
#include <fstream.h>

class Conduit;
class Kernel;
class Visitor;
class Actor;
class Expander;
class Kernel_Governor;
class Debugger;
class Cluster;
class Control;

class Log {
  friend class Conduit;
  friend class Kernel;
  friend class Visitor;
  friend class Actor;
  friend class Expander;
  friend class Kernel_Governor;
  friend class Debugger;
  friend class Cluster;
  friend class Control; // Special case, don't do this
  friend bool Join(Conduit* c1, Conduit* c2, Visitor* v1, Visitor* v2);
  friend bool Sever(Conduit* c1, Conduit* c2, Visitor* v1, Visitor* v2);
  friend bool VisPipe(char * pipename);
  friend void Visualize(char * pipename);
  friend void AppendCommentToLog(const char * comment);
  friend void AppendEventToLog(const char * event);
  friend void DumpRecord(ostream & os);
  friend Log & theLog(void);
  friend ostream * GetLog(const char * name);
private:

  void AppendToRecord(const char * stuffToAppend);
  void ClearRecord(void);

  Log(void);
  ~Log();

  static list<char *> * _record;
  static bool           _pipe_on;
  static ofstream     * _pipe;
  static bool           _use_fd_not_stream;
  static int            _out_fd;
  static Log          * _instance;
  static int            _event_number;

  static dictionary<const char *, ostream *> _logs;
};

#endif
