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
#ifndef __OV_PARSER_H__
#define __OV_PARSER_H__
#ifndef LINT
static char const _ov_parser_h_rcsid_[] =
"$Id: ov_parser.h,v 1.3 1999/01/13 17:37:38 mountcas Exp $";
#endif

class ov_main;
class ov_data;
class ifstream;

class ov_parser {
public:

  ov_parser( ov_main * m, ov_data * d,
	     int argc, char ** argv );
  ~ov_parser();

  int parse( int time = -1 );

  bool continuing(void) const;
  void continuing(bool b);
  
  void restart(const char * filename = 0);

  const char * input(void) const;
  double finalTime(void) const;

private:

  ov_main  * _parent;
  ov_data  * _data;

  double     _finalTime;
  char       _chunk[4][256];
  bool       _continuing;
  char     * _filename;
  ifstream * _input;
};

#endif // __OV_PARSER_H__

