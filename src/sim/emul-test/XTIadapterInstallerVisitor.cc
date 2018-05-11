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
static char const _XTIadapterInstallerVisitor_cc_rcsid_[] =
"$Id: XTIadapterInstallerVisitor.cc,v 1.2 1998/12/23 14:12:30 battou Exp $";
#endif
#include "XTIadapterInstallerVisitor.h"
#include <common/cprototypes.h>
#include <FW/basics/diag.h>

XTIadapterInstallerVisitor::XTIadapterInstallerVisitor(u_int vpvc, Conduit *c) : Visitor(_my_type), 
  _vpvc(vpvc), _c(c) { }


XTIadapterInstallerVisitor::~XTIadapterInstallerVisitor(void)
{
}


XTIadapterInstallerVisitor::XTIadapterInstallerVisitor(vistype &child_type,
						       u_int vpvc,
						       Conduit *c) : 
  Visitor(child_type.derived_from(_my_type)), 
  _vpvc(vpvc), _c(c) { }




XTIadapterInstallerVisitor::XTIadapterInstallerVisitor(const XTIadapterInstallerVisitor &rhs):
  Visitor(rhs), 
  _vpvc(rhs._vpvc), _c(rhs._c) { }


Visitor * XTIadapterInstallerVisitor::dup(void) const
{  return new XTIadapterInstallerVisitor(*this);  }


void XTIadapterInstallerVisitor::on_death(void) const
{
  DIAG("xti.visitors.dumplog", DIAG_INFO, 
       if (DumpLog(cout)) cout << endl;);
}


const vistype &XTIadapterInstallerVisitor::GetClassType(void) const
{
  return _my_type;
}

u_int XTIadapterInstallerVisitor::GetVPVC(void)
{
  return _vpvc;
}


Conduit *XTIadapterInstallerVisitor::GetConduit(void)
{
  return _c;
}


const VisitorType XTIadapterInstallerVisitor::GetType(void) const 
{ return VisitorType(GetClassType()); }


