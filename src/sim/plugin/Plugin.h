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
 * @version $Id: Plugin.h,v 1.7 1998/04/09 20:03:50 talmage Exp $
 *
 * Purpose: Implements RegisterPlugins(), the function that associates
 * a plugin library name with its library at run-time.
 */

#if !defined(__POLICY_H__)
#define __POLICY_H__

#ifndef LINT
static char const _Policy_h_rcsid_[] =
"$Id: Plugin.h,v 1.7 1998/04/09 20:03:50 talmage Exp $";
#endif

/** RegisterPlugins() permits a choice of routing and call admission
  policies in switches at run-time.

  @param name is the prefix of a collection of C functions that
  allocate ACACPolicy objects, AggregationPolicy objects, and
  BaseLogos objects at run-time.  The full names of the functions are
  name + name + "ACACPolicyConstruct", name +
  "AggregationPolicyConstruct", and "LogosConstruct" and ,
  respectively.

  @param library_loc the optional name of the library that contains
  the functions.  If omitted, RegisterPlugins() assumes that the
  functions come from "lib" + name + ".so".

 
  The prototypes of the allocator functions for any library Foo are:

	extern "C" ACACPolicy *FooACACPolicyConstruct(void);
  	extern "C" AggregationPolicy *FooAggregationPolicyConstruct(void);
	extern "C" BaseLogos *FooLogosConstruct(Logos *logos);


  Each returns an object derived from the appropriate base class.


  If you need to use allocators from different libraries then you can
  use dlopen() access the libraries and dlsym() to find the functions.
  Suppose, for example, that you need the ACACPolicy allocator from
  libFoo.so, the AggregationPolicy allocator from libBar.so, and the
  BaseLogos allocator from libBaz.so.  Your library, libMixed, could
  contain functions like these:

  static ACACPolicy *(*acac_policy_ptr)() = 0;
  static AggregationPolicy *(*aggregation_policy_ptr)() = 0;
  static BaseLogos *(*logos_ptr)(Logos *) = 0;


  static void _init(void)	// Find some way to make this use C linkage
  {				// If you can do that, then dlopen()
  				// calls _init() for you.
    void * handleACAC = 0;
    void * handleAggregator = 0;
    void * handleLogos = 0;


    if ( (handleACAC = dlopen("libFoo.so", RTLD_LAZY)) ) {
      acac_policy_ptr = 
      (ACACPolicy * (*) ()) dlsym(handleACAC,"FooACACPolicyConstruct");
    }

    if ( (handleAggregator = dlopen("libBar.so", RTLD_LAZY)) ) {
      aggregation_policy_ptr = 
      (AggregationPolicy * (*) ()) dlsym(handleAggregator,
					"BarAggregationPolicyConstruct");
    }

    if ( (handleLogos = dlopen("libBaz.so", RTLD_LAZY)) ) {
      logos_ptr = (BaseLogos * (*) (Logos *)) dlsym(handleLogos, 
							"BazLogosConstruct");
    }

  }

  extern "C" ACACPolicy *MixedACACPolicyConstruct(void)
  {
    return (*acac_policy_ptr)();
  }

  extern "C" AggregationPolicy *MixedAggregationPolicyConstruct(void)
  {
    return (*aggregation_policy_ptr)();
  }

  extern "C" BaseLogos *MixedLogosConstruct(Logos *logos)
  {
    return (*logos_ptr)(logos);
  }

*/

void RegisterPlugins(const char * name, const char * library_loc = 0);

#endif
