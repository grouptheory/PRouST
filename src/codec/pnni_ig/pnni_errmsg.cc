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
static char const _pnni_errmsg_cc_rcsid_[] =
"$Id: pnni_errmsg.cc,v 1.5 1998/08/06 04:02:52 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>
#include <codec/pnni_ig/pnni_errmsg.h>

errmsg::errmsg(cause cv, diag_type dt, int id) :
  _cause(cv), _errtype(dt), _errid(id) { }

errmsg::~errmsg() { }

bool errmsg::PrintSpecific(ostream & os)
{
  os << "! errmsg::" << endl;
  os << "     Cause::";
  switch (_cause) {
    case normal_unspecified: os << "normal unspecified"; break;
    case incomplete_ig: os << "incomplete ig"; break;
    case invalid_contents: os << "invalid contents"; break;
    default: os << "unknown"; break;
  }
  os << endl;
  os << "     Type::";
  switch (_errtype) {
    case none: os << "none"; break;
    case empty_ig: os << "empty_ie"; break;
    case wrong_id: os << "wrong_id"; break;
    default: os << "unknown"; break;
  }
  os << endl;
  os << "     ID::" << _errid << endl << endl;
  return true;
}

ostream & operator << (ostream & os, errmsg & err)
{
  err.PrintSpecific(os);
  return os;
}
