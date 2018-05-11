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

#if !defined(LINT)
static char const _AddrPtr_h_rcsid_[] =
"$Id: AddrPtr.h,v 1.2 1997/06/21 00:25:27 bilal Exp $";
#endif
/*
 * File: AddrPtr.h
 * Author: talmage
 * Version: $Id $
 * Purpose: Provides a named type that is a pointer to an object of class
 * Addr.  It is used by the AddrPtrSLList class that we create with
 * genclass.
 *
 *	genclass AddrPtr val SLList
 *	genclass AddrPtr val defs
 */

#if !defined(ADDR_PTR_H)
#define ADDR_PTR_H
#include <codec/uni_ie/addr.h>

typedef Addr *AddrPtr;

#endif
