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
#ifndef __ROUTECONTROL_H__
#define __ROUTECONTROL_H__

#ifndef LINT
static char const _RouteControl_h_rcsid_[] =
"$Id: RouteControl.h,v 1.25 1999/02/09 14:34:31 mountcas Exp $";
#endif

#include <FW/actors/State.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <codec/uni_ie/PNNI_crankback.h>
#include <DS/containers/dictionary.h>

class NodeID;
class Database;
class Visitor;
class FastUNIVisitor;

class RouteControl : public State {
public:

  RouteControl(const NodeID *);
  virtual ~RouteControl();

  State * Handle(Visitor * v);
  void Interrupt(SimEvent * e);

private:
  
  void SaveSetup(int CREF, FastUNIVisitor * fuv);
  bool SetupFromOutside(FastUNIVisitor * fuv);
  bool SetupFromInside(FastUNIVisitor * fuv);

  class MessageContainer {
  public:
    MessageContainer(const FastUNIVisitor * fuv, SimEvent * se = 0);
    MessageContainer(const MessageContainer & rhs);
    ~MessageContainer( );

    void                           AddCrankback(const PNNI_crankback * c);
    const list<PNNI_crankback *> * GetCrankbacks(void) const;
    const FastUNIVisitor         * GetSetupMessage(void) const;
    void                           KillSetupsDTL(void);
    SimEvent                     * GetSimEvent(void);
    void                           SetCREF(int crv);

  private:

    void RemoveCrankbacks(void);

    FastUNIVisitor         * _setup;
    list<PNNI_crankback *> * _crankbacks;
    SimEvent               * _se;
  };
  friend ostream & operator << (ostream & os, const MessageContainer & mc);

  // Needs to change to CREF/PORT <---> Setup Message  !!!!!!!!!!!!!!!!!!!!!!!
  // CREF <---> (SetupMessage, list<PNNI_crankback *> *) *
  dictionary<int, MessageContainer *>  _saved_setups;
  // CREFEvent::_key <--> (SetupMessage, list<PNNI_crankback *> *) *
  dictionary<int, MessageContainer *>  _pending_setups;

  NodeID   * _myNode;

  static const VisitorType * _fast_uni_type;
};

#endif // __ROUTECONTROL_H__
