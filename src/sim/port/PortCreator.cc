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
 * File: PortCreator.cc
 * Author: 
 * Version: $Id: PortCreator.cc,v 1.44 1999/02/10 19:13:29 mountcas Exp $
 * Purpose: Creates Port Conduits as directed by PortInstallerVisitors.
 * BUGS:
 */
#ifndef LINT
static char const rcsid[] =
"$Id: PortCreator.cc,v 1.44 1999/02/10 19:13:29 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/behaviors/Cluster.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include "NullState.h"
#include "PortCreator.h"
#include "PortExpander.h"
#include <fsm/hello/HelloVisitor.h>
#include <sim/switch/PortInstallerVisitor.h>
#include <sim/switch/StrikeBreakerVisitor.h>
#include <sim/switch/StrikeVisitor.h>
#include <FW/basics/VisitorType.h>

// ----------------- PortCreator ---------------------
const VisitorType * PortCreator::_strike_vistype   = 0;
const VisitorType * PortCreator::_sbreaker_vistype = 0;
const VisitorType * PortCreator::_hello_vistype    = 0;
const VisitorType * PortCreator::_portinst_vistype = 0;

PortCreator::PortCreator(void) : _onStrike(false)
{
  if (! _strike_vistype)
    _strike_vistype = QueryRegistry(STRIKE_VISITOR_NAME);
  if (! _sbreaker_vistype)
    _sbreaker_vistype = QueryRegistry(STRIKE_BREAKER_VISITOR_NAME);
  if (! _hello_vistype)
    _hello_vistype = QueryRegistry(HELLO_VISITOR_NAME);
  if (! _portinst_vistype)
     _portinst_vistype = QueryRegistry(PORT_INSTALLER_VISITOR_NAME);
}

PortCreator::~PortCreator() { }

Conduit * PortCreator::Create(Visitor * v)
{
  Conduit * answer = 0;
  VisitorType vt = v->GetType();

  if (vt.Is_A(_strike_vistype)) {
    _onStrike = true;
    v->Suicide();
  } else if (vt.Is_A(_sbreaker_vistype)) {
    _onStrike = false;
    v->Suicide();
  } else if (!_onStrike && vt.Is_A(_portinst_vistype)) {
    PortInstallerVisitor * pv = (PortInstallerVisitor *)v;
    int port = pv->GetOutPort();
    NodeID * node = pv->StealNode();
    
    char buf[2048];
    sprintf(buf, "Port-%d", port);
    Expander * ex = new PortExpander( port, node );
    Register(answer = new Conduit(buf, ex));
    v->Suicide();
  } else if (vt.Is_A(_hello_vistype)) {
    HelloVisitor * hv = (HelloVisitor *)v;
    diag("sim.port", DIAG_FATAL, 
	 "PortCreator received a HelloVisitor (%x) with in port %d and out port %d\n"
	 "The most likely reason for seeing this message is that you "
	 "miscalculated the number of ports on this switch.", 
	 hv, hv->GetInPort(), hv->GetOutPort());
    v->Suicide();
  }
  return answer;
}
