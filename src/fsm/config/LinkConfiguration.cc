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
// @file LinkConfiguration.h
// @author talmage
// @version $Id: LinkConfiguration.cc,v 1.3 1999/02/19 21:22:56 marsh Exp $
//
// Purpose: Describes the configuration of a switch.
//

#ifndef LINT
static char const _LinkConfiguration_cc_rcsid_[] =
"$Id: LinkConfiguration.cc,v 1.3 1999/02/19 21:22:56 marsh Exp $";
#endif

#include "LinkConfiguration.h"
#include <DS/util/String.h>
#include <DS/containers/list.h>
#include <codec/pnni_ig/resrc_avail_info.h>

// Owns all pointers
LinkConfiguration::LinkConfiguration(ds_String *name, 
				     int aggregation_token,
				     list<ig_resrc_avail_info *> *raigs) :
  _name(name), _aggregation_token(aggregation_token), _raigs(raigs)
{
}


// Deletes all pointers
LinkConfiguration::~LinkConfiguration(void)
{
  delete _name;

  if (_raigs != 0) {

    while (_raigs->size() > 0) {
      ig_resrc_avail_info *raig = _raigs->pop();
      delete raig;
    }

    delete _raigs;
  }
}

// caller owns the pointer
ds_String *LinkConfiguration::Name(void)
{
  ds_String *answer = 0;

  if (_name != 0) answer = new ds_String(*_name);

  return answer;
}


int LinkConfiguration::AggregationToken(void)
{
  return _aggregation_token;
}


// caller owns the pointer
//
// Makes a deep copy of _raigs.
list<ig_resrc_avail_info *> *LinkConfiguration::RAIGs(void)
{
  list<ig_resrc_avail_info *> *answer = 0;

  if (_raigs != 0) {
    list_item litem;
    answer = new list<ig_resrc_avail_info *>;

    //
    // XXX Danger!  Does forall_items() preserve the order of the list?
    //
    forall_items(litem, *_raigs) {
      ig_resrc_avail_info *raig = _raigs->inf(litem);
      ig_resrc_avail_info *raig_copy = new ig_resrc_avail_info(*raig);
      answer->append(raig_copy);
    }
  }

  return answer;
}

