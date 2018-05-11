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
static char const _ElectionVisitor_cc_rcsid_[] =
"$Id: ElectionVisitor.cc,v 1.24 1998/08/10 21:00:15 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_pkt/pkt_incl.h>
#include <FW/basics/Visitor.h>
#include <FW/behaviors/Mux.h>
#include <FW/actors/Accessor.h>
#include <FW/basics/VisitorType.h>
#include <FW/basics/Conduit.h>
#include <fsm/election/ElectionVisitor.h>
#include <fsm/election/ElectionState.h>

ElectionVisitor::ElectionVisitor(NodeID *nid, 
				 sortseq<const NodeID *, Nodalinfo *> *  nodelist, 
				 Purpose reason) : 
  VPVCVisitor(_my_type, 0, 0, 0, nid, 0), _mynode(nid), _list(nodelist), 
  _purpose(reason), _nodalig(0), _nodaligNode(0), _pgl(0), _mypriority(0)
{
  SetLoggingOff();
}

ElectionVisitor::ElectionVisitor(NodeID *nid, 
				 Purpose reason, 
				 ig_nodal_info_group * nig, 
				 NodeID * ignode) :
  VPVCVisitor(_my_type, 0,0,0, nid, 0), _mynode(nid),  
  _purpose(reason), _nodalig(nig), _nodaligNode(ignode), 
  _pgl(0), _mypriority(0), _list(0)
{
  SetLoggingOff();
}

ElectionVisitor::ElectionVisitor(const ElectionVisitor & rhs) :
  VPVCVisitor(rhs), _mynode(0), _purpose(rhs._purpose), 
  _action(rhs._action), _nodalig(0), _nodaligNode(0), 
  _pgl(0), _mypriority(rhs._mypriority), _list(0)
{
  if (rhs._mynode)
    _mynode = new NodeID(*(rhs._mynode));
  if (rhs._nodalig)
    _nodalig = (ig_nodal_info_group *)rhs._nodalig->copy();
  if (rhs._nodaligNode)
    _nodaligNode = new NodeID(*(rhs._nodaligNode));
  if (rhs._pgl)
    _pgl = new NodeID(*(rhs._pgl));
}

ElectionVisitor::ElectionVisitor(NodeID *nid, Purpose reason) :
  VPVCVisitor(_my_type, 0,0,0, nid, 0), _mynode(nid),  
  _purpose(reason), _nodalig(0), _nodaligNode(0), _pgl(0), 
  _mypriority(0), _list(0)
{
  SetLoggingOff();
}

ElectionVisitor::ElectionVisitor(NodeID *nid, const NodeID *pgl, 
				 int mypriority, Purpose reason, Action act) :
  VPVCVisitor(_my_type, 0,0,0, nid, 0), _mynode(nid), _purpose(reason), 
  _nodalig(0), _nodaligNode(0), _pgl(0), _mypriority(mypriority), 
  _list(0), _action(act)
{
  if (pgl) _pgl = new NodeID(*pgl);
  SetLoggingOff();
}

ElectionVisitor::~ElectionVisitor() { if (_pgl) delete _pgl; }

const VisitorType ElectionVisitor::GetType(void) const
{ 
  return VisitorType(GetClassType());
}

const vistype & ElectionVisitor::GetClassType(void) const
{
  return _my_type;
}

NodeID * ElectionVisitor::GetNode(void) 
{
  return _mynode;
}

sortseq<const NodeID *, Nodalinfo *> * ElectionVisitor::GetNodalInfo(void)
{
  return _list;
}

ElectionVisitor::Purpose ElectionVisitor::GetPurpose()
{
  return _purpose;
}

ig_nodal_info_group *  ElectionVisitor::GetNodalIG()
{
  return _nodalig;
}

NodeID * ElectionVisitor::GetNodalIGNode(void)
{
  return _nodaligNode;
}

const NodeID * ElectionVisitor::GetPGL(void)
{
  return _pgl;
}

int ElectionVisitor::GetPriority(void)
{
  return _mypriority;
}

ElectionVisitor::Action ElectionVisitor::GetAction(void)
{
  return _action;
}

Visitor * ElectionVisitor::dup(void) const { return new ElectionVisitor(*this); }

void ElectionVisitor::at(Mux * m, Accessor *a)
{
  bool cont = false;
  Conduit * dest = 0;

  assert(m && a);

  switch(EnteredFrom()) {
  case A_SIDE:
    SetLast( CurrentConduit() );
    if (a) {
      if (dest = a->GetNextConduit(this)) {
	dest->Accept(this);
	cont = true;
      }
    }
    break;
  case B_SIDE:
  case OTHER:
    Visitor::at((Behavior *)m);
    cont = true;
    break;
  }

  if (!cont)
    Suicide();
}
