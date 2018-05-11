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
#ifndef _ELECTION_VISTORS_H_
#define _ELECTION_VISTORS_H_
#ifndef LINT
static char const _ElectionVisitor_h_rcsid_[] =
"$Id: ElectionVisitor.h,v 1.18 1998/08/11 18:34:40 mountcas Exp $";
#endif

#include <fsm/visitors/VPVCVisitor.h>
#include <DS/containers/list.h>
#include <DS/containers/sortseq.h>

class NodeID;
class Nodalinfo;
class ig_nodal_info_group;

#define ELECTION_VISITOR_NAME "ElectionVisitor"

class ElectionVisitor: public VPVCVisitor {
public:

  enum Purpose {
    RequestConnectivityInfo,
    InsertedNodalInfo,
    HorizontalLinkUp,
    HorizontalLinkDown,
    ToOriginateANodalinfo
  };

  // For Communication between 
  // ElectionFSM -> ACAC UNIdirectional
  enum Action {
    PGLE5,
    PGLE6,
    PGLE7,
    PGLE8,
    PGLE9
  };

  // To GCAC
  ElectionVisitor(NodeID * nid, sortseq<const NodeID *, Nodalinfo *> *, 
		  Purpose reason);
  // To Election informing nodal_info
  ElectionVisitor(NodeID *nid, Purpose reason, ig_nodal_info_group *, 
		  NodeID *);
  // To Election informing Horizotalid
  ElectionVisitor(NodeID *nid, Purpose reason);
  // To Originate a NodalInfo with priority sent to acac from the Election
  ElectionVisitor(NodeID *nid, const NodeID *pgl, int priority, Purpose reason,
		  Action act);
 
  virtual ~ElectionVisitor(void);
  virtual const VisitorType GetType(void) const;

  NodeID * GetNode(void);
  sortseq<const NodeID *, Nodalinfo *> * GetNodalInfo(void);
  Purpose                          GetPurpose(void);
  ig_nodal_info_group *            GetNodalIG(void);
  NodeID *                         GetNodalIGNode(void); 
  const NodeID *                   GetPGL(void);
  int                              GetPriority(void); 
  Action                           GetAction(void);

  void at(Mux * m, Accessor * a);

protected:

  virtual Visitor * dup(void) const;

  ElectionVisitor(const ElectionVisitor & rhs);
  virtual const vistype & GetClassType(void) const;
  
  Purpose               _purpose;
  Action                _action;
  
private:

   static vistype          _my_type;

   NodeID              * _mynode;      // Used as a destination node when
   NodeID              * _nodaligNode; // Used to indicate which node originated the nodalig
   ig_nodal_info_group * _nodalig;
   NodeID              * _pgl;
   int                   _mypriority;
   sortseq<const NodeID *, Nodalinfo *> * _list;
};

#endif  // __ELECTION_VISITOR_H__
