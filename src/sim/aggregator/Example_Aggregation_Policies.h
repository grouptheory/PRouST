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
#ifndef __EXAMPLE_AGGREGATION_POLICIES_H__
#define __EXAMPLE_AGGREGATION_POLICIES_H__
#ifndef LINT
static char const _Example_Aggregation_Policies_h_rcsid_[] =
"$Id: Example_Aggregation_Policies.h,v 1.1 1999/02/16 00:29:28 bilal Exp $";
#endif

//------------------------------------------------------------------------
void AggrNode_Constant(LogosGraph & lg, ComplexRep & cr, 
		   int mcr, 
		   int acr, 
		   int weight, 
		   int ctd, 
		   int cdv, 
		   sh_int clr0, 
		   sh_int clr01);

//------------------------------------------------------------------------
void AggrNode_BestCase_Uniform(LogosGraph & lg, ComplexRep & cr);

//------------------------------------------------------------------------
void AggrNode_BestCase_PerClass(LogosGraph & lg, ComplexRep & cr);

//------------------------------------------------------------------------
void AggrNode_Random_Full(LogosGraph & lg, ComplexRep & cr, 
		      int min_mcr,      int max_mcr, 
		      int min_acr,      int max_acr, 
		      int min_weight,   int max_weight, 
		      int min_ctd,      int max_ctd, 
		      int min_cdv,      int max_cdv, 
		      sh_int min_clr0,  sh_int max_clr0, 
		      sh_int min_clr01, sh_int max_clr01);



//------------------------------------------------------------------------
ig_resrc_avail_info * AggrLink_Constant( const list <const  ig_resrc_avail_info *>& lst,
					 int service_category,
					 int mcr, 
					 int acr, 
					 int weight, 
					 int ctd, 
					 int cdv, 
					 sh_int clr0, 
					 sh_int clr01 );

//------------------------------------------------------------------------
ig_resrc_avail_info * AggrLink_Average( const list <const ig_resrc_avail_info *>& lst,
					int service_category);

//------------------------------------------------------------------------
ig_resrc_avail_info * AggrLink_Best( const list <const ig_resrc_avail_info *>& lst,
				     int service_category);

//------------------------------------------------------------------------
ig_resrc_avail_info * AggrLink_Worst( const list <const ig_resrc_avail_info *>& lst,
				      int service_category);

#endif // __EXAMPLE_AGGREGATION_POLICIES__
