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
#ifndef __REGISTRY_H__
#define __REGISTRY_H__

#ifndef LINT
static char const _Registry_h_rcsid_[] =
"$Id: Registry.h,v 1.1 1999/01/21 14:56:25 mountcas Exp $";
#endif

#include <DS/containers/dictionary.h>
#include <DS/containers/stack.h>

class Actor;
class Visitor;
class VisitorType;
class vistype;
class Expander;

extern "C" {
  void mallocErrHandler(void);
};

class Registry {
  friend class Actor;
  friend class Visitor;
  friend class vistype;
  friend class Conduit;
  friend class Expander;
  friend void  InitFW(void);
  friend void  DeleteFW(void);
  friend const VisitorType * const QueryRegistry(const char * const n);
private:

  Registry(void);
  ~Registry();

  void Insert(const char * const name,const vistype* v);
  const vistype * _Insert(const char * const name,const vistype* v);
  const vistype * Lookup(const char * const name);

  const char * const GetName(const vistype* v);
  
  dictionary<const char * , const vistype *> * _map;
  dictionary<const vistype *, const char *>  * _invmap;
  stack<Expander *> _expanders;
};


#endif // __REGISTRY_H__
