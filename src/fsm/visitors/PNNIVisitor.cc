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
 * File: PNNIVisitor.cc
 * @author talmage
 * @version $Id: PNNIVisitor.cc,v 1.22 1998/12/15 17:34:09 mountcas Exp $
 *
 * Carries PNNI messages around in a switch.
 *
 * BUGS:
 */
#ifndef LINT
static char const rcsid[] =
"$Id: PNNIVisitor.cc,v 1.22 1998/12/15 17:34:09 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <codec/pnni_ig/id.h>
#include "PNNIVisitor.h"

//
// MAKE_VPI() and MAKE_VCI() assume 32-bit inputs.
//
#define MAKE_VPI(vpvc) (((vpvc) & 0x00ff0000) >> 16)
#define MAKE_VCI(vpvc) ((vpvc) & 0x0000ffff)


PNNIVisitor::PNNIVisitor(int port, int agg, int vpvc, 
			 const NodeID * source_node, 
			 const NodeID * dest_node)
  : VPVCVisitor( _mytype, port, agg, vpvc, source_node, dest_node ),
    PacketVisitor( _mytype ) { }

const VisitorType PNNIVisitor::GetType(void) const 
{   return VisitorType(GetClassType());  }

PNNIVisitor::PNNIVisitor(const PNNIVisitor & rhs) 
  : VPVCVisitor( rhs ), PacketVisitor( _mytype ) { }

PNNIVisitor::PNNIVisitor(vistype & child_type, int port, int agg, int vpvc, 
			 const NodeID * source_node, const NodeID * dest_node)
  : VPVCVisitor( child_type.derived_from( _mytype ), port, agg, vpvc,
		 source_node, dest_node ),
    PacketVisitor( child_type.derived_from(_mytype) ) { }

PNNIVisitor::~PNNIVisitor() {  }

const vistype & PNNIVisitor::GetClassType(void) const
{
  return _mytype;
}
