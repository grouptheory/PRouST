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
#ifndef __DATABASEINTERFACES_H__
#define __DATABASEINTERFACES_H__

#ifndef LINT
static char const _DatabaseInterfaces_h_rcsid_[] =
"$Id: DatabaseInterfaces.h,v 1.32 1999/03/05 17:30:32 marsh Exp $";
#endif

#include <FW/interface/Interface.h>
#include <DS/containers/list.h>
#include <iostream.h>

class Database;
class ig_nodal_hierarchy_list;
class ig_uplink_info_attr;
class ig_nodal_info_group;
class NodeID;
class ds_String;
class Omni;

class DatabaseInterface : public fw_Interface {
  friend class Database;
public:

  // Caller owns the pointer
  NodeID *  PhysicalNodeID(void) const;
  const int Ports(void) const;
  const int PhysicalLevel(void) const;
  const int NextLevelDown( int level ) const;
  const int NextLevelUp( int level ) const;
  const int PeerGroupLeadershipPriority( int level ) const;
  // Caller owns the pointer
  NodeID *  PreferredPeerGroupLeader( int level ) const;
  // Returns true if this switch is 'Omniscient'
  bool      Omniscient(void) const;
  // Caller owns the pointer on all of the following methods
  ds_String * ACACPluginPrefix(void) const;
  ds_String * LogosPluginPrefix(void) const;
  ds_String * AggregatorPluginPrefix(void) const;
  ds_String * ACACPluginLibrary(void) const;
  ds_String * LogosPluginLibrary(void) const;
  ds_String * AggregatorPluginLibrary(void) const;

  double    PTSERefreshInterval(void) const;
  short int PTSELifetime(void) const;
  bool      LogicalNodeIsMe(const NodeID * nid) const;
  bool      LogicalNodeIsMe(const int level) const;
  bool      AmILeader(int level) const;

  ig_nodal_hierarchy_list * GetNHL(const NodeID * origin) const;

protected:

  DatabaseInterface(Database * db);
  virtual ~DatabaseInterface();

  void ShareableDeath(void);

  Database * _db;
};

class DBHelloInterface : public DatabaseInterface {
  friend class Database;
public:

  ig_nodal_hierarchy_list * ObtainNodalHierarchyList(const NodeID * originator) const;
  ig_uplink_info_attr * ObtainUplinkInformationAttribute(const NodeID * originator) const;

  void SVCCIsUp(const NodeID * remote_node);

  // This returns true if we've won at that level, 
  // and false if we've lost OR the election hasn't taken place yet.
  bool ElectionStatusAtLevel(int level) const;

  bool LogicalNodeIsMe(const NodeID * logical_node);

  void PrintDatabaseContents(ostream & os);

  int  NewestVersionSupported(void) const;
  int  OldestVersionSupported(void) const;

  void ReinitiateSVC( const NodeID * locNode, const NodeID * upNode );

protected:

  DBHelloInterface(Database * db);
  virtual ~DBHelloInterface();
};

class DatabaseSumPkt;
class PTSEReqPkt;
class PTSPPkt;
class ig_ptse;

class DBNodePeerInterface : public DatabaseInterface {
  friend class Database;
public:

  DatabaseSumPkt  * GetDatabaseSummary(int level = 96);
  PTSEReqPkt      * ReqDiffDB(DatabaseSumPkt * dbp, NodeID * RemoteNodeID);
  list<PTSPPkt *> * FloodDiffDB(PTSEReqPkt * prp);
  ig_ptse         * ReqPTSE(NodeID * nid, ig_ptse * ptse);
  void              FlushDeadPTSES( void );

protected:

  DBNodePeerInterface(Database * db);
  virtual ~DBNodePeerInterface();
};

class DBACACInterface : public DatabaseInterface {
  friend class Database;
public:

  ig_ptse * ReqPTSE(NodeID * nid, ig_ptse * ptse);
  ig_ptse * ReqPTSE(const NodeID * originator, int id) const;
  bool      Expire(ig_ptse * ptse);

protected:

  DBACACInterface(Database * db);
  virtual ~DBACACInterface();
};

class DBLeadershipInterface : public DatabaseInterface {
  friend class Database;
public:

  void WonElection(int level);
  void LostElection(int level);
  bool Expire(ig_ptse * ptse);
  ig_ptse * ReqPTSE(NodeID * nid, ig_ptse * ptse);
  ig_nodal_info_group * GetNodalIG(const NodeID * target) const;

protected:

  DBLeadershipInterface(Database * db);
  virtual ~DBLeadershipInterface();
};

class DBKey;

class DBLogosInterface : public DatabaseInterface {
  friend class Database;
  friend class Omni;
public:

  // returns a list containing all nodal ig ptses
  list< ig_ptse * > * NodalInfoGroupList(void) const;
  // returns a list containing all hlinks and uplinks ptses
  list< ig_ptse * > * HorizontalUplinkList(void) const;
  // returns a list containing all nsp ptses
  list< ig_ptse * > * NodalStateParamsList(void) const;
  // determines the originator of a ptse
  const DBKey * ObtainOriginator( ig_ptse * p ) const;

protected:

  DBLogosInterface(Database * db);
  virtual ~DBLogosInterface();
};

#endif // __DATABASEINTERFACES_H__
