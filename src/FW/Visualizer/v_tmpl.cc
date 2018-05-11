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
static char const _v_tmpl_cc_rcsid_[] =
"$Id: v_tmpl.cc,v 1.2 1999/03/01 19:27:47 mountcas Exp $";
#endif

#include <qcolor.h>

#include <DS/templates/DS_tmpl.h>
#include "v_conduit.h"

class QPixmap;

int compare(v_conduit * const & lhs, v_conduit * const & rhs)
{
  return ((long)lhs > (long)rhs ? 1 : (long)lhs > (long)rhs ? -1 : 0);
}

int compare(v_visitor * const & lhs, v_visitor * const & rhs)
{
  return ((long)lhs > (long)rhs ? 1 : (long)lhs > (long)rhs ? -1 : 0);
}

int compare(Adjacency * const & lhs, Adjacency * const & rhs)
{
  return ((long)lhs->_neighbor > (long)rhs->_neighbor ?  1 : 
	  (long)lhs->_neighbor < (long)rhs->_neighbor ? -1 : 0);
}

#include <DS/containers/list.cc>
template class list<char *> ;
template class list<v_conduit *> ;
template class list<v_visitor *> ;
template class list<Adjacency *> ;

#include <DS/containers/stack.cc>
template class stack<v_conduit *> ;

#include <DS/containers/dictionary.cc>
template class dictionary<char *, QColor> ;
template class dictionary<char *, v_visitor *> ;
template class dictionary<char *, v_conduit *> ;
template class dictionary<char *, QPixmap *> ;
