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
static char const _Anchor_cc_rcsid_[] =
"$Id: Anchor.cc,v 1.2 1999/02/26 14:14:05 mountcas Exp $";
#endif

#include <codec/pnni_ig/id.h>

#include "Anchor.h"

Anchor::Anchor(int ptse_id, NodeID* origin, int port) {
  _origin = origin;
  _port = port;
  _ptse_id = ptse_id;
}

Anchor::~Anchor() {
  delete _origin;
  _port    = -1;
  _ptse_id = -1;
}

NodeID* Anchor::ShareOrigin(void) const {
  return _origin;
}

int Anchor::GetPort(void) const {
  return _port;
}

int Anchor::GetID(void) const {
  return _ptse_id;
}
