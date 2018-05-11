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
static char const _NPFloodVisitor_cc_rcsid_[] =
"$Id: NPFloodVisitor.cc,v 1.33 1999/01/15 21:23:09 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <fsm/visitors/NPFloodVisitor.h>
#include <FW/basics/Conduit.h>
#include <FW/behaviors/Mux.h>
#include <FW/actors/Accessor.h>
#include <codec/pnni_pkt/ptsp.h>
#include <codec/pnni_ig/id.h>

// These Visitors are not mean to leave the switch.
NPFloodVisitor::NPFloodVisitor(PTSPPkt * lPtsp, bool mgmt, 
			       const NodeID * RemoteNodeID, 
			       const NodeID * SourceNodeID) 
  : VPVCVisitor(_mytype, 0, 0, 0, SourceNodeID, RemoteNodeID), 
    _ptsp(lPtsp), _mgmt(mgmt), _RemoteNodeId(0)
{
  if (RemoteNodeID)
    _RemoteNodeId = new NodeID( *RemoteNodeID );
}


NPFloodVisitor::NPFloodVisitor(const NPFloodVisitor & rhs) 
  : VPVCVisitor(rhs), _ptsp(0), _mgmt(rhs._mgmt), _RemoteNodeId(0)
{
  if (rhs._RemoteNodeId)
    _RemoteNodeId = new NodeID(*(rhs._RemoteNodeId));
  if (rhs._ptsp)
    _ptsp = (PTSPPkt *)(rhs._ptsp->copy());
}

NPFloodVisitor::NPFloodVisitor(vistype & child_type, 
			       PTSPPkt * lPtsp)
  : VPVCVisitor(child_type.derived_from(_mytype), 0, 0, 0, 0), 
    _ptsp(lPtsp), _mgmt(false), _RemoteNodeId(0) { }

NPFloodVisitor::~NPFloodVisitor() 
{ 
  if (_ptsp)
    _ptsp->ClearExpiredPtses();
  delete _ptsp;
  delete _RemoteNodeId;
}

void NPFloodVisitor::at(Mux * m, Accessor * a)
{
  Conduit * dest;

  assert(m && a);

  switch(EnteredFrom()) {
  case A_SIDE:
    SetLast( CurrentConduit() );
    if (a) {
      if (dest = a->GetNextConduit(this))
        dest->Accept(this);
      else
        m->Broadcast(this);
    } else {
      if (dest = SideB()) 
        dest->Accept(this);
      else Suicide();
    }
    break;
  case B_SIDE:
  case OTHER:
    Visitor::at(m);
    break;
  }
}

const vistype & NPFloodVisitor::GetClassType(void) const
{
  return _mytype;
}

const VisitorType NPFloodVisitor::GetType(void) const 
{ 
  return VisitorType(GetClassType());
}

const PTSPPkt * NPFloodVisitor::GetFloodPTSP(void) 
{
  // PTSPPkt * temp = _ptsp; _ptsp = 0; return temp; 
  return _ptsp; 
}
  
bool NPFloodVisitor::Getmgmt(void) const { return _mgmt; }


void NPFloodVisitor::SetPTSP(PTSPPkt * pkt) { _ptsp =  pkt; }

const NodeID * NPFloodVisitor::GetRemoteNodeID(void) const { return _RemoteNodeId; }

Visitor * NPFloodVisitor::dup(void) const { return new NPFloodVisitor(*this); }
