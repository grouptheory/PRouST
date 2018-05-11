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

#ifndef LINT
static char const _PortMux_cc_rcsid_[] =
"$Id: VPIVCImux.cc,v 1.5 1998/12/23 15:07:56 battou Exp $";
#endif
#include <FW/basics/diag.h>
#include <fsm/visitors/VPVCVisitor.h>
#include "VPIVCImux.h"
#include "XTIadapterInstallerVisitor.h"

VPVCaccessor::VPVCaccessor()
{
}

VPVCaccessor::~VPVCaccessor()
{
}

bool VPVCaccessor::Broadcast(Visitor *v)
{
  return false;
}

bool VPVCaccessor::Add(Conduit * c, Visitor * v)
{
  VisitorType vt1 = v->GetType();
  const VisitorType *vt2;
  vt2 = QueryRegistry(XTI_VISITOR_NAME);
  if (vt2 && vt1.Is_A(vt2))
    {
      XTIadapterInstallerVisitor *xv = (XTIadapterInstallerVisitor *)v;
      u_int key = xv->GetVPVC();
      _conduits.insert(key, c);
      v->Suicide();
      return true;
    }
  return false;
}

bool VPVCaccessor::Del(Conduit * c)
{
  return false;
}

bool VPVCaccessor::Del(Visitor * v)
{
  return false;
}

Conduit * VPVCaccessor::GetNextConduit(Visitor * v)
{ 
  const VisitorType *vt2;
  VisitorType vt1 = v->GetType();
  vt2 = QueryRegistry(VPVC_VISITOR_NAME);
  if (vt2 && vt1.Is_A(vt2))
    {
      VPVCVisitor *vpvc_vis = (VPVCVisitor *)v;
      u_int vpi = vpvc_vis->GetOutVP();
      u_int vci = vpvc_vis->GetOutVC();
      u_int key = ((vpi&0xff) << 16) | ((vci) & 0xffff);
      dic_item di;
      if((di = _conduits.lookup(key)))
	return _conduits.inf(di);
    }
  return 0;
}


void VPVCaccessor::AddBinding(u_int vpvc, Conduit *c)
{
  _conduits.insert(vpvc, c);
}








