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
static char const _Log_cc_rcsid_[] =
"$Id: Log.cc,v 1.1 1999/01/21 14:56:25 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "Log.h"
#include <FW/kernel/Kernel.h>

extern "C" {
#include <fcntl.h>
};

list<char *> * Log::_record = 0;
bool           Log::_pipe_on = false;
ofstream     * Log::_pipe = 0;
bool           Log::_use_fd_not_stream = false;
int            Log::_out_fd = -1;
int            Log::_event_number = 0;
Log          * Log::_instance = 0;
dictionary<const char *, ostream *> Log::_logs;

Log & theLog(void) 
{
  if (Log::_instance == 0)
    Log::_instance = new Log( );
  return *(Log::_instance);
}

ostream * GetLog(const char * name)
{
  ostream * rval = 0;

  dic_item di;
  if (di = Log::_logs.lookup(name))
    rval = Log::_logs.inf(di);
  else
    Log::_logs.insert(strdup(name), rval = new ofstream(name));

  return rval;
}

Log::Log(void)
{ 
  _record = new list<char *> ;
  _pipe = new ofstream( );
}

Log::~Log() 
{ 
  ClearRecord();
  delete _record;

  if (_pipe) {
    _pipe->close();
    delete _pipe;
  }
}

void Log::AppendToRecord(const char * str)
{
  if (!str || !strcmp(str,""))
    return;

  if (_pipe_on) {
    if (_use_fd_not_stream) {
      char eolterminated[255];
      strcpy(eolterminated, str);
      strcat(eolterminated, "\n");
      write(_out_fd, eolterminated, strlen(eolterminated));
    } else {
      (*(_pipe)) << str << endl << flush;
    }
  } else if (getenv("LOGGING_ON") != 0) {
    // text is not appended if it's going out to the pipe
    char * text = new char [strlen(str) + 1];
    strcpy(text, str);
    _record->append(text);
  }
}

void Log::ClearRecord(void)
{
  if ((!Log::_record) ||
      (Log::_record->empty())) 
    return;

  list_item li;
  forall_items(li, *Log::_record) {
    char * st = Log::_record->inf(li);
    delete [] st;
  }
  Log::_record->clear();
}

void DumpRecord(ostream & os)
{
  if ((!Log::_record)  ||
      (Log::_record->empty()))
    return;
  
  list_item li;
  forall_items(li, *Log::_record) {
    char * st = Log::_record->inf(li);
    os << st << endl;
  }
  os << endl;
}

void strip_newlines(char * event)
{
  for (int i = 0; i < strlen(event); i++)
    if (event[i] == '\n')
      event[i] = ' ';
}

void AppendCommentToLog(const char * comment)
{
  Log & log = theLog();

  char tmp[1024];
  sprintf(tmp, "# %s", comment);
  strip_newlines(tmp);
  log.AppendToRecord(tmp);
}

void AppendEventToLog(const char * event)
{
  Log & log = theLog();

  char tmp[1024];
  sprintf(tmp, "!event %s:%d", event, log._event_number++);
  strip_newlines(tmp);
  log.AppendToRecord(tmp);
}

bool VisPipe(char * pipename)
{
  Log & log = theLog();

  if (!pipename || !*pipename) {
    if (log._pipe_on)
      (log._pipe)->close();
    log._pipe_on = false;
  } else {
    (log._pipe)->open(pipename);
    log._use_fd_not_stream = false;

    if (log._pipe->bad())
      log._pipe_on = false;
    else
      log._pipe_on = true;
  }
  return log._pipe_on;
}

void Visualize(char * pipename)
{
  char inpipe[255];
  sprintf(inpipe, "%s.in", pipename);
  theKernel().Set_Lockstep_Mode_On ( inpipe );    // opens the FW read path

  pid_t pid;
  if ((pid = fork()) == 0) {
    // child process
    char buf[1024];
    char * ptr = getenv("NV_BINARY");
    if (ptr) {
      sprintf(buf, "%s -p %s", ptr, pipename);      // NV opens its read path, write path
      system(buf);
    } else {
      cout << "Visualize() failed: You need to set your NV_BINARY" 
	   << " environment variable to be the full path of NV." << endl;
    }
    theKernel().Set_Lockstep_Mode_Off();
    exit(0);
  }
  Log & log = theLog();
  log._use_fd_not_stream = true;
  log._out_fd = open(pipename, O_WRONLY|O_NDELAY);
  log._pipe_on = true;
}
