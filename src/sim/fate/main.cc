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

/* -*- C++ -*-
 * File: main.cc
 * Author: mountcas
 * Version: $Id: main.cc,v 1.7 1999/02/05 16:18:25 talmage Exp $
 *
 * Usage: FC [debug] < input_file
 *
 * where debug is any string.  If it's specified, the program runs
 * in debug mode, spewing forth great gobs and chunks of internal
 * parser information.
 */
#ifndef LINT
static char const _main_cc_rcsid_[] =
"$Id: main.cc,v 1.7 1999/02/05 16:18:25 talmage Exp $";
#endif
#include <common/cprototypes.h>
#include <iostream.h>

int yylex();
int yyerror(char* s);

#include "syntax.yac.tab.c"
#include "lex.yy.c"

int yyerror(char* s)
{
  fprintf(stderr, "\n         Fatal: %s\n", s);
  fprintf(stderr, "     at Line #: %d\n", recentlineno);
  fprintf(stderr, "Offending line: %s\n", recentline);
  exit(1);
}
int yywrap(void) { return 1; }

char *LibraryFileName(const char *inputFileName)
{
  char *answer = 0;
  // Find the last slash in the file name
  char *last_slash = strrchr(inputFileName, '/');
  if (last_slash == 0) last_slash = (char *)inputFileName;
  else last_slash++;	// Skip past the slash

  // Find the last dot in the file name
  char *last_dot = strrchr(inputFileName, '.');
  if (last_dot == 0) last_dot = (char *)inputFileName + strlen(inputFileName);

  int basename_length = abs(last_slash - last_dot);
  int answer_length =  basename_length + 5; // ".lib"
  answer = new char [answer_length];

  // Copy the characters between the last slash and the last dot into
  // answer.  Don't include the slash or the dot.
  (void)strncpy(answer, last_slash, basename_length);

  // Copy the new suffix, ".lib" into answer
  (void)strcpy(answer+basename_length, ".lib");

  return answer;
}


int main(int argc, char** argv)
{
  int answer = 1;

  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <fate_script> [verbose]" << endl;
  } else {

    if (argc > 2) yydebug = 1;

    char *library_file_name = LibraryFileName(argv[1]);
    FILE *input_file = fopen(argv[1], "r");


    if (input_file != 0) {

      //
      // Open the FATE global library repository.
      // It's for writing names of libraries, one per line,
      // for compile.sh to parse and turn into arguments for ld.
      //
      v_FATE_library_file = fopen(library_file_name, "w");

      if (v_FATE_library_file != 0) {

	YY_BUFFER_STATE buf = yy_create_buffer(input_file, YY_BUF_SIZE);
	if (buf != 0) {
	  yy_switch_to_buffer(buf);
	  answer = yyparse();
	  yy_delete_buffer(buf);
	} else {
	  cerr << argv[0] << ": Bison can't create a buffer for" 
	       << library_file_name << endl;
	}

	fclose(v_FATE_library_file);
      } else {
	cerr << argv[0] << ": can't open library file " 
	     << library_file_name << endl;
      }

      fclose(input_file);
    } else {
      cerr << argv[0] << ": can't open file " << argv[1] << endl; 
    }

    delete [] library_file_name;
  }

  return answer;
}
