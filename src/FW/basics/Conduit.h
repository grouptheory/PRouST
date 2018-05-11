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
#ifndef __CONDUIT_H__
#define __CONDUIT_H__

#ifndef LINT
static char const _Conduit_h_rcsid_[] =
"$Id: Conduit.h,v 1.3 1999/01/28 15:53:35 mountcas Exp $";
#endif

#include <FW/behaviors/Behavior.h>
#include <FW/behaviors/Cluster.h>
#include <FW/actors/Creator.h>
#include <FW/actors/Expander.h>
#include <FW/basics/Log.h>
#include <fstream.h>

class Conduit;
class Visitor;
class Factory;
class Creator;

class Conduit {
  friend class Kernel;

  // Behavior Connect() / Disconnect() methods can reach inside a Conduit
  friend bool Behavior::ConnectA(Conduit *a, Visitor *v);
  friend bool Behavior::DisconnectA(Conduit * c, Visitor *v);
  friend bool Behavior::ConnectB(Conduit *b, Visitor *v);
  friend bool Behavior::DisconnectB(Conduit * c, Visitor *v);

  // Cluster (Behavior) Connect() / Disconnect() methods can reach inside a Conduit
  friend bool Cluster::ConnectA(Conduit *a, Visitor *v);
  friend bool Cluster::DisconnectA(Conduit * c, Visitor *v);
  friend bool Cluster::ConnectB(Conduit *b, Visitor *v);
  friend bool Cluster::DisconnectB(Conduit * c, Visitor *v);
  friend void Cluster::SetOwnerName(const char *);

  // Behaviors can ask to see our _sideA and _sideB pointers
  friend Conduit * Behavior::OwnerSideB(void) const;
  friend Conduit * Behavior::OwnerSideA(void) const;

  // Cluster (Behavior) can manipulate our _sideA and 
  // _sideB pointers when we declare our ownership of the Cluster.
  friend void Cluster::SetOwner(Conduit * c);
  
  // at initialization, InitFW stores the special visitor types used by conduits
  friend void InitFW(void);
  friend void DeleteFW(void);

  friend void Actor::RegisterConduit(Conduit * c);

  friend void Creator::Register(Conduit * c);
  friend void Creator::Deregister(Conduit * c);
  friend Creator::~Creator();

  friend void Expander::SetParent(const Conduit *);
  friend const Conduit * Expander::GetParent(void) const;
  friend void Expander::ExpanderUpdate(const char * const name);

  friend class Visitor;

public:

  // CONSTRUCTOR:
  // sets the _name to be a newly allocated string whose contents match the name argument
  // PRECONDITIONS:
  //    Behavior b must be allocated on the heap.
  //    Behaviors and Conduits are in a 1-to-1 relation
  Conduit(const char * name, Behavior* b);

  Conduit(const char * name, Accessor * a);
  Conduit(const char * name, Creator  * c);
  Conduit(const char * name, Expander * e);
  Conduit(const char * name, State    * s);
  Conduit(const char * name, Terminal * t);


  // DESTRUCTOR:
  // deletes the behavior
  virtual ~Conduit();

  // If the Visitor is a 
  //   --Non-framework Visitor, then 
  //       we ask our Behavior to Accept() it
  //   --Framework Visitor, then 
  //       <internals>
  void Accept(Visitor* v);

  // Join() and Sever() requires this manipulator A_half()
  // !_composite, then returns this Conduit after setting the _h field of c to A_HALF
  //  _composite && _sideAhalf==A_HALF, then returns A_half(c->_sideA)
  //  _composite && _sideAhalf==B_HALF, then returns B_half(c->_sideA)
  friend Conduit* A_half(Conduit* c);

  // Join() and Sever() requires this manipulator B_half()
  // !_composite, then returns this Conduit after setting the _h field of c to B_HALF
  //  _composite && _sideBhalf==A_HALF, then returns A_half(c->_sideB)
  //  _composite && _sideBhalf==B_HALF, then returns B_half(c->_sideB)
  friend Conduit* B_half(Conduit* c);

  // Attempts to Join the Conduit c1 to the Conduit c2
  // PRECONDITION:
  //   A_half() or B_half() have been called, on each of the arguments c1,c2

  // This silliness is being done to support Linux's egcs compiler which appears to be quite picky
  friend bool Join(Conduit  * c1, Conduit * c2);
  friend bool Sever(Conduit * c1, Conduit * c2);
  friend bool Join(Conduit  * c1, Conduit * c2, Visitor * v1, Visitor * v2);
  friend bool Sever(Conduit * c1, Conduit * c2, Visitor * v1, Visitor * v2);

  inline const char * const GetName(void) const { return _name; }

  // the maximum length of a Conduit name
  enum {MAX_CONDUIT_NAMELENGTH=1024, MAX_LOG_SIZE=4096};

protected:

  void SetName(const char* name);
  // the Conduit name
  char     * _name;

  enum which_half { HALF_NOT_SET, A_HALF, B_HALF };

  // A_half() and B_half() set this varia
  which_half _h;

  // this helper function is used by friends to manipulate our _sideA/_sideAhalf members
  bool set_sideA(Conduit * a);

  // this helper function is used by friends to manipulate our _sideB/_sideBhalf members
  bool set_sideB(Conduit * b);


  // these helper functions are used to save current connections when Joining.
  inline const Conduit * GetA(Visitor * v = 0) { return _behavior->GetA(v); }
  inline const Conduit * GetB(Visitor * v = 0) { return _behavior->GetB(v); }

  // this helper function is used to retrieve the current Conduit on this' sideA
  friend const Conduit * GetSideA(Conduit * c);

  // this helper function is used to retrieve the current Conduit on this' sideB
  friend const Conduit * GetSideB(Conduit * c);

  // INVARIANT:
  //
  // !_composite && _sideA is a pointer to our single neighboring Conduit in the A direction
  //             && _sideAhalf==HALF_NOT_SET
  //   OR
  // 
  //  _composite && _sideA is a Mux whose B side maintains the connections
  //                       to all our neighbors in the A direction
  //             && _sideAhalf==B_HALF
  //   OR
  // 
  //  _composite && _sideA is the border conduit (in the A direction)
  //                       of our internal Expander
  //             && _sideAhalf is the side (A_HALF or B_HALF) of this 
  //                           border Conduit that is facing the "outside"

  Conduit  * _sideA;
  which_half _sideAhalf;
  
  // INVARIANT:
  //
  // !_composite && _sideB is a pointer to our single neighboring Conduit in the B direction
  //             && _sideBhalf==HALF_NOT_SET
  //   OR
  // 
  //  _composite && _sideB is a Mux whose B side maintains the connections
  //                       to all our neighbors in the A direction
  //             && _sideBhalf==B_HALF
  //   OR
  // 
  //  _composite && _sideB is the border conduit (in the B direction)
  //                       of our internal Expander
  //             && _sideBhalf is the side (A_HALF or B_HALF) of this 
  //                           border Conduit that is facing the "outside"

  Conduit  * _sideB;
  which_half _sideBhalf;

  bool       _composite;
  Behavior * _behavior;

  // The conduit's parent if it is part of a Cluster
  const Conduit * _parent;
  void SetParent(const Conduit * c);
  const Conduit * GetParent(void) const;
  
  // The conduit's creator if it was alloc'ed by a factory.
  Creator * _creator;

  void BuildRegistry(void);
  void AddToRegistry(void);

  // the special visitor types used by conduits
  static Registry * _registry;
  static const VisitorType * _disconnect_type;
  static const VisitorType * _clusterkiller_type;

  int             _kdb_conduit_id;
  bool            _registered;

  // These are things common among all conduits
  void global_conduit_ctors(void);

  static int            _population;

  bool _freed;
  friend class Actor;
  friend class Debugger;

  void Set_Free(void);
};

// Kills the Conduit either by Detonate or dtor
class Bomb {
public:

  Bomb(Conduit * victim);

  void Detonate(void);

protected:

  ~Bomb();

  Conduit * _victim;
};

#endif // __CONDUIT_H__
