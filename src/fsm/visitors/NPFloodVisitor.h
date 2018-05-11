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
#ifndef __NPFLOODVISITOR_H__
#define __NPFLOODVISITOR_H__
#ifndef LINT
static char const _NPFloodVisitor_h_rcsid_[] =
"$Id: NPFloodVisitor.h,v 1.18 1999/01/15 21:23:11 mountcas Exp $";
#endif

#include <fsm/visitors/VPVCVisitor.h>

class PTSPPkt;
class NodeID;

#define NPFLOOD_VISITOR_NAME "NPFloodVisitor"

class NPFloodVisitor : public VPVCVisitor {
public:

  NPFloodVisitor(PTSPPkt * lPtsp, bool mgmt = true, 
		 const NodeID * RemoteNodeID = 0L, 
		 const NodeID * SourceNodeID = 0L);
  
  virtual const VisitorType GetType(void) const;

  const PTSPPkt * GetFloodPTSP(void);

  bool  Getmgmt(void) const;
  const NodeID * GetRemoteNodeID(void) const;
  void  SetPTSP(PTSPPkt * pkt);

protected:

  virtual Visitor * dup(void) const;

  virtual ~NPFloodVisitor();

  NPFloodVisitor(const NPFloodVisitor & rhs);
  NPFloodVisitor(vistype &child_type, PTSPPkt *lPtsp);            
  virtual void at(Mux * m, Accessor * a);
  virtual const vistype &GetClassType(void) const;

private:

  PTSPPkt * _ptsp;
  bool      _mgmt;
  NodeID  * _RemoteNodeId;

  static vistype _mytype;
};

#endif
