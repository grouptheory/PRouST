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
#ifndef __LIST_H__
#define __LIST_H__

#ifndef LINT
static char const _list_h_rcsid_[] =
"$Id: list.h,v 1.30 1999/01/26 22:44:24 mountcas Exp $";
#endif

#include <DS/containers/memory.h>

class list_node {
public:

  // -----------------------------
  list_node(void) 
    : _next(0), _prev(0), _data(0), _owner(0) { }

  // -----------------------------
  list_node(void* owner, void * x, 
	    list_node * n = 0, 
	    list_node * p = 0) 
    : _next(n), _prev(p), _data(x), _owner(owner) { }

  // -----------------------------
  ~list_node() { 
    _next  = 0;
    _prev  = 0;
    _data  = 0;
    _owner = 0;
  }

  // -------- data --------
  list_node * _next;
  list_node * _prev;
  void	    * _data;
  void	    * _owner;
};

typedef list_node * list_item;

//template <class T> int compare(const T & x, const T & y);

#ifndef forall_items
#define forall_items( y, x ) \
  for ( (y) = (x).first() ; (y) ; (y) = (x).next(y) )
#endif

    enum dir {
      before   = 1,
      after    = 2,
      forward  = 3,
      backward = 4
    };

template <class T> class list {
public:

  // Default ctor
  list(void);
  // Does node by node copy. This is so constructed objects
  // that have list data will copy correctly.
  list(const list & rhs);
  // dtor
  virtual ~list();

  // assignment op
  list & operator = (const list & rhs);

  // Accessor methods
  int  length(void) const;
  int  size(void) const;
  bool empty(void) const;
  T    contents(list_item it) const;
  T &  inf(list_item it) const;

  const T & operator [] (list_item it) const;
  T & operator [] (int index) const;

  list & operator &= (const list & rhs);
  list & operator += (const list & rhs);

  list_item search(const T item) const;
  list_item lookup(const T item) const;
  int       rank(const T & item) const;
  int       rank(const list_item it) const;

  T head(void) const;
  T tail(void) const;

  list_item first(void) const;
  list_item last(void) const;

  // Update methods
  list_item push(T item);
  list_item append(T item);
  list_item insert(T item);
  list_item insert(T item, 
		   list_item it, 
		   dir direction = after);

  void assign(list_item it, T item);

  T   pop(void);
  T   Pop(void);
  T   del_item(list_item it);
  T   del(const T & item);

  void clear(void);
  
  list_item next(list_item li) const;
  list_item succ(list_item li) const;
  list_item pred(list_item li) const;

  bool contains(list_item li) const;

  void set_compare( int (*_func)(const T &, const T &) );

  void sort(void);

private:

  void recompute_length(void);
  
  // --------- data ---------
  int (*_cmp_ptr)(const T &, const T &);

  list_node * _head;
  list_node * _tail;
  int         _length;
};

#endif // __LIST_H__
