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

/* -*- C++ -*-
 * File: PassThroughProtocolCreator.cc
 * Author: 
 * Version: $Id: PassThroughProtocolCreator.cc,v 1.4 1997/06/09 20:56:55 talmage Exp $
 * Purpose: Creates Conduits with NullProtocols.
 * BUGS:
 */

#if !defined(LINT)
static char const rcsid[] =
"$Id: PassThroughProtocolCreator.cc,v 1.4 1997/06/09 20:56:55 talmage Exp $";
#endif


extern "C" {
#include <stdio.h>
};

#include <FW/basics/Conduit.h>
#include <sim/port/NullState.h>
#include "PassThroughProtocolCreator.h"
#include <FW/behaviors/Protocol.h>
#include <FW/actors/State.h>
#include <FW/basics/Visitor.h>

// -------------- PassThruProtocol Creator ------------------
PassThroughProtocolCreator::PassThroughProtocolCreator() { }

PassThroughProtocolCreator::~PassThroughProtocolCreator() { }

Conduit * PassThroughProtocolCreator::Create(Visitor * v)
{
  State    * s = new NullState;
  Protocol * p = new Protocol(s);
  char buf[60];
  sprintf(buf, "%s NullProto %x", OwnerName(), p);
  return (new Conduit(buf, p));
}

