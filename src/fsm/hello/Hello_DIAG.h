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
 * File: Hello_DIAG.h
 * Author: talmage
 * Version: $Id: Hello_DIAG.h,v 1.2 1998/05/04 18:43:26 talmage Exp $
 * Purpose: Defines constants to use for accessing the Hello FSM
 * diagnostic messages.
 */
#if !defined(__HELLO_DIAG_H__)
#define __HELLO_DIAG_H__

#ifndef LINT
static char const _Hello_DIAG_h_rcsid_[] =
"$Id: Hello_DIAG.h,v 1.2 1998/05/04 18:43:26 talmage Exp $";
#endif

#define FSM_HELLO "fsm.hello"

#define FSM_HELLO_PHYSICAL "fsm.hello.physical"
#define FSM_HELLO_PHYSICAL_LINK "fsm.hello.physical.link"

#define FSM_HELLO_LGN "fsm.hello.lgn"
#define FSM_HELLO_LGN_LINK "fsm.hello.lgn.link"

#define FSM_HELLO_RCC "fsm.hello.rcc"

#endif
