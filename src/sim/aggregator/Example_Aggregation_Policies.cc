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
static char const _Example_Aggregation_Policies_cc_rcsid_[] =
"$Id: Example_Aggregation_Policies.cc,v 1.2 1999/02/18 22:26:37 marsh Exp $";
#endif

#include "ComplexRep.h"
#include "costMatrix.h"

#include "Example_Aggregation_Policies.h"

#include <common/cprototypes.h>
#include <DS/random/distributions.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <FW/basics/diag.h>
#include <sim/logos/LogosGraph.h>

#define PROHIBITIVELY_HIGH_COST 1000000



// ****************************************************************
//              EXAMPLE NODE AGGREGATION SCHEMES 
// ****************************************************************

//------------------------------------------------------------------------
void AggrNode_Constant(LogosGraph & lg, ComplexRep & cr, 
		   int mcr, 
		   int acr, 
		   int weight, 
		   int ctd, 
		   int cdv, 
		   sh_int clr0, 
		   sh_int clr01) {

  // Add radius
  cr.Set_Link(0, 0, ComplexRep::ALL_SERVICE_CLASSES,
	      weight, mcr, acr, ctd, cdv, clr0, clr01);
}

const char *service_class_name(ComplexRep::service_class sc)
{
  static const char
    cbr[]   = "CBR   ",
    rtvbr[] = "RTVBR ", 
    nrtvbr[]= "NRTVBR",
    abr[]   = "ABR   ",
    ubr[]   = "UBR   ",
    all[]   = "ALL   ", //_SERVICE_CLASSES };
    bad[]   = "Unknown";
  const char *ret = bad;
  
  switch(sc) {
  case ComplexRep::CBR:
    ret = cbr;
    break;
  case ComplexRep::RTVBR:
    ret = rtvbr;
    break;
  case ComplexRep::NRTVBR:
    ret = nrtvbr;
    break;
  case ComplexRep::ABR:
    ret = abr;
    break;
  case ComplexRep::UBR:
    ret = ubr;
    break;
  case ComplexRep::ALL_SERVICE_CLASSES:
    ret = all;
    break;
  }
  return ret;
}

const char *service_class_name(int sc)
{
  return service_class_name((ComplexRep::service_class) sc);
}

//------------------------------------------------------------------------
void AggrNode_BestCase_Uniform(LogosGraph & lg, ComplexRep & cr) {
  costMatrix    costs(lg);

  int mcr=      (int) costs.computeDiameter(costMatrix::mcr);
  int acr=      (int) costs.computeDiameter(costMatrix::acr);
  int ctd=      (int) costs.computeDiameter(costMatrix::ctd);
  int cdv=      (int) costs.computeDiameter(costMatrix::cdv);
  int weight=   (int) costs.computeDiameter(costMatrix::adm);
  sh_int clr0=  (sh_int) costs.computeDiameter(costMatrix::clr0);
  sh_int clr01= (sh_int) costs.computeDiameter(costMatrix::clr01);

  // Add radius
  cr.Set_Link(0, 0, ComplexRep::ALL_SERVICE_CLASSES,
	      weight, mcr, acr, ctd, cdv, clr0, clr01);
  DIAG("complexnode_aggregation_report", DIAG_DEBUG, cout 
       << "Aggr BestCase Uniform: " << *lg._creator
       //<< " " << lg._border_index.size() << " ports"
       << endl
       << "  "<< service_class_name(ComplexRep::ALL_SERVICE_CLASSES)
       << ": wgt="   << weight << ", mcr="   << mcr << ", acr="<< acr
       << ", ctd="  << ctd    << ", cdv="   << cdv
       << ", clr0=" << clr0   << ", clr0+1=" << clr01 << endl;);
}

//------------------------------------------------------------------------
void AggrNode_BestCase_PerClass(LogosGraph & lg, ComplexRep & cr) {
  costMatrix    costs(lg);

  // this DIAG depends on costs being quiet
  DIAG("complexnode_aggregation_report", DIAG_DEBUG, cout
       << "Aggr BestCase PerClass: " << *lg._creator
       // << " " << lg._border_index.size() << " ports"
       << endl;);

  for (int sclass=0;
       sclass < 5;
       sclass++) {
  int mcr=      (int) costs.computeDiameter((costMatrix::cost_service_class)
					    sclass,
					    costMatrix::mcr);
  int acr=      (int) costs.computeDiameter((costMatrix::cost_service_class)
					    sclass,
					    costMatrix::acr);
  int ctd=      (int) costs.computeDiameter((costMatrix::cost_service_class)
					    sclass,
					    costMatrix::ctd);
  int cdv=      (int) costs.computeDiameter((costMatrix::cost_service_class)
					    sclass,
					    costMatrix::cdv);
  int weight=   (int) costs.computeDiameter((costMatrix::cost_service_class)
					    sclass,
					    costMatrix::adm);
  sh_int clr0=  (sh_int) costs.computeDiameter((costMatrix::cost_service_class)
					       sclass,
					       costMatrix::clr0);
  sh_int clr01= (sh_int) costs.computeDiameter((costMatrix::cost_service_class)
					       sclass,
					       costMatrix::clr01);

  cr.Set_Link(0, 0, (ComplexRep::service_class) sclass,
	      weight, mcr, acr, ctd, cdv, clr0, clr01);
  
  DIAG("complexnode_aggregation_report", DIAG_DEBUG, cout 
       << "  " <<service_class_name(sclass)
       << ": wgt="   << weight << ", mcr="   << mcr << ", acr="<< acr
       << ", ctd="  << ctd    << ", cdv="   << cdv
       << ", clr0=" << clr0   << ", clr0+1=" << clr01 << endl;);
  }
}

//------------------------------------------------------------------------
void AggrNode_Random_Full(LogosGraph & lg, ComplexRep & cr, 
		      int min_mcr,      int max_mcr, 
		      int min_acr,      int max_acr, 
		      int min_weight,   int max_weight, 
		      int min_ctd,      int max_ctd, 
		      int min_cdv,      int max_cdv, 
		      sh_int min_clr0,  sh_int max_clr0, 
		      sh_int min_clr01, sh_int max_clr01) {

  AggrNode_BestCase_Uniform(lg,cr);

  list<int> * lportList = cr.Get_Logical_Ports_List();
  if ( lportList != 0 && lportList->empty() == false ) {
    list_item li;
    forall_items(li, *lportList) {
      int p1 = lportList->inf(li);
      // Add exceptions for all ports
      if (p1 > 0) {
	
	cr.Set_Link(p1, 0, ComplexRep::ALL_SERVICE_CLASSES,
		    (int) uniform_distribution::sample((double)min_weight,(double)max_weight),
		    (int) uniform_distribution::sample((double)min_mcr,(double)max_mcr), 
		    (int) uniform_distribution::sample((double)min_acr,(double)max_acr), 
		    (int) uniform_distribution::sample((double)min_ctd,(double)max_ctd), 
		    (int) uniform_distribution::sample((double)min_cdv,(double)max_cdv), 
		    (sh_int) uniform_distribution::sample((double)min_clr0,(double)max_clr0),
		    (sh_int) uniform_distribution::sample((double)min_clr01,(double)max_clr01));

	list_item li2;
	forall_items(li2, *lportList) {
	  int p2 = lportList->inf(li2);
	  // Add all bypasses as well
	  if (p2 > 0 && p2 != p1)
	    cr.Set_Link(p2, p1, ComplexRep::ALL_SERVICE_CLASSES,
		    (int) uniform_distribution::sample((double)min_weight,(double)max_weight),
		    (int) uniform_distribution::sample((double)min_mcr,(double)max_mcr), 
		    (int) uniform_distribution::sample((double)min_acr,(double)max_acr), 
		    (int) uniform_distribution::sample((double)min_ctd,(double)max_ctd), 
		    (int) uniform_distribution::sample((double)min_cdv,(double)max_cdv), 
		    (sh_int) uniform_distribution::sample((double)min_clr0,(double)max_clr0),
		    (sh_int) uniform_distribution::sample((double)min_clr01,(double)max_clr01));
	}
      }
    }
  }
}


// ****************************************************************
//              EXAMPLE LINK AGGREGATION SCHEMES 
// ****************************************************************

// In the link aggregation function below, 
// the 'service_category' argument must be
// one of the following:
//
//               RAIG_FLAG_CBR    
//               RAIG_FLAG_RTVBR  
//               RAIG_FLAG_NRTVBR 
//               RAIG_FLAG_ABR    
//               RAIG_FLAG_UBR    
//

//------------------------------------------------------------------------
ig_resrc_avail_info * AggrLink_Constant( const list <const ig_resrc_avail_info *>& lst,
					 int service_category,
					 int mcr, 
					 int acr, 
					 int weight, 
					 int ctd, 
					 int cdv, 
					 sh_int clr0, 
					 sh_int clr01 ) {

  ig_resrc_avail_info* raig = new ig_resrc_avail_info(ig_resrc_avail_info::outgoing, 
						      service_category,
						      weight, mcr, acr, ctd, cdv, clr0, clr01);
  return raig;
}

//------------------------------------------------------------------------
ig_resrc_avail_info * AggrLink_Average( const list <const ig_resrc_avail_info *>& lst,
					int service_category) {
  assert( ! lst.empty() );
  list_item li;
  int mcr=      0;
  int acr=      0;
  int weight=   0;
  int ctd=      0;
  int cdv=      0;
  int clr0=     0;
  int clr01=    0;

  double count = 0;

  forall_items( li, lst ) {
    const ig_resrc_avail_info * entry = lst.inf(li);
    u_int flg = entry->GetFlags();
    if ( flg & service_category ) 
      { // logical or, 
	// is this raig pertinent to the computed average?
	weight += entry->GetAdminWeight();
	mcr += entry->GetMCR();
	acr += entry->GetACR();
	ctd += entry->GetCTD();
	cdv += entry->GetCDV();
	clr0  += entry->GetCLR0();
	clr01 += entry->GetCLR01();
	count++;
      }
  }

  if (count==0) {
    mcr    = 0;
    acr    = 0;
    weight = PROHIBITIVELY_HIGH_COST;
    ctd    = PROHIBITIVELY_HIGH_COST;
    cdv    = PROHIBITIVELY_HIGH_COST;
    clr0   = 0;
    clr01  = 0;
  }
  else {
    mcr    = (int)( (double)mcr/(double)count );
    acr    = (int)( (double)acr/(double)count );
    weight = (int)( (double)weight/(double)count );
    ctd    = (int)( (double)ctd/(double)count );
    cdv    = (int)( (double)cdv/(double)count );
    clr0   = (int)( (double)clr0/(double)count );
    clr01  = (int)( (double)clr01/(double)count );
  }
  ig_resrc_avail_info* raig = new ig_resrc_avail_info(ig_resrc_avail_info::outgoing, 
						      service_category,
						      weight, mcr, acr, ctd, cdv, (sh_int)clr0, (sh_int)clr01);
  return raig;
}

//------------------------------------------------------------------------
ig_resrc_avail_info * AggrLink_Best( const list <const ig_resrc_avail_info *>& lst,
				     int service_category) {
  assert( ! lst.empty() );
  list_item li;
  int mcr=      0;
  int acr=      0;
  int weight=   0;
  int ctd=      0;
  int cdv=      0;
  int clr0=     0;
  int clr01=    0;

  double count = 0;

  forall_items( li, lst ) {
    const ig_resrc_avail_info * entry = lst.inf(li);
    u_int flg = entry->GetFlags();
    if ( flg & service_category ) 
      { // logical or, 
	// is this raig pertinent to the computed average?
	if (weight > entry->GetAdminWeight()) weight = entry->GetAdminWeight();
	if (ctd    > entry->GetCTD())         ctd = entry->GetCTD();
	if (cdv    > entry->GetCDV())         cdv = entry->GetCDV();
	if (mcr    < entry->GetMCR())         mcr = entry->GetMCR();
	if (acr    < entry->GetACR())         acr = entry->GetACR();
	if (clr0   < entry->GetCLR0())        clr0 = entry->GetCLR0();
	if (clr01  < entry->GetCLR01())       clr01 = entry->GetCLR01();
	count++;
      }
  }

  if (count==0) {
    mcr    = 0;
    acr    = 0;
    weight = PROHIBITIVELY_HIGH_COST;
    ctd    = PROHIBITIVELY_HIGH_COST;
    cdv    = PROHIBITIVELY_HIGH_COST;
    clr0   = 0;
    clr01  = 0;
  }
  ig_resrc_avail_info* raig = new ig_resrc_avail_info(ig_resrc_avail_info::outgoing, 
						      service_category,
						      weight, mcr, acr, ctd, cdv, (sh_int)clr0, (sh_int)clr01);
  return raig;
}

//------------------------------------------------------------------------
ig_resrc_avail_info * AggrLink_Worst( const list <const ig_resrc_avail_info *>& lst,
				      int service_category) {
  assert( ! lst.empty() );
  list_item li;
  int mcr=      0;
  int acr=      0;
  int weight=   0;
  int ctd=      0;
  int cdv=      0;
  int clr0=     0;
  int clr01=    0;

  double count = 0;

  forall_items( li, lst ) {
    const ig_resrc_avail_info * entry = lst.inf(li);
    u_int flg = entry->GetFlags();
    if ( flg & service_category ) 
      { // logical or, 
	// is this raig pertinent to the computed average?
	if (weight < entry->GetAdminWeight()) weight = entry->GetAdminWeight();
	if (ctd    < entry->GetCTD())         ctd = entry->GetCTD();
	if (cdv    < entry->GetCDV())         cdv = entry->GetCDV();
	if (mcr    > entry->GetMCR())         mcr = entry->GetMCR();
	if (acr    > entry->GetACR())         acr = entry->GetACR();
	if (clr0   > entry->GetCLR0())        clr0 = entry->GetCLR0();
	if (clr01  > entry->GetCLR01())       clr01 = entry->GetCLR01();
	count++;
      }
  }

  if (count==0) {
    mcr    = 0;
    acr    = 0;
    weight = PROHIBITIVELY_HIGH_COST;
    ctd    = PROHIBITIVELY_HIGH_COST;
    cdv    = PROHIBITIVELY_HIGH_COST;
    clr0   = 0;
    clr01  = 0;
  }
  ig_resrc_avail_info* raig = new ig_resrc_avail_info(ig_resrc_avail_info::outgoing, 
						      service_category,
						      weight, mcr, acr, ctd, cdv, (sh_int)clr0, (sh_int)clr01);
  return raig;
}


