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

//-*-C++-*-
#ifndef __HELLOVISITOR_H__
#define __HELLOVISITOR_H__

#ifndef LINT
static char const _HelloVisitor_h_rcsid_[] =
"$Id: HelloVisitor.h,v 1.71 1998/12/23 19:59:13 mountcas Exp bilal $";
#endif

#include <FW/basics/Visitor.h>

#include <fsm/visitors/PNNIVisitor.h>
#include <fsm/visitors/VPVCVisitor.h>

/*
 * Internal visitors can be derived from Visitor, while external
 * visitors, which leave the switch, must be derived from VPVCVisitor.
 */
#define HELLO_VISITOR_NAME "HelloVisitor"
#define HLINK_VISITOR_NAME "HorLinkVisitor"
#define SVCC_PORT_INFO_VISITOR "SVCCPortInfoVisitor"

// not needed anymore
#define HELLO_MISMATCH_VISITOR_NAME    "HelloMismatchVisitor"
#define ADD_ILINK_VISITOR_NAME         "AddILinkVisitor"
#define DROP_ILINK_VISITOR_NAME        "DropILinkVisitor"
#define DROP_LILINK_VISITOR_NAME       "DropLILinkVisitor"

class HelloPkt;
class ig_nodal_hierarchy_list;
class ig_uplink_info_attr;
class ig_lgn_horizontal_link_ext;
class Mux;
class Accessor;
class PeerID;

/*
 * This visitor must be consumed by any Loer level HelloFSM 
 * whose port matches. For HelloFSM's running on PVCC the (vpi,vci) 
 * must also match.
 */
class HelloVisitor : public PNNIVisitor {
public:

  enum HelloType {
    Unknown  = -1,
    PhyHello = 0,
    LgnHello = 1,
    SVCHello = 2
  };

  HelloVisitor(HelloType htype = PhyHello, HelloPkt * hp = 0, int port = 0,
	       u_int vpvc = 18, u_char cver = 1);
  HelloVisitor(const HelloVisitor & rhs);

  virtual ~HelloVisitor();


  virtual const VisitorType GetType(void) const;

  void      PacketToVisitor(HelloPkt *hp);
  void      VisitorToPacket(void) const;
  void      HelloDump(void);
  HelloType GetHelloType(void);

  // retrieve hello contents
  int      GetAGGR(void);
  const    NodeID * GetNodeID(void);
  int      GetPortID(void);
  const    NodeID * GetRemNodeID(void);
  int      GetRemPortID(void);
  const    PeerID * GetRemPeerGroupID(void);
  const    PeerID * GetPeerGroupID(void);
  int      GetHelloInt(void);
  int      GetCVER(void);
  int      GetNVER(void);
  int      GetOVER(void);
  void     ClearDS();

  ig_nodal_hierarchy_list    * GetNHL(void);
  ig_uplink_info_attr        * GetULIA(void);
  ig_lgn_horizontal_link_ext * GetHLE(void);

  void SetNHL(ig_nodal_hierarchy_list *);
  void SetULIA(ig_uplink_info_attr *);
  void SetHLE(ig_lgn_horizontal_link_ext *);

  const PeerID               * Get_CPGID(void);
  const NodeID               * Get_UpNodeID(void);
  const Addr                 * Get_UpNodeAddr(void);
  void                         SetHelloPkt(HelloPkt *);
  const HelloPkt             * GetHelloPkt(void) const ;

  void SetUpNodeID(const NodeID * n);
  void SetUpNodeAddr(const Addr * a);

  void SetNID(const NodeID * n);
  void SetPGID(const PeerID * p);
  void SetRNID(const NodeID * n);
  void SetRPID(const int rp);
  void SetPID(const int lp);
  void SetHI(const int hi);
  void SetCPGID(const PeerID *);
  void SetAGGR(const int a);

  bool encode(      unsigned char *& buffer, int & len) const;
  bool decode(const unsigned char *& buffer, int & len);
  
protected:

  virtual Visitor * dup(void) const;

  HelloVisitor(vistype & child_type, HelloType htype, 
	       HelloPkt * hp, int port, u_int vpvc, u_char cver = 1);
  virtual const vistype & GetClassType(void) const;

  HelloType  _htype;
  HelloPkt * _hp;
  // from the hello packet
  u_short    _len;
  u_char     _cver;
  u_char     _nver;
  u_char     _over;

  int        _AGGR;
  int        _HI;

  NodeID   * _NID;
  int        _PID;
  NodeID   * _RNID;
  int        _RPID;
  PeerID   * _PGID;
  PeerID   * _RPGID;
  PeerID   * _CPGID;
  NodeID   * _UpNodeID;
  Addr     * _UpNodeAddr;

  ig_nodal_hierarchy_list    * _NHL;
  ig_uplink_info_attr        * _ULIA;
  ig_lgn_horizontal_link_ext * _HLE;

private:

  static vistype  _my_type;
};


// ------------------ HorLinkVisitor -------------------
class HorLinkVisitor : public VPVCVisitor {
public:

  enum HLVisitorTypes {
    UnknownType = 0,
    HLInacTimerExp,
    HelloMismatch,
    BadNeighbor,
    AddInducedLink,
    DropInducedLink,
    DropLastInducedLink,
    LgnHello,
    HLinkUp,
    HLinkDown,
    DropALLLinks,
    ReqLogicalPort
  };

  // Used to declare the First AddInducedUpLink to a new
  // remote node which will create a new LGNHelloFSM
  HorLinkVisitor(HLVisitorTypes hlvtype, const NodeID * SourceNode,
                 const NodeID * DestNode , const NodeID * borderNode = 0,
                 int borderPort = -1, int aggrToken = -1, int localPort = -1,
                 int remotePort = -1, bool broadcast = false);
  // copy ctor
  HorLinkVisitor(const HorLinkVisitor & rhs);

  virtual ~HorLinkVisitor();

  virtual const VisitorType GetType(void) const;

  HLVisitorTypes GetVT(void);
  void SetVT(HLVisitorTypes vt);
  const char * PrintVT(void) const;

  int  GetBorderPort(void);
  void SetBorderPort(int pid);

  const NodeID * GetBorderNode(void) const;
  void  SetBorderNode(const NodeID * nid);

  void SetToBroadCast(bool Value);
  bool ToBroadCast(void) const;

  void SetLocalPort(int val);
  int  GetLocalPort(void);

  void SetRemotePort(int val);
  int  GetRemotePort(void);

protected:

  virtual Visitor * dup(void) const;

  HLVisitorTypes     _vt;
  NodeID           * _borderNode;
  int                _borderPort;
  bool               _broadCast;
  int                _localPort;
  int                _remotePort;

  HorLinkVisitor(vistype & child_type, HLVisitorTypes hlvtype,
                 int aggr, int borderPort, int aggrToken ,
                 const NodeID * borderNode, const NodeID * src,
                 const NodeID * dst, int localPort, int remotePort,
                 bool broadcast = 0);

  virtual void at(Mux * m, Accessor * a);

  virtual const vistype & GetClassType(void) const;

private:

  void Fill_Subtype_Name(char * buf) const;

  static vistype  _my_type;
};

// Between SVCCH and LGN NP to give svcc port
// info and the vpi vci to be used by the Lgn NP.
// to get in vp & vc call GetInVP GetInVC (of VPVC Visitor)
class SVCCPortInfoVisitor : public VPVCVisitor {
public:

  SVCCPortInfoVisitor(int portState, int vpi, int vci);
  SVCCPortInfoVisitor(const SVCCPortInfoVisitor & rhs);

  virtual ~SVCCPortInfoVisitor();
  virtual const VisitorType GetType(void) const;
  bool PortUp(void) const;

protected:

  virtual Visitor * dup(void) const;

  SVCCPortInfoVisitor(vistype &child_type, int portState, int vpi, int vci);
  virtual const vistype &GetClassType(void) const;
  bool _portUp;

private:

  static vistype _my_type;
};

#endif // __HELLOVISITOR_H__
