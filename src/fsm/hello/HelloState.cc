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
static char const _HelloState_cc_rcsid_[] =
"$Id: HelloState.cc,v 1.359 1999/02/10 19:06:58 mountcas Exp battou $";
#endif
#include <common/cprototypes.h>

#include <fsm/hello/Hello_DIAG.h>
#include <fsm/hello/HelloState.h>
#include <fsm/hello/HelloTimers.h>
#include <fsm/hello/HelloVisitor.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/nodepeer/NPVisitors.h>
#include <fsm/visitors/NPFloodVisitor.h>
#include <fsm/visitors/LinkUpVisitor.h>
#include <fsm/visitors/LinkDownVisitor.h>
#include <fsm/visitors/BorderVisitor.h>
#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/visitors/PortDownVisitor.h>
#include <fsm/netstats/NetStatsCollector.h>

#include <FW/basics/diag.h>

#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/nodal_hierarchy_list.h>
#include <codec/pnni_pkt/hello.h>
#include <codec/uni_ie/addr.h>

// prototype test helper func
void MakeBogusRAIGs(LinkUpVisitor * luv);

// ------------------------------------------------------------------
InternalHelloState * HelloState::_StateLinkDown      = 0;
InternalHelloState * HelloState::_StateAttempt       = 0;
InternalHelloState * HelloState::_StateOneWayInside  = 0;
InternalHelloState * HelloState::_StateTwoWayInside  = 0;
InternalHelloState * HelloState::_StateOneWayOutside = 0;
InternalHelloState * HelloState::_StateTwoWayOutside = 0;
InternalHelloState * HelloState::_StateCommon        = 0;
int                  HelloState::_ref_count          = 0;

const VisitorType * HelloState::_hello_type    = 0;
const VisitorType * HelloState::_npflood_type  = 0;
const VisitorType * HelloState::_linkup_type   = 0;
const VisitorType * HelloState::_linkdown_type = 0;
const VisitorType * HelloState::_npstate_type  = 0;
const VisitorType * HelloState::_vpvc_type     = 0;

void HelloState::AllocateStates(void)
{
  if (!_StateLinkDown)
    _StateLinkDown = new StateLinkDown();
  if (!_StateAttempt)
    _StateAttempt = new StateAttempt();
  if (!_StateOneWayInside)
    _StateOneWayInside = new State1WayInside();
  if (!_StateTwoWayInside)
    _StateTwoWayInside = new State2WayInside();
  if (!_StateOneWayOutside)
    _StateOneWayOutside = new State1WayOutside();
  if (!_StateTwoWayOutside)
    _StateTwoWayOutside = new State2WayOutside();
  if (!_StateCommon)
    _StateCommon = new StateCommon();

  // Allocate the VisitorTypes as well
  if (!_hello_type)
    _hello_type = QueryRegistry(HELLO_VISITOR_NAME);
  if (!_npflood_type)
    _npflood_type = QueryRegistry(NPFLOOD_VISITOR_NAME);
  if (!_linkup_type)
    _linkup_type = QueryRegistry(LINK_UP_VISITOR_NAME);
  if (!_linkdown_type)
    _linkdown_type = QueryRegistry(LINK_DOWN_VISITOR_NAME);
  if (!_npstate_type)
    _npstate_type = QueryRegistry(NPSTATE_VISITOR_NAME);
  if (!_vpvc_type)
    _vpvc_type = QueryRegistry(VPVC_VISITOR_NAME);
}

void HelloState::DeallocateStates(void)
{
  if (_StateLinkDown) {
    delete _StateLinkDown;
    _StateLinkDown = 0;
  }
  if (_StateAttempt) {
    delete _StateAttempt;
    _StateAttempt = 0;
  }
  if (_StateOneWayInside) {
    delete _StateOneWayInside;
    _StateOneWayInside = 0;
  }
  if (_StateTwoWayInside) {
    delete _StateTwoWayInside;
    _StateTwoWayInside = 0;
  }
  if (_StateOneWayOutside) {
    delete _StateOneWayOutside;
    _StateOneWayOutside = 0;
  }
  if (_StateTwoWayOutside) {
    delete _StateTwoWayOutside;
    _StateTwoWayOutside = 0;
  }
  if (_StateCommon) {
    delete _StateCommon;
    _StateCommon = 0;
  }
  if (_hello_type) {
    delete _hello_type;
    _hello_type = 0;
  }
  if (_npflood_type) {
    delete _npflood_type;
    _npflood_type = 0;
  }
  if (_linkup_type) {
    delete _linkup_type;
    _linkup_type= 0;
  }
  if (_linkdown_type) {
    delete _linkdown_type;
    _linkdown_type = 0;
  }
  if (_npstate_type) {
    delete _npstate_type;
    _npstate_type = 0;
  }
  if (_vpvc_type) {
    delete _vpvc_type;
    _vpvc_type = 0;
  }
}

HelloState::HelloState( u_char nid[22], u_int pid, 
			u_short cver, 
			u_short nver, 
			u_short over, 
			int vpi, int vci) 
  : State(), _LocNodeID(0), _RemNodeID(0), _vpi(vpi), _vci(vci), 
    _PortIsUP(false), _seqn(1), _cver(cver), _nver(nver), _over(over),
    _HelloInterval(TimerInterval), _InactivityFactor(InactivityFactor),
    _RemPortID(0), _hp(0), _CommonPeerGroupID(0), _UpNodeID(0), 
    _UpNodeATMaddr(0), _theRAIGs(0), 
    _TrxULIASeqNum(0), _RcvULIASeqNum(0), _RcvNHLSeqNum(0), _DerLinkAggToken(-1), 
    _ConfLinkAggToken(-1), _RemLinkAggToken(-1), _RemPeerGroupID(0), _Hcnt(0), 
    _Icnt(0)
{
  AllocateStates();
  _ref_count++;

  _cs = _StateLinkDown;

  _vpvc = (((vpi & 0x0fff) << 16) | (vci & 0xffff));

  // build timers
  _HelloTimer = new HelloTimer(this, _HelloInterval);
  assert(_HelloTimer != 0);

  _InactivityTimer = 
    new InactivityTimer(this, 1.0 * _HelloInterval * _InactivityFactor);
  assert(_InactivityTimer != 0);

  // build local info 
  _LocPortID = pid;
  assert(pid != 0); // port 0 and 0xffffffff are reserved

  // build the PGID where first byte is 160 then copy level number
  // of bytes starting at byte 3 and padd with zero's
  // build the ATM address
  char ATMAddr[20];
  bcopy((void *)(nid + 2), ATMAddr, 20);
  _LocAddr = new NSAP_DCC_ICD_addr(&ATMAddr[0]);
  assert(_LocAddr != 0);

  // build the NodeID
  _LocNodeID = new NodeID(nid);
  assert(_LocNodeID != 0);
  DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << "LocNodeID: " << *_LocNodeID
       << " (" << _LocNodeID << ")" << endl;);

  _LocPeerGroupID = _LocNodeID->GetPeerGroup();
  DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << "LocPeerGroupID: " 
       << *_LocPeerGroupID 
       << " (" << _LocPeerGroupID << ")" << endl;);

  DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << 
       "   LocPortID: " << (int)_LocPortID << "   RemPortID: " 
       << (int)_RemPortID <<
       "   AggrToken: " << _ConfLinkAggToken << endl <<
       "   VPI/VCI: " << _vpi << "/" << _vci << "   Version: " << _cver
       << endl);
}

HelloState::HelloState(const NodeID * nidptr, u_int pid, 
		       u_short cver, u_short nver, 
		       u_short over, int vpi, 
		       int vci) :
  State(), _LocNodeID(0), _RemNodeID(0), _vpi(vpi), _vci(vci), 
  _PortIsUP(false), _seqn(1), _cver(cver), _nver(nver), _over(over),
  _HelloInterval(TimerInterval), _InactivityFactor(InactivityFactor),
  _RemPortID(0), _hp(0), _CommonPeerGroupID(0), _UpNodeID(0), 
  _UpNodeATMaddr(0), _TrxULIASeqNum(0), _RcvULIASeqNum(0), _RcvNHLSeqNum(0), 
  _DerLinkAggToken( -1 ), _ConfLinkAggToken( -1 ), _RemLinkAggToken( -1 ), 
  _RemPeerGroupID(0), _Hcnt(0), _Icnt(0), _theRAIGs(0)
{
  AllocateStates();
  _ref_count++;
  _cs = _StateLinkDown;

  _vpvc = (((vpi & 0x0fff) << 16) | (vci & 0xffff));
  // build timers
  _HelloTimer = new HelloTimer(this, _HelloInterval);
  assert(_HelloTimer != 0);

  _InactivityTimer = 
    new InactivityTimer(this, 1.0*_HelloInterval*_InactivityFactor);
  assert(_InactivityTimer != 0);

  // build local info 
  _LocPortID = pid;
  assert(pid != 0); // port 0 and 0xffffffff are reserved
  if (nidptr)
    _LocNodeID = new NodeID (*nidptr);
  const u_char * nid = nidptr->GetNID();

  // build the PGID where first byte is 160 then copy level number
  // of bytes starting at byte 3 and padd with zero's
  _LocPeerGroupID = _LocNodeID->GetPeerGroup();
  assert(_LocPeerGroupID != 0);
  DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << "LocPeerGroupID: "
       << *_LocPeerGroupID << " (" << _LocPeerGroupID << ")" << endl;);

  // build the ATM address
  char ATMAddr[20];
  bcopy((u_char *)(nid + 2), ATMAddr, 20);
  _LocAddr = new NSAP_DCC_ICD_addr;
  int i = 20;
  _LocAddr->decode((u_char *)ATMAddr, i);
  assert(_LocAddr != 0);

  // build the NodeID
  assert(_LocNodeID != 0);
  DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << "LocNodeID: "
       << *_LocNodeID << " (" << _LocNodeID << ")" << endl;);

  diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, 
       "   LocPortID: %d\t  RemPortID: %d\t  AggrToken: %d\n", 
       _LocPortID, _RemPortID, _ConfLinkAggToken);
}

HelloState::~HelloState() 
{
  if (--_ref_count == 0)
    DeallocateStates();

  delete _LocNodeID;      _LocNodeID = 0;
  delete _LocAddr;        _LocAddr = 0;
  delete _LocPeerGroupID; _LocPeerGroupID = 0;
  delete _RemNodeID;      _RemNodeID = 0;
  delete _UpNodeID;       _UpNodeID = 0;
  delete _HelloTimer;     _HelloTimer = 0;
  delete _InactivityTimer;_InactivityTimer = 0;
}

void HelloState::Dump(void) 
{
  Dump(cout);
}

void HelloState::Dump(ostream & os)
{
  os << OwnerName() << ":" << endl
     << "_LocNodeID: (" << _LocNodeID << ") ";
  if (_LocNodeID)
    os << *_LocNodeID;
  os << endl;

  os << "_RemNodeID: (" << _RemNodeID << ") ";
  if (_RemNodeID)
    os << *_RemNodeID;
  os << endl;

  os << "_UpNodeID: (" << _UpNodeID << ") ";
  if (_UpNodeID)
    os << *_UpNodeID;
  os << endl;

  os << "_LocPeerGroupID: (" << _LocPeerGroupID << ") ";
  if (_LocPeerGroupID)
    os << *_LocPeerGroupID;
  os << endl;

  os << "_LocAddr: (" << _LocAddr << ") ";
  if (_LocAddr)
    os << *_LocAddr;
  os << endl;

  os << "_UpNodeATMaddr: (" << _UpNodeATMaddr << ") ";
  if (_UpNodeATMaddr)
    os << *_UpNodeATMaddr;
  os << endl;

  os << endl;
}

State * HelloState::Handle(Visitor * v)
{
  assert(_cs);
  _cs->Handle(this, v);
  return this;
}

void HelloState::Interrupt(SimEvent * ev)
{
  diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "Ack! %s has been interrupted.\n", 
       OwnerName());
}

void HelloState::SetHelloTimer(void)
{
  Register(_HelloTimer);
  _Hcnt++;
  diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "Register: Hcnt = %x\n", (int)_Hcnt);
}

void HelloState::StopHelloTimer(void)
{
  Cancel(_HelloTimer);
  _Hcnt--;
  diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "Cancel: Hcnt = %x\n", (int)_Hcnt);
}

void HelloState::SetInactivityTimer(void)
{
  Register(_InactivityTimer);
  _Icnt++;
  diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "Register: Icnt = %x\n", (int)_Icnt);
}

void HelloState::StopInactivityTimer(void)
{
  Cancel(_InactivityTimer);
  _Icnt--;
  diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "Cancel: Icnt = %x\n", (int)_Icnt);
}

const char * HelloState::GetName(void) { return OwnerName(); }

void  HelloState::SendHello(void)
{
  if (_cs == _StateOneWayOutside || 
      _cs == _StateTwoWayOutside || 
      _cs == _StateCommon) {
    SendHelloWithNHL();
    return;
  }

  // The VP,VC should be 0,18
  HelloVisitor * v = new HelloVisitor();
  assert(v != 0);
  v->SetLoggingOn();
  v->SetSourceNID( _LocNodeID );
  v->SetDestNID( _RemNodeID );

  // These all make copies
  v->SetNID(_LocNodeID);
  v->SetPGID(_LocPeerGroupID);
  v->SetRNID(_RemNodeID);
  v->SetRPID(_RemPortID);
  v->SetPID(_LocPortID);
  v->SetHI(_HelloInterval);
  v->SetNHL(0); v->SetULIA(0); v->SetHLE(0); v->SetCPGID(0);
  v->SetUpNodeID(0); v->SetUpNodeAddr(0L);
  v->SetOutPort(_LocPortID);
  v->SetInPort(_LocPortID);
  DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << 
       OwnerName() << " sending a " << v->GetType() << " ("<<v<<").\n";);
  PassVisitorToB(v);
}

// Actually it is with NHL and ULIA
void  HelloState::SendHelloWithNHL(void)
{
  diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "%s : SendHelloWithNHL\n", OwnerName());
  // The VP,VC should be 0,18
  HelloVisitor * v = new HelloVisitor();
  assert(v != 0);
  v->SetLoggingOn();
  // copies are made for src and dst
  v->SetSourceNID( _LocNodeID );  // Makes a copy
  v->SetDestNID( _RemNodeID );    // Makes a copy

  // These methods all make copies
  v->SetNID(_LocNodeID);
  v->SetPGID(_LocPeerGroupID);
  v->SetRNID(_RemNodeID);
  v->SetRPID(_RemPortID);
  v->SetPID(_LocPortID);
  v->SetHI(_HelloInterval);
  v->SetOutPort(_LocPortID);
  v->SetInPort(_LocPortID);
  v->SetAGGR(_DerLinkAggToken);  // Changed 5/13/98 - mountcas

  ig_nodal_hierarchy_list * nhl = 0;
  ig_uplink_info_attr * ulia = 0;

  DBHelloInterface * dbi = (DBHelloInterface *)QueryInterface("Database");
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  nhl  = dbi->ObtainNodalHierarchyList(_LocNodeID);
  ulia = dbi->ObtainUplinkInformationAttribute(_LocNodeID);
  dbi->Unreference();

  if (!nhl) {
    DIAG("fsm.hello.physical", DIAG_DEBUG, 
	 cout << "WARNING:  There is no NHL in the database for " << *_LocNodeID << endl);
  } else
    v->SetNHL(nhl);	// This makes a deep copy of nhl.  DWT 981016.
  
  delete nhl;	// DWT 981016.  Avoids a leak scope error.

  // 03/12/98: if the ULIA is in the Database use it otherwise generate
  // one from RAIGS
  if (!ulia && _theRAIGs) {
    // make a ULIA with copies of the RAIGs
    ig_uplink_info_attr * tmp = new ig_uplink_info_attr(_seqn++);
    v->SetULIA(tmp);
    delete tmp;

    list_item li;
    forall_items(li, *(_theRAIGs)) {
      ig_resrc_avail_info * raig = _theRAIGs->inf(li);
      v->GetULIA()->AddIG(raig->copy());
    }
  } else
    v->SetULIA(ulia);

  DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << 
       OwnerName() << " send a " << v->GetType() << " (" << v << 
       ") with NHL (" << v->GetNHL() 
       << ") and all outgoing RAIGS for this link\n";);
  PassVisitorToB(v);
}

void HelloState::Clear(void)
{
  _RemPortID = 0;
  _RemNodeID = 0;
  _RemPeerGroupID = 0;
  _UpNodeID = 0;
  _CommonPeerGroupID = 0;
  _UpNodeATMaddr = 0;
  _RcvULIASeqNum = 0;
  _RcvNHLSeqNum = 0;
}

InternalHelloState * HelloState::GetCS(void) const { return _cs; }

const int      HelloState::GetVP(void) const { return _vpi; }

const int      HelloState::GetVC(void) const { return _vci; }

const u_short  HelloState::GetVersion(void) const { return _cver; }

const u_short  HelloState::GetNewVersion(void) const { return _nver; }

const NodeID * HelloState::GetRemoteNode(void) const { return _RemNodeID; }

const NodeID * HelloState::GetLocalNode(void) const { return _LocNodeID; }

const int      HelloState::GetRemotePort(void) const { return _RemPortID; }

const int      HelloState::GetLocalPort(void) const { return _LocPortID; }

const PeerID * HelloState::GetRemotePeer(void) const 
{ return _RemPeerGroupID; }

const PeerID * HelloState::GetLocalPeer(void) const { return _LocPeerGroupID; }

const PeerID * HelloState::GetCommonPeer(void) const 
{ return _CommonPeerGroupID; }

const NodeID * HelloState::GetUpNode(void) const { return _UpNodeID; }

const Addr   * HelloState::GetUpNodeATM(void) const { return _UpNodeATMaddr; }

const u_int    HelloState::GetRcvNHLSeqNum(void) const 
{ return _RcvNHLSeqNum; }

const int    HelloState::GetConfLinkAggToken(void) const 
{ return _ConfLinkAggToken; }

void HelloState::SetConfLinkAggToken(int al)
{ _ConfLinkAggToken = al; }

const int    HelloState::GetDerLinkAggToken(void) const 
{ return _DerLinkAggToken; }

void HelloState::SetDerLinkAggToken(int al)
{ _DerLinkAggToken = al; }

const int    HelloState::GetRemLinkAggToken(void) const 
{ return _RemLinkAggToken; }

void HelloState::SetRemLinkAggToken(int al)
{ 
  _RemLinkAggToken = al; 

  // page 133 of the spec
  if (_RemLinkAggToken == _ConfLinkAggToken)
    _DerLinkAggToken = _ConfLinkAggToken;
  else if (_RemLinkAggToken == 0 && _ConfLinkAggToken > 0)
    _DerLinkAggToken = _ConfLinkAggToken;
  else if (_ConfLinkAggToken == 0 && _RemLinkAggToken > 0)
    _DerLinkAggToken = _RemLinkAggToken;
  else if ((_ConfLinkAggToken > 0 && _RemLinkAggToken > 0) &&
	   (_ConfLinkAggToken != _RemLinkAggToken))
    _DerLinkAggToken = 0;
}

const u_int    HelloState::GetRcvULIASeqNum(void) const 
{ return _RcvULIASeqNum; }

const ig_nodal_hierarchy_list     * HelloState::GetNHL(const NodeID * n) const
{ 
  ig_nodal_hierarchy_list * nhl = 0;

  DBHelloInterface * dbi = (DBHelloInterface *)QueryInterface("Database");
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  nhl = dbi->ObtainNodalHierarchyList(n);
  dbi->Unreference();

  return nhl;
}

const list<ig_resrc_avail_info *> * HelloState::GetTheRAIGs(void) const
{  
  return _theRAIGs;  
}

void HelloState::SetRcvNHLSeqNum(u_int i) { _RcvNHLSeqNum = i; }

void HelloState::SetRcvULIASeqNum(u_int i) { _RcvULIASeqNum = i; }

void HelloState::SetTheRAIGs(list<ig_resrc_avail_info *> * r) 
{
  if (r) {
    if (!_theRAIGs) _theRAIGs = new list<ig_resrc_avail_info *> ;
    else _theRAIGs->clear();

    list_item li;
    forall_items(li, *r) {
      ig_resrc_avail_info * raig = r->inf(li);
      _theRAIGs->append((ig_resrc_avail_info *)raig->copy());
    }
  } else {
    if (_theRAIGs) delete _theRAIGs;
    _theRAIGs = 0;
  }
}

void HelloState::SetRemotePort(int port) { _RemPortID = port; }

void HelloState::SetRemoteNode(NodeID * n) { _RemNodeID = n; }

void HelloState::SetRemotePeer(PeerID * p) { _RemPeerGroupID = p; }

void HelloState::SetCommonPeer(PeerID * p) { _CommonPeerGroupID = p; }

void HelloState::SetUpNode(NodeID * n) { _UpNodeID = n; }

void HelloState::SetUpNodeATM(Addr * a) { _UpNodeATMaddr = a; }

void HelloState::SetVersion(u_short v) { _cver = v; }

bool HelloState::PortIsUp(void) const { return _PortIsUP; }

void HelloState::PortIsUp(bool p) { _PortIsUP = p; }

void HelloState::PassThruVis(Visitor * v)
{ PassThru(v); }

void HelloState::PassVisToA(Visitor * v)
{ PassVisitorToA(v); }

void HelloState::PassVisToB(Visitor * v)
{ PassVisitorToB(v); }

Visitor::which_side HelloState::VisFrom(Visitor * v)
{  return VisitorFrom(v);  }

void HelloState::ChangeState(InternalHelloState * ns)
{
  assert( (ns == _StateLinkDown)      ||
	  (ns == _StateAttempt)       ||
	  (ns == _StateOneWayInside)  ||
	  (ns == _StateTwoWayInside)  ||
	  (ns == _StateOneWayOutside) ||
	  (ns == _StateTwoWayOutside) ||
	  (ns == _StateCommon) );
  _cs = ns;
}

// ----------------------- InternalHelloState ----------------------
InternalHelloState::InternalHelloState(void) { }

InternalHelloState::~InternalHelloState() { }

// returns the Transition Name
const char * const InternalHelloState::GetTN(HelloTransitions code)
{
  switch (code) {
    case Invalid:
      return "Invalid"; break;
    case passthru:
      return "passthru"; break;
    case LinkIsUp:
      return "LinkIsUp"; break;
    case WayInside1:
      return "WayInside"; break;
    case WayInside2:
      return "WayInside2"; break;
    case WayOutside1:
      return "WayOutside1"; break;
    case WayOutside2:
      return "WayOutside2"; break;
    case CommHier:
      return "CommHier"; break;
    case HelloMisMatch:
      return "HelloMisMatch"; break;
    case HierMisMatch:
      return "HierMisMatch"; break;
    case HelloTimerExpiry:
      return "HelloTimerExpiry"; break;
    case InactivityTimerExpiry:
      return "InactivityTimerExpiry"; break;
    case LinkIsDown:
      return "LinkIsDown"; break;
    default:
      return "UnKnown"; break;
  }
  return "Ack! An Error has occured!";
}

/*
 * we mainly work on 3 types of Visitors for now:
 * LinkUpVisitor, LinkDownVisitor, and HelloVisitor.
 * The first 2 translate into LinkUp and LinkDown events respectively,
 * while the last one will translate into one of theses events
 *    passthru              = 0,
 *    LinkUp                = 1,
 *    WayInside1            = 2,
 *    WayInside2            = 3,
 *    WayOutside1           = 4,
 *    WayOutside2           = 5,
 *    CommHier              = 6,
 *    HelloMisMatch         = 7,
 *    HierMisMatch          = 8,
 *    HelloTimerExpiry      = 9,
 *    InactivityTimerExpiry = 10,
 *    LinkDown              = 12
 *
 * depending on the current state and the contents of the Hello Packet
 * received.
 */
InternalHelloState::HelloTransitions 
InternalHelloState::GetEventType(HelloState * s, Visitor * v) const
{
  VisitorType vt1 = v->GetType();

  Level  * level = 0;
  PeerID * cpgid = 0;
  NodeID * UpNodeID = 0;
  Addr   * UpNodeAddr = 0;
  HelloTransitions rval = passthru;

  DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout <<
       s->GetName() << " in state " << CurrentStateName() <<
       " received a " << v->GetType() << endl);

  if (vt1.Is_A(HelloState::_hello_type)) { 
    // Until we receive a LinkUpVisitor, all Hellos are ignored
    if (! s->PortIsUp()) {
      DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << 
	   s->GetName() << ": Port is not UP -- suiciding a " 
	   << v->GetType() << " (" << v << ").\n";);
      return Invalid;
    } 
    // Port is UP
    HelloVisitor * hv = (HelloVisitor *)v;
    diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, 
	 "HelloVisitor's (vpi, vci) is (%d,%d)\n", 
	 hv->GetInVP(), hv->GetInVC());
    
    diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, 
	 "%s: HV has NHL: %x  AGGR: %d  ULIA: %x\n", 
	 s->GetName(), hv->GetNHL(), hv->GetAGGR(), hv->GetULIA());
    
    s->SetRemLinkAggToken(hv->GetAGGR());

    if (hv->GetInVP() != s->GetVP() || hv->GetInVC()  != s->GetVC()) {
      // must be a hello for a LGN
      DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << 
	   s->GetName() << " passing through a SVCC " << v->GetType() 
	   << " (" << v << ")" << endl;);
      return passthru;
    }
    // HelloMisMatch has precedence so we consider it first
    if (s->GetVersion() != hv->GetCVER()) {
      diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, 
	   "%s: version does not match: My Version: %d\t His Version: %d\n",
	   s->GetName(), s->GetVersion(), hv->GetCVER());
      return (HelloMisMatch);
    }
    if (s->GetRemoteNode() && 
	(! s->GetRemoteNode()->equals(hv->GetNodeID()))) {
      diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "%s: RemNodeID does not match.\n", 
	   s->GetName());
      return (HelloMisMatch);
    }
    if (s->GetRemotePort() && (s->GetRemotePort() != hv->GetPortID())) {
      diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "%s: RemPortID does not match.\n", 
	   s->GetName());
      return (HelloMisMatch);
    }
    if (s->GetRemotePeer() && 
	(! s->GetRemotePeer()->equals(hv->GetPeerGroupID()))) {
      diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, 
	   "%s: RemPeerGroupID does not match.\n", s->GetName());
      return (HelloMisMatch);
    }
    // OneWayInside & TwoWayInside
    if (s->GetLocalPeer()->equals(hv->GetPeerGroupID())) {
      diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "%s: PeerGroupIDs match.\n", 
	   s->GetName());
      if (((hv->GetRemNodeID() == 0) || (hv->GetRemNodeID()->IsZero())) && 
	  (hv->GetRemPortID() == 0)) {
	diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "%s: We are in OneWayInside.\n", 
	     s->GetName());
	return (WayInside1);
      }
      if ((s->GetVersion() == hv->GetCVER()) &&
	  (s->GetLocalNode()->equals(hv->GetRemNodeID()))  &&
	  (s->GetLocalPort() == hv->GetRemPortID())) {
	diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "%s: We are in TwoWayInside.\n", 
	     s->GetName());
	return (WayInside2);
      }
      DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << 
	   s->GetName() << ": We have a Hello Mismatch.\n";
	   if (s->GetVersion() != hv->GetCVER()) {
	     cout << "   Version numbers do not match " << 
	       s->GetVersion() << " != " << hv->GetCVER() << endl;
	   }
	   if (!(s->GetLocalNode()->equals(hv->GetRemNodeID()))) {
	     cout << "   My NodeID doesn't match his remote NodeID " << endl 
		  << "   " << *(s->GetLocalNode()) 
		  << " != " << endl << "   " 
		  << *(hv->GetRemNodeID()) << endl;
	   }
	   if (s->GetLocalPort() != hv->GetRemPortID()) {
	     cout << "   My port doesn't match his remote port " << 
	       s->GetLocalPort() << " != " << hv->GetRemPortID() << endl;
	   });
      return (HelloMisMatch);
    }
    // Outside
    DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, 
	 cout << s->GetName() << ": PeerGroupIDs do not match." << endl 
	 << "Local " << *(s->GetLocalPeer()) << " != Remote ";
	 if ( hv->GetPeerGroupID() ) cout << *(hv->GetPeerGroupID());
	 else cout << "(null)";
	 cout << endl);
    
    if ((hv->GetRemNodeID() == 0 || 
	 (hv->GetRemNodeID() && hv->GetRemNodeID()->IsZero())) && 
	(hv->GetRemPortID() == 0)) {
      // OneWayOutside
      diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, 
	   "%s: RNID = 0 & RPID = 0 : We are in OneWayOutside.\n",
	   s->GetName());
      return (WayOutside1);
    }
    if (s->GetLocalNode()->equals(hv->GetRemNodeID()) && 
	(s->GetLocalPort() == hv->GetRemPortID())) {
      // this is common to all remaining so do it here, this is a COPY so delete it
      const ig_nodal_hierarchy_list * myNHL = s->GetNHL(s->GetLocalNode());

      if (hv->GetNHL())
	if (myNHL && (level = hv->GetNHL()->FindCommonPGID((ig_nodal_hierarchy_list *)myNHL)))
	  cpgid = level->GetPID();

      delete myNHL;
      
      // test for TwoWayOutside
      if ((s->GetCommonPeer() == 0) &&
	  (s->GetUpNodeATM() == 0)) {
	// case A
	if ((!hv->GetNHL() || hv->GetAGGR() == -1 || !hv->GetULIA()) ||
	    (hv->GetNHL() && !cpgid)) {
	  delete cpgid;
	  return WayOutside2;
	}
      }

      if (hv->GetNHL() && (hv->GetAGGR() != -1) && hv->GetULIA()) {
	// we have an NHL, and AGGR, and a ULIA
	
	/* check for CommonHier event
	 * V second dot: the NHL contains a cpgid already set in DS and
	 * That is the NHL seqnum is same as the one in the DS
	 * NHL contains a common peer group id (cpgid) AND the
	 * CPGID, the UpNodeID, and UpNodeATMAddr in the DS are zero
	 * OR match the ones in the hello
	 */
	if (level) {
	  PeerID * pid = level->GetPID();
	  hv->SetCPGID(pid);	// DWT 981016.  Makes a copy of pid
	  delete pid;		// DWT 981016.  Avoids a leak return error.
	  NodeID * n = level->GetNID();
	  hv->SetUpNodeID(n);
	  delete n;
	  Addr * a = level->GetATM();
	  hv->SetUpNodeAddr(a);
	  delete a;
	}
	if (cpgid &&
	    ((hv->GetNHL()->GetSequenceNum() == s->GetRcvNHLSeqNum()) && 
	     s->GetCommonPeer() != 0 &&
	     (s->GetUpNode() && hv->Get_UpNodeID() &&
	      s->GetUpNode()->equals(hv->Get_UpNodeID())) &&
	     (s->GetUpNodeATM() && hv->Get_UpNodeAddr() &&
	      s->GetUpNodeATM()->equals(hv->Get_UpNodeAddr()))) ||
	    (((s->GetCommonPeer() == 0) &&
	      (s->GetUpNodeATM() == 0))    ||
	     (s->GetCommonPeer()->equals(cpgid)  &&
	      (s->GetUpNodeATM()->equals(hv->Get_UpNodeAddr())) &&
	      (s->GetUpNode()->equals(hv->Get_UpNodeID()))))) {
	  diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, 
	       "%s: We are in CommonOutside.\n", s->GetName());
	  s->SetRcvNHLSeqNum(hv->GetNHL()->GetSequenceNum());
	  rval = CommHier;
	}
	
	if (rval == CommHier) {
	  delete cpgid;
	  return rval;
	}
      }

      // check for HierMismatch
      if ((!hv->GetULIA() || (hv->GetAGGR() == -1) || !hv->GetNHL()) ||
	  ((hv->GetNHL() && (hv->GetNHL()->GetSequenceNum() != s->GetRcvNHLSeqNum())) &&
	   (!cpgid || !cpgid->equals(s->GetCommonPeer()) ||
	    (!s->GetUpNodeATM()->equals(hv->Get_UpNodeAddr())) ||
	    (!s->GetUpNode()->equals(hv->Get_UpNodeID())))) ||
	  (s->GetUpNode() && hv->Get_UpNodeID() && !s->GetUpNode()->equals(hv->Get_UpNodeID())) ||
	  (s->GetUpNodeATM() && hv->Get_UpNodeAddr() && !s->GetUpNodeATM()->equals(hv->Get_UpNodeAddr()))) {
	rval = HierMisMatch;
      }
      if (rval == HierMisMatch) {
	delete cpgid;
	return rval;
      }
    }
    rval = HelloMisMatch;
  }
  if (vt1.Is_A(HelloState::_npflood_type)) {
    // All NPFloodVisitors should NOT leave the switch
    rval = Invalid;
  }
  // this is not a HelloVisitor
  if (vt1.Is_A(HelloState::_linkup_type)) {
    // need to deal with multiple LinkUpVisitors
    if (! s->PortIsUp()) {
      s->PortIsUp(true);
      rval = LinkIsUp;
    }
  }
  if (vt1.Is_A(HelloState::_linkdown_type)) {
    s->PortIsUp(false);
    rval = LinkIsDown;
  }
  if (vt1.Is_A(HelloState::_npstate_type))
    rval = Invalid;

  delete cpgid;
  return rval;
}

int InternalHelloState::LinkUp(HelloState *, Visitor *)         
{ return(0); }

int InternalHelloState::RcvWayInside1(HelloState *, HelloVisitor *)  
{ return(-1); }

int InternalHelloState::RcvWayInside2(HelloState *, HelloVisitor *)  
{ return(-1); }

int InternalHelloState::RcvWayOutside1(HelloState *, HelloVisitor *) 
{ return(-1); }

int InternalHelloState::RcvWayOutside2(HelloState *, HelloVisitor *) 
{ return(-1); }

int InternalHelloState::RcvCommHier(HelloState * s, HelloVisitor * v) 
{ 
  s->ChangeState(HelloState::_StateCommon);
  do_Hp(s, v, 7);
  return 0; 
}
int InternalHelloState::RcvHelloMisMatch(HelloState * s, HelloVisitor * v) 
{ 
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, v, 8);
  return 0; 
}
int InternalHelloState::RcvHierMisMatch(HelloState *, HelloVisitor *)
{ return(-1); }

int InternalHelloState::ExpHelloTimer(HelloState * s) 
{ 
  do_Hp(s, 0, 15); 
  return 0; 
}

int InternalHelloState::ExpInactivityTimer(HelloState * s) 
{ 
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, 0, 8);
  return 0; 
}
int InternalHelloState::LinkDown(HelloState * s, HelloVisitor * v) 
{ 
  s->ChangeState(HelloState::_StateLinkDown);
  do_Hp(s, v, 9);
  return 0; 
}

void InternalHelloState::PassTHRU(HelloState *s, Visitor * v)
{
  s->PassThruVis(v);
}

void InternalHelloState::PassVisitorTOA(HelloState * s, Visitor * v)
{
  s->PassVisToA(v);
}

void InternalHelloState::PassVisitorTOB(HelloState * s, Visitor * v)
{
  s->PassVisToB(v);
}

void InternalHelloState::Handle(HelloState * s, Visitor * v)
{
  HelloTransitions event = GetEventType(s, v);

  if (event != passthru) {
    DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << 
	 s->GetName() << " (" << this << "): In state " << 
	 CurrentStateName() << " received event " << GetTN(event) << ".\n";);
  }

  HelloVisitor * hv = 0;
  if (v->GetType().Is_A(HelloState::_hello_type))
    hv = (HelloVisitor *)v;
  
  switch (event) {
    case WayInside1:
      RcvWayInside1(s, hv);
      break;
    case WayInside2:
      DIAG("fsm.hello", DIAG_INFO, cout <<
	   s->GetName() << ":" << *(s->GetLocalNode()) << 
	   " entered 2WayInside with ";
	   const char * tmp = s->GetRemoteNode() != 0 ? s->GetRemoteNode()->Print() : "(null)";
	   cout << tmp << endl);
      RcvWayInside2(s, hv);
      break;
    case WayOutside1:
      RcvWayOutside1(s, hv);
      break;
    case WayOutside2:
      RcvWayOutside2(s, hv);
      break;
    case HierMisMatch:
      DIAG("fsm.hello", DIAG_INFO, cout << *(s->GetLocalNode()) << 
	   " received Hierarchy Mismatch with ";
	   const char * tmp = s->GetRemoteNode() ? s->GetRemoteNode()->Print() : "(null)";
	   cout << tmp << endl);
      RcvHierMisMatch(s, hv);
      break;
    case LinkIsUp:
      LinkUp(s, (LinkUpVisitor *)v);
      // PassTHRU(s, v);
      v->Suicide();
      hv = 0;
      break;
    case CommHier:
      DIAG("fsm.hello", DIAG_INFO, cout <<
	   s->GetName() << ":" << *(s->GetLocalNode()) << 
	   " entered common hierarchy with ";
	   const char * tmp = s->GetRemoteNode() ? s->GetRemoteNode()->Print() : "(null)";
	   cout << tmp << endl);
      RcvCommHier(s, hv);
      break;
    case HelloMisMatch:
      RcvHelloMisMatch(s, hv);
      break;
    case  LinkIsDown:
      LinkDown(s, hv);
      // PassTHRU(s, v);
      v->Suicide();
      hv = 0;
      break;
    case passthru: 
      {
	VisitorType vt1 = v->GetType();
	if (vt1.Is_A(HelloState::_vpvc_type) && 
	    s->VisFrom(v) == Visitor::B_SIDE) {
	  // if (0,18) hammer SourceNodeID to RemNodeID
	  VPVCVisitor * vpvc = (VPVCVisitor *)v;
	  if (vpvc->GetInVP() == 0 && vpvc->GetInVC() == 18) {
	    if (s->GetRemoteNode()) {
	      vpvc->SetSourceNID( s->GetRemoteNode() );
	      vpvc->SetDestNID( s->GetLocalNode() );
	    }
	  }
	}
	PassTHRU(s, v);
	hv = 0;
      }
      break;
    case Invalid:
      DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, 
	   cout << s->GetName() << ": Unwanted " << v->GetType() << " (" 
	   << v << ").\n");
      break;
    default:
      DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, 
	   cout << s->GetName() << ": Unknown Visitor " << v->GetType()
	   << " (" <<v<< ").\n");
      break;
  }
  // Kill any Visitors that weren't handled properly
  if (hv) {
    VisitorType vt1 = v->GetType();
    if (vt1.Is_A(HelloState::_hello_type)) {
      DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << s->GetName() << 
	   " suicided a " << v->GetType() << " (" << v << ").\n");
      v->Suicide();
    }
  }
}

const char * InternalHelloState::OwnerName(HelloState * s) const
{
  return s->GetName();
}

void InternalHelloState::do_Hp(HelloState * s, Visitor * v, int code)
{
  HelloVisitor      * hv = (HelloVisitor*)v;
  BorderUpVisitor   * buv;
  BorderDownVisitor * bdv;
  PortUpVisitor     * puv;
  PortDownVisitor   * pdv;
  NodeID            * ln = 0, * rn = 0, * un = 0;
  PeerID            * cpgid = 0;

  list<ig_resrc_avail_info *> * tst;

  diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, "%s: Doing Hello Protocol %d.\n", 
       s->GetName(), code);
  switch (code) {
    // descriptive enumerations for the codes .... please
    case 0:
      break;

    case 1:
      // Retain the list of RAIGs from v.
      tst = ((LinkUpVisitor *)v)->GetRAIGs();
      if (tst) {
	s->SetTheRAIGs(tst);
	// We have stolen the raigs from the Visitor we must now clean up
	while (!tst->empty()) {
          ig_resrc_avail_info *the_raig = tst->pop();
	  delete the_raig;
        }
	tst->clear();
        delete tst;	// Without this, we have a leak scope.
      } else {
	MakeBogusRAIGs((LinkUpVisitor*)v);
	s->SetTheRAIGs( ((LinkUpVisitor *)v)->GetRAIGs());
      }
      // Add the two lines below 5/13/98 - mountcas
      s->SetConfLinkAggToken( ((LinkUpVisitor *)v)->GetAgg() );
      s->SetDerLinkAggToken( ((LinkUpVisitor *)v)->GetAgg() );
      s->SendHello();
      s->SetHelloTimer();
      break;

    case 2:
      s->StopInactivityTimer();
      s->SetInactivityTimer();
      assert(v != 0);
      s->SetRemotePort(hv->GetPortID());
      s->SetRemoteNode(hv->GetNodeID()->copy());
      s->SetRemotePeer(hv->GetPeerGroupID()->copy());
      if (hv->GetNVER() > s->GetNewVersion())
	s->SetVersion(s->GetNewVersion());
      else
	s->SetVersion(hv->GetNVER());
      s->SendHello();
      // restart hello timer
      s->StopHelloTimer();
      s->SetHelloTimer();
      break;

    case 3:
      do_Hp(s, v, 2);
      ln = (NodeID *)s->GetLocalNode();
      if (s->GetRemoteNode())
	rn = (NodeID *)s->GetRemoteNode();

      DIAG(FSM_HELLO_PHYSICAL_LINK, DIAG_INFO, 
	   cout << s->GetName()
	   << "Link UP - HLINK "
	   << *ln << " port " << s->GetLocalPort()
	   << " --> ";
	   if (rn != 0) cout << *rn;
	   else cout << "0";
	   cout << " port " << s->GetRemotePort() << endl;);

      // call the NetStatsCollector with type Hello_Up
      theNetStatsCollector().ReportNetEvent("Hello_Up",
					    OwnerName(s),
					    s->_RemNodeID ? s->_RemNodeID->Print() : "TwoWayInside", 
					    s->_LocNodeID);

      //
      // This PortUpVisitor is for DataForwarder and the Control port.
      //
      puv = new PortUpVisitor(0, ln, s->GetLocalPort(), rn, 
			      s->GetRemotePort(), s->GetDerLinkAggToken());
      puv->SetInPort( s->GetLocalPort() );
      puv->SetRAIGs( s->GetTheRAIGs() );
      s->SetTheRAIGs(0);

      DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << 
	   s->GetName() << ": Sending a " << puv->GetType() << " to ACAC.\n";);

      PassVisitorTOA(s, puv);

      //
      // This PortUpVisitor is for Translator in the NNIExpander.
      // Translator uses it to learn if he is master or slave for the
      // purpose of choosing vpi/vci.  Translator needs no RAIGs, so
      // this PortUpVisitor carries none.
      //
      puv = new PortUpVisitor(0, ln, s->GetLocalPort(), rn, 
			      s->GetRemotePort(), 
			      s->GetDerLinkAggToken());
      puv->SetInPort(s->GetLocalPort());
      puv->SetRAIGs(0);
      PassVisitorTOB(s, puv);
      break;

    case 4:
      // restart the inactivity timer
      s->StopInactivityTimer();
      s->SetInactivityTimer();
      // invoke the corresponding NP fsm with AddPort
      ln = (NodeID *)s->GetLocalNode();
      if (s->GetRemoteNode())
	rn = (NodeID *)s->GetRemoteNode();

      DIAG(FSM_HELLO_PHYSICAL_LINK, DIAG_INFO, 
	   cout << s->GetName()
	   << "Link UP - HLINK "
	   << *ln << " port " << s->GetLocalPort()
	   << " --> ";
	   if (rn != 0) cout << *rn;
	   else cout << "0";
	   cout << " port " << s->GetRemotePort() << endl;);

      // call the NetStatsCollector with type Hello_Up
      theNetStatsCollector().ReportNetEvent("Hello_Up",
					    OwnerName(s),
					    s->_RemNodeID ? s->_RemNodeID->Print() : "TwoWayInside", 
					    s->_LocNodeID);

      //
      // This PortUpVisitor is for DataForwarder and the Control port.
      //
      puv = new PortUpVisitor(0, ln, s->GetLocalPort(), rn, 
			      s->GetRemotePort(), 
			      s->GetDerLinkAggToken());
      puv->SetInPort( s->GetLocalPort() );
      puv->SetRAIGs( s->GetTheRAIGs() );
      s->SetTheRAIGs(0);

      DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << 
	   s->GetName() << ": Sending a " << puv->GetType() << " to ACAC.\n";);

      PassVisitorTOA(s, puv);

      //
      // This PortUpVisitor is for Translator in the NNIExpander.
      // Translator uses it to learn if he is master or slave for the
      // purpose of choosing vpi/vci.  Translator needs no RAIGs, so
      // this PortUpVisitor carries none.
      //
      puv = new PortUpVisitor(0, ln, s->GetLocalPort(), rn, 
			      s->GetRemotePort(), 
			      s->GetDerLinkAggToken());
      puv->SetInPort(s->GetLocalPort());
      puv->SetRAIGs(0);
      PassVisitorTOB(s, puv);
      break;

    case 5:
      s->StopInactivityTimer();
      s->SetInactivityTimer();
      s->SetRemotePort(hv->GetPortID());
      s->SetRemoteNode( hv->GetNodeID() ? hv->GetNodeID()->copy() : 0 );
      s->SetRemotePeer( hv->GetPeerGroupID() ? hv->GetPeerGroupID()->copy() : 0 );
      if (hv->GetNVER() > s->GetNewVersion())
	s->SetVersion(s->GetNewVersion());
      else
	s->SetVersion(hv->GetNVER());
      // SendHello() with nodal hierarchy list and all outgoing service
      // category metrics IG's describing this link.
      s->SendHelloWithNHL();
      // restart hello timer
      s->StopHelloTimer();
      s->SetHelloTimer();
      break;

    case 6:
      s->StopInactivityTimer();
      s->SetInactivityTimer();
      s->SetRemotePort(hv->GetPortID());
      s->SetRemoteNode( hv->GetNodeID() ? hv->GetNodeID()->copy() : 0 );
      s->SetRemotePeer( hv->GetPeerGroupID() ? hv->GetPeerGroupID()->copy() : 0 );
      if (hv->GetNVER() > s->GetNewVersion())
	s->SetVersion(s->GetNewVersion());
      else
	s->SetVersion(hv->GetNVER());
      s->SetCommonPeer(hv->Get_CPGID() ? hv->Get_CPGID()->copy() : 0);
      s->SetUpNode(hv->Get_UpNodeID() ? hv->Get_UpNodeID()->copy() : 0);
      s->SetUpNodeATM(hv->Get_UpNodeAddr() ? hv->Get_UpNodeAddr()->copy() : 0);
      // SendHello() with with NHL and all outgoing ULIA's describing
      // this link.
      s->SendHelloWithNHL();
      // restart hello timer
      s->StopHelloTimer();
      s->SetHelloTimer();
      // the link can now be advertized using PTSE's as an uplink to
      // the upnode this BorderUpVisitor is for ACAC to do the
      // advertisement
      ln = (NodeID *)s->GetLocalNode();

      rn = (s->GetRemoteNode()) ? s->GetRemoteNode()->copy() : 0;

      un = (s->GetUpNode()) ? s->GetUpNode()->copy() : 0;

      if (s->GetCommonPeer())
	cpgid = s->GetCommonPeer()->copy();
      else
	cpgid = 0;

      // call the NetStatsCollector with type Hello_Up
      theNetStatsCollector().ReportNetEvent("Hello_Up",
					    OwnerName(s),
					    s->_RemNodeID ? s->_RemNodeID->Print() : "CommonHierarchy", 
					    s->_LocNodeID);

      buv = new BorderUpVisitor(s->GetLocalPort(), (NodeID *)s->GetLocalNode(),
				s->GetLocalPort(), rn, s->GetRemotePort(), un, 
				cpgid, 0, 0, (u_int)s->GetDerLinkAggToken());
      buv->SetRAIGs( s->GetTheRAIGs() );
      buv->SetInPort( s->GetLocalPort() );
      PassVisitorTOA(s, buv);
      break;

    case 7:
      s->StopInactivityTimer();
      s->SetInactivityTimer();
      s->SetCommonPeer(hv->Get_CPGID() ? hv->Get_CPGID()->copy() : 0);
      s->SetUpNode(hv->Get_UpNodeID() ? hv->Get_UpNodeID()->copy() : 0);
      s->SetUpNodeATM(hv->Get_UpNodeAddr() ? hv->Get_UpNodeAddr()->copy() : 0);
      // the link can now be advertized using PTSE's as an uplink to
      // the upnode this BorderUpVisitor is for ACAC to do the
      // advertisement
      ln = (NodeID *)s->GetLocalNode();

      // Wierd, BorderUp/DownVisitor has a VERY BAD interface !!
      //   It makes copies of ln and rn, but not un, this should be fixed eventually.  - mountcas
      rn = (NodeID *)s->GetRemoteNode();

      un = (s->GetUpNode()) ? s->GetUpNode()->copy() : 0;

      // fsm.hello.physical.link
      DIAG(FSM_HELLO_PHYSICAL_LINK, DIAG_INFO, 
	   cout << s->GetName()
	   << "Link UP - BORDER "
	   << *ln << " port " << s->GetLocalPort()
	   << " --> ";
	   if (rn != 0) cout << *rn;
	   else cout << "0";
	   cout << " port " << s->GetRemotePort() << endl;);

      if (s->GetCommonPeer())
	cpgid = s->GetCommonPeer()->copy();
      else
	cpgid = 0;

      // call the NetStatsCollector with type Hello_Up
      theNetStatsCollector().ReportNetEvent("Hello_Up",
					    OwnerName(s),
					    s->_RemNodeID ? s->_RemNodeID->Print() : "CommonHierarchy", 
					    s->_LocNodeID);

      buv = new BorderUpVisitor(s->GetLocalPort(), ln, s->GetLocalPort(),
				rn, s->GetRemotePort(), un, cpgid, 0, 0,
				s->GetDerLinkAggToken());
      delete un;
      delete cpgid;

      buv->SetRAIGs( s->GetTheRAIGs() );
      buv->SetInPort( s->GetLocalPort() );

      DIAG(FSM_HELLO_PHYSICAL, DIAG_DEBUG, cout << 
	   s->GetName() << ": Sending a " << buv->GetType() << " to ACAC.\n";);

      PassVisitorTOA(s, buv);
      break;

    case 8:
      s->StopInactivityTimer();
      s->Clear();
      s->SendHello();
      // restart hello timer
      s->StopHelloTimer();
      s->SetHelloTimer();
      break;

    case 9:
      s->StopHelloTimer();
      s->StopInactivityTimer();
      s->Clear();
      break;

    case 10:
      s->StopInactivityTimer();
      s->SetInactivityTimer();
      s->SendHello();
      s->StopHelloTimer();
      s->SetHelloTimer();
      // invoke the NP fsm with DropPort
      ln = (NodeID *)s->GetLocalNode();
      if (s->GetRemoteNode())
	rn = (NodeID *)s->GetRemoteNode();
      else
	rn = 0;

      DIAG(FSM_HELLO_PHYSICAL_LINK, DIAG_INFO, 
	   cout << s->GetName()
	   << "Link DOWN - HLINK "
	   << *ln << " port " << s->GetLocalPort()
	   << " --> ";
	   if (rn != 0) cout << *rn;
	   else cout << "0";
	   cout << " port " << s->GetRemotePort() << endl;);

      // call the NetStatsCollector with type Hello_Down
      theNetStatsCollector().ReportNetEvent("Hello_Down",
					    OwnerName(s),
					    s->_RemNodeID ? s->_RemNodeID->Print() : "HelloMisMatch", 
					    s->_LocNodeID);

      //
      // This PortDownVisitor is for DataForwarder and the Control port.
      //
      pdv = new PortDownVisitor(0, ln, s->GetLocalPort(), rn, 
				s->GetRemotePort(), 
				s->GetDerLinkAggToken());
      PassVisitorTOA(s, pdv);

      //
      // This PortDownVisitor is for Translator in the NNIExpander.
      // Translator uses it to learn that the port is inactive.
      //
      pdv = new PortDownVisitor(0, ln, s->GetLocalPort(), rn, 
				s->GetRemotePort(), 
				s->GetDerLinkAggToken());
      PassVisitorTOB(s, pdv);
      break;

    case 11:
      s->StopInactivityTimer();
      s->SetInactivityTimer();

      // remove this link from any PTSE originated by this node in
      // which it was advertized as an Uplink by sending a
      // BorderDownVisitor to ACAC
      ln = (NodeID *)s->GetLocalNode();
      if (s->GetRemoteNode())
	rn = (NodeID *)s->GetRemoteNode();
      else
	rn = 0;
      if (s->GetCommonPeer())
	cpgid = s->GetCommonPeer()->copy();
      else
	cpgid = 0;

      DIAG(FSM_HELLO_PHYSICAL_LINK, DIAG_INFO, 
	   cout << s->GetName()
	   << "Link DOWN - BORDER "
	   << *ln << " port " << s->GetLocalPort()
	   << " --> ";
	   if (rn != 0) cout << *rn;
	   else cout << "0";
	   cout << " port " << s->GetRemotePort() << endl;);

      // call the NetStatsCollector with type Hello_Down
      theNetStatsCollector().ReportNetEvent("Hello_Down",
					    OwnerName(s),
					    s->_RemNodeID ? s->_RemNodeID->Print() : "HierarchyMisMatch", 
					    s->_LocNodeID);

      bdv = new BorderDownVisitor(0, ln, s->GetLocalPort(), rn, 
				  s->GetRemotePort(), cpgid, 0, 0,
				  s->GetDerLinkAggToken());
      bdv->SetInPort(s->GetLocalPort());
      PassVisitorTOA(s, bdv);
      s->SetUpNode(0);
      s->SetCommonPeer(0);
      s->SetUpNodeATM(0);
      delete cpgid;
      break;

    case 12:
      diag(FSM_HELLO_PHYSICAL, DIAG_DEBUG, 
	   "%s: Restart Inactivity Timer since we received a Hello.\n",
	   s->GetName());
      s->StopInactivityTimer();
      s->SetInactivityTimer();
      break;

    case 13:
      s->StopInactivityTimer();
      s->SetInactivityTimer();
      s->SetRcvULIASeqNum(0);
      s->SetRcvNHLSeqNum(0);
      // SendHello() with with NHL and all outgoing ULIA's describing
      // this link.
      s->SendHelloWithNHL();
      s->StopHelloTimer();
      s->SetHelloTimer();
      break;

    case 14:
      s->StopInactivityTimer();
      s->SetInactivityTimer();
      // SendHello() with NHL and all outgoing ULIA's describing this
      // link.
      s->SendHelloWithNHL();
      s->StopHelloTimer();
      s->SetHelloTimer();
      // remove this link from any PTSE originated by this node in
      // which it was advertized as an Uplink. Send a
      // BorderDownVisitor to ACAC.
      ln = (NodeID *)s->GetLocalNode();
      if (s->GetRemoteNode())
	rn = (NodeID *)s->GetRemoteNode();
      else
	rn = 0;
      if (s->GetCommonPeer())
	cpgid = s->GetCommonPeer()->copy();
      else
	cpgid = 0;

      DIAG(FSM_HELLO_PHYSICAL_LINK, DIAG_INFO, 
	   cout << s->GetName()
	   << "Link DOWN - BORDER "
	   << *ln << " port " << s->GetLocalPort()
	   << " --> ";
	   if (rn != 0) cout << *rn;
	   else cout << "0";
	   cout << " port " << s->GetRemotePort() << endl;);

      // call the NetStatsCollector with type Hello_Down
      theNetStatsCollector().ReportNetEvent("Hello_Down",
					    OwnerName(s),
					    s->_RemNodeID ? s->_RemNodeID->Print() : "HierarchyMisMatch", 
					    s->_LocNodeID);

      bdv = new BorderDownVisitor(0, ln, s->GetLocalPort(), rn,
				  s->GetRemotePort(), cpgid, 0, 0,
				  s->GetDerLinkAggToken());
      bdv->SetInPort(s->GetLocalPort());
      PassVisitorTOA(s, bdv);
      s->SetUpNode(0);
      s->SetCommonPeer(0);
      s->SetUpNodeATM(0);
      s->SetRcvULIASeqNum(0);
      s->SetRcvNHLSeqNum(0);
      break;

    case 15:
      s->SendHello();
      s->StopHelloTimer();
      s->SetHelloTimer();
      break;

    case 16:
      // Invoke the NP fsm with DropPort by sending it a PortDownVisitor.
      ln = (NodeID *)s->GetLocalNode();
      if (s->GetRemoteNode())
	rn = (NodeID *)s->GetRemoteNode();
      else
	rn = 0;
      
      DIAG(FSM_HELLO_PHYSICAL_LINK, DIAG_INFO, 
	   cout << s->GetName()
	   << "Link DOWN - HLINK "
	   << *ln << " port " << s->GetLocalPort()
	   << " --> ";
	   if (rn != 0) cout << *rn;
	   else cout << "0";
	   cout << " port " << s->GetRemotePort() << endl;);

      // call the NetStatsCollector with type Hello_Down
      theNetStatsCollector().ReportNetEvent("Hello_Down",
					    OwnerName(s),
					    s->_RemNodeID ? s->_RemNodeID->Print() : "HelloMisMatch", 
					    s->_LocNodeID);

      //
      // This PortDownVisitor is for DataForwarder and the Control port.
      //
      pdv = new PortDownVisitor(0, ln, s->GetLocalPort(), rn, 
				s->GetRemotePort(), 
				s->GetDerLinkAggToken());
      PassVisitorTOA(s, pdv);

      //
      // This PortDownVisitor is for Translator in the NNIExpander.
      // Translator uses it to learn that the port is inactive.
      //
      pdv = new PortDownVisitor(0, ln, s->GetLocalPort(), rn, 
				s->GetRemotePort(), 
				s->GetDerLinkAggToken());
      PassVisitorTOB(s, pdv);
      do_Hp(s, v, 8);
      break;

    case 17:
      // remove this link from any PTSE originated by this node in
      // which it was advertized as an Uplink. Tell ACAC again
      ln = (NodeID *)s->GetLocalNode();
      if (s->GetRemoteNode())
	rn = (NodeID *)s->GetRemoteNode();
      else
	rn = 0;
      if (s->GetCommonPeer())
	cpgid = s->GetCommonPeer()->copy();
      else
	cpgid = 0;

      DIAG(FSM_HELLO_PHYSICAL_LINK, DIAG_INFO, 
	   cout << s->GetName()
	   << "Link DOWN - BORDER "
	   << *ln << " port " << s->GetLocalPort()
	   << " --> ";
	   if (rn != 0) cout << *rn;
	   else cout << "0";
	   cout << " port " << s->GetRemotePort() << endl;);

      // call the NetStatsCollector with type Hello_Down
      theNetStatsCollector().ReportNetEvent("Hello_Down",
					    OwnerName(s),
					    s->_RemNodeID ? s->_RemNodeID->Print() : "HierarchyMisMatch", 
					    s->_LocNodeID);

      bdv = new BorderDownVisitor(0, ln, s->GetLocalPort(), rn,
				  s->GetRemotePort(), cpgid, 0, 0,
				  s->GetDerLinkAggToken());
      bdv->SetInPort(s->GetLocalPort());
      PassVisitorTOA(s, bdv);
      do_Hp(s, v, 8);
      break;

    case 18:
      // Invoke the NP fsm with DropPort by sending it a PortDownVisitor.
      ln = (NodeID *)s->GetLocalNode();
      if (s->GetRemoteNode())
	rn = (NodeID *)s->GetRemoteNode();
      else
	rn = 0;

      DIAG(FSM_HELLO_PHYSICAL_LINK, DIAG_INFO, 
	   cout << s->GetName()
	   << "Link DOWN - HLINK "
	   << *ln << " port " << s->GetLocalPort()
	   << " --> ";
	   if (rn != 0) cout << *rn;
	   else cout << "0";
	   cout << " port " << s->GetRemotePort() << endl;);

      // call the NetStatsCollector with type Hello_Down
      theNetStatsCollector().ReportNetEvent("Hello_Down",
					    OwnerName(s),
					    s->_RemNodeID ? s->_RemNodeID->Print() : "HelloMisMatch", 
					    s->_LocNodeID);

      //
      // This PortDownVisitor is for DataForwarder and the Control port.
      //
      pdv = new PortDownVisitor(0, ln, s->GetLocalPort(), rn, 
				s->GetRemotePort(), s->GetDerLinkAggToken());
      PassVisitorTOA(s, pdv);

      //
      // This PortDownVisitor is for Translator in the NNIExpander.
      // Translator uses it to learn that the port is inactive.
      //
      pdv = new PortDownVisitor(0, ln, s->GetLocalPort(), rn, 
				s->GetRemotePort(), s->GetDerLinkAggToken());
      PassVisitorTOB(s, pdv);
      do_Hp(s, v, 9);
      break;

    case 19:
      // remove this link from any PTSE originated by this node in
      // which it was advertized as an Uplink. Tell ACAC again.
      ln = (NodeID *)s->GetLocalNode();
      if (s->GetRemoteNode())
	rn = (NodeID *)s->GetRemoteNode();
      else
	rn = 0;
      if (s->GetCommonPeer())
	cpgid = s->GetCommonPeer()->copy();
      else
	cpgid = 0;

      DIAG(FSM_HELLO_PHYSICAL_LINK, DIAG_INFO, 
	   cout << s->GetName()
	   << "Link DOWN - BORDER "
	   << *ln << " port " << s->GetLocalPort()
	   << " --> ";
	   if (rn != 0) cout << *rn;
	   else cout << "0";
	   cout << " port " << s->GetRemotePort() << endl;);

      // call the NetStatsCollector with type Hello_Down
      theNetStatsCollector().ReportNetEvent("Hello_Down",
					    OwnerName(s),
					    s->_RemNodeID ? s->_RemNodeID->Print() : "HierarchyMisMatch", 
					    s->_LocNodeID);

      bdv = new BorderDownVisitor(0, ln, s->GetLocalPort(), rn,
				  s->GetRemotePort(), cpgid, 0, 0,
				  s->GetDerLinkAggToken());
      bdv->SetInPort(s->GetLocalPort());
      PassVisitorTOA(s, bdv);
      do_Hp(s, v, 9);
      break;

    case 20: // Iam in CommonOutside State and Got a CommonHierarchy event
      s->StopInactivityTimer();
      s->SetInactivityTimer();
      // if the ULIA seq_num does not match the recieved one update
      // the Uplink advertisement with the recieved ULIA and
      // re-originate it subject to PTSE holdown.
      if (hv->GetULIA()) {
	if (hv->GetULIA()->GetSequenceNum() != s->GetRcvULIASeqNum()) {
	  s->SetRcvULIASeqNum(hv->GetULIA()->GetSequenceNum());
	  // advertize this new ULIA thru ACAC
	  ln = (NodeID *)s->GetLocalNode();

	  rn = (s->GetRemoteNode()) ? s->GetRemoteNode()->copy() : 0;
	  
	  un = (s->GetUpNode()) ? s->GetUpNode()->copy() : 0;

	  if (s->GetCommonPeer())
	    cpgid = s->GetCommonPeer()->copy();
	  else
	    cpgid = 0;

	  // call the NetStatsCollector with type Hello_Up
	  theNetStatsCollector().ReportNetEvent("Hello_Up",
						OwnerName(s),
						s->_RemNodeID ? s->_RemNodeID->Print() : "CommonHierarchy", 
						s->_LocNodeID);

	  buv = new BorderUpVisitor(s->GetLocalPort(), ln, s->GetLocalPort(), 
				    rn, s->GetRemotePort(), un, cpgid, 
				    hv->GetULIA(), 
				    0, s->GetDerLinkAggToken());
	  buv->SetRAIGs( s->GetTheRAIGs() ); // This makes a copy
	  buv->SetInPort( s->GetLocalPort() );
	  PassVisitorTOA(s, buv);
	}
      }
      break;
  }
}

// ---------------------- LinkDown ------------------------
StateLinkDown::StateLinkDown(void) : InternalHelloState() { }
StateLinkDown::~StateLinkDown() { }

int StateLinkDown::LinkDown(HelloState *, HelloVisitor *)
{ return  0; }

int StateLinkDown::RcvWayInside1(HelloState *, HelloVisitor *v)    
{ return  0; }

int StateLinkDown::RcvWayOutside1(HelloState *, HelloVisitor *v)   
{ return  0; }

int StateLinkDown::RcvCommHier(HelloState *, HelloVisitor *v)      
{ return -1; }

int StateLinkDown::RcvHelloMisMatch(HelloState *, HelloVisitor *v) 
{ return -1; }

int StateLinkDown::ExpHelloTimer(HelloState *)               
{ return -1; }

int StateLinkDown::ExpInactivityTimer(HelloState *)
{ return -1; }

const char * StateLinkDown::CurrentStateName(void) const { return "LinkDown"; }

int StateLinkDown::LinkUp(HelloState * s, Visitor * v)
{
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, v, 1);
  return(0);
}

// ---------------------- Attempt ------------------------
StateAttempt::StateAttempt(void) { }
StateAttempt::~StateAttempt() { }

int StateAttempt::LinkDown(HelloState * s, HelloVisitor * v)
{
  s->ChangeState(HelloState::_StateLinkDown);
  do_Hp(s, v, 9);
  return 0;
}

int StateAttempt::RcvWayInside1(HelloState * s, HelloVisitor * v)
{
  s->ChangeState(HelloState::_StateOneWayInside);
  do_Hp(s, v, 2);
  return 0;
}

int StateAttempt::RcvWayInside2(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateTwoWayInside);
  do_Hp(s, v, 3);
  return 0;
}
int StateAttempt::RcvWayOutside1(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateOneWayOutside);
  do_Hp(s, v, 5);
  // if can't become a border node: do Hp0 and stay in Attempt
  return 0;
}

int StateAttempt::RcvWayOutside2(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateTwoWayOutside);
  do_Hp(s, v, 5);
  // if can't become a border node: do Hp0 and stay in Attempt
  return 0;
}

int StateAttempt::RcvCommHier(HelloState * s, HelloVisitor * v)
{
  s->ChangeState(HelloState::_StateLinkDown);
  do_Hp(s, v, 6);
  // if node can't become a border node: do Hp0 and stay in Attempt
  return 0;
}

int StateAttempt::RcvHelloMisMatch(HelloState * s, HelloVisitor * v)
{  return 0;  }

int StateAttempt::ExpHelloTimer(HelloState * s)
{
  do_Hp(s, 0, 15);
  return 0;
}

int StateAttempt::LinkUp(HelloState * s, Visitor * v) 
{ return  0; }

int StateAttempt::ExpInactivityTimer(HelloState *)
{ return -1; }

int StateAttempt::RcvHierMisMatch(HelloState * s, HelloVisitor *v) 
{ return -1; }

const char * StateAttempt::CurrentStateName(void) const { return "Attempt"; }

// -------------------------- One Way Inside -----------------------
State1WayInside::State1WayInside(void) { }
State1WayInside::~State1WayInside()    { }

int State1WayInside::LinkUp(HelloState * s, Visitor *v)
{
  return(0);
}
int State1WayInside::RcvWayInside1(HelloState * s, HelloVisitor *v)
{
  do_Hp(s, v, 12);
  return(0);
}
int State1WayInside::RcvWayInside2(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateTwoWayInside);
  do_Hp(s, v, 4);
  return 0;
}

int State1WayInside::RcvWayOutside1(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State1WayInside::RcvWayOutside2(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State1WayInside::RcvCommHier(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State1WayInside::RcvHelloMisMatch(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, v, 8);
  return 0;
}

int State1WayInside::RcvHierMisMatch(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State1WayInside::ExpHelloTimer(HelloState * s)
{
  do_Hp(s, 0, 15);
  return 0;
}

int State1WayInside::ExpInactivityTimer(HelloState * s)
{
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, 0, 8);
  return 0;
}

int State1WayInside::LinkDown(HelloState * s, HelloVisitor * v)
{
  s->ChangeState(HelloState::_StateLinkDown);
  do_Hp(s, v, 9);
  return 0;
}

const char * State1WayInside::CurrentStateName(void) const 
{ return "OneWayInside"; }

// --------------------------- Two Way Inside ---------------------------
State2WayInside::State2WayInside(void) { }
State2WayInside::~State2WayInside()    { }

int State2WayInside::LinkUp(HelloState * s, Visitor *v)
{
  return(0);
}

int State2WayInside::RcvWayInside1(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateOneWayInside);
  do_Hp(s, v, 10);
  return 0;
}

int State2WayInside::RcvWayInside2(HelloState * s, HelloVisitor *v)
{
  do_Hp(s, v, 12);
  return(0);
}

int State2WayInside::RcvWayOutside1(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State2WayInside::RcvWayOutside2(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State2WayInside::RcvCommHier(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State2WayInside::RcvHelloMisMatch(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, v, 16);
  return 0;
}

int State2WayInside::RcvHierMisMatch(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State2WayInside::ExpHelloTimer(HelloState * s)
{
  do_Hp(s, 0, 15);
  return 0;
}

int State2WayInside::ExpInactivityTimer(HelloState * s)
{
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, 0, 16);
  return 0;
}

int State2WayInside::LinkDown(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateLinkDown);
  do_Hp(s, v, 18);
  return 0;
}
const char * State2WayInside::CurrentStateName(void) const 
{ return "TwoWayInside"; }

// ------------------------ One Way Outside ----------------------------
State1WayOutside::State1WayOutside(void) { }
State1WayOutside::~State1WayOutside() { }

int State1WayOutside::LinkUp(HelloState * s, Visitor *v)
{
  return(0);
}

int State1WayOutside::RcvWayInside1(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State1WayOutside::RcvWayInside2(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State1WayOutside::RcvWayOutside1(HelloState * s, HelloVisitor *v)
{
  do_Hp(s, v, 12);
  return 0;
}

int State1WayOutside::RcvWayOutside2(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateTwoWayOutside);
  do_Hp(s, v, 12);
  return 0;
}

int State1WayOutside::RcvCommHier(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateCommon);
  do_Hp(s, v, 7);
  return 0;
}

int State1WayOutside::RcvHelloMisMatch(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, v, 8);
  return 0;
}

int State1WayOutside::RcvHierMisMatch(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State1WayOutside::ExpHelloTimer(HelloState * s)
{
  do_Hp(s, 0, 15);
  return 0;
}

int State1WayOutside::ExpInactivityTimer(HelloState * s)
{
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, 0, 8);
  return 0;
}

int State1WayOutside::LinkDown(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateLinkDown);
  do_Hp(s, v, 9);
  return 0;
}
const char * State1WayOutside::CurrentStateName(void) const 
{ return "OneWayOutside"; }

// ---------------------- Two Way Outside ------------------------
State2WayOutside::State2WayOutside(void) { }
State2WayOutside::~State2WayOutside() { }

int State2WayOutside::LinkUp(HelloState * s, Visitor *v)
{
  return(0);
}

int State2WayOutside::RcvWayInside1(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State2WayOutside::RcvWayInside2(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State2WayOutside::RcvWayOutside1(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateOneWayOutside);
  do_Hp(s, v, 13);
  return 0;
}

int State2WayOutside::RcvWayOutside2(HelloState * s, HelloVisitor *v)
{
  do_Hp(s, v,12);
  return 0;
}

int State2WayOutside::RcvCommHier(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateCommon);
  do_Hp(s, v,7);
  return 0;
}

int State2WayOutside::RcvHelloMisMatch(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, v, 8);
  return 0;
}

int State2WayOutside::RcvHierMisMatch(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int State2WayOutside::ExpHelloTimer(HelloState * s)
{
  do_Hp(s, 0, 15);
  return 0;
}

int State2WayOutside::ExpInactivityTimer(HelloState * s)
{
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, 0, 8);
  return 0;
}

int State2WayOutside::LinkDown(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateLinkDown);
  do_Hp(s, v, 9);
  return 0;
}
const char * State2WayOutside::CurrentStateName(void) const 
{ return "TwoWayOutside"; }

// ---------------------------- Common ------------------------
StateCommon::StateCommon(void) { }
StateCommon::~StateCommon() { }

int StateCommon::LinkUp(HelloState * s, Visitor *v)
{
  return(0);
}

int StateCommon::RcvWayInside1(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int StateCommon::RcvWayInside2(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int StateCommon::RcvWayOutside1(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateOneWayOutside);
  do_Hp(s, v, 14);
  return 0;
}

int StateCommon::RcvWayOutside2(HelloState * s, HelloVisitor *v)
{
  return(-1);
}

int StateCommon::RcvCommHier(HelloState * s, HelloVisitor *v)
{
  do_Hp(s, v, 20);
  return 0;
}

int StateCommon::RcvHelloMisMatch(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, v, 17);
  return 0;
}

int StateCommon::RcvHierMisMatch(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateTwoWayOutside);
  do_Hp(s, v, 11);
  return 0;
}

int StateCommon::ExpHelloTimer(HelloState * s)
{
  do_Hp(s, 0, 15);
  return 0;
}

int StateCommon::ExpInactivityTimer(HelloState * s)
{
  s->ChangeState(HelloState::_StateAttempt);
  do_Hp(s, 0, 17);
  return 0;
}

int StateCommon::LinkDown(HelloState * s, HelloVisitor *v)
{
  s->ChangeState(HelloState::_StateLinkDown);
  do_Hp(s, v, 19);
  return 0;
}
const char * StateCommon::CurrentStateName(void) const 
{ return "CommonHierarchy"; }

// -----------------------------------------------------------------------------
void MakeBogusRAIGs(LinkUpVisitor * luv)
{
  ig_resrc_avail_info * rai = CreateRAIG(OC3);
  luv->AddRAIG( rai );  // This doesn't make a copy

  diag(FSM_HELLO_PHYSICAL, DIAG_WARNING, 
       "Manufactured Bogus RAIGs because of vacuous LinkUpVisitor.\n");
}
