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
 * File: Dummy_q93b.cc
 * Author: 
 * Version: $Id: Dummy_q93b.h,v 1.1 1997/12/24 19:34:56 bilal Exp $
 * Purpose: Implements a do-nothing FSM.  It just passes Visitors along.
 * BUGS:
 */

#ifndef __DUMMY_Q93B_H__
#define __DUMMY_Q93B_H__
#ifndef LINT
static char const _Dummy_q93b_h_rcsid_[] =
"$Id: Dummy_q93b.h,v 1.1 1997/12/24 19:34:56 bilal Exp $";
#endif

#include <FW/actors/State.h>
#include <fsm/nni/VCAllocator.h>
#include <codec/pnni_ig/id.h>

// Dummy_q93b passes a Visitor along and returns itself (thus there is only one State).
class SimEvent;

class Dummy_q93b: public State {
public:

  Dummy_q93b(int port, NodeID* node, VCAllocator* vcpool);

  virtual State * Handle(Visitor* v);
  virtual void Interrupt(SimEvent *);

  void SetIdentity(Conduit* c);

protected:
  virtual ~Dummy_q93b();

  VCAllocator* _vc_oracle;
  Conduit*     _me;
};

#endif