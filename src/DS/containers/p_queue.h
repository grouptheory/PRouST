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
#ifndef __P_QUEUE_H__
#define __P_QUEUE_H__

#ifndef LINT
static char const _p_queue_h_rcsid_[] =
"$Id: p_queue.h,v 1.5 1999/01/26 18:00:38 mountcas Exp $";
#endif

#include <DS/containers/dictionary.h>

typedef dic_item pq_item;

template <class P, class I> 
class p_queue : protected dictionary < P, I > {
public:

  p_queue(void);
  p_queue(const p_queue & rhs);
  virtual ~p_queue();

  p_queue & operator = (const p_queue & rhs);

  P prio(pq_item it) const;
  I inf(pq_item it) const;

  pq_item insert(P prio, I info);
  pq_item find_min(void) const;

  P del_min(void);

  void del_item(pq_item it);
  void change_inf(pq_item it, I info);
  void decrease_p(pq_item it, P prio);

  int  size(void) const;
  bool empty(void) const;
  void clear(void);

  pq_item first_item(void) const;
  pq_item next_item(pq_item it) const;
  pq_item next(pq_item it) const;
  pq_item first(void) const;
};

#endif
