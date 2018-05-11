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
static char const _trinode_cc_rcsid_[] =
"$Id: trinode.cc,v 1.1 1999/01/21 14:55:54 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "trinode.h"
#include "ShareableRegistry.h"
#include <FW/basics/diag.h>

template < class T >
trinode< T >::trinode(const char * name, T val)
  : _val(val), _blocking(false)
{
  assert(name);
  strncpy(_name, name, 255);
}
 
template < class T >
trinode< T >::~trinode( ) 
{ 
  if (_children.empty() == false) {
    dic_item di;
    forall_items(di, _children) {
      trinode<T> * child = _children.inf(di);
      child->set_parent(0);
    }
  }
}

template < class T >
T trinode< T >::data(void) const
{ 
  return _val;
}

template < class T >
const char * trinode< T >::name(void) const
{
  return _name;
}

template < class T >
bool trinode< T >::blocking(void) const
{
  return _blocking;
}

template < class T >
void trinode< T >::set_blocking(bool b)
{
  _blocking = b; 
}

template < class T >
void trinode< T >::add_child(trinode * t)
{
  dic_item di;
  if (di = _children.lookup(t->name())) {
    diag("FW.interface", DIAG_FATAL, 
	 "Attempting to insert the same child (%s) into %s more than once!\n",
	 t->name(), name());
  }
  _children.insert(t->name(), t);
  t->set_parent(this);
  diag("FW.interface", DIAG_DEBUG,
       "Inserting %s into the hierarchy below %s.\n", t->name(), name());
}

template < class T >
void trinode< T >::rem_child(trinode * t)
{
  dic_item di;
  if (di = _children.lookup(t->name())) {
    _children.inf(di)->set_parent(0);
    _children.del_item(di);
    diag("FW.interface", DIAG_DEBUG,
	 "Removing %s from below %s.\n", t->name(), name());
  }
}

template < class T >
const dictionary<const char *, trinode<T> *> & trinode< T >::children(void) const
{
  return _children;
}

template < class T >
trinode<T> * trinode< T >::parent(void) const
{
  return _parent;
}

template < class T >
void trinode< T >::set_parent(const trinode * p)
{
  _parent = (trinode *)p;
}

template < class T >
void trinode< T >::clear_children(void)
{
  _children.clear();
}
