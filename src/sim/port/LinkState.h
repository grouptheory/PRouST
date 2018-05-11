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

/* -*- C++ -*-
 * File: LinkState.cc
 * Author: 
 * Version: $Id: LinkState.h,v 1.34 1999/02/19 22:42:19 marsh Exp $
 * Purpose: Implements a Link object, a model of the physical connection
 * between two Conduits.
 * BUGS:
 */
#ifndef __LINK_STATE_H__
#define __LINK_STATE_H__

#ifndef LINT
static char const _LinkState_h_rcsid_[] =
"$Id: LinkState.h,v 1.34 1999/02/19 22:42:19 marsh Exp $";
#endif

#include <FW/actors/State.h>
#include <DS/containers/queue.h>

class SimEvent;

#define VPVC_HEADER 3

class SimEvent;
class HelloVisitor;
class DSVisitor;
class PTSPVisitor;
class ReqVisitor;
class AckVisitor;
class FastUNIVisitor;
class PortVisitor;
class VPVCVisitor;
class Q93bVisitor;

// When deriving a new State these methods must be defined:
//
//      // Handles the passed Visitor and returns itself or a new State object.
//      State * Handle(Visitor * v);
//

// LinkState is an abstract base class, ALL LinkState's must be derived from
//   this class.  There will be NO exceptions!
class LinkState : public State {
public:

  LinkState(int PortA = -1, int PortB = -1);

  void disable(void);
  void enable(void);

protected:

  virtual ~LinkState( );

  int  _portA;
  int  _portB;
  bool _disabled;
};


// DefaultLinkState passes a Visitor along and returns itself (thus there is
// only one State).  LinkState behaves in a special way toward
// PortVisitors.
//
// When a PortVisitor arrives on side A and the LinkState's portB is
// not -1, LinkState changes the Visitor's portB.
//
// When a PortVisitor arrives on side B and the LinkState's portA is
// not -1, LinkState changes the Visitor's portA.
//
class DefaultLinkState : public LinkState {
public:

  DefaultLinkState(int PortA = -1, int PortB = -1);

  virtual State * Handle(Visitor * v);
  virtual void Interrupt(SimEvent * event);

protected:

  virtual ~DefaultLinkState( );

  static const VisitorType * _port_visitor_type;
  static const VisitorType * _pnni_visitor_type;
  // Shouldn't have to do this.
  static const VisitorType * _q93b_visitor_type;
  static const VisitorType * _vpvc_visitor_type;

  static u_char _buf[8192];
};

class PurifyingLinkState : public LinkState {
public:

  PurifyingLinkState(int portA = -1, int portB = -1);

  virtual State * Handle(Visitor* v);
  virtual void Interrupt(SimEvent *event);

protected:

  virtual ~PurifyingLinkState();

private:

  static const VisitorType * _pnni_visitor_type;
  static const VisitorType * _port_visitor_type;
};

class QueuingLinkState : public LinkState {
public:

  QueuingLinkState(double svc_interval, int portA = -1, int portB = -1, int bw = -1);

  virtual State * Handle(Visitor* v);
  virtual void Interrupt(SimEvent *event);

protected:

  virtual ~QueuingLinkState();

  void PtoA(Visitor * v);
  void PtoB(Visitor * v);

  int              _bw;
  bool             _jitter;
  double           _service_time;
  double           _old_svc_time;
  SimEvent       * _serviceEvent;
  ds_queue<Visitor *> _qFromA;	// Visitors that enter on the A side
  ds_queue<Visitor *> _qFromB;	// Visitors that enter on the B side

  static const VisitorType * _port_visitor_type;
  static const VisitorType * _pnni_visitor_type;
  static const VisitorType * _vpvc_visitor_type;
  static const VisitorType * _q93b_visitor_type;
  static const VisitorType * _hello_visitor_type;
  static const VisitorType * _ds_visitor_type;
  static const VisitorType * _req_visitor_type;
  static const VisitorType * _ptsp_visitor_type;
  static const VisitorType * _ack_visitor_type;

  static u_char _buf[8192];
};

#endif


