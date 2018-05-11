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
static char const _NPVisitors_cc_rcsid_[] =
"$Id: NPVisitors.cc,v 1.39 1999/01/07 20:46:40 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <fsm/nodepeer/NPVisitors.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/Visitor.h>
#include <FW/basics/diag.h>
#include <FW/behaviors/Mux.h>
#include <codec/pnni_pkt/database_sum.h>
#include <codec/pnni_pkt/ptse_req.h>
#include <codec/pnni_pkt/ptse_ack.h>
#include <codec/pnni_pkt/ptsp.h>

// -------------------- DSVisitor --------------------
DSVisitor::DSVisitor(NodeID *nid, NodeID *rnid, DatabaseSumPkt * pkt, u_int port) : 
  PNNIVisitor(_my_type, port, -1, PNNI_VPVC, nid,rnid), _ds(pkt)
{
  DIAG(FSM, DIAG_DEBUG, if (CurrentConduit()) 
                          cout << "from dsvisitor" << CurrentConduit()->GetName() << *(_ds) << endl;);
  SetLoggingOn();
}

DSVisitor::DSVisitor(const DSVisitor & rhs) 
  : PNNIVisitor(rhs), _ds(0)
{
  if (rhs._ds)
    _ds = (DatabaseSumPkt *)rhs._ds->copy();
}

DSVisitor::DSVisitor(vistype &child_type,NodeID *nid, NodeID *rnid,DatabaseSumPkt * pkt, u_int port) : 
  PNNIVisitor(child_type.derived_from(_my_type), port, -1, PNNI_VPVC, nid,rnid), _ds(pkt)
{
  DIAG(FSM, DIAG_DEBUG, if (CurrentConduit()) cout << CurrentConduit()->GetName() << *(_ds) << endl;);
  SetLoggingOn();
}

DSVisitor::~DSVisitor() { delete _ds; }

Visitor * DSVisitor::dup(void) const { return new DSVisitor(*this); }

const VisitorType DSVisitor::GetType(void) const
{ 
  return VisitorType(GetClassType());
}

const vistype& DSVisitor::GetClassType(void) const
{
  return _my_type;
}

const DatabaseSumPkt * DSVisitor::GetDSPktptr(void) const
{
  return (_ds);
}

void DSVisitor::SetDSPkt(DatabaseSumPkt * dp)
{
  if (_ds) delete _ds;
  _ds = dp;
}

bool DSVisitor::encode(      unsigned char *& buffer, int & len) const
{
  bool rval = false;

  if ( _ds && ( _ds->encode( buffer, len ) ))
    rval = true;
  return rval;
}

bool DSVisitor::decode(const unsigned char *& buffer, int & len)
{
  bool rval = false;

  _ds = new DatabaseSumPkt( );
  if ( ! _ds->decode( buffer ) )
    rval = true;

  return rval;
}

//--------- ReQ Visitor Here ------------------------------
ReqVisitor::ReqVisitor(NodeID *nid, NodeID *rnid, PTSEReqPkt *rp, u_int port) : 
  PNNIVisitor(_my_type, port, -1, PNNI_VPVC, nid, rnid), _rp(0)
{
  if (rp != 0)
    _rp = (PTSEReqPkt *)rp->copy();
  SetLoggingOn();
}

ReqVisitor::ReqVisitor(const ReqVisitor & rhs) : 
  PNNIVisitor(rhs), _rp(0)
{
  if (rhs._rp)
    _rp = (PTSEReqPkt *)rhs._rp->copy();
}

ReqVisitor::ReqVisitor(vistype &child_type, NodeID *nid, NodeID *rnid,PTSEReqPkt *rp, u_int port) : 
  PNNIVisitor(child_type.derived_from(_my_type), port, -1, PNNI_VPVC, nid,rnid), _rp(0)
{
  if (rp != 0)
    _rp = (PTSEReqPkt *)rp->copy();
  SetLoggingOn();
}

ReqVisitor::~ReqVisitor() 
{ 
  delete _rp;
} 

Visitor * ReqVisitor::dup(void) const { return new ReqVisitor(*this); }

const VisitorType ReqVisitor::GetType(void) const
{ 
  return VisitorType(GetClassType());
}

const vistype& ReqVisitor::GetClassType(void) const
{
  return _my_type;
}

const PTSEReqPkt * ReqVisitor::GetReqPktptr(void) { return _rp; }

void  ReqVisitor::SetReqPkt(PTSEReqPkt * rp)
{
  if(_rp) delete _rp;
  _rp = rp;
}

bool ReqVisitor::encode(      unsigned char *& buffer, int & len) const
{
  bool rval = false;

  if ( _rp && ( _rp->encode( buffer, len ) ))
    rval = true;

  return rval;
}

bool ReqVisitor::decode(const unsigned char *& buffer, int & len)
{
  bool rval = false;

  _rp = new PTSEReqPkt( );
  if ( ! _rp->decode( buffer ) )
    rval = true;

  return rval;
}

//--------------PTSP Visitor Here---------------------
PTSPVisitor::PTSPVisitor(NodeID *nid, NodeID *rnid,PTSPPkt * pkt, u_int port) :
  PNNIVisitor(_my_type, port, -1, PNNI_VPVC, nid,rnid), _pp(pkt)
{
  SetLoggingOn();
}

PTSPVisitor::PTSPVisitor(const PTSPVisitor & rhs) :
  PNNIVisitor(rhs), _pp(0)
{
  if (rhs._pp)
    _pp = (PTSPPkt *)rhs._pp->copy();
}

PTSPVisitor::PTSPVisitor(vistype &child_type,NodeID *nid, NodeID *rnid,PTSPPkt * pkt, u_int port) :
  PNNIVisitor(child_type.derived_from(_my_type), port, -1, PNNI_VPVC, nid,rnid), _pp(pkt)
{
  SetLoggingOn();
}

PTSPVisitor::~PTSPVisitor() {
  // Added below by Sandeep
  delete _pp;
  // Added above by Sandeep
}

Visitor * PTSPVisitor::dup(void) const { return new PTSPVisitor(*this); }

const VisitorType PTSPVisitor::GetType(void) const
{
  return VisitorType(GetClassType());
}

const vistype& PTSPVisitor::GetClassType(void) const
{
  return _my_type;
}

PTSPPkt *PTSPVisitor::GetPTSPPktptr(void) { return _pp; }

void PTSPVisitor::SetPTSPPkt(PTSPPkt *  pp)
{
  if (_pp) delete _pp;
  _pp = pp;
}

bool PTSPVisitor::encode(      unsigned char *& buffer, int & len) const
{
  bool rval = false;

  if ( _pp && ( _pp->encode(buffer, len) ))
    rval = true;

  return rval;
}

bool PTSPVisitor::decode(const unsigned char *& buffer, int & len)
{
  bool rval = false;

  _pp = new PTSPPkt( );
  if (! _pp->decode( buffer ))
    rval = true;

  return rval;
}

//---------------AcK Visitor Here ---------------------
AckVisitor::AckVisitor(NodeID *nid, NodeID *rnid,PTSEAckPkt *ap, u_int port) :
  PNNIVisitor(_my_type, port, -1, PNNI_VPVC, nid ,rnid), _ap(ap) 
{
  SetLoggingOn();
}

AckVisitor::AckVisitor(const AckVisitor  & rhs) :
  PNNIVisitor(rhs), _ap(0)
{
  if (rhs._ap)
    _ap = (PTSEAckPkt *)rhs._ap->copy();
}

AckVisitor::AckVisitor(vistype &child_type,NodeID *nid, NodeID *rnid,PTSEAckPkt *ap, u_int port) :
  PNNIVisitor(child_type.derived_from(_my_type), port, -1, PNNI_VPVC, nid,rnid), _ap(ap) 
{
  SetLoggingOn();
}

AckVisitor::~AckVisitor() { 
  // Added below Aug 2 - Sandeep
  delete _ap;
  // Added below Aug 2 - Sandeep
}

Visitor * AckVisitor::dup(void) const { return new AckVisitor(*this); }

const VisitorType AckVisitor::GetType(void) const
{ 
  return VisitorType(GetClassType());
}

const vistype& AckVisitor::GetClassType(void) const
{
  return _my_type;
}

PTSEAckPkt *AckVisitor::GetAckPktptr(void) { return _ap; }

void AckVisitor::SetAckPkt(PTSEAckPkt * ap)
{
 if(_ap) delete _ap;
 _ap = ap;
}

bool AckVisitor::encode(      unsigned char *& buffer, int & len) const
{
  bool rval = false;

  if ( _ap && ( _ap->encode(buffer, len) ))
    rval = true;

  return rval;
}

bool AckVisitor::decode(const unsigned char *& buffer, int & len)
{
  bool rval = false;

  _ap = new PTSEAckPkt( );
  if ( ! _ap->decode( buffer ) )
    rval = true;

  return rval;
}

//---------------NPStateVisitor Here ---------------------
NPStateVisitor::NPStateVisitor(NPStateVisitorType vt,NodeID *nid, NodeID *rnid,u_int port) 
  : VPVCVisitor(_my_type, port, -1, PNNI_VPVC, nid , rnid), _vt(vt)
{
  SetLoggingOn();
}

NPStateVisitor::NPStateVisitor(const NPStateVisitor & rhs) 
  : VPVCVisitor(rhs), _vt(rhs._vt) { }

NPStateVisitor::NPStateVisitor(vistype &child_type,NPStateVisitorType vt, NodeID *nid,
			       NodeID *rnid,u_int port) 
  : VPVCVisitor(child_type.derived_from(_my_type), port, -1, 
		PNNI_VPVC, nid, rnid), _vt(vt) 
{
  SetLoggingOn();
}

NPStateVisitor::~NPStateVisitor() {  }

Visitor * NPStateVisitor::dup(void) const { return new NPStateVisitor(*this); }

const VisitorType NPStateVisitor::GetType(void) const
{ 
  return VisitorType(GetClassType());
}

const vistype & NPStateVisitor::GetClassType(void) const
{
  return _my_type;
}

// Everywhere else it acts as a Visitor.
void NPStateVisitor::at(Mux * m, Accessor * a)
{
  Conduit * dest;

  assert(m && a);

  switch(EnteredFrom()) {
  case A_SIDE:
    SetLast( CurrentConduit() );
    
    if (a) {
      // Send the visitor to the other Conduits in the Mux
      m->Broadcast(this);
    } else if (dest = SideB()) 
      dest->Accept(this);
    else Suicide();
    break;
  case B_SIDE:
  case OTHER:
    // Call at::(Behavior)
    Visitor::at(m);
    break;
  }
}

NPStateVisitor::NPStateVisitorType  
NPStateVisitor::GetVT(void) { return _vt; }

void NPStateVisitor::SetVT(NPStateVisitorType vt) 
{ _vt = vt; }
