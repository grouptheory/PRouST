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

//-*-C++-*-
#ifndef _HELLOTIMERS_H_
#define _HELLOTIMERS_H_

#ifndef LINT
static char const _HelloTimers_h_rcsid_[] =
"$Id: HelloTimers.h,v 1.24 1998/06/08 13:33:42 mountcas Exp $";
#endif

#include <FW/kernel/Handlers.h>

class HelloDS;
class HelloState;
class LgnHelloState;
class RCCHelloState;

//--- Physical Hello timers --------------------------
class HelloTimer : public TimerHandler {
public:

  HelloTimer(HelloState *h, double ht);
  virtual ~HelloTimer();
  void Callback(void);
protected:
  HelloState *_owner;
  double _t;
};

class InactivityTimer : public TimerHandler {
public:
  InactivityTimer(HelloState *h, double it);
  virtual ~InactivityTimer();
  void Callback(void);

protected:

  HelloState *_owner;
  double _t;
};


//--- RCC Hello timers --------------------------
class RCCHelloTimer : public TimerHandler {
public:

  RCCHelloTimer(RCCHelloState * s, double ht);
  virtual ~RCCHelloTimer();

  void Callback(void);

protected:

  RCCHelloState * _owner;
  double          _t;
};

// ---------------------------------------------
class RCCInactivityTimer : public TimerHandler {
public:

  RCCInactivityTimer(RCCHelloState  *s, double it);
  virtual ~RCCInactivityTimer();

  void Callback(void);

protected:

  RCCHelloState * _owner;
  double          _t;
};

// --------------------------------------------
class RCCIntegrityTimer : public TimerHandler {
public:

  RCCIntegrityTimer(RCCHelloState * s, double t);
  virtual ~RCCIntegrityTimer();

  void Callback(void);
  bool IsRunning(void) const;

protected:

  RCCHelloState * _owner;
  double          _t;
};

// ---------------------------------------
class RCCInitTimer : public TimerHandler {
public:

  RCCInitTimer(RCCHelloState * s, double t);
  virtual ~RCCInitTimer();

  void Callback(void);

protected:

  RCCHelloState * _owner;
  double          _t;
};

class RetryLGNSVCTimer : public TimerHandler {
public:

  RetryLGNSVCTimer(RCCHelloState * s, double t);
  virtual ~RetryLGNSVCTimer();

  void Callback(void);

protected:

  RCCHelloState * _owner;
  double          _t;
};

//--- LGN Hello timers --------------------------
class LgnIntegrityTimer : public TimerHandler {
public:

  LgnIntegrityTimer(LgnHelloState *h, double t);
  virtual ~LgnIntegrityTimer();
  void Callback(void);

protected:

  LgnHelloState * _owner;
  double          _t;
};

class LgnInactivityTimer : public TimerHandler {
public:

  LgnInactivityTimer(LgnHelloState * h, double t);
  virtual ~LgnInactivityTimer();
  void Callback(void);

protected:

  LgnHelloState * _owner;
  double          _t;
};

#endif // _HELLOTIMERS_H_
