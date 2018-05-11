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

#ifndef LINT
static char const _PluginTest_cc_rcsid_[] =
"$Id: PluginTest.cc,v 1.6 1998/08/06 04:05:31 bilal Exp $";
#endif
#include <common/cprototypes.h>
/* -*- C++ -*-
 * @file PluginTest.cc
 * @author talmage
 * @version $Id: PluginTest.cc,v 1.6 1998/08/06 04:05:31 bilal Exp $
 *
 * Purpose: Implements BaseLogos and ACACPolicy allocators for testing
 * the plugin mechanism.
 */

#include "PluginTest.h"
#include <iostream.h>
#include <sim/acac/NRLACACPolicy.h>
#include <sim/aggregator/AggregationPolicy.h>
#include <sim/logos/BaseLogos.h>
#include <sim/logos/Logos.h>

ACACPolicy * PluginTestACACPolicyConstruct(void)
{
  cout << "PluginTestACACPolicyConstruct(void)" << endl;
  return new NRLACACPolicy();
}


AggregationPolicy * PluginTestAggregationPolicyConstruct(void)
{
  cout << "PluginTestAggregationPolicyConstruct(void)" << endl;
  return new AggregationPolicy();
}


BaseLogos * PluginTestLogosConstruct(Logos *logos)
{
  cout << "PluginTestLogosConstruct(logos = " 
       << hex << logos << dec << ")" << endl;
  return BaseLogosConstruct(logos);
}
