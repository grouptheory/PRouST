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
 * File: VPVCRebindingVisitor.cc
 * @author talmage
 * @version $Id: VPVCRebindingVisitor.cc,v 1.4 1998/10/19 19:26:07 mountcas Exp $
 *
 * Carries two vpvcs, old and new.  The Muxes that recognize this
 * Visitor change their vpvc->Conduit bindings from old_vpvc->Conduit
 * to new_vpvc->Conduit.  It is not supposed to leave a switch.
 *
 * BUGS:
 */

#ifndef LINT
static char const rcsid[] =
"$Id: VPVCRebindingVisitor.cc,v 1.4 1998/10/19 19:26:07 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "VPVCRebindingVisitor.h"
#include <codec/pnni_ig/id.h>

// --------------------- VPVCRebindingVisitor ---------------------------
#define MAKE_VPVC(vpi, vci) (((vpi&0xff) << 16) | ((vci) & 0xffff))
//
// MAKE_VPI() and MAKE_VCI() assume 32-bit inputs.
//
#define MAKE_VPI(vpvc) (((vpvc) & 0x00ff0000) >> 16)
#define MAKE_VCI(vpvc) ((vpvc) & 0x0000ffff)

VPVCRebindingVisitor::VPVCRebindingVisitor(int port, int agg, int vpvc, 
					   int old_vpvc, int new_vpvc,
					   const NodeID * src) :
  VPVCVisitor(_mytype, port, agg, vpvc, src)
{
  _old_vp = MAKE_VPI(old_vpvc);
  _old_vc = MAKE_VCI(old_vpvc);

  _new_vp = MAKE_VPI(new_vpvc);
  _new_vc = MAKE_VCI(new_vpvc);
}


VPVCRebindingVisitor::VPVCRebindingVisitor(vistype &child_type, 
					   int port, int agg, 
					   int vpvc, 
					   int old_vpvc,
					   int new_vpvc, 
					   const NodeID * src_nid):
  VPVCVisitor(child_type.derived_from(_mytype), port, agg, vpvc, src_nid)
{
  _old_vp = MAKE_VPI(old_vpvc);
  _old_vc = MAKE_VCI(old_vpvc);

  _new_vp = MAKE_VPI(new_vpvc);
  _new_vc = MAKE_VCI(new_vpvc);
}


VPVCRebindingVisitor::~VPVCRebindingVisitor() 
{ 
}


const vistype &VPVCRebindingVisitor::GetClassType(void) const
{
  return _mytype;
}

// Old VPVC

int VPVCRebindingVisitor::GetOldVP(void) const { return _old_vp; }

int VPVCRebindingVisitor::GetOldVC(void) const { return _old_vc; }

void VPVCRebindingVisitor::SetOldVP(int vp) { _old_vp = vp; }

void VPVCRebindingVisitor::SetOldVC(int vc) { _old_vc = vc; }

// New VPVC

int VPVCRebindingVisitor::GetNewVP(void) const { return _new_vp; }

int VPVCRebindingVisitor::GetNewVC(void) const { return _new_vc; }

void VPVCRebindingVisitor::SetNewVP(int vp) { _new_vp = vp; }

void VPVCRebindingVisitor::SetNewVC(int vc) { _new_vc = vc; }

const VisitorType VPVCRebindingVisitor::GetType(void) const 
{   return VisitorType(GetClassType());  }

VPVCRebindingVisitor & VPVCRebindingVisitor::operator = 
(const VPVCRebindingVisitor & rhs)
{
  _old_vp = rhs._old_vp;
  _old_vc = rhs._old_vc;

  _new_vp = rhs._new_vp;
  _new_vc = rhs._new_vc;

  return (VPVCRebindingVisitor &)(*(VPVCVisitor *)this = 
				  (VPVCVisitor &)rhs);
}


VPVCRebindingVisitor::VPVCRebindingVisitor(const VPVCRebindingVisitor & x) : 
  VPVCVisitor(x), _old_vp(x._old_vp), _old_vc(x._old_vc),
  _new_vp(x._new_vp), _new_vc(x._new_vc)
{
}


ostream & operator << (ostream & os, const VPVCRebindingVisitor & v)
{
  os << v.GetType() << endl
     << "In    VP= " << v._in_vp    << " In    VC= " << v._in_vc << endl
     << "Out   VP= " << v._out_vp   << " Out   VC= " << v._out_vc << endl
     << "Saved VP= " << v._saved_vp << " Saved VC= " << v._saved_vc << endl
     << "Old   VP= " << v._old_vp    << " Old   VC= " << v._old_vc << endl
     << "New   VP= " << v._new_vp    << " New   VC= " << v._new_vc << endl;
  if (v.GetSourceNID())
    os << "Source:      " << *(v.GetSourceNID()) << endl;
  if (v.GetDestNID())
    os << "Destination: " << *(v.GetDestNID()) << endl;
  return os;
}

Visitor * VPVCRebindingVisitor::dup(void) const
{ return new VPVCRebindingVisitor(*this); }

