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
#ifndef __CREATOR_H__
#define __CREATOR_H__

#ifndef LINT
static char const _Creator_h_rcsid_[] =
"$Id: Creator.h,v 1.1 1999/01/13 19:12:11 mountcas Exp $";
#endif

#include <FW/actors/Actor.h>
#include <FW/kernel/SimEntity.h>
#include <FW/basics/Visitor.h>
#include <DS/containers/list.h>

class Conduit;
class Visitor;
class Factory;
class Behavior;
class Handler;

class Creator : public Actor, public SimEntity {
  friend class Actor;
  friend class Factory;
  friend class Behavior;
  friend class Handler;
  friend class Visitor;
public:

  Creator(void);

  virtual Conduit * Create(Visitor * v) = 0;

  void Deregister(Conduit * c);

protected:
	
  void PassThru(Visitor * v);
  void PassVisitorToA(Visitor* v);
  void PassVisitorToB(Visitor* v);
  enum Visitor::which_side VisitorFrom(Visitor* v);
  
  void Register(Conduit * c);

  virtual ~Creator( );

  list<Conduit *> * _creations;
};

#endif // __CREATOR_H__

