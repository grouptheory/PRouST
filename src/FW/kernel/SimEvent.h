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
#ifndef __SIMEVENT_H__
#define __SIMEVENT_H__

#ifndef LINT
static char const _SimEvent_h_rcsid_[] =
"$Id: SimEvent.h,v 1.1 1999/01/13 19:12:37 mountcas Exp $";
#endif

class Kernel;

#include <FW/kernel/KernelToken.h>
#include <iostream.h>

class SimEvent {
  friend class Kernel;
  friend class SimEntity;
  friend void  InitFW(void);
  friend class TimedDeliveryHandler;
  friend class Debugger;
  friend class Handler;
public:

  SimEvent(SimEntity * from, SimEntity * to, int code = 0);
  virtual ~SimEvent();

  bool IsRegistered(void) const;

  operator int () { return _code; }

  KernelToken GetID(void) const { return _token; }
  int  GetCode(void) const      { return _code; }
  void SetCode(int code)        { _code = code; }

  virtual void Print(ostream & os);

private:

  bool Deregister(void);
  bool Register(void);

  void SetToken(KernelToken kt);

  KernelToken     _token;	 
  bool            _reg;

  SimEntity     * _from;
  SimEntity     * _to;
  int             _code;
};

#endif
