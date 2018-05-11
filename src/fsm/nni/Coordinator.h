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
#ifndef __SSCF_H__
#define __SSCF_H__
#ifndef LINT
static char const _SSCF_h_rcsid_[] =
"$Id: Coordinator.h,v 1.10 1999/02/19 22:42:18 marsh Exp $";
#endif

#include <FW/actors/State.h>
#include <DS/containers/queue.h>

class Coordinator : public State {
public:

  enum CoordinatorFunction { DoNotToggleCREF_Flag, ToggleCREF_Flag };

  /// When the Coordinator is constructed with the coordinator_function
  /// argument set to ToggleCREF_Flag, the Coordinator toggles the call
  /// reference flag on incoming Q93b messages.  This permits the
  /// NNIExpander's CREF Mux to "route" the message to the right instance
  /// of the NNI.  E.g. When an incoming call reference is 8001 (i.e. has
  /// the cref flag set), the Coordinator toggles the cref flag, making
  /// the cref 0001.
  ///
  /// When the Coordinator is constructed with the coordinator_function
  /// argument set to DoNotToggleCREF_Flag, it does not modify the
  /// incoming call reference.
  ///
  Coordinator(CoordinatorFunction coordinator_function = DoNotToggleCREF_Flag);
  virtual ~Coordinator(void);

  virtual State * Handle(Visitor* v);
  virtual void Interrupt(SimEvent *event);

protected:

  bool IsPortActive(void);

private:

  CoordinatorFunction _coordinator_function;
  bool                _port_active;
  /// This is required to queue up visitors until SSCF comes up
  ds_queue<Visitor *>    _q_for_uni;
  /// SSCOP sends us this visitor type with linkstatus
  static const VisitorType * _linkstatus_type;
  static const VisitorType * _q93b_type;
  static const VisitorType * _vpvc_type;
};

#endif
