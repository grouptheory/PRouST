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
static char const _set_cc_rcsid_[] =
"$Id: set.cc,v 1.27 1999/01/26 18:07:14 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <DS/containers/set.h>

template < class T >
set< T >::set(void) { }

template < class T >
set< T >::~set()  { }

template < class T >
void set< T >::insert(T x)
{
  if ( list< T >::lookup( x ) == 0 )
    list< T >::insert( x );
}

template < class T >
void set< T >::del(T x)
{
  list< T >::del(x);
}

template < class T >
bool set< T >::member(T x) const
{
  list_item li;
  if ( li = list< T >::lookup(x) )
    return true;
  return false;
}

template < class T >
T set< T >::choose(void) const
{
  return list< T >::head();
}

template < class T >
list_item set< T >::first(void) const
{ return list< T >::first(); }

template < class T >
list_item set< T >::next(list_item li) const
{ return list< T >::next(li); }

template < class T >
int set< T >::size(void) const
{ return list< T >::size(); }

template < class T >
T   set< T >::inf(list_item li) const
{ return list< T >::inf(li); }

template < class T >
void set< T >::clear(void)
{ 
  list< T >::clear(); 
}

template < class T >
bool set< T >::empty(void) const
{ 
  return list< T >::empty();
}

template < class T >
T & set< T >::operator [] (int index) const
{
  return list< T >::operator [] (index);
}

template < class T >
set< T > & set< T >::operator &= (const set & rhs)
{
  seq_item si, nsi;
  for (si = first(); si; si = nsi) {
    nsi = next(si);
    if (! rhs.member( inf(si) ))
      del( inf(si) );
  }
  return *this;
}

template < class T >
set< T > & set< T >::operator += (const set & rhs)
{
  seq_item si;
  for (si = rhs.first(); si; si = rhs.next(si))
    insert( rhs.inf(si) );
  return *this;
}

template < class T >
set< T > & set< T >::operator -= (const set & rhs)
{
  seq_item si;
  for (si = rhs.first(); si; si = rhs.next(si))
    if (member( rhs.inf(si) ))
      del( rhs.inf(si) );
  return *this;
}

template < class T > set< T > operator & (set< T > const & lhs, set< T > const & rhs)
{
  set< T > rval;
  list_item li;
  for (li = lhs.first(); li; li = lhs.next(li)) {
    if (rhs.member( lhs.inf(li) ))
      rval.insert( lhs.inf(li) );
  }
  return rval;
}

template < class T > set< T > operator + (set< T > const & lhs, set< T > const & rhs)
{
  set< T > rval(lhs);

  list_item li;
  for (li = rhs.first(); li; li = rhs.next(li))
    rval.insert( rhs.inf(li) );

  return rval;
}

template < class T > set< T > operator - (set< T > const & lhs, set< T > const & rhs)
{
  set< T > rval(lhs);

  list_item li;
  for (li = lhs.first(); li; li = lhs.next(li))
    if (! rhs.member( lhs.inf(li) ))
      rval.insert( lhs.inf(li) );
  return rval;
}
