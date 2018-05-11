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
#ifndef __RB_TREE_H__
#define __RB_TREE_H__

#ifndef LINT
static char const _rb_tree_h_rcsid_[] =
"$Id: rb_tree.h,v 1.29 1999/01/26 22:22:50 mountcas Exp $";
#endif

#include <assert.h>

class rb_node;
extern rb_node * rb_null;

class rb_node {
public:

  rb_node(void) :
    _data(0), _parent( rb_null ), _left( rb_null ), _right( rb_null ), 
    _leftmost( rb_null ), _rightmost( rb_null ), _red(false), _size(1) { 
      if (rb_null==0) {
	_parent = _left = _right = _leftmost = _rightmost = this;
      }
  }

  rb_node(void * data, bool red = true,
	  rb_node * parent = rb_null,
	  rb_node * left   = rb_null,
	  rb_node * right  = rb_null) :
    _data(data), _left( left ), _right( right ), 
    _leftmost( rb_null ), _rightmost( rb_null ), 
    _red(red), _size(1), _parent( parent ) { 
      if (rb_null == 0) {
	_parent = _left = _right = _leftmost = _rightmost = this;
	_size = 0;
      } else {
	assert( _parent != this );
      }
  }

  ~rb_node( ) { }

  rb_node * parent(void)     { return _parent; }
  rb_node * left(void)       { return _left;   }
  rb_node * right(void)      { return _right;  }
  rb_node * leftmost(void)   { return _leftmost;   }
  rb_node * rightmost(void)  { return _rightmost;  }
  bool      red(void) const  { return _red;    }
  void    * data(void) const { return _data;   }
  int       size(void)       { return _size;   }

  void parent(rb_node * n)      { assert( n != this ); _parent = n; }
  void left(rb_node * n)        { _left = n;   }
  void right(rb_node * n)       { _right = n;  }
  void red(bool r)              { _red = r;    }
  void size(int s)              { _size = s;   }

  rb_node & operator = (const rb_node & rhs) 
  { 
    _data   = rhs._data;
    //    _left   = rhs._left;
    //    _right  = rhs._right;
    //    _parent = rhs._parent;
    //    _red    = rhs._red;
  }

private:

  void    * _data;
  rb_node * _left;
  rb_node * _right;
  rb_node * _parent;
  rb_node * _leftmost;
  rb_node * _rightmost;
  int       _size;
  bool      _red;
};

typedef rb_node * rb_item;

#ifndef forall_items
#define forall_items(x, y) \
  for ( (x) = (y).first(); (x); (x) = (y).next(x) )
#endif

template <class T> class rb_tree {
public:

  rb_tree(void);
  rb_tree(const rb_tree & rhs);

  virtual ~rb_tree();

  rb_tree & operator = (const rb_tree & rhs);

  int  length(void) const;
  bool empty(void) const;

  T & inf(rb_item it) const;

  const T & operator [] (rb_item it) const;

  rb_item lookup(const T item) const;
  rb_item search(const T item) const;

  rb_item first(void) const;
  rb_item last(void) const;
  rb_item next(rb_item it) const;
  rb_item succ(rb_item it) const;
  rb_item prev(rb_item it) const;
  rb_item pred(rb_item it) const;

  rb_item insert(T item);
  T       del_item(rb_item ri);
  T       del(const T & item);

  void clear(void);

  void set_compare( int (*_func)(const T &, const T &) );
  
  int  rank( const T &  ) const;
  int  rank( rb_item ri ) const;

private:

  rb_item _first(void) const;
  rb_item _last(void) const;
  rb_item _next(rb_item it) const;
  rb_item _prev(rb_item it) const;

  rb_item _lookup(const T item) const;

  void    left_rotate(rb_item it);
  void    right_rotate(rb_item it);
  void    delete_fixup(rb_item it);
  void    tree_insert(rb_item it);
  // Locates the min of the tree rooted at it
  rb_item tree_min(rb_item it) const;
  // Locates the max of the tree rooted at it
  rb_item tree_max(rb_item it) const;

  // --------- data ---------
  int (*_cmp_ptr)(const T &, const T &);

  rb_item _root;
  int     _length;
};

#endif
