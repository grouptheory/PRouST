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
static char const _rb_tree_cc_rcsid_[] =
"$Id: rb_tree.cc,v 1.22 1999/02/04 14:52:40 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "rb_tree.h"

#ifndef __TEMPLATES__
rb_node * rb_null = 0;
#endif

#define CHECK_RB  assert( rb_null->parent() == rb_null );

//-----------------------------------------------------
template <class T> 
rb_tree< T >::rb_tree(void)
  : _root( rb_null ), _length(0), _cmp_ptr(0)
{
  int (*p)(const T &, const T &) = compare;
  _cmp_ptr = p;

  char * mem = new char [ sizeof(T) ];
  assert( mem != 0 );
  memset( mem, 0, sizeof( T ) );

  if ( ! rb_null) {
    rb_null = new rb_node(mem);
    _root = rb_null;
  }
}

//-----------------------------------------------------
template <class T> 
rb_tree< T >::rb_tree(const rb_tree & rhs)
  : _root( rb_null), _length(0), _cmp_ptr( rhs._cmp_ptr )
{
  *this = rhs;

  char * mem = new char [ sizeof(T) ];
  assert( mem != 0 );
  memset( mem, 0, sizeof( T ) );

  if ( ! rb_null) 
    rb_null = new rb_node(mem);
}

//-----------------------------------------------------
template <class T> 
rb_tree< T >::~rb_tree()
{
  clear();
  delete rb_null; rb_null=0;
}

//-----------------------------------------------------
template <class T> 
rb_tree< T > & rb_tree< T >::operator = (const rb_tree & rhs)
{
  CHECK_RB 
  clear();

  // copy everything over
  rb_item ri;
  for (ri = rhs._first(); ri !=  rb_null; ri = rhs._next(ri)) {
    T item = rhs.inf(ri);
    insert(item);
  }
  
  return *this;
}

//-----------------------------------------------------
template <class T> 
int rb_tree< T >::length(void) const
{
  CHECK_RB 
  return _length;
}

//-----------------------------------------------------
template <class T> 
bool rb_tree< T >::empty(void) const
{
  CHECK_RB 
  return (_length == 0);
}

//-----------------------------------------------------
template <class T> 
T & rb_tree< T >::inf(rb_item it) const
{
  CHECK_RB 
  assert(it != rb_null);
  return *((T *)it->data()); 
}

//-----------------------------------------------------
template <class T> 
const T & rb_tree< T >::operator [] (rb_item it) const
{
  CHECK_RB 
  return inf(it);
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::lookup(const T item) const
{
  CHECK_RB 
  rb_item rval = _lookup( item );

  if ( rval == rb_null )
    return 0;
  return rval;
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::_lookup(const T item) const
{
  CHECK_RB 
  rb_item rval = rb_null;

  // This is only worth doing if we have a tree
  if (_root) {
    // start at the root and work our way to the requested item
    rb_item iter = _root;
    int i = 0;
    // While we have not found what we're looking for,
    //     move to the next item
    while ( iter != rb_null && (i = (*_cmp_ptr)(item, inf(iter))) != 0 ) {
      if (i < 0) // If item < inf(iter)
        iter = iter->left();
      else // if item > inf(iter)
        iter = iter->right();
    }
    // It may not reside within the tree, so check again
    if ( iter != rb_null && (*_cmp_ptr)(item, inf(iter)) == 0 )
      rval = iter;
  }
  return rval;
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::search(const T item) const
{
  CHECK_RB 
  return lookup(item);
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::first(void) const
{
  CHECK_RB 
  rb_item rval = _first();
  if ( rval == rb_null )
    return 0;
  return rval;
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::_first(void) const
{
  CHECK_RB 
  return tree_min( _root );
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::last(void) const
{
  CHECK_RB 
  rb_item rval = _last();
  if ( rval == rb_null )
    return 0;
  return rval;
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::_last(void) const
{
  CHECK_RB 
  return tree_max( _root );
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::next(rb_item it) const
{
  CHECK_RB 
  rb_item rval = _next( it );
  if ( rval == rb_null )
    return 0;
  return rval;
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::_next(rb_item it) const
{
  CHECK_RB 
  rb_item parent = it->parent();
  rb_item rval;

  if (it->right() != rb_null) {
    rval = tree_min( it->right() );
  }
  else if (parent != rb_null) {
    while ((parent != rb_null) &&
	   (parent->left() != it)) {
      it = parent;
      parent = it->parent();
    }
    rval = parent;
  }
  else 
    rval = rb_null;

  return rval;
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::succ(rb_item it) const
{
  CHECK_RB 
  return next(it);
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::prev(rb_item it) const
{
  CHECK_RB 
  rb_item rval = _prev( it );
  if ( rval == rb_null )
    return 0;
  return rval;
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::_prev(rb_item it) const
{
  CHECK_RB 
  rb_item rval = it->parent();

  if (it->left() != rb_null)
    rval = tree_min( it->left() );
  else {
    while (rval != rb_null && it == rval->left()) {
      it = rval;
      rval = rval->parent();
    }
  }
  return rval;
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::pred(rb_item it) const
{
  CHECK_RB 
  return prev(it);
}

 
//-----------------------------------------------------
template <class T> 
void    rb_tree< T >::left_rotate(rb_item x)
{
  CHECK_RB 
  if ( x == rb_null )
    return;

  rb_item y = x->right();
  x->right( y->left() );

  if ( y->left() != rb_null ) {
    assert( y->left() != x );
    y->left()->parent( x );
  }
  
  assert( x->parent() != y );
  y->parent( x->parent() );

  if ( x->parent() == rb_null )
    _root = y;
  else if ( x == x->parent()->left() )
    x->parent()->left( y );
  else
    x->parent()->right( y );

  y->left( x );
  assert( y != x );
  x->parent( y );
  y->size( x->size() );
  x->size( x->left()->size() + x->right()->size() + 1 );
  CHECK_RB 
}

//-----------------------------------------------------
template <class T> 
void    rb_tree< T >::right_rotate(rb_item y)
{
  CHECK_RB 
  if ( y == rb_null )
    return;

  rb_item x = y->left();
  y->left( x->right() );

  if ( x->right() != rb_null ) {
    assert( x->right() != y );
    x->right()->parent( y );
  }

  assert( y->parent() != x );
  x->parent( y->parent() );

  if ( y->parent() == rb_null )
    _root = x;
  else if ( y == y->parent()->right() )
    y->parent()->right( x );
  else
    y->parent()->left( x );

  x->right( y );
  assert( x != y );
  y->parent( x );
  x->size( y->size() );
  y->size( y->left()->size() + y->right()->size() + 1 );
  CHECK_RB 
}

//-----------------------------------------------------
template <class T> 
rb_item rb_tree< T >::insert(T item)
{
  CHECK_RB 
  char * mem = new char [ sizeof(T) ];
  assert( mem != 0 );
  memcpy(mem, &item, sizeof(T));

  rb_item rval = new rb_node(mem);  // default color is red

  tree_insert( rval );

  rb_item x = rval, y = rb_null;
  assert( x->red() );

  while ( x != _root && x->parent()->red() ) {

    if ( x->parent() == x->parent()->parent()->left() ) {
      y = x->parent()->parent()->right();

      if ( y && y->red() ) {
	x->parent()->red( false );
	y->red( false );
	x->parent()->parent()->red( true );
	x = x->parent()->parent();
      }
      else {
	if ( x == x->parent()->right() ) {
	  x = x->parent();
	  left_rotate( x );
	}

	x->parent()->red( false );
	x->parent()->parent()->red( true );
	right_rotate( x->parent()->parent() );
      }
    }
    else
      {
	y = x->parent()->parent()->left();

	if ( y && y->red() ) {
	  x->parent()->red( false );
	  y->red( false );
	  x->parent()->parent()->red( true );
	  x = x->parent()->parent();
	}
	else {
	  if ( x == x->parent()->left() ) {
	    x = x->parent();
	    right_rotate( x );
	  }

	  x->parent()->red( false );
	  x->parent()->parent()->red( true );
	  left_rotate( x->parent()->parent() );
	}
      }
  };

  _root->red( false );

  CHECK_RB 
  return rval;
}

//-----------------------------------------------------
template <class T> 
T rb_tree< T >::del_item(rb_item z)
{
  CHECK_RB 

  rb_item up = z->parent();
  while ( up != rb_null ) {
    up->size( up->size()-1 );
    assert( up != up->parent() );
    up = up->parent();
  }

  T rval = inf(z);
  rb_item x = rb_null, y = rb_null;

  if (z->left() == rb_null || z->right() == rb_null)
    y = z;
  else
    y = _next(z);

  if (y->left() != rb_null)
    x = y->left();
  else
    x = y->right();

  if ( x != rb_null ) {
    assert( y->parent() != x );
    x->parent( y->parent() );
  }

  if ( y->parent() == rb_null )
    _root = x;
  else {
    if ( y == y->parent()->left() )
      y->parent()->left( x );
    else
      y->parent()->right( x );
  }

  if ( y != z )
    *z = *y;

  if ( y->red() == false )
    delete_fixup( x );
    
  _length--;

  CHECK_RB 
  return rval;
}

template <class T> 
T rb_tree< T >::del(const T & item)
{
  CHECK_RB 
  rb_item ri = _lookup( item );
  assert( ri != 0 );
  return del_item( ri );
}

//-----------------------------------------------------
template <class T> 
void rb_tree< T >::clear(void)
{
  CHECK_RB 
  rb_item ri, nri;
  for (ri = _first(); ri; ri = nri) {
    nri = _next(ri);

    if (_length == 0)
      break;

    if (ri == rb_null)
      continue;

    del_item(ri);
  }
  CHECK_RB 
}

//-----------------------------------------------------
template <class T> 
void    rb_tree< T >::delete_fixup(rb_item x)
{
  CHECK_RB 
  rb_item w = rb_null;

  while ( x != _root && x->red() == false ) {

    if ( x == x->parent()->left() ) {
      w = x->parent()->right();
      
      if ( w->red() ) {
	w->red( false );
	x->parent()->red( true );
	left_rotate( x->parent() );
	w = x->parent()->right();
      }
      if ( w->left()->red() == false &&
	   w->right()->red() == false ) {
	w->red( true );
	x = x->parent();
      } else {
	if ( w->right()->red() == false ) {
	  w->left()->red( false );
	  w->red( true );
	  right_rotate( w );
	  w = x->parent()->right();
	}

	w->red( x->parent()->red() );
	x->parent()->red( false );
	w->right()->red( false );
	left_rotate( x->parent() );
	x = _root;
      }
    }
    else {
      w = x->parent()->left();
      
      if ( w->red() ) {
	w->red( false );
	x->parent()->red( true );
	right_rotate( x->parent() );
	w = x->parent()->left();
      }
      if ( w->right()->red() == false &&
	   w->left()->red() == false ) {
	w->red( true );
	x = x->parent();
      } else {
	if ( w->left()->red() == false ) {
	  w->right()->red( false );
	  w->red( true );
	  left_rotate( w );
	  w = x->parent()->left();
	}

	w->red( x->parent()->red() );
	x->parent()->red( false );
	w->left()->red( false );
	right_rotate( x->parent() );
	x = _root;
      }
    }
  };

  x->red( false );
  CHECK_RB 
}

//-----------------------------------------------------
template <class T> 
void rb_tree< T >::tree_insert(rb_item it)
{
  CHECK_RB 
  rb_item y = rb_null, x = _root;

  while ( x != rb_null ) {
    y = x;
    if ( (*_cmp_ptr)(inf(it), inf(y)) < 0 )
      x = x->left();
    else
      x = x->right();
  }
  assert( y != it );
  it->parent( y );

  if ( y == rb_null )
    _root = it;
  else if ( (*_cmp_ptr)(inf(it), inf(y)) < 0 )
    y->left( it );
  else
    y->right( it );

  rb_item up = it->parent();
  while ( up != rb_null ) {
    up->size( up->size() + 1 );
    assert( up != up->parent() );
    up = up->parent();
  };

  _length++;
  CHECK_RB 
}

//-----------------------------------------------------
// Locates the min of the tree rooted at it
template <class T> 
rb_item rb_tree< T >::tree_min(rb_item it) const
{
  CHECK_RB 
  while ( it->left() != rb_null )
    it = it->left();

  CHECK_RB 
  return it;
}

//-----------------------------------------------------
// Locates the max of the tree rooted at it
template <class T> 
rb_item rb_tree< T >::tree_max(rb_item it) const
{
  CHECK_RB 
  while ( it->right() != rb_null )
    it = it->right();

  CHECK_RB 
  return it;
}

template < class T >
void rb_tree< T >::set_compare( int (*func)(const T &, const T &) ) 
{
  CHECK_RB 
  if (!func) {
    int (*p)(const T &, const T &) = compare;
    _cmp_ptr = p;
  } else
    _cmp_ptr = func;
  CHECK_RB 
}

template < class T >
int rb_tree< T >::rank( const T & item ) const
{
  CHECK_RB 
  rb_item ri = _lookup( item );
  return rank( ri );
}

template < class T >
int rb_tree< T >::rank( rb_item ri ) const
{
  CHECK_RB 
  int rank = ri->size() - ri->right()->size() + 1;

  while ( ri != _root ) {
    rb_item new_ri = ri->parent();

    if ( new_ri->left() == ri ) {
      // emerge from left + 0
    } else if ( new_ri->right() == ri ) {
      // emerge from right, left subtree size + 1
      rank += new_ri->left()->size() + 1;
    }
    ri = new_ri;
  }
  CHECK_RB 
  return rank;
}


