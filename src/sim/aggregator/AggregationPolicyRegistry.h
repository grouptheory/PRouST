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
#ifndef __AGGREGATION_POLICY_REGISTRY_H__
#define __AGGREGATION_POLICY_REGISTRY_H__

#ifndef LINT
static char const _AggregationPolicyRegistry_h_rcsid_[] =
"$Id: AggregationPolicyRegistry.h,v 1.1 1999/02/16 00:29:28 bilal Exp $";
#endif

#include <DS/containers/dictionary.h>

class AggregationPolicy;

typedef AggregationPolicy * (* AGG_FPTR)(void);

class AggregationPolicyRegistry {
  friend void RegisterAggregationPolicy(const char * name, 
					AggregationPolicy * (*ptr)(void));
  friend AggregationPolicy * AllocateAggregationPolicy(const char * name);
  friend AggregationPolicyRegistry * theAggregationPolicyRegistry(void);
private:

  AggregationPolicyRegistry(void);
  ~AggregationPolicyRegistry();

  AGG_FPTR defined(const char * name) const;
  void     remove(const char * name);
  void     insert(const char * name, AggregationPolicy * (*ptr)(void));

  dictionary<const char *, AGG_FPTR>         _name_to_ptr;
  static AggregationPolicyRegistry   * _agg_singleton;
};

#endif // __AGGREGATION_POLICY_REGISTRY_H__
