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
static char const _PluginTest_h_rcsid_[] =
"$Id: PluginTest.h,v 1.4 1998/04/09 21:04:00 talmage Exp $";
#endif
/* -*- C++ -*-
 * @file PluginTest.h
 * @author talmage
 * @version $Id: PluginTest.h,v 1.4 1998/04/09 21:04:00 talmage Exp $
 *
 * Purpose: Interface to BaseLogos and ACACPolicy allocators for testing
 * the plugin mechanism.
 */

#if !defined(__PluginTest_h__)
#define __PluginTest_h__

class ACACPolicy;
class AggregationPolicy;
class BaseLogos;
class Logos;

extern "C" ACACPolicy * PluginTestACACPolicyConstruct(void);

extern "C" AggregationPolicy * PluginTestAggregationPolicyConstruct(void);

extern "C" BaseLogos * PluginTestLogosConstruct(Logos *logos);

#endif
