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

/** -*- C++ -*-
 * File: VPVCVisitor.cc
 * @author talmage
 * @version $Id: VPVCVisitor.cc,v 1.49 1998/12/01 13:44:07 mountcas Exp $
 * Base class for a hierarchy of classes that travel through Ports 
 * in a Switch and possibly over Links to other Ports in other Switches.
 * BUGS:
 */
#ifndef LINT
static char const rcsid[] =
"$Id: VPVCVisitor.cc,v 1.49 1998/12/01 13:44:07 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "VPVCVisitor.h"

// --------------------- VPVCVisitor ---------------------------
#define MAKE_VPVC(vpi, vci) (((vpi&0xff) << 16) | ((vci) & 0xffff))
//
// MAKE_VPI() and MAKE_VCI() assume 32-bit inputs.
//
#define MAKE_VPI(vpvc) (((vpvc) & 0x00ff0000) >> 16)
#define MAKE_VCI(vpvc) ((vpvc) & 0x0000ffff)

// u_char VPVCVisitor::_VPVCInternalUseOnly[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 					       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  };

VPVCVisitor::VPVCVisitor(int port, int agg, int vpvc, 
			 const NodeID * src, const NodeID * dest)
  : NodeIDVisitor(_mytype, port, agg, src, dest),
    _saved_vp(-1), _saved_vc(-1), _in_vp(-1), _in_vc(-1), 
    _data(0), _cref(0xffffffff)
{
  _out_vp = MAKE_VPI(vpvc);
  _out_vc = MAKE_VCI(vpvc);
}

VPVCVisitor::VPVCVisitor(const VPVCVisitor & rhs) 
  : NodeIDVisitor(rhs), _in_vp(rhs._in_vp), _in_vc(rhs._in_vc),
    _out_vp(rhs._out_vp), _out_vc(rhs._out_vc), _saved_vp(rhs._saved_vp),
    _saved_vc(rhs._saved_vc), _cref(rhs._cref) 
{ }

VPVCVisitor::VPVCVisitor(vistype & child_type, int port, int agg, int vpvc, 
			 const NodeID * src_nid, const NodeID * dest_nid) 
  : NodeIDVisitor(child_type.derived_from(_mytype), port, agg, src_nid, dest_nid), 
    _saved_vp(-1), _saved_vc(-1), _in_vp(-1), _in_vc(-1), _data(0), _cref(0xffffffff)
{
  _out_vp = MAKE_VPI(vpvc);
  _out_vc = MAKE_VCI(vpvc);
}

VPVCVisitor::~VPVCVisitor()  {  }

const vistype & VPVCVisitor::GetClassType(void) const
{
  return _mytype;
}

// Outgoing

int VPVCVisitor::GetOutVP(void) const { return _out_vp; }

int VPVCVisitor::GetOutVC(void) const { return _out_vc; }

void VPVCVisitor::SetOutVP(int vp) { _out_vp = vp; }

void VPVCVisitor::SetOutVC(int vc) { _out_vc = vc; }

// Incoming

int VPVCVisitor::GetInVP(void) const { return _in_vp; }

int VPVCVisitor::GetInVC(void) const { return _in_vc; }

void VPVCVisitor::SetInVP(int vp) { _in_vp = vp; }

void VPVCVisitor::SetInVC(int vc) { _in_vc = vc; }

// Saved

int  VPVCVisitor::GetSavedVP(void) const { return _saved_vp; }

void VPVCVisitor::SetSavedVP(int vp) { _saved_vp = vp; }

int  VPVCVisitor::GetSavedVC(void) const { return _saved_vc; }

void VPVCVisitor::SetSavedVC(int vc) { _saved_vc = vc; }

void VPVCVisitor::ClearRoutingInfo(void)
{
  _in_vp  = 0;  _in_vc  = 0;
  _out_vp = 0;  _out_vc = 0;

  NodeID * tmp = 0;
  SetSourceNID(tmp);
  SetDestNID(tmp);
}

inline void VPVCVisitor::SetByteStream(u_char * data)
{
  if (_data) delete _data;
  _data = data;
}

inline u_char * VPVCVisitor::GetByteStream(void) const
{
  return _data;
}

const VisitorType VPVCVisitor::GetType(void) const 
{   return VisitorType(GetClassType());  }

VPVCVisitor & VPVCVisitor::operator = (const VPVCVisitor & rhs)
{
  _in_vp = rhs._in_vp;
  _in_vc = rhs._in_vc;

  _out_vp = rhs._out_vp;
  _out_vc = rhs._out_vc;

  _saved_vp = rhs._saved_vp;
  _saved_vc = rhs._saved_vc;

  return (VPVCVisitor &)(*(NodeIDVisitor *)this = (NodeIDVisitor &)rhs);
}

void VPVCVisitor::SwapSrcDest(void)
{
  NodeID * src  = TakeSourceNID();
  NodeID * dest = TakeDestNID();
  
  SetSourceNID(dest);
  SetDestNID(src);
}

ostream & operator << (ostream & os, const VPVCVisitor & v)
{
  os << v.GetType() << endl
     << "In    VP= " << v._in_vp    << " In    VC= " << v._in_vc << endl
     << "Out   VP= " << v._out_vp   << " Out   VC= " << v._out_vc << endl
     << "Saved VP= " << v._saved_vp << " Saved VC= " << v._saved_vc << endl;
  return os;
}

Visitor * VPVCVisitor::dup(void) const { return new VPVCVisitor(*this); }

void   VPVCVisitor::SetCREF(u_long cref)
{  _cref = cref;  }

u_long VPVCVisitor::GetCREF(void) const
{  return _cref;  }
