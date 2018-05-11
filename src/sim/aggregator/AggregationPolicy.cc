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
#ifndef LINT
static char const _AggregationPolicy_cc_rcsid_[] =
"$Id: AggregationPolicy.cc,v 1.1 1999/02/16 00:29:28 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include "AggregationPolicy.h"
#include "Aggregator.h"
#include "ComplexRep.h"
#include "costMatrix.h"
#include "Example_Aggregation_Policies.h"

#include <FW/basics/diag.h>

#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/uplinks.h>

#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_pkt/ptsp.h>

#include <fsm/visitors/NPFloodVisitor.h>
#include <fsm/visitors/LinkVisitor.h>

extern const u_long OC12;

//---------------------------------------------------
AggregationPolicy::AggregationPolicy(void) { }

AggregationPolicy::~AggregationPolicy() { }

void AggregationPolicy::SetIdentity(const NodeID & myNode, const PeerID & myPeer) 
{
  _myNode = new NodeID(myNode); 
  _myPeerGroup = new PeerID(myPeer);
}

// flush the Hlink
void AggregationPolicy::FlushHLink(int id, int seqnum)
{  InjectPTSE((ig_horizontal_links*)0,id,seqnum);  }
 
// Default Uplink bundle aggregation scheme
list <ig_resrc_avail_info *> * 
AggregationPolicy::
AggregateUpLinks(const dictionary<Anchor*, ig_uplinks*> * links,
		 const ig_horizontal_links * currently_advertised) 
{
  assert(links);
  list <ig_resrc_avail_info*> tmp[6];
  
  links->size();

  dic_item di;
  list_item lli;
  forall_items(di, *links) {
    Anchor* anch = links->key(di);
    ig_uplinks * iter = links->inf(di);
    assert(iter);
    const list<InfoGroup *> * igs = iter->ShareIGs();
    igs->size();

    if (igs->empty()) continue;

    forall_items(lli, *igs) {
      InfoGroup * ig_base = igs->inf(lli);
      if (ig_base->GetId() == InfoGroup::ig_outgoing_resource_avail_id ||
	  ig_base->GetId() == InfoGroup::ig_incoming_resource_avail_id) {
	ig_resrc_avail_info * loop = (ig_resrc_avail_info *)ig_base;
	u_int x = loop->GetFlags();
	if (x & RAIG_FLAG_CBR)	  tmp[0].insert(loop);
	if (x & RAIG_FLAG_RTVBR)  tmp[1].insert(loop);
	if (x & RAIG_FLAG_NRTVBR) tmp[2].insert(loop);
	if (x & RAIG_FLAG_ABR)	  tmp[3].insert(loop);
	if (x & RAIG_FLAG_UBR)	  tmp[4].insert(loop);
      }
    }
  }
  
  list <ig_resrc_avail_info *> * sol = new list<ig_resrc_avail_info *>;
  bool nonempty = false;
  for (int i = 0; i < 5; i++) {
    ig_resrc_avail_info * ans;

    const list<ig_resrc_avail_info*> * adv_raigs = 0;
    if (currently_advertised)
      adv_raigs = & ( currently_advertised->GetRAIGS() );
    ig_resrc_avail_info* current = 0;
    if (adv_raigs) {
      list_item eli;
      forall_items(eli,*adv_raigs) {
	ig_resrc_avail_info* raig = adv_raigs->inf(eli);
	u_int x = raig->GetFlags();
	if (x & RAIG_FLAG_CBR)	  { current = raig; break; }
	if (x & RAIG_FLAG_RTVBR)  { current = raig; break; }
	if (x & RAIG_FLAG_NRTVBR) { current = raig; break; }
	if (x & RAIG_FLAG_ABR)	  { current = raig; break; }
	if (x & RAIG_FLAG_UBR)	  { current = raig; break; }
      }
    }

    switch (i) {
    case 0: 
      ans = Compute_CBR((const list<const ig_resrc_avail_info*>&)tmp[i],
			current); 
      nonempty = true; 
      break;

    case 1: 
      ans = Compute_RTVBR((const list<const ig_resrc_avail_info*>&)tmp[i],
			  current); 
      nonempty = true; 
      break;

    case 2: 
      ans = Compute_NRTVBR((const list<const ig_resrc_avail_info*>&)tmp[i],
			   current); 
      nonempty = true; 
      break;

    case 3: 
      ans = Compute_ABR((const list<const ig_resrc_avail_info*>&)tmp[i],
			current); 
      nonempty = true; 
      break;

    case 4: 
      ans = Compute_UBR((const list<const ig_resrc_avail_info*>&)tmp[i],
			current); 
      nonempty = true; 
      break;

    default: 
      break;
    }
    if ((nonempty) && (ans)) {
      sol->append(ans);
      ans = 0;
    }
  }
  return sol;
}

// builds and resends the fresh HLink PTSE to the DB
void AggregationPolicy::InjectPTSE(ig_horizontal_links * hlink, int id, int seqnum)
{
  assert(hlink);

  ig_ptse * ptse = new ig_ptse(InfoGroup::ig_horizontal_links_id,
			       id, seqnum, 0, _life);
  assert(ptse);

  NodeID * tmp = (NodeID *)(hlink->GetRemoteID());
  NodeID * originator = _myNode->copy();
  originator->SetLevel( tmp->GetLevel() );

  PeerID * origPG = originator->GetPeerGroup();
  delete tmp;

  ptse->AddIG(hlink);

  // This must be originated at the proper level for it to flow
  PTSPPkt * ptsp = new PTSPPkt( originator->GetNID(), origPG->GetPGID());
  ptsp->AddPTSE(ptse);
  
  NPFloodVisitor * ptspv = new NPFloodVisitor(ptsp);
  ptspv->SetSourceNID( originator );
  ptspv->SetDestNID( originator );
  _master->Advertise(ptspv);

  delete origPG;
  delete originator;
}



// builds and resends the fresh UpLink PTSE to the DB
void AggregationPolicy::InjectPTSE(ig_uplinks * uplink, int id, int seqnum, NodeID* orig)
{
  assert(uplink);

  ig_ptse * ptse = new ig_ptse(InfoGroup::ig_uplinks_id,
			       id, seqnum, 0, _life);
  assert(ptse);

  NodeID * originator = orig->copy();
  PeerID * origPG = originator->GetPeerGroup();

  ptse->AddIG(uplink);

  // This must be originated at the proper level for it to flow
  PTSPPkt * ptsp = new PTSPPkt( originator->GetNID(), origPG->GetPGID());
  ptsp->AddPTSE(ptse);
  
  NPFloodVisitor * ptspv = new NPFloodVisitor(ptsp);
  ptspv->SetSourceNID( originator );
  ptspv->SetDestNID( originator );
  _master->Advertise(ptspv);

  delete origPG;
  delete originator;
}




ig_resrc_avail_info * 
AggregationPolicy::
Default_Aggregate_Links(const list<const ig_resrc_avail_info *>& lst,
			int service_cat,
			ig_resrc_avail_info* current) {
  int mcr=      OC12;
  int acr=      OC12;
  int weight=   5040;
  int ctd=      202;
  int cdv=      182;
  int clr0=     8;
  int clr01=    8;

  // default link aggregation
  ig_resrc_avail_info * raig = AggrLink_Constant(lst, service_cat, mcr, acr, weight, ctd, cdv, clr0, clr01);

  // Now for the overrides
  
  // This forces only at most one policy to be active in testing
  DiagLevel("link_aggregation_policy",DIAG_ERROR);

  DIAG("link_aggregation_policy.average", DIAG_DEBUG,
       delete raig;
       raig = AggrLink_Average(lst, service_cat););
       
  DIAG("link_aggregation_policy.worst", DIAG_DEBUG,
       delete raig;
       raig = AggrLink_Worst(lst, service_cat););

  DIAG("link_aggregation_policy.best", DIAG_DEBUG,
       delete raig;
       raig = AggrLink_Best(lst, service_cat););

  DIAG("link_aggregation_policy.print_metrics", DIAG_DEBUG,
       list_item ri;
       const u_int flg = raig->GetFlags();
       if (flg & 0x8000) cout << "CBR ";       // Constant Bit Rate
       if (flg & 0x4000) cout << "RTVBR ";     // Realtime Variable Bit Rate
       if (flg & 0x2000) cout << "NRTVBR ";    // Non-realtime Variable Bit Rate
       if (flg & 0x1000) cout << "ABR ";       // Available Bit Rate
       if (flg & 0x0800) cout << "UBR ";       // Unspecified Bit Rate
       if (flg & 0x0001) cout << "GCAC_CLP ";  // Cell Loss Priority bit
       cout << ": ";
       
       const int weight = raig->GetAdminWeight();
       const int mcr = raig->GetMCR();
       const int acr = raig->GetACR();
       const int ctd = raig->GetCTD();
       const int cdv = raig->GetCDV();
       const int clr0 = raig->GetCLR0();
       const int clr01 = raig->GetCLR01();
       
       cout << "weight=" << weight << ", ";
       cout << "mcr=" << mcr << ", ";
       cout << "acr=" << acr << ", ";
       cout << "ctd=" << ctd << ", ";
       cout << "cdv=" << cdv << ", ";
       cout << "clr0=" << clr0 << ", ";
       cout << "clr01=" << clr01 << ", ";
       cout << endl; );

  return raig;
}

// Default RAIG computation algorithm for logical links, CBR service category
ig_resrc_avail_info * 
AggregationPolicy::
Compute_CBR(const list <const ig_resrc_avail_info *>& lst, 
	    ig_resrc_avail_info* current) 
{
  return Default_Aggregate_Links(lst, RAIG_FLAG_CBR, current);
}

// Default RAIG computation algorithm for logical links, RT VBR service
ig_resrc_avail_info * 
AggregationPolicy::
Compute_RTVBR(const list <const ig_resrc_avail_info *>& lst, 
	      ig_resrc_avail_info* current) 
{
  return Default_Aggregate_Links(lst, RAIG_FLAG_RTVBR, current);
}

// Default RAIG computation algorithm for logical links, NRT VBR service
ig_resrc_avail_info * 
AggregationPolicy::
Compute_NRTVBR(const list <const ig_resrc_avail_info *>& lst, 
	       ig_resrc_avail_info* current) 
{
  return Default_Aggregate_Links(lst, RAIG_FLAG_NRTVBR, current);
}

// Default RAIG computation algorithm for logical links, ABR service category
ig_resrc_avail_info * 
AggregationPolicy::
Compute_ABR(const list <const ig_resrc_avail_info *>& lst, 
	    ig_resrc_avail_info* current) 
{
  return Default_Aggregate_Links(lst, RAIG_FLAG_ABR, current);
}

// Default RAIG computation algorithm for logical links, UBR service category
ig_resrc_avail_info * 
AggregationPolicy::
Compute_UBR(const list <const ig_resrc_avail_info *>& lst, 
	    ig_resrc_avail_info* current) 
{
  return Default_Aggregate_Links(lst, RAIG_FLAG_UBR, current);
}




double AggregationPolicy::PTSERefreshInterval(void) const 
{  return _refresh;  }

void AggregationPolicy::SetDBLifetime(int life) 
{ _life = life; }

void AggregationPolicy::SetDBRefresh(double interval) 
{ _refresh = interval; }

void AggregationPolicy::SetMaster(Aggregator * s) 
{ _master = s; }

void AggregationPolicy::Register_ComplexRep(ComplexRep * cr)
{
  dic_item di;
  NodeID * lgn = cr->Get_LGN();  // This makes a copy each time it's called
  assert(! (di = _complexReps.lookup( lgn )));
  _complexReps.insert( lgn, cr);
}

void AggregationPolicy::Deregister_ComplexRep(ComplexRep * cr)
{
  dic_item di;
  NodeID * lgn = cr->Get_LGN();  // This makes a copy
  assert(di = _complexReps.lookup( lgn ));
  delete _complexReps.key( di );  // This was a copy
  _complexReps.del_item( di );
  delete lgn;
}


void AggregationPolicy::Update_Complex_Node(LogosGraph & lg, ComplexRep & cr)
{
  // Ultimately, we must check to see if there 
  // has been a significant change by comparing the
  // existing ComplexRep with the LogosGraph

  int mcr=      OC12;
  int acr=      OC12;
  int weight=   5040;
  int ctd=      202;
  int cdv=      182;
  sh_int clr0=  8;
  sh_int clr01= 8;

  // Clear all links
  cr.Clear_Links(); 

  // default aggregation
  AggrNode_Constant(lg,cr, mcr, acr, weight, ctd, cdv, clr0, clr01);

  // Now for the overrides

  // This forces only at most one policy to be active in testing
  DiagLevel("complexnode_aggregation_policy",DIAG_ERROR);

  DIAG("complexnode_aggregation_policy.bestcase_uniform", DIAG_DEBUG,
       cr.Clear_Links(); 
       AggrNode_BestCase_Uniform(lg,cr););
       
  DIAG("complexnode_aggregation_policy.bestcase_perclass", DIAG_DEBUG,
       cr.Clear_Links(); 
       AggrNode_BestCase_PerClass(lg,cr););

  DIAG("complexnode_aggregation_policy.random_full", DIAG_DEBUG,
       cr.Clear_Links(); 
       AggrNode_Random_Full(lg,cr,
			    0, mcr, 
			    0, acr, 
			    0, weight, 
			    0, ctd, 
			    0, cdv, 
			    0, clr0, 
			    0, clr01););

    DIAG("sim.aggregator.complexrep.print_topology",DIAG_DEBUG,
	 cr.Print_Topology(cout));
    DIAG("sim.aggregator.complexrep.print_metrics",DIAG_DEBUG,
	 cr.Print_Metrics(cout));
}

void AggregationPolicy::Update_Complex_Rep(const NodeID * lgn, int lgport, bool add)
{
  dic_item di;

  assert(lgn);
  assert( di = _complexReps.lookup( lgn ) );

  ComplexRep * cr = _complexReps.inf( di );

  if ( add ) {
    bool res = cr->Add_LogicalPort( lgport );
    if (!res) {
      // not too big a deal, since the LogicalPort is added when the SVC gets setup 
      DIAG ( "sim.aggregator", DIAG_INFO,
	     cout << "Requested addition of logical port " << lgport << " for LGN " 
	     << lgn->Print() << " has failed.  The port is already there!" << endl; );
    }
    else {
      cr->ReAggregate_PortCountChanged();
    }
  }
  else {
    bool res = cr->Remove_LogicalPort( lgport );
    if (!res) {
      // not too big a deal, since the LogicalPort is removed when the SVC gets released
      DIAG ( "sim.aggregator", DIAG_INFO,
	     cout << "Requested removal of logical port " << lgport << " for LGN " 
	     << lgn->Print() << " has failed.  The port is not there!" << endl; );
    }
    else {
      cr->ReAggregate_PortCountChanged();
    }
  }
}

const char * AggregationPolicy::GetName(void) const
{
  return _master->PrintName();
}
