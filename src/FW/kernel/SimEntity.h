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
#ifndef __SIMENTITY_H__
#define __SIMENTITY_H__

#ifndef LINT
static char const _SimEntity_h_rcsid_[] =
"$Id: SimEntity.h,v 1.1 1999/01/13 19:12:37 mountcas Exp $";
#endif

#include <DS/containers/list.h>

class SimEvent;
class InputHandler;
class OutputHandler;
class TimerHandler;
class TimedDeliveryHandler;
class Debugger;

class SimEntity {
  friend class TimedDeliveryHandler;
  friend class Debugger;
public:

  SimEntity(void);
  virtual ~SimEntity();

  virtual void Interrupt(SimEvent* e) = 0;

protected:

  void Deliver(SimEvent * e, double time = 0);
  void ReturnToSender(SimEvent * e, double time = 0);
  bool Cancel(SimEvent* e);

  bool Register(TimerHandler* t);
  bool Register(InputHandler* i);
  bool Register(OutputHandler* o);

  bool Cancel(TimerHandler* t);
  bool Cancel(InputHandler* i);
  bool Cancel(OutputHandler* o);

  void ReturnToSenderSynchronous(SimEvent * e);
  void DeliverSynchronous(SimEvent * e);

private:

friend class Creator;
friend class Terminal;
friend class State;
friend class Handler;

  enum se_types {
    UNKNOWN = 0,
    STATE,
    TERMINAL,
    CREATOR
  };

  SimEntity(se_types t, bool regme = true);
  se_types GetType(void) const;

  bool _Deliver(SimEvent* e);

  void UnhookMe(TimedDeliveryHandler * tdh);

  list<TimedDeliveryHandler *> _timers;

  int      _kdb_simentity_id;
  se_types _type;
  bool     _register;
};

#endif //  __SIMENTITY_H__
