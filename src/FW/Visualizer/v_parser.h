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
#ifndef __v_PARSER_H__
#define __v_PARSER_H__

#ifndef LINT
static char const _v_parser_h_rcsid_[] =
"$Id: v_parser.h,v 1.2 1999/02/09 17:19:42 mountcas Exp $";
#endif

#include <DS/containers/stack.h>

class v_data;
class v_conduit;
class ifstream;

#define NO_ERR            0
#define PARSE_TOKEN_FOUND 1
#define PARSE_XXX_LINE    2
#define ERR               -1

class v_parser {
public:

  v_parser( v_data * d, int argc, char ** argv );
  ~v_parser();

  // default is to rewind the file;
  void restart(const char * filename = 0);

  int parse(void);

  bool continuing(void) const;
  void continuing(bool c);

  const char * input(void) const;

private:

  v_data   * _data;
  ifstream * _inFile;
  char     * _filename;
  bool       _continuing;

  stack<v_conduit *> _current_expanders;

  static const int  _MAX_WORDS;
  char       _scan_str[64];
  double     _finalTime;
};

#endif
