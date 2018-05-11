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
 * @file ACACPolicyRegistry.cc
 * @author marsh
 * @version $Id: ACACPolicyRegistry.cc,v 1.8 1999/02/12 17:46:38 talmage Exp $
 *
 * Purpose: Implements functions for registering a call admission
 * policy at run-time.  ACAC will consult the policy when it is asked
 * to admit a call.  
 */

#ifndef LINT
static char const _acac_policy_registry_cc_rcsid_[] =
"$Id: ACACPolicyRegistry.cc,v 1.8 1999/02/12 17:46:38 talmage Exp $";
#endif
#include <common/cprototypes.h>

#include "ACACPolicyRegistry.h"
#include "NRLACACPolicy.h"

ACACPolicyRegistry *ACACPolicyRegistry::_acac_singleton = 0;

void RegisterACACPolicy(const char * name, ACACPolicy * (*ptr)(void))
{
  dic_item di;

  if (name != 0) {
    // if it's already in there remove it
    forall_items(di, theACACPolicyRegistry()->_name_to_ptr) {
      const char * str = theACACPolicyRegistry()->_name_to_ptr.key(di);

      if (!strcmp(str, name)) {
	delete [] str;
	theACACPolicyRegistry()->_name_to_ptr.del_item(di);      
	break;
      }
    }

    // now it's definitely not in the dictionary - so add it
    char * n = new char [strlen(name) + 1];
    strcpy(n, name);
    theACACPolicyRegistry()->_name_to_ptr.insert(n, ptr);
  }
}

// name should be in the form of XXXXLogosConstruct OR XXXXX
ACACPolicy * AllocateACACPolicy(const char * name)
{
  ACACPolicy * answer = 0;
  dic_item di;

  if (name != 0) {
    forall_items(di, theACACPolicyRegistry()->_name_to_ptr) {
      const char * str = theACACPolicyRegistry()->_name_to_ptr.key(di);

      if (!strcmp(str, name)) {
	answer = (theACACPolicyRegistry()->_name_to_ptr.inf(di))();
      }
    }
  }

  if (answer == 0) {
    if ((name != 0) && (*name != 0))
      cerr << "AllocateACACPolicy: " << name << "ACACPolicyConstruct"
	   << " not found - using default" << endl;
    answer = new NRLACACPolicy();
  }

  return answer;
}

ACACPolicyRegistry * theACACPolicyRegistry(void)
{
  if (ACACPolicyRegistry::_acac_singleton == 0)
    ACACPolicyRegistry::_acac_singleton = new ACACPolicyRegistry();

  return (ACACPolicyRegistry::_acac_singleton);
}

ACACPolicyRegistry::ACACPolicyRegistry() { }
ACACPolicyRegistry::~ACACPolicyRegistry() { }

