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

#ifndef _UNI_TIMERS_H_
#define _UNI_TIMERS_H_

#ifndef LINT
static char const _Q93bTimers_h_rcsid_[] =
"$Id: Q93bTimers.h,v 1.4 1998/07/14 15:09:32 marsh Exp $";
#endif

#include <FW/kernel/Handlers.h>



class Party;
class Call;

// timers are expressed in msec's

class Q93b_timer: public TimerHandler {
public:
  enum q93b_timer_type {
    timer_t301,
    timer_t303,
    timer_t308,
    timer_t309,
    timer_t310,
    timer_t313,
    timer_t316,
    timer_t317,
    timer_t322,
    timer_t398,
    timer_t399,
  };
  enum q93b_timers {
    t301   = 180000, // 3 min
    t303   = 4000,
    t308   = 30000,
    t309   = 10000,
    t310   = 10000,
    t313   = 4000,
    t316   = 120000,
    t317   = 100000,
    t322   = 4000,
    t398   = 4000,
    t399   = 14000,
  };

  Q93b_timer(q93b_timers t, Call *ca,Party *party);
  virtual  ~Q93b_timer();
  virtual q93b_timer_type Timer_type() = 0; 
  void Callback();
  int SetTimer();
  int StopTimer();
  Call *get_call();
  Party *get_party();
 protected:
  char _timer_diag[3];
  Call *_call;
  Party *_party;
};
  
class Q93b_t301 : public Q93b_timer{
public:
  Q93b_t301(Call *ca,Party *party);
  void Callback();
  q93b_timer_type Timer_type();
};

class Q93b_t303 : public Q93b_timer{
public:
  Q93b_t303(Call *ca,Party *party);
  void Callback();
  q93b_timer_type Timer_type();
};

class Q93b_t308 : public Q93b_timer{
public:
  Q93b_t308(Call *ca,Party *party);
  void Callback();
  q93b_timer_type Timer_type();
};


class Q93b_t309 : public Q93b_timer{
public:
  Q93b_t309(Call *ca,Party *party);
  void Callback();
  q93b_timer_type Timer_type();
};

class Q93b_t310 : public Q93b_timer{
public:
  Q93b_t310(Call *ca,Party *party);
  void Callback();
  q93b_timer_type Timer_type();
};

class Q93b_t313 : public Q93b_timer{
public:
  Q93b_t313(Call *ca,Party *party);
  void Callback();
  q93b_timer_type Timer_type();
};

class Q93b_t316 : public Q93b_timer{
public:
  Q93b_t316(Call *ca,Party *party);
  void Callback();
  q93b_timer_type Timer_type();
};

class Q93b_t317 : public Q93b_timer{
public:
  Q93b_t317(Call *ca,Party *party);
  void Callback();
  q93b_timer_type Timer_type();
};

class Q93b_t322 : public Q93b_timer{
public:
  Q93b_t322(Call *ca,Party *party);
  void Callback();
  q93b_timer_type Timer_type();
};

class Q93b_t398 : public Q93b_timer{
public:
  Q93b_t398(Call *ca, Party *party);
  void Callback();
  q93b_timer_type Timer_type();
};

class Q93b_t399 : public Q93b_timer{
public:
  Q93b_t399(Call *ca,Party *party);
  void Callback();
  q93b_timer_type Timer_type();
};

#endif

