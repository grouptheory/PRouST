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
static char const _Event_cc_rcsid_[] =
"$Id: Event.cc,v 1.4 1999/02/01 20:24:39 mountcas Exp $";
#endif

#include "Event.h"
#include <codec/pnni_ig/id.h>

//----------------------------------------
// Methods of class 'Event'
// ---------------------------------------
Event::Event( const double time,
	      const char * type,
	      const char * loc,
	      const char * misc,
	      const NodeID * n )
  : _time(time), _type(0), _loc(0), _misc(0), _addr(n)
{
  _type = new char [ strlen(type) + 1 ];
  strcpy(_type, type);
  
  if (loc) {
    _loc  = new char [ strlen(loc) + 1 ];
    strcpy(_loc, loc);
  }
  if (misc && strcmp(misc, "(null)")) {
    _misc = new char [ strlen(misc) + 1 ];
    strcpy(_misc, misc);
  }
}

//----------------------------------------
Event::~Event( )
{
  delete [] _type;
  delete [] _loc;
  delete [] _misc;
  delete    _addr;
}

//----------------------------------------
const double   Event::getTime(void) const
{
  return _time;
}

//----------------------------------------
const char   * Event::getType(void) const
{
  return _type;
}

//----------------------------------------
const char   * Event::getLoc(void) const
{
  return _loc;
}

//----------------------------------------
const char   * Event::getMisc(void) const
{
  if (_misc != 0)
    return _misc;
  return "";
}

//----------------------------------------
const NodeID * Event::getNode(void) const
{
  return _addr;
}


