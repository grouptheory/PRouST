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

#ifndef _TEST1_
#define _TEST1_

#ifndef LINT
static char const _test1_h_rcsid_[] =
"$Id: test1.h,v 1.2 1998/07/01 14:50:48 mountcas Exp $";
#endif

#include <FW/basics/FW.h>
#include <FW/basics/Visitor.h>
#include <FW/actors/State.h>
#include <FW/actors/Terminal.h>
#include <FW/kernel/Handlers.h>
#include <DS/containers/list.h>

class Database;

class StartTimer : public TimerHandler {
public:
  StartTimer(Terminal *t, Visitor *v);
  ~StartTimer();
 void Callback(void);
  Terminal *_t;
  Visitor *_v;
};

class HelloTerm: public Terminal{
public:
  HelloTerm(Database * db);
  ~HelloTerm();
  void Interrupt(SimEvent* e) {}
  void Absorb(Visitor * v);

  StartTimer *_go1;
  Database * db;
  int seq_num;
};

#endif
