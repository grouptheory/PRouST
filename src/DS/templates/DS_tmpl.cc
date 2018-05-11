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
static char const _DS_tmpl_cc_rcsid_[] =
"$Id: DS_tmpl.cc,v 1.16 1999/01/26 17:12:29 mountcas Exp $";
#endif

#include <DS/templates/DS_tmpl.h>
#include <DS/containers/list.cc>
#include <DS/containers/rb_tree.cc>
#include <DS/containers/dictionary.cc>
#include <DS/containers/set.cc>

class association;
template class list<association *> ;
template class rb_tree<association *> ;
class node_struct;
class edge_struct;
template class dictionary<char *, node_struct *>;
template class dictionary<node_struct *, bool>;
template class list<node_struct *> ;
template class list<edge_struct *> ;
template class list<int> ;
template class list<double> ;
template class set<int> ;
template class list<set<int> *>;

template class dictionary<int,int>;

// -------------------------------------------------
int compare(association * const & x, association * const & y)
{
  // REMINDER:  If you have arrived here from a dictionary lookup
  //            then your dictionary may not have been properly
  //            constructed (unless your keys are dic/pq_items).
  assert(sizeof(association *) == sizeof(long));
  long lx = (long)x, ly = (long)y;
  // Sorry our debugger complains maddeningly about unrelated ptr compares
  return (lx > ly ? 1 : lx < ly ? -1 : 0);
}

int compare(node_struct * const & x, node_struct * const & y)
{ 
  assert(sizeof(node_struct *) == sizeof(long));
  long lx = (long)x, ly = (long)y;
  // Sorry our debugger complains maddeningly about unrelated ptr compares
  return (lx > ly ? 1 : lx < ly ? -1 : 0);
}

int compare(edge_struct * const & x, edge_struct * const & y)
{ 
  assert(sizeof(edge_struct *) == sizeof(long));
  long lx = (long)x, ly = (long)y;
  // Sorry our debugger complains maddeningly about unrelated ptr compares
  return (lx > ly ? 1 : lx < ly ? -1 : 0);
}

int compare(char * const & x, char * const & y)
{
  assert(x && y);
  return (strcmp(x,y));
}

int compare(int const & x, int const & y)
{ 
  // return (x > y ? 1 : y > x ? -1 : 0); 
  return x - y;
}

int compare(unsigned int const & x, unsigned int const & y)
{ 
  // return (x > y ? 1 : y > x ? -1 : 0); 
  return x - y;
}

int compare(long const & x, long const & y)
{ 
  // return (x > y ? 1 : y > x ? -1 : 0); 
  return x - y;
}

int compare(double const & x, double const & y)
{ 
  return (x > y ? 1 : y > x ? -1 : 0); 
}

int compare(set<int> *const & x, set<int> * const & y) 
{
  return (x > y ? 1 : x < y ? -1 : 0);
}
