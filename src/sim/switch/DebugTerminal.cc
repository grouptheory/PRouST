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

/* -*- C++ -*-
 * File: DebugTerminal.cc
 * Author: 
 * Version: $Id: DebugTerminal.cc,v 1.17 1998/12/03 18:36:17 mountcas Exp $
 * Purpose: This is a Terminal object.  It prints its Visitors 
 * and kills them.
 * BUGS:
 */
#ifndef LINT
static char const rcsid[] =
"$Id: DebugTerminal.cc,v 1.17 1998/12/03 18:36:17 mountcas Exp $";
#endif

#include <iostream.h>
#include <sim/switch/DebugTerminal.h>
#include <FW/basics/diag.h>
#include <FW/basics/Visitor.h>
#include <FW/kernel/Kernel.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/netstats/NetStatsCollector.h>
#include <codec/pnni_ig/id.h>

const VisitorType * DebugTerminal::_fastuni_type = 0;

DebugTerminal::DebugTerminal(const NodeID * n) 
  : Terminal(), _LocNodeID(0)
{ 
  assert(n != 0);
  _LocNodeID = n->copy();

  if (! _fastuni_type)
    _fastuni_type = QueryRegistry(FAST_UNI_VISITOR_NAME);
}

DebugTerminal::~DebugTerminal( ) { }

void DebugTerminal::Register(TimerHandler * h) 
{ 
  SimEntity::Register(h); 
}

void DebugTerminal::Absorb(Visitor* v)
{
  VisitorType vt = v->GetType();

  DIAG("sim.terminal", DIAG_DEBUG, cout << OwnerName() 
       << " received " << vt << " at time " << theKernel().CurrentElapsedTime() << endl);

  if (vt.Is_A(_fastuni_type)) {
    FastUNIVisitor * fuv = (FastUNIVisitor *)v;
    if (fuv->GetMSGType() == FastUNIVisitor::FastUNISetup)
      // call the NetStatsCollector with type Call_Arrival
      theNetStatsCollector().ReportNetEvent("Call_Arrival",
					  OwnerName(),
					  0, _LocNodeID);
  }
  v->Suicide();
}

void DebugTerminal::InjectionNotification(const Visitor * v) const
{
  VisitorType vt = v->GetType();

  if (vt.Is_A(_fastuni_type)) {
    const FastUNIVisitor * fuv = (const FastUNIVisitor *)v;
    if (fuv->GetMSGType() == FastUNIVisitor::FastUNISetup) {
      // call the NetStatsCollector with type Call_Submission
      theNetStatsCollector().ReportNetEvent("Call_Submission",
					  ((DebugTerminal *)this)->OwnerName(),
					  0, _LocNodeID);
    }
  }
}

void DebugTerminal::Interrupt(SimEvent *event)
{
  DIAG(SIM, DIAG_DEBUG, 
       cout << OwnerName() << " interrupted by " << (int)event << endl);
}
