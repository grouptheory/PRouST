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
 * File: PNNIVisitor.h
 * @author talmage
 * @version $Id: PNNIVisitor.h,v 1.15 1998/12/15 17:31:22 mountcas Exp $
 *
 * Carries PNNI messages around in a switch.
 *
 * BUGS:
 */

#ifndef __PNNI_VISITOR_H__
#define __PNNI_VISITOR_H__

#ifndef LINT
static char const _PNNIVisitor_h_rcsid_[] =
"$Id: PNNIVisitor.h,v 1.15 1998/12/15 17:31:22 mountcas Exp $";
#endif

#include <fsm/visitors/VPVCVisitor.h>
#include <fsm/visitors/PacketVisitor.h>

#define PNNI_VPVC 0x12
#define PNNI_VISITOR_NAME "PNNIVisitor"

class Addr;
class NodeID;

class PNNIVisitor : public VPVCVisitor, public PacketVisitor {
public:

  PNNIVisitor(int port = 0, int agg = -1, int vpvc = PNNI_VPVC, 
	      const NodeID * source_node = 0, const NodeID * dest_node = 0);
  
  virtual const VisitorType GetType(void) const;

protected:

  virtual ~PNNIVisitor();

  PNNIVisitor(const PNNIVisitor & rhs);
  PNNIVisitor(vistype & child_type, int port = 0, int agg = -1, int vpvc = PNNI_VPVC, 
	      const NodeID * source_node = 0, const NodeID * dest_node = 0);

  virtual const vistype & GetClassType(void) const;

private:

  static vistype _mytype;
};

#endif // __PNNIVISITOR_H__
