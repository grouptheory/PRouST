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
static char const _StatRecord_cc_rcsid_[] =
"$Id: StatRecord.cc,v 1.9 1999/02/03 15:55:05 marsh Exp $";
#endif

#include "StatRecord.h"
#include <codec/pnni_ig/id.h>

StatRecord::StatRecord(const char * type,
		       const char * loc,
		       const char * misc,
		       const NodeID * node,
		       const double time)
  : _type(0), _loc(0), _misc(0), _node(0), _time(time)
{ 
  assert( type != 0 );

  _type = new char [ strlen(type) + 1 ];
  strcpy( _type, type );
  if ( loc != 0 ) {
    _loc  = new char [ strlen(loc) + 1 ];
    strcpy( _loc, loc );
  }
  if (misc && *misc) {
    _misc = new char [ strlen(misc) + 1 ];
    strcpy( _misc, misc );
  }
  if (node)
    _node = new NodeID(*node);
}

StatRecord::~StatRecord() 
{ 
  delete [] _type;
  delete [] _loc;
  delete [] _misc;
  delete    _node;
}

const char * StatRecord::getType(void) const
{
  return _type;
}

const char * StatRecord::getLoc(void) const
{
  return _loc;
}

const char * StatRecord::getMisc(void) const
{
  return _misc;
}

const NodeID * StatRecord::getNode(void) const
{
  return _node;
}

const double StatRecord::getTime(void) const
{
  return _time;
}
