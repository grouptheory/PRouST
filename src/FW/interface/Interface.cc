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
static char const _Interface_cc_rcsid_[] =
"$Id: Interface.cc,v 1.2 1999/03/05 17:30:29 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include "Interface.h"
#include "Shareable.h"
#include "Moniker.h"

fw_Interface::fw_Interface(Shareable * s) 
  : _good(true), _refcount(0), _shareable(s) { }

fw_Interface::~fw_Interface() 
{ 
  if (_refcount) 
    cerr << "Warning!  Refcount for fw_Interface " << this << " is not 0!" << endl;
}

bool fw_Interface::good(void) const { return _good; }
void fw_Interface::Reference(void)  { _refcount++; }
void fw_Interface::Unreference(void) 
{
  if (--_refcount == 0 && !_good)
    delete this;
}

const char * fw_Interface::NameOfShareable(void) const
{
  return _shareable->FullySpecifiedName().asString();
}

void fw_Interface::die(void) 
{ 
  _good = false; 
  ShareableDeath(); 
  if (!_refcount) 
    delete this; 
}
