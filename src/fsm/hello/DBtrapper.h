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
#ifndef __DBTRAPPER_H__
#define __DBTRAPPER_H__

#ifndef LINT
static char const _DBtrapper_h_rcsid_[] =
"$Id: DBtrapper.h,v 1.5 1997/08/28 12:25:18 mountcas Exp $";
#endif

#include <FW/actors/State.h> 
#include <FW/kernel/Handlers.h>

class DBtrapper;

class LinkupSendingTimer : public TimerHandler {
public:

  LinkupSendingTimer(DBtrapper *t, Visitor *v1, Visitor *v2);
  ~LinkupSendingTimer();
  void Callback(void);

  DBtrapper *_t;
  Visitor   *_v1;
  Visitor   *_v2;
};


class DBtrapper : public State {
public:

  DBtrapper(void);
  ~DBtrapper();
  virtual State *Handle(Visitor *v);
  virtual void Interrupt(SimEvent *ev);
  int GetStatus() {return _gotit;};
  void BootupLink(Visitor* a, Visitor* b);  

protected:

  int _gotit;
  LinkupSendingTimer *_go1;
};

#endif // __DBTRAPPER_H__
