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
 * File: PortCreator.h
 * Author: 
 * Version: $Id: PortCreator.h,v 1.17 1999/02/10 19:07:49 mountcas Exp $
 * Purpose: Creates Port Conduits as directed by PortInstallerVisitors.
 * BUGS:
 */

#ifndef __PORT_CREATOR_H__
#define __PORT_CREATOR_H__
#ifndef LINT
static char const _PortCreator_h_rcsid_[] =
"$Id: PortCreator.h,v 1.17 1999/02/10 19:07:49 mountcas Exp $";
#endif

#include <FW/actors/Creator.h>

class Database;
class VisitorType;

// Allocates only Ports
class PortCreator : public Creator {
public:

  PortCreator(void);

  virtual Conduit * Create(Visitor * v);

  void Interrupt(SimEvent * e) { }

protected:

  virtual ~PortCreator(void);

  bool _onStrike;	/* Create() sets this to true when
			 * it is visited by a StrikeVisitor.
			 * Create() sets this to false when
			 * it is visited by a StrikeBreakerVisitor.
			 *
			 * Initially, _onStrike is false.
			 */

  static const VisitorType * _strike_vistype;
  static const VisitorType * _sbreaker_vistype;
  static const VisitorType * _hello_vistype;
  static const VisitorType * _portinst_vistype;
};

#endif // __PORT_CREATOR_H__
 
