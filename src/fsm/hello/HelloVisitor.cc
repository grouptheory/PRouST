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
static char const _HelloVisitor_cc_rcsid_[] =
"$Id: HelloVisitor.cc,v 1.106 1999/01/25 13:45:06 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/Visitor.h>
#include <FW/behaviors/Mux.h>
#include <FW/actors/Accessor.h>
#include <codec/pnni_pkt/pkt_incl.h>
#include <FW/basics/VisitorType.h>
#include <FW/basics/Conduit.h>
#include <fsm/hello/HelloVisitor.h>
#include <FW/basics/diag.h>

HelloVisitor::HelloVisitor(HelloType htype, HelloPkt * hp, 
			   int port, u_int vc, u_char cver) :
  PNNIVisitor(_my_type, port, -1, vc, 0, 0), _htype(htype), _cver(cver),
  _over(cver), _nver(cver), _hp(hp), _NID(0), _RNID(0), _PGID(0), _RPGID(0),
  _NHL(0), _ULIA(0), _HLE(0), _CPGID(0), _UpNodeID(0), _UpNodeAddr(0), 
  _AGGR(-1), _HI(15)
{
  SetInVP(0); SetOutVP(0); SetInVC(vc); SetOutVC(vc);
  SetLoggingOn();

  if (! _hp )
    _hp = new HelloPkt( );
}

HelloVisitor::HelloVisitor(const HelloVisitor & rhs) :
  PNNIVisitor(rhs), _htype(rhs._htype), _cver(rhs._cver),
  _over(rhs._over), _nver(rhs._nver), _hp(rhs._hp), _NID(0), 
  _RNID(0), _PGID(0), _RPGID(0), _NHL(0), _ULIA(0), _HLE(0), 
  _CPGID(0), _UpNodeID(0), _UpNodeAddr(0), _AGGR(-1), _HI(15)
{
  if (rhs._NID)
    _NID = new NodeID(*(rhs._NID));
  if (rhs._RNID)
    _RNID = new NodeID(*(rhs._RNID));
  if (rhs._PGID)
    _PGID = new PeerID(*(rhs._PGID));
  if (rhs._RPGID)
    _RPGID = new PeerID(*(rhs._RPGID));
  if (! _hp )
    _hp = new HelloPkt( );
}

HelloVisitor::HelloVisitor(vistype & child_type, HelloType htype, HelloPkt *hp, int port,
			   u_int vc, u_char cver) :
  PNNIVisitor(child_type.derived_from(_my_type), port, -1, vc, 0, 0), _htype(htype), 
  _cver(cver), _over(cver), _nver(cver), _hp(hp), _NID(0), _RNID(0), _PGID(0), _RPGID(0),
  _NHL(0), _ULIA(0), _HLE(0), _CPGID(0), _UpNodeID(0), _UpNodeAddr(0), 
  _AGGR(-1), _HI(15)
{
  SetInVP(0); SetOutVP(0); SetInVC(vc); SetOutVC(vc);
  SetLoggingOn();

  if (! _hp )
    _hp = new HelloPkt( );
}

Visitor * HelloVisitor::dup(void) const { return new HelloVisitor(*this); }

HelloVisitor::~HelloVisitor()
{
  if (_NID)        delete _NID;        _NID = 0;
  if (_RNID)       delete _RNID;       _RNID = 0;
  if (_PGID)       delete _PGID;       _PGID = 0;
  if (_RPGID)      delete _RPGID;      _RPGID = 0;
  if (_NHL)        delete _NHL ;       _NHL = 0;
  if (_ULIA)       delete _ULIA;       _ULIA = 0;
  if (_HLE)        delete _HLE;        _HLE = 0;
  if (_CPGID)      delete _CPGID;      _CPGID = 0;
  if (_UpNodeID)   delete _UpNodeID;   _UpNodeID = 0;
  if (_UpNodeAddr) delete _UpNodeAddr; _UpNodeAddr = 0;
  // if (_hp)         delete _hp;         _hp = 0;
}

void HelloVisitor::SetNHL(ig_nodal_hierarchy_list * n)
{
  if (n) _NHL = (ig_nodal_hierarchy_list *)n->copy();
  else _NHL = 0;
}
void HelloVisitor::SetULIA(ig_uplink_info_attr * u)
{
  if (u) _ULIA = (ig_uplink_info_attr *)u->copy();
  else _ULIA = 0;
}

void HelloVisitor::SetHLE(ig_lgn_horizontal_link_ext * h)
{
  if (h) _HLE = (ig_lgn_horizontal_link_ext *)h->copy();
  else _HLE = 0;
}

void HelloVisitor::SetNID(const NodeID * n)
{
  if (n) _NID = n->copy();
  else _NID = 0;
}

void HelloVisitor::SetPGID(const PeerID * p)
{
  if (p) _PGID = p->copy();
  else _PGID = 0;
}

void HelloVisitor::SetRNID(const NodeID * n)
{
  if (n) _RNID = n->copy();
  else _RNID = 0;
}

void HelloVisitor::SetRPID(const int rp)
{
  _RPID = rp;
}

void HelloVisitor::SetPID(const int lp)
{
  _PID = lp;
}

void HelloVisitor::SetHI(const int hi)
{
  _HI = hi;
}

void HelloVisitor::SetCPGID(const PeerID * c)
{
  if (c) _CPGID = c->copy();
  else _CPGID = 0;
}

void HelloVisitor::SetAGGR(const int a)
{
  _AGGR = a;
}

void HelloVisitor::SetUpNodeID(const NodeID * n)
{
  if (n) _UpNodeID = n->copy();
  else _UpNodeID = 0;
}

void HelloVisitor::SetUpNodeAddr(const Addr * a)
{
  if (a) _UpNodeAddr = a->copy();
  else _UpNodeAddr = 0;
}

const VisitorType HelloVisitor::GetType(void) const
{ 
  return VisitorType(GetClassType());
}

const vistype & HelloVisitor::GetClassType(void) const
{
  return _my_type;
}

/* Copy the contents of the
 * hello packet into the HelloVisitor for easy access.
 */
void HelloVisitor::PacketToVisitor(HelloPkt * hp)
{
  assert(hp != 0);

  SetHI(hp->GetHelloInterval());
  SetNID(hp->GetNodeID());
  SetPGID(hp->GetPeerGroupID());
  SetPID(hp->GetPortID());
  SetRNID(hp->GetRemoteNodeID());
  SetRPID(hp->GetRemotePortID());
  SetAGGR((hp->GetAggregationToken() ? 
	   (((ig_aggregation_token *)hp->GetAggregationToken())->GetToken()) : -1));
  SetNHL((ig_nodal_hierarchy_list *)hp->GetNodalHierarchyList());
  SetULIA((ig_uplink_info_attr *)hp->GetUplinkInfoAttr());
  SetHLE((ig_lgn_horizontal_link_ext *)hp->GetLGNHorizontalLinkExt());

  _cver = (u_char)hp->GetCurrentVer();
  _nver = (u_char)hp->GetNewVer();
  _over = (u_char)hp->GetOldVer();
}

/* Copy the contents of the HelloVisitor into 
 * the hello packet so it can be encoded.
 */
void HelloVisitor::VisitorToPacket(void) const
{
  //  if(!_hp)
  //    _hp = new HelloPkt(_NID,_RNID,_PID,_RPID,_HI);
  assert( _hp != 0 );

  if (_AGGR != -1)
    _hp->SetAggregationToken(new ig_aggregation_token(_AGGR));

  _hp->SetPortID( _PID );
  _hp->SetRemotePortID( _RPID );
  _hp->SetHelloInterval( _HI );
  if (_NID)
    _hp->SetNodeID( (unsigned char *)_NID->GetNID() );
  if (_RNID)
    _hp->SetRemoteNodeID( (unsigned char *)_RNID->GetNID() );
  if (_PGID)
    _hp->SetPeerGroupID( (unsigned char *)_PGID->GetPGID() );
  if (_NHL)
    _hp->SetNodalHierarchyList(_NHL);
  if (_ULIA)
    _hp->SetUplinkInfoAttr(_ULIA);
  if (_HLE)
    _hp->SetLGNHorizontalLinkExt(_HLE);
}


void HelloVisitor::HelloDump(void)
{
  if (_NID)
    cout << " NodeID: " << *_NID << endl;
  if (_PGID)
    cout << " PGID: " << *_PGID << endl;
  cout << " portID: "         << _PID            << endl;
  if (_RNID)
    cout << " RemoteNodeID: " << *_RNID << endl;
  cout << " RemPortID: "      << _RPID           << endl;
  cout << " AggrToken: "      << _AGGR           << endl;
}

HelloVisitor::HelloType HelloVisitor::GetHelloType(void) {return _htype;}

// retrieve hello contents
int HelloVisitor::GetAGGR(void) { return _AGGR; }

const NodeID * HelloVisitor::GetNodeID(void) { return _NID; }

int  HelloVisitor::GetPortID(void) { return _PID; }

const NodeID * HelloVisitor::GetRemNodeID(void) { return _RNID; }

int  HelloVisitor::GetRemPortID(void) { return _RPID; }

const PeerGroupID * HelloVisitor::GetRemPeerGroupID(void) { return _RPGID; }

const PeerGroupID * HelloVisitor::GetPeerGroupID(void) { return _PGID; }

int HelloVisitor::GetHelloInt(void) { return _HI; }

int HelloVisitor::GetCVER(void) { return _cver; }

int HelloVisitor::GetNVER(void) { return _nver; }

int HelloVisitor::GetOVER(void) { return _over; }

void HelloVisitor::ClearDS(void)
{
  if (_hp) 
    delete _hp;

  _hp     = 0;
  _len    = 0;   _cver = 0;      _nver = 0;
  _over   = 0;   _AGGR = 0;      _HI = 0;
  _NID    = 0;   _PID = 0;       _RNID = 0;
  _RPID   = 0;   _PGID = 0;      _RPGID = 0;
  _CPGID  = 0;   _UpNodeID = 0;  _UpNodeAddr = 0;
  _NHL    = 0;   _ULIA = 0;
  _HLE    = 0;
}

ig_nodal_hierarchy_list * HelloVisitor::GetNHL(void) { return _NHL; }

ig_uplink_info_attr * HelloVisitor::GetULIA(void) { return _ULIA; }

ig_lgn_horizontal_link_ext * HelloVisitor::GetHLE(void) { return _HLE; }

const PeerID * HelloVisitor::Get_CPGID(void) { return _CPGID; }

const NodeID      * HelloVisitor::Get_UpNodeID(void) { return _UpNodeID; }

const Addr        * HelloVisitor::Get_UpNodeAddr(void) { return _UpNodeAddr; }

void HelloVisitor::SetHelloPkt(HelloPkt * hp) { _hp = hp; }

const HelloPkt * HelloVisitor::GetHelloPkt(void) const { return _hp; }

bool HelloVisitor::encode(unsigned char *& buffer, int & len) const
{
  bool rval = false;
  VisitorToPacket();
  if (_hp != 0 && ( _hp->encode( buffer, len ) ))
    rval == true;
  return rval;
}

bool HelloVisitor::decode(const unsigned char *& buffer, int & len)
{
  bool rval = false;

  _hp = new HelloPkt( );
  if(!_hp->decode(buffer))
    {
      PacketToVisitor(_hp);
      HelloDump();
      rval = true;
    }
  return rval;
}

// --------------------- HorLinkVisitor -----------------------
HorLinkVisitor::HorLinkVisitor(HLVisitorTypes hlvtype, const NodeID * src,
                               const NodeID * dst, const NodeID * borderNode,
                               int borderPort, int aggrToken, int localPort,
                               int remotePort, bool broadcast)
  : VPVCVisitor(_my_type, 0, aggrToken, 0, src, dst),
    _vt(hlvtype), _borderNode(0), _borderPort(borderPort), 
    _broadCast(broadcast), _localPort(localPort), _remotePort(remotePort)
{
  if (borderNode)
    _borderNode = borderNode->copy();
}

HorLinkVisitor::HorLinkVisitor(const HorLinkVisitor & rhs) 
  : VPVCVisitor(rhs), _vt(rhs._vt), _borderPort(rhs._borderPort), 
    _broadCast(rhs._broadCast), _localPort(rhs._localPort),
    _remotePort(rhs._remotePort), _borderNode(0)
{
  if (rhs._borderNode)
    _borderNode = new NodeID(*(rhs._borderNode));
}

HorLinkVisitor::HorLinkVisitor(vistype & child_type, HLVisitorTypes hlvtype,
                               int aggr, int borderPort, int aggrToken ,
                               const NodeID * borderNode, const NodeID *src,
                               const NodeID *dst, int localPort,
                               int remotePort, bool broadcast) 
  : VPVCVisitor(child_type.derived_from(_my_type), 0, 
		aggrToken, 0, (NodeID *)src, (NodeID *)dst), 
    _vt(hlvtype), _borderPort(borderPort), 
    _borderNode(0), _localPort(localPort),
    _remotePort(remotePort), _broadCast(broadcast)
{
  if (borderNode)
    _borderNode = borderNode->copy();
}

HorLinkVisitor::~HorLinkVisitor() 
{ 
  // Temporary, so I can see who's killing them off
  //  diag("fsm.hello.HorLinkVisitor", DIAG_DEBUG, "A pox on you and your decendants for killing me!\n");
  delete _borderNode;
}

Visitor * HorLinkVisitor::dup(void) const { return new HorLinkVisitor(*this); }

const VisitorType HorLinkVisitor::GetType(void) const
{  return VisitorType(GetClassType());  }

const vistype & HorLinkVisitor::GetClassType(void) const
{  return _my_type;  }

HorLinkVisitor::HLVisitorTypes HorLinkVisitor::GetVT(void)
{  return _vt;  }

void HorLinkVisitor::SetVT(HorLinkVisitor::HLVisitorTypes vt)
{  _vt = vt;  }

const char * HorLinkVisitor::PrintVT(void) const
{
  const char * rval = 0;
  switch (_vt) {
    case HLInacTimerExp:  rval = "InactivityTimerExpiry"; break;
    case HelloMismatch:   rval = "HelloMismatch"; break;
    case BadNeighbor:     rval = "BadNeighbor"; break;
    case AddInducedLink:  rval = "AddInducedLink"; break;
    case DropInducedLink: rval = "DropInducedLink"; break;
    case DropLastInducedLink: rval = "DropLastInducedLink"; break;
    case LgnHello:        rval = "LgnHello"; break;
    case HLinkUp:         rval = "HLinkUp"; break;
    case HLinkDown:       rval = "HLinkDown"; break;
    case DropALLLinks:    rval = "DropAllLinks"; break;
    case ReqLogicalPort:  rval = "ReqLogicalPort"; break;
    default:              rval = "Unknown"; break;
  }
  return rval;
}

int  HorLinkVisitor::GetBorderPort(void)
{  return _borderPort;  }

void HorLinkVisitor::SetBorderPort(int pid)
{  _borderPort = pid;  }

// It's still mine, don't delete it;
const NodeID *  HorLinkVisitor::GetBorderNode(void) const
{  return _borderNode;  }

void HorLinkVisitor::SetBorderNode(const NodeID * nid)
{  
  if (_borderNode)
    delete _borderNode;
  _borderNode = (nid ? nid->copy() : (NodeID *)0);
}

void HorLinkVisitor::SetLocalPort(int val)
{  _localPort = val;  }

int HorLinkVisitor::GetLocalPort(void)
{  return _localPort;  }

void HorLinkVisitor::SetRemotePort(int val)
{  _remotePort = val;  }

int HorLinkVisitor::GetRemotePort(void)
{  return _remotePort;  }

void HorLinkVisitor::at(Mux * m, Accessor * a)
{
  Conduit * dest;
    
  assert(m && a);
  
  switch(EnteredFrom()) {
    case A_SIDE:
      SetLast( CurrentConduit() );
      if (a) {
	if (ToBroadCast())
	  m->Broadcast(this);
	else if (dest = a->GetNextConduit(this))
	  dest->Accept(this);
	else {
	  if (dest = SideB())
	    dest->Accept(this);
	  else Suicide();
	}
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

void  HorLinkVisitor::SetToBroadCast(bool Value)
{  _broadCast = Value;  }

bool HorLinkVisitor::ToBroadCast(void) const
{  return _broadCast;  }

void HorLinkVisitor::Fill_Subtype_Name(char * buf) const
{
  switch (_vt) {
  case HLInacTimerExp:      strcpy(buf,"HLV_InactivityTimerExpiry"); break;
  case HelloMismatch:       strcpy(buf,"HLV_HelloMismatch"); break;
  case BadNeighbor:         strcpy(buf,"HLV_BadNeighbor"); break;
  case AddInducedLink:      strcpy(buf,"HLV_AddInducedLink"); break;
  case DropInducedLink:     strcpy(buf,"HLV_DropInducedLink"); break;
  case DropLastInducedLink: strcpy(buf,"HLV_DropLastInducedLink"); break;
  case LgnHello:            strcpy(buf,"HLV_LgnHello"); break;
  case HLinkUp:             strcpy(buf,"HLV_HLinkUp"); break;
  case HLinkDown:           strcpy(buf,"HLV_HLinkDown"); break;
  case DropALLLinks:        strcpy(buf,"HLV_DropAllLinks"); break;
  case ReqLogicalPort:      strcpy(buf,"HLV_ReqLogicalPort"); break;
  default:                  strcpy(buf,"HLV_Unknown"); break;
  }
}

//------------SVCCPortInfoVisitor-----------------------------------------------
SVCCPortInfoVisitor::SVCCPortInfoVisitor(int portState, int vpi, int vci) :
                     VPVCVisitor(_my_type, -1, -1, vci),
                     _portUp(portState)
{
  SetInVP(vpi);
  SetInVC(vci);
}

SVCCPortInfoVisitor::SVCCPortInfoVisitor(vistype & child_type, int portState,
                                         int vpi, int vci) :
                     VPVCVisitor(child_type.derived_from(_my_type), -1, -1, vci),
                     _portUp(portState)
{
  SetInVP(vpi);
  SetInVC(vci);
}

SVCCPortInfoVisitor::SVCCPortInfoVisitor(const SVCCPortInfoVisitor & rhs) :
  VPVCVisitor(rhs), _portUp(rhs._portUp) { }

SVCCPortInfoVisitor::~SVCCPortInfoVisitor() { }

const VisitorType SVCCPortInfoVisitor::GetType(void) const
{
  return VisitorType(GetClassType());
}

bool SVCCPortInfoVisitor::PortUp(void) const
{
  return _portUp;
}

const vistype& SVCCPortInfoVisitor::GetClassType(void) const
{
  return _my_type;
}

Visitor * SVCCPortInfoVisitor::dup(void) const { return new SVCCPortInfoVisitor(*this); }

void HVPrintPGID(char *label, PeerGroupID *pgid)
{
  const u_char *temp = pgid->GetPGID();
  char buf[80];
  buf[0] ='\0';
  for (int i = 1; i < 14; i++)
    sprintf(buf, "%s%2x ", buf, temp[i]);

  diag(FSM, DIAG_DEBUG, "%s (%x): %d:%s\n", label, pgid, temp[0], buf);
}

void HVPrintNID(char *label, NodeID *nid)
{
  const u_char *temp = nid->GetNID();
  char buf[80];
  buf[0] ='\0';
  for (int i = 2; i < 22; i++)
    sprintf(buf, "%s%2x ", buf, temp[i]);

  diag(FSM, DIAG_DEBUG, "%s (%x): %d:%d:%s\n", label, nid, temp[0], temp[1], buf);
}
