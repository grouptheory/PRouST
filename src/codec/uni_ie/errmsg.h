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
#ifndef _ERRMSG_H_
#define _ERRMSG_H_

#ifndef LINT
static char const _errmsg_h_rcsid_[] =
"$Id: errmsg.h,v 1.3 1997/10/30 13:56:01 mountcas Exp $";
#endif

#include <codec/uni_ie/cause.h>

class ie_errmsg {
  friend ostream & operator << (ostream & os, ie_errmsg & x);

public:

  enum error {
    ERROR = -1,
    OK    = 0
  };
  enum diag_type {
    none,
    empty_ie,
    wrong_id
  };

  ie_errmsg(ie_cause::CauseValues cv = ie_cause::normal_unspecified, diag_type dt = none, int id = 0);

  virtual char PrintSpecific(ostream & os);

  const int _cause;
  diag_type _errtype;
  int       _errid;
};


#endif
