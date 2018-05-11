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
#ifndef __SHAREABLEREGISTRY_H__
#define __SHAREABLEREGISTRY_H__

#ifndef LINT
static char const _InterfaceRegistry_h_rcsid_[] =
"$Id: ShareableRegistry.h,v 1.1 1999/01/21 14:55:54 mountcas Exp $";
#endif

#include <iostream.h>
#include <DS/containers/dictionary.h>

#include "Moniker.h"
#include "trinode.h"

class Shareable;

bool equal(const char * s1, const char * s2);

class ShareableRegistry {
  friend class Shareable;
  friend trinode<Shareable *> * share_to_trie(const Shareable * s);
  friend Shareable * trie_to_share(const trinode<Shareable *> * t);
  friend ShareableRegistry & theShareableRegistry(void);
public:

  void update(const char * name, Shareable * s);

  trinode<Shareable *> * locate(Moniker requested, 
				 trinode<Shareable *> * requestor, 
				 int & num_found);
  trinode<Shareable *> * locate_below(Moniker requested, 
				       trinode<Shareable *> * requestor, 
				       int & num_found);
  void save(const char * filename) const;
  void print(ostream & os) const;

private:

  ShareableRegistry();
  virtual ~ShareableRegistry();

  void write(trinode<Shareable *> * current, ostream & os) const;

  // add a shareable to the tree
  void insert(const char * name, Shareable * s);
  // move a shareable within the tree
  void modify(const char * name, Shareable * s);
  // locate a shareable within the tree

  dictionary<Shareable *, trinode<Shareable *> *> _s2t;
  // root of the tree
  static trinode<Shareable *>                   * _root;
  // the Shareable Registry is a singleton
  static ShareableRegistry                       * _me;
};

#endif // __SHAREABLEREGISTRY_H__
