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
static char const _p_queue_cc_rcsid_[] =
"$Id: p_queue.cc,v 1.8 1998/08/06 04:02:23 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <DS/containers/p_queue.h>

template < class P, class I >
p_queue< P, I >::p_queue(void) { }

template < class P, class I >
p_queue< P, I >::p_queue(const p_queue & rhs) 
  : dictionary< P, I >(rhs) { }

template < class P, class I >
p_queue< P, I >::~p_queue() { }

template < class P, class I >
p_queue < P, I > & p_queue< P, I >::operator = (const p_queue & rhs)
{
  dictionary< P, I >::copy(rhs);
  // *((dictionary< P, I > *)this) = dictionary< P, I >::operator = (rhs);
  return *this;
}

template < class P, class I >
P p_queue< P, I >::prio(pq_item it) const
{ return dictionary<P,I>::key(it); }

template < class P, class I >
I p_queue< P, I >::inf(pq_item it) const
{ return dictionary<P,I>::inf(it); }

template < class P, class I >
pq_item p_queue< P, I >::insert(P prio, I info) 
{ return dictionary<P,I>::insert(prio, info); }

template < class P, class I >
pq_item p_queue< P, I >::find_min(void) const 
{ return dictionary<P,I>::first(); };

template < class P, class I >
P p_queue< P, I >::del_min(void) 
{ 
  pq_item pi = dictionary<P,I>::first();
  P rval = prio(pi);
  dictionary<P,I>::del_item(pi);
  return rval;
}

template < class P, class I >
void p_queue< P, I >::del_item(pq_item it) 
{ dictionary<P,I>::del_item(it); }

template < class P, class I >
void p_queue< P, I >::change_inf(pq_item it, I info) 
{ dictionary<P,I>::change_inf(it, info); }

template < class P, class I >
void p_queue< P, I >::decrease_p(pq_item it, P prio) 
{ 
  // Ack!  see change inf for dictionaries
  I info = dictionary<P,I>::inf(it);
  dictionary<P,I>::del_item(it);
  // reinsert the data so the dictionary is sorted
  dictionary<P,I>::insert(prio, info);
}

template < class P, class I >
int p_queue< P, I >::size(void) const
{ return dictionary<P,I>::size(); };

template < class P, class I >
bool p_queue< P, I >::empty(void) const 
{ return dictionary<P,I>::empty(); }

template < class P, class I >
void p_queue< P, I >::clear(void) 
{ dictionary<P,I>::clear(); }

template < class P, class I >
pq_item p_queue< P, I >::first_item(void) const 
{ return dictionary<P,I>::first(); }

template < class P, class I >
pq_item p_queue< P, I >::next_item(pq_item it) const
{ return dictionary<P,I>::next(it); }

template < class P, class I >
pq_item p_queue< P, I >::next(pq_item it) const
{ return next_item(it); }

template < class P, class I >
pq_item p_queue< P, I >::first(void) const 
{ return first_item(); }
