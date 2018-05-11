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
#ifndef __ACTOR_H__
#define __ACTOR_H__

#ifndef LINT
static char const _Actor_h_rcsid_[] =
"$Id: Actor.h,v 1.1 1999/01/13 19:12:11 mountcas Exp $";
#endif

#include <FW/interface/Shareable.h>

class Conduit;
class State;
class Creator;
class Behavior;
class Registry;
class VisitorType;
class Terminal;
class Cluster;
class Protocol;

class Actor : public Shareable {
  friend class Conduit;
  friend class Behavior;
  friend class Cluster;
  friend class State;
  friend class Accessor;
  friend class Creator;
  friend class Terminal;
  friend class Expander;
  friend class Protocol;
  friend void InitFW(void);
  friend void DeleteFW(void);
  friend const VisitorType * const QueryRegistry(const char * const name);
protected:

  Actor(void);
  virtual ~Actor();

  const char * const OwnerName(void);
  
  void Free(void);
  void RegisterConduit(Conduit * c);

  virtual void ExpanderUpdate(const char * const name);
  virtual void SetParent(const Conduit * c);

private:

  Behavior        * _behavior;
  const char      * _name;
  static Registry * _registry;
  Expander        * _parent_expander;

  void SetOwnerName(const char * const name);

  void       SetBehavior(Behavior* b);
  Behavior * GetBehavior(void) const;
  
  void Authorize_Deletion(void);
  bool _deletion_violation;
};

#endif
 
