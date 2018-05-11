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
 * File: ULIAVisitor.cc
 * @author talmage
 * @version $Id: ULIAVisitor.cc,v 1.3 1998/08/10 20:59:44 mountcas Exp $
 *
 * Carries Link messages around in a switch.
 *
 * BUGS:
 */

#ifndef LINT
static char const rcsid[] =
"$Id: ULIAVisitor.cc,v 1.3 1998/08/10 20:59:44 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <fsm/visitors/LinkVisitor.h>
#include <FW/basics/diag.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/uplink_info_attr.h>

ULIAVisitor::ULIAVisitor(int local_port, int remote_port, 
			 ig_resrc_avail_info *theRAIG)
  LinkVisitor(_mytype, local_port, -1, 0, 0)
{ 
  SetLocalPort(local_port);
  SetRemotePort(remote_port);
  AddRAIG(theRAIG);
}

ULIAVisitor::ULIAVisitor(const ULIAVisitor & rhs) : LinkVisitor(rhs)
{ 
}

ULIAVisitor::ULIAVisitor(vistype &child_type, int local_port, int remote_port, 
			 ig_resrc_avail_info *theRAIG)
  LinkVisitor(child_type.derived_from(_mytype), local_port, -1, 0, 0)
{ 
  SetLocalPort(local_port);
  SetRemotePort(remote_port);
  AddRAIG(theRAIG);
}

ULIAVisitor::~ULIAVisitor() 
{
}

Visitor * ULIAVisitor::dup(void) const { return new ULIAVisitor(*this); }

void ULIAVisitor::on_death(void) const
{
  DIAG(FSM, DIAG_INFO, 
       if (DumpLog(cout)) cout << endl;);
}

const vistype &ULIAVisitor::GetClassType(void) const
{  return _mytype; }

const VisitorType ULIAVisitor::GetType(void) const 
{  return VisitorType(GetClassType());  }


ig_uplink_info_attr * ULIAVisitor::TakeULIA(void)
{
  ig_uplink_info_attr *answer = new ig_uplink_info_attr(0);

  
}
