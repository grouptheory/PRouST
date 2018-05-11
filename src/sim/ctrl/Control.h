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
#ifndef __CONTROL_H__
#define __CONTROL_H__

#ifndef LINT

static char const _Control_h_rcsid_[] =
"$Id: Control.h,v 1.44 1999/02/19 21:22:47 marsh Exp $";
#endif

#include <FW/actors/Expander.h>
#include <FW/actors/Creator.h>

class Visitor;
class Addr;
class ResourceManager;
class NodeID;
class PeerID;
class SimEvent;
class ds_String;
class VCAllocator;

/** Control is the Expander which encapsulates most of the
 * functionality of a switch.  Its components are the CAC Expander
 * which contains ACAC, GCAC, the Database and Logos; the PhysicalNode
 * Expander which contains two muxes, a NodePeerFSM Factory, and an
 * ElectionFSM; the LogicalNode Expander which contains a VC-to-Remote
 * NodeID mapper, two muxes, an ElectionFSM, and a LogicalSVCExpander,
 * which contains the SVC HelloFSM, two muxes and a HelloFSM Factory.  
 */
class Control : public Expander {
  friend class SwitchExpander;
public:

  Control( ds_String *key, NodeID *& myNode,
	   Conduit *& controlTerminal );

  /// Destructor
  virtual ~Control(void);

  /// Returns the logical A-half of Control (in this case the Destination Node ID Mux)
  Conduit * GetAHalf(void) const;
  /// Returns the logical B-half of Control (in this case Logos)
  Conduit * GetBHalf(void) const;

  const NodeID * GetAddress(void) const { return _my_nid; }

private:

  /// Pointer to the Destination Node ID Mux
  Conduit       *_topmux;
  /// Pointer to the Source Node ID Mux
  Conduit       *_botmux;
  /// Pointer to the Call Admission Control Expander
  Conduit       *_manage;
  /// Physical node
  Conduit       *_physical;
  /// Logical Factory.
  Conduit       *_logical_fact;
  ///
  Conduit       *_term;
  /// NodeID of this switch
  NodeID        *_my_nid;
  /// Ptr to CAC Expander
  ResourceManager * _resman;
  ///
  bool  _added;
};

/** Creator that resides in the LogicalNode Factory which is used to
     allocate LogicalNodes when uplinks are received.
 */
class LogicalNodeExpCreator : public Creator {
public:

  /// Constructor, Database is stored so it can be passed to
  ///  all allocated Logical Nodes.
  LogicalNodeExpCreator(void);
  /// Destructor
  virtual ~LogicalNodeExpCreator();

  /// Method which allocates a new Logical Node.
  Conduit * Create(Visitor * v);
  /// Method to handle the arrival of events.
  void      Interrupt(SimEvent * e);

private:

  static const VisitorType * _lgn_inst_type;
  static const VisitorType * _lgn_dest_type;
};

#endif // __CONTROL_H__
