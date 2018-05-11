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
#ifndef __OMNI_H__
#define __OMNI_H__

#ifndef LINT
static char const _Omni_h_rcsid_[] =
"$Id: Omni.h,v 1.21 1999/02/10 16:54:12 mountcas Exp $";
#endif

#include <DS/containers/dictionary.h>
#include <fsm/visitors/NPFloodVisitor.h>
#include "PTSESniffer.h"

class Omni;
class DBKey;
class Database;
class DBLogosInterface;
class Logos;
class NodeID;
class Conduit;
class PTSPPkt;
class ig_ptse;

#define OMNI_NODE_ID "96:160:0123456789abcdef0123456789abcdef01234567" 
                   
class Omni {
  friend class OmniPOC;
  friend class ResourceManager;
  friend class Logos;
  friend class fsm_atexitCleanup;

  // perhaps this should be a reference to Omni, since it should never be null
  friend Omni & theOmni(void);
public:

  enum pg_convergence_status {
    not_converged = 0,
    converged     = 1
  };

  enum pg_convergence_status_change {
    unchanged                = 0,
    changed_to_converged     = 1,
    changed_to_not_converged = 2
  };

  // testing of synchrony of every PG at every level
  bool FullConvergenceTest( int& num_nodes ) const;

  // sw is the switch where election or NP activity has occurred.
  // pgl will be set to the switch that the return value pertains to.
  pg_convergence_status_change
     LocalConvergenceTest(const NodeID * sw, const NodeID *& pgl);

  // Dump the tree of elections
  void DumpElectionTree(const char * fname = 0);

  void UpdateOmniDB(NPFloodVisitor* npv);
  // Omni will own the key passed in, so make a copy first if needed
  bool Report_PTSE(NodeID * citizen, DBKey * key);

  void setCertificationMode(bool val);
  const char* getCertificate(void) const;

  void DiscordLog(void) const;

  bool AmIOmniscient( const Database * d ) const;

  // recompute global synchrony after a ptse insertion at citizen
  void RecomputeGlobalSynchrony( NodeID* citizen );

  bool isConverged(void) const;

private:  

  DBLogosInterface * LogosInterface( void ) const;

  void ProcessNPFlood( NPFloodVisitor * npv );
  void Process_PTSP(const PTSPPkt * ptsp);
  void Process_NodalIG(const NodeID * Originator, ig_ptse * ptse);

  // In these 4 methods, Omni will own the 2nd argument, 
  // but it does not own the first argument.
  void Leadership_Notice(const NodeID * Originator, NodeID * leader);

  // PTSE information for every switch in the network
  dictionary<const NodeID*, dictionary<DBKey*, int>*>  _census;

  // Downward adjacencies in election tree
  dictionary<const NodeID*, list<NodeID*>* > _leader2subjects;

  // Upward adjacency in election tree
  dictionary<const NodeID*, NodeID* >        _subject2leader;

  // Current state of synchrony for each node
  dictionary<const NodeID*, int >            _PGL2status;

  Database * _omni_database;
  Conduit  * _omni_database_conduit;

  NodeID   * _omni_node_id;

  // See if two nodes in the same PG agree
  bool InSynch(NodeID* node1, NodeID* node2) const;
  
  // See if all subjects of a PGL are in synch
  bool InSynch(NodeID* leader) const;

  void DiscordLog(NodeID * leader) const;
  void DiscordLog(NodeID * s1, NodeID* s2) const;

  Omni(void);
  virtual ~Omni();
  
  bool    _globalSynchrony;
  
  static const int CERTIFICATE_SIZE = 8192;
  char * _certificate;
  static const int ENTRY_SIZE = 1024;
  char * _entry;

  bool _certify;

  static Omni * _singleton;

  PTSESniffer sniffer1;
  PTSESniffer sniffer2;
  PTSESniffer sniffer3;
  PTSESniffer sniffer4;
};

#endif
