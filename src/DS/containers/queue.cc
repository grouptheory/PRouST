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
static char const _queue_cc_rcsid_[] =
"$Id: queue.cc,v 1.7 1999/02/19 22:42:20 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include <DS/containers/queue.h>

template <class T>
ds_queue< T >::ds_queue(void) { }

template <class T>
ds_queue< T >::~ds_queue() { }

template <class T>
T ds_queue< T >::top(void) 
{ return list< T >::head(); }

template <class T>
T ds_queue< T >::pop(void) 
{ return list< T >::pop(); }

template <class T>
void ds_queue< T >::append(T item)
{ list< T >::append(item); }

template <class T>
int ds_queue< T >::size(void)
{ return list< T >::size(); }

template <class T>
bool ds_queue< T >::empty(void) const 
{ return list< T >::empty(); }

template <class T>
void ds_queue< T >::clear(void)
{ list< T >::clear(); }
