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

/** -*- C++ -*-
 * File: NNIExpander.h
 * @author 
 * @version $Id: NNIExpander.h,v 1.10 1999/02/10 19:10:03 mountcas Exp $
 * @memo
 * BUGS:
 */

#ifndef __NNI_EXPANDER_H__
#define __NNI_EXPANDER_H__

#ifndef LINT
static char const _NNIExpander_h_rcsid_[] =
"$Id: NNIExpander.h,v 1.10 1999/02/10 19:10:03 mountcas Exp $";
#endif

#include <FW/actors/Expander.h>
#include <codec/pnni_ig/id.h>

class Database;
class Mux;
class NodeID;
class Protocol;
class QueueState;
class VCAllocator;

#define UNI_UPMUX_NAME       "UNI-VPVC-UpperMux"
#define UNI_DOWNMUX_NAME     "UNI-CREF-LowerMux"
#define UNI_FACTORY_NAME     "VC-alloc-Factory"
#define UNI_TRANSLATOR_NAME  "Translator"
#define UNI_LOWER_COORD_NAME "Lower-Coord"
#define UNI_UPPER_COORD_NAME "Upper-Coord"

// NNI
// 
//     Translator
//         |
//   Upper Coordinator
//         |
//     CREF Mux
//    /        \
//  Calls ... Q93BCreator
//    \        /
//     CREF Mux
//         |
//   Lower Coordinator
//
// The NNIExpander represents the Network-Network Interface and the
// layers that support it.  The Translator converts FastUNIVisitors
// into Q93bVisitors and vice versa.  FastUNIVisitors carry Q93b
// messages around inside the switch between the DataForwarder and the
// Control port.  Q93bVisitors carry Q93b messages between the
// Translator and the outside world.  They are converted to byte
// streams of Q93b messages before leaving the switch.  The A side of
// the Translator is the A side of the NNI Expander.
//
// The Upper Coordinator acts as a queue for outgoing messages when
// the link is down.
//
// The VPVC Mux routes outgoing messages to Calls based on VPI and
// VCI.
//
// Each Call is a finite state machine that represents the NNI
// signalling state of a call.
//
// The CREF Mux routes incoming messages to Calls based on call
// reference.
//
// The Lower Coordinator morphs incoming Q93b messages into their
// corresponding indication messages.  E.g. a Q93b setup becomes a
// Q93b setup indication.  It also toggles the call reference flag of
// incoming Q93b messages.  The B side of the Lower Coordinator is the
// B side of the NNI Expander.
//
class NNIExpander : public Expander {
public:

  NNIExpander(int port, const NodeID * node, VCAllocator * vcpool);

  virtual Conduit *GetAHalf(void) const;
  virtual Conduit *GetBHalf(void )const;

protected:

  virtual ~NNIExpander(void);

  Conduit* _translator;
  // Conduit* _queue; // Test
  Conduit* _upper_coord;
  Conduit* _vpvc_mux;
  Conduit* _q93b_factory;
  Conduit* _cref_mux;
  Conduit* _lower_coord;

  //
  // Every port has a number.
  //
  int _port;

  //
  // Every port has a NodeID.
  //
  NodeID *_node;
};

#endif // __NNI_EXPANDER_H__
