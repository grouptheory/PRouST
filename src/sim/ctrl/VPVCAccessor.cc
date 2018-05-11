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
static char const _VPVCAccessor_cc_rcsid_[] =
"$Id: VPVCAccessor.cc,v 1.13 1999/02/12 16:13:13 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/Conduit.h>
#include <sim/ctrl/VPVCAccessor.h>
#include <fsm/visitors/VPVCVisitor.h>
#include <fsm/visitors/VPVCRebindingVisitor.h>
#include <fsm/visitors/NPFloodVisitor.h>

const VisitorType * VCAccessor::_vpvc_type = 0;
const VisitorType * VCAccessor::_vpvc_rebinding_type = 0;
const VisitorType * VCAccessor::_npflood_type = 0;

VCAccessor::VCAccessor(VCAccessor::direction dir, bool slave) :
  _dir(dir), _slave(slave)
{ 
  if (!_vpvc_type)
    _vpvc_type = QueryRegistry(VPVC_VISITOR_NAME);
  if (!_vpvc_rebinding_type)
    _vpvc_rebinding_type = QueryRegistry(VPVC_REBINDING_VISITOR_NAME);
  if (!_npflood_type)
    _npflood_type = QueryRegistry(NPFLOOD_VISITOR_NAME);
}

VCAccessor::~VCAccessor() { }

Conduit * VCAccessor::GetNextConduit(Visitor * v)
{
  Conduit * answer = 0;
  int vc = -1;

  if (!(v->GetType().Is_A(_npflood_type)) &&
      v->GetType().Is_A(_vpvc_type)) {
    //
    // Bind the conduit to a different VCI
    //
    if (v->GetType().Is_A(_vpvc_rebinding_type)) {
      VPVCRebindingVisitor *vrbv = (VPVCRebindingVisitor *)v;
      int old_vci = vrbv->GetOldVC();
      int new_vci = vrbv->GetNewVC();

      if (old_vci != new_vci) {
	//
	// Unbind the old vci
	dic_item di = _access_map.lookup(old_vci);
	answer = _access_map.inf(di);

	_access_map.del_item(di);

	//
	// Bind the Conduit to the new vci
	//
	_access_map.insert(new_vci, answer);

	//
	// Note how we carefully do not touch vc
	// so that it remains -1 and answer
	// remains unchanged.
	//
      }
    } else {
      VPVCVisitor * vcv = (VPVCVisitor *)v;
      
      if (_dir == VCAccessor::OUT)
	vc = vcv->GetOutVC();
      else if (_dir == VCAccessor::IN)
	vc = vcv->GetInVC();
    }
  }
  
  if (vc != -1) {
    dic_item di;
    if (di = _access_map.lookup(vc)) 
      answer = _access_map.inf(di);
  }
  return answer;
}

bool VCAccessor::Broadcast(Visitor * v)
{
  int count = _access_map.size();

  dic_item di;
  forall_items(di, _access_map) {
    count--;

    Visitor * ToSend = v;
    if (count)
      ToSend = v->duplicate();

    _access_map.inf(di)->Accept(ToSend);
  }
  return true;
}

bool VCAccessor::Add(Conduit * c, Visitor * v)
{
  int vc = -1;
  if (v->GetType().Is_A(_vpvc_type)) {
    VPVCVisitor * vcv = (VPVCVisitor *)v;

    // This is master accessor and it uses OUT VC OR
    //   this is slave accessor and it uses IN VC
    if ((_dir == VCAccessor::OUT && !_slave) ||
	(_slave && _dir == VCAccessor::IN))
      vc = vcv->GetOutVC();
    else if ((_dir == VCAccessor::IN && !_slave) ||
	       (_slave && _dir == VCAccessor::OUT))
      vc = vcv->GetInVC();
  }

  // perhaps we should assert that vc >= 0 here?

  if (vc != -1) {
    dic_item di;
    if (di = _access_map.lookup(vc))
      return false;
    _access_map.insert(vc, c);
    return true;
  }
  return false;
}

bool VCAccessor::Del(Conduit * c)
{
  dic_item di;
  forall_items(di, _access_map) {
    if (_access_map.inf(di) == c) {
      _access_map.del_item(di);
      return true;
    }
  }
  return false;
}

bool VCAccessor::Del(Visitor * v)
{
  int vc = -1;
  if (v->GetType().Is_A(_vpvc_type)) {
    VPVCVisitor * vpv = (VPVCVisitor *)v;

    // This is master accessor and it uses OUT VC OR
    //   this is slave accessor and it uses IN VC
    if ((_dir == VCAccessor::OUT && !_slave) ||
	(_slave && _dir == VCAccessor::IN))
      vc = vpv->GetOutVC();
    else if ((_dir == VCAccessor::IN && !_slave) ||
	     (_slave && _dir == VCAccessor::OUT))
      vc = vpv->GetInVC();
  }

  if (vc != -1) {
    dic_item di;
    if (di = _access_map.lookup(vc)) {
      _access_map.del_item(di);
      return true;
    }
  }
  return false;
}
