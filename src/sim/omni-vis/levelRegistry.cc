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

#ifndef LINT
static char const _levelRegistry_cc_rcsid_[] =
"$Id: levelRegistry.cc,v 1.2 1999/01/05 16:16:18 mountcas Exp $";
#endif
#include "levelRegistry.h"

extern "C" {
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <assert.h>
};

levelRegistry* levelRegistry::_singleton = 0;

levelRegistry* theLevelRegistry(void) {
  assert(levelRegistry::_singleton);
  return levelRegistry::_singleton;
}

bool levelRegistry::isSelected(int level) const {
  if (_all_selected) 
    return true;
  if (_none_selected)
    return false;

  dic_item di = _state.lookup(level);
  if (!di) 
    return false;

  int val = _state.inf(di);
  if (val==0) return false;
  else return true;
}

void levelRegistry::setSelected(int level, bool val) {
  _all_selected  = false;
  _none_selected = false;
  dic_item di = _state.lookup( level );
  if (di) {
    _state.change_inf(di, (int)val);
  }
  else {
    _state.insert(level, (int)val);
  }
}

void levelRegistry::setSelectedAll(void) {
  _all_selected  = true;
  _none_selected = false;
  _state.clear();
}

void levelRegistry::setSelectedNone(void) {
  _all_selected  = false;
  _none_selected = true;
  _state.clear();
}

levelRegistry::levelRegistry(void) {
  assert( !_singleton );
  _singleton=this;
  setSelectedAll();
}

levelRegistry::~levelRegistry(void) {
  assert( (_singleton) && (_singleton==this) );
  _singleton=0;
}
