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
#ifndef __LIST_CC__
#define __LIST_CC__

#ifndef LINT
static char const _list_cc_rcsid_[] =
"$Id: list.cc,v 1.56 1999/01/26 22:44:58 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <DS/containers/list.h>

extern int maxListSize;

#ifdef OPTIMIZE
#define CHECK_LIST
#else
#define CHECK_LIST  assert( (_head == _tail) || \
			    (_head != 0 && _tail != 0) )
#endif

template < class T >
list< T >::list(void) 
  : _head(0), _tail(0), _length(0), _cmp_ptr(0)
{ 
  int (*p)(const T &, const T &) = compare;
  _cmp_ptr = p;

  CHECK_LIST;
}

template < class T >
list< T >::list(const list & rhs) 
  : _head(0), _tail(0), _length(0), 
    _cmp_ptr(rhs._cmp_ptr)
{
  list_node * node = rhs._head;
  while (node) {
    append( rhs.contents(node) );
    node = node->_next;
  }
  recompute_length();

  CHECK_LIST;
}

template < class T >
list< T >::~list() 
{ clear(); }

template < class T >
list< T > & list< T >::operator = (const list & rhs) 
{
  // Clear this list
  clear();
  // perform a node by node copy
  list_item li;
  for (li = rhs.first(); li != 0; li = rhs.next(li)) {
    insert( rhs.contents(li) );
  }
  recompute_length();

  CHECK_LIST;
  return ((list< T > &)*this);
}

template < class T >
int list< T >::length(void) const
{
  CHECK_LIST;
  if (_length <= 0)
    ((list< T > *)this)->recompute_length();
  CHECK_LIST;
  return _length;
}

template < class T >
int list< T >::size(void) const
{ 
  return ((list< T > *)this)->length(); 
}

template < class T >
bool list< T >::empty(void) const 
{ 
  CHECK_LIST;
  return (_head == 0); 
}

template < class T >
T list< T >::contents(list_item it) const 
{
  CHECK_LIST;

  assert( it->_owner == this );
  return *((T *)it->_data); 
}

template < class T >
T & list< T >::inf(list_item it) const 
{ 
  CHECK_LIST;

  assert( it->_owner == this );
  return *((T *)it->_data); 
}

template < class T >
const T & list< T >::operator [] (list_item it) const
{
  CHECK_LIST;

  assert( it->_owner == this );
  return *((T *)it->_data); 
}

template < class T >
T & list< T >::operator [] (int index) const
{
  assert(index >= 0 && index < size());
  CHECK_LIST;
  list_item li;
  int i = 0;
  for (li = first(); li; li = next(li), i++) {
    if (index == i)
      break;
  }
  return inf(li);
}

template < class T >
list< T > & list< T >::operator &= (const list & rhs)
{
  CHECK_LIST;
  list_item li, nli;
  for (li = first(); li; li = nli) {
    nli = next(li);
    T val = contents(li);
    if (! rhs.lookup(val))
      del_item(li);
  }
  return *this;
}

template < class T >
list< T > & list< T >::operator += (const list & rhs)
{
  CHECK_LIST;
  list_item li;
  for (li = rhs.first(); li; li = rhs.next(li))
    append( rhs.inf(li) );

  return *this;
}

template < class T >
list_item list< T >::search(const T item) const
{
  CHECK_LIST;
  list_item li, rli = 0;

  // Don't bother wasting time if the list is empty ...
  if (!empty()) {
    forall_items(li, ((list< T > &)*this)) {
      if (! (*_cmp_ptr)(item, contents(li))) {
	rli = li;
	break;
      }
    }
  }
  return rli;
}
  
template < class T >
list_item list< T >::lookup(const T item) const
{ 
  CHECK_LIST;
  return search(item); 
}

template < class T >
int list< T >::rank(const T & item) const
{ 
  int rval = 0;
  list_item li, targ = lookup(item);
  for (li = first(); li; li = next(li), rval++) {
    if (li == targ)
      break;
  }
  CHECK_LIST;
  return rval;
}

template < class T >
int list< T >::rank(const list_item it) const
{   
  assert( it->_owner == this );

  int rval = 0;
  list_item li;
  for (li = first(); li; li = next(li), rval++) {
    if (li == it)
      break;
  }
  CHECK_LIST;
  return rval;
}

template < class T >
T list< T >::head(void) const 
{ 
  CHECK_LIST;
  return contents(_head); 
}

template < class T >
T list< T >::tail(void) const 
{ 
  CHECK_LIST;
  return contents(_tail); 
}

template < class T >
list_item list< T >::first(void) const 
{ 
  CHECK_LIST;
  return _head; 
}

template < class T >
list_item list< T >::last(void) const  
{ 
  CHECK_LIST;
  return _tail; 
}

template < class T >
list_item list< T >::push(T item) 
{ 
  CHECK_LIST;
  return insert(item, first(), before); 
}

template < class T >
list_item list< T >::append(T item) 
{ 
  CHECK_LIST;
  return insert(item, last(), after); 
}

template < class T >
list_item list< T >::insert(T item) 
{ 
  CHECK_LIST;
  if (!_cmp_ptr)
    set_compare(0);
  
  bool inserted = false;
  char * mem = new char [ sizeof(T) ];
  assert( mem != 0 );
  memcpy(mem, &item, sizeof(T));

  list_item li, newli = new list_node((void*)this, mem);
  assert( newli != 0 );
  forall_items(li, ((list< T > &)*this)) {
    if ((*_cmp_ptr)(contents(li), item) > 0) {
      // Insert item before this li
      list_item before = li->_prev;
      newli->_prev = before,
	newli->_next = li;
      li->_prev = newli;
      if (before)
	before->_next = newli;
      else
	_head = newli;
      inserted = true;
      break;
    }
  }
  if (!inserted) {
    // append it to the end ...
    if (!_tail) // If there is no tail, then there is no head
      _head = _tail = newli;
    else {
      _tail->_next = newli;
      newli->_prev = _tail;
      _tail = newli;
    }
  }
  _length++;

  if ((maxListSize > 0) && (_length > maxListSize)) {
  //  if (getenv("DSBLOAT") && (atoi(getenv("DSBLOAT")) < _length)) {
    cerr << "******* FATAL ERROR:  List " << this 
	 << " has grown beyond the maximum allowed size (" 
	 << maxListSize << ")." << endl;
    abort();
  }

  CHECK_LIST;
  return newli;
}

template < class T >
list_item list< T >::insert(T item, 
			    list_item it, 
			    dir direction) 
{
  CHECK_LIST;

  if ( it != 0 ) {
    assert( it->_owner == this );
  }

  list_item next = (it ? it->_next : 0);
  list_item prev = (it ? it->_prev : 0);
  char * mem = new char [ sizeof(T) ];
  assert( mem != 0);
  memcpy(mem, &item, sizeof(T));
  list_item i = new list_node((void*)this, mem);
  assert( i != 0);
  
  if (direction == before) {
    if (prev) {
      prev->_next = i;
      i->_prev = prev;
    } else 
      _head = i;
    
    if (it) {
      i->_next = it;
      it->_prev = i;
    } else
      _tail = i;
  } else if (direction == after) {
    if (it) {
      it->_next = i;
      i->_prev = it;
    } else
      _head = i;
    
    if (next) {
      i->_next = next;
      next->_prev = i;
    } else
      _tail = i;
  }
  _length++;

  if ((maxListSize > 0) && (_length > maxListSize)) {
  //  if (getenv("DSBLOAT") && (atoi(getenv("DSBLOAT")) < _length)) {
    cerr << "******* FATAL ERROR:  List " << this 
	 << " has grown beyond the maximum allowed size (" 
	 << maxListSize << ")." << endl;
    abort();
  }

  CHECK_LIST;
  return i;
}

template < class T >
void list< T >::assign(list_item it, T item) 
{
  CHECK_LIST;

  assert( it->_owner == this );

  char * mem = (char *)it->_data;
  if (sizeof(T) > sizeof(void *))
    (*(T *)mem).T::~T();
  delete [] mem;
  mem = new char [ sizeof(T) ];
  assert( mem != 0);
  memcpy(mem, &item, sizeof(T));
  it->_data = (void *)mem;

  CHECK_LIST;
}

template < class T >
T list< T >::pop(void) 
{ 
  CHECK_LIST;
  return del_item(first()); 
}

template < class T >
T list< T >::Pop(void) 
{ 
  CHECK_LIST;
  return del_item(last()); 
}

template < class T >
T list< T >::del_item(list_item it) 
{ 
  CHECK_LIST;

  assert( it->_owner == this );

  T rval = contents(it);

  list_item prev = it->_prev;
  list_item next = it->_next;
  // close the gap
  if (prev) prev->_next = next;
  if (next) next->_prev = prev;
  if (it == _head) _head = next;
  if (it == _tail) _tail = prev;
  
  char * mem = (char *)it->_data;
  if (sizeof(T) > sizeof(void *))
    (*(T *)mem).T::~T();
  delete [] mem;
  delete it;
  _length--;

  CHECK_LIST;
  return rval;
}

template < class T >
T list< T >::del(const T & item) 
{
  CHECK_LIST;
  // return del_item( search(item) );
  list_item li;
  for (li = first(); li; li = next(li)) {
    T info = inf(li);
    if (info == item)
      return del_item( li );
  }
  abort();
}

template < class T >
void list< T >::clear(void) 
{
  CHECK_LIST;
  list_item li = first();
  while (li != 0) {
    list_item bye = li;
    li = next(li);
    del_item(bye);
  }
  CHECK_LIST;
  _length = 0;
}
  
template < class T >
list_item list< T >::next(list_item li) const 
{ 
  CHECK_LIST;
  assert( li->_owner == this );

  return li->_next; 
}

template < class T >
list_item list< T >::succ(list_item li) const 
{ 
  CHECK_LIST;
  return next(li);  
}

template < class T >
list_item list< T >::pred(list_item li) const 
{ 
  CHECK_LIST;
  assert( li->_owner == this );

  return li->_prev; 
}

template < class T >
bool list< T >::contains(list_item it) const
{
  bool rval = false;
  
  if ( it != 0 ) {
    list_item li;
    for ( li = first(); li; li = li->_next ) {
      if (li == it) {
	rval = true;
	break;
      }
    }
  }
  return rval;
}

template < class T >
void list< T >::set_compare( int (*func)(const T &, const T &) ) 
{
  CHECK_LIST;
  if (!func) {
    int (*p)(const T &, const T &) = compare;
    _cmp_ptr = p;
  } else
    _cmp_ptr = func;
}

template < class T >
void list< T >::sort(void) 
{
  CHECK_LIST;
  list<T> tmp(*this);
  // clear us
  clear();
  
  list_item li;
  forall_items(li, tmp) {
    T item = tmp.inf(li);
    insert( item );
  }
  // Now we should be sorted
  CHECK_LIST;
}

template < class T >
void list< T >::recompute_length(void) 
{
  CHECK_LIST;
  _length = 0;
  list_item li;
  forall_items(li, ((list< T > &)*this)) {
    _length++;
  }
  CHECK_LIST;
}

#endif // __LIST_CC__
