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
#ifndef __ERRMSG_H__
#define __ERRMSG_H__

#ifndef LINT
static char const _pnni_errmsg_h_rcsid_[] =
"$Id: pnni_errmsg.h,v 1.4 1997/08/13 21:21:31 mountcas Exp $";
#endif

class ostream;

/** Error Message contains the data required to figure out what went wrong
    in the case of an exception.
  */
class errmsg {
  friend ostream & operator << (ostream & os, errmsg & err);
public:

  enum diag_type {
    none,
    empty_ig,
    wrong_id
  };

  enum cause {
    normal_unspecified = 0,
    incomplete_ig,
    invalid_contents,
    // Anything else
  };

  /// Constructor.
  errmsg(cause cv = normal_unspecified, diag_type dt = none, int id = 0);

  /// Destructor.
  ~errmsg();

  /**@name Accessor Methods.
   */
  //@{
    /// Returns the cause of the error.
  cause     GetCause(void) const { return _cause; }

    /// Returns the type of error.
  diag_type GetType(void) const { return _errtype; }

    /// Returns the ID of the IG that caused the error.
  int       GetID(void) const { return _errid; }
  //@}

protected:

  bool PrintSpecific(ostream & os);

  const cause _cause;
  diag_type   _errtype;
  int         _errid;
};

#endif // __ERRMSG_H__
