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
#ifndef __VTYPE_H__
#define __VTYPE_H__

#ifndef LINT
static char const _VisitorType_h_rcsid_[] =
"$Id: VisitorType.h,v 1.1 1999/01/21 14:56:25 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <iostream.h>

class vistype;
class VisitorType;

class Registry;

class vistype {
  friend void InitFW(void);
  friend void DeleteFW(void);
  friend class VisitorType;
public:

  vistype(const char * const name);
  ~vistype();

  friend int compare(vistype const *const & x, vistype const *const & y);
  friend int compare(vistype * const & x, vistype * const & y);

  friend ostream & operator << (ostream & os, const vistype & v);

  vistype & derived_from(vistype & vistype) {
    if (!_parents->search(&vistype)) 
      _parents->append(&vistype);
    return vistype;
  }
  
private:

  bool Is_A(const vistype* other) const;

  bool Is_Exactly(const vistype* other) const;

  const char * const Name (void) const;

  static Registry* _registry;
  list<vistype*> * _parents;
};


class VisitorType {
public:

  VisitorType(const vistype & vt);
  ~VisitorType();
  
  friend ostream& operator << (ostream & os, const VisitorType & v);

  bool Is_Exactly(const VisitorType* other) const;
  bool Is_A(const VisitorType* other) const;
  const char * const Name (void) const;

private:

  const vistype* _type;
};

#endif // __VTYPE_H__
