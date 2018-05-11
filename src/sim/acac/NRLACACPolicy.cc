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
 * File: NRLACACPolicy.cc
 * Author: talmage
 * Version: $Id: NRLACACPolicy.cc,v 1.15 1999/02/25 19:22:44 talmage Exp $
 * Purpose: Implementation of a call admission policy
 * BUGS: none
 */
#ifndef LINT
static char const _NRL_acac_policy_cc_rcsid[] =
"$Id: NRLACACPolicy.cc,v 1.15 1999/02/25 19:22:44 talmage Exp $";
#endif

#include <common/cprototypes.h>
#include "ACAC_DIAG.h"
#include "NRLACACPolicy.h"
#include <FW/basics/diag.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/uni_ie/UNI40_td.h>
#include <codec/uni_ie/UNI40_bbc.h>
#include <codec/q93b_msg/setup.h>

NRLACACPolicy::NRLACACPolicy(void) : ACACPolicy() { }

NRLACACPolicy::~NRLACACPolicy( ) { }

ACACPolicy::CallStates 
NRLACACPolicy::AdmitCall(u_int service_category,
	  const ig_resrc_avail_info * const thePreceedingRAIG,
	  const ig_resrc_avail_info * const theSucceedingRAIG,
	  const q93b_setup_message  * const theSetup,
	  ACACPolicy::Direction &failureDirection)
{
  ACACPolicy::CallStates answer = ACACPolicy::Rejected;
  failureDirection = ACACPolicy::Unknown;

  // we need to extract the proper IE's from the setup message
  assert(theSetup != 0);

  const UNI40_traffic_desc * td = 
    (UNI40_traffic_desc *)theSetup->ie(InfoElem::ie_traffic_desc_ix);
  
  const UNI40_alt_traffic_desc * alt_td = 
    (UNI40_alt_traffic_desc *)theSetup->ie(InfoElem::UNI40_alt_traff_desc_ix);
  
  const UNI40_min_traffic_desc * min_td = 
    (UNI40_min_traffic_desc *)theSetup->ie(InfoElem::UNI40_min_traff_desc_ix);
  
  const ie_bbc * bbc = 
    (ie_bbc *)theSetup->ie(InfoElem::ie_broadband_bearer_cap_ix);
  
  assert(td && bbc);
  
  failureDirection = ACACPolicy::Unknown;

  //
  // Simple test of required forward cell rate vs. maximum cell rate
  // on the succeeding link.
  //
  // AdmitCall() could adjust td or min_td
  //
  if (theSucceedingRAIG != 0) {
    int required_cell_rate = td->get_FPCR_01();
    int maximum_cell_rate = theSucceedingRAIG->GetMCR();
    
    if (required_cell_rate <= maximum_cell_rate) {
      const int available_cell_rate = theSucceedingRAIG->GetACR();
      
      if (required_cell_rate <= available_cell_rate) {
	answer = ACACPolicy::AcceptedSignificantChange;
      } else {
	failureDirection = ACACPolicy::Succeeding;

	diag(SIM_ACAC_CALL_ADMISSION_POLICY, DIAG_DEBUG, 
	     "NRLACACPolicy: user_cell_rate_unavailable\n"
	     "\trequired cell rate (%d) exceeds available cell rate(%d)\n",
	     required_cell_rate, available_cell_rate);
      }
      
    } else {
      failureDirection = ACACPolicy::Succeeding;

      diag(SIM_ACAC_CALL_ADMISSION_POLICY, DIAG_DEBUG, 
	   "NRLACACPolicy:user_cell_rate_unavailable\n"
	   "\trequired cell rate (%d) exceeds maximum cell rate (%d)\n",
	   required_cell_rate, maximum_cell_rate);
      
    }
  }

  if ((thePreceedingRAIG != 0) && (failureDirection == ACACPolicy::Unknown)) {
    //
    // Simple test of required backward cell rate vs. maximum cell
    // rate on the preceeding link.
    // 
    // AdmitCall() could adjust td or min_td
    //
    int required_cell_rate = td->get_BPCR_01();
    int maximum_cell_rate = thePreceedingRAIG->GetMCR();
    
    if (required_cell_rate <= maximum_cell_rate) {
      const int available_cell_rate = thePreceedingRAIG->GetACR();
      
      if (required_cell_rate <= available_cell_rate) {
	answer = ACACPolicy::AcceptedSignificantChange;
      } else {
	failureDirection = ACACPolicy::Preceeding;

	diag(SIM_ACAC_CALL_ADMISSION_POLICY, DIAG_DEBUG, 
	     "NRLACACPolicy:user_cell_rate_unavailable\n"
	     "\trequired cell rate (%d) exceeds available cell rate(%d)\n",
	     required_cell_rate, available_cell_rate);
      }
      
    } else {
      failureDirection = ACACPolicy::Preceeding;

      diag(SIM_ACAC_CALL_ADMISSION_POLICY, DIAG_DEBUG, 
	   "NRLACACPolicy: user_cell_rate_unavailable\n"
	   "\trequired cell rate (%d) exceeds maximum cell rate (%d)\n",
	   required_cell_rate, maximum_cell_rate);
      
    }
  }

  return answer;
}
