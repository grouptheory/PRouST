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
static char const _HostFunctions_h_rcsid_[] =
"$Id: HostFunctions.h,v 1.8 1999/02/19 21:22:52 marsh Exp $";
#endif
/* -*- C++ -*-
 * File: HostFunctions.h
 * Author: talmage
 * Version: $Id: HostFunctions.h,v 1.8 1999/02/19 21:22:52 marsh Exp $
 * Purpose: Shortcuts to creating and manipulating hosts.
 * BUGS:
 */

#if !defined(__HOST_FUNCTIONS_H__)
#define __HOST_FUNCTIONS_H__

#include <FW/basics/Conduit.h>

class LinkStateWrapper;
class ds_String;

//
// Create a host with name 'name'.  It has only one port, port 0.
//
// To pass a Visitor into the top of the host, wrap the
// Conduit in A_half().  I.e. A_half(host)->Accept(visitor);
//
// Connect a host to a switch with LinkSwitches().  See SwitchFunctions.h.
//
extern Conduit *MakeHost(ds_String *key, ds_String *name, Addr *& myAddress);


#endif


