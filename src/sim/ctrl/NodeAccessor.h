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
#ifndef __NODEACCESSOR_H__
#define __NODEACCESSOR_H__

#ifndef LINT
static char const _NodeAccessor_h_rcsid_[] =
"$Id: NodeAccessor.h,v 1.29 1999/02/02 17:37:53 talmage Exp $";
#endif

#include <FW/actors/Accessor.h>
#include <codec/pnni_ig/id.h>
#include <DS/containers/dictionary.h>

class SimEvent;

/** The NodeIDAccessor multiplexes Visitors based upon the Visitor's Node ID.
 */
class NodeIDAccessor : public Accessor {
public:

  /**@name Enumeration of ID's for all of the IGs.
   */
  //@{
  enum direction { 
    /// The Accessor should use the Visitor's Source Node ID
    SOURCE = 0, 
    /// The Accessor should use the Visitor's Destination Node ID
    DESTINATION = 1 
  };
  //@}

  /// Constructor takes an enumerated direction
  NodeIDAccessor(direction dir, bool slave = false);

  /// Method which returns the proper destination for the Visitor
  Conduit * GetNextConduit(Visitor * v);

protected:

  /// Destructor
  virtual ~NodeIDAccessor( );

  /// Broadcasts the Visitor on to every other Conduit connected to this Mux.
  bool Broadcast(Visitor * v);
  /// Add a Conduit, Visitor mapping
  bool Add(Conduit * c, Visitor * v);
  /// Delete a Conduit, Visitor mapping
  bool Del(Conduit * c);
  /// Delete a Conduit, Visitor mapping
  bool Del(Visitor * v);

  /// Direction to use as the key.
  direction                      _dir;
  /// if true, this accessor delegates to it's peer when adding new conduits
  bool                           _slave;
  /// The map which contains NodeID --> destination
  dictionary<const NodeID *, Conduit *>  _access_map;
  /// Class-wide pointer to the VPVC type of Visitor
  static const VisitorType     * _vpvc_type;
  /// Class-wide pointer to the Link type of Visitor
  static const VisitorType     * _link_type;

  static const VisitorType     * _npflood_type;
};


/** The AddrAccessor multiplexes Visitors based upon the Visitor's Node ID.
 */
class AddrAccessor : public Accessor {
public:

  /**@name Enumeration of ID's for all of the IGs.
   */
  //@{
  enum direction { 
    /// The Accessor should use the Visitor's Source Node ID
    SOURCE = 0, 
    /// The Accessor should use the Visitor's Destination Node ID
    DESTINATION = 1 
  };
  //@}

  /// Constructor takes an enumerated direction
  AddrAccessor(direction dir, bool slave = false);

  /// Method which returns the proper destination for the Visitor
  Conduit * GetNextConduit(Visitor * v);

protected:

  /// Destructor
  virtual ~AddrAccessor( );

  /// Broadcasts the Visitor on to every other Conduit connected to this Mux.
  bool Broadcast(Visitor * v);
  /// Add a Conduit, Visitor mapping
  bool Add(Conduit * c, Visitor * v);
  /// Delete a Conduit, Visitor mapping
  bool Del(Conduit * c);
  /// Delete a Conduit, Visitor mapping
  bool Del(Visitor * v);

  /// Direction to use as the key.
  direction                      _dir;
  /// if true, this accessor delegates to it's peer when adding new conduits
  bool                           _slave;
  /// The map which contains Addr --> destination
  dictionary<Addr *, Conduit *>  _access_map;
  /// Class-wide pointer to the VPVC type of Visitor
  static const VisitorType     * _vpvc_type;
  /// Class-wide pointer to the Link type of Visitor
  static const VisitorType     * _link_type;
};

/** The AggAccessor is used by the LogicalSVCExp to multiplex Visitors
    based upon their Aggregation Token.
 */
class AggAccessor : public Accessor {
public:

  /// Constructor
  AggAccessor(void);

  /// Method which returns the proper destination for the Visitor
  Conduit * GetNextConduit(Visitor * v);

  void InformOfSVCExp(Conduit* exp);

protected:

  /// Destructor
  virtual ~AggAccessor();
  
  /// Broadcasts the Visitor to all connected Conduits.
  bool Broadcast(Visitor * v);
  /// Adds a Conduit, Visitor mapping to the table
  bool Add(Conduit * c, Visitor * v);
  /// Deletes a Conduit, Visitor mapping from the table based upon the Conduit
  bool Del(Conduit * c);
  /// Deletes a Conduit, Visitor mapping from the table based upon the Visitor
  bool Del(Visitor * v);

  /// The map which contains AggregationToken --> destination
  dictionary<int, Conduit *>  _access_map;
  /// Class-wide pointer to the UNI type of Visitor
  static const VisitorType  * _port_visitor_type;
  /// Class-wide pointer to the UNI type of Visitor
  static const VisitorType  * _fast_uni_type;

  // The SVCExp conduit in which this Mux resides
  Conduit* _exp;
};

#endif // __NODEACCESSOR_H__
