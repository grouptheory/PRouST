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
#ifndef __SWITCH_TERMINAL_H__
#define __SWITCH_TERMINAL_H__

#ifndef LINT
static char const _SwitchTerminal_h_rcsid_[] =
"$Id: SwitchTerminal.h,v 1.2 1998/12/03 18:12:38 mountcas Exp $";
#endif

#include <FW/actors/Terminal.h>

// When deriving new Terminals these methods must be declared:
//
//      // Takes the Visitor and either passes it to some other framework, or
//      //   processes it internally.
//      void Absorb(Visitor * v);
//

class VisitorType;
class TimerHandler;
class NodeID;

// Prints the receipt of visitors and then destroys them.
class SwitchTerminal : public Terminal {
public:

  SwitchTerminal(const NodeID * n);

  virtual void Absorb(Visitor * v);
  virtual void Interrupt(SimEvent * event);

  void Register(TimerHandler * h) { SimEntity::Register(h); }

protected:

  virtual void InjectionNotification(const Visitor * v) const;

  virtual ~SwitchTerminal() { }

  const NodeID * _LocNodeID;

  static const VisitorType * _fastuni_type;
};

#endif // __SWITCH_TERMINAL_H__
