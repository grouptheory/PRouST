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
#ifndef _ELECTIONTIMERS_H
#define _ELECTIONTIMERS_H
#ifndef LINT
static char const _ElectionTimers_h_rcsid_[] =
"$Id: ElectionTimers.h,v 1.4 1997/12/12 15:19:51 mountcas Exp $";
#endif

#include <FW/kernel/Handlers.h>

class ElectionState;
class SearchPeerTimer;
class ReelectionTimer;
class PGLInitTimer;
class OverrideUnanimityTimer;

class SearchPeerTimer : public TimerHandler {
public:

  SearchPeerTimer(ElectionState * e, double t);
  virtual ~SearchPeerTimer();
  void Callback(void);

protected:

  ElectionState * _owner;
};


class ReelectionTimer : public TimerHandler {
public:

  ReelectionTimer(ElectionState *e, double t);
  virtual ~ReelectionTimer();
  void Callback(void);

protected:

  ElectionState *_owner;
};

class PGLInitTimer : public TimerHandler {
public:

  PGLInitTimer(ElectionState *e, double t);
  virtual ~PGLInitTimer();
  void Callback(void);

protected:

  ElectionState *_owner;
};

class OverrideUnanimityTimer : public TimerHandler {
public:
  OverrideUnanimityTimer(ElectionState *e, double t);
  virtual ~OverrideUnanimityTimer();
  void Callback(void);

protected:

  ElectionState *_owner;
};

#endif


