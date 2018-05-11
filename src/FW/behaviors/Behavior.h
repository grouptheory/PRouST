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
#ifndef __BEHAVIOR_H__
#define __BEHAVIOR_H__

#ifndef LINT
static char const _Behavior_h_rcsid_[] =
"$Id: Behavior.h,v 1.1 1999/01/13 19:12:50 mountcas Exp $";
#endif

#include <FW/basics/Visitor.h>

class Behavior;
class Conduit;
class Cluster;
class Visitor;
class Actor;
class State;
class Accessor;
class Terminal;
class Expander;

class Behavior {
  friend class Actor;
  friend class Conduit;  
  friend class Cluster;
  friend class State;
  friend class Terminal;
  friend class Creator;
  friend class Accessor;
  friend class Expander;
  //  friend class Visitor;

  friend bool  Join(Conduit* c1, Conduit* c2, Visitor* v1, Visitor* v2);
  friend bool  Sever(Conduit* c1, Conduit* c2, Visitor* v1, Visitor* v2);
  friend const Conduit * GetSideA(Conduit * c);
  friend const Conduit * GetSideB(Conduit * c);

  friend Conduit* Visitor::SideA(void);
  friend Conduit* Visitor::SideB(void);
  friend void Visitor::SaveLocation(Behavior* b,Actor* a);
  friend enum Visitor::which_side Visitor::EnteredFrom(void);
public:

  const char * GetOwnerName(void) const;

  enum types {
    UNKNOWN = 0,
    ADAPTER,
    CLUSTER,
    FACTORY,
    MUX,
    PROTOCOL
  };

  types GetType(void) const;

protected:

  Behavior(Actor * a, types type = UNKNOWN);
  virtual ~Behavior();

  virtual bool ConnectA(Conduit *a, Visitor *v = 0);
  virtual bool DisconnectA(Conduit *a, Visitor *v = 0);

  virtual bool ConnectB(Conduit *b, Visitor *v = 0);
  virtual bool DisconnectB(Conduit *b, Visitor *v = 0);

  virtual void Accept(Visitor * v, int is_breakpoint = 0);

  virtual void Suicide(void);

  virtual void SetOwner(Conduit* c);
  virtual void SetOwnerName(const char * const name);
  
  Conduit* OwnerSideB(void) const;
  Conduit* OwnerSideA(void) const;
  Conduit* GetOwner(void) const;
  
  virtual const Conduit * GetA(Visitor * v);
  virtual const Conduit * GetB(Visitor * v);

  virtual  bool     IsComposite(void) const;

  Conduit * _owner;
  Actor   * _actor;
  types     _type;

private:

  void Authorize_Deletion(void) const;

  const Actor * GetActor(void) const;
};

#endif // __BEHAVIOR_H__
