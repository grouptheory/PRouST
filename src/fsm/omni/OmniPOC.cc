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
static char const _OmniPOC_cc_rcsid_[] =
"$Id: OmniPOC.cc,v 1.9 1999/01/14 13:36:04 mountcas Exp $";
#endif

#include "OmniPOC.h"
#include "Omni.h"
#include "OmniProxy.h"
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/next_hi_level_binding_info.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_pkt/ptsp.h>
#include <FW/basics/VisitorType.h>
#include <FW/basics/diag.h>
#include <FW/kernel/SimEvent.h>
#include <FW/kernel/SimEntity.h>
#include <fsm/visitors/NPFloodVisitor.h>

VisitorType * OmniPOC::_npflood_type = 0;

//----------------------------------------
OmniPOC::OmniPOC(const NodeID * me, bool logging)
  : _me( 0 ), _logging( logging ), _the_omni( theOmni() )
{
  _me = me->copy();

  if (!_npflood_type)
    _npflood_type = (VisitorType *)QueryRegistry(NPFLOOD_VISITOR_NAME);
}

//----------------------------------------
State * OmniPOC::Handle(Visitor * v) 
{
  if (v->GetType().Is_A(_npflood_type)) {
    NPFloodVisitor* npv = (NPFloodVisitor *) v;
    
    // here we
    // 1) keep the omni apprised of the DBKeys being altered in our database
    // 2) any election changes we are making
    Process_NPFV((NPFloodVisitor *) v);  

    // When the event arrives, we will compute synchrony
    SimEvent* se = new SimEvent(this, this, 0);  // interrupt me ASAP
    Deliver(se, 0.0);
  }

  PassThru(v);
  
  return this;
}

//----------------------------------------
void OmniPOC::Process_NPFV(NPFloodVisitor* npv) 
{
  const PTSPPkt * fptsp = npv->GetFloodPTSP();

  bool from_a = (VisitorFrom(npv) == Visitor::A_SIDE);

  NodeID oid(fptsp->GetOID());
  bool made_by_me_at_some_level = _me->Is_Me_At_Some_Level( &oid );

  bool only_nullbodies;
  if ( ! from_a )                            // if it comes from our DB
    {
      if (made_by_me_at_some_level) {        // and we are the originator
      only_nullbodies = true;

      // we make note of our nullbody PTSEs
      Process_PTSP(fptsp, only_nullbodies, made_by_me_at_some_level);   
    }
    // else... but if we aren't the originator, we don't care...
  }
  else // we always make note of *all* PTSPs from outside the switch
    { 
      only_nullbodies = false;
      Process_PTSP(fptsp, only_nullbodies, made_by_me_at_some_level);
    }

  if (made_by_me_at_some_level) {
    NPFloodVisitor * npv_copy = (NPFloodVisitor*) ( npv->duplicate() );
    _the_omni.UpdateOmniDB(npv_copy);
  }
}

//----------------------------------------
void OmniPOC::Process_PTSP(const PTSPPkt * ptsp, 
			   bool only_nullbodies,
			   bool made_by_me_at_some_level)
{
  assert(ptsp != 0);

  // loop through ptsp->_elements (all of which are ig_ptse *),
  // storing them.
  const list<ig_ptse *> & ptse_list = ptsp->GetElements();

  NodeID nid(ptsp->GetOID());
  PeerID pid(ptsp->GetPGID());
  
  list_item li;
  forall_items(li, ptse_list) {
    ig_ptse * tmp = (ptse_list.inf(li));

    if ( ! only_nullbodies)
      Process_PTSE(&nid, tmp, made_by_me_at_some_level);
    else // we are under mandate to only process nullbodies
      {
	if (tmp->GetTTL() == DBKey::ExpiredAge) 
	  Process_PTSE(&nid, tmp, made_by_me_at_some_level);
      }
  }
}

//----------------------------------------
void OmniPOC::Process_PTSE(NodeID* Originator, 
			   ig_ptse * ptse,
			   bool made_by_me_at_some_level) 
{
  assert(ptse != 0);

  // Generate a proper DBKey for this PTSE
  PeerID * pgid = Originator->GetPeerGroup();
  DBKey * dbk = new DBKey(ptse->GetType(), Originator, ptse->GetID(),
			  ptse->GetSN(), pgid, 
			  ptse->GetTTL());

  if ( _logging )
    theOmniProxy().notify( _me, dbk );

  // for synchrony calculations
  _the_omni.Report_PTSE( _me, dbk );

  if (made_by_me_at_some_level) { 
    switch (ptse->GetType()) {
    case InfoGroup::ig_nodal_info_group_id :
      Process_My_Nodal_IG(Originator, ptse);
      break;
    default:
      break;
    }
  }
}

//----------------------------------------
void OmniPOC::Process_My_Nodal_IG(NodeID* Originator, 
				  ig_ptse * ptse)
 {
  const list<InfoGroup *> * igs = ptse->ShareMembers();

  // if someone is expiring this PTSE...

  if (( ! igs ) ||
      ( igs->size() == 0 )) {
    NodeID * undecided = 
      new NodeID("00:000:0000000000000000000000000000000000000000");
    _the_omni.Leadership_Notice( Originator, undecided );
    return;
  }

  // otherwise, there are IGs...

  int ct = 0;
  list_item li;
  forall_items(li, *igs) {
    ig_nodal_info_group* nig = (ig_nodal_info_group*) (igs->inf(li));
    const ig_next_hi_level_binding_info * nhlb = nig->GetNextHigherLevel();
    if (nhlb) {
      NodeID * lgn = (NodeID*) (nhlb->GetParentLogGroupID());
      assert(lgn);
      _the_omni.Leadership_Notice( Originator, lgn );
      ct++;
    }
    else {
      NodeID * undecided = 
	new NodeID("00:000:0000000000000000000000000000000000000000");
      _the_omni.Leadership_Notice( Originator, undecided );
      ct++;
    }
  }
  assert( ct <= 1 );
}

//----------------------------------------
void OmniPOC::Interrupt(SimEvent * se) 
{
  _the_omni.RecomputeGlobalSynchrony( _me );
  delete se;
}

//----------------------------------------
void OmniPOC::SetLogging(bool l)
{
  _logging = l;
}

//----------------------------------------
OmniPOC::~OmniPOC() 
{
  delete _me;
}
