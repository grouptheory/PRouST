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
#ifndef __CASINO_STRING_H__
#define __CASINO_STRING_H__
#ifndef LINT
static char const _String_h_rcsid_[] =
"$Id: String.h,v 1.9 1999/02/19 21:22:59 marsh Exp $";
#endif

class ostream;

class ds_String {
public:

  ds_String();
  ds_String(const char *str);
  ds_String(const ds_String &him);
  ~ds_String();
  ds_String & operator += (const char * str);
  ds_String & operator += (char c);
  ds_String & operator += (const ds_String & str);
  const char * chars(void);
  int size(void) const;

  friend ostream & operator << (ostream & os, const ds_String & him);

private:

  void grow( int size );

  int _size;
  int _occupied;
  char * _buf;
};

#endif
