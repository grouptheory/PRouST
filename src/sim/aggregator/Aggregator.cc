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
static char const _Aggregator_cc_rcsid_[] =
"$Id: Aggregator.cc,v 1.4 1999/02/25 14:52:33 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "Aggregator.h"
#include "AggregatorInterfaces.h"
#include "LogicalPeer.h"
#include "AggregationPolicy.h"
#include "LogosGraphVisitor.h"
#include <fsm/database/Database.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/NPFloodVisitor.h>
#include <fsm/hello/HelloVisitor.h>
#include <FW/basics/diag.h>
#include <FW/kernel/SimEvent.h>
#include <FW/interface/Interface.h>
#include <codec/pnni_pkt/ptsp.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/cause.h>

AggregationPolicy * AllocateAggregationPolicy(const char * name);

const VisitorType * Aggregator::_npflood_type = 0;
const VisitorType * Aggregator::_fastuni_type = 0;
const VisitorType * Aggregator::_horlink_type = 0;
const VisitorType * Aggregator::_logosgraph_type = 0;

/*

Basic functions:

WHEN an SVC setup arrives from our Database, as would happen if we see
an uplink IG generated within our peergroup where we are leader and
our nodeID is lower than that of the remote upnode.  THEN 

WHEN an LGNHelloInstantiator arrives from our Database, as would
happen if we see an uplink IG generated within our peergroup where we
are leader and our nodeID is higher than that of the remote upnode.
THEN

WHEN an NPFlood arrives from our Database, for an uplink IG generated
within our peergroup where we are leader.  THEN we must re-aggregate
the link, as its raigs may have changed.  If the link is a null-body PTSE:

WHEN an Hlink comes up (LGNHello reaches 2way), THEN we must
reaggregate the corresponding LogicalLink and perhaps (if this is the
first LGNhello for a particular LogicalPeer) update the number of
ports in the ComplexRep by adding the new port

WHEN an Hlink goes down (LGNHello leaves 2way), THEN we must
reaggregate the corresponding LogicalLink and update the number of
ports in the ComplexRep by removing the failed port

*/


//-------------------------------------------------------------------------
Aggregator::Aggregator(const NodeID & myNode, const PeerID & myPeer, 
		       const char * agg_pol ) : _me(myNode.copy())
{
  _nextid = LOGICAL_LINK_BASE_ID;

  _aggr = AllocateAggregationPolicy(agg_pol);
  // Tell the PlugIn who's boss and where we are in the network.
  _aggr->SetMaster(this);
  _aggr->SetIdentity(myNode, myPeer);
  _aggr->SetDBLifetime( (int)(Database::Default_PTSERefreshInterval * 
			      Database::Default_PTSELifetimeFactor) );
  _aggr->SetDBRefresh( Database::Default_PTSERefreshInterval );

  // Set up Visitor sensitivities
  if (_fastuni_type == 0)
    _fastuni_type = QueryRegistry(FAST_UNI_VISITOR_NAME);
  if (_npflood_type == 0)
    _npflood_type = QueryRegistry(NPFLOOD_VISITOR_NAME);
  if (_horlink_type == 0)
    _horlink_type = QueryRegistry(HLINK_VISITOR_NAME);
  if (_logosgraph_type == 0)
    _logosgraph_type = QueryRegistry(LOGOS_GRAPH_VISITOR_NAME);

  assert(_fastuni_type && _npflood_type && _horlink_type);

  _qse = new SimEvent(this, this, QUEUE_SERVICE_SIM_EVENT_CODE);

  AddPermission("*",          new AggregatorInterface(this));
  AddPermission("Leadership", new AggLeadershipInterface(this));
}

//-------------------------------------------------------------------------
Aggregator::~Aggregator() 
{ 
  dic_item di;                        // flush peer data structures
  forall_items(di, _peers) {
    // Delete the NodeID
    delete (NodeID *)_peers.key(di);
    // Delete the Logical Peer
    delete _peers.inf(di);
  }
  _peers.clear();

  list_item li;                       // flush SVC records
  forall_items(li, _svcs) {
    delete (NodeID *)_svcs.inf(li);
  }
  _svcs.clear();
  delete _me;

  forall_items(di, _lhi_map) {        // flush pending SVC records
    delete _lhi_map.key(di);
    ds_queue<FastUNIVisitor *> * qptr = _lhi_map.inf(di);
    
    while (!(qptr->empty()))
      qptr->pop()->Suicide();

    delete qptr;
  }

  delete _qse;
}

//-------------------------------------------------------------------------
State * Aggregator::Handle(Visitor * v)
{  
  bool safe2passthru = true;
  VisitorType vt = v->GetType();

  // dispatch to helper function

  switch (VisitorFrom(v)) {             
    case Visitor::A_SIDE: // Outside the Switch
      if (vt.Is_A(_horlink_type))    
	{ safe2passthru = HandleHLink((HorLinkVisitor*)v); }
      else if (vt.Is_A(_fastuni_type))
	{ safe2passthru = HandleSVCFromOutside((FastUNIVisitor *)v); }
      else if (vt.Is_A(_logosgraph_type))
	{ safe2passthru = HandleLogosGraphFromOutside((LogosGraphVisitor *)v); }
      break;
    case Visitor::B_SIDE: // Inside the Switch
      if (vt.Is_A(_npflood_type))    
	{ safe2passthru = HandleNPFlood((NPFloodVisitor*)v); }
      else if (vt.Is_A(_fastuni_type))
	{ safe2passthru = HandleSVCFromInside((FastUNIVisitor *)v); }
      break;
    default:
      break;
  }
  if (safe2passthru)
    PassThru(v);   // Pass it on
  return this;
}

//-------------------------------------------------------------------------
// Floods coming out of our Database
bool Aggregator::HandleNPFlood(NPFloodVisitor * v)
{
  const PTSPPkt * ptsp = v->GetFloodPTSP();

  // If I am nobody to the originator of this PTSE, 
  // then I have no responsibilities...
  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  int level = *(ptsp->GetOID());
  bool IAmLeader = dbi->AmILeader( level );
  dbi->Unreference();

  if ( ! IAmLeader )
    return true;

  // Process all the PTSEs within the PTSP
  const list<ig_ptse *> lp = ptsp->GetElements();

  list_item li;
  forall_items(li, lp) {
    ig_ptse * ptse = lp.inf(li);
    if (ptse && ptse->GetType() == InfoGroup::ig_uplinks_id) {
      
      if ((ptse->ShareMembers() != 0) &&
	  (ptse->ShareMembers()->size() != 0)) {
	// We have a PTSE containing FULL-BODIED uplinks ...

	int id_of_new_ptse = ptse->GetID();

	const list<InfoGroup *> * lig = ptse->ShareMembers();
	assert(lig);

	list_item ili;
	forall_items(ili, *lig) {
	  if (lig->inf(ili)->GetId() == InfoGroup::ig_uplinks_id) {
	    ig_uplinks * ulig = (ig_uplinks *)(lig->inf(ili));
	    NodeID * nid = ulig->GetRemoteID();
	    dic_item di;

	    if (!(di = _peers.lookup(nid))) {
	      di = _peers.insert(nid, new LogicalPeer(nid, this, _aggr));
	    }
	    else {
	      delete nid;  // we don't need this NodeID any more
	    }

	    // We need to reaggregate the link,
	    // so mark the corresponding LogicalLink as dirty
	    (_peers.inf(di))->Update( id_of_new_ptse, new NodeID( ptsp->GetOID() ), ulig);
	  }
	} // End of forall InfoGroup in PTSE
      }
      else { // We have a NULL-BODY uplinks ptse
	
	int id_of_dying_ptse = ptse->GetID();

	dic_item di = 0;
	int affected = 0;
	// Warn LogicalPeers that is an uplink is expiring
	forall_items(di, _peers) {
	  affected += _peers.inf(di)->Update( id_of_dying_ptse, new NodeID( ptsp->GetOID()) );
	}
	assert( affected == 1 );
      }
    } // if ptse type == uplink 
    else // type is not uplink IG so we can skip this PTSE
      continue;
  } // End of forall PTSEs in PTSP

  // We will not reaggregate now.
  // Only when HorLinkVisitors arrive (from SVC Hellos) do we aggregate.
  return true;
}

//-------------------------------------------------------------------------
bool Aggregator::HandleHLink(HorLinkVisitor * hv)
{
  const NodeID * remNode = hv->GetDestNID();

  if (!remNode)
    return false;

  if (hv->GetVT() == HorLinkVisitor::LgnHello) {
    diag("sim.aggregator", DIAG_ERROR, 
	 "%s received a %s of type LgnHello.  LgnHellos should flow only "
	 "between RCCHelloState and LgnHelloState.\n", OwnerName(), hv->GetType().Name());
    hv->Suicide();
    return false;
  }

  // If I am not the leader at the level of the HLink I should not be dealing with it.
  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  bool IAmLeader = dbi->AmILeader( GetNextLevelDown(remNode->GetLevel()) );
  dbi->Unreference();

  if ( ! IAmLeader )
    return true;

  LogicalPeer * lp = 0;
  dic_item di = 0;
  // Find the LogicalPeer that is associated with this HLink
  forall_items(di, _peers) {
    if (_peers.key(di)->equals(remNode)) {
      lp = _peers.inf(di);
      break;
    }
  }

  if (hv->GetAgg() == -1)
    return true;

  switch (hv->GetVT()) {
    // The HLink has come up
    case HorLinkVisitor::HLinkUp:
      if (lp) {
	int agg  = hv->GetAgg();
	lp->HLinkUp(agg,true);
	int lgport = lp->Aggregate(hv);

	if ( lgport > 0 ) {
	  const NodeID * src = hv->GetSourceNID();
	  const NodeID * dst = lp->GetRemoteNID();

	  assert( ! src->equals(dst) && src->GetLevel() == dst->GetLevel() );
	  _aggr->Update_Complex_Rep( src, lgport, true );
	}
      }
      break;
    // The HLink has gone down
    case HorLinkVisitor::HLinkDown:
      if (lp) {
	int agg  = hv->GetAgg();
	lp->HLinkUp(false, agg);
	int lgport = lp->DeAggregate(hv);

	if ( lgport > 0 ) {
	  const NodeID * src = hv->GetSourceNID();
	  const NodeID * dst = lp->GetRemoteNID();

	  assert( ! src->equals(dst) && src->GetLevel() == dst->GetLevel() );
	  _aggr->Update_Complex_Rep( src, lgport, false );
	}
      }
      break;
    // The LgnHelloFSM is requesting its logical port
    case HorLinkVisitor::ReqLogicalPort:
      // This will alloc a logical port if necessary and send the SimEvent back to LgnHello
      ObtainLogicalPort(hv);
      break;
    default:
      diag("sim.aggregator", DIAG_DEBUG, 
	   "Aycarumba!  I cannot handle a HorLinkVisitor::%s\n", hv->PrintVT());
      break;
  }
  return true;
}


//-------------------------------------------------------------------------
bool Aggregator::HandleSVCFromInside(FastUNIVisitor * v)
{
  bool rval = true;

  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  bool LogicalNodeIsMe = v->GetSourceNID() != 0 ? dbi->LogicalNodeIsMe(v->GetSourceNID()) : false;
  dbi->Unreference();

  if (// Is it a setup?
      (v->GetMSGType() == FastUNIVisitor::FastUNISetup) &&
      // Am I the originator of this SVCC setup?
      LogicalNodeIsMe && (v->GetSourceNID()->GetChildLevel() < 160)) {

    // Do I already have an SVCC 
    if (!IsSVCSetup(v->GetDestNID())) {
      // No I don't
      SVCSetup(v->GetDestNID());

      int lp = ObtainLogicalPort(v->GetDestNID(), v->GetAgg());
      v->SetLogicalPort(lp);

      // if not stamp the logical port into the uplink response
      DIAG("sim.aggregator", DIAG_INFO, cout << *(v->GetSourceNID()) <<
	       " setting up SVCC to " << *(v->GetDestNID()) << endl);
    } else {
      // Why, yes, I do have an SVCC setup to this remote Node, but I 
      // may not have setup an LgnHelloFSM for this aggregation token.
      int lp = ObtainLogicalPort(v->GetDestNID(), v->GetAgg());
      v->SetLogicalPort(lp);
      v->SetMSGType(FastUNIVisitor::FastUNILHI);
    }
  } else if (// Is it an LgnHelloInstantiator?
	     (v->GetMSGType() == FastUNIVisitor::FastUNILHI) &&
	     // Am I the originator?
	     LogicalNodeIsMe ) {
    // Do I have an SVCC setup to the remote node?
    if (!IsSVCSetup(v->GetDestNID())) {
      // Nope, so I need to store it until the SVCC has been setup.
      const NodeID * rnid = v->GetDestNID();
      
      dic_item di;
      if (!(di = _lhi_map.lookup(rnid))) {
	di = _lhi_map.insert(rnid->copy(), new ds_queue<FastUNIVisitor *>);
      }
      ds_queue<FastUNIVisitor *> * qptr = _lhi_map.inf(di);

      qptr->append(v);
      rval = false;
    } else {
      // else we must let it pass, but not before providing it with a logical port number
      int lp = ObtainLogicalPort(v->GetDestNID(), v->GetAgg());
      v->SetLogicalPort(lp);
    }
  }
  return rval;
}

//-------------------------------------------------------------------------
bool Aggregator::HandleSVCFromOutside(FastUNIVisitor * v)
{
  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  bool LogicalNodeIsMe = false;
  switch (v->GetMSGType()) {
  case FastUNIVisitor::FastUNISetup:
  case FastUNIVisitor::FastUNIConnect:
    LogicalNodeIsMe = dbi->LogicalNodeIsMe(v->GetDestNID());
  case FastUNIVisitor::FastUNISetupFailure:
  case FastUNIVisitor::FastUNIRelease:
    LogicalNodeIsMe = dbi->LogicalNodeIsMe(v->GetSourceNID());
    break;
  };
  dbi->Unreference();

  // Aggregator only deals with SVCs
  if ( LogicalNodeIsMe ) {
    switch (v->GetMSGType()) {
      case FastUNIVisitor::FastUNISetup:
	abort();
	// Evidently this is no longer ACAC's concern, just let it pass.
	// diag("sim.aggregator", DIAG_WARNING, 
	//	     "Danger! Danger! Will Robinson.\n"
	//	     "%s has received a Call Setup which ACAC should've suicided!\n", 
	//           OwnerName());
	break;

      case FastUNIVisitor::FastUNIConnect:
	{
	  assert (v->GetSourceNID());

	  // If it's made it down here I assume the SVC was setup properly
	  // Add the remote NodeID to my mapping -- do we even have the source NID at this point?
	  SVCSetup(v->GetSourceNID());
	  // The SVCC has been successfully set up then ...
	  DIAG("sim.aggregator", DIAG_INFO, cout << *(v->GetDestNID());
	       if (*(v->GetDestNID()) > *(v->GetSourceNID()))
	         cout << " has setup SVCC to ";
	       else 
	         cout << " received SVCC setup from ";
	       cout << *(v->GetSourceNID()) << endl);
	  // Pull the first LgnHelloInstantiator out of the appropriate queue and send it up.
	  dic_item di;
	  if (di = _lhi_map.lookup(v->GetSourceNID())) {
	    ds_queue<FastUNIVisitor *> * qptr = _lhi_map.inf(di);
	    
	    if (!(qptr->empty())) {
	      // This LgnHI is special, it will rebind the LgnHello
	      FastUNIVisitor * pv = qptr->pop();
	      int lp = ObtainLogicalPort(v->GetSourceNID(), pv->GetAgg());
	      pv->SetLogicalPort(lp);
	      pv->SetMSGType(FastUNIVisitor::FastUNILHIRebinder);
	      PassVisitorToA(pv);
	    }
	     
	    // If there are others left, register the SimEvent for delivery
	    if (!(qptr->empty()) && !(_qse->IsRegistered()))
	      Deliver(_qse, QUEUE_SVC_INTERVAL);

	  }
	  // Allow it to pass on to the Terminal ... for now.
	  return true;
        }
        break;

      case FastUNIVisitor::FastUNISetupFailure:
      case FastUNIVisitor::FastUNIRelease:
	// Teardown the SVC
	assert(v->GetSourceNID());

	SVCTeardown(v->GetSourceNID());
	DIAG("sim.aggregator", DIAG_DEBUG, cout <<
	     OwnerName() << " received " << v->GetType() << " SVC Teardown from " 
	     << *(v->GetSourceNID()) << endl);
	break;

      default:
	break;
    }
  }
  return true;
}

//-------------------------------------------------------------------------
bool Aggregator::HandleLogosGraphFromOutside(LogosGraphVisitor * v)
{
  // Get the graph out of it;
  LogosGraph * lg = v->TakeGraph();
  // Do something with the graph

  // Then delete the Visitor
  v->Suicide();
  return false;
}

//-------------------------------------------------------------------------
void Aggregator::Advertise(NPFloodVisitor * npv)
{
  if (npv) PassVisitorToB(npv);            
}

//-------------------------------------------------------------------------
void Aggregator::Interrupt(SimEvent * e) 
{ 
  if (e->GetCode() == QUEUE_SERVICE_SIM_EVENT_CODE)
    ServiceLHIQueues();
}

//-------------------------------------------------------------------------
void Aggregator::SVCSetup(const NodeID * nid)
{
  if (nid && (!nid->IsZero())) {
    if (!_svcs.search((NodeID *)nid)) {
      NodeID * cpy = nid->copy();
      _svcs.append(cpy);
    }
    if (!_peers.lookup((NodeID *)nid)) {
      NodeID * cpy = nid->copy();
      _peers.insert(cpy, new LogicalPeer(cpy, this, _aggr));
    }
  }
}

//-------------------------------------------------------------------------
void Aggregator::SVCTeardown(const NodeID * nid)
{
  list_item li;
  if (li = _svcs.search((NodeID *)nid)) {
    dic_item di = _peers.lookup((NodeID *) nid);
    assert(di);
    delete _peers.inf(di);
    _peers.del_item(di);
    delete (NodeID *)_svcs.inf(li);
    _svcs.del_item(li);
  }
}

//-------------------------------------------------------------------------
bool Aggregator::IsSVCSetup(const NodeID * nid) const
{
  list_item li;
  if (li = _svcs.search((NodeID *)nid))
    return true;
  return false;
}

//-------------------------------------------------------------------------
int Aggregator::NextID(void) 
{                
  return ++_nextid; // PTSE Identifier allocation
}

//-------------------------------------------------------------------------
// Returns the next avaialable logical port at the specified level
int Aggregator::ObtainNextAvailLogicalPort(int level)
{
  int rval = 0;

  dic_item di;
  di = _LGport_table.lookup(level);
  assert(di);

  dictionary<RACont *, int> * internal = _LGport_table.inf(di);

  dic_item di2;
  forall_items(di2, *internal) {
    int lp = internal->inf(di2);
      
    if (lp > rval) 
      rval = lp;
  }

  return rval+1;
}

//-------------------------------------------------------------------------
// Obtains (through lookup or alloc) the next available logical port
void Aggregator::ObtainLogicalPort(HorLinkVisitor * hv)
{
  int lp = ObtainLogicalPort(hv->GetDestNID(), hv->GetAgg());
  // Return the SimEvent to the LgnHello with the logical port
  SimEvent * se = hv->GetReturnEvent();
  se->SetCode(lp);
  Deliver(se);
}

//-------------------------------------------------------------------------
int Aggregator::ObtainLogicalPort(const NodeID * remNode, int aggTok)
{
  int level = remNode->GetLevel();

  dic_item di, di2;
  if (!(di = _LGport_table.lookup(level)))
    di = _LGport_table.insert(level, new dictionary<RACont *, int>);
  dictionary<RACont *, int> * LGtable = _LGport_table.inf(di);

  RACont * rac = new RACont(remNode, aggTok);
  if (!(di2 = LGtable->lookup(rac)))
    di2 = LGtable->insert(rac, ObtainNextAvailLogicalPort(level));
  else
    delete rac;

  return LGtable->inf(di2);
}

//-------------------------------------------------------------------------
void Aggregator::ServiceLHIQueues(void)
{
  bool rereg = false;

  dic_item di;
  forall_items(di, _lhi_map) {
    NodeID * rnid = (NodeID *)_lhi_map.key(di);
    ds_queue<FastUNIVisitor *> * qptr = _lhi_map.inf(di);

    if (IsSVCSetup(rnid) && (qptr->size() > 0)) {
      // The SVCC has been setup, so send up one LgnHelloInstantiator
      FastUNIVisitor * v = qptr->pop();
      int lp = ObtainLogicalPort(rnid, v->GetAgg());
      v->SetLogicalPort(lp);
      PassVisitorToA(v);
    } else if (qptr->size() > 0)
      rereg = true;
  }

  if (rereg)
    ReturnToSender(_qse, QUEUE_SVC_INTERVAL);
  else
    Cancel(_qse);
}

//-------------------------------------------------------------------------
// --------------------------- RACont -------------------------------
//-------------------------------------------------------------------------
Aggregator::RACont::RACont(const NodeID * rn, int agg) : _aggtok(agg)
{ _rnid = rn->copy(); }

Aggregator::RACont::RACont(const Aggregator::RACont & rhs) 
  : _aggtok(rhs._aggtok)
{ _rnid = rhs._rnid->copy(); }

Aggregator::RACont::~RACont() { delete _rnid; }

//-------------------------------------------------------------------------
Aggregator::RACont & Aggregator::RACont::operator = (const Aggregator::RACont & rhs)
{  
  if (_rnid) delete _rnid;
  _rnid = (rhs._rnid ? rhs._rnid->copy() : 0);
  _aggtok = rhs._aggtok;
  return *this;
}

//-------------------------------------------------------------------------
int operator == (const Aggregator::RACont & lhs, const Aggregator::RACont & rhs)
{
  return !compare((Aggregator::RACont * const)&lhs, 
		  (Aggregator::RACont * const)&rhs);
}

//-------------------------------------------------------------------------
ostream & operator << (ostream & os, const Aggregator::RACont & rhs)
{
  if (rhs._rnid)
    os << *(rhs._rnid) << " ";
  os << "AggTok: " << rhs._aggtok << endl;
  return os;
}

//-------------------------------------------------------------------------
const char * Aggregator::PrintName(void) const
{
  return ((Aggregator *)this)->OwnerName();
}

//-------------------------------------------------------------------------
int Aggregator::GetPhysicalLevel( void ) {
  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  int phy_level = dbi->PhysicalLevel();
  dbi->Unreference();
  return phy_level;
}

//-------------------------------------------------------------------------
int Aggregator::GetNextLevelUp( int level ) 
{
  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  int next_level = dbi->NextLevelUp( level );
  dbi->Unreference();
  return next_level;
}

//-------------------------------------------------------------------------
int Aggregator::GetNextLevelDown( int level ) 
{
  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  int next_level = dbi->NextLevelDown( level );
  dbi->Unreference();
  return next_level;
}

bool Aggregator::AmILeader( int level ) const
{
  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  bool IAmLeader = dbi->AmILeader( level );
  dbi->Unreference();
  return IAmLeader;
}
