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
 * File: Q93BCreator.cc
 * Author: 
 * Version: $Id: Q93bCreator.cc,v 1.30 1999/02/10 19:12:28 mountcas Exp $
 * Purpose: Creates Q93B FSM Conduits.
 * BUGS:
 */
#ifndef LINT
static char const Q93bCreator_cc_rcsid[] =
"$Id: Q93bCreator.cc,v 1.30 1999/02/10 19:12:28 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include <FW/basics/VisitorType.h>
#include <fsm/visitors/LinkUpVisitor.h>
#include <fsm/forwarder/VCAllocator.h>
#include <fsm/nni/Q93bCreator.h>
#include <fsm/nni/Q93bCall.h>
#include <fsm/nni/Q93bVisitors.h>

// ----------------- Q93bCreator ---------------------
const VisitorType * Q93bCreator::_uni_visitor_type = 0;
const VisitorType * Q93bCreator::_linkup_visitor_type = 0;

Q93bCreator::Q93bCreator(int port, const NodeID * node, VCAllocator* vcpool)
  : _counter(0), _port(port), _vcpool(vcpool), _node(0)
{
  assert( node != 0 );
  _node = new NodeID( *node );

  if (!_uni_visitor_type)
    _uni_visitor_type = (VisitorType *)QueryRegistry(Q93B_VISITOR_NAME);
  if (!_linkup_visitor_type)
    _linkup_visitor_type = (VisitorType *)QueryRegistry(LINK_UP_VISITOR_NAME);
}

Q93bCreator::~Q93bCreator() 
{ 
  delete _node;
  if ( _vcpool != 0 )
    _vcpool->UnReference(); 
}

Conduit * Q93bCreator::Create(Visitor * v)
{
  Conduit * c = 0;
  
  VisitorType vt = v->GetType();

  if (vt.Is_A(_uni_visitor_type)) {
    Q93bVisitor * qv = (Q93bVisitor *)v;

    DIAG("fsm.nni", DIAG_DEBUG, cout <<
	 "*** " << OwnerName() << " (" << this << 
	 ") *** received " << v->GetType() << " " 
	 << hex << (int)qv->get_crv() << dec << endl);

    if (qv->GetVT() == Q93bVisitor::setup_req || qv->GetVT() == Q93bVisitor::setup_ind) {
      char buf[Conduit::MAX_CONDUIT_NAMELENGTH];
      Call * s = new Call(_port, _vcpool);
      sprintf(buf, "Call-%d", _counter++);
      Register( c = new Conduit(buf, s) );
      s->SetIdentity(c);
    } else {
      diag("fsm.nni", DIAG_FATAL, "%s:%s going thru the Q93bCreator! FIX IT!!\n",
	   v->GetType().Name(), qv->label());
      }
  } else if (vt.Is_A(_linkup_visitor_type)) {
    LinkUpVisitor* lv = (LinkUpVisitor*)v;
    lv->SetVCAllocator(_vcpool);
  } else {
    DIAG("fsm.nni", DIAG_DEBUG, cout <<
	 "*** " << OwnerName() << " (" << this << 
	 ") *** received " << v->GetType() << endl);
  }
  return c;
}


