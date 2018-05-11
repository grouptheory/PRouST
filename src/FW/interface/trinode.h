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
#ifndef __TRINODE_H__
#define __TRINODE_H__

#ifndef LINT
static char const _trinode_h_rcsid_[] =
"$Id: trinode.h,v 1.4 1999/02/09 18:45:07 mountcas Exp $";
#endif

#include <DS/containers/dictionary.h>
class Shareable;

#define TRINODE_MAX_NAME_LENGTH 256

// NOTE: The misspelling is intentional, so as not to conflict with diag's trinode
template < class T >
class trinode {
public:

  trinode(const char * name, T val);
  ~trinode( );

  T            data(void) const;
  const char * name(void) const;

  bool blocking(void) const;
  void set_blocking(bool b);

  void add_child(trinode * t);
  void rem_child(trinode * t);

  const dictionary<const char *, trinode<T> *> & children(void) const;
  trinode<T> * parent(void) const;
  void set_parent(const trinode * p);

  void clear_children(void);

private:

  T    _val;
  char _name[ TRINODE_MAX_NAME_LENGTH ];
  bool _blocking;

  dictionary<const char *, trinode<T> *> _children;
  trinode<T>                           * _parent;
};

#endif // __TRINODE_H__
