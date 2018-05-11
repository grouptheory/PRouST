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
#ifndef __FSM_TMPL_H__
#define __FSM_TMPL_H__

#ifndef LINT
static char const _fsm_tmpl_h_rcsid_[] =
"$Id: fsm_tmpl.h,v 1.11 1999/01/30 01:36:50 marsh Exp $";
#endif
#include <codec/templates/codec_tmpl.h>

class  NNIReplayer;
class  FastUNIVisitor;
class  RXEntry;
class  Investigator;
class  StatRecord;
class  HorLinkVisitor;
class  InvestigatorTimer;
class  Investigator;

extern int compare(NNIReplayer * const & x, NNIReplayer * const & y);
extern int compare(FastUNIVisitor * const &, FastUNIVisitor * const &);
extern int compare(RXEntry * const &, RXEntry * const &);
extern int compare(Investigator * const &, Investigator * const &);
extern int compare(StatRecord * const &, StatRecord * const &);
extern int compare(HorLinkVisitor * const &, HorLinkVisitor * const &);
extern int compare(InvestigatorTimer * const &, InvestigatorTimer * const &);
extern int compare(Investigator * const &, Investigator * const &);
extern int compare(StatRecord * const &, StatRecord * const &);

#endif
