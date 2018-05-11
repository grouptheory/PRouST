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
#ifndef __EXPANDER_H__
#define __EXPANDER_H__

#ifndef LINT
static char const _Expander_h_rcsid_[] =
"$Id: Expander.h,v 1.1 1999/01/13 19:12:11 mountcas Exp $";
#endif

#include <FW/actors/Actor.h>
#include <DS/containers/list.h>

class Conduit;
class Cluster;

class Expander : public Actor {
  friend class Cluster;
  friend class Conduit;
  friend Conduit * A_half(const Expander *);
  friend Conduit * B_half(const Expander *);
public:

  virtual Conduit * GetAHalf(void) const = 0;
  virtual Conduit * GetBHalf(void) const = 0;

  bool ProperlyConstructed(void) const;

protected:

  Expander(void);
  virtual ~Expander();
  
  const Conduit * GetParent(void) const;

  // This must be the last line in all derived Expander ctors
  void DefinitionComplete(void);

  void ExpanderUpdate(const char * const name);
  void SetParent(const Conduit * c);

private:

  // Added to fix the grouping problems
  char _prevname[1024];
  bool _properly_constructed;

  list<Conduit *> _children;
};

#endif // __EXPANDER_H__
