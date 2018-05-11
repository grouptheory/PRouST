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

/* -*- C++ -*-
 * @file ACACPolicyRegistry.h
 * @author marsh
 * @version $Id: ACACPolicyRegistry.h,v 1.5 1998/07/01 18:12:54 mountcas Exp $
 *
 * Purpose: Interface to functions for registering a call admission
 * policy at run-time.  ACAC will consult the policy when it is asked
 * to admit a call.
 */

#ifndef __ACAC_POLICY_REGISTRY_H__
#define __ACAC_POLICY_REGISTRY_H__

#ifndef LINT
static char const _acac_policy_registry_h_rcsid_[] =
"$Id: ACACPolicyRegistry.h,v 1.5 1998/07/01 18:12:54 mountcas Exp $";
#endif

#include <DS/containers/dictionary.h>

class ACACPolicy;

typedef ACACPolicy * (* ACAC_FPTR)(void);

class ACACPolicyRegistry {
  friend void RegisterACACPolicy(const char * name, 
				 ACACPolicy * (*ptr)(void));
  friend ACACPolicy * AllocateACACPolicy(const char * name);
  friend ACACPolicyRegistry * theACACPolicyRegistry(void);

private:

  ACACPolicyRegistry();
  ~ACACPolicyRegistry();

  dictionary<const char *, ACAC_FPTR> _name_to_ptr;
  static ACACPolicyRegistry                * _acac_singleton;
};

#endif
