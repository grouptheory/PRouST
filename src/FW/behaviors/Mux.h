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
#ifndef __MUX_H__
#define __MUX_H__

#ifndef LINT
static char const _Mux_h_rcsid_[] =
"$Id: Mux.h,v 1.1 1999/01/13 19:12:50 mountcas Exp $";
#endif

#include <FW/behaviors/Behavior.h>
#include <FW/actors/Accessor.h>

class Mux;
class Visitor;
class Accessor;
class Conduit;

// Mux Behavior delegates Visitors to it's accessor.
class Mux : public Behavior {
  friend class Behavior;  
public:

  Mux(Accessor * access);

  bool Broadcast(Visitor * v);

protected:

  virtual const Conduit * GetB(Visitor * v);

private:

  virtual ~Mux();

  void Accept(Visitor * v, int is_breakpoint = 0);
  void Suicide(void);

  bool ConnectB(Conduit * b, Visitor * v);
  bool DisconnectB(Conduit * c, class Visitor * v);

  Conduit * MyFactory(void);
};

#endif // __MUX_H__
