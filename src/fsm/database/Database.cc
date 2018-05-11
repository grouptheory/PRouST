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
static char const _Database_cc_rcsid_[] =
"$Id: Database.cc,v 1.385 1999/02/24 15:00:30 mountcas Exp $";
#endif

#include <iostream.h>
#include <fstream.h>

#include <fsm/database/Database.h>
#include <fsm/database/DatabaseEvent.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/database/ExecutionerPTSE.h>
#include <fsm/forwarder/VCAllocator.h>
#include <fsm/config/Configurator.h>

#include <FW/basics/diag.h>
#include <FW/basics/Log.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/SimEvent.h>

#include <fsm/hello/LgnHelloState.h>
#include <fsm/visitors/NPFloodVisitor.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/election/ElectionVisitor.h>
#include <fsm/election/ElectionInterface.h>
#include <fsm/omni/Omni.h>

#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_ig/req_ptse_header.h>
#include <codec/pnni_ig/nodal_ptse_summary.h>
#include <codec/pnni_ig/lgn_horizontal_link_ext.h>
#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/next_hi_level_binding_info.h>
#include <codec/pnni_ig/nodal_hierarchy_list.h>
#include <codec/pnni_pkt/ptsp.h>
#include <codec/pnni_pkt/ptse_req.h>
#include <codec/pnni_pkt/database_sum.h>
#include <codec/pnni_ig/lgn_horizontal_link_ext.h>
#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/addr.h>

#include <common/cprototypes.h>

class InfoGroup;

#ifndef OPTIMIZE
void MEM_CHECK(void)
{ 
  char * block[12]; int sz = 1, i;  
  for (i = 0; i < 12; i++) {  
    block[i] = new char[sz];
    for (int j = 0; j < sz; j++) 
      block[i][j] = 0xAA; 
    sz *= 2;
  } 
  for (i =  0; i < 12; i++)
    delete [] block[i]; 
}

void mallocbug(int ph)
{
  const char foo[22] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  NodeID ** arr;
  cout << "***** Entering Phase " << ph << " *****" << endl;
  for (int iterator = 1; iterator < 20; iterator++) {
    arr = new NodeID * [1000 * iterator];
    for (int yy = 0; yy < (1000 * iterator); yy++)
      arr[yy] = new NodeID((const unsigned char*)foo);
  }
  // cleanup
  for (int it = 1; it < 20; it++) {
    for (int xx= 0; xx < (1000 * it); xx++)
      delete arr[xx];
  }
  delete [] arr;
}
#endif


// ------------------ Database -----------------------
VisitorType * Database::_npflood_type = 0;
//const double  Database::Default_PTSERefreshInterval = 1800.0;
//const double  Database::Default_PTSELifetimeFactor = 2.0;

Database::Database(ds_String * key, const NodeID * mynid) 
  : _key(0), _LocNodeID(0), _executioner(0), _victim_ptse(0),  
    _seq_num(0), _victim_qitem(0), _globalKernel(theKernel()),
    _vca(0)
{
  assert( key != 0 );

  _key = new ds_String( *key );

  if ( mynid != 0 )
    _LocNodeID           = new NodeID(*mynid);
  else
    _LocNodeID           = theConfigurator().BaseNodeID( _key );

  // ---- This MUST be a physical NodeID ----
  assert( _LocNodeID->GetChildLevel() == 160 );

  _vca = new VCAllocator( );
  assert( _vca != 0 );

  if (!_npflood_type)
    _npflood_type = (VisitorType *)QueryRegistry(NPFLOOD_VISITOR_NAME);

  _PTSERefreshInterval = Default_PTSERefreshInterval;
  _PTSELifetimeFactor  = Default_PTSELifetimeFactor;

  _pq.clear();
  _map.clear();
  _trans.clear();
  _uplinks.clear();

  // int level = (int)_LocNodeID->GetLevel();
  // _level[level] = true;
  _mynodes.insert(((int)_LocNodeID->GetLevel()), _LocNodeID->copy());

  _db_event = new DatabaseEvent(this, this);

  // Register any Interfaces we wish to make public
  AddPermission("*",                    new DatabaseInterface(this));
  AddPermission("HelloState",           new DBHelloInterface(this));
  AddPermission("RCCHelloState",        new DBHelloInterface(this));
  AddPermission("NodePeerState*",       new DBNodePeerInterface(this));
  AddPermission("CallAdmissionControl", new DBACACInterface(this));
  AddPermission("Leadership",           new DBLeadershipInterface(this));
  AddPermission("Logos",                new DBLogosInterface(this));
}


//----------------------------------------------------------
Database::~Database(void)
{
  // Clean out the queue and sorted seqs here!
  seq_item si;
  dic_item di;

  delete _key;
  delete _LocNodeID;

  forall_items(si, _map) {
    DBKey * tmp = _map.key(si);
    delete tmp;
    pq_item pi = _map.inf(si);
    ig_ptse * ptse = _pq.inf(pi);
    ptse->UnReference();
  }
  _pq.clear();
  _map.clear();
  _trans.clear();
  
  forall_items(si, _death_row) {
    delete _death_row.key(si);
    ig_ptse * ptse = _death_row.inf(si);
    ptse->UnReference();
  }
  _death_row.clear();

  forall_items(di, _mynodes) {
    delete _mynodes.inf(di);
  }
  _mynodes.clear();

  forall_items(di, _uplinks) {
    delete _uplinks.key(di);
    delete _uplinks.inf(di);
  }
  _uplinks.clear();
  
  delete _db_event;
}

// ---------------------------------------------------------
ig_ptse * Database::ReqPTSE(NodeID * nid, ig_ptse * ptse)
{
  DBKey key(ptse->GetType(), nid, ptse->GetID(), ptse->GetSN(), 
            0, ptse->GetTTL());

  seq_item si;
  if (si = _map.lookup(&key))
    return _pq.inf(_map.inf(si));
  if (si = _death_row.lookup(&key))
    return _death_row.inf(si);
  return 0;
}

//----------------------------------------------------------
void Database::FloodPTSE(PTSPPkt * lPtsp, NodeID * RemoteNodeID, int level)
{
  assert(lPtsp != 0);

#if 0
  ig_nodal_info_group * nig = GetNodalIG( level );
  // This saves us in case the nig doesn't exist;
  NodeID * SourceNodeID     = nig ? new NodeID(nig->GetATMAddress()) :_LocNodeID->GetNodeID(level); 
  int childlevel = SourceNodeID->GetChildLevel();

  do {
    PTSPPkt * out_ptsp = (PTSPPkt *)(lPtsp->copy(true));
    NPFloodVisitor * npfv =
      new NPFloodVisitor(out_ptsp, 0, RemoteNodeID, SourceNodeID);
    PassVisitorToA(npfv);

    delete SourceNodeID;
    level = childlevel;
    nig = GetNodalIG( level );
    SourceNodeID = nig ? new NodeID(nig->GetATMAddress()) :_LocNodeID->GetNodeID(level);
    childlevel = SourceNodeID->GetChildLevel();
  } while (childlevel < 160);
#else
  while (level <= 96) {
    int child_level = theConfigurator().NextLevelDown( _key, level );
    if ( child_level == -1 )
      child_level = 160;

    NodeID * SourceNodeID = new NodeID( level, child_level, _LocNodeID );

    PTSPPkt * out_ptsp = (PTSPPkt *)(lPtsp->copy(true));
    NPFloodVisitor * npfv =
      new NPFloodVisitor(out_ptsp, 0, RemoteNodeID, SourceNodeID);
    delete SourceNodeID;
    PassVisitorToA(npfv);

    // Prepare to flood it at the next level
    level = child_level;
  }
#endif

  lPtsp->ClearExpiredPtses();
  delete lPtsp;
}

//----------------------------------------------------------
bool Database::Expire(ig_ptse * ptse)
{
  ig_ptse * exp = 0;

  seq_item si;
  forall_items(si, _map) {
    if (ptse == (_pq.inf(_map.inf(si)))) {
      exp = _pq.inf(_map.inf(si));
      break;
    }
  }

  if (exp) {
    // ### HandleUpLinks decides if we need to Release an SVCC
    // because this non-empty uplink is expiring.
    if ((exp->GetType() == InfoGroup::ig_uplinks_id) &&
	( exp->ShareMembers() ) &&
	( ! exp->ShareMembers()->empty() )) {
      NodeID * Originator = PTSEOriginator(exp);
      //      cout << "###Bilal###" << " HandleUplinks for Expire\n";
      HandleUpLinks(Originator, exp, false  /* remove-mode */ ); 
      delete Originator;
    }
    // ### 

    ig_ptse * new_ptse = exp->copy_husk();
    new_ptse->RemMembers();
    new_ptse->SetSN(exp->GetSN() + 1);
    new_ptse->SetTTL(DBKey::ExpiredAge);

    seq_item seqi = _death_row.insert(_map.key(si), new_ptse);
    new_ptse->Sentence(seqi, &_death_row);
    
    // Create a PTSPpkt to be sent
    DBKey * dbk = _map.key(si);
    PTSPPkt * dPtsp = new PTSPPkt( dbk->GetOrigin(), dbk->GetPeer() );
    
    // Remove the old PTSE (that had a body)
    Purge(si);

    // Notify Omni about the new ptse
    if ( ! theOmni().AmIOmniscient( this ) ) {

      NodeID tmp;
      dbk->GetOrigin( tmp );  // This should fill it in
      if ( _LocNodeID->Is_Me_At_Some_Level( &tmp ) ) {
	PTSPPkt * ptsp = new PTSPPkt( tmp.GetNID(),
				      tmp.GetPID() );
	ptsp->AddPTSE( (ig_ptse*)new_ptse->copy() );
	NPFloodVisitor * npv = new NPFloodVisitor(ptsp, true, &tmp, &tmp );
	theOmni().UpdateOmniDB( npv );
      }

      int orig_level = ( dbk->GetOrigin() ) [0];
      int recv_level = orig_level;
      while (recv_level <= 96) {
	if ((recv_level == 96) ||
	    (AmILeader(recv_level + 8))) {
	  int child_level = theConfigurator().NextLevelDown( _key, recv_level );
	  if ( child_level == -1 )
	    child_level = 160;
	  NodeID * recip = new NodeID( recv_level, child_level, _LocNodeID );
	  theOmni().Report_PTSE( recip, new DBKey( *dbk ) );
	  theOmni().RecomputeGlobalSynchrony( recip );
	  delete recip;
	}
	recv_level += 8;
      }
    }

    // Flood the newly emaciated PTSE, and 
    // have our NpeerFSMs replace their references to
    // the old one
    dPtsp->AddPTSE(new_ptse);
    FloodContainer * fc = new FloodContainer(dPtsp, 0);
    _pending_floods.append(fc);
    Deliver(_db_event, 0);
    return true;
  }
  return false;
}


// This MUST be private, can only be called by Insert(PTSPPkt, ...) 
// because it doesn't do any flooding
const ig_ptse * Database::Insert(const NodeID * Originator, ig_ptse * ptse, bool management)
{
  const ig_ptse * rval = 0;
  ig_ptse * existingPTSE = 0;  // This one can't be const
  bool IAmOriginator = false;

  if ( _LocNodeID->Is_Me_At_Some_Level( Originator ) )
    IAmOriginator = true;
  
  // Obtain the local NodeID at the specified level
  const NodeID * localNodeID = GetMyNodeID( Originator->GetLevel() );

  PeerID * pgid = Originator->GetPeerGroup();
  // Generate a proper DBKey for this PTSE
  DBKey * dbk = new DBKey(ptse->GetType(), Originator, ptse->GetID(),
			  ptse->GetSN(), pgid, 
			  ptse->GetTTL());
  delete pgid;

  DIAG("emul.database", DIAG_INFO,
       char debug_key[255];
       dbk->Print(debug_key);
       cout << Originator->Print() << " gave us " << debug_key;
       if ((!ptse->ShareMembers()) || (ptse->ShareMembers()->size()==0)) {
	 cout << " NULLBODY "; 
       }
       cout << "\n"; );

  // DEBUGGING
  char myAddr[64];
  sprintf(myAddr, "%s", _LocNodeID->Print());
  diag("fsm.database", DIAG_DEBUG, "Database %s:\n\tInserting PTSE of type %s from %s.\n",
       myAddr, IG2Name(ptse->GetType()), Originator->Print());

  DIAG("bilal.database.logging", DIAG_DEBUG,
       char *str;
       NodeID *debug_sw = 0;

       str = getenv("DEBUG_SWITCH");
       if ((!str) ||
	   ( strlen(str)>48 ) || 
	   ( strlen(str)<44 )) {
	 cout << "environment variable DEBUG_SWITCH must be set to something like\n" 
	      << "96:160:47000580ffde0000000003110200000000000000" << endl;
	 abort();
       }
       debug_sw = new NodeID(str);
       assert(debug_sw);

       if ((debug_sw) && (debug_sw->equals(_LocNodeID))) {
	 char buf[64];
	 sprintf(buf, "Database.%s__%s", getenv("USER"), debug_sw->Print());
	 ostream * log = GetLog(buf);
	 if (log && ((ptse->GetType() == InfoGroup::ig_nodal_info_group_id) ||
		     (ptse->GetType() == InfoGroup::ig_uplinks_id))) {
	   *log << theKernel().CurrentElapsedTime() << ": ";
	   *log << "Database " << *_LocNodeID << " inserted PTSE " 
		<< IG2Name(ptse->GetType()) << " at time " << theKernel().CurrentElapsedTime() 
		<< endl;
	   if (ptse->GetType() == InfoGroup::ig_nodal_info_group_id) {
	     ig_nodal_info_group * nig = 
	       (ig_nodal_info_group *)(ptse->ShareMembers() ? ptse->ShareMembers()->head() : 0);
	     // print the origin, PGL, seqnum, ttl
	     *log << "\tOriginator: " << *Originator << endl;
	     if (nig)
	       *log << "\tPrefPGL: " << *nig->GetPreferredPGL() << endl;
	     *log << "\tSeqNum: " << ptse->GetSN() << " TTL: " << ptse->GetTTL() << endl;
	   } else if (ptse->GetType() == InfoGroup::ig_uplinks_id) {
	     ig_uplinks * upl = 
	       (ig_uplinks *)(ptse->ShareMembers() ? ptse->ShareMembers()->head() : 0);
	     // print the origin, remote upnode, seqnum, ttl
	     *log << "\tOriginator: " << *Originator << endl;
	     if (upl)
	       *log << "\tRemNode: " << *upl->GetRemoteID() << endl;
	     *log << "\tSeqNum: " << ptse->GetSN() << " TTL: " << ptse->GetTTL() << endl;
	   }
	 }
       }
     );    // End of DIAG

  // Locate the specified PTSE if we already know of it
  seq_item si;
  if (!(si = _map.lookup( dbk ))) {
    // Perhaps it's on death row?
    if (si = _death_row.lookup( dbk ))
      existingPTSE = _death_row.inf(si);
  } else
    existingPTSE = _pq.inf( _map.inf(si) );
	
  // PNNI Spec v1.0 page 108 
  if (!existingPTSE) {

    // cases 5a, 6a i and ii
    if (Originator->equals(localNodeID) && !management)  
      ptse->SetTTL(DBKey::ExpiredAge);
    if (ptse->GetTTL() == DBKey::ExpiredAge) {
      // No existing PTSE and new PTSE is expired age, 
      // it needs to be convicted and flooded
      ptse->RemMembers();
      ptse->Sentence( _death_row.insert( dbk, ptse ), &_death_row );
      
      diag("fsm.database", DIAG_DEBUG, "Database %s:\n\tFlushing PTSE of type %s from %s.\n",
	   myAddr, IG2Name(ptse->GetType()), Originator->Print());
      rval = ptse;
    } else {
      // No existing PTSE and new PTSE is not expired age.  
      // It needs to be inserted
      double     TimeToLive = ptse->GetTTL();
      KernelTime ExpireTime = (KernelTime)_globalKernel.CurrentTime() + 
	                      (KernelTime)TimeToLive;

      pq_item pi = _pq.insert(ExpireTime, ptse); 
      _trans.insert( pi, _map.insert( dbk, pi ) );

      // Notify Omni about the new ptse
      if ( ! theOmni().AmIOmniscient( this ) ) {

	if ( IAmOriginator ) {
	  PTSPPkt * ptsp = new PTSPPkt( Originator->GetNID(),
					Originator->GetPID() );
	  ptsp->AddPTSE( (ig_ptse*)ptse->copy() );
	  NPFloodVisitor * npv = new NPFloodVisitor(ptsp, true, (NodeID*)Originator, (NodeID*)Originator );
	  theOmni().UpdateOmniDB( npv );
	}

	int orig_level = ( dbk->GetOrigin() ) [0];
	int recv_level = orig_level;
	while (recv_level <= 96) {
	  if ((recv_level==96) ||
	      (AmILeader(recv_level+8))) {
	    int child_level = theConfigurator().NextLevelDown( _key, recv_level );
	    if ( child_level == -1 )
	      child_level = 160;
	    NodeID * recip = new NodeID( recv_level, child_level, _LocNodeID );
	    theOmni().Report_PTSE( recip, new DBKey( *dbk ) );
	    theOmni().RecomputeGlobalSynchrony( recip );
	    delete recip;
	  }
	  recv_level += 8;
	};
      }

      SendElectionUpdate(Originator, ptse);
    }
  } else { 
    // There is an existing PTSE...
    
    if (ptse->GetTTL() != DBKey::ExpiredAge) {

      /* ------------NEW LOCATION */
      if ( ( existingPTSE != 0 ) && 
	   (! existingPTSE->ShareMembers())) {
	assert( existingPTSE->GetTTL() == DBKey::ExpiredAge );
	// a dead-body PTSE is as good as nothing -- bilal
	SendElectionUpdate(Originator, ptse);
      } else
	SendElectionUpdate(Originator, ptse, existingPTSE);
      /* ------------NEW LOCATION  */
    }

    // case 6c:  If existingPTSE is expired age it will get deleted
    //           when all of the FSM's no longer need it.
    if (existingPTSE->GetTTL() != DBKey::ExpiredAge) {
      assert(si != 0);
      pq_item pqi = _map.inf(si);

      if (ptse->GetTTL() == DBKey::ExpiredAge &&
	  ptse->GetType() == InfoGroup::ig_uplinks_id)
	HandleUpLinks(Originator, existingPTSE, false /* remove-mode*/ );
      
      if ( existingPTSE->GetType() != InfoGroup::ig_horizontal_links_id ||
	   ( existingPTSE->GetType() != InfoGroup::ig_nodal_info_group_id ||
	     ptse->GetTTL() == DBKey::ExpiredAge ) ) {
	// This existing PTSE is not going to be flooded, simply discarded
	existingPTSE->UnReference();
	existingPTSE = 0;
      }
      // Remove the existing PTSE from the Database
      _pq.del_item( pqi );
      _trans.del_item( _trans.lookup( pqi ) );
      DBKey * tmp = _map.key( si );
      delete tmp;
      _map.del_item( si );
      // Check if this is the PTSE the executioner is waiting to kill
      if (_victim_qitem == pqi) {
	delete _executioner; 
	_executioner = 0;
	_victim_ptse = 0;
	_victim_qitem = 0;
      }
    }

    // Check if the ptse is expired
    if (ptse->GetTTL() == DBKey::ExpiredAge) {

      ptse->Sentence( _death_row.insert( dbk, ptse ), &_death_row );
      diag("fsm.database", DIAG_DEBUG, "Database %s:\n\tFlushing PTSE of type %s from %s.\n",
	   myAddr, IG2Name(ptse->GetType()), Originator->Print());
      rval = ptse;
    } else {

      double     TimeToLive = ptse->GetTTL();
      KernelTime ExpireTime = (KernelTime)_globalKernel.CurrentTime() + 
	                      (KernelTime)TimeToLive;

      pq_item pi = _pq.insert(ExpireTime, ptse);  
      _trans.insert( pi, _map.insert( dbk, pi ) );

      // Notify Omni about the new ptse
      if ( ! theOmni().AmIOmniscient( this ) ) {

	if ( IAmOriginator ) {
	  PTSPPkt * ptsp = new PTSPPkt( Originator->GetNID(),
					Originator->GetPID() );
	  ptsp->AddPTSE( (ig_ptse*)ptse->copy() );
	  NPFloodVisitor * npv = new NPFloodVisitor(ptsp, true, (NodeID*)Originator, (NodeID*)Originator );
	  theOmni().UpdateOmniDB( npv );
	}

	int orig_level = ( dbk->GetOrigin() ) [0];
	int recv_level = orig_level;
	while (recv_level <= 96) {
	  if ((recv_level==96) ||
	      (AmILeader(recv_level+8))) {
	    int child_level = theConfigurator().NextLevelDown( _key, recv_level );
	    if ( child_level == -1 )
	      child_level = 160;
	    NodeID * recip = new NodeID( recv_level, child_level, _LocNodeID );
	    theOmni().Report_PTSE( recip, new DBKey( *dbk ) );
	    theOmni().RecomputeGlobalSynchrony( recip );
	    delete recip;
	  }
	  recv_level += 8;
	};
      }

      // existingPTSE could possibly set to 0 above ...
      if (existingPTSE && existingPTSE->GetTTL() != DBKey::ExpiredAge)
	existingPTSE->UnReference();
    }
  }

  // Check if the database is empty
  if (_pq.empty()) {
    _victim_qitem = 0;
    _victim_ptse  = 0;
    // There is no PTSE about to expire next, 
    // so release the executioner from his work
    if (_executioner) 
      delete _executioner; 
    _executioner  = 0;
    return 0L;            // this is the only premature return
  }
  // Setup the executioner to take care of the PTSE which expires next
  SetupExecutioner();

  // ### If this PTSE was a non-empty uplink HandleUpLinks will
  // determine if an SVCC needs to be setup
  if (( ptse->ShareMembers() ) &&
      ( ! ptse->ShareMembers()->empty()) &&
      (ptse->GetType() == InfoGroup::ig_uplinks_id)) {
    HandleUpLinks(Originator, ptse, true /* insert-mode*/ );
  }
  // ### 

  return rval;
}

//----------------------------------------------------------
bool Database::Insert(PTSPPkt * ptsp, bool mgmt, NodeID * RemoteNodeID)
{
  assert(ptsp != 0);

  // loop through ptsp->_elements (all of which are ig_ptse *),
  // storing them.
  const list<ig_ptse *> & ptse_list = ptsp->GetElements();

  // Flood all of the PTSEs inside one really, really big PTSP
  // (prevents the queue from getting RSI)
  PTSPPkt * floodPTSP = new PTSPPkt(ptsp->GetOID(), ptsp->GetPGID());

  NodeID nid(ptsp->GetOID());
  PeerID pid(ptsp->GetPGID());
  
  list_item li;
  forall_items(li, ptse_list) {
    ig_ptse * tmp = (ptse_list.inf(li));
    //    ig_ptse * dead_ptse = Insert(&nid, tmp, &pid, mgmt, RemoteNodeID, false);
    const ig_ptse * dead_ptse = Insert(&nid, tmp, mgmt);

    if (dead_ptse)
      floodPTSP->AddPTSE((ig_ptse *)dead_ptse);
    else {
      // insert all the PTSES -- Added for reduced flooding
      ig_ptse * new_ptse_for_flood_use_only = (ig_ptse *)(tmp->copy());
      floodPTSP->AddPTSE(new_ptse_for_flood_use_only);
    }
  }

  const NodeID * mynode = GetMyNodeID(nid.GetLevel());

  // Am I supposed to know whom else to flood it to?
  if (nid == *mynode) {
    FloodContainer * fc = new FloodContainer(floodPTSP, 0);
    _pending_floods.append(fc);
    Deliver(_db_event, 0);
    // FloodPTSE(floodPTSP, 0, level);
  } else {
    FloodContainer * fc = new FloodContainer(floodPTSP, RemoteNodeID);
    _pending_floods.append(fc);
    Deliver(_db_event, 0);
    // FloodPTSE(floodPTSP, RemoteNodeID, level);
  }
  ptsp->ClearElements();
  delete ptsp;

  return true;
}

// ---------------------------------------------------------
bool Database::Insert(NPFloodVisitor * v) 
{ 
  const PTSPPkt * fptsp = v->GetFloodPTSP();
  v->SetPTSP(0);
  return Insert((PTSPPkt *)fptsp, v->Getmgmt(), (NodeID*)v->GetRemoteNodeID()); 
}

// ------------------------------
void Database::Purge(seq_item si)
{
  pq_item pqi = _map.inf(si);
  (_pq.inf(pqi))->UnReference();
  _pq.del_item(pqi);
  // remove it from the translation
  seq_item dci = _trans.lookup(pqi);
  _trans.del_item(dci);
  // remove it from the map.
  _map.del_item(si);
  
  if (_victim_qitem == pqi) {
    delete _executioner; 
    _executioner = 0;
    _victim_ptse = 0;
    _victim_qitem = 0;
  }
  SetupExecutioner();
}

// ----------------------------------
void Database::SetupExecutioner(void)
{
  bool change_executioner = true;

  if (_pq.empty()) {
    if (_executioner) 
      delete _executioner;
    _executioner = 0;
    _victim_ptse = 0;
    _victim_qitem = 0;
    return;
  }

  if (_executioner) {
    if (_pq.find_min() == _victim_qitem)
      change_executioner = false;
    else
      delete _executioner;
  }
  
  if (change_executioner) {
    if (_victim_qitem = _pq.find_min()) {
      _victim_ptse = _pq.inf(_victim_qitem);
      double date_of_death  = _pq.prio(_victim_qitem);
      double remaining_time = (KernelTime)date_of_death - 
	(KernelTime)_globalKernel.CurrentTime();
      _executioner = new ExecutionerPTSE(remaining_time, *this);
      Register(_executioner);
    } else {
      diag("fsm.database", DIAG_ERROR, "Database: No victim, yet queue not empty...\n");
    }
  }
}


// Called ONLY by the ExecutionerPTSE
void Database::ExpirePTSE(void)
{
  if (_victim_qitem == 0)
    return;

  seq_item ti;
  if (ti = _trans.lookup(_victim_qitem)) {
    pq_item  trans_pqitem = _trans.key(ti);
    seq_item trans_sqitem = _trans.inf(ti);
    ig_ptse * ptse = _pq.inf(trans_pqitem);
    DBKey * key    = _map.key(trans_sqitem);

    if (!Expire(ptse)) {
      diag("fsm.database", DIAG_ERROR, "ExpirePTSE failed to find the victim PTSE!\n");
    }
  } else
    _victim_qitem = 0;
}

void Database::UpdatePTSELists(void)
{
  seq_item loop_di;
  forall_items(loop_di, _npses) {
    delete _npses.key(loop_di);
    _npses.inf(loop_di)->clear();  // bilal-aug16
  }
  _npses.clear();

  seq_item si;
  forall_items(si, _map) { 
    ig_ptse * curr_ptse = _pq.inf(_map.inf(si));
    OriginKey * ogk = new OriginKey(*_map.key(si));

    // If not in there insert it.
    if (!(loop_di = _npses.lookup(ogk))) 
      loop_di = _npses.insert(ogk, new list<ig_ptse *>);
    else
      delete ogk;
    // Append PTSE.
    assert(loop_di);
    _npses.inf(loop_di)->append(curr_ptse);
  }
}

void Database::SendElectionUpdate(const NodeID * origin, 
				  ig_ptse * newPTSE, 
                                  ig_ptse * oldPTSE)
{
  const NodeID * SourceNodeID = GetMyNodeID((int)origin->GetLevel());
  ElectionVisitor * ev = 0;
  
  // If there is an oldPTSE and both are nodal IGs
  if (oldPTSE != 0 &&
      (newPTSE->GetType() == InfoGroup::ig_nodal_info_group_id) &&
      (oldPTSE->GetType() == InfoGroup::ig_nodal_info_group_id)) { 

    ig_nodal_info_group * newNodalIG = 
      (ig_nodal_info_group *)((newPTSE->ShareMembers())->head());
    
    ig_nodal_info_group * oldNodalIG = 
      (ig_nodal_info_group *)((oldPTSE->ShareMembers())->head());

    NodeID * newPreferredPGL = (NodeID *)newNodalIG->GetPreferredPGL(),
           * oldPreferredPGL = (NodeID *)oldNodalIG->GetPreferredPGL();

    if ( ( newNodalIG->IsSet(ig_nodal_info_group::ntrans_ele_bit) != 
	   oldNodalIG->IsSet(ig_nodal_info_group::ntrans_ele_bit) ) ||
	 ( newNodalIG->GetLeadershipPriority() !=
	   oldNodalIG->GetLeadershipPriority() ) ||
	 ( newNodalIG->IsSet(ig_nodal_info_group::leader_bit) !=
	   oldNodalIG->IsSet(ig_nodal_info_group::leader_bit) ) ||
	 ( (newPreferredPGL && !oldPreferredPGL) ||
	   (!newPreferredPGL && oldPreferredPGL) ) ||
	 !( newPreferredPGL->equals(oldPreferredPGL) )) {
      ev = new ElectionVisitor((NodeID *)SourceNodeID, 
			       ElectionVisitor::InsertedNodalInfo, 
			       newNodalIG, (NodeID *)origin);
    }
    delete newPreferredPGL;
    delete oldPreferredPGL;
  } else if (oldPTSE == 0 && 
	     (newPTSE->GetType() == InfoGroup::ig_nodal_info_group_id) &&
	     (newPTSE->ShareMembers()->empty() == false)) {
    ig_nodal_info_group * newNodalIG = 
      (ig_nodal_info_group *)((newPTSE->ShareMembers())->head());
    ev = new ElectionVisitor((NodeID *)SourceNodeID, 
			     ElectionVisitor::InsertedNodalInfo, 
			     newNodalIG, (NodeID *)origin);
  } else if (oldPTSE != 0 && 
	     (newPTSE->GetType() == InfoGroup::ig_horizontal_links_id) &&
	     (oldPTSE->GetType() == InfoGroup::ig_horizontal_links_id)) {
    if ((newPTSE->GetTTL() == DBKey::ExpiredAge) &&
	(oldPTSE->GetTTL() != DBKey::ExpiredAge)) {
      ev = new ElectionVisitor((NodeID *)SourceNodeID, 
			       ElectionVisitor::HorizontalLinkDown);
    } else if ((newPTSE->GetTTL() != DBKey::ExpiredAge) &&
	       (oldPTSE->GetTTL() == DBKey::ExpiredAge)) {
      ev = new ElectionVisitor((NodeID *)SourceNodeID, 
			       ElectionVisitor::HorizontalLinkUp);
    }
  } else if (oldPTSE == 0 && 
	     (newPTSE->GetType() == InfoGroup::ig_horizontal_links_id)) {
    if (newPTSE->GetTTL() != DBKey::ExpiredAge) {
      ev = new ElectionVisitor((NodeID *)SourceNodeID, 
			       ElectionVisitor::HorizontalLinkUp);
    }
  }

  if (ev != 0) {
    char buf[64], logString[64];

    int level       = SourceNodeID->GetLevel();
    int child_level = theConfigurator().NextLevelDown( _key, level );
    if ( child_level == -1 )
      child_level = 160;

    sprintf(logString, "Logical-%d", level );
    sprintf(buf, "%s.ElectionState", level >= 96 ? "Physical" : logString);
    
    if ( ! theOmni().AmIOmniscient( this ) &&
	 ( child_level == 160 || 
	   AmILeader( child_level ) ) ) {
      // If I'm NOT omniscient AND we are the physical level OR we are leader at the child level, 
      // notify the election that cares about this nodal IG
      ElectionInterface * eif = (ElectionInterface *)QueryInterface( buf );
      assert( eif != 0 && eif->good() );
      eif->Reference();
      eif->ElectionUpdate( ev );
      eif->Unreference();
      // else
      // PassVisitorToA( ev );
    }
  }
}

const NodeID * Database::GetMyNodeID(const int level) const
{
  const NodeID * mynode = 0;
  dic_item di;

  if (!(di = _mynodes.lookup(level))) {
    int child_level = theConfigurator().NextLevelDown( _key, level );
    if ( child_level == -1 )
      child_level = 160;
    mynode = new NodeID( level, child_level, _LocNodeID );
    Database * db = (Database *)this;
    db->_mynodes.insert(level, mynode);
  } else
    mynode = _mynodes.inf(di);
 
  return mynode;
}

const list<ig_ptse *> * Database::GetPTSEListByOrigin(NodeID *origin)
{
  // Make sure the lists are up to date.
  UpdatePTSELists();
  const u_char * originator = origin->GetNID();
  OriginKey master(originator);
  
  seq_item di;
  if (di = _npses.lookup(&master))
    return _npses.inf(di);
  return 0;
}

DatabaseSumPkt * Database::GetDatabaseSummary(int RequestedLevel)
{
  DatabaseSumPkt * rval = 0;

  // Make sure the lists are up to date.
  UpdatePTSELists();

  // iterate over all the PTSEs in the database, sorted by Originator
  seq_item si;
  forall_items(si, _npses)  { 
    const u_char * origin  = _npses.key(si)->GetOrigin();
    const u_char * peer    = _npses.key(si)->GetPeer();
    int OriginatorLevel    = origin[0];

    // This allows information to be flooded down the hierarchy
    if (RequestedLevel >= OriginatorLevel) {
      list<ig_ptse *> * lptr = _npses.inf(si);    
	  
      // Construct a PTSE summary
      ig_nodal_ptse_summary * npsp =
	new ig_nodal_ptse_summary((u_char *)origin, (u_char *)peer);

      // Append a summary of each PTSE
      list_item li;
      forall_items(li, *(lptr))  {
        ig_ptse * curr_ptse = lptr->inf(li);
        // sh_int type, int id, int seq, sh_int check, sh_int life
        npsp->AddSum(curr_ptse->GetType(), curr_ptse->GetID(),
                     curr_ptse->GetSN(), curr_ptse->GetCS(), 
                     curr_ptse->GetTTL());
      }
      // we will add dead ptse's also
      seq_item dsi;
      NodeID lhs(origin);

      forall_items(dsi, _death_row) {
        DBKey dkey = *(_death_row.key(dsi));
        NodeID rhs(dkey.GetOrigin());

	// Check to make sure this dead PTSE was originated by the same node
        if (lhs == rhs) {
          ig_ptse * curr_ptse = _death_row.inf(dsi);
	  
          if (curr_ptse)
            npsp->AddSum(curr_ptse->GetType(), curr_ptse->GetID(),
                         curr_ptse->GetSN(), curr_ptse->GetCS(),
                         curr_ptse->GetTTL());
	  else {
             diag("fsm.database", DIAG_ERROR, 
                  "In death row, entry without PTSE.\n");
	   }
        }
      }

      if (!rval) 
	rval = new DatabaseSumPkt(_seq_num++);
      rval->AddNodalPTSESum(npsp);
    }
  }

  return rval;
}

int Database::DumpCheckpoint(char * filename)
{
  ofstream of(filename);

  if (of.bad())
    return -1;

  return DumpCheckpoint(of);
}

int Database::DumpCheckpoint(ostream & os)
{
  const int BUFSIZE = 32000;
  unsigned char buffer[BUFSIZE];
  unsigned char * buffer_write_ptr = buffer;
  int written = 0;

  seq_item di;
  diag("fsm.database", DIAG_DEBUG, "Number of PTSEs in Database is %d.\n", _map.size());

  // Make sure the lists are up to date.
  UpdatePTSELists();
  int size = 0;
  // iterate through the dictionary of lists.
  forall_items(di, _npses) {
    list<ig_ptse *> * lptr = _npses.inf(di);

    if (lptr->empty())
      continue;

    const u_char * origin = _npses.key(di)->GetOrigin();
    const u_char * peer   = _npses.key(di)->GetPeer();

    PTSPPkt * lPtsp = new PTSPPkt(origin, peer);

    list_item li;
    forall_items(li, *(lptr)) {
      ig_ptse * curr_ptse = lptr->inf(li);
      lPtsp->AddPTSE(curr_ptse);
    }

    int this_shot = BUFSIZE - written;
    buffer_write_ptr = lPtsp->encode(buffer_write_ptr,this_shot);
    written += this_shot;
    delete lPtsp;
  }

  int ctr = 0;
  for (int i = 0; i < written; i++) {
    os << hex << (int)buffer[i] << " ";
    if (++ctr == 40) {
      os << endl;
      ctr = 0;
    }
  }
  os << dec;
  return written;
}

PTSEReqPkt * Database::ReqDiffDB(DatabaseSumPkt * dsp, 
                                 NodeID * RemoteNodeID)
{

  list<ig_nodal_ptse_summary *> nodal_summaries = dsp->GetNodalSummaries();
  PTSEReqPkt * reqpkt = new PTSEReqPkt();

  int numreqs = 0;
  list_item li_sum1node;
  // for each header

  forall_items(li_sum1node, nodal_summaries) {
    ig_nodal_ptse_summary * sum1node = nodal_summaries.inf(li_sum1node);
    list<SumContainer *> cont_list = sum1node->GetContainers();

    const NodeID *tempOrigin = sum1node->GetOrigin();
    const PeerID *tempPeer   = sum1node->GetPeer();

    OriginKey ogk(tempOrigin->GetNID());
    int level = (int)tempOrigin->GetLevel();

    // My equivalent node at this level
    const NodeID * mynode = GetMyNodeID(level);

    ig_req_ptse_header * rph = new ig_req_ptse_header(tempOrigin);
    seq_item  si_deathmap, si_dbkeymap;
    list_item li_cont;

    // for each sum container
    forall_items(li_cont, cont_list) {
      SumContainer * sc = cont_list.inf(li_cont);
      // See both in live and death list to see if present

      DBKey hiskey((InfoGroup::ig_id)sc->_ptse_type, tempOrigin, 
		   sc->_ptse_id, sc->_ptse_seq, tempPeer, sc->_ptse_rem_life);
      si_dbkeymap = _map.lookup(&hiskey);

      NodeID hisnid(hiskey.GetOrigin());

      DBKey * ourkey;
      
      if (si_dbkeymap) 
	ourkey = _map.key(si_dbkeymap);
      else {
	if (si_deathmap = _death_row.lookup(&hiskey))
	  ourkey = _death_row.key(si_deathmap);
      }

      // si_dbkeymap <-> item found in our _map
      // si_deathmap <-> item found in our _deathrow
      if (si_dbkeymap || si_deathmap) {
	if (*ourkey < hiskey) {
	  // See if mynode
	  if (*(mynode) == hisnid) {
	    // My node make & Insert in PTSE
	    // this will take care of reorgination
	    ig_ptse * lPtse =  new ig_ptse((InfoGroup::ig_id)sc->_ptse_type, 
					   sc->_ptse_id, 
					   sc->_ptse_seq, 
					   sc->_ptse_checksum, 
					   sc->_ptse_rem_life);
	    NodeID nid(ourkey->GetOrigin());
	    PeerID pid(ourkey->GetPeer());
            // Inserted due to a  peer node hence 0 last argument
#if 1
	    PTSPPkt * ptsp = new PTSPPkt(ourkey->GetOrigin(), ourkey->GetPeer());
	    ptsp->AddPTSE(lPtse);
	    Insert(ptsp, false, RemoteNodeID);
#else
	    Insert(&nid, lPtse, &pid, 0, RemoteNodeID, true);
#endif
	  }
	  // if other nodes & most recent
	  // if expired, call expire
	  else if (hiskey.GetTTL() == DBKey::ExpiredAge) {
	    if ((ourkey->GetTTL() != DBKey::ExpiredAge)) {
	      ig_ptse * lPtse = _pq.inf(_map.inf(si_dbkeymap));
              // Expire PTSE increments one and we
              // have the SN of dead ptse same as that of his
              lPtse->SetSN((hiskey.GetSN())-1);
	      Expire(lPtse);
	    }
	  }
	  // other node & not expired most recent include in summary
	  // ReqContainer is list of structs which contains
	  // ptse id & seqno & TTL. Because we require to
	  // compare this info with a ptsp which comes to us
	  // (to see if a less recent ptse has arrived than one
	  // requested
	  else {
	    rph->AddReqContainer(sc->_ptse_id, sc->_ptse_seq, 
				 sc->_ptse_rem_life);
	    numreqs++;
	  }
	}
      } else {
	// Not present anywhere
	if (*(mynode) == hisnid) {
	  // My node & not present
	  // must expire & flood, this will take care
	  ig_ptse * lPtse =  new ig_ptse((InfoGroup::ig_id)sc->_ptse_type, 
					 sc->_ptse_id, 
					 sc->_ptse_seq, 
					 sc->_ptse_checksum, 
					 sc->_ptse_rem_life);
	  NodeID  nid(ourkey->GetOrigin());
	  PeerID  pid(ourkey->GetPeer());

#if 1
	  PTSPPkt * ptsp = new PTSPPkt(ourkey->GetOrigin(), ourkey->GetPeer());
	  ptsp->AddPTSE(lPtse);
	  Insert(ptsp, false, RemoteNodeID);
#else
	  Insert(&nid, lPtse, &pid, 0, RemoteNodeID, true);
#endif
	} else if (hiskey.GetTTL() == DBKey::ExpiredAge) {
	  // Not present but expired
	  // add in del list & flood
	  DBKey * key = new DBKey((InfoGroup::ig_id)sc->_ptse_type, tempOrigin, sc->_ptse_id, 
				  sc->_ptse_seq, tempPeer, sc->_ptse_rem_life);

	  ig_ptse * ptse = new ig_ptse((InfoGroup::ig_id)sc->_ptse_type, sc->_ptse_id, 
				       sc->_ptse_seq, sc->_ptse_checksum, 
				       sc->_ptse_rem_life);
	  seq_item DoA = _death_row.insert(key, ptse);
	  ptse->Sentence(DoA, &_death_row);
	  PTSPPkt * lPtsp = new PTSPPkt(tempOrigin->GetNID(), 
					tempPeer->GetPGID());
	  lPtsp->AddPTSE(ptse);
	  FloodContainer * fc = new FloodContainer(lPtsp, RemoteNodeID);
	  _pending_floods.append(fc);
	  Deliver(_db_event, 0);
	  // FloodPTSE(lPtsp, RemoteNodeID, lPtsp->GetOID()->GetLevel());
	} else {
	  // other nodes, not present & needed by our node
	  // commented for now 
	  rph->AddReqContainer(sc->_ptse_id, sc->_ptse_seq, sc->_ptse_rem_life); 
	  numreqs++;
	}
      }
    }
    delete (NodeID *)tempOrigin;
    delete (PeerID *)tempPeer;

    if (!(rph->GetReqSummary().empty()))
      // See ptse_req.cc AddNodalPTSEReq for another instance of leak
      // aug 2 Sandeep
      reqpkt->AddNodalPTSEReq(rph);
    else 
      delete rph;
  }
  if (!numreqs) {
    delete reqpkt;
    reqpkt = 0;
  }
  return reqpkt;
}

// This must return a list because there can be only one PTSPPkt per
// originator.
list<PTSPPkt *> * Database::FloodDiffDB(PTSEReqPkt * prp)
{
  int nreqs = 0;
  list_item li;
  list<PTSPPkt *> * ptsp_list = 0;
  PTSPPkt * ptsp = 0;

  list<ig_req_ptse_header *> header_list = prp->GetHeaders();

  // iterate through the headers in the request, culling the requested
  //   PTSEs from the the _npses item which contains the list for that
  //   originator and add it to the PTSP.
  list_item header_li, id_li;

  DIAG("sim.emul", DIAG_DEBUG, 
       cout << "ABDELLA There are " << header_list.size() << "headers in the PTSE_Req packet\n");
  forall_items(header_li, header_list) {
    ig_req_ptse_header * req_hdr = header_list.inf(header_li);

    list<ReqContainer *> id_list = req_hdr->GetReqSummary(); 
    const NodeID * tempOrigin    = req_hdr->GetOrigin();

    DIAG("sim.emul", DIAG_DEBUG,
	 cout << "ABDELLA There are " << id_list.size() << "containers in the header\n");
    forall_items(id_li, id_list) {
      // Lookup PTSE by id, originator in ptse_list
      seq_item ptse_si;
      ReqContainer * rRcont = id_list.inf(id_li); 

      DIAG("sim.emul", DIAG_DEBUG,
	   cout << "ABDELLA REQUESTS AN ID= " << rRcont->_ptse_id << endl);
      DBKey hiskey(InfoGroup::ig_unknown_id, tempOrigin, rRcont->_ptse_id, 0, 0, 0);

      if (ptse_si = _map.lookup(&hiskey)) {
        DBKey * dbk = _map.key(ptse_si);
        if (!ptsp)
          ptsp = new PTSPPkt(tempOrigin->GetNID(), (u_char *)dbk->GetPeer());

	ig_ptse * ptse = _pq.inf(_map.inf(ptse_si));
        ptsp->AddPTSE((ig_ptse *)(ptse->copy()));
      }
      // Added as shown below Aug 2 - Sandeep
      else if (ptse_si = _death_row.lookup(&hiskey)) {
        DBKey * dbk = _death_row.key(ptse_si);
        if (!ptsp)
          ptsp = new PTSPPkt(tempOrigin->GetNID(), (u_char *)dbk->GetPeer());
        ig_ptse * ptse = _death_row.inf(ptse_si);
        ptsp->AddPTSE((ig_ptse *)(ptse->copy()));
      } else {
	diag("fsm.database", DIAG_WARNING, 
             "Received request for PTSE which is not in our Database.\n");
	if (ptsp_list) {
	  /// Must return as this is a BadPtse request.
	  /// Clear everything before you leave
	  forall_items(li, *(ptsp_list)) {
	    PTSPPkt * lPtsp = ptsp_list->inf(li);
	    delete lPtsp;
	  }
	  ptsp_list->clear();
	}
	if (ptsp)
	  delete ptsp;

	DIAG("fsm.database", DIAG_DEBUG, cout << "Bad PTSE request, returning from Database.\n";
	     Print(cout));
	return 0;
      }
    }
    delete (NodeID *)tempOrigin;

    // If there are actually PTSEs in the PTSP then add it to the list.
    if (ptsp) { 
      nreqs++;
      if (!ptsp_list)
        ptsp_list = new list<PTSPPkt *>;
      ptsp_list->append(ptsp);
      ptsp = 0;
    }
  }

  if (!nreqs)
    return 0;
  return ptsp_list;
}

// Returns the nodalIG of the specified node
ig_nodal_info_group * Database::GetNodalIG(const NodeID * target) const
{
  ig_nodal_info_group * rval = 0;

  seq_item map_si;
  forall_items(map_si, _map) {
    DBKey * dbk = _map.key(map_si);
    ig_ptse * ptse = _pq.inf(_map.inf(map_si));

    if ((dbk->has_same_originator(target)) &&
	ptse && ptse->GetType() == InfoGroup::ig_nodal_info_group_id) {
      const list<InfoGroup *> * ig_list = ptse->ShareMembers();

      list_item li;
      forall_items(li, *ig_list) {
	InfoGroup * ig = ig_list->inf(li);
	if (ig->GetId() == InfoGroup::ig_nodal_info_group_id) {
	  rval = (ig_nodal_info_group *)ig;
	  break;
	}
      }
    }
  }
  return rval;
}

// Returns the Node Hierarchy List for the specified originator
ig_nodal_hierarchy_list * Database::GetNHL(const NodeID * originator) const
{
  ig_nodal_hierarchy_list * rval = 0;
  ig_nodal_info_group * nodal_ig = GetNodalIG(originator);

  // Iterate over all of our nodal IGs to generate the correct NHL
  while (nodal_ig) {
    const ig_next_hi_level_binding_info * nhlbi = nodal_ig->GetNextHigherLevel();
    if (!nhlbi) break;

    if (!rval) rval = new ig_nodal_hierarchy_list(GetNHLSeqNum(originator));

    NodeID * parent = (NodeID *)nhlbi->GetParentLogGroupID();
    Addr   * p_addr = (Addr *)parent->GetAddr();
    PeerID * p_peer = (PeerID *)nhlbi->GetParentPeerGroupID();
    // Add the next higher level parent to the NHL
    rval->AddLevel(parent, p_addr, p_peer);
    nodal_ig = GetNodalIG(parent);

    delete parent; delete p_addr; delete p_peer;
  }
  
  return rval;
}

int Database::GetNHLSeqNum(const NodeID * originator) const
{
  int rval = -1;
  ig_ptse * ptse = 0;
  const u_char * origin = originator->GetNID();

  seq_item map_si;
  forall_items(map_si, _map) {
    if ((_map.key(map_si)->has_same_originator(origin)) &&
	(ptse = _pq.inf(_map.inf(map_si))) &&
	(ptse->GetType() == InfoGroup::ig_nodal_hierarchy_list_id)) {
      const list<InfoGroup *> * ig_list = ptse->ShareMembers();

      list_item li;
      forall_items(li, * ig_list) {
	InfoGroup * ig = ig_list->inf(li);
	if (ig->GetId() == InfoGroup::ig_nodal_hierarchy_list_id) {
	  ig_nodal_hierarchy_list * nhl = 
	    (ig_nodal_hierarchy_list *)ig;
	  rval = nhl->GetSequenceNum();
	  break;
	}
      }
    }
  }
  return rval;
}

// You own the list, but NOT it's contents, so call rval->clear(), then delete rval.
list<ig_uplinks *> * Database::GetUplinks(int level, 
					  const NodeID * upNode, 
					  int aggToken) const
{
  list<ig_uplinks *> * rval = 0;

  list<ig_uplinks *> * tmp = GetULIG();

  if (tmp) {
    list_item li;
    forall_items(li, *tmp) {
      ig_uplinks * ulig = tmp->inf(li);

      if (ulig->GetAggTok() == aggToken &&
	  upNode->equals(ulig->GetRemoteID())) {
	if (!rval) rval = new list<ig_uplinks *>;
	rval->append(ulig);
      }
    }
  }
  return rval;
}

// You don't own the returned list or its contents, don't f**k it up
list<ig_horizontal_links *> * Database::GetHLinks(const NodeID * originator, 
						  int local_lport) const
{
  list<ig_horizontal_links *> * rval = 0;

  seq_item map_si;
  forall_items(map_si, _map) {
    ig_ptse * ptse = _pq.inf(_map.inf(map_si));

    if (ptse && (ptse->GetType() == InfoGroup::ig_horizontal_links_id) &&
	(_map.key(map_si)->has_same_originator(originator->GetNID()))) {
      const list<InfoGroup *> * ig_list = ptse->ShareMembers();
      assert(ig_list);

      list_item ig_listitem;
      forall_items(ig_listitem, *ig_list) {
	InfoGroup * igptr = ig_list->inf(ig_listitem);
	if (igptr->GetId() == InfoGroup::ig_horizontal_links_id) {
	  ig_horizontal_links * hl = (ig_horizontal_links *)igptr;

	  if ((local_lport == -1) || (local_lport == hl->GetLocalPID())) {
	    if (!rval) rval = new list<ig_horizontal_links *>;
	    rval->append(hl);
	  }
	} // end if (igptr->GetId() == InfoGroup::ig_horizontal_links_id) {
      } // end forall_items(ig_listitem, *ig_list) {
    } // end if
  } // end forall
  return rval;
}

// if remNode is null returns all uplinks in DB
list<ig_uplinks *> * Database::GetULIG(const NodeID * remNode) const
{
  list<ig_uplinks *> * rval = 0;
  const u_char * origin = _LocNodeID->GetNID();

  seq_item map_si;
  forall_items(map_si, _map) {
    ig_ptse * ptse = _pq.inf(_map.inf(map_si));

    if (ptse && ptse->GetType() == InfoGroup::ig_uplinks_id &&
	(!remNode || (_map.key(map_si)->has_same_originator(origin)))) {
      list<InfoGroup *> * ig_list = (list<InfoGroup *> *)ptse->ShareMembers();
      assert(ig_list);

      if (ig_list->size() != 1) {
	diag("fsm.database", DIAG_WARNING, "Uplinks IG PTSE's list of IG's may be fookered!\n");
      }
      list_item ig_listitem;
      forall_items(ig_listitem, *ig_list) {
	InfoGroup * igptr = ig_list->inf(ig_listitem);
	if (igptr->GetId() == InfoGroup::ig_uplinks_id) {
	  ig_uplinks * upl = (ig_uplinks *)igptr;

	  if (!remNode || (remNode && upl->GetRemoteID()->equals(remNode))) {
	    if (!rval) rval = new list<ig_uplinks *>;
	    rval->append(upl);
	  }
	} // end if (igptr->GetId() == InfoGroup::ig_uplinks_id) {
      } // end forall_items(ig_listitem, *ig_list) {
    } // end if
  } // end forall
  return rval;
}

// You do not own the returned ulia
ig_uplink_info_attr * Database::GetULIA(const NodeID * origin_obj) const
{
  const u_char * origin = origin_obj->GetNID();

  seq_item map_si;
  forall_items(map_si, _map) {
    ig_ptse * ptse = 0;
    if ((_map.key(map_si)->has_same_originator(origin)) &&
	(ptse = _pq.inf(_map.inf(map_si))) &&
	(ptse->GetType() == InfoGroup::ig_uplinks_id)) {
      
      list<InfoGroup *> * ig_list = (list<InfoGroup *> *)ptse->ShareMembers();
      assert(ig_list);

      if (ig_list->size() != 1) {
	diag("fsm.database", DIAG_WARNING, "Uplinks IG PTSE's list of IG's may be fookered!\n");
      }
      list_item ig_listitem;
      forall_items(ig_listitem, *ig_list) {
	InfoGroup * igptr = ig_list->inf(ig_listitem);
	if (igptr->GetId() == InfoGroup::ig_uplinks_id) {
	  ig_uplinks * upl = (ig_uplinks *)igptr;

	  const list<InfoGroup *> * upl_list = upl->ShareIGs();
	  assert(upl_list);

	  list_item upl_li;
	  forall_items(upl_li, *upl_list) {
	    igptr = upl_list->inf(upl_li);
	    if (igptr->GetId() == InfoGroup::ig_uplink_info_attr_id) {
	      ig_uplink_info_attr * rval = (ig_uplink_info_attr *)igptr;
	      // There should only be ONE NHL per switch
	      return rval;
	    } // end if (igptr->GetId() == InfoGroup::ig_uplink_info_attr_id) {
	  } // end forall_items(upl_li, *upl_list) {
	} // end if (igptr->GetId() == InfoGroup::ig_uplinks_id) {
      } // end forall_items(ig_listitem, *ig_list) {
    } // end if
  } // end forall
  return 0;
}

// Returns the IG containing the specified link.
InfoGroup * Database::GetLink(InfoGroup::ig_id id, NodeID * remotenode, PeerID * commonpid, 
			       int localport, int remoteport, int agg)
{
  ig_ptse * ptse = 0;
  seq_item map_si;

  // Iterate through all of the PTSEs in the Database
  forall_items(map_si, _map) {
    if ((ptse = _pq.inf(_map.inf(map_si))) &&
	(ptse->GetType() == id)) {
      
      const list<InfoGroup *> * ig_list = ptse->ShareMembers();
      
      list_item ig_listitem;
      // iterate through all of the IGs within the PTSE
      forall_items(ig_listitem, *ig_list) {
	InfoGroup * igptr = ig_list->inf(ig_listitem);
	if (igptr->GetId() == InfoGroup::ig_lgn_horizontal_link_ext_id) {
	  ig_lgn_horizontal_link_ext * hle = (ig_lgn_horizontal_link_ext *)igptr;

	  list<ig_lgn_horizontal_link_ext::HLinkCont *> hle_list = hle->GetLinks();
	  
	  list_item hle_li;
	  // Iterate through the containers within the HLE
	  forall_items(hle_li, hle_list) {
	    ig_lgn_horizontal_link_ext::HLinkCont * hptr = 
	      hle_list.inf(hle_li);
	    if ((hptr->_aggregation_token == agg) &&
		(hptr->_local_lgn_port == localport) &&
		(hptr->_remote_lgn_port == remoteport))
	      return igptr;
	  }
	} else if (igptr->GetId() == InfoGroup::ig_horizontal_links_id) {
	  ig_horizontal_links * hl = (ig_horizontal_links *)igptr;

	  if ((hl->GetRemoteID()->equals(remotenode)) &&
	      (hl->GetRemotePID() == remoteport) &&
	      (hl->GetLocalPID() == localport) &&
	      (hl->GetAggTok() == agg))
	    return igptr;
	} else if (igptr->GetId() == InfoGroup::ig_uplinks_id) {
	  ig_uplinks * ups = (ig_uplinks *)igptr;

	  if ((ups->GetRemoteID()->equals(remotenode)) &&
	      (*(ups->GetCommonPGID()) == *commonpid) &&
	      (ups->GetLocalPID() == localport) &&
	      (ups->GetAggTok() == agg))
	    return igptr;
	} else
	  continue;
      }
    }
  }
  return 0;
}


// Forceably Expire all PTSE containing horizontal link info from
// local to remote
void Database::StripLinks(NodeID * origin_obj, int port, NodeID * neighbor_obj, int agg) 
{
  const u_char * origin = origin_obj->GetNID();
  const u_char * neighbor = neighbor_obj->GetNID();

  seq_item map_si;
  forall_items(map_si, _map) {
    if (_map.key(map_si)->has_same_originator(origin)) {
      ig_ptse * ptse = _pq.inf(_map.inf(map_si));

      // Either Uplink or Hlink
      if (ptse->GetType() == InfoGroup::ig_horizontal_links_id || 
	  ptse->GetType() == InfoGroup::ig_uplinks_id)
	Expire(ptse);
    }
  }
  return;
}

// ----------------------------------
State * Database::Handle(Visitor * v)
{
  if (v->GetType().Is_A(_npflood_type))
    Insert((NPFloodVisitor *) v);
  PassThru(v);
  return this;
}

const NodeID * Database::GetNID(void) const { return _LocNodeID; }

// Find the DBKey of the corresponding ptse by pointer not by
// ig_ptse::equals
const DBKey * Database::lookup(const ig_ptse * ptse)
{
  DBKey * rval = 0;

  seq_item si;
  forall_items(si, _map) {
    // if (ptse->equals(_pq.inf(_map.inf(si)))) // you die if you uncomment
    if (ptse == (_pq.inf(_map.inf(si)))) {
      rval = new DBKey( *_map.key(si) );
      break;
    }
  }
  return rval;
}

ostream & operator << (ostream & os, const Database & db)
{
  db.Print(os);
  return os;
}

void Database::Print(ostream & os) const
{
  seq_item mi;
  os << "-------------- Database Contents --------------" << endl;
  forall_items(mi, _map) {
    pq_item pqi = _map.inf(mi);
    p_queue<double, ig_ptse *> & pq = (p_queue<double,ig_ptse *> &)_pq;
    int lsize = pq.inf(pqi)->ShareMembers()->size();
    ig_ptse * ptse = pq.inf(pqi);
    assert(ptse);
    os << "PTSE (" << ptse << ") -- (valid until " << pq.prio(pqi) 
       << ") -- " << *(_map.key(mi)) << "[" << lsize << "]" << endl
       << *ptse << endl;
  }
  os << "Total: " << _map.size() << endl;
}

void Database::Interrupt(SimEvent * e)
{
  if (e->GetCode() == DATABASE_EVENT) {
    list_item li;
    // MUTEX on _pending_floods
    forall_items(li, _pending_floods) {
      FloodContainer * fc = _pending_floods.inf(li);
      NodeID originator(fc->_ptsp->GetOID());
      FloodPTSE(fc->_ptsp, fc->_rNID, originator.GetLevel());
      delete fc;
    }
    _pending_floods.clear();
    // End MUTEX
  } else // The Database doesn't appreciate being interrutped.
    delete e;
}

void Database::RegisterTimer(TimerHandler * th)
{
  SimEntity::Register(th);
}

void Database::FlushDeadPTSES(void) 
{
  seq_item si;
  forall_items(si, _death_row) {
    ig_ptse * ptse = _death_row.inf(si);
    if (ptse->GetRefs() == 0)
      _death_row.del_item(si);
  }
}

//
// How often to refresh a PTSE
//
double Database::PTSERefreshInterval(void) const
{  return _PTSERefreshInterval;  }

//
// The time to live of any PTSE
//
sh_int Database::PTSELifetime(void) const
{  return (sh_int) (_PTSELifetimeFactor * _PTSERefreshInterval);  }

// 
void Database::HandleUpLinks(const NodeID  * originator,
                             const ig_ptse * ptse,
			     bool insert_mode)
{
  assert(ptse);
  const list<InfoGroup *> * members = ptse->ShareMembers();
  assert((members) && 
	 (members->empty() == false));

  // If I am PGL at the same level as the originator
  if (AmILeader((int)(originator->GetLevel()))) {
    if (insert_mode) {
      // insert_mode
      //
      // I am leader in the PG of the originator of this uplink...
      // we are inserting a PTSE into the Database.
      //

      int old_style = 0;  // Change to 1 if you'd like to gamble with the old code

      list_item li;
      forall_items(li, *members) {
	ig_uplinks * ulig = (ig_uplinks *)members->inf(li);
	const NodeID * upnode = ulig->GetRemoteID();
	
	int properLevel = theConfigurator().NextLevelUp( _key, originator->GetLevel() );
	//   If the upnode level is not equal to my next level
	if (upnode->GetLevel() != properLevel) { 

	  if (old_style) {
	    //     originate an uplinks ig at the next level.
	    int child_level = theConfigurator().NextLevelDown( _key, properLevel );
	    if ( child_level == -1 )
	      child_level = 160;
	    NodeID * next_level = new NodeID( properLevel, child_level, _LocNodeID );
	    
	    ig_uplinks * ins_ulig = (ig_uplinks *)ulig->copy();
	    int sequence_num = 2;
	    dic_item di;
	    if (di = _nid2seq.lookup(next_level)) {
	      sequence_num = _nid2seq.inf(di) + 2;
	      _nid2seq.change_inf(di, sequence_num);
	    } else
	      _nid2seq.insert(next_level, sequence_num);
	    
	    ig_ptse * ins_ptse = new ig_ptse(InfoGroup::ig_uplinks_id,
					     ptse->GetID(),
					     sequence_num);
	    PTSPPkt * ins_ptsp = new PTSPPkt(next_level->GetNID(), 
					     next_level->GetPID());
	    
	    ins_ptse->AddIG(ins_ulig);
	    ins_ptsp->AddElement(ins_ptse);
	    // This will cause this method to be called again
	    Insert(ins_ptsp);
	  }
	  
	  //   else if upnode level is equal to my next level
	} else if (upnode->GetLevel() == properLevel) {
	  //     setup an SVCC to the remote node
	  int child_lev = originator->GetLevel();
	  int new_level = theConfigurator().NextLevelUp( _key, child_lev );
	  NodeID * SourceNID = new NodeID( new_level, child_lev, _LocNodeID );

	  // See page 60 of af-pnni-0055.000 item A.3
	  FastUNIVisitor * fuv 
	    = new FastUNIVisitor(SourceNID, (NodeID *)upnode, 
				 0, 0x05, ulig->GetAggTok(), 0, 
				 FastUNIVisitor::FastUNISetup, 0,
				 SVCCBandwidthReq);
	  fuv->SetInPort(0);
	  fuv->SetBorderPort(ulig->GetLocalPID());
	  fuv->SetBorder(originator->copy());
	  fuv->SetForSVCC(true);
	  
	  InducedUplink * Key = 
	    new InducedUplink((const NodeID *)originator, 
			      (int)ulig->GetLocalPID());
	  
	  _uplinks.insert(Key, upnode->copy());

	  if (*SourceNID < *upnode) {
	    // our NodeID is smaller
	    dic_item di;
	    if ( (di = _SVCCStates.lookup( (NodeID*)upnode )) &&
		( _SVCCStates.inf( di ) != false ))  // SVCC has already been setup
	      fuv->SetMSGType(FastUNIVisitor::FastUNILHI);
	    else {  // SVCC hasn't been setup yet
	      fuv->Suicide();
	      fuv = 0;
	    }
	  }
	  if ( fuv != 0 )
	    PassVisitorToA(fuv);

	  delete SourceNID;
	}
	delete upnode;
      }
    } else {
      // ### remove_mode
      //
      // I originated this PTSE, but 
      // we are expiring a PTSE from the Database.
      // MUST CHECK: we "were" leader at  some point...
      
      // NOTE: the reason this code is complicated is because
      // many uplinks to the same remote upnode/aggtok
      // may be being expired in this PTSE.
      
      int level = (int)(originator->GetLevel());
      
      list<ig_uplinks *> victims;  // ultimately, LGNHs that must die
      list_item li;
      forall_items(li, *members) {
	// Consider each uplink that is expiring
	ig_uplinks * ulig = (ig_uplinks *)members->inf(li);
	const NodeID * upnode = ulig->GetRemoteID();
	int aggToken = ulig->GetAggTok();
	
	// How many other equivalent uplinks are leaving in this PTSE?
	list_item li_rem;
	list<ig_uplinks *> downlist;
	forall_items(li_rem, *members) {
	  ig_uplinks * ulig_rem = (ig_uplinks *)members->inf(li);
	  const NodeID * upnode_rem = ulig_rem->GetRemoteID();
	  int aggToken_rem = ulig_rem->GetAggTok();
	  
	  if ((aggToken_rem==aggToken) &&
	      (upnode_rem->equals(upnode))) {
	    downlist.append(ulig_rem);
	  }
	  delete upnode_rem;
	}
	int num_being_removed = downlist.size();
	
	// Count the uplinks in the DB that match the departing one
	list<ig_uplinks *> * uplist = GetUplinks(level, upnode, aggToken);
	int num_in_db = uplist->size();
	
	// Does this mean the LGNH is now unwarranted?
	if (num_being_removed==num_in_db) {
	  
	  // But is it already a victim?
	  list_item chk;
	  bool redundant = false;
	  forall_items(chk, victims) {
	    ig_uplinks * ulig_chk = (ig_uplinks *)members->inf(li);
	    const NodeID * upnode_chk = ulig_chk->GetRemoteID();
	    int aggToken_chk = ulig_chk->GetAggTok();
	    if ((aggToken_chk==aggToken) &&
		(upnode_chk->equals(upnode))) {
	      redundant = true;
	      break;
	    }
	    delete upnode_chk;
	  }
	  if ( ! redundant) {
	    victims.append(ulig);
	  }
	}
	
	delete upnode;
	
      } // outermost: forall_items(li, *members)
      
      // Now we have a list of victims
      list_item obsolete_it;
      forall_items(obsolete_it, victims) {
	ig_uplinks * ulig_victim = victims.inf(obsolete_it);
	const NodeID * upnode_victim = ulig_victim->GetRemoteID();
	int aggToken_victim = ulig_victim->GetAggTok();
	
	FastUNIVisitor * fuv = new FastUNIVisitor(originator, upnode_victim,
						  0, -1 ,    /* port, vpvc */
						  aggToken_victim,
						  0,         /* cref */
						  FastUNIVisitor::FastUNILHD,
						  0,         /* dtl file */
						  (u_long) 0 /* bandwidth */ );
	delete upnode_victim;
	PassVisitorToA(fuv);
      }
    }
    // ### 
  }
}


void Database::ReinitiateSVC(const NodeID * LocNodeID, const NodeID * UpNodeID)
{
  list<ig_uplinks *> * list_uligs = GetULIG(UpNodeID);

  if (list_uligs != 0) {
    ig_uplinks * ulig = list_uligs->head();
    
    InducedUplink * key = new InducedUplink(LocNodeID, (int)(ulig->GetLocalPID()));
    
    dic_item di;
    if (di = _uplinks.lookup(key)) {
      // This uplink has already been stored
      NodeID * old_upnode = _uplinks.inf(di);
      
      if (!(old_upnode->equals(UpNodeID))) {
	_uplinks.change_inf(di, UpNodeID->copy());
	delete old_upnode;
      }
      
      FastUNIVisitor * fuv = new FastUNIVisitor((NodeID *)LocNodeID, (NodeID *)UpNodeID, 
						0, 0x05, ulig->GetAggTok(), 0, 
						FastUNIVisitor::FastUNISetup, 0,
						SVCCBandwidthReq);
      fuv->SetInPort(0);
      fuv->SetForSVCC(true);
      PassVisitorToA(fuv);
    }
  }
#if 0
 else {
    DIAG("fsm.database", DIAG_FATAL, cout << *LocNodeID << 
	 " No Uplinks IG present for " << *UpNodeID << endl);
    assert(list_uligs);
  }
#endif
}

void Database::WonElection(int level)
{
  _level.insert(level, true);
}

void Database::LostElection(int level)
{
  _level.insert(level, false);
}

bool Database::AmILeader(int level) const
{
  bool rval = false;
  ig_nodal_info_group * nig = GetNodalIG( GetMyNodeID( level ) );

  if ( nig != 0 ) {
    // We have obtained our Nodal IG forr this level, and if the leader bit is set, we won!
    rval = nig->IsSet( ig_nodal_info_group::leader_bit );
  }

  return rval;
}

// Returns a list containing all of the PTSEs of a given type from the specified originator.
// NOTE: YOU DO NOT OWN THE POINTERS IN THE LIST, SO DON'T MESS WITH THEM!!!
// You may however delete the list itself, just besure to clear it first
list<ig_ptse *> * Database::GetPTSEs(InfoGroup::ig_id type, const NodeID * originator)
{
  list<ig_ptse *> * rval = 0;

  seq_item si;
  forall_items(si, _map) {
    DBKey * dbk = _map.key(si);

    if (dbk->has_same_originator(originator)) {
      ig_ptse * ptse = _pq.inf(_map.inf(si));

      if (ptse->GetType() == type) {
	if (!rval) rval = new list<ig_ptse *> ;
	rval->append(ptse);
      }
    }
  }
  return rval;
}

// You own the list, but not its contents!!  Consider yourself warned.
list<ig_nodal_state_params *> * Database::GetNSP(const NodeID * originator)
{
  list<ig_ptse *> * tmp = GetPTSEs(InfoGroup::ig_nodal_state_params_id, originator);
  list<ig_nodal_state_params *> * rval = 0;

  if (!tmp)
    return rval;

  list_item li;
  forall_items(li, *tmp) {
    ig_ptse * p = tmp->inf(li);
    const list<InfoGroup *> * internal_list = p->ShareMembers();
    list_item lli;
    forall_items(lli, *internal_list) {
      InfoGroup * ig = internal_list->inf(lli);
      
      if (ig->GetId() == InfoGroup::ig_nodal_state_params_id) {
	if (!rval)
	  rval = new list<ig_nodal_state_params *> ;
	rval->append((ig_nodal_state_params *)ig);
      }
    }
  }
  // Clean up
  tmp->clear();
  delete tmp;

  return rval;
}

// ---------------------- OriginKey ------------------------
Database::OriginKey::OriginKey(const DBKey & key)
{ 
  memcpy(_oid, key.GetOrigin(), 22);
  memcpy(_pid, key.GetPeer(), 14);
}

Database::OriginKey::OriginKey(const OriginKey & key)
{ 
  memcpy(_oid, key._oid, 22);
  memcpy(_pid, key._pid, 14);
}

Database::OriginKey::OriginKey(const u_char * origin, const u_char * pid) 
{ 
  memcpy(_oid, origin, 22);
  if (pid) 
    memcpy(_pid, pid, 14);
  else 
    memset(_pid, 0, 14);
}

Database::OriginKey::~OriginKey() { }

Database::OriginKey Database::OriginKey::operator = (const Database::OriginKey & him)
{
  memcpy(_oid, him._oid, 22);
  memcpy(_pid, him._pid, 14);
  return *this;
}

int operator == (const Database::OriginKey & x, const Database::OriginKey & y)
{
  return memcmp(y._oid, x._oid, 22);
}

const u_char * Database::OriginKey::GetOrigin(void) const { return _oid; }
const u_char * Database::OriginKey::GetPeer(void) const   { return _pid; }

ostream & operator << (ostream & os, const Database::OriginKey & key)
{
  NodeID nid(key._oid);
  os << "OriginKey " << "OID: " << nid;
  PeerID pid(key._pid);
  os << " PID: " << pid << " ";
  return os;
}

// -------------------------------------------------------------------
// Remember to delete [] the string that MatchingLevel() returns.
const u_char * Database::MatchingLevel(const NodeID * nid)
{
  const u_char * rval = 0;
  int bits_matching = bits_equal(2 + _LocNodeID->GetNID(),
                                 2 + nid->GetNID(),
                                 _LocNodeID->GetLevel());

  ig_nodal_hierarchy_list * hlevels = GetNHL(_LocNodeID);

  if (hlevels) {
    const list<Level *> * levels = hlevels->ShareLevels();
    list_item li;
    forall_items(li, *levels) {
      NodeID * me = (levels->inf(li))->GetNID();
      if (me->GetLevel() <= bits_matching) {
	rval = new u_char[22];
	bcopy(me->GetNID(), (u_char *)rval, 22);
	delete me;
	break;
      }
    }

    delete hlevels;
  }
  return rval;
}

// returns true if we are the winner at the child PG level
bool Database::LogicalNodeIsMe(const NodeID * remNode) const
{
  bool rval = false;

  // I'll no longer tolerate pranksters!
  assert( remNode != 0 );

#if 1
  const u_char * loc = _LocNodeID->GetNID() + 2;
  const u_char * rem = remNode->GetNID() + 2;
  int level       = remNode->GetLevel();
  int lower_level = theConfigurator().NextLevelDown( _key, level );

  if ( _LocNodeID->equals( remNode ) ||  // They match EXACTLY
       (( ! memcmp( loc, rem, 19 ) ) &&  // They match 19 bytes
	( AmILeader( lower_level ))) )   //   and we are leader
    rval = true;

#else
  if ( _LocNodeID->equals( remNode ) ) {
    rval = true;
  } else {
    int level       = remNode->GetLevel();
    int lower_level = theConfigurator().NextLevelDown( _key, level );

    // changed final parameter from level to lower_level - 02/12/99 mountcas
    if ( lower_level != -1 && 
	 bitcmp( _LocNodeID->GetNID() + 2, remNode->GetNID() + 2, lower_level ) == 0 )
      rval = AmILeader( lower_level );
  }
#endif
  return rval;
}

bool Database::LogicalNodeIsMe(const int level)
{
  return LogicalNodeIsMe(GetMyNodeID(level));
}

const NodeID * Database::GetPreferredPGL(const NodeID * nid) const
{
  const NodeID * rval = 0;
  ig_nodal_info_group * nig = GetNodalIG(nid);
  if (nig)
    rval = nig->GetPreferredPGL();
  // Ummm, FYI, nig->GetPreferredPGL alloc's a NodeID, so it had better be deleted.
  return rval;
}

// You own the returned list, so please clean it up
list<NodeID *> * Database::LogicalPortToBorder(const NodeID * originator, 
					     int lport,
					     const NodeID * upnode)
{
  assert(originator);
  list<NodeID *> * rval = 0;

  // Obtain a list of [logical] horizontal HLinks with a local logical
  // port of 'lport' and originated by 'originator'
  list<ig_horizontal_links *> * h_list = GetHLinks(originator, lport);

  if (h_list->empty() == false) {
    // Obtain the aggregation token from the logical HLink
    int agg_token = (h_list->head())->GetAggTok();
    // Search DB for all uplinks with the above aggregation token
    // and remote node 'upnode'
    list<ig_uplinks *> * u_list = GetUplinks(originator->GetLevel(), upnode, agg_token);
    
    if (u_list && !u_list->empty()) {
      rval = new list<NodeID *> ;
      list_item li;

      forall_items(li, *u_list) {
	ig_uplinks * ulig = u_list->inf(li);
	rval->append( PTSEOriginator( IGtoPTSE( ulig ) )->copy() ); // ulig->GetRemoteID()->copy() );
      }
    }
  }
  return rval;
}

int Database::NewestVersion(void) const
{
  return _newest_version;
}

int Database::OldestVersion(void) const
{
  return _oldest_version;
}

// You own the return value
NodeID  * Database::PTSEOriginator(const ig_ptse * ptse) const
{
  NodeID * rval = 0;
  if (ptse) {
    // Iterate over the PQ to locate the pq_item
    pq_item pqi;
    forall_items(pqi, _pq) {
      ig_ptse * tmp = _pq.inf(pqi);
      if (ptse->equals( tmp )) {
	seq_item si;
	forall_items(si, _map) {
	  pq_item pi = _map.inf(si);
	  if (pi == pqi) {
	    DBKey * dbk = _map.key(si);
	    rval = new NodeID( dbk->GetOrigin() );
	    break;
	  }
	}
	break;
      }
    }
  }
  return rval;
}

const ig_ptse * Database::IGtoPTSE(const InfoGroup * ig) const
{
  ig_ptse * rval = 0;
  if (ig && (_pq.empty() == false)) {
    pq_item pqi;
    forall_items(pqi, _pq) {
      ig_ptse * tmp = _pq.inf(pqi);
      if (tmp->Contains(ig)) {
	rval = tmp;
	break;
      }
    }
  }
  return rval;
}

VCAllocator * Database::LHIAllocator( void ) const
{
  return _vca;
}

void Database::GenerateLHI(const NodeID * remote_node)
{
  dic_item di;
  if ( ! (di = _SVCCStates.lookup( (NodeID *)remote_node )) )
    di = _SVCCStates.insert( remote_node->copy(), false );


  if ( ! _SVCCStates.inf( di ) ) {
    _SVCCStates.change_inf( di, true );

    // obtain all uplinks referring to a particular remode node
    list<ig_uplinks *> * uligs = GetULIG( remote_node );

    if ( uligs && uligs->empty() == false ) {
      list_item li;
      forall_items(li, *uligs) {
	ig_uplinks * upl = uligs->inf( li );

	NodeID * SourceNID = (NodeID *)GetMyNodeID( remote_node->GetLevel() );
	FastUNIVisitor * fuv 
	  = new FastUNIVisitor(SourceNID, remote_node, 
			       0, 0x05, upl->GetAggTok(), 0, 
			       FastUNIVisitor::FastUNILHI, 0,
			       SVCCBandwidthReq);
	fuv->SetInPort(0);
	fuv->SetBorderPort(upl->GetLocalPID());
	fuv->SetForSVCC(true);
	assert( *SourceNID < *remote_node );
	PassVisitorToA( fuv );
      }
    }
  }
}

const DBKey * Database::ObtainOriginator( const ig_ptse * p ) const
{
  const DBKey * rval = 0;

  seq_item si;
  forall_items( si, _map ) {
    pq_item pqi = _map.inf( si );
    
    if ( _pq.inf( pqi ) == p ) {
      rval = _map.key( si );
      break;
    }
  }

  return rval;
}
