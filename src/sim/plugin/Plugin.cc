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
 * @file Plugin.cc
 * @author marsh
 * @version $Id: Plugin.cc,v 1.7 1998/11/05 16:20:04 mountcas Exp $
 *
 * Purpose: Implements RegisterPlugins(), the function that associates
 * a plugin library name with its library at run-time.
 */

#ifndef LINT
static char const _Plugin_cc_rcsid_[] =
"$Id: Plugin.cc,v 1.7 1998/11/05 16:20:04 mountcas Exp $";
#endif

#include "Plugin.h"
#include <common/cprototypes.h>
#include <sim/aggregator/AggregationPolicyRegistry.h>
#include <sim/logos/DynamicLogos.h>
#include <sim/acac/ACACPolicyRegistry.h>

//#if defined(__SunOS5__) || defined(__IRIX64__)
extern "C" {
#include  <dlfcn.h>
};
//#endif

/* Associate name with a library.  If library_loc is not NULL, then
 * associate name with it.  Otherwise, assume that name comes from
 * "lib" + name + ".so".  
 *
 * name is the prefix of a collection of C functions that allocate
 * ACACPolicy objects, AggregationPolicy objects, and BaseLogos
 * objects at run-time.  The full names of the functions are name + name +
 * "ACACPolicyConstruct", name + "AggregationPolicyConstruct", and
 * "LogosConstruct" and , respectively.
 *
 */
void RegisterPlugins(const char * name, const char * library_loc)
{
  char library[4096];

  if (library_loc)
    strncpy(library, (char *)library_loc, 4096);
  else
    sprintf(library, "lib%s.so", name);

  void * handle = 0;
  if ( (handle = dlopen(library, RTLD_LAZY)) ) {
    char str[80];

    ACACPolicy *(*acac_policy_ptr)() = 0;
    sprintf(str, "%sACACPolicyConstruct", name);
    if ( (acac_policy_ptr = (ACACPolicy * (*) () ) 
	  dlsym(handle, str)) ) {
      RegisterACACPolicy(str, acac_policy_ptr);
      RegisterACACPolicy(name, acac_policy_ptr);
    }

    AggregationPolicy *(*aggregation_policy_ptr)() = 0;
    sprintf(str, "%sAggregationPolicyConstruct", name);
    if ( (aggregation_policy_ptr = (AggregationPolicy * (*) () ) 
	  dlsym(handle, str)) ) {
      RegisterAggregationPolicy(str, aggregation_policy_ptr);
      RegisterAggregationPolicy(name, aggregation_policy_ptr);
    }

    BaseLogos *(*logos_ptr)(Logos *) = 0;
    sprintf(str, "%sLogosConstruct", name);
    if ( (logos_ptr = (BaseLogos * (*) (Logos *) ) dlsym(handle, str)) ) {
      RegisterLogos(str, logos_ptr);
      RegisterLogos(name, logos_ptr);
    }
  }
}
