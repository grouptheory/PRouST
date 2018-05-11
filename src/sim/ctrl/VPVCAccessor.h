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
#ifndef __VCACCESSOR_H__
#define __VCACCESSOR_H__

#ifndef LINT
static char const _VCAccesor_h_rcsid_[] =
"";
#endif

#include <FW/actors/Accessor.h>
#include <codec/pnni_ig/id.h>
#include <DS/containers/dictionary.h>

class VCAccessor : public Accessor {
public:

  enum direction {
    UNKNOWN = 0,
    IN,
    OUT
  };

  /// Constructor
  VCAccessor(direction dir, bool slave = false);

  /// Method which returns the proper destination for the Visitor
  Conduit * GetNextConduit(Visitor * v);

protected:

  /// Destructor
  virtual ~VCAccessor();


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
  /// The map which contains VC --> destination
  dictionary<int, Conduit *>     _access_map;
  /// Class-wide pointer to the VPVC type of Visitor
  static const VisitorType     * _vpvc_type;
  static const VisitorType     * _vpvc_rebinding_type;
  static const VisitorType     * _npflood_type;
};

#endif // __VCMAPPER_H__
