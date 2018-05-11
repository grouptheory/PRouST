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

#ifndef LINT
static char const _ov_tmpl_cc_rcsid_[] =
"$Id: ov_tmpl.cc,v 1.6 1999/02/01 20:26:12 mountcas Exp $";
#endif
#include "ov_data.h"
#include "OVNode.h"
#include "Event.h"

//----------------------------------------
// formal template instantiations 
// and associated friend funtions
// ---------------------------------------

int compare(OVNode * const & lhs, OVNode * const & rhs)
{
  return (strcmp( lhs->getName(), rhs->getName() ));
}

#include <DS/containers/list.cc>
template class list< Event * > ;
template class list< OVNode  * > ;

#include <DS/containers/dictionary.cc>
template class dictionary< double, list< Event * > * > ;

#include <DS/templates/DS_tmpl.h>
template class dictionary< char *, float * >;

class NodeID;
template class dictionary< char *, NodeID * >;
template class dictionary< char *, Event * > ;
template class dictionary< int , OVNode  * >;

class levelSubgraph;
template class dictionary<int, levelSubgraph *>;

template class dictionary<int, int>;

//------------------------------------------------------
#include "connComp.h"

int compare(connComp * const & c1, connComp * const & c2) 
{
  int rval = (((long)c1 < (long)c2) ? 1 : 
	      (((long)c1 > (long)c2) ? -1 : 0));
  return rval;
}
template class list<connComp *>;

//------------------------------------------------------
template class list< double > ;

int compare(list_node * const & c1, class list_node * const & c2) 
{
  int rval = (((long)c1 < (long)c2) ? 1 : 
	      (((long)c1 > (long)c2) ? -1 : 0));
  return rval;
}
template class list< list_item > ;

//------------------------------------------------------
#include <codec/pnni_ig/id.h>

int compare(Event * const & lhs, Event * const & rhs)
{
  int rval = 0;
  double tl = lhs->getTime();
  double tr = rhs->getTime();
  
  // no, you may not swap the plus and minus one!
  rval = ((tr < tl) ? 1 : ((tr > tl) ? -1 : 0));

  if (!rval) {
    if (!(rval = strcmp( lhs->getLoc(), rhs->getLoc() )))
      if (!(rval = strcmp( lhs->getType(), rhs->getType() )))
	if (!(rval = strcmp( lhs->getMisc(), rhs->getMisc() )))
	  if ( lhs->getNode() != 0 && rhs->getNode() != 0 )
	    rval = compare( lhs->getNode(), rhs->getNode() );
  }

  return rval;
}
