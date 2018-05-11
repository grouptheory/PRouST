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
static char const _NodeAccessor_cc_rcsid_[] =
"$Id: NodeAccessor.cc,v 1.52 1998/11/12 14:23:42 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <sim/ctrl/NodeAccessor.h>
#include <sim/ctrl/LogicalNode.h>
#include <fsm/visitors/VPVCVisitor.h>
#include <fsm/visitors/PortVisitor.h>
#include <fsm/visitors/LinkVisitor.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/NPFloodVisitor.h>
#include <FW/basics/Conduit.h>
#include <codec/uni_ie/addr.h>

/// ------------------- AddrAccessor ---------------------
const VisitorType * AddrAccessor::_vpvc_type = 0;
const VisitorType * AddrAccessor::_link_type = 0;

AddrAccessor::AddrAccessor(AddrAccessor::direction dir, bool slave) : 
  _dir(dir), _slave(slave)
{
  if (!_vpvc_type)
    _vpvc_type = QueryRegistry(VPVC_VISITOR_NAME);
  if (!_link_type)
    _link_type = QueryRegistry(LINK_VISITOR_NAME);
}

AddrAccessor::~AddrAccessor() { }

Conduit * AddrAccessor::GetNextConduit(Visitor * v)
{
  const Addr   * add = 0;

  if (v->GetType().Is_A(_vpvc_type)) {
    VPVCVisitor  * vpv = (VPVCVisitor *)v;
    if (_dir == AddrAccessor::DESTINATION)
      add = (vpv->GetDestNID() ? vpv->GetDestNID()->GetAddr() : 0);
    else
      add = (vpv->GetSourceNID() ? vpv->GetSourceNID()->GetAddr() : 0);
  }
  if (v->GetType().Is_A(_link_type)) {
    LinkVisitor  * lkv = (LinkVisitor *)v;
    if (_dir == AddrAccessor::DESTINATION)
      add = (lkv->GetDestNID() ? lkv->GetDestNID()->GetAddr() : 0);
    else
      add = (lkv->GetSourceNID() ? lkv->GetSourceNID()->GetAddr() : 0);
  }
    
  if (add) {
    NSAP_DCC_ICD_addr * target = (NSAP_DCC_ICD_addr *)add;

    dic_item di;
    forall_items(di, _access_map) {
      NSAP_DCC_ICD_addr * nsap = 
	(NSAP_DCC_ICD_addr*)(_access_map.key(di));
      if (nsap->equals(target)) {
	delete add;
	return _access_map.inf(di);
      }
    }
  }
  delete add;
  return 0;
}

bool AddrAccessor::Broadcast(Visitor * v)
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

bool AddrAccessor::Add(Conduit * c, Visitor * v)
{
  const Addr  * nid = 0;

  if (v->GetType().Is_A(_vpvc_type)) {
    VPVCVisitor * vpv = (VPVCVisitor *)v;
    if ((_dir == AddrAccessor::DESTINATION && !_slave) ||
	(_slave && _dir == AddrAccessor::SOURCE))
      nid = (vpv->GetDestNID() ? vpv->GetDestNID()->GetAddr() : 0);
    else if ((_dir == AddrAccessor::SOURCE && !_slave) ||
	       (_slave && _dir == AddrAccessor::DESTINATION))
      nid = (vpv->GetSourceNID() ? vpv->GetSourceNID()->GetAddr() : 0);
  }
  if (v->GetType().Is_A(_link_type)) {
    LinkVisitor * lkv = (LinkVisitor *)v;
    if ((_dir == AddrAccessor::DESTINATION && !_slave) ||
	(_slave && _dir == AddrAccessor::SOURCE))
      nid = (lkv->GetDestNID() ? lkv->GetDestNID()->GetAddr() : 0);
    else if ((_dir == AddrAccessor::SOURCE && !_slave) ||
	       (_slave && _dir == AddrAccessor::DESTINATION))
      nid = (lkv->GetSourceNID() ? lkv->GetSourceNID()->GetAddr() : 0);
  }
    
  if (nid) {
    NSAP_DCC_ICD_addr * target = (NSAP_DCC_ICD_addr*)nid;
    dic_item di;
    forall_items(di, _access_map) {
      NSAP_DCC_ICD_addr * nsap = 
	(NSAP_DCC_ICD_addr *)(_access_map.key(di));
      if (nsap->equals(target)) 
	return false;
    }
    _access_map.insert((Addr *)nid, c);
    return true;
  }
  return false;
}

bool AddrAccessor::Del(Conduit * c)
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

bool AddrAccessor::Del(Visitor * v)
{
  const Addr * nid = 0;
  VPVCVisitor * vpv = 0;
  LinkVisitor * lkv = 0;

  if (v->GetType().Is_A(_vpvc_type))
    vpv = (VPVCVisitor *)v;
  if (v->GetType().Is_A(_link_type))
    lkv = (LinkVisitor *)v;
    
  if (vpv || lkv) {
    // This is master accessor and it uses destination Addr OR
    //   this is slave accessor and it uses source Addr
    if ((_dir == AddrAccessor::DESTINATION && !_slave) ||
	(_slave && _dir == AddrAccessor::SOURCE)) {
      if (vpv)
	nid = (vpv->GetDestNID()->GetAddr());
      else
	nid = (lkv->GetDestNID()->GetAddr());
    } else if ((_dir == AddrAccessor::SOURCE && !_slave) ||
	       (_slave && _dir == AddrAccessor::DESTINATION)) {
      if (vpv)
	nid = (vpv->GetSourceNID()->GetAddr());
      else
	nid = (lkv->GetSourceNID()->GetAddr());
    }
  }

  if (nid) {
    NSAP_DCC_ICD_addr * nsap;
    NSAP_DCC_ICD_addr * target = (NSAP_DCC_ICD_addr*)(nid);
    dic_item di;
    forall_items(di, _access_map) {
      nsap = (NSAP_DCC_ICD_addr*)(_access_map.key(di));
      if (nsap->equals(target)) {
	_access_map.del_item(di);
	return true;
      }
    }
  }
  return false;
}

/// ------------------- NodeIDAccessor ---------------
const VisitorType * NodeIDAccessor::_vpvc_type = 0;
const VisitorType * NodeIDAccessor::_link_type = 0;
const VisitorType * NodeIDAccessor::_npflood_type = 0;

NodeIDAccessor::NodeIDAccessor(NodeIDAccessor::direction dir, bool slave) : 
  _dir(dir), _slave(slave)
{
  if (!_vpvc_type) {
    VPVCVisitor * vis = new VPVCVisitor;
    _vpvc_type = new VisitorType(vis->GetType());
    vis->Suicide();
  }
  if (!_link_type) {
    LinkVisitor * v = new LinkVisitor;
    _link_type = new VisitorType(v->GetType());
    v->Suicide();
  }
  if (!_npflood_type)
    _npflood_type = QueryRegistry(NPFLOOD_VISITOR_NAME);
}

NodeIDAccessor::~NodeIDAccessor() { }

Conduit * NodeIDAccessor::GetNextConduit(Visitor * v)
{
  const NodeID * nid = 0;
  VPVCVisitor  * vpv = 0;
  LinkVisitor  * lkv = 0;

  if (v->GetType().Is_A(_vpvc_type))
    vpv = (VPVCVisitor *)v;
  if (v->GetType().Is_A(_link_type))
    lkv = (LinkVisitor *)v;
  if (v->GetType().Is_A(_npflood_type)) {
    lkv = 0;
    vpv = (VPVCVisitor *)lkv;
  }

  if (vpv || lkv) {
    if (_dir == NodeIDAccessor::DESTINATION) {
      if (vpv)
	nid = vpv->GetDestNID();
      else
	nid = lkv->GetDestNID();
    } else {
      if (vpv)
	nid = vpv->GetSourceNID();
      else
	nid = lkv->GetSourceNID();
    }
  } 
    
  if (nid) {
    NSAP_DCC_ICD_addr* nsap;
    NSAP_DCC_ICD_addr* target = (NSAP_DCC_ICD_addr*)(nid->GetAddr());
    dic_item di;
    forall_items(di,_access_map) {
      nsap = (NSAP_DCC_ICD_addr*)(_access_map.key(di)->GetAddr());
      if (nsap->equals(target)) {
	delete nsap; delete target;
	return _access_map.inf(di);
      }
      delete nsap;
    }
    delete target;
  }
  return 0;
}

bool NodeIDAccessor::Broadcast(Visitor * v)
{
  int count = _access_map.size();
  dic_item di;
  forall_items(di, _access_map) {
    count--;

    Visitor * ToSend = v;
    if (count)
      ToSend = v->duplicate();
    else // this should be the last one
      v = 0;

    // This way we do not need to waste resources by doing an unnecessary copy and delete.
    _access_map.inf(di)->Accept(ToSend);
  }
  if (v)  // This way we clean-up if no connections
    v->Suicide();
  return true;
}

bool NodeIDAccessor::Add(Conduit * c, Visitor * v)
{
  const NodeID * nid = 0;
  VPVCVisitor * vpv = 0;
  LinkVisitor * lkv = 0;

  if (v->GetType().Is_A(_vpvc_type))
    vpv = (VPVCVisitor *)v;
  if (v->GetType().Is_A(_link_type))
    lkv = (LinkVisitor *)v;
    
  if (vpv || lkv) {
    // This is master accessor and it uses destination NodeID OR
    //   this is slave accessor and it uses source NodeID
    if ((_dir == NodeIDAccessor::DESTINATION && !_slave) ||
	(_slave && _dir == NodeIDAccessor::SOURCE)) {
      if (vpv)
	nid = vpv->GetDestNID();
      else
	nid = lkv->GetDestNID();
    } else if ((_dir == NodeIDAccessor::SOURCE && !_slave) ||
	       (_slave && _dir == NodeIDAccessor::DESTINATION)) {
      if (vpv)
	nid = vpv->GetSourceNID();
      else
	nid = lkv->GetSourceNID();
    }
  }

  if (nid) {
    NSAP_DCC_ICD_addr* nsap;
    NSAP_DCC_ICD_addr* target = (NSAP_DCC_ICD_addr*)(nid->GetAddr());
    dic_item di;
    forall_items(di,_access_map) {
      nsap = (NSAP_DCC_ICD_addr*)(_access_map.key(di)->GetAddr());
      if (nsap->equals(target)) {
	delete target; delete nsap;
	return false;
      }
      delete nsap;
    }
    _access_map.insert(nid->copy(), c);
    delete target;
    return true;
  }
  return false;
}

bool NodeIDAccessor::Del(Conduit * c)
{
  dic_item di;
  forall_items(di, _access_map) {
    if (_access_map.inf(di) == c) {
      delete (NodeID *)_access_map.key(di);
      _access_map.del_item(di);
      return true;
    }
  }
  return false;
}

bool NodeIDAccessor::Del(Visitor * v)
{
  const NodeID * nid = 0;
  VPVCVisitor * vpv = 0;
  LinkVisitor * lkv = 0;

  if (v->GetType().Is_A(_vpvc_type))
    vpv = (VPVCVisitor *)v;
  if (v->GetType().Is_A(_link_type))
    lkv = (LinkVisitor *)v;
    
  if (vpv || lkv) {
    // This is master accessor and it uses destination NodeID OR
    //   this is slave accessor and it uses source NodeID
    if ((_dir == NodeIDAccessor::DESTINATION && !_slave) ||
	(_slave && _dir == NodeIDAccessor::SOURCE)) {
      if (vpv)
	nid = vpv->GetDestNID();
      else
	nid = lkv->GetDestNID();
    } else if ((_dir == NodeIDAccessor::SOURCE && !_slave) ||
	       (_slave && _dir == NodeIDAccessor::DESTINATION)) {
      if (vpv)
	nid = vpv->GetSourceNID();
      else
	nid = lkv->GetSourceNID();
    }
  }

  if (nid) {
    NSAP_DCC_ICD_addr* nsap;
    NSAP_DCC_ICD_addr* target = (NSAP_DCC_ICD_addr*)(nid->GetAddr());
    dic_item di;
    forall_items(di,_access_map) {
      nsap = (NSAP_DCC_ICD_addr*)(_access_map.key(di)->GetAddr());
      if (nsap->equals(target)) {
	delete nsap; delete target;
	delete (NodeID *)_access_map.key(di);
	_access_map.del_item(di);
	return true;
      }
      delete nsap;
    }
    delete target;
  }
  return false;
}

/// ------------------- AggAccessor ------------------
const VisitorType * AggAccessor::_port_visitor_type = 0;
const VisitorType * AggAccessor::_fast_uni_type = 0;

AggAccessor::AggAccessor(void) 
  : _exp(0)
{
  if (!_port_visitor_type)
    _port_visitor_type = QueryRegistry(PORT_VISITOR_NAME);
  if (!_fast_uni_type)
    _fast_uni_type = QueryRegistry(FAST_UNI_VISITOR_NAME);
}

AggAccessor::~AggAccessor() { }

void AggAccessor::InformOfSVCExp(Conduit* exp) {
  assert(exp);  assert(!_exp);  // WORM
  _exp = exp;
}

Conduit * AggAccessor::GetNextConduit(Visitor * v)
{
  Conduit * answer = 0;

  if (v->GetType().Is_A(_fast_uni_type)) {
    FastUNIVisitor * fv = (FastUNIVisitor *)v;
    
    if (fv->GetMSGType() == FastUNIVisitor::FastUNILHD) {
      int tok = fv->GetAgg();
      dic_item di;
      if ((_access_map.size()==1) &&
	  (di = _access_map.lookup(tok))) {
	answer = _access_map.inf(di);

	// _access_map.del_item(di);
	cout << "###Bilal### We would have deleted the SVCExp now\n";

	// delete _exp;
	// Unsafe? Probably.
      }
    }

    if (fv->GetMSGType() == FastUNIVisitor::FastUNILHIRebinder) {
      int new_agg = fv->GetAgg();

      dic_item di;
      if (di = _access_map.lookup(0)) {
	answer = _access_map.inf(di);

	_access_map.del_item(di);

	// rebind
	_access_map.insert(new_agg, answer);

	// Now it should fall into the next if statement and pass into the expander
      }
    }
  }

  if (v->GetType().Is_A(_port_visitor_type)) {
    PortVisitor * vis = (PortVisitor *)v;

    int agg = vis->GetAgg();

    dic_item di;
    if (di = _access_map.lookup(agg)) 
      answer = _access_map.inf(di);
  }
  return answer;
}

bool AggAccessor::Broadcast(Visitor * v)
{
  assert(v);
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

bool AggAccessor::Add(Conduit * c, Visitor * v)
{
  if (v->GetType().Is_A(_port_visitor_type)) {
    PortVisitor * vis = (PortVisitor *)v;

    int agg = vis->GetAgg();

    dic_item di;
    if (di = _access_map.lookup(agg))
      return false;
    _access_map.insert(agg, c);
    return true;
  }
  return false;
}

bool AggAccessor::Del(Conduit * c)
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

bool AggAccessor::Del(Visitor * v)
{
  if (v->GetType().Is_A(_port_visitor_type)) {
    PortVisitor * vis = (PortVisitor *)v;

    int agg = vis->GetAgg();

    dic_item di;
    if (di = _access_map.lookup(agg)) {
      _access_map.del_item(di);
      return true;
    }
  }
  return false;
}

