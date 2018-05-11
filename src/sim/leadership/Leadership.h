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
#ifndef __LEADERSHIP_H__
#define __LEADERSHIP_H__

#ifndef LINT
static char const _Leadership_h_rcsid_[] =
"$Id: Leadership.h,v 1.42 1999/02/16 20:23:35 mountcas Exp $";
#endif

#include <FW/actors/State.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/ptse.h>
#include <DS/containers/dictionary.h>
#include <sim/aggregator/ComplexRep.h>
#include <iostream.h>

#define DEFAULT_LIFETIME 1800
#define DEFAULT_PRIORITY 1

class ElectionVisitor;
class Aggregator; // Necessary for Bilal's new ComplexRep implementation
class Logos;
class ig_nodal_state_params;
class ig_nodal_info_group;
class LeadershipInterface;

class Leadership : public State {
  friend class LeadershipInterface;
public:

  Leadership(const NodeID * myNode, const NodeID * preferredPGL = 0 );
  virtual ~Leadership();

  State * Handle(Visitor * v);
  void    Interrupt(SimEvent * e);

  void    Print(ostream & os = cout);

private:
  
  void ProcessElectionVisitor(ElectionVisitor * ev);
  void ASyncNodalIGOrig(u_char level, NodeID * leader = 0);
  void OriginateNodalIG(u_char level, NodeID * leader = 0);
  void OriginateNodalHierarchy(u_char level);
  void OustNodalIG(u_char level);
  void OustNodalHierarchy(u_char level);

  void Insert2DB(ig_ptse * ptse, u_char level);

  NodeID * OneLevelDown(const NodeID * nid);
  NodeID * OneLevelUp(const NodeID * nid);
  NodeID * PhysicalNodeID(void) const;
  NodeID * PGLAtLevel(const u_char level) const;

  void Notify( const ig_nodal_info_group * ig );

  class LevelInfo {
  public:
    LevelInfo(const NodeID * myNode, const NodeID * prefpgl,
	      int priority = DEFAULT_PRIORITY, 
	      const NodeID * pgl = 0);
    LevelInfo(const LevelInfo & rhs);
    ~LevelInfo( );

    LevelInfo & operator = (const LevelInfo & rhs);

    NodeID             * _myNode;
    NodeID             * _prefPGL;
    NodeID             * _pgl;
    list<const NodeID *> _remoteNodes;
    int                  _priority;
    bool                 _iAmLeader;
  };
  friend ostream & operator << (ostream & os, const LevelInfo & li);
  friend int operator == (const LevelInfo & lhs, const LevelInfo & rhs);

  SimEvent   * _se;
  u_char       _init_lev;

  dictionary<u_char, LevelInfo *> _elections;
  dictionary<const NodeID *, int> _seqs;

  // ---------- Added 12-1-98 for Aggregator ----------
  friend void ComplexRep::Reoriginate_NSP(void);
  void GenerateComplexRep(u_char level, 
			  list<ig_nodal_state_params*> * nsplist);
  void Make_ComplexRep(u_char level);
  void Kill_ComplexRep(u_char level);

  int _nsp_seqnum;
  dictionary<const NodeID *, ComplexRep *> _complexReps;
  // ---------- Added 12-1-98 for Aggregator ----------
  static VisitorType * _election_vistype;
  static VisitorType * _fastuni_vistype;
  static VisitorType * _npflood_vistype;

  bool _originating;
};

#endif
