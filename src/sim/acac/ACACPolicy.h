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
 * File: ACACPolicy.h
 * Author: talmage
 * Version: $Id: ACACPolicy.h,v 1.7 1999/02/25 19:23:18 talmage Exp $
 * Purpose: Interface to the base class of ACAC call admission policy
 * BUGS: none

This is my first draft of the ACACPolicy plugin.

To tell PRouST about it, you'll use a function like RegisterPlugins():

    RegisterACACPolicy("NRL");  // Get the policy from libNRL.so

It implies that libNRL.so contains a function 

    ACACPolicy *NRLACACPolicyConstruct(void).  

That function returns a pointer to an instance of a class that is
derived from ACACPolicy.

The ACAC constructor will take the same string as
RegisterACACPolicy().

    ACAC *the_actual_call_admission_control = new ACAC("NRL", <other args>);

The constructor calls a function

    ACACPolicy *AllocateACACPolicy("NRL")

which calls the ACACPolicy allocator that it finds in the ACACPolicy
registry, NRLACACPolicyConstruct() in this case, and returns the value
of that function (i.e. NRLACACPolicyConstruct()) or a pointer to a
BaseACACPolicy object.  The BaseACACPolicy class accepts all calls,
regardless of traffic descriptor and RAIG.

This ACACPolicy object is given to each port_info object that ACAC()
creates.  Each port_info applies the same policy rules.  None may
delete it the shared ACACPolicy object.

When ACAC is asked to admit a call on some port, it checks with the
port_info object for that port.  The port_info object, in turn, asks
the shared ACACPolicy object through one of the three AdmitCall()
methods.

*/
#ifndef _ACAC_POLICY_H_
#define _ACAC_POLICY_H_

#ifndef LINT
static char const _acac_policy_h_rcsid_[] =
"$Id: ACACPolicy.h,v 1.7 1999/02/25 19:23:18 talmage Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/Conduit.h>

class ig_resrc_avail_info;
class q93b_setup_message;
#if 0
class UNI40_traffic_desc;
class UNI40_alt_traffic_desc;
class UNI40_min_traffic_desc;
#endif

class ACACPolicy {
public:
  //
  // AdmitCall() returns one of these values.
  //
  enum CallStates {
    Rejected = 0,		// Don't admit the call
    Accepted,			// Admit the call
    AcceptedSignificantChange	/* Admit the call and reoriginate the
				 * HLINK or UPLINK for the port.
				 */
  };

  //
  // AdmitCall() sets its failureDirection parameter to one
  // of these values.
  //
  enum Direction {
    Unknown = 0,		// Don't know which direction
    Preceeding = 1,		// Refers to the node before us
    Succeeding = 2		// Refers to the node after us
  };

  ACACPolicy(void);

  virtual ~ACACPolicy(void);

  /** AdmitCall() implements a call admission policy.  It expects that
  * all of its parameters are valid.  See section 6.5.2.3.4 "Procedures
  * for negotiation of traffic parameters during call/connection setup"
  * pages 214-215 of the PNNI 1.0 spec.
  *
  * AdmitCall() is responsible for pulling out the apropriate fields
  * from the setup.  It is passed a copy of the setup so no harm may
  * be done.  AdmitCall() is permitted to adjust the traffic
  * descriptor and/or the minimum traffic descriptor.
  *
  * AdmitCall() need not handle the case of a setup that contains both
  * the alternative traffic descriptor and the minimum traffic
  * descriptor.  ACAC handles that case before calling AdmitCall().
  *
  * @returns Accepted if it's OK to admit the call.
  *
  * @returns AcceptedSignficantChange if it's OK to admit the call and
  * the change in resources is significant enough to warrant the
  * reorigination of the corresponding HLINK or UPLINK IG.  
  *
  * @returns Rejected if it's not OK to admit the call.  Rejected is
  * an indication to set the cause and crankback cause to #37, "User
  * cell rate unavailable"
  *
  * @param service_category A bitset that identifies the type of
  * traffic in the call.  The members are defined in
  * <codec/pnni_ig/resrc_avail_info.h>.  They are #defines beginning
  * with "RAIG_FLAG".
  *
  * @param thePreceedingRAIG The resource availability information
  * group for the link to the preceeding node.  It is either the node
  * that sent the setup to this node or zero (when this node is the
  * originator of the call).  AdmitCall() tests the backward resources
  * required by the setup against the resources represented by this
  * RAIG.  thePreceedingRAIG may be zero.
  *
  * @param theSucceedingRAIG The resource availability information
  * group for the link to the succeeding node.  It may not be zero.
  * AdmitCall() tests the forward resources required by the setup
  * against the resources presented by this RAIG.  theSuceedingRAIG
  * may be zero.
  *
  * @param theSetup A copy of the setup from the calling party,
  * either this node (theSucceedingRAIG == 0) or the node whose link
  * is represented by the non-zero theSucceedingRAIG.
  *
  * @param failureDirection If an AdmitCall() rejects a setup
  * attempt, it must set failureDirection to indicate which leg of
  * the call caused the failure.  */
  virtual ACACPolicy::CallStates 
  AdmitCall(u_int service_category,
	    const ig_resrc_avail_info * const thePreceedingRAIG,
	    const ig_resrc_avail_info * const theSucceedingRAIG,
	    const q93b_setup_message  * const theSetup,
	    ACACPolicy::Direction &failureDirection) = 0;
#if 0
 /*
  * traffic descriptor but no alternative traffic descriptor and no
  * minimum traffic descriptor
  */
  virtual CallStates AdmitCall(u_int service_category, 
			       const ig_resrc_avail_info * const theRAIG,
			       // TDs contained in the SETUP
			       const UNI40_traffic_desc * const td) = 0;

  /*
  * traffic descriptor and minimum traffic descriptor
  *
  * If we can support td then progress the call with both td and
  * min_td, setting new_td and new_min_td zero.
  *
  * If we cannot support td but we can support min_td, create new_td,
  * adjusting the rates from td to something we can support.  After
  * the adjustment, if any rates in min_td are less than those in
  * new_td, create new_min_td using only those rates.  Otherwise, let
  * new_min_td be zero.  The call will be progressed with new_td (and
  * new_min_td if it is not zero).
  */
  virtual CallStates AdmitCall(u_int service_category, 
			       const ig_resrc_avail_info * const theRAIG,
			       // TDs contained in the SETUP
			       const UNI40_traffic_desc * const td, 
			       const UNI40_min_traffic_desc * const min_td,
			       UNI40_traffic_desc * *new_td,
			       UNI40_min_traffic_desc * *new_min_td) = 0;

  /*
  * If we can support td and alt_td then progress the call with both,
  * setting new_td and new_alt_td to zero.
  *
  * If we can support td but not alt_td then create new_td as a copy
  * of td.  The call will be progressed with new_td in place of td
  * and no alt_td.  new_td and td must be equal.  No fair changing td
  * on the sly!  There is no way to check.
  *
  * If we can support alt_td but not td, then create new_td from the
  * values of alt_td.  The call will be progressed with new_td in the
  * place of td and no alt_td.
  */
  virtual CallStates AdmitCall(u_int service_category, 
			       const ig_resrc_avail_info * const theRAIG,
			       // TDs contained in the SETUP
			       const UNI40_traffic_desc * const td, 
			       const UNI40_alt_traffic_desc * const alt_td,
			       UNI40_traffic_desc * *new_td) = 0;
#endif // #if 0
};

#endif * _ACAC_POLICY_H_
