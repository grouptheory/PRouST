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
#ifndef __NPEERVISITORS_H__
#define __NPEERVISITORS_H__

#ifndef LINT
static char const _NPVisitors_h_rcsid_[] =
"$Id: NPVisitors.h,v 1.29 1998/12/22 19:11:49 battou Exp $";
#endif

#include <fsm/visitors/PNNIVisitor.h>
#include <fsm/visitors/VPVCVisitor.h>

#define MAXDB   5
#define MAXREQ  10
#define MAXACK  15

#define DS_VISITOR_NAME       "DSVisitor"
#define REQ_VISITOR_NAME      "ReqVisitor"
#define PTSP_VISITOR_NAME     "PTSPVisitor"
#define ACK_VISITOR_NAME      "AckVisitor"
#define NPSTATE_VISITOR_NAME  "NPStateVisitor"

class NodeID;
class DatabaseSumPkt;
class PTSEReqPkt;
class PTSPPkt;
class PTSEAckPkt;

//  A brief summary of all the visitors for
//  the nodepeerfsm which are defined here.

// DSVisitor:  Database Summary Visitor
//   Contains: Database Summary Pkt and carries
//             it to peer node
// ReqVisitor: Request Visitor
//   Contains: A PTSE Request packet and carries
//             it to the peer
// PTSPVisitor: PTSP Visitor
//   Contains: PTSP packet and carries to the peer node
// AckVisitor: PTSP Ack packet Visitor
//   Contains: An Acknowledgement packet which is sent
//             to the peer node as an ack to ptsp it sent
// Note: Packets in the Visitors are destroyed at the
//      receiving node


//----------DS Visitor------------------
class DSVisitor : public PNNIVisitor {
public:

  DSVisitor(NodeID *nid = 0, NodeID *rnid = 0, DatabaseSumPkt * pkt = 0, u_int port = 0);
  virtual ~DSVisitor();

  virtual const VisitorType GetType(void) const;
  const DatabaseSumPkt * GetDSPktptr(void) const;
  void  SetDSPkt(DatabaseSumPkt * dp);

  bool encode(      unsigned char *& buffer, int & len) const;
  bool decode(const unsigned char *& buffer, int & len);

protected:

  DSVisitor(const DSVisitor & rhs);
  DSVisitor(vistype &child_type,NodeID *nid, NodeID *rnid,DatabaseSumPkt *pkt, u_int port);
  virtual const vistype& GetClassType(void) const;
  virtual Visitor * dup(void) const;

  DatabaseSumPkt * _ds;

private:

  static vistype  _my_type;
};


//-----------Req Visitor------------------
class ReqVisitor: public PNNIVisitor {
public:

  ReqVisitor(NodeID *nid = 0, NodeID *rnid = 0, PTSEReqPkt *pkt = 0, u_int port = 0);
  virtual ~ReqVisitor();

  virtual const VisitorType GetType(void) const;
  const PTSEReqPkt * GetReqPktptr(void);
  void SetReqPkt(PTSEReqPkt * rp);

  bool encode(      unsigned char *& buffer, int & len) const;
  bool decode(const unsigned char *& buffer, int & len);

protected:

  ReqVisitor(const ReqVisitor & rhs);
  ReqVisitor(vistype &child_type,NodeID *nid, NodeID *rnid,PTSEReqPkt *pkt, u_int port);
  PTSEReqPkt * _rp;
  virtual const vistype& GetClassType(void) const;
  virtual Visitor * dup(void) const;

private:

  static vistype  _my_type;
};

//----------PTSP Visitor------------------

class PTSPVisitor: public PNNIVisitor {
public:

  PTSPVisitor(NodeID *nid = 0, NodeID *rnid = 0, PTSPPkt *pkt = 0, u_int port = 0);
  virtual ~PTSPVisitor();

  virtual const VisitorType GetType(void) const;
  PTSPPkt *GetPTSPPktptr(void);
  void SetPTSPPkt(PTSPPkt * pp);

  bool encode(      unsigned char *& buffer, int & len) const;
  bool decode(const unsigned char *& buffer, int & len);

protected:

  PTSPVisitor(const PTSPVisitor & rhs);
  PTSPVisitor(vistype &child_type,NodeID *nid, NodeID *rnid,PTSPPkt *pkt, u_int port);
  PTSPPkt * _pp;
  virtual const vistype& GetClassType(void) const;
  virtual Visitor * dup(void) const;

private:

  static vistype  _my_type;
};


//-------------AcK Visitor---------------------
class AckVisitor: public PNNIVisitor {
public:

  AckVisitor(NodeID *nid = 0, NodeID *rnid = 0, PTSEAckPkt *pkt = 0, u_int port = 0);
  virtual ~AckVisitor();

  virtual const VisitorType GetType(void) const;
  PTSEAckPkt *GetAckPktptr(void);
  void SetAckPkt(PTSEAckPkt * ap);

  bool encode(      unsigned char *& buffer, int & len) const;
  bool decode(const unsigned char *& buffer, int & len);

protected:

  AckVisitor(const AckVisitor & rhs);
  AckVisitor(vistype &child_type,NodeID *nid, NodeID *rnid,PTSEAckPkt *pkt, u_int port);
  PTSEAckPkt *_ap;
  virtual const vistype& GetClassType(void) const;
  virtual Visitor * dup(void) const;

private:

  static vistype  _my_type;
};


//-------------NPStateVisitor---------------------
class NPStateVisitor : public VPVCVisitor {
public:

  enum NPStateVisitorType {
    FullState    = 0,
    BadPTSEReq   = 1,
    DSMismatch   = 2,
    NonFullState = 3
  };

  NPStateVisitor(NPStateVisitorType vt, NodeID * nid, NodeID * rnid, u_int port);
  virtual ~NPStateVisitor();

  NPStateVisitorType GetVT(void);
  void SetVT(NPStateVisitorType vt);
  virtual const VisitorType GetType(void) const;

protected:

  virtual void at(Mux * m, Accessor * a);

  NPStateVisitor(const NPStateVisitor & rhs);
  NPStateVisitor(vistype &child_type,NPStateVisitorType vt, NodeID *nid,
		 NodeID *rnid, u_int port);
  NPStateVisitorType _vt;
  virtual const vistype& GetClassType(void) const;
  virtual Visitor * dup(void) const;

private:

  static vistype  _my_type;
};

#endif // __NPEERVISITORS_H__
