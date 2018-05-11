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
static char const _LGNVisitors_cc_rcsid_[] =
"$Id: LGNVisitors.cc,v 1.20 1998/08/10 20:52:32 mountcas Exp $";
#endif
#include <common/cprototypes.h>

extern "C" {
#include <stdio.h>
#include <assert.h>
};

#include <fsm/visitors/LGNVisitors.h>
#include <FW/basics/Conduit.h>
#include <FW/behaviors/Factory.h>
#include <FW/actors/Creator.h>
#include <FW/basics/diag.h>

LGNInstantiationVisitor::LGNInstantiationVisitor(const NodeID * logical)
  : VPVCVisitor(_my_type, 0, -1, 0, logical, logical) { }

LGNInstantiationVisitor::~LGNInstantiationVisitor() { }

LGNInstantiationVisitor::LGNInstantiationVisitor(const LGNInstantiationVisitor & rhs) :
  VPVCVisitor(rhs) { }

Visitor * LGNInstantiationVisitor::dup(void) const
{ return new LGNInstantiationVisitor(*this); }

const VisitorType LGNInstantiationVisitor::GetType(void) const
{ return VisitorType(GetClassType()); }

const vistype & LGNInstantiationVisitor::GetClassType(void) const
{ return _my_type; }

Conduit * LGNInstantiationVisitor::at(Factory * f, Creator * c)
{
  Conduit * a = SideA(), * b = SideB();
  
  assert(f && c);

  Conduit * newc = 0;
  if (a && b) {
    newc = c->Create(this);
    if (!newc) {
      Suicide();
      return 0;
    }

    if (Join(B_half(a), A_half(newc), this, this)) {
      if (Join(B_half(b), B_half(newc), this, this)) {
	switch (EnteredFrom()) {
	case A_SIDE:
	  A_half(newc)->Accept(this);
	  break;
	case B_SIDE:
	  B_half(newc)->Accept(this);
	  break;
	case OTHER:
	  break;
	}
      } else {
	delete newc;
	Suicide();
      }
    } else {
      delete newc;
      Suicide();
    } // newc is all joined up and ready to go ...
  } else 
    Suicide();
  
  return newc;
}

// ---------------- LGNDestructionVisitor -------------------
LGNDestructionVisitor::LGNDestructionVisitor(const NodeID * logical)
  : VPVCVisitor(_my_type, 0, -1, 0, logical, logical) { }

LGNDestructionVisitor::LGNDestructionVisitor(const LGNDestructionVisitor & rhs)
  : VPVCVisitor(rhs) { }

LGNDestructionVisitor::~LGNDestructionVisitor() { }

Visitor * LGNDestructionVisitor::dup(void) const
{ return new LGNDestructionVisitor(*this); }

const VisitorType LGNDestructionVisitor::GetType(void) const
{ return VisitorType(GetClassType()); }

const vistype & LGNDestructionVisitor::GetClassType(void) const
{ return _my_type; }

Conduit * LGNDestructionVisitor::at(Factory * f, Creator * c)
{
  Conduit * a = SideA(), * b = SideB();
  
  assert(f && c);

  Conduit * newc = 0;
  if (a && b) {
    newc = c->Create(this);
    if (!newc) {
      Suicide();
      return 0;
    }

    if (Join(B_half(a), A_half(newc), this, this)) {
      if (Join(B_half(b), B_half(newc), this, this)) {
	switch (EnteredFrom()) {
	case A_SIDE:
	  A_half(newc)->Accept(this);
	  break;
	case B_SIDE:
	  B_half(newc)->Accept(this);
	  break;
	case OTHER:
	  break;
	}
      } else {
	delete newc;
	Suicide();
      }
    } else {
      delete newc;
      Suicide();
    } // newc is all joined up and ready to go ...
  } else 
    Suicide();

  return newc;
}


 
