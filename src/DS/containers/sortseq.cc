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
static char const _sortseq_cc_rcsid_[] =
"$Id: sortseq.cc,v 1.7 1998/08/06 04:02:25 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <DS/containers/sortseq.h>

template < class K, class I > 
sortseq< K, I >::sortseq(void) 
  : dictionary< K, I >() { }

template < class K, class I > 
sortseq< K, I >::sortseq(const sortseq & rhs) 
  : dictionary< K, I >(rhs) { }

template < class K, class I > 
sortseq< K, I >::~sortseq() { }

template < class K, class I > 
sortseq< K, I > & sortseq< K, I >::operator = (const sortseq & rhs) 
{
  *((dictionary< K, I > *)this) = dictionary< K, I >::operator = (rhs);
  return *this;
}

template < class K, class I >
seq_item sortseq< K, I >::min(void) const
{
  return first();
}

template < class K, class I >
seq_item sortseq< K, I >::max(void) const
{
  return last();
}
