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
 * File: NRLACACPolicy.h
 * Author: talmage
 * Version: $Id: NRLACACPolicy.h,v 1.7 1999/01/15 21:54:33 talmage Exp $
 * Purpose: Interface to a call admission
 * BUGS: none
 */
#ifndef LINT
static char const _NRLACACPolicy_h_rcsid_[] =
"$Id: NRLACACPolicy.h,v 1.7 1999/01/15 21:54:33 talmage Exp $";
#endif

#ifndef _NRL_ACAC_POLICY_H_
#define _NRL_ACAC_POLICY_H_

#include "ACACPolicy.h"

class NRLACACPolicy : public ACACPolicy {
public:

  NRLACACPolicy(void);
  virtual ~NRLACACPolicy(void);

  virtual ACACPolicy::CallStates 
  AdmitCall(u_int service_category,
	    const ig_resrc_avail_info * const thePreceedingRAIG,
	    const ig_resrc_avail_info * const theSucceedingRAIG,
	    const q93b_setup_message  * const theSetup,
	    ACACPolicy::Direction &failureDirection);

};

#endif
