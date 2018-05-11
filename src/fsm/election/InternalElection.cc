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
static char const _InternalElection_cc_rcsid_[] =
"$Id: InternalElection.cc,v 1.71 1999/02/11 19:24:36 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <fsm/election/InternalElection.h>
#include <fsm/election/ElectionState.h>
#include <fsm/election/ElectionTimers.h>
#include <fsm/election/ElectionVisitor.h>
#include <fsm/nodepeer/NPVisitors.h>
#include <fsm/hello/HelloVisitor.h>
#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/visitors/PortDownVisitor.h>
#include <fsm/visitors/LGNVisitors.h>
#include <fsm/netstats/NetStatsCollector.h>

#include <FW/basics/diag.h>
#include <FW/basics/Conduit.h>
#include <FW/kernel/Kernel.h>

#include <codec/pnni_ig/nodal_info_group.h>

extern const double SEARCH_PEER_INTERVAL;
extern const double PGL_INIT_INTERVAL;
extern const double OVERRIDE_INTERVAL;
extern const double REELECTION_INTERVAL;

VisitorType * InternalElectionState::_portup_vistype   = 0;
VisitorType * InternalElectionState::_svcc_vistype     = 0;
VisitorType * InternalElectionState::_portdown_vistype = 0;
VisitorType * InternalElectionState::_npstate_vistype  = 0;
VisitorType * InternalElectionState::_election_vistype = 0;
VisitorType * InternalElectionState::_lgndest_vistype  = 0;

InternalElectionState::InternalElectionState(void) 
{ 
  if (!_portup_vistype)
    _portup_vistype = (VisitorType *)QueryRegistry(PORT_UP_VISITOR_NAME);
  if (!_portdown_vistype)
    _portdown_vistype = (VisitorType *)QueryRegistry(PORT_DOWN_VISITOR_NAME);
  if (!_svcc_vistype)
    _svcc_vistype   = (VisitorType *)QueryRegistry(SVCC_PORT_INFO_VISITOR);
  if (!_npstate_vistype)
    _npstate_vistype  = (VisitorType *)QueryRegistry(NPSTATE_VISITOR_NAME);
  if (!_election_vistype)
    _election_vistype = (VisitorType *)QueryRegistry(ELECTION_VISITOR_NAME);
  if (!_lgndest_vistype)
    _lgndest_vistype  = (VisitorType *)QueryRegistry(LGN_DESTRUCTION_VISITOR);
}

InternalElectionState::~InternalElectionState( ) { }

void InternalElectionState::RegisterTimer(ElectionState * e, 
					  ElectionState::ElectionTimers t)
{ e->RegisterTimer(t); }

void InternalElectionState::CancelTimer(ElectionState * e, 
					ElectionState::ElectionTimers t)
{ e->CancelTimer(t); }

bool InternalElectionState::PeerFound(ElectionState * e, Visitor *& v)
{ return false; }

bool InternalElectionState::LostAllPeers(ElectionState * e, Visitor * v)
{ return false; }

bool InternalElectionState::DBReceived(ElectionState * e, Visitor *& v)
{ return false; }

bool InternalElectionState::InsertNodeID(ElectionState * e, ElectionVisitor * v)
{ return false; }

bool InternalElectionState::InsertHorzID(ElectionState * e, Visitor * v)
{ return false; }

InternalElectionState::ElectionTransitions 
InternalElectionState::GetEventType(ElectionState * e, Visitor * v)
{
  ElectionTransitions rval = Invalid;
  if (v->GetType().Is_A(_portup_vistype)) {
    UpdatePorts(e, (LinkVisitor *)v);
    rval = _PeerFound;
  } else if (v->GetType().Is_A(_svcc_vistype) &&
	     e->GetNodeType() == ElectionState::LogicalNode) {
    UpdatePorts(e, (VPVCVisitor *)v);
    rval = _PeerFound;
  } else if (v->GetType().Is_A(_portdown_vistype)) {
    if (AllPeersLost(e, (LinkVisitor *)v))
      rval = _LostAllPeers; 
    else
      rval = PassThrough;
  } else if (v->GetType().Is_A(_npstate_vistype)) {
    NPStateVisitor::NPStateVisitorType  type = ((NPStateVisitor *)v)->GetVT();
    if (type == NPStateVisitor::FullState)
      rval = _DBReceived; 
    else
      rval = PassThrough;
  } else if (v->GetType().Is_A(_election_vistype)) {
    ElectionVisitor::Purpose reason = ((ElectionVisitor *)v)->GetPurpose();
    if (reason == ElectionVisitor::InsertedNodalInfo)
      rval = _NodalIGInserted; 
    else if (reason == (ElectionVisitor::HorizontalLinkUp) || 
             (reason == (ElectionVisitor::HorizontalLinkDown)))
      rval = _HorzlinkInserted;
    else
      rval = Invalid;
  } else if (rval == Invalid)
    rval = PassThrough;

  return rval;
}

void InternalElectionState::PassVisitorToA(ElectionState * e, Visitor * v)
{ if (v) e->SendVisitor(v, Visitor::A_SIDE); }

void InternalElectionState::PassVisitorToB(ElectionState * e, Visitor * v)
{ if (v) e->SendVisitor(v, Visitor::B_SIDE); } 

void InternalElectionState::PassThru(ElectionState * e, Visitor * v)
{ if (v) e->SendVisitor(v, Visitor::OTHER); }

// Delete the return from this method and DIE!!!!!!!!!!
const NodeID * InternalElectionState::GetPreferredPGL(ElectionState * e) const
{
  assert(e);

  const NodeID * rval = e->GetPreferredPGL();

  if (!rval) {
    rval = (NodeID *)e->GetPreferredPGL(e->GetLocalNID());
    e->SetPreferredPGL( rval );
  }
  return rval;
}

const NodeID * InternalElectionState::GetOldPGL(ElectionState * e) const
{ 
  return e->GetOldPGL(); 
}

const Nodalinfo * InternalElectionState::GetNodalInfo(ElectionState * e, 
						      const NodeID * node) const
{ 
  return e->GetNodalinfo(node);
}

bool InternalElectionState::PreferMyself(ElectionState * e) const
{
  const NodeID * PPGLnid = GetPreferredPGL(e);
  bool rval = false;

  if (e && (PPGLnid && e->GetLocalNID())) {
    rval = (*PPGLnid == *(e->GetLocalNID()));
    DIAG("fsm.election", DIAG_DEBUG, cout << "The LocalNodeID is " 
	 << *GetLocalNode(e) << endl << "The PreferredPGL is " 
	 << *PPGLnid<< endl);
  }
  return rval;
}

bool InternalElectionState::OldIsPref(ElectionState * e) const
{
  if (e && GetPreferredPGL(e) && GetOldPGL(e)) {
    DIAG("fsm.election", DIAG_DEBUG, cout <<
	 "Preferred PGL " << *(GetPreferredPGL(e)) << endl <<
	 "Old PGL " << *(GetOldPGL(e)) << endl);
    return (GetPreferredPGL(e)->equals(GetOldPGL(e)));
  }
  return false;
}

const char * InternalElectionState::OwnerName(ElectionState * e) const
{ 
  assert(e);
  return e->GetName();
}

const NodeID * InternalElectionState::GetLocalNode(ElectionState * e) const
{ 
  if (e) return e->GetLocalNID(); return 0; 
}

void InternalElectionState::PrefPrioToOld(ElectionState * e) 
{ 
  if (e) {
    e->SetOldPrio( e->GetPreferredPrio() );
    e->SetOldPGL( e->GetPreferredPGL() );
  }
}

void InternalElectionState::ChangeState(ElectionState * e, 
					ElectionState::StateID s)
{ 
  if (e) e->ChangeState(s); 
}

void InternalElectionState::SetPreferredPGL(ElectionState * e, const NodeID * pgl)
{
  assert(e != 0);
  e->SetPreferredPGL(pgl);
}

void InternalElectionState::SetPrefPrio(ElectionState * e, int prio)
{ 
  if (e) 
    e->SetLocalPrio( prio );
}

void InternalElectionState::SetPrefPGLPrio(ElectionState * e, int prio)
{ if (e) e->SetPreferredPrio( prio ); }

int InternalElectionState::GetPrefPrio(ElectionState * e) const 
{ if (e) return e->GetLocalPrio(); return 0; } 

int InternalElectionState::GetPrefPGLPrio(ElectionState * e) const
{ if (e) return e->GetPreferredPrio(); return 0; }

int InternalElectionState::GetOldPGLPrio(ElectionState * e) const
{ if (e) return e->GetOldPrio(); return 0; }

int InternalElectionState::GetVotes(ElectionState * e) const 
{ if (e) return e->GetVotes(); return 0; }

void InternalElectionState::Election(ElectionState * e)
{
  e->CalculateVotes();

  DIAG("fsm.election", DIAG_DEBUG, 
       if (e->NumVoters() > 0)
         cout << OwnerName(e) << " The vote count is " 
         << GetVotes(e) << "/" << e->NumVoters() << endl;
       else
         cout << OwnerName(e) << "NOTICE: I am the only voting member." << endl;
      );
}

bool InternalElectionState::Unanimity(ElectionState * e)
{
  if (GetVotes(e) == (e->NumVoters())) { 
    do_PGLE(e, 0, 8); 
    diag("fsm.election", DIAG_DEBUG, 
	 "%s Electionstate is PGL \n", OwnerName(e));

    ChangeState(e, ElectionState::OperPGL);
    return true;
  } else {
    diag("fsm.election", DIAG_DEBUG, "%s Unanimity failed\n"
	 "%s Total votes: %d\nVotes for: %d \n",
	 OwnerName(e), OwnerName(e), e->NumVoters(),
	 GetVotes(e));
    return false; 
  }
}

bool InternalElectionState::HelloFSMStarted(ElectionState * e, Visitor * v)  
{ return false; }

bool InternalElectionState::SearchPeerTimerExpiry(ElectionState * e)   
                                                          { return false; } 
bool InternalElectionState::PGLInitTimerExpiry(ElectionState *e)      
                                                          { return false; }
bool InternalElectionState::OverrideUnanimityExpiry(ElectionState * e) 
                                                          { return false; } 
bool InternalElectionState::OverrideUnanimitySucc(ElectionState *e)   
                                                          { return false; }
bool InternalElectionState::OverrideUnanimityFail(ElectionState *e)   
                                                          { return false; }
bool InternalElectionState::LoseConnToPGL(ElectionState * e)           
                                                          { return false; }
bool InternalElectionState::GetConnToPGL(ElectionState *e)            
                                                          { return false; }
bool InternalElectionState::ReelectionTimerExpiry(ElectionState * e)   
                                                          { return false; }
void InternalElectionState::PreferredPGLNotMe(Visitor *) { }
void InternalElectionState::PreferredPGLMe(Visitor *) { }

bool InternalElectionState::TwoThirdReached(ElectionState * e)
{
  float sz = e->NumVoters();
  float twoThirds = (2.0 / 3.0) * sz;
  // We must have atleast 1 voting member
  if ((sz > 0) && (GetVotes(e) >= (int)twoThirds))
    return true;
  return false;
}

bool InternalElectionState::ChangePreferredPGL(ElectionState * e)
{
  // this is for OperNotPGL, AwtUnan, HungEle, AwtRele only
  do_PGLE(e, 0, 4);
  return true;
}

NodeID * InternalElectionState::EvalPreferredPGL(ElectionState * e, int & prio)
{
  if (e->NumVoters() == 0) {
    prio = GetPrefPGLPrio(e); // BK 1/19/99 
    return (NodeID*)e->GetLocalNID();
  }

  seq_item si;
  NodeID * rval = (NodeID*)GetPreferredPGL(e);

  // The NodeID of my current preferred node
  NodeID * OldPGL = (NodeID *)GetPreferredPGL(e);
  NodeID * tmpPGL = OldPGL;

  // Iterate over the list of nodes I currently know about
  const sortseq<const NodeID *, Nodalinfo *> * members = e->GetVoters();

  forall_items(si, *members) {
    const NodeID * nid = members->key(si);
    Nodalinfo    * nif = members->inf(si);

    if (nif->GetElectionFlag()) {
      // if they want to be involved in the election
      DIAG("fsm.election", DIAG_DEBUG, cout << "**** " << *e->GetLocalNID()
	   << ": Nodes for eval preferred pgl are " << endl
	   << *nid << " " << nif->GetPriority() << endl);

      // If I prefer no one, perhaps I should prefer who this guy prefers
      if (!tmpPGL) {
        prio = nif->GetPriority();     // SetPrefPGLPrio(e, nif->GetPriority()); 
        rval = (NodeID *)nid;          // SetPreferredPGL(e, nid);
	tmpPGL = rval;
      }

      if ((GetPrefPGLPrio(e) < nif->GetPriority()) &&
	  !nid->IsZero()) {
	// If the leadership priority of this node is greater 
	// than my preferred node's priority, switch sides.
        prio = nif->GetPriority();                                  // SetPrefPGLPrio(e, nif->GetPriority()); 
        rval = (NodeID *)nid;                                       // SetPreferredPGL(e, nid);
      } else if ( (GetPrefPGLPrio(e) == nif->GetPriority()) &&
		  (( GetPreferredPGL(e) == 0 || *nid > *GetPreferredPGL(e)) ||
		   ( rval != 0 && *nid > *rval)) ) {

	prio = nif->GetPriority(); // BK 1/19/99

	// If the priority is the same, the tie needs to be broken.
	rval = (NodeID *)nid;                                       // SetPreferredPGL(e, nid);
      } else
	prio = GetPrefPGLPrio(e);
      // else my preferred PGL has a higher leadership priority
    }
    // else this node doesn't want to be involved in the elections
  }

  // e->SetOldPGL( OldPGL );

  assert(rval);
  assert(prio != -1); // BK 1/19/99
  /*
  DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
       << " EvalPreferredPGL Result is " << endl << "\t" << rval->Print()
       << " with a priority of " << prio << endl;
       if (! (rval->equals( OldPGL )) )
         cout << "\t!!!!! The PGL has changed from " << OldPGL->Print() << endl;
         cout << "\t!!!!! The PGL has changed to " << rval->Print() << endl;
       );
       */
  //  if (! (GetPreferredPGL(e)->equals( OldPGL )) )
  //    return true;
  //  return false;
  return rval;
}

void InternalElectionState::UpdatePorts(ElectionState * e, LinkVisitor * v)
{
  e->UpdatePorts(v);
}

void InternalElectionState::UpdatePorts(ElectionState * e, VPVCVisitor * v)
{
  e->UpdatePorts(v);
}

bool InternalElectionState::AllPeersLost(ElectionState * e, LinkVisitor * v)
{
  const NodeID * rnid = v->GetSourceNID();

  return e->ClearPorts(rnid, v);
}

bool InternalElectionState::DetermineConnToNode(ElectionState * e, 
						const NodeID * node, 
						InsertionFlags flag)
{
  DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e) 
       << " Determine conn to " 
       << *node << endl;);

  seq_item s, si;
  list_item li;

  const sortseq<const NodeID *, Nodalinfo *> * members = e->GetVoters();
  const sortseq<const NodeID *, list<int> *> * ports = e->GetPorts();

  if (node && (s = members->lookup(node))) {
    Nodalinfo * nif = members->inf(s);
    if (nif->GetPriority() && nif->GetElectionTransit()) {
      if ((flag == Electiontransit) ||
          (flag == HlinkUp))  {
        if (si = ports->lookup(node)) {
          if (!nif->GetElectionFlag())
            nif->SetElectionFlag(true); 
          return true;
        } else {
          if (!nif->GetElectionFlag())
            return false;
          return true;
        }
      } else if ((flag == ElectionNontransit) ||
		 (flag == HlinkDown)) {
        if (si = ports->lookup(node)) {
          if (!nif->GetElectionFlag())
            nif->SetElectionFlag(true);
          return true;
        } else {
          if (nif->GetElectionFlag())
            return false;
          return true;
        } 
      } else if (flag == all) {
        if (si = ports->lookup(node)) {
          if (!nif->GetElectionFlag())
            nif->SetElectionFlag(true);
          return true;
        }
        return false;
      }
    } else {
      return true;
    }
  }
  return false;
}

void InternalElectionState::DetermineConnToNode(ElectionState * e, 
						InternalElectionState::InsertionFlags flag)
{
  seq_item si;
  sortseq<const NodeID *, Nodalinfo *> listOfNodes;

  diag("fsm.election", DIAG_DEBUG, 
       "%s Total numbers in _members whose conn is be found is %d \n", 
       OwnerName(e), e->NumVoters());

  const sortseq<const NodeID *, Nodalinfo *> * members = e->GetVoters();
  forall_items(si, *members) { 
    const NodeID * node = members->key(si); 
    Nodalinfo    * nif  = members->inf(si);
    if (node->equals(e->GetLocalNID())) 
      continue;
    if (!nif->GetPriority() || !nif->GetElectionTransit()) continue;
    if (!DetermineConnToNode(e, node, flag))
      listOfNodes.insert(node, nif);
  }
  diag("fsm.election", DIAG_DEBUG,
       "%s ListOFNodes size in determine connection %d \n", 
       OwnerName(e), listOfNodes.size());
  if (!listOfNodes.empty()) {
    ElectionVisitor * cv = 
      new ElectionVisitor((NodeID *)GetLocalNode(e), &listOfNodes, 
                          ElectionVisitor::RequestConnectivityInfo);
    PassVisitorToB(e, cv);
  }
}

bool InternalElectionState::RemoveEntry(ElectionState * e, NodeID * nid)
{
  seq_item si;

  const sortseq<const NodeID *, Nodalinfo *> * members = e->GetVoters();
  if (nid && (si = members->lookup(nid))) {
    Nodalinfo * coentry = members->inf(si);
    coentry->SetElectionFlag(false);
    return true;
  }
  return false;
}

// This is the only place where _members can be modified
void InternalElectionState::StoreEntry(ElectionState * e, 
				       NodeID * nid, 
				       ig_nodal_info_group * nig)
{
  // Find out whom the newly inserted node prefers
  NodeID * nig_prefpgl = (NodeID *)nig->GetPreferredPGL();

  seq_item si;
  const sortseq<const NodeID *, Nodalinfo *> * members = e->GetVoters();
  if (nid && (si = members->lookup(nid))) {
    // This Node has already been entered into our sorted sequence
    Nodalinfo    * nif = members->inf(si);
    const NodeID * old_ppgl = nif->GetPrefPGL();

    DIAG("fsm.election", DIAG_INFO, cout << "------- " << *e->GetLocalNID()
	 << ": Change in PrefPGL for " << endl << "\t" << *nid << endl;
	 if (nif->GetPrefPGL()) cout << "\tOld PGL " << *old_ppgl << endl;
	 if (nig_prefpgl) cout << "\tNew PGL " << *nig_prefpgl << endl);
	 
    nif->SetPriority(nig->GetLeadershipPriority());
    // If this is our nodal IG, set our preferred priority
    if (nid->equals(e->GetLocalNID()))
      e->SetLocalPrio( nig->GetLeadershipPriority() );

    if (old_ppgl && nig_prefpgl && !(old_ppgl->equals(nig_prefpgl)))
      nif->SetPrefPGL(nig_prefpgl);
    else  if (old_ppgl && !nig_prefpgl)
      nif->SetPrefPGL(0);
    else if (!old_ppgl && nig_prefpgl)
      nif->SetPrefPGL(nig_prefpgl);

    nif->SetElectionTransit(!(nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)));
    // If the node specified no priority or doesn't want to be involved in elections
    //    set the election fit to false
    if (!nif->GetElectionTransit())
      nif->SetElectionFlag(false);
    else
      nif->SetElectionFlag(true);
  } else {
    DIAG("fsm.election", DIAG_DEBUG, cout << "------- " << *e->GetLocalNID() << 
	 ": Adding new Election Information for " << endl << "\t" << *nid
	 << endl << "\tPreferredPGL is ";
	 if (nig_prefpgl)
	   cout << *nig_prefpgl << endl;
	 else
	   cout << "0" << endl;
	 );

    NodeID * pgl_node = 0;
    if (nig_prefpgl)
      pgl_node = new NodeID(*nig_prefpgl);
    Nodalinfo * nif = 
      new Nodalinfo(nid, nig->GetLeadershipPriority(), 
		    pgl_node, !(nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)));

    // If this is our nodal IG and we want to be involved in elections and
    //   we have specified a leadership priority, set the election fit to
    //   true, and set our priority.
    if ((nid->equals(e->GetLocalNID())) && nif->GetElectionTransit() && 
        nif->GetPriority()) {
      nif->SetElectionFlag(true);
      e->SetLocalPrio( nig->GetLeadershipPriority() );
    }

    // Changed 2/9/99 mountcas
    nif->SetElectionFlag( ( !nif->GetElectionTransit() ) ? false : true );
    // nif->SetElectionFlag( (!nif->GetElectionTransit() || !nif->GetPriority()) ? false : true );

    const sortseq<const NodeID *, Nodalinfo *> * members = e->GetVoters();
    if (si = members->lookup(nid))
      delete members->inf(si);
    e->AddVoter(nid->copy(), nif);

    if (pgl_node) delete pgl_node;
  }
  delete nig_prefpgl;
}

Nodalinfo * InternalElectionState::GetEntry(ElectionState * e,
					    const NodeID * nid) const
{
  seq_item si;
  const sortseq<const NodeID *, Nodalinfo *> * members = e->GetVoters();
  if (nid && (si = members->lookup(nid))) {
    Nodalinfo * nif = members->inf(si); 
    return nif;
  } else
    return 0;
}

void InternalElectionState::SetEntry(ElectionState * e, const NodeID * nid)
{
  seq_item si;

  const sortseq<const NodeID *, Nodalinfo *> * members = e->GetVoters();
  if (nid && (si = members->lookup(nid))) {
    Nodalinfo * nif = members->inf(si);
    DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e) << " " << *nid 
         << " Node is election eligible." << endl);
    nif->SetElectionFlag(true);
  }
  return;
}

void InternalElectionState::do_PGLE(ElectionState * e, Visitor * v, int code, 
				    NodeID * pref = 0, int prefPrio = 0)
{
  NodeID * myself = 0;

  Kernel & kern = theKernel();
  ElectionVisitor * send = 0;
  switch (code) {
    case 0:
      break;
    case 1:
      DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
	   << " Doing PGLE1 at " << kern.CurrentSimTime() << endl;);
      RegisterTimer(e, ElectionState::SearchPeerTIMER);
      break;
    case 2:
      DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
	   << " Doing PGLE2 at " << kern.CurrentSimTime() << endl;);
      CancelTimer(e, ElectionState::SearchPeerTIMER);
      break;
    case 3:
      DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
	   << " Doing PGLE3 at " << kern.CurrentSimTime() << endl;);
      RegisterTimer(e, ElectionState::SearchPeerTIMER);
      break;
    case 4:
      {
	DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
	     << " Doing PGLE4 at " << kern.CurrentSimTime() << endl;);
	CancelTimer(e, ElectionState::OverrideUnanimityTIMER);
	CancelTimer(e, ElectionState::ReelectionTIMER);
	// Chose the people to be considered to be chose for PGL(or election)
	int pprio = -1;
	NodeID * pref = EvalPreferredPGL(e, pprio);
	if ( pref != 0 ) {  //      if (GetPreferredPGL(e)) { 
	  if ( pref->equals( e->GetLocalNID() ) ) // 	if (PreferMyself(e))
	    do_PGLE(e, 0, 9, pref, pprio); // BK-bug: prefPrio);
	  else {
	    do_PGLE(e, 0, 7, pref, pprio); // BK-bug: prefPrio);
	    
	    PrefPrioToOld(e);
	    diag("fsm.election", DIAG_DEBUG, "%s ElectionState is NOTPGL\n", 
		 OwnerName(e));
	    ChangeState(e, ElectionState::OperNotPGL);
	  }
	} else {
	  // Unable to Evaluate a preferred PGL. Because all nodes
	  // declared a zero priority  lets go to OperNotPglstate.
	  // and wait.
	  diag("fsm.election", DIAG_DEBUG, "%s ElectionState is NOTPGL\n", 
	       OwnerName(e));
	  ChangeState(e, ElectionState::OperNotPGL);
	}
      }
      break;
    case 5:
      DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
	   << " Doing PGLE5 at " << kern.CurrentSimTime() << endl;);
      RegisterTimer(e, ElectionState::PGLInitTIMER);
      // Pass a visitor to acac to originate a ig ptse with nodal flag
      DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e) <<
           " Sending an ElectionVisitor to ACAC declaring priority at"
           " initial delay stage." << endl << OwnerName(e) << " Priority is " 
           << GetPrefPrio(e) << endl;);

      send = new ElectionVisitor((NodeID *)GetLocalNode(e), 0, 0,
                                 ElectionVisitor::ToOriginateANodalinfo, 
                                 ElectionVisitor::PGLE5);
      PassVisitorToB(e, send);
      break;
    case 6: 
      {
	DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
	     << " Doing PGLE6 at " << kern.CurrentSimTime() << endl;);
	// Add capability here to give up PGL
	DIAG("fsm.election", DIAG_INFO, 
	     cout << OwnerName(e) 
	     << " Election: at "
	     << kern.CurrentSimTime() 
	     << *(e->GetLocalNID())
	     << " is no longer PGL" << endl;);
	
	int pprio = -1;
	NodeID * pref = EvalPreferredPGL(e, pprio);
	if (pref != 0) { // if (GetPreferredPGL(e)) {
	  if (pref->equals(e->GetLocalNID())) { // if (PreferMyself(e)) {
	    diag("fsm.election", DIAG_DEBUG, 
		 " %s ERROR: Our Node itself PGL again after EvalPreffered PGL",
		 OwnerName(e));
	  }
	}
	send = new ElectionVisitor((NodeID *)GetLocalNode(e), 
				   pref, // (NodeID *)GetPreferredPGL(e), 
				   pprio, // GetPrefPrio(e), 
				   ElectionVisitor::ToOriginateANodalinfo, 
				   ElectionVisitor::PGLE6);
	PassVisitorToB(e, send);
	PrefPrioToOld(e);
	ChangeState(e, ElectionState::OperNotPGL); 
      }
      break;
    case 7:
      {
	DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
	     << " Doing PGLE7 at " << kern.CurrentSimTime() << endl);
	
	if ( pref != 0 ) {
	  SetPreferredPGL( e, pref );
	  SetPrefPGLPrio( e, prefPrio );
	}

	DIAG("fsm.election", DIAG_INFO, 
	     cout << OwnerName(e) 
	     << " Election: at "
	     << kern.CurrentSimTime() 
	     << " the PGL for " 
	     << *(e->GetLocalNID())
	     << " is ";
	     if (GetPreferredPGL(e) != 0) cout << *GetPreferredPGL(e) << endl;
	     else cout << "0" << endl; );

	// SHOULD WE BE ASSERTING THAT WE PREFER SOMEONE HERE?
	// assert( GetPreferredPGL( e ) != 0 );

	// If the preferred Peer Group Leader is different from the previously 
	// advertised one, a new instance of the Nodal Information PTSE must be 
	// originated with the new preferred Peer Group Leader and the 'I am 
	// leader' bit set to  zero.
	if ( GetPreferredPGL( e ) != 0 &&
	     ! GetPreferredPGL( e )->equals( GetOldPGL( e ) ) ) {
	  send = new ElectionVisitor((NodeID *)GetLocalNode(e), 
				     pref ? pref : (NodeID *)GetPreferredPGL(e), 
				     prefPrio ? prefPrio : GetPrefPrio(e), 
				     ElectionVisitor::ToOriginateANodalinfo, 
				     ElectionVisitor::PGLE7);
	  PassVisitorToB(e, send);
	  PrefPrioToOld(e);
	}
      }
      break;
    case 8:
      DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
	   << " Doing PGLE8 at " << kern.CurrentSimTime() << endl;);

      DIAG("fsm.election", DIAG_INFO,
	   cout << OwnerName(e) 
	   << " Election: at "
	   << kern.CurrentSimTime() 
	   << " the PGL for " 
	   << *(e->GetLocalNID())
	   << " is "
	   << *(GetPreferredPGL(e)) << endl);

      CancelTimer(e, ElectionState::OverrideUnanimityTIMER);
      send =  new ElectionVisitor((NodeID *)GetLocalNode(e), 
				  pref ? pref : GetPreferredPGL(e), 
                                  prefPrio ? prefPrio : GetPrefPrio(e), 
                                  ElectionVisitor::ToOriginateANodalinfo, 
                                  ElectionVisitor::PGLE8);
      PassVisitorToB(e, send);
      break;
    case 9:
      DIAG("fsm.election", DIAG_INFO,
	   cout << OwnerName(e) << " Election: "
	   << *(e->GetLocalNID()) << " is now waiting for Unanimity." << endl);

      DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
	   << " Doing PGLE9 at " << kern.CurrentSimTime() << endl);

      // BK 1/19/99 -- We must vote for ourselves NOW
      { 
	e->SetPreferredPGL( (myself = e->GetLocalNID()->copy()) );
	{
	  Nodalinfo * niff = (Nodalinfo *) GetNodalInfo(e, myself);
	  if (niff) {
	    niff->SetPrefPGL( myself );
	  }
	  else {
	    // We must insert a new Nodalinfo into the _members 
	    // datastructure...  But why wasn't there one already
	    // made *before* we registered the Searchpeertimer?
	    // In fact, why did we do PGLE4,PGLE9 upon expiry of
	    // the Searchpeertimer anyway?
	    abort();
	  }
	}
	delete myself;
      }

      Election(e);
      if (!Unanimity(e)) {
        send = new ElectionVisitor((NodeID *)GetLocalNode(e), 
				   GetPreferredPGL(e), 
				   GetPrefPrio(e),
                                   ElectionVisitor::ToOriginateANodalinfo, 
                                   ElectionVisitor::PGLE9);
        PassVisitorToB(e, send);
        // the line under may not be required here.
	PrefPrioToOld(e);
        RegisterTimer(e, ElectionState::OverrideUnanimityTIMER);
        diag("fsm.election", DIAG_DEBUG, 
	     "%s ElectionState is AwaitUnanimity \n",
             OwnerName(e));
        ChangeState(e, ElectionState::AwaitUnanimity);
      }
      break;

    case 10:
      DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
	   << " Doing PGLE10 at " << kern.CurrentSimTime() << endl;);
      diag("fsm.election", DIAG_DEBUG, "Started ReelectionTimer\n");
      RegisterTimer(e, ElectionState::ReelectionTIMER);
      break;
    case 11:
      DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) 
	   << " Doing PGLE11 at " << kern.CurrentSimTime() << endl;);
      diag("fsm.election", DIAG_DEBUG, "%s Cancelled Reelection Timer\n", 
           OwnerName(e));
      CancelTimer(e, ElectionState::ReelectionTIMER);
      break;
  }
}

InternalElectionState * InternalElectionState::Handle(ElectionState * e, 
						      Visitor * v)
{
  if (v->GetType().Is_A(_lgndest_vistype)) {
    v->Suicide();
    e->DeleteParent();
    return 0;
  }

  switch (GetEventType(e, v)) {
    case _PeerFound:
      PeerFound(e,v);
      PassThru(e,v);
      break;
    case _LostAllPeers:
      LostAllPeers(e,v);
      PassThru(e,v);
      break;
    case _DBReceived:
      DBReceived(e,v);
      PassThru(e, v);
      break;
    case _NodalIGInserted:
      InsertNodeID(e,(ElectionVisitor *)v);
      v->Suicide();
      break;
    case _HorzlinkInserted:
      InsertHorzID(e, (ElectionVisitor *)v);
      v->Suicide();
      break;
    default:
      if (!(v->GetType().Is_A(_election_vistype)))
	PassThru(e,v);
      else
	v->Suicide();
      break;
  }
  return this;
}

// ---------------------------------------------------------------------------
StateStarting::StateStarting(void) : InternalElectionState() { }

StateStarting::~StateStarting() { }

const char * StateStarting::StateName(void) const { return "StateStarting"; }

// ---------------------------------------------------------------------------
StateAwaiting::StateAwaiting(void) : InternalElectionState() { }
StateAwaiting::~StateAwaiting() { }

const char * StateAwaiting::StateName(void) const { return "StateAwaiting"; }

bool StateAwaiting::SearchPeerTimerExpiry(ElectionState * e)
{
  DIAG("fsm.election", DIAG_DEBUG, cout << *(e->GetLocalNID()) <<
       " " << StateName() << 
       " **** SearchPeerTimer expired, going to elect myself. ****" << endl);
  DetermineConnToNode(e, all);
  ChangePreferredPGL(e);
  return true;
}

bool StateAwaiting::PeerFound(ElectionState * e, Visitor *& v)
{
  diag("fsm.election", DIAG_DEBUG, "%s StAwaiting: A Peer found \n",
       OwnerName(e));
  do_PGLE(e, v, 2);
  ChangeState(e, ElectionState::AwaitingFull);
  diag("fsm.election", DIAG_DEBUG, "%s Election State Awaiting Full \n", 
       OwnerName(e));

  if (v->GetType().Is_A(_svcc_vistype) && 
      e->GetNodeType() == ElectionState::LogicalNode) {
    v->Suicide();
    v = 0;
  }
  return true;
}

bool StateAwaiting::InsertNodeID(ElectionState * e, ElectionVisitor * v)
{
  NodeID              * nid = v->GetNodalIGNode();
  ig_nodal_info_group * nig = v->GetNodalIG();

  DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e) 
       << " StAwaitng: A node is inserted " << *nid << endl);

  StoreEntry(e, nid, nig);
  return true;
}

InternalElectionState::ElectionTransitions 
StateAwaiting::GetEventType(ElectionState * e, Visitor * v)
{
  ElectionTransitions rval = Invalid;

  DIAG("fsm.election", DIAG_DEBUG, cout <<
       *(GetLocalNode(e)) << " in " << StateName() << 
       " received a " << v->GetType() << endl);

  if (v->GetType().Is_A(_portup_vistype)) {
    UpdatePorts(e, ((LinkVisitor *)v));
    rval = _PeerFound;
  } else if (v->GetType().Is_A(_svcc_vistype) && 
	     e->GetNodeType() == ElectionState::LogicalNode) {
    UpdatePorts(e, (VPVCVisitor *)v);
    rval = _PeerFound;
  } else if (v->GetType().Is_A(_portdown_vistype)) {
    diag("fsm.election", DIAG_ERROR, "Port Down in Awaiting\n");
    rval = PassThrough;
  } else if (v->GetType().Is_A(_npstate_vistype)) {
    NPStateVisitor::NPStateVisitorType  type = ((NPStateVisitor *)v)->GetVT();
    if (type == NPStateVisitor::FullState)
      rval = _DBReceived; 
    else
      rval = PassThrough;
  } else if (v->GetType().Is_A(_election_vistype)) {
    ElectionVisitor::Purpose reason = ((ElectionVisitor *)v)->GetPurpose();
    if (reason == ElectionVisitor::InsertedNodalInfo)
      rval = _NodalIGInserted; 
    else if (reason == (ElectionVisitor::HorizontalLinkUp) || 
             (reason == (ElectionVisitor::HorizontalLinkDown)))
      rval = _HorzlinkInserted;
    else
      rval = Invalid;
  } else if (rval == Invalid)
    rval = PassThrough;

  return rval;
}

// ---------------------------------------------------------------------------
StateAwaitingFull::StateAwaitingFull(void) : InternalElectionState() { }
StateAwaitingFull::~StateAwaitingFull() { }

const char * StateAwaitingFull::StateName(void) const 
{ return "StateAwaitingFull"; }

bool StateAwaitingFull::LostAllPeers(ElectionState * e, Visitor * v)
{
  do_PGLE(e, v, 3);
  diag("fsm.election", DIAG_DEBUG, 
       "%s StAwaitingFull: Changed to  Awaiting \n", OwnerName(e)); 
  ChangeState(e, ElectionState::Awaiting); 
  diag("fsm.election", DIAG_DEBUG, 
       "Getting back to Awaiting state After Lostallpeers \n");
  return true;
}

bool StateAwaitingFull::DBReceived(ElectionState * e, Visitor *& v)
{
  theNetStatsCollector().ReportNetEvent("Start_Elections",
					e->GetName(),
					0, GetLocalNode(e) );

  do_PGLE(e, v, 5);
  diag("fsm.election", DIAG_DEBUG,
       "%s StAwaitingFull: Changing to InitialDelays \n", OwnerName(e));
  ChangeState(e, ElectionState::InitialDelay);

  if (e->GetNodeType() == ElectionState::LogicalNode) {
    v->Suicide(); v = 0;
  }
  return true;
}

bool StateAwaitingFull::InsertNodeID(ElectionState * e, ElectionVisitor * v)
{
  NodeID              * nid = v->GetNodalIGNode();
  ig_nodal_info_group * nig = v->GetNodalIG();

  DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e) 
       << " StAwaitFull: A node entered is " << *nid << endl);

  StoreEntry(e, nid, nig);
  return true;
}

// ---------------------------------------------------------------------------
StateInitialDelay::StateInitialDelay(void) : InternalElectionState() { }
StateInitialDelay::~StateInitialDelay() { }

const char * StateInitialDelay::StateName(void) const 
{ return "StateInitialDelay"; }

bool StateInitialDelay::PGLInitTimerExpiry(ElectionState * e)
{
  diag("fsm.election", DIAG_DEBUG, "%s InitalDelay: PGL Init expired \n", 
       OwnerName(e));
  DetermineConnToNode(e, all);
  ChangePreferredPGL(e);
  return true;
}

bool StateInitialDelay::InsertNodeID(ElectionState * e, ElectionVisitor *v)
{
  NodeID              * nid = v->GetNodalIGNode();
  ig_nodal_info_group * nig = v->GetNodalIG();

  DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e) 
       << " InitalDelay: node inserted\n " << *nid << endl);

  StoreEntry(e, nid, nig);
  return true;
}

// ---------------------------------------------------------------------------
StateCalculating::StateCalculating(void) : InternalElectionState() { }
StateCalculating::~StateCalculating() { }

const char * StateCalculating::StateName(void) const 
{ return "StateCalculating"; }

// ---------------------------------------------------------------------------
StateOperNotPGL::StateOperNotPGL(void) : InternalElectionState() { }
StateOperNotPGL::~StateOperNotPGL() { }

const char * StateOperNotPGL::StateName(void) const 
{ return "StateOperNotPGL"; }

bool StateOperNotPGL::LoseConnToPGL(ElectionState * e)
{ 
  DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e)
       << " NotPGL: Conn Lost to PGL " << *GetOldPGL(e) << endl;);
  do_PGLE(e, 0, 10);
  ChangeState(e, ElectionState::AwaitReelection);
  return true;
}

bool StateOperNotPGL::InsertNodeID(ElectionState * e, ElectionVisitor * v)
{
  NodeID              * nid = v->GetNodalIGNode();
  ig_nodal_info_group * nig = v->GetNodalIG();
  Nodalinfo           * nif = GetEntry(e, nid);

  DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e) 
       << " NOTPGL: Inserting NodeID " << *nid << endl);

  if (nig->IsSet(ig_nodal_info_group::leader_bit)) {
    // If the Visitor's node is the leader
    if (!(nid->equals(GetPreferredPGL(e)))) {
      // and if our preferred PGL doesn't match the leader notify us
      /*
      diag("fsm.election", DIAG_WARNING, "%s received a(n) %s\n"
	   "\twith PGL %s\n"
	   "\tour current PGL is %s, recalculating ...\n",
	   e->GetLocalNID()->Print(), v->GetType().Name(), 
	   nid->Print(), GetPreferredPGL(e)->Print());
	   */
    }  
  }
  
  if (nid->equals(GetPreferredPGL(e))) {  
    StoreEntry(e, nid, nig);
    if (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit) || 
        !nig->GetLeadershipPriority()) { 
      DetermineConnToNode(e, ElectionNontransit); 
      ChangePreferredPGL(e);
    } else if (GetPrefPGLPrio(e) < nig->GetLeadershipPriority()) { 
      int pprio = -1;
      SetPreferredPGL( e, EvalPreferredPGL(e, pprio) );
      SetPrefPrio( e, pprio );
    } else if (GetPrefPGLPrio(e) > nig->GetLeadershipPriority()) {
      int pprio = -1;
      NodeID * pref = EvalPreferredPGL( e, pprio );
      if (! pref->equals( GetPreferredPGL(e) )) {
	SetPreferredPGL( e, pref );
	SetPrefPrio( e, pprio );
        ChangePreferredPGL(e);
      } else {
	SetPreferredPGL( e, pref );
	SetPrefPrio( e, pprio );
      }
    }
    return true;
  }
  
  if (nif) {  // Existing node
    if (nif->GetElectionFlag()) {
      if (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)) {
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, ElectionNontransit);
        const Nodalinfo * niff = GetNodalInfo(e, GetPreferredPGL(e));

        if (!niff->GetElectionFlag())
          LoseConnToPGL(e); 
      } else if (nig->GetLeadershipPriority() > GetPrefPGLPrio(e)) {
        StoreEntry(e, nid, nig);
        ChangePreferredPGL(e);
      } else {
        StoreEntry(e, nid, nig);
      }
    } else {  // Election Non fit
      if (!nif->GetElectionTransit() &&
          (!nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))) {
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, Electiontransit);
	int pprio = -1;
	NodeID * pref = EvalPreferredPGL(e, pprio);
	if (! pref->equals( GetPreferredPGL(e) )) {
	  SetPreferredPGL( e, pref );
	  SetPrefPrio( e, pprio );
          ChangePreferredPGL(e);
        } else {
	  SetPreferredPGL( e, pref );
	  SetPrefPrio( e, pprio );
	}
      } else if (nif->GetElectionTransit() &&
                 (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))) {
        // A node decalred non transit for election now
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, ElectionNontransit);
        const Nodalinfo * niff = GetNodalInfo(e, GetPreferredPGL(e));

        if (!niff->GetElectionFlag()) {
          LoseConnToPGL(e);
        }
      } else if (!nif->GetPriority() &&
                 nig->GetLeadershipPriority() &&
                 !nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)) {
        StoreEntry(e, nid, nig);
        SetEntry(e, nid);
        if (nig->GetLeadershipPriority() > GetPrefPGLPrio(e)) {
          // which was zero before
          ChangePreferredPGL(e);
        }
      } else
        StoreEntry(e, nid, nig);
    }
    return true;
  } else {
    // New Entry
    StoreEntry(e, nid, nig);
    return true;  
  }
  return false;
}

bool StateOperNotPGL::InsertHorzID(ElectionState * e, Visitor * v)
{
  ElectionVisitor * vis = (ElectionVisitor *)v;

  ElectionVisitor::Purpose reason = vis->GetPurpose();
  if (reason == ElectionVisitor::HorizontalLinkUp) {
    DetermineConnToNode(e, HlinkUp); 

    int pprio = -1;
    NodeID * pref = EvalPreferredPGL(e, pprio);
    if (! pref->equals( GetPreferredPGL(e) )) {
      SetPreferredPGL( e, pref );
      SetPrefPrio( e, pprio );
      ChangePreferredPGL(e);
    } else {
      SetPreferredPGL( e, pref );
      SetPrefPrio( e, pprio );
    }
  } else if (reason == ElectionVisitor::HorizontalLinkDown) {
    DetermineConnToNode(e, HlinkDown);
    const Nodalinfo * niff = GetNodalInfo(e, GetPreferredPGL(e));

    if (!niff->GetElectionFlag())
      LoseConnToPGL(e);
  }
  return true;
}

// ---------------------------------------------------------------------------
StateOperPGL::StateOperPGL(void) : InternalElectionState() { }
StateOperPGL::~StateOperPGL() { }

const char * StateOperPGL::StateName(void) const { return "StateOperPGL"; }

bool StateOperPGL::ChangePreferredPGL(ElectionState * e)
{
  do_PGLE(e, 0, 6); 
  return true;
}

bool StateOperPGL::InsertNodeID(ElectionState * e, ElectionVisitor *v)
{
  NodeID              * nid = v->GetNodalIGNode();
  ig_nodal_info_group * nig = v->GetNodalIG();
  Nodalinfo           * nif = GetEntry(e, nid);

  DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e) 
       << " PGL: Inserting nodeid" << *nid << endl);

  if (nig->IsSet(ig_nodal_info_group::leader_bit)) {
    if (!(*nid == *GetPreferredPGL(e))) {
      DIAG("fsm.election", DIAG_ERROR, cout 
           << "Our preffered PGL differes from this :"<< *nid << endl;);
    }  
  }
  
  if (*nid == *GetLocalNode(e)) { // Lets use Loc Node Id here
    StoreEntry(e, nid, nig);
    if (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit) || 
        !nig->GetLeadershipPriority()) { 
      ChangePreferredPGL(e);
    } else if (GetPrefPGLPrio(e) < nig->GetLeadershipPriority()) {
      int pprio = -1;
      SetPreferredPGL( e, EvalPreferredPGL(e, pprio) );
      SetPrefPrio( e, pprio );
    } else if ( GetPrefPGLPrio(e) >  nig->GetLeadershipPriority()) {
      int pprio = -1;
      NodeID * pref = EvalPreferredPGL(e, pprio);
      if (! pref->equals( GetPreferredPGL(e) )) {
	SetPreferredPGL( e, pref );
	SetPrefPrio( e, pprio );
        ChangePreferredPGL(e);
      } else {
	SetPreferredPGL( e, pref );
	SetPrefPrio( e, pprio );
      }
    }
    return true;
  }
  
  if (nif) {  // Existing node
    if (nif->GetElectionFlag()) {
      if (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)) {
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, ElectionNontransit);
      } else if (nig->GetLeadershipPriority() > GetPrefPGLPrio(e)) {
        StoreEntry(e, nid, nig);
        ChangePreferredPGL(e);
      } else
        StoreEntry(e, nid, nig);
    } else {  // Election Non fit
      if (!nif->GetElectionTransit() &&
          (!nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))) {
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, Electiontransit);
	int pprio = -1;
	NodeID * pref = EvalPreferredPGL(e, pprio);
	if (! pref->equals( GetPreferredPGL(e))) {  // We got a change in leadership
	  SetPreferredPGL( e, pref );
	  SetPrefPrio( e, pprio );
          ChangePreferredPGL(e);
        } else {
	  SetPreferredPGL( e, pref );
	  SetPrefPrio( e, pprio );
	}
      } else if (nif->GetElectionTransit() &&
                 (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))) {
        // A node decalred non transit for election now
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, ElectionNontransit);
      } else if (!nif->GetPriority() &&
                 nig->GetLeadershipPriority() &&
                 !nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)) {
        StoreEntry(e, nid, nig);
        SetEntry(e, nid);
        if (nig->GetLeadershipPriority() > GetPrefPGLPrio(e)) {
          // which was zero before
          ChangePreferredPGL(e);
        }
      } else
        StoreEntry(e, nid, nig);
    }
    return true;
  } else {
    // New Entry
    StoreEntry(e, nid, nig);
    return true;  
  }
  return false;
}

bool StateOperPGL::InsertHorzID(ElectionState * e, Visitor * v)
{
  ElectionVisitor * vis = (ElectionVisitor *)v;

  ElectionVisitor::Purpose reason = vis->GetPurpose();

  if (reason == ElectionVisitor::HorizontalLinkUp) {   
    DetermineConnToNode(e, HlinkUp);
    int pprio = -1;
    SetPreferredPGL( e, EvalPreferredPGL(e, pprio) );
    SetPrefPrio( e, pprio );
    if (!PreferMyself(e))
      ChangePreferredPGL(e);
  } else if (reason == ElectionVisitor::HorizontalLinkDown)
    DetermineConnToNode(e, HlinkDown);
  return true;
}

// ---------------------------------------------------------------------------
StateAwaitUnanimity::StateAwaitUnanimity(void) : InternalElectionState() { }
StateAwaitUnanimity::~StateAwaitUnanimity() { }

const char * StateAwaitUnanimity::StateName(void) const 
{ return "StateAwaitUnanimity"; }

bool StateAwaitUnanimity::OverrideUnanimityExpiry(ElectionState * e)
{
  diag("fsm.election", DIAG_DEBUG, 
       "%s STAwUnanimity: OverrideUnanimity timer expired \n", OwnerName(e));

  Election(e);
  if (TwoThirdReached(e)) {
    // Its Just below this function
    OverrideUnanimitySucc(e);
    diag("fsm.election", DIAG_DEBUG, 
         "%s STAwUnanimity: OverrideUnanimitySucc \n", OwnerName(e));
  } else {
    OverrideUnanimityFail(e);
    diag("fsm.election", DIAG_DEBUG, 
         "%s STAwUnanimity: OverrideUnanimityfailed \n", OwnerName(e));
    // Re-register the unanimity timer - Added 4/9/98 mountcas
    RegisterTimer(e, ElectionState::OverrideUnanimityTIMER);
  }
  return true;
}

bool StateAwaitUnanimity::OverrideUnanimitySucc(ElectionState * e)
{
  do_PGLE(e, 0,8);
  diag("fsm.election", DIAG_DEBUG, 
       "%s STAwUnanimity: State Changing to OperPGL\n", OwnerName(e));
  ChangeState(e, ElectionState::OperPGL);
  return true;
}

bool StateAwaitUnanimity::OverrideUnanimityFail(ElectionState * e)
{
  diag("fsm.election", DIAG_DEBUG, 
       "%s STAwUnanimity: State Changing to HungElection \n", OwnerName(e));
  ChangeState(e, ElectionState::HungElection);
  return true;
}

//
// See how many times we have to delete pref in this method?  What a
// mess!  This method is a poster child for the rule "no more than one
// return in any method".
//
bool StateAwaitUnanimity::InsertNodeID(ElectionState * e, ElectionVisitor * v)
{
  NodeID              * nid  = v->GetNodalIGNode();
  ig_nodal_info_group * nig  = v->GetNodalIG();

  // DWT 981019 -- GetPreferredPGL() returns a copy that we must
  // delete if we wish to avoid a memory leak.  Why it returns a const
  // NodeID *, I'll never understand.
  NodeID              * pref = (NodeID *)nig->GetPreferredPGL();  

  DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e) 
       << " STAwUnanimity: Node inserted " << *nid << endl);

  Nodalinfo * nif = GetEntry(e, nid);

  if (*(nid) == *(GetLocalNode(e))) { 
    StoreEntry(e, nid, nig);
    if (GetPrefPrio(e) > nig->GetLeadershipPriority()) {
      int pprio = -1;
      NodeID * pref2 = EvalPreferredPGL(e, pprio);
      if (! pref2->equals( GetPreferredPGL(e) )) {
	SetPreferredPGL( e, pref2 );
	SetPrefPrio( e, pprio );
        ChangePreferredPGL(e);
      } else {
	SetPreferredPGL( e, pref2 );
	SetPrefPrio( e, pprio );
      }
    } else if (GetPrefPGLPrio(e) < nig->GetLeadershipPriority()) {
      int pprio = -1;
      SetPreferredPGL( e, EvalPreferredPGL(e, pprio) );
      SetPrefPrio( e, pprio );
    } else if ((nig->GetLeadershipPriority() == 0) || 
             nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))
      ChangePreferredPGL(e);

    delete pref;
    return true;
  } 

  if (nif) {  // An existing Entry 
    if (nif->GetElectionFlag()) { // we are election fit
      if (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)) {
        // We got a non transit election from this node
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, ElectionNontransit);
        Election(e);
        Unanimity(e);
      } else if (nig->GetLeadershipPriority() == 0) {
        //  We got a leadrship  priority zero for the node 
        StoreEntry(e, nid, nig);
        Election(e);
        Unanimity(e);
      } else if (nig->GetLeadershipPriority() > GetPrefPGLPrio(e)) {
        StoreEntry(e, nid, nig);
        ChangePreferredPGL(e);
      } else if (pref && (*GetPreferredPGL(e) == *pref) && 
		 !(*(nif->GetPrefPGL()) == *pref)) {
        // The nodes preferredpgl changed to us.
        StoreEntry(e, nid, nig); 
        Election(e);
        Unanimity(e);
      } else
        StoreEntry(e, nid, nig);
    } else {  // Election Non fit
      if (!nif->GetElectionTransit() && 
          (!nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))) { 
        // We got a TE set.
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, Electiontransit);
	int pprio = -1;
	NodeID * pref = EvalPreferredPGL(e, pprio);
	if(! pref->equals(GetPreferredPGL(e))) { // We got a change in leadership
	  SetPreferredPGL( e, pref );
	  SetPrefPrio( e, pprio );
          ChangePreferredPGL(e);
	} else {
	  SetPreferredPGL( e, pref );
	  SetPrefPrio( e, pprio );
	}
      } else if (nif->GetElectionTransit() &&
                 (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))) {
        // A node decalred non transit for election now
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, ElectionNontransit);
        Election(e);
        Unanimity(e);
      } else if (!nif->GetPriority() &&
                 nig->GetLeadershipPriority() && 
                 !nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)) { 
        StoreEntry(e, nid, nig);
        SetEntry(e, nid);
        // We need not know if have the connectivity 
        // or not what the heck both priority and
        // and  electiontransit w/o connectivity how can it come here?
        if ((nig->GetLeadershipPriority() > GetPrefPGLPrio(e))) {  
          // We have a priority declared for a node which
          // which was zero before
          ChangePreferredPGL(e);
        }
      } else // May not be required at all
        StoreEntry(e, nid, nig);
    }
    delete pref;
    return true;
  } else {
    // New Entry 
    StoreEntry(e, nid, nig);
    delete pref;
    return true;
  }

  delete pref;
  return false;
}

bool StateAwaitUnanimity::InsertHorzID(ElectionState * e, Visitor * v)
{
  ElectionVisitor * vis = (ElectionVisitor *)v;

  ElectionVisitor::Purpose reason = vis->GetPurpose();

  if (reason == ElectionVisitor::HorizontalLinkUp) {   
    diag("fsm.election", DIAG_DEBUG, "%s AwUnanimity: hlinkup inserted \n", 
         OwnerName(e));
    DetermineConnToNode(e, HlinkUp);

    int pprio = -1;
    NodeID * pref = EvalPreferredPGL(e, pprio);
    if (! pref->equals(GetPreferredPGL(e))) {
      SetPreferredPGL( e, pref );
      SetPrefPrio( e, pprio );
      ChangePreferredPGL(e);
    } else {
      SetPreferredPGL( e, pref );
      SetPrefPrio( e, pprio );
    }
  } else if (reason == ElectionVisitor::HorizontalLinkDown) {
    diag("fsm.election", DIAG_DEBUG, "%s AwUnanimity: hlinkdown inserted \n", 
         OwnerName(e));
    DetermineConnToNode(e, HlinkDown);
    Election(e);
    Unanimity(e);
  }
  return true;
}

// ---------------------------------------------------------------------------
StateHungElection::StateHungElection(void) : InternalElectionState() { }
StateHungElection::~StateHungElection() { }

const char * StateHungElection::StateName(void) const 
{ return "StateHungElection"; }

bool StateHungElection::InsertNodeID(ElectionState * e, ElectionVisitor * v)
{
  NodeID              * nid = v->GetNodalIGNode();
  ig_nodal_info_group * nig = v->GetNodalIG();
  Nodalinfo           * nif = GetEntry(e, nid);
  
  DIAG("fsm.election", DIAG_DEBUG,  cout << OwnerName(e)
       << " Hung: node inserted " << *nid << endl;);  

  if (*(nid) == *(GetLocalNode(e))) { 
    StoreEntry(e, nid, nig);
    if (GetPrefPGLPrio(e) > nig->GetLeadershipPriority()) {
      int pprio = -1;
      NodeID * pref = EvalPreferredPGL(e, pprio);
      if (! pref->equals(GetPreferredPGL(e))) {
	SetPreferredPGL( e, pref );
	SetPrefPrio( e, pprio );
        ChangePreferredPGL(e);
      } else {
	SetPreferredPGL( e, pref );
	SetPrefPrio( e, pprio );
      }
    } else if ( GetPrefPGLPrio(e) < nig->GetLeadershipPriority()) {
      int pprio = -1;
      SetPreferredPGL( e, EvalPreferredPGL(e, pprio) );
      SetPrefPrio( e, pprio );
    } else if (nig->GetLeadershipPriority() == 0 || 
             nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))
      ChangePreferredPGL(e);

    return true;
  } 

  if (nif) {  // An existing Entry 
    if (nif->GetElectionFlag()) { // we are election fit
      if (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)) {
        // We got a non transit election from this node
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, ElectionNontransit);
        Election(e);
        if (TwoThirdReached(e)) {
          do_PGLE(e, 0,8);
          diag("fsm.election", DIAG_DEBUG, 
	       "%s Hung: State Changing to OperPGL %n", 
               OwnerName(e));
          ChangeState(e, ElectionState::OperPGL);
        } 
      } else if (nig->GetLeadershipPriority() == 0) {
        //  We got a leadrship  priority zero for the node 
        StoreEntry(e, nid, nig);
        Election(e);
        if (TwoThirdReached(e)) {
          do_PGLE(e, 0,8);
          diag("fsm.election", DIAG_DEBUG, 
	       "%s Hung: State Changing to OperPGL\n", 
               OwnerName(e));
          ChangeState(e, ElectionState::OperPGL);
        }
      } else if (nig->GetLeadershipPriority() > GetPrefPGLPrio(e)) {
        StoreEntry(e, nid, nig);
        ChangePreferredPGL(e);
      } else if (nig->GetPreferredPGL() && 
                 (*(GetPreferredPGL(e)) == *(nig->GetPreferredPGL()))
                 && !(*(nif->GetPrefPGL()) == *(nig->GetPreferredPGL()))) {
        // The nodes preferredpgl changed to us.
        StoreEntry(e, nid, nig); 
        Election(e);
        if (TwoThirdReached(e)) {
          do_PGLE(e, 0,8);
          diag("fsm.election", DIAG_DEBUG, "%s Hung: State is PGL \n",
               OwnerName(e));
          ChangeState(e, ElectionState::OperPGL);
        }
      } else
        StoreEntry(e, nid, nig);
    } else {  // Election Non fit
      if (!nif->GetElectionTransit() && 
          (!nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))) { 
        // We got a TE set.
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, Electiontransit);
	int pprio = -1;
	NodeID * pref = EvalPreferredPGL(e, pprio);
	if (pref != 0) {
	  if (! pref->equals(GetPreferredPGL(e))) { // We got a change in leadership
	    SetPreferredPGL( e, pref );
	    SetPrefPrio( e, pprio );
	    ChangePreferredPGL(e);
	  } else {
	    SetPreferredPGL( e, pref );
	    SetPrefPrio( e, pprio );
	  }
	}
        else {
          Election(e);
          if (TwoThirdReached(e)) {
            do_PGLE(e, 0,8);
            diag("fsm.election", DIAG_DEBUG, 
		 "%s Hung: State changing to  PGL \n",
                 OwnerName(e));
            ChangeState(e, ElectionState::OperPGL);
          } 
        }
      } else if (nif->GetElectionTransit() &&
                 (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))) {
        // A node decalred non transit for election now
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, ElectionNontransit);
        Election(e);
        if (TwoThirdReached(e)) {
          do_PGLE(e, 0,8);
          diag("fsm.election", DIAG_DEBUG, 
	       "%s Hung: State changing to PGL \n", 
               OwnerName(e));
          ChangeState(e, ElectionState::OperPGL);
          return true;
        }
      } else if (!nif->GetPriority() &&
                 nig->GetLeadershipPriority() && 
                 !nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)) { 
        StoreEntry(e, nid, nig);
        SetEntry(e, nid);
        if (nig->GetLeadershipPriority() > GetPrefPGLPrio(e)) { 
          // We have a priority declared for a node which
          // which was zero before
          ChangePreferredPGL(e);
        } else if (*(nig->GetPreferredPGL()) == *GetLocalNode(e)) {
          Election(e);
          if (TwoThirdReached(e)) {
            do_PGLE(e, 0,8);
            diag("fsm.election", DIAG_DEBUG,
                 "%s Hung: State Changing to  PGL \n", OwnerName(e));
            ChangeState(e, ElectionState::OperPGL);
          }
        }
      } else
        StoreEntry(e, nid, nig);
    }
    return true;
  } else {
    // A new node connectivity to be confirmed
    StoreEntry(e, nid, nig);
    return true;
  }
  return false;
}
 
bool StateHungElection::InsertHorzID(ElectionState * e, Visitor * v)
{
  ElectionVisitor * vis = (ElectionVisitor *)v;
  ElectionVisitor::Purpose reason = vis->GetPurpose();

  if (reason == ElectionVisitor::HorizontalLinkUp) {   
    diag("fsm.election", DIAG_DEBUG, "%s Hung: hlinkup inserted \n", 
         OwnerName(e));
    DetermineConnToNode(e,HlinkUp);
    int pprio = -1;
    NodeID * pref = EvalPreferredPGL(e, pprio);
    if (pref != 0) {
      if (! pref->equals( GetPreferredPGL(e) )) {
	SetPreferredPGL( e, pref );
	SetPrefPrio( e, pprio );
	ChangePreferredPGL(e);
      } else {
	SetPreferredPGL( e, pref );
	SetPrefPrio( e, pprio );
      }
    } else {
      Election(e);
      if (TwoThirdReached(e)) {
        do_PGLE(e, 0,8);
        diag("fsm.election", DIAG_DEBUG, 
	     "%s Hung: State Changing to OperPGL \n", 
             OwnerName(e));
        ChangeState(e, ElectionState::OperPGL);
      }
    }
  } else if (reason == ElectionVisitor::HorizontalLinkDown) {
    diag("fsm.election", DIAG_DEBUG, "%s Hung: hlinkdown inserted \n", 
         OwnerName(e));
    DetermineConnToNode(e, HlinkDown);
    Election(e);
    if (TwoThirdReached(e)) {
      do_PGLE(e, 0,8);
      diag("fsm.election", DIAG_DEBUG, "%s ElectionState is PGL \n", 
           OwnerName(e));
      ChangeState(e, ElectionState::OperPGL);
    }
  }
  return true;
}

// ---------------------------------------------------------------------------
StateAwaitReelection::StateAwaitReelection(void) : InternalElectionState() { }
StateAwaitReelection::~StateAwaitReelection() { }

const char * StateAwaitReelection::StateName(void) const 
{ return "StateAwaitReelection"; }

bool StateAwaitReelection::ReelectionTimerExpiry(ElectionState * e)
{
  RemoveEntry(e, (NodeID *)GetPreferredPGL(e)); // I think it would already
                                   // have GetElectionFlag reset
  diag("fsm.election", DIAG_DEBUG, "%s Reelection Timer expired \n", 
       OwnerName(e));
  SetPrefPGLPrio(e,0);
  ChangePreferredPGL(e); // Does the same thing
  return true;
}

bool StateAwaitReelection::GetConnToPGL(ElectionState * e)
{
  DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e) 
       << " Reelection: PGL connection obtained" << *GetOldPGL(e) << endl;);
  do_PGLE(e, 0, 11);
  diag("fsm.election", DIAG_DEBUG, 
       "%s Reelection: State changing to NOTPGL \n",
       OwnerName(e));
  ChangeState(e, ElectionState::OperNotPGL);
  return true;
}

bool StateAwaitReelection::InsertNodeID(ElectionState * e, ElectionVisitor * v)
{
  NodeID              * nid = v->GetNodalIGNode();
  ig_nodal_info_group * nig = v->GetNodalIG();
  Nodalinfo           * nif = GetEntry(e, nid);

  DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName(e) 
       << " Reelection: Node Inserted " << *nid << endl);

  if (nig->IsSet(ig_nodal_info_group::leader_bit)) {
    if (!(*nid == *GetPreferredPGL(e))) {
      DIAG("fsm.election", DIAG_ERROR, 
           cout << "Our preffered PGL differes from this: " << *nid << endl;);
    }  
  }
  
  if (*nid == *GetOldPGL(e)) {  
    // An existing Entry can happen if links are announced later 
    // and the id's come first let us get the connectivity then.
    StoreEntry(e, nid, nig);
    if (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit) || 
        !nig->GetLeadershipPriority()) { 
      DetermineConnToNode(e, ElectionNontransit);
      ChangePreferredPGL(e);
    } else if (GetOldPGLPrio(e) < nig->GetLeadershipPriority()) { 
      SetEntry(e, GetOldPGL(e));
      int pprio = -1;
      SetPreferredPGL( e, EvalPreferredPGL(e, pprio) );
      SetPrefPrio( e, pprio );
      GetConnToPGL(e);
    } else if (GetOldPGLPrio(e) > nig->GetLeadershipPriority()) {
      SetEntry(e, GetOldPGL(e));
      int pprio = -1;
      NodeID * pref = EvalPreferredPGL(e, pprio);
      if (! pref->equals(GetPreferredPGL(e))) {
	SetPreferredPGL( e, pref );
	SetPrefPrio( e, pprio );
        ChangePreferredPGL(e);
      } else {
	SetPreferredPGL( e, pref );
	SetPrefPrio( e, pprio );
      }
      GetConnToPGL(e);
    }
    return true;
  }
  
  if (nif) {  // Existing node
    if (nif->GetElectionFlag()) {
      if (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)) {
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, ElectionNontransit);
      } else if (nig->GetLeadershipPriority() > GetOldPGLPrio(e)) {
        StoreEntry(e, nid, nig);
        ChangePreferredPGL(e);
      } else {
        StoreEntry(e, nid, nig);
      }
    } else {  // Election Non fit
      if (!nif->GetElectionTransit() &&
          (!nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))) {
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, Electiontransit);
	int pprio = -1;
        SetPreferredPGL( e, EvalPreferredPGL(e, pprio) );
	SetPrefPrio( e, pprio );
        if (!(*GetPreferredPGL(e) == *GetOldPGL(e))) {
          Nodalinfo * oldpgl = GetEntry(e, GetOldPGL(e));
          Nodalinfo * newpgl = GetEntry(e, GetPreferredPGL(e));
          if (newpgl->GetPriority() > oldpgl->GetPriority()) {
            ChangePreferredPGL(e);
          }
        } else if (*GetPreferredPGL(e) == *GetOldPGL(e)) {
          GetConnToPGL(e);
        }
      } else if (nif->GetElectionTransit() &&
                 (nig->IsSet(ig_nodal_info_group::ntrans_ele_bit))) {
        // A node decalred non transit for election now
        StoreEntry(e, nid, nig);
        DetermineConnToNode(e, ElectionNontransit);
      } else if (!nif->GetPriority() &&
                 nig->GetLeadershipPriority() &&
                 !nig->IsSet(ig_nodal_info_group::ntrans_ele_bit)) {
        StoreEntry(e, nid, nig);
        SetEntry(e, nid);
        if (nig->GetLeadershipPriority() > GetOldPGLPrio(e)) {
          // which was zero before
          ChangePreferredPGL(e);
        }
      } else
        StoreEntry(e, nid, nig);
    }
    return true;
  } else {
    // New Entry
    StoreEntry(e, nid, nig);
    return true;  
  }
  return false;
}

bool StateAwaitReelection::InsertHorzID(ElectionState * e, Visitor * v)
{
  ElectionVisitor * vis = (ElectionVisitor *)v;
  ElectionVisitor::Purpose reason = vis->GetPurpose();

  if (reason == ElectionVisitor::HorizontalLinkUp) {   
    diag("fsm.election", DIAG_DEBUG, 
         "%s Reelection: HorizontalLinkup inserted \n", OwnerName(e));
    DetermineConnToNode(e, HlinkUp);
    int pprio = -1;
    SetPreferredPGL( e, EvalPreferredPGL(e, pprio) );
    SetPrefPrio( e, pprio );
    if (!(*GetPreferredPGL(e) == *GetOldPGL(e))) {
      Nodalinfo * oldpgl = GetEntry(e, GetOldPGL(e));
      Nodalinfo * newpgl = GetEntry(e, GetPreferredPGL(e));
      if (newpgl->GetPriority() > oldpgl->GetPriority())
        ChangePreferredPGL(e);
    } else if (*GetPreferredPGL(e) == *GetOldPGL(e))
      GetConnToPGL(e);
  } else if (reason == ElectionVisitor::HorizontalLinkDown) {
    diag("fsm.election", DIAG_DEBUG, 
         "%s  Reelection: HorizontalLinkdown inserted \n", OwnerName(e));
    DetermineConnToNode(e, HlinkDown);
  }
  return true;
}
