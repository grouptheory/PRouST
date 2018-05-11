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
#ifndef __RXENTRY_H__
#define __RXENTRY_H__

#ifndef LINT
static char const _RXEntry_h_rcsid_[] =
"$Id: RXEntry.h,v 1.2 1998/08/17 16:08:17 mountcas Exp $";
#endif

class PTSPPkt;
class PTSPRxTimer;

// class RXEntry:
// an Entry to be made in the NodePeerFsm's datastructure,
// in PTSP retransmission list. So that when a 
// timer expires the corresponding ptsp is resent. If ptsp
// is acknowledged the RXEntry is removed and the timer
// is removed. The ptsp and the timer assocated are 
// stored in this class
class RXEntry {
  friend int compare(RXEntry * const & lhs, RXEntry * const & rhs);
  // friend ostream & operator << (ostream & os, const RXEntry & lhs);
public:

  RXEntry(PTSPPkt * pkt, PTSPRxTimer * timer);
  ~RXEntry();
  PTSPPkt * GetPTSP(void);
  PTSPRxTimer * GetTimer(void);

private:

  PTSPPkt     * _pp;
  PTSPRxTimer * _timer;
};

#endif // __RXENTRY_H__
