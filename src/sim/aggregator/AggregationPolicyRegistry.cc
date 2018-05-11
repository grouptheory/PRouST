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
static char const _AggregationPolicyRegistry_cc_rcsid_[] =
"$Id: AggregationPolicyRegistry.cc,v 1.1 1999/02/16 00:29:28 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include "AggregationPolicyRegistry.h"
#include "AggregationPolicy.h"

void RegisterAggregationPolicy(const char * name, 
			       AggregationPolicy * (*ptr)(void))
{
  // Don't deal with null pointers or empty strings
  if (name && *name) {
    if (theAggregationPolicyRegistry()->defined(name))
      theAggregationPolicyRegistry()->remove(name);

    // now add it to the dictionary
    char * n = new char [strlen(name) + 1];
    strcpy(n, name);
    theAggregationPolicyRegistry()->insert(n, ptr);
  }
}

AggregationPolicy * AllocateAggregationPolicy(const char * name)
{
  AggregationPolicy * (* ptr)(void) = theAggregationPolicyRegistry()->defined(name);
  AggregationPolicy * rval = 0;

  if (ptr)
    rval = (*ptr)();
  else
    rval = new AggregationPolicy();
  return rval;
}

AggregationPolicyRegistry * theAggregationPolicyRegistry(void)
{
  if (!AggregationPolicyRegistry::_agg_singleton)
    AggregationPolicyRegistry::_agg_singleton = new AggregationPolicyRegistry();
  return (AggregationPolicyRegistry::_agg_singleton);
}

AggregationPolicyRegistry * AggregationPolicyRegistry::_agg_singleton = 0;

AggregationPolicyRegistry::AggregationPolicyRegistry(void) { }

AggregationPolicyRegistry::~AggregationPolicyRegistry() 
{ 
  dic_item di;
  forall_items(di, _name_to_ptr) {
    char * n = (char *)_name_to_ptr.key(di);
    delete [] n;
  }
  _name_to_ptr.clear();
}

AGG_FPTR AggregationPolicyRegistry::defined(const char * name) const
{
  AggregationPolicy * (* rval)(void) = 0;

  if ((name != 0) && (*name != 0)) {
    dic_item di;
    forall_items(di, _name_to_ptr) {
      const char * n = _name_to_ptr.key(di);
      if (!strcmp(n, name)) {
	rval = _name_to_ptr.inf(di);
	break;
      }
    }
  }

  return rval;
}

void AggregationPolicyRegistry::remove(const char * name)
{
  if ((name != 0) && (*name != 0)) {
    dic_item di;
    forall_items(di, _name_to_ptr) {
      const char * n = _name_to_ptr.key(di);
      if (!strcmp(n, name)) {
	delete [] n;
	_name_to_ptr.del_item(di);
	break;
      }
    }
  }
}

// name is MINE now so don't touch it
void AggregationPolicyRegistry::insert(const char * name, 
				       AggregationPolicy * (*ptr)(void))
{
  if ((name != 0) && (*name != 0))
    _name_to_ptr.insert((char *)name, ptr);
}
