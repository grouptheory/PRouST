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
#ifndef __DIAG_H__
#define __DIAG_H__

#ifndef LINT
static char const _diag_h_rcsid_[] =
"$Id: diag.h,v 1.3 1999/02/09 17:20:51 mountcas Exp $";
#endif

#include <iostream.h>
#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>

extern "C" {
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
};


//-------------------------------------------------
class trienode {
public:

  trienode(const char * name, int level);
  virtual ~trienode();
  
public:
  
  void Print(char * accum, ostream & os, int ancestor_level);
  
  void new_child_level(int level);
  void search_and_set(const char* remaining_mask, int level);
  int  search_and_get(const char* remaining_mask);
  
private:

  int   _level;
  
  static const int MAX_NAME_LENGTH = 50;
  char  _name[MAX_NAME_LENGTH];
  
  dictionary <const char *, trienode *> _children;
};

//-------------------------------------------------
class diag_internals {
  friend void DeleteFW(void);
  friend void InitFW(void);
public:

  enum diag_level {
    DIAG_INHERITED = 0,
    DIAG_FATAL,
    DIAG_ERROR,
    DIAG_WARNING,
    DIAG_INFO,
    DIAG_DEBUG,
    DIAG_ENV
  };

  enum diag_flags {
    SHOW_NONE = 0,
    SHOW_MOD  = 1,
    SHOW_LEV  = 2,
    SHOW_ALL  = (SHOW_MOD | SHOW_LEV)
  };

  friend void diag(const char* mask, diag_internals::diag_level level, 
		   const char * str, ...);

  friend void vdiag(const char* mask, diag_internals::diag_level level,
		    const char * fmt, va_list ap);

  friend void DiagLevel(char* mask, diag_internals::diag_level level);
  friend void DiagLevel(diag_internals::diag_level level);
  friend diag_internals::diag_level DiagLevel(const char* mask);
  friend diag_internals::diag_level NameToLevel(const char * level_name);

  friend const ostream & DiagStream(ostream & os);

  friend int  DiagFlags(void);
  friend void DiagFlags(int);

  friend void DiagSetFlag(int flags);
  friend void DiagClrFlag(int flags);

  static void AppendLevelName(const char* str, int level);
  friend void PrintDiagMIB(ostream & os);

private:

  diag_internals(void);
  ~diag_internals(void);

  static const ostream & SetOutput(ostream & os);

  friend void InitDiag(void);
  friend void FreeDiag(void);

private:

  enum local_const { LEVEL_NAME_LENGTH = 50,
		     BUF_SIZ = 1024 };

  static trienode        * _root_diag_tree;
  static list<ostream *> * _os_list;
  static int               _flags;
  static char              _level_name[6][LEVEL_NAME_LENGTH];

  static bool              _diag_is_initialized;
};

//-------------------------------------------------------

#define SHOW_NONE    diag_internals::SHOW_NONE
#define SHOW_MOD     diag_internals::SHOW_MOD
#define SHOW_LEV     diag_internals::SHOW_LEV
#define SHOW_ALL     diag_internals::SHOW_ALL

#define DIAG_ENV     diag_internals::DIAG_ENV
#define DIAG_FATAL   diag_internals::DIAG_FATAL
#define DIAG_ERROR   diag_internals::DIAG_ERROR
#define DIAG_WARNING diag_internals::DIAG_WARNING
#define DIAG_INFO    diag_internals::DIAG_INFO
#define DIAG_DEBUG   diag_internals::DIAG_DEBUG

//-------------------------------------------------------

// Super diag, which allows you to execute 
// blocks of code based on diag levels.

#ifdef OPTIMIZE
#define DIAG(mod, level, code)        { if (level == DIAG_FATAL) { code; abort(); } }
#define diag(mod, level, string, ...) { if (level == DIAG_FATAL) { cout << string << endl; abort(); } }
#else
#define DIAG(mod, level, code) { if (level <= DiagLevel(mod)) { code; } if (level == DIAG_FATAL) abort(); }
#endif


void loadDiagPrefs(void);
char * stripSlash(const char * string);

#include <FW/basics/diag-mib>

                                 
#endif // __DIAG_H__extern compare(char const *const &, char const *const &);
