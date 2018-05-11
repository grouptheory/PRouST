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
static char const _FW_TMPL_rcsid_[] =
"$Id: FW_tmpl.cc,v 1.2 1999/03/05 17:30:31 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include <iostream.h>

// ----------------------------------
#include <FW/templates/FW_tmpl.h>
#include <DS/containers/dictionary.cc>

#include <FW/kernel/KernelToken.h>
class TimerHandler;
class InputHandler;
class OutputHandler;
template class dictionary<KernelToken, dic_item> ;
template class dictionary<KernelToken, TimerHandler *> ;
template class dictionary<KernelToken, InputHandler *> ;
template class dictionary<KernelToken, OutputHandler *> ;
class SimEvent;
template class dictionary<KernelToken, SimEvent *> ;
template class dictionary<int, InputHandler *> ;
template class dictionary<int, OutputHandler *> ;
class BPT;
template class dictionary<int, BPT *> ;
class TimedDeliveryHandler;
template class dictionary <int, TimedDeliveryHandler *> ;
class Conduit;
template class dictionary <int, Conduit *> ;
class SimEntity;
template class dictionary <int, SimEntity *> ;
class Visitor;
template class dictionary <int, Visitor *> ;
template class dictionary <int, TimerHandler *> ;
class vistype;
template class dictionary<const char *, vistype const *> ;
template class dictionary<const vistype *, const char *> ;
class trienode;
template class dictionary<char const *, trienode *> ;
template class dictionary<const char *, ostream *> ;

// ----------------------------------
#include <DS/containers/p_queue.cc>
#include <FW/kernel/KernelTime.h>
class TimerHandler;
template class p_queue<KernelTime, TimerHandler *> ;

// ----------------------------------
#include <DS/containers/list.cc>

template class list<Conduit *> ;
template class list<char *> ;
template class list<TimedDeliveryHandler *> ;
template class list<vistype *> ;
template class list<Visitor *> ;
template class list<unsigned int> ;
template class list<int> ;
template class list<ostream *> ;
class Expander;
template class list<Expander *> ;

#include <FW/interface/trinode.cc>
template class trinode<Shareable *> ;
template class dictionary<const char *, trinode<Shareable *> *> ;
template class dictionary<Shareable *, trinode<Shareable *> *> ;
#include <FW/interface/Moniker.h>
template class dictionary<int, Moniker *> ;
class fw_Interface;
template class dictionary<Moniker *, fw_Interface *> ;

#include <DS/containers/stack.cc>
template class stack<Expander *> ;

// ---- compares ----
int compare(ostream *const & x, ostream *const & y)
{ return (x > y ? 1 : y > x ? -1 : 0); }

int compare(Moniker * const & lhs, Moniker * const & rhs)
{
  if (lhs && rhs)
    return strcmp(lhs->_string, rhs->_string);
  else
    return (lhs > rhs ? 1 : rhs > lhs ? -1 : 0);
}

int compare(Shareable *const & lhs, Shareable *const & rhs)
  // int compare(const Shareable * & lhs, const Shareable * & rhs)
{ 
  return ((long)lhs > (long)rhs ? 1 : (long)rhs > (long)lhs ? -1 : 0); 
}

int compare(Expander *const & x, Expander *const & y)
{ return (x > y ? 1 : y > x ? -1 : 0); }

int compare(Visitor *const & x, Visitor *const & y)
{
  return ((x<y) ? -1 : ((x>y) ? 1 : 0));
}

int compare(vistype * const & x, vistype * const & y)
{
  return ((x<y) ? -1 : ((x>y) ? 1 : 0));
}

int compare(vistype const *const & x, vistype const *const & y) 
{
  return ((x<y) ? -1 : ((x>y) ? 1 : 0));
}

int compare(Conduit * const & lhs, Conduit * const & rhs)
{
  if ((long)rhs < (long)lhs)
    return -1;
  if ((long)rhs > (long)lhs)
    return 1;
  return 0;
}

int compare(TimedDeliveryHandler *const &x, TimedDeliveryHandler *const &y)
{  
  return (((long)x < (long)y) ? -1 : (((long)x > (long)y) ? 1 : 0));  
}

int compare(const KernelTime & x, const KernelTime & y)
{
  return ((x._time < y._time) ? -1 : ((x._time > y._time) ? 1 : 0));
}

int compare(const KernelToken & lhs, const KernelToken & rhs)
{
  long l = lhs._id, r = rhs._id;

  return ( (l < r) ? -1 : ((l > r) ? 1 : 0) );
}

int compare(char const * const & x, char const * const & y)
{
  assert(x && y);
  return (strcmp(x,y));
}

/*
int compare(char const *const & lhs, char const *const & rhs) {
  assert(lhs && rhs);
  return (strcmp(lhs,rhs));
}
*/

/*
int compare(char * const & lhs, char * const & rhs)
{ 
  assert(lhs && rhs);
  return (strcmp(lhs,rhs));
}
*/
