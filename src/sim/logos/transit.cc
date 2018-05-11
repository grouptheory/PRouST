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
#ifndef LINT
static char const _transit_cc_rcsid_[] =
"$Id: transit.cc,v 1.10 1998/10/16 20:17:44 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include "transit.h"
#include <codec/pnni_ig/id.h>
#include <DS/containers/dictionary.h>


Transit::Transit (const NodeID * nid, int port)
  : _transit_nid(0), _port(port)
{
  assert(nid);
  _transit_nid = new NodeID(*nid);
}

Transit::Transit (const Transit & rhs)
  : _transit_nid(0), _port(rhs._port)
{
  assert(rhs._transit_nid);
  _transit_nid = new NodeID(*rhs._transit_nid);
}

Transit::~Transit()
{
  if (_transit_nid)
    delete  _transit_nid;
}

// remove and return Transit node id
const NodeID * Transit::TakeNID(void)
{
  NodeID * val = _transit_nid;
  _transit_nid = 0;
  return val;
}

// share NodeID pointer 
const NodeID * Transit::ShareNID(void)
{
  return _transit_nid;
}

int Transit::GetPort(void) { return _port; }

int Transit::GetLevel(void)
{
  return (_transit_nid != 0 ? _transit_nid->GetLevel() : -1);
}

// friends
ostream & operator << (ostream & os, const Transit rhs)
{
  if (rhs._transit_nid)
    os << *(rhs._transit_nid) << ":" << rhs._port << endl;
  return os;
}

int compare (Transit *const &lhs, Transit *const  &rhs)
{
  int result = compare (*lhs->_transit_nid, *rhs->_transit_nid);
  if (result == 0)
    result = (lhs->_port > rhs->_port) ? 1: (lhs->_port < rhs->_port) ? -1 : 0;
  return result;
}
