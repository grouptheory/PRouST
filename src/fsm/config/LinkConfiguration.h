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
// @version $Id: LinkConfiguration.h,v 1.3 1999/02/19 21:22:58 marsh Exp $
//
// Purpose: Describes the configuration of a switch.
//

#ifndef __LINKCONFIGURATION_H__
#define __LINKCONFIGURATION_H__

#ifndef LINT
static char const _LinkConfiguration_h_rcsid_[] =
"$Id: LinkConfiguration.h,v 1.3 1999/02/19 21:22:58 marsh Exp $";
#endif

#include <DS/containers/list.h>

class ig_resrc_avail_info;
class ds_String;

class LinkConfiguration {
public:

  // Owns all pointers
  LinkConfiguration(ds_String *name, int aggregation_token, 
		    list<ig_resrc_avail_info *> *raigs);

  // Deletes all pointers
  ~LinkConfiguration(void);

  // caller owns the pointer
  // Returns a copy of the name of the link.
  ds_String *Name(void);

  int AggregationToken(void);

  // caller owns the pointer
  // Returns a deep copy of the list of RAIGs.
  list<ig_resrc_avail_info *> *RAIGs(void);

private:
  ds_String *_name;
  int _aggregation_token;
  list<ig_resrc_avail_info *> *_raigs;
};

#endif
