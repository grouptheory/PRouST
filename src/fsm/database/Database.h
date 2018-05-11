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
#ifndef __DATABASE_H__
#define __DATABASE_H__

#ifndef LINT
static char const _Database_h_rcsid_[] =
"$Id: Database.h,v 1.136 1999/02/19 21:22:45 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include <DS/containers/p_queue.h>
#include <DS/containers/h_array.h>
#include <DS/containers/sortseq.h>
#include <DS/containers/dictionary.h>
#include <DS/containers/list.h>
#include <DS/util/String.h>
#include <FW/actors/State.h>
#include <codec/pnni_ig/InfoGroup.h>
#include <codec/pnni_ig/id.h>

class NodeID;
class PeerID;
class Kernel;
class TimerHandler;
class DatabaseEvent;
class VCAllocator;

class NPFloodVisitor;
class PTSPPkt;
class DatabaseSumPkt;
class PTSEReqPkt;

class ig_nodal_state_params;
class ig_nodal_hierarchy_list;
class ig_nodal_info_group;
class ig_horizontal_links;
class ig_uplink_info_attr;
class ig_uplinks;
class ig_ptse;

class InducedUplink;
class DatabaseInterface;

#include <codec/pnni_ig/DBKey.h>

/**  Database

     As the name implies the Database is the Information Group
     repository of the switch.  The Database contains all the
     information necessary to determine the network topology from this
     particular switch's point of view.  It provides methods for
     querying specific information, and for inserting new information.
     The Database maintains a priority queue based on the lifetime of
     the information so it can quickly eject stale information.
 */
class Database : public State {
  friend class ExecutionerPTSE;
  friend class DatabaseInterface;
  friend class DBLogosInterface;
  friend ostream & operator << (ostream & os, const Database & rhs);
  friend Conduit * InitDB(FILE * fp, const char * dbConduitName, 
			  Database *& db, NodeID *& myNodeID, 
			  NodeID *& myParentNodeID,
			  NodeID *& myPreferredPeerGroupLeader,
			  bool more, int level);
  friend Conduit * ExtInitDB(FILE * fp, const char * dbConduitName, 
			     Database *& db, NodeID *& myNodeID, 
			     NodeID *& myParentNodeID,
			     NodeID *& myPreferredPeerGroupLeader);
  friend void _parse_origin_block(char * linebuf, Database * db, NodeID *& myPrefPGL, FILE * fp);
public:

  static const u_long SVCCBandwidthReq = 12500;

  /// Constructor takes our key and NodeID
  Database(ds_String * key, const NodeID * ournid = 0);
  /// Destructor
  virtual ~Database(void);

  /// Returns all PTSEs from the passed Originator, BUT REMEMBER to 
  ///    immediately Reference() (and UnReference() when done with) all the 
  ///    PTSEs in the list immediately, or they may become invalid pointers!
  const list<ig_ptse *> * GetPTSEListByOrigin(NodeID *orig);

  /// Compute the current summary
  DatabaseSumPkt  * GetDatabaseSummary(int level = 96);

  /// Compute Requests on the basis of the difference 
  /// between our DB and the passed summary dbp
  PTSEReqPkt      * ReqDiffDB(DatabaseSumPkt * dbp, NodeID * RemoteNodeID);

  /// Generates PTSPs in response to Request prp
  list<PTSPPkt *> * FloodDiffDB(PTSEReqPkt * prp);

  /// Expire the PTSE (if it is in the DB) 
  /// according to 5.8.3.8 II - IV (NPeerFSM does part I)
  bool              Expire(ig_ptse * ptse);

  /// Query DB to see if any version of a PTSE is already there.
  ///   BUT REMEMBER to Reference() and UnReference() the returned value.
  ///   NPeerFSM uses this method to check if an incoming PTSE is already in the DB
  ///   This method returns 0 if there is no occurence in the DB.
  ig_ptse         * ReqPTSE(NodeID * nid, ig_ptse * ptse);

  /// Send the PTSE to all NPeerFSMs, 
  ///   if rep is non-zero they MUST replace rep in their *Lists with src
  void              FloodPTSE(PTSPPkt * lPtsp, NodeID * RemoteNodeID = 0L, int level = 96);

  /// Returns the NodalIG of the specified Node if it's in the Database
  ig_nodal_info_group * GetNodalIG(const NodeID * target) const;

  /// return nodal hierarchy list for the passed originator
  ig_nodal_hierarchy_list * GetNHL(const NodeID * origin) const;
  
  int GetNHLSeqNum(const NodeID * originator) const;

  /// returns the ULIA for the specified originator
  ig_uplink_info_attr * GetULIA(const NodeID * origin) const;

  /// returns all the Uplinks in our PG at the specified level, 
  ///   that have the specified UpNode and aggregation token.
  list<ig_uplinks *> * GetUplinks(int level, const NodeID * upNode, int aggToken) const;

  /// returns all the ULIGs between us and the remote node
  list<ig_uplinks *> * GetULIG(const NodeID * remNode = 0) const;

  /// returns all the Horizontal Links originated by originator (and optionally with the local_lport)
  list<ig_horizontal_links *> * GetHLinks(const NodeID * originator, int local_lport = -1) const;

  /// returns list of border nodes that match the logical port
  list<NodeID *> * LogicalPortToBorder(const NodeID * originator, int lport, const NodeID * upnode);

  /// returns all the Nodal State Params from the specified originator;
  list<ig_nodal_state_params *> * GetNSP(const NodeID * originator);

  /// Returns a list of all the PTSEs of the specified type from the specified originator
  list<ig_ptse *> * GetPTSEs(InfoGroup::ig_id type, const NodeID * originator);

  /// Returns a pointer to the IG containing the specified link.
  InfoGroup * GetLink(InfoGroup::ig_id id, NodeID * remotenode, 
		      PeerID * commonpid, int localport, int remoteport, int agg = -1);

  /// prematurely expires an Hlink or Uplink
  void                      StripLinks(NodeID * local, int localport, NodeID * remote, int agg = -1);

  /// After NP's reach FULL state flush out any ptse in death_row
  /// which has a reference of zero.
  void FlushDeadPTSES();

  /// Search for PTSE by pointer not by ig_ptse::equals
  const DBKey * lookup(const ig_ptse * ptse);

  /// Entry point for dealing with Visitors.
  State * Handle(Visitor * v);
  /// Deals with SimEvents (unused).
  void    Interrupt(SimEvent * e);
  /// Necessary to Register Timers from outside of the Database (unused).
  void    RegisterTimer(TimerHandler * th);

  /// Prints the contents of the Database to the specified ostream.
  void Print(ostream & os = cout) const;
  /// Returns the NodeID of the Database.
  const NodeID * GetNID(void) const;
  /// Dumps checkpointing information to the specified file so the Database can continue where it left off.
  int DumpCheckpoint(char * filename);
  /// Dumps checkpointing information to the specified ostream which enables the Database to continue after a crash.
  int DumpCheckpoint(ostream & os);

  /// Returns the default PTSE Refresh Interval
  double    PTSERefreshInterval(void) const;
  /// Returns the default PTSE Lifetime.
  short int PTSELifetime(void) const;

  /// Notifies the Database that this switch has won the election at a particular level.
  void      WonElection(int level);
  /// Notifies the Database that this switch has lost the election at a particular level.
  void      LostElection(int level);
  /// Returns true if this switch won the election at the specified level.
  bool      AmILeader(int level) const; 
  /// Returns true if the specified NodeID is this switch at some level.
  bool      LogicalNodeIsMe(const NodeID * logical_node) const;
  /// Returns true if we are the winner at the specified level.
  bool      LogicalNodeIsMe(const int level);
  /// returns a ptr to the level and child level at which the nid matches our NodeID
  const u_char *  MatchingLevel(const NodeID * nid);
  /// Returns a pointer to this switch's preferred Peer Group Leader.
  const NodeID * GetPreferredPGL(const NodeID * nid) const;
  /// Used to reinitiate the SVCC after it has gone down.
  void ReinitiateSVC(const NodeID * LocNodeID, const NodeID * UpNodeID);

  int NewestVersion(void) const;
  int OldestVersion(void) const;

  VCAllocator * LHIAllocator(void) const;
  void GenerateLHI(const NodeID * remote_node);

  // 
  // af-pnni-0055.00, page 254: "This is the time in seconds between
  // reoriginations of a self originated PTSE in the absence of
  // triggered updates.  A node will reoriginate its PTSEs at this
  // rate in order to prevent flushing of these PTSEs by other nodes."
  // 
  static const double Default_PTSERefreshInterval = 1800.0;

  //
  // af-pnni-0055.00, page 254: "This is used to calculate the initial
  // lifetime of self originated PTSEs.  The initial lifetime is set
  // to the product of te PTSERfreshInterval and the
  // PTSELifetimeFactor.
  //
  static const double Default_PTSELifetimeFactor = 2.0;

  const DBKey * ObtainOriginator( const ig_ptse * p ) const;

private:

  NodeID  * PTSEOriginator(const ig_ptse * ptse) const;
  const ig_ptse * IGtoPTSE(const InfoGroup * ig) const;

  /// Inserts contained PTSPPkt
  bool Insert(NPFloodVisitor * v);

  /// Inserts all contained PTSEs
  bool Insert(PTSPPkt * ptsp, bool mgmt = true, NodeID * RemoteNodeID = 0L);

  /// Inserts a PTSE according to those parts of the
  ///    rules of 5.8.3.3 that involve the Database (page 108).
  const ig_ptse * Insert(const NodeID * Originator, ig_ptse * ptse, bool mgmt = true);

  /// Handles the insertion of an uplinks information group.
  void      HandleUpLinks(const NodeID * nid, const ig_ptse * ptse,
			  bool insert_mode);

  class OriginKey {
  public:
    OriginKey(const DBKey & key);
    OriginKey(const OriginKey & key);
    OriginKey(const u_char * origin, const u_char * pid = 0);
    ~OriginKey();

    OriginKey operator = (const OriginKey & him);
    const u_char * GetOrigin(void) const;
    const u_char * GetPeer(void) const;
    
    u_char _oid[22];    
    u_char _pid[14];
  };
  // if declared inside OriginKey the IRIX CC compiler reports
  // OriginKey arguments as inaccessable
  friend int operator == (const OriginKey & x, const OriginKey & y);
  friend int compare(const OriginKey & lhs, const OriginKey & rhs);
  friend int compare(OriginKey *const & lhs, OriginKey *const & rhs);
  friend ostream & operator << (ostream & os, const OriginKey & key);

  class FloodContainer {
  public:

    FloodContainer(PTSPPkt * ptsp, NodeID * remNode = 0L) : 
      _ptsp(ptsp), _rNID(0) { if (remNode != 0) _rNID = remNode->copy(); }
    ~FloodContainer() { delete _rNID; }

    PTSPPkt * _ptsp;
    NodeID  * _rNID;
  };
  friend int compare(FloodContainer * const & x, FloodContainer * const & y);

  /// Maintain priority queue / timer invariant
  void SetupExecutioner(void);

  /// Absolutely remove a PTSE from the Database,
  ///   Called from inside Expire()
  void Purge(seq_item si);

  /// The ExecutionerPTSE calls this to signal natural death by aging
  void ExpirePTSE(void);

  /// Internal use, for DatabaseSumPkt building
  void UpdatePTSELists(void);

  /// Sends updated election information to the Election FSM.
  void SendElectionUpdate(const NodeID * origin, ig_ptse * now, ig_ptse * then = 0);

  /// Find my equivalent node in this level
  const NodeID * GetMyNodeID(const int level) const;

  /// Switch Key
  ds_String                             * _key;
  /// Local Node ID.
  NodeID                              * _LocNodeID;
  /// Current Sequence Number.
  int                                   _seq_num;
  /// dictionary from Originator to list of PTSEs originated
  sortseq<OriginKey *, list<ig_ptse *> *> _npses; 
  /// The priority queue of PTSEs.
  p_queue<double, ig_ptse *>            _pq;
  /// A Mapping from DBKey to pq_item.
  sortseq<DBKey *, pq_item>             _map;
  /// A Mapping from pq_item to seq_item
  sortseq<pq_item, seq_item>            _trans;
  /// Sorted Sequence of PTSEs on death row.
  sortseq<DBKey *, ig_ptse *>           _death_row;
  /// List of requested PTSEs
  list<PTSEReqPkt *>                    _requests;
  /// List or PTSPs
  list<PTSPPkt *>                       _ptsps;
  /// Hash array which keeps track of which levels this switch has won at.
  h_array<int, bool>                    _level;
  /// Hash array which maps from level to NodeID.
  //  h_array<int, const NodeID *>          _mynodes;
  dictionary<int, const NodeID *>       _mynodes;
  /// Dictionary containing sequence numbers for (re)originating ig uplinks
  dictionary<const NodeID *, int>       _nid2seq;
  /// Dictionary of induced uplinks.
  dictionary<InducedUplink *, NodeID *> _uplinks;

  /// VCAllocator for LGNHI's
  VCAllocator                         * _vca;

  /// PNNI versions supported
  int                                   _newest_version;
  int                                   _oldest_version;
  
  ExecutionerPTSE                     * _executioner;
  ig_ptse                             * _victim_ptse;
  pq_item                               _victim_qitem;
    
  Kernel                              & _globalKernel;

  DatabaseEvent                       * _db_event;
  list<FloodContainer *>                _pending_floods;

  double                                _PTSERefreshInterval;
  double                                _PTSELifetimeFactor;

  dictionary< const NodeID *, bool >          _SVCCStates;

  static VisitorType                  * _npflood_type;
};

#endif // __DATABASE_H__
