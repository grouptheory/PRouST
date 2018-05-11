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
 * File: PortDataVisitor.cc
 * @author talmage
 * @version $Id: PortDataVisitor.cc,v 1.11 1998/07/30 21:10:32 mountcas Exp $
 * @memo Carries data into a Port.
 * BUGS:
 */

#if !defined(LINT)
static char const rcsid[] =
"$Id: PortDataVisitor.cc,v 1.11 1998/07/30 21:10:32 mountcas Exp $";
#endif

#include <FW/actors/Creator.h>
#include <FW/behaviors/Factory.h>
#include "PortDataVisitor.h"

// --------------------- PortDataVisitor ---------------------------

PortDataVisitor::PortDataVisitor(int port, int vpvc, void *data) : 
  VPVCVisitor(_mytype, port, -1, vpvc, 0, 0), _data(data) { }


PortDataVisitor::PortDataVisitor(vistype &child_type, int port, 
				 int vpvc, void *data) :
  VPVCVisitor(child_type.derived_from(_mytype), port, -1, vpvc, 0, 0), _data(data)
{ }


PortDataVisitor::~PortDataVisitor() {   delete _data;  }

const vistype &PortDataVisitor::GetClassType(void) const
{  return _mytype; }

//
// A PortDataVisitor visits a Factory when there is no Protocol for
// its VPVC in the Port.  A PortDataVisitor has no business in a
// Factory.  Suicide is the only honorable action at that point.
//
Conduit * PortDataVisitor::at(Factory * f, Creator * c)
{  
  Suicide();  
  return 0;
}

const void * PortDataVisitor::GetData(void) const
{  return _data; }

void PortDataVisitor::SetData(void *data)
{  _data = data; }

const VisitorType PortDataVisitor::GetType(void) const 
{   return VisitorType(GetClassType());  }

