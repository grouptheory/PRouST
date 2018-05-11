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
#ifndef __DICTIONARY_CC__
#define __DICTIONARY_CC__

#ifndef LINT
static char const _dictionary_cc_rcsid_[] =
"$Id: dictionary.cc,v 1.11 1999/01/28 15:38:33 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <DS/containers/dictionary.h>

#ifndef __USE_RB_TREE__
#define ASSOC_ITEM list_item
#else
#define ASSOC_ITEM rb_item
#endif

template <class K, class I> 
int dictionary< K, I >::compare(const association * & x, const association * & y) 
{
  return ::compare( *((K *)x->_key), *((K *)y->_key) );
}

template <class K, class I> 
dictionary< K, I >::dictionary(void) : _list() 
{

#ifdef __CCC_GENERIC__
  int (*p)(class association *const &, class association *const &) = 
    dictionary<K,I>::compare;
#endif

#ifdef __CCC_EGCS_29__

#ifndef __IRIX64__
  int (*p)(const association *&, const association *&) = 
    dictionary<K,I>::compare;
#else
  int (* p )(association *const &, association *const &) =
    (int (*)(association *const &, association *const &))dictionary<K,I>::compare;
#endif

#endif

#ifdef __CCC_GCC_272__
  int (*p)(class association *const &, class association *const &) = 
    (int (*)(class association *const &, class association *const &))dictionary<K,I>::compare;
#endif

  _list.set_compare( p );
}

template <class K, class I> 
dictionary< K, I >::dictionary(const dictionary & rhs)
{
  copy(rhs);
}

template < class K, class I >
void dictionary< K, I >::copy(const dictionary & rhs)
{
  // clean us out ...
  clear();

  // reset the compare method to be sure
#ifdef __CCC_GENERIC__
  int (*p)(class association *const &, class association *const &) = 
    dictionary<K,I>::compare;
#endif
  // -----------------------------------------------------------
#ifdef __CCC_EGCS_29__

#ifndef __IRIX64__
  int (*p)(const association *&, const association *&) = 
    dictionary<K,I>::compare;
#else
  int (* p )(association *const &, association *const &) =
    (int (*)(association *const &, association *const &))dictionary<K,I>::compare;
#endif

#endif
  // -----------------------------------------------------------
#ifdef __CCC_GCC_272__
  int (*p)(class association *const &, class association *const &) = 
    (int (*)(class association *const &, class association *const &))dictionary<K,I>::compare;
#endif

  _list.set_compare( p );

  // fill us up ...
  ASSOC_ITEM li;
  forall_items(li, rhs._list) {
    dic_item di = rhs._list.inf(li);
    K k = rhs.key(di);
    I i = rhs.inf(di);
    insert(k, i);
  }
}

template <class K, class I> 
dictionary< K, I >::~dictionary() { }

template <class K, class I> 
dictionary< K, I > & dictionary< K, I >::operator = (const dictionary & rhs) 
{
  copy(rhs);
  return *this;
}

template <class K, class I> 
K dictionary< K, I >::key(dic_item it) const 
{
  if (it) {
    assert( it->_owner == (void*)this );
    return *((K *)it->_key);
  }
  // Don't go passing NULL dic_items to me!
  abort();
}

template <class K, class I> 
I & dictionary< K, I >::inf(dic_item it) const 
{
  if (it) {
    assert( it->_owner == (void*)this );
    return *((I *)it->_info);
  }
  // Don't go passing NULL dic_items to me!
  abort();
}

template <class K, class I> 
I dictionary< K, I >::access(K & key) const
{
  return inf(lookup(key));
}

template <class K, class I> 
dic_item dictionary< K, I >::insert(K keyi, I info) 
{
  char * kmem = new char [ sizeof(K) ];
  char * imem = new char [ sizeof(I) ];
  assert(kmem && imem);
  memcpy(kmem, &keyi, sizeof(K));
  memcpy(imem, &info, sizeof(I));
  dic_item di = new association((void*)this, kmem, imem, sizeof(K), sizeof(I));
  ASSOC_ITEM li = _list.insert(di);
  
  assert( info == inf( _list.inf(li) ) );
  assert( keyi == key( _list.inf(li) ) );
  return di;
}

template <class K, class I> 
dic_item dictionary< K, I >::lookup(K keyi) const
{
  dic_item di = 0;
  char * kmem = new char [ sizeof(K) ];
  memcpy(kmem, &keyi, sizeof(K));
  association as((void*)this, (void *)kmem, 0, sizeof(K), sizeof(I));
  // compare(assoc, assoc) calls compare(key, key)
  ASSOC_ITEM li;
  if (li = _list.search(&as))
    di = _list.inf(li);
  
  if (sizeof(K) > sizeof(void *))
    (*(K *)kmem).K::~K();
  delete [] kmem;
  
  return di;
}

template <class K, class I> 
void dictionary< K, I >::del(K & key) 
{ del_item( lookup(key) ); }

template <class K, class I> 
void dictionary< K, I >::del_item(dic_item it) 
{
  assert( it->_owner == (void*)this );
  
  char * kmem = (char *)it->_key;
  char * imem = (char *)it->_info;
  // Allow the underlying list to remove this item
  _list.del(it);
  
  if (sizeof(K) > sizeof(void *))
    (*(K *)kmem).K::~K();
  delete [] kmem;
  
  if (sizeof(I) > sizeof(void *))
    (*(I *)imem).I::~I();
  delete [] imem;
  
  // Delete the association
  delete it;
}

template <class K, class I> 
void dictionary< K, I >::change_inf(dic_item it, I info) 
{
  assert( it->_owner == (void*)this );

  char * imem = (char *)it->_info;
  if (sizeof(I) > sizeof(void *))
    (*(I *)imem).I::~I();
  delete [] imem;
  // now allocate the new space
  imem = new char [ sizeof(I) ];
  memcpy(imem, &info, sizeof(I));
  it->_info = imem;
}
  
template <class K, class I> 
void dictionary< K, I >::clear(void) 
{
  while ( _list.empty() == false ) {
    dic_item di = _list.inf( _list.first() );
    // This cleans up the memory for the associations
    del_item( di );
  }
  // just to be safe
  _list.clear();
}

template <class K, class I> 
int dictionary< K, I >::size(void) const
{ return _list.length(); };

template <class K, class I>
int dictionary< K, I >::rank(const K & keyi) const
{ 
  int rval = -1;
  char * kmem = new char [ sizeof(K) ];
  memcpy(kmem, &keyi, sizeof(K));
  association as((void*)this, (void *)kmem, 0, sizeof(K), sizeof(I));
  // compare(assoc, assoc) calls compare(key, key)
  rval = _list.rank(&as);
  delete [] kmem;
  return rval;
}

template <class K, class I> 
bool dictionary< K, I >::empty(void) const 
{ return _list.empty(); }

template <class K, class I> 
dic_item dictionary< K, I >::first(void) const 
{
  ASSOC_ITEM li;
  if (li = _list.first())
    return _list.inf(li);
  return 0;
}

template <class K, class I> 
dic_item dictionary< K, I >::last(void) const 
{
  ASSOC_ITEM li;
  if (li = _list.last())
    return _list.inf(li);
  return 0;
}

template <class K, class I> 
dic_item dictionary< K, I >::next(dic_item it) const
{
  assert( it->_owner == (void*)this );
  dic_item rval = 0;
  ASSOC_ITEM li;
#ifndef __USE_RB_TREE__
  for (li = _list.first(); li; li = _list.next(li)) {
    dic_item tmp = _list.inf( li );
    if ((tmp == it) && _list.next( li )) {
      rval = _list.inf( _list.next( li ));
      break;
    }
  }
#else
  li = _list.lookup( it );
  assert( li != 0 );
  ASSOC_ITEM nli = _list.next( li );
  if ( nli )
    rval = _list.inf ( nli );
#endif
  return rval;
}

template <class K, class I> 
dic_item dictionary< K, I >::prev(dic_item it) const
{
  assert( it->_owner == (void*)this );

  dic_item rval = 0;
  ASSOC_ITEM li;

#ifndef __USE_RB_TREE__
  for ( li = _list.last(); li; li = _list.pred( li ) ) {
    dic_item tmp = _list.inf(li);
    if ((tmp == it) && _list.pred( li )) {
      rval = _list.inf( _list.pred( li ) );
      break;
    }
  }
#else
  li = _list.lookup( it );
  assert( li != 0 );
  ASSOC_ITEM nli = _list.prev( li );
  if ( nli )
    rval = _list.inf ( nli );
#endif
  return rval;
}

template <class K, class I> 
dic_item dictionary< K, I >::succ(dic_item it) const
{
  return next(it);
}

template <class K, class I> 
dic_item dictionary< K, I >::pred(dic_item it) const
{
  return prev(it);
}

template <class K, class I>
bool dictionary< K, I >::contains(dic_item it) const
{
  bool rval = false;

  if ( it != 0 ) {
    dic_item di;
    for (di = first(); di; di = next(di)) {
      if (di == it) {
	rval = true;
	break;
      }
    }
  }
  return rval;
}
  
template <class K, class I> 
I & dictionary< K, I >::operator [] (K & key) const
{
  dic_item di = lookup(key);
  if (di)
    return *((I *)di->_info);
  abort();
}

template <class K, class I> 
dictionary< K, I > & dictionary< K, I >::operator &= (const dictionary & rhs)
{
  dic_item di, ndi;
  for (di = first(); di; di = ndi) {
    ndi = next(di);
    K keyi = key(di);
    if (! rhs.lookup(keyi))
      del_item(di);
  }

  return *this;
}

template <class K, class I> 
dictionary< K, I > & dictionary< K, I >::operator += (const dictionary & rhs)
{
  dic_item di;
  for (di = rhs.first(); di; di = rhs.next(di))
    insert( rhs.key(di), rhs.inf(di) );
  
  return *this;
}

#endif // __DICTIONARY_CC__
