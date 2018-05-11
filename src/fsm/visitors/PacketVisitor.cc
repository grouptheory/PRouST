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
static char const _PacketVisitor_cc_rcsid_[] =
"$Id: PacketVisitor.cc,v 1.8 1998/12/22 20:33:00 bilal Exp $";
#endif

#include "PacketVisitor.h"
#include <FW/basics/diag.h>

PacketVisitor::PacketVisitor(vistype & child_type) 
{
  child_type.derived_from( _my_type );
}

PacketVisitor::~PacketVisitor() { }

// -----------------------------------------------------------------------
PacketVisitor * decode(const unsigned char *& buffer, int & length)
{
  PacketVisitor * rval = 0;

  switch (buffer[0]) {
    case 0:
      break;
    default:
      diag("fsm.visitors", DIAG_FATAL, "Unable to decode a buffer into a Visitor, dying ...\n");
      break;
  }

  return rval;
}

// -----------------------------------------------------------------------
