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
#ifndef __HANDLERS_H__
#define __HANDLERS_H__

#ifndef LINT
static char const _Handlers_h_rcsid_[] =
"$Id: Handlers.h,v 1.1 1999/01/13 19:12:37 mountcas Exp $";
#endif

#include <FW/kernel/Kernel.h>
#include <FW/basics/Visitor.h>
#include <FW/actors/State.h>
#include <FW/actors/Terminal.h>

class TimerHandler;
class InputHandler;
class OutputHandler;
class SimEntity;
class SimEvent;
class Kernel;
class State;

// Basic class for the four different types of handlers.
class Handler {
  friend class Kernel;
  friend void test_timeline(void);
  friend class SimEntity;
  friend class TimerHandler;
  friend class InputHandler;
  friend class OutputHandler;
  friend void  InitFW(void);
  friend void  test_kernel(bool);
public:

  virtual void Callback(void) = 0;
  virtual void Print(ostream & os) = 0;

  bool IsRegistered(void) const;

protected:

  Handler(void);
  Handler(SimEntity * owner);

  virtual ~Handler();

  void KernelStop(void);
  void KernelRun(void);
			    
  // for use in callbacks of derived classes

  void PassVisitorToA(Visitor* v);
  void PassVisitorToB(Visitor* v);
  void PassVisitorToSelf(Visitor * v);
  enum Visitor::which_side VisitorFrom(Visitor* v);

  bool Register(TimerHandler * t);
  bool Register(InputHandler * i);
  bool Register(OutputHandler* o);
  bool Register(SimEvent * e);

  bool Cancel(Handler * h);
  bool Cancel(SimEvent * e);

private:

  bool Deregister(void);
  void SetToken(KernelToken kt);

  KernelToken     _token;	 
  SimEntity     * _owner;
};

// ------------------------- TimerHandler ------------------
class TimerHandler : public Handler {
  friend class Handler;
  friend class SimEntity;
  friend class Kernel;
  friend class TimedDeliveryHandler;
public:

  TimerHandler(SimEntity * owner, double timefromnow = 0.0);
  virtual ~TimerHandler();

  KernelTime GetExpirationTime(void) const;

  void ExpiresIn(double timefromnow);

  virtual void Print(ostream& os);
  virtual void Callback(void) = 0;

  virtual bool IsInterruptMessenger(void);

private:

  bool _Register(void);

  void SetExpirationTime(void);
  void SetExpirationTime(KernelTime t);

  KernelTime  _expiry; // time from registration

  double _timefromcreation;

  int    _kdb_timer_id;

protected:

  TimerHandler(double timefromnow=0.0, int override=0);
};

// ------------------------- InputHandler ------------------
class InputHandler : public Handler {
  friend class SimEntity;

public:

  InputHandler(SimEntity * owner, int fd);
  virtual ~InputHandler();

  int GetFD(void) const;

  virtual void Print(ostream& os);
  virtual void Callback(void) = 0;

private:

  bool _Register(void);
  
  int _fd;

  int _kdb_input_id;
};

// ------------------------- OutputHandler ------------------
class OutputHandler : public Handler {
  friend class SimEntity;
public:

  OutputHandler(SimEntity * owner, int fd);
  virtual ~OutputHandler();

  int GetFD(void) const;

  virtual void Print(ostream& os);
  virtual void Callback(void) = 0;

private:

  bool _Register(void);
  
  int _fd;

  int _kdb_output_id;
};

// ------------------------- TimedDeliveryHandler ------------------
class TimedDeliveryHandler : public TimerHandler {
  friend class SimEntity;
  friend class Kernel;
  friend class Debugger;
  friend int compare(TimedDeliveryHandler *const &x, TimedDeliveryHandler *const &y);
public:

  TimedDeliveryHandler(SimEntity * owner, SimEvent * se, double timefromnow);
  virtual ~TimedDeliveryHandler();

  virtual void Print(ostream & os);
  virtual void Callback(void);

  bool IsInterruptMessenger(void);

private:

  SimEvent  * _se;
  SimEntity * _own;

  int _kdb_interrupt_id;
};

class StopTimer : public TimerHandler {
  friend class Kernel;
public:

  StopTimer(double t);
  virtual ~StopTimer();

  void Callback(void);
};

class XXXTimer : public TimerHandler {
  friend class Kernel;
public:

  XXXTimer(double t);
  virtual ~XXXTimer();

  void Callback(void);
};

class QueueTimer : public TimerHandler {
  friend class State;
public:

  QueueTimer(State * s, double time);
  virtual ~QueueTimer();

  void Callback(void);

private:

  State *  _s;
};


#endif // __HANDLERS_H__


