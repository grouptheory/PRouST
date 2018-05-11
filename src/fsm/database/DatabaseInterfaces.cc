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
static char const _DatabaseInterfaces_cc_rcsid_[] =
"$Id: DatabaseInterfaces.cc,v 1.30 1999/03/05 17:30:31 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include "DatabaseInterfaces.h"
#include "Database.h"
#include <codec/pnni_ig/InfoGroup.h>
#include <codec/pnni_ig/ptse.h>
#include <fsm/config/Configurator.h>

// ------------------- Default Interface ---------------------
DatabaseInterface::DatabaseInterface(Database * db)
  : fw_Interface(db), _db(db) { assert(db != 0); }

DatabaseInterface::~DatabaseInterface() { }

void DatabaseInterface::ShareableDeath(void)
{ 
  _db = 0; // assert( !good() ); 
}

NodeID * DatabaseInterface::PhysicalNodeID(void) const
{
  NodeID * rval = 0;
  if ( _db->GetNID() != 0 )
    rval = _db->GetNID()->copy();
  return rval;
}

const int DatabaseInterface::Ports(void) const
{
  return theConfigurator().Ports( _db->_key );
}

const int DatabaseInterface::PhysicalLevel(void) const
{
  return theConfigurator().PhysicalLevel( _db->_key );
}

const int DatabaseInterface::NextLevelDown( int level ) const
{
  return theConfigurator().NextLevelDown( _db->_key, level );
}

const int DatabaseInterface::NextLevelUp( int level ) const
{
  return theConfigurator().NextLevelUp( _db->_key, level );
}

const int DatabaseInterface::PeerGroupLeadershipPriority( int level ) const
{
  return theConfigurator().PeerGroupLeadershipPriority( _db->_key, level );
}

NodeID *  DatabaseInterface::PreferredPeerGroupLeader( int level ) const
{
  return theConfigurator().PreferredPeerGroupLeader( _db->_key, level );
}

bool      DatabaseInterface::Omniscient(void) const
{
  return theConfigurator().Omniscient( _db->_key );
}

ds_String * DatabaseInterface::ACACPluginPrefix(void) const
{
  return theConfigurator().ACACPluginPrefix( _db->_key );
}

ds_String * DatabaseInterface::LogosPluginPrefix(void) const
{
  return theConfigurator().LogosPluginPrefix( _db->_key );
}

ds_String * DatabaseInterface::AggregatorPluginPrefix(void) const
{
  return theConfigurator().AggregatorPluginPrefix( _db->_key );
}

ds_String * DatabaseInterface::ACACPluginLibrary(void) const
{
  return theConfigurator().ACACPluginLibrary( _db->_key );
}

ds_String * DatabaseInterface::LogosPluginLibrary(void) const
{
  return theConfigurator().LogosPluginLibrary( _db->_key );
}

ds_String * DatabaseInterface::AggregatorPluginLibrary(void) const
{
  return theConfigurator().AggregatorPluginLibrary( _db->_key );
}

double    DatabaseInterface::PTSERefreshInterval(void) const
{
  return _db->PTSERefreshInterval();
}

short int DatabaseInterface::PTSELifetime(void) const
{
  return _db->PTSELifetime();
}

bool      DatabaseInterface::LogicalNodeIsMe(const NodeID * nid) const
{
  return _db->LogicalNodeIsMe( nid );
}

bool      DatabaseInterface::LogicalNodeIsMe(const int level) const
{
  return _db->LogicalNodeIsMe( level );
}

bool      DatabaseInterface::AmILeader(int level) const
{
  return _db->AmILeader( level );
}

ig_nodal_hierarchy_list * DatabaseInterface::GetNHL(const NodeID * origin) const
{
  return _db->GetNHL( origin );
}

// ------------------- Hello Interface ---------------------
DBHelloInterface::DBHelloInterface(Database * db) 
  : DatabaseInterface(db) { }

DBHelloInterface::~DBHelloInterface() { }

ig_nodal_hierarchy_list * 
DBHelloInterface::ObtainNodalHierarchyList(const NodeID * originator) const
{
  return _db->GetNHL(originator);
}

ig_uplink_info_attr * 
DBHelloInterface::ObtainUplinkInformationAttribute(const NodeID * originator) const
{
  return _db->GetULIA(originator);
}

void DBHelloInterface::SVCCIsUp(const NodeID * remote_node)
{
  _db->GenerateLHI(remote_node);
}

bool DBHelloInterface::ElectionStatusAtLevel(int level) const
{
  return _db->AmILeader(level);
}

bool DBHelloInterface::LogicalNodeIsMe(const NodeID * logical_node)
{
  return _db->LogicalNodeIsMe(logical_node);
}

void DBHelloInterface::PrintDatabaseContents(ostream & os)
{
  _db->Print(os);
}

int  DBHelloInterface::NewestVersionSupported(void) const
{
  return _db->NewestVersion();
}

int  DBHelloInterface::OldestVersionSupported(void) const
{
  return _db->OldestVersion();
}

void DBHelloInterface::ReinitiateSVC( const NodeID * locNode, const NodeID * upNode )
{
  _db->ReinitiateSVC( locNode, upNode );
}

// ------------------- NodePeer Interface ---------------------
DBNodePeerInterface::DBNodePeerInterface(Database * db)
  : DatabaseInterface(db) { }

DBNodePeerInterface::~DBNodePeerInterface() { }

DatabaseSumPkt * DBNodePeerInterface::GetDatabaseSummary(int level)
{
  return _db->GetDatabaseSummary(level);
}

PTSEReqPkt * DBNodePeerInterface::ReqDiffDB(DatabaseSumPkt * dbp, NodeID * RemoteNodeID)
{
  return _db->ReqDiffDB(dbp, RemoteNodeID);
}

list<PTSPPkt *> * DBNodePeerInterface::FloodDiffDB(PTSEReqPkt * prp)
{
  return _db->FloodDiffDB(prp);
}

ig_ptse * DBNodePeerInterface::ReqPTSE(NodeID * nid, ig_ptse * ptse)
{
  return _db->ReqPTSE(nid, ptse);
}

void      DBNodePeerInterface::FlushDeadPTSES( void )
{
  _db->FlushDeadPTSES( );
}

// ------------------- ACAC Interface ---------------------
DBACACInterface::DBACACInterface(Database * db)
  : DatabaseInterface( db ) { }

DBACACInterface::~DBACACInterface() { }

ig_ptse * DBACACInterface::ReqPTSE(NodeID * nid, ig_ptse * ptse)
{
  return _db->ReqPTSE( nid, ptse );
}

ig_ptse * DBACACInterface::ReqPTSE(const NodeID * originator, int id) const
{
  return 0;
}

bool      DBACACInterface::Expire(ig_ptse * ptse)
{
  return _db->Expire( ptse );
}

// ------------------- Leadership Interface ---------------------
DBLeadershipInterface::DBLeadershipInterface(Database * db)
  : DatabaseInterface( db ) { }

DBLeadershipInterface::~DBLeadershipInterface() { }

void DBLeadershipInterface::WonElection(int level)
{
  _db->WonElection( level );
}

void DBLeadershipInterface::LostElection(int level)
{
  _db->LostElection( level );
}

bool DBLeadershipInterface::Expire(ig_ptse * ptse)
{
  return _db->Expire( ptse );
}

ig_ptse * DBLeadershipInterface::ReqPTSE(NodeID * nid, ig_ptse * ptse)
{
  return _db->ReqPTSE( nid, ptse );
}

ig_nodal_info_group * DBLeadershipInterface::GetNodalIG(const NodeID * target) const
{
  return _db->GetNodalIG( target );
}

// ------------------- Logos Interface ---------------------
DBLogosInterface::DBLogosInterface(Database * db) : DatabaseInterface( db ) { }

DBLogosInterface::~DBLogosInterface() { }

list< ig_ptse * > * DBLogosInterface::NodalInfoGroupList(void) const
{
  list< ig_ptse * > * rval = 0;

  seq_item si;
  forall_items( si, _db->_map ) {
    pq_item pqi = _db->_map.inf( si );
    assert( pqi != 0 );
    ig_ptse * ptse = _db->_pq.inf( pqi );
    assert( ptse != 0 );
    if ( ptse->GetType() == InfoGroup::ig_nodal_info_group_id ) {
      if ( ! rval )
	rval = new list< ig_ptse * > ;
      rval->append( ptse );
    }
  }

  return rval;
}

list< ig_ptse * > * DBLogosInterface::HorizontalUplinkList(void) const
{
  list< ig_ptse * > * rval = 0;

  seq_item si;
  forall_items( si, _db->_map ) {
    pq_item pqi = _db->_map.inf( si );
    assert( pqi != 0 );
    ig_ptse * ptse = _db->_pq.inf( pqi );
    assert( ptse != 0 );
    if ( ptse->GetType() == InfoGroup::ig_horizontal_links_id || 
	 ptse->GetType() == InfoGroup::ig_uplinks_id ) {
      if ( ! rval )
	rval = new list< ig_ptse * > ;
      rval->append( ptse );
    }
  }

  return rval;
}

list< ig_ptse * > * DBLogosInterface::NodalStateParamsList(void) const
{
  list< ig_ptse * > * rval = 0;

  seq_item si;
  forall_items( si, _db->_map ) {
    pq_item pqi = _db->_map.inf( si );
    assert( pqi != 0 );
    ig_ptse * ptse = _db->_pq.inf( pqi );
    assert( ptse != 0 );
    if ( ptse->GetType() == InfoGroup::ig_nodal_state_params_id ) {
      if ( ! rval )
	rval = new list< ig_ptse * > ;
      rval->append( ptse );
    }
  }

  return rval;
}

const DBKey * DBLogosInterface::ObtainOriginator( ig_ptse * p ) const
{
  return _db->ObtainOriginator( p );
}
