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
#ifndef __TERMINAL_H__
#define __TERMINAL_H__
#if !defined(LINT)
static char const _Terminal_h_rcsid_[] =
"$Id: Terminal.h,v 1.1 1999/01/13 19:12:11 mountcas Exp $";
#endif

#include <FW/actors/Actor.h>
#include <FW/kernel/SimEntity.h>
class Visitor;
class Adapter;

class Terminal : public Actor, public SimEntity {
  friend class Adapter;
public:

  inline Terminal(void) 
    : Actor(), SimEntity(SimEntity::TERMINAL) { }

  virtual void Absorb(Visitor * v) = 0;
  void Inject(Visitor * v);

protected:

  // This method may NOT modify the Visitor OR the Terminal
  //   It may only make queries.  If you cast away the constness
  //   I will hunt you down and kill you.
  virtual void InjectionNotification(const Visitor * v) const { }

  inline virtual ~Terminal() { }
};

#endif
