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
#ifndef __NPEERTIMERS_H__
#define __NPEERTIMERS_H__

#ifndef LINT
static char const _NpeerTimers_h_rcsid_[] =
"$Id: NodePeerTimers.h,v 1.11 1998/08/17 16:06:11 mountcas Exp $";
#endif

#include <FW/kernel/Handlers.h>

// A Brief note about timers here and their use:
//
// DSRxTimer: Started when a database summary packet
//            is sent in the NodePeerFsm. When the
//            timer expires the pkt is resent Similarly
//            We have
// ReqRxTimer: for Ptse Request timer sent
// AckTimer: for Ptse Acknowledgement packets
// PTSPRxTimer: For Ptsp packets to be retransmitted

class NodePeerState;
class PTSPPkt;

class DSRxTimer : public TimerHandler {
public:

  DSRxTimer(NodePeerState * s, double time);
  virtual ~DSRxTimer();
  void Callback();

protected:

  NodePeerState * _owner;
};

class ReqRxTimer : public TimerHandler {
public:

  ReqRxTimer(NodePeerState * s, double time);
  virtual ~ReqRxTimer();
  void Callback(); 

protected:

  NodePeerState * _owner;
};

class AckTimer : public TimerHandler {
public:

  AckTimer(NodePeerState * s, double time);
  virtual ~AckTimer();
  void Callback();

protected:

  NodePeerState * _owner;
};

class  PTSPRxTimer : public TimerHandler {
public:

  PTSPRxTimer(NodePeerState * s, PTSPPkt * p, double timer);
  virtual ~PTSPRxTimer();
  void Callback();

protected:

  NodePeerState * _owner;
  PTSPPkt * _for;
};

#endif // __NPEERTIMERS_H__
