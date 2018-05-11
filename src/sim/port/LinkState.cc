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
 * File: LinkState.cc
 * Author: 
 * Version: $Id: LinkState.cc,v 1.61 1999/02/24 15:20:01 mountcas Exp $
 * Purpose: Implements a Link object, a model of the physical connection
 * between two Conduits.
 * BUGS:
 */
#ifndef LINT
static char const rcsid[] =
"$Id: LinkState.cc,v 1.61 1999/02/24 15:20:01 mountcas Exp $";
#endif

#include <common/cprototypes.h>

#include <DS/random/distributions.h>
#include <FW/basics/diag.h>
#include <FW/basics/Log.h>
#include <FW/kernel/SimEvent.h>
#include <fsm/visitors/PortVisitor.h>
#include <sim/port/LinkState.h>
#include <fsm/queue/QueueState.h>
#include <fsm/nodepeer/NPVisitors.h>
#include <fsm/hello/HelloVisitor.h>
#include <fsm/nni/Q93bVisitors.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/PortVisitor.h>
#include <fsm/visitors/VPVCVisitor.h>
#include <fsm/visitors/PNNIVisitor.h>
#include <fsm/visitors/LinkVisitor.h>
#include <fsm/netstats/NetStatsCollector.h>
#include <fsm/nodepeer/NPVisitors.h>
#include <fsm/hello/HelloVisitor.h>
#include <codec/pnni_pkt/hello.h>
#include <codec/pnni_pkt/ptsp.h>
#include <codec/pnni_pkt/database_sum.h>
#include <codec/pnni_pkt/ptse_ack.h>
#include <codec/pnni_pkt/ptse_req.h>

#include <iostream.h>

// ------------------------- LinkState --------------------------
LinkState::LinkState(int PortA, int PortB) : 
  State(), _portA(PortA), _portB(PortB), _disabled(false) { }

LinkState::~LinkState( ) { }

void LinkState::disable(void) { _disabled = true; }
void LinkState::enable(void) { _disabled = false; }

// ------------------------- DefaultLinkState --------------------------
u_char              DefaultLinkState::_buf[8192];
const VisitorType * DefaultLinkState::_port_visitor_type = 0;
const VisitorType * DefaultLinkState::_q93b_visitor_type = 0;
const VisitorType * DefaultLinkState::_vpvc_visitor_type = 0;
const VisitorType * DefaultLinkState::_pnni_visitor_type = 0;

DefaultLinkState::DefaultLinkState(int PortA, int PortB) 
  : LinkState(PortA, PortB)
{
  if (_port_visitor_type == 0)
    _port_visitor_type = QueryRegistry(PORT_VISITOR_NAME);
  if (_q93b_visitor_type == 0)
    _q93b_visitor_type = QueryRegistry(Q93B_VISITOR_NAME);
  if (_vpvc_visitor_type == 0)
    _vpvc_visitor_type = QueryRegistry(VPVC_VISITOR_NAME);
  if (_pnni_visitor_type == 0)
    _pnni_visitor_type = QueryRegistry(PNNI_VISITOR_NAME);
}

DefaultLinkState::~DefaultLinkState( ) { }

State * DefaultLinkState::Handle(Visitor * v)
{
  VisitorType vt = v->GetType();
  
  // LOG Visitor Types that pass thru links
  char buf[64];
  sprintf(buf, "LINKSTATE: %s travelling over a link.", vt.Name());
  AppendCommentToLog(buf);

  NetStatsCollector::TrafficTypes type = NetStatsCollector::Unknown;
  double bytes = 0;

  if (_disabled)
    v->Suicide();
  else {
    switch (VisitorFrom(v)) {
      case Visitor::A_SIDE:
	// Added 8:19am 1-30-98 mountcas -- 
	// I SHOULDN'T HAVE TO DO THIS, Q93bVisitors should be inherited from PortVis
	if ((v->GetType().Is_A(_q93b_visitor_type)) && (_portB != -1)) {
	  Q93bVisitor * qv = (Q93bVisitor *)v;
	  qv->set_port(_portB);
	  assert( qv->share_msg() != 0 );
	} else
	  if ((v->GetType().Is_A(_port_visitor_type)) && (_portB != -1)) {
	    PortVisitor * pv = (PortVisitor *)v;
	    pv->SetOutPort(_portB);
	    pv->SetInPort(_portB);
	  }
	// Added 12:54pm 9-14-98 mountcas
	if (v->GetType().Is_A(_vpvc_visitor_type)) {
	  VPVCVisitor * vis = (VPVCVisitor *)v;
	  vis->SetInVP( vis->GetOutVP() );
	  vis->SetInVC( vis->GetOutVC() );
	}

	if (v->GetType().Is_A(_pnni_visitor_type)) {
	  PNNIVisitor * pnni = (PNNIVisitor *)v;
	  int len = 0; u_char * bptr = _buf;
	  
	  DIAG("codec.bilal",DIAG_INFO, 
	       cout << "LinkState is beginning encoding of a packet" << endl; 
	       pnni->encode( bptr, len );
	       cout << "LinkState has finished encoding of a packet" << endl; );
	  bytes = len;
	}
	
	PassVisitorToB(v);
	break;
	
      case Visitor::B_SIDE:
	// Added 8:19am 1-30-98 mountcas --
	// I SHOULDN'T HAVE TO DO THIS, Q93bVisitors should be inherited from PortVis
	if ((v->GetType().Is_A(_q93b_visitor_type)) && (_portA != -1)) {
	  Q93bVisitor * qv = (Q93bVisitor *)v;
	  qv->set_port(_portA);
	  assert( qv->share_msg() != 0 );
	} else
	  if ((v->GetType().Is_A(_port_visitor_type)) && (_portA != -1)) {
	    PortVisitor * pv = (PortVisitor *)v;
	    pv->SetOutPort(_portA);
	    pv->SetInPort(_portA);
	  }
	// Added 12:54pm 9-14-98 mountcas
	if (v->GetType().Is_A(_vpvc_visitor_type)) {
	  VPVCVisitor * vis = (VPVCVisitor *)v;
	  vis->SetInVP( vis->GetOutVP() );
	  vis->SetInVC( vis->GetOutVC() );
	}
	
	if (v->GetType().Is_A(_pnni_visitor_type)) {
	  PNNIVisitor * pnni = (PNNIVisitor *)v;
	  int len = 0; u_char * bptr = _buf;

	  DIAG("codec.bilal",DIAG_INFO, 
	       cout << "LinkState is beginning encoding of a packet" << endl; 
	       pnni->encode( bptr, len );
	       cout << "LinkState has finished encoding of a packet" << endl; );
	  bytes = len;
	}

	PassVisitorToA(v);
	break;
	
      case Visitor::OTHER:
	cerr << "State ERROR: A visitor entered the Protocol from an unexpected source.\n";
	v->Suicide();
	break;
    }
  }

  if (bytes > 0)
    theNetStatsCollector().TrafficNotification( OwnerName(), bytes, type );

  return this;
}

void DefaultLinkState::Interrupt(SimEvent * event) { }

// ----------------------- PurifyingLinkState -------------------
const VisitorType * PurifyingLinkState::_pnni_visitor_type = 0;
const VisitorType * PurifyingLinkState::_port_visitor_type = 0;

PurifyingLinkState::PurifyingLinkState(int portA, int portB) 
  : LinkState(portA, portB)
{
  if (_pnni_visitor_type == 0)
    _pnni_visitor_type = QueryRegistry(PNNI_VISITOR_NAME);  
  if (_port_visitor_type == 0)
    _port_visitor_type = QueryRegistry(PORT_VISITOR_NAME);  
}

PurifyingLinkState::~PurifyingLinkState( ) { }

State * PurifyingLinkState::Handle(Visitor * v)
{
  if (_disabled)
    v->Suicide();
  else {
    if (v->GetType().Is_A(_pnni_visitor_type)) {
      PNNIVisitor * pnni = (PNNIVisitor *)v;
      int len = 0; u_char buf[256], *bptr = buf;
      
      pnni->encode( bptr, len );
      // v->Suicide();
      // pnni =  decode( bptr, len );
      // v = pnni;
    }
    if (v->GetType().Is_A( _port_visitor_type )) {
      PortVisitor * pv = (PortVisitor *)v;

      switch (VisitorFrom(v)) {
        case Visitor::A_SIDE:
	  pv->SetOutPort(_portB);
	  pv->SetInPort(_portB);
	  break;
        case Visitor::B_SIDE:
	  pv->SetOutPort(_portA);
	  pv->SetInPort(_portA);
	  break;
      }
    }
    PassThru(v);
  }
  return this;
}

void PurifyingLinkState::Interrupt(SimEvent *event)
{
  DIAG(SIM, DIAG_DEBUG, 
       cout << OwnerName() << " interrupted by " << (int)event << endl;)
}

// ----------------- QueuingLinkState ----------------------
u_char              QueuingLinkState::_buf[8192];
const VisitorType * QueuingLinkState::_port_visitor_type  = 0;
const VisitorType * QueuingLinkState::_pnni_visitor_type  = 0;
const VisitorType * QueuingLinkState::_vpvc_visitor_type  = 0;
const VisitorType * QueuingLinkState::_q93b_visitor_type  = 0;
const VisitorType * QueuingLinkState::_hello_visitor_type = 0;
const VisitorType * QueuingLinkState::_ds_visitor_type    = 0;
const VisitorType * QueuingLinkState::_req_visitor_type   = 0;
const VisitorType * QueuingLinkState::_ptsp_visitor_type  = 0;
const VisitorType * QueuingLinkState::_ack_visitor_type   = 0;

#define IN_QUEUE_EVENT    0x1010BEEF
#define OUT_QUEUE_EVENT   0x1010CAFE
#define MAX_JITTER        (256.0 / (double)OC3)

QueuingLinkState::QueuingLinkState(double svc_interval, int portA, int portB, int bw) :
  LinkState(portA, portB), _service_time(svc_interval), 
  _old_svc_time(svc_interval), _bw( OC12 ), _serviceEvent(0),
  _jitter( false )
{
  if (_port_visitor_type == 0)
    _port_visitor_type = QueryRegistry(PORT_VISITOR_NAME);
  if (_pnni_visitor_type == 0)
    _pnni_visitor_type = QueryRegistry(PNNI_VISITOR_NAME);
  if (_vpvc_visitor_type == 0)
    _vpvc_visitor_type = QueryRegistry(VPVC_VISITOR_NAME);
  if (_q93b_visitor_type == 0)
    _q93b_visitor_type = QueryRegistry(Q93B_VISITOR_NAME);
  if (_hello_visitor_type == 0)
    _hello_visitor_type = QueryRegistry(HELLO_VISITOR_NAME);
  if (_ds_visitor_type == 0)
    _ds_visitor_type = QueryRegistry(DS_VISITOR_NAME);
  if (_req_visitor_type == 0)
    _req_visitor_type = QueryRegistry(REQ_VISITOR_NAME);
  if (_ptsp_visitor_type == 0)
    _ptsp_visitor_type = QueryRegistry(PTSP_VISITOR_NAME);
  if (_ack_visitor_type == 0)
    _ack_visitor_type = QueryRegistry(ACK_VISITOR_NAME);

  if (bw != -1)
    _bw = bw;

  DIAG("link.jitter", DIAG_DEBUG, 
       _jitter = true;
       );
}

QueuingLinkState::~QueuingLinkState() 
{ 
  Visitor * v;

  // Drain the A-side queue
  while (!_qFromA.empty()) {
    v = _qFromA.pop();
    v->Suicide();
  }
  // Drain the B-side queue
  while (!_qFromB.empty()) {
    v = _qFromB.pop();
    v->Suicide();
  }
}

State * QueuingLinkState::Handle(Visitor * v) 
{ 
  if (_disabled) {
    // perhaps we should clean out the queues if we are disabled?
    //  or should this be done in the Interrupt method?
    v->Suicide();
  } else {
    NetStatsCollector::TrafficTypes type = NetStatsCollector::Unknown;
    double bytes = 0;
    _service_time = _old_svc_time;
    VisitorType vt = v->GetType();

    // Added by Bilal 1/14/99
    if (v->GetType().Is_A(_vpvc_visitor_type)) {
      VPVCVisitor * vis = (VPVCVisitor *)v;
      vis->SetInVP( vis->GetOutVP() );
      vis->SetInVC( vis->GetOutVC() );
    }

    switch (VisitorFrom(v)) {
      case Visitor::A_SIDE:

	if (QueueState::Is_An_OverrideType(vt))
	  PtoB(v);
	else if (QueueState::Is_A_KillMeType(vt))
	  v->Suicide();
	else {
	  if ( v->GetType().Is_A(_q93b_visitor_type) ) {
	    Q93bVisitor * qv = (Q93bVisitor *)v;
	    if (_portB != -1)
	      qv->set_port(_portB);
	    assert( qv->share_msg() != 0 );

	    int len = 0; u_char * bptr = _buf;
	    qv->encode( bptr, len );
	    _service_time = (double)len / (double)_bw;
	    bytes = len;

	    type = NetStatsCollector::UNI;
	  } else if (v->GetType().Is_A(_pnni_visitor_type)) {
	    PNNIVisitor * pnni = (PNNIVisitor *)v;
	    int len = 0; u_char * bptr = _buf;

	    pnni->encode( bptr, len );
	    _service_time = (double)len / (double)_bw;
	    bytes = len;

	    if ( v->GetType().Is_A( _hello_visitor_type ) )
	      type = NetStatsCollector::Hello;
	    else if ( v->GetType().Is_A( _ds_visitor_type )   ||
		      v->GetType().Is_A( _req_visitor_type )  ||
		      v->GetType().Is_A( _ptsp_visitor_type ) ||
		      v->GetType().Is_A( _ack_visitor_type ))
	      type = NetStatsCollector::NodePeer;
	  }

	  _serviceEvent = new SimEvent( this, this, IN_QUEUE_EVENT );

	  if ( _jitter )
	    _service_time += uniform_distribution::sample( 0.0, MAX_JITTER );

	  Deliver(_serviceEvent, _service_time);
	  _qFromA.append(v);
	}
	break;
	
      case Visitor::B_SIDE:

	if (QueueState::Is_An_OverrideType(vt))
	  PtoA(v);
	else if (QueueState::Is_A_KillMeType(vt))
	  v->Suicide();
	else {
	  if (v->GetType().Is_A(_q93b_visitor_type)) {
	    Q93bVisitor * qv = (Q93bVisitor *)v;
	    if ( _portA != -1 ) 
	      qv->set_port(_portA);
	    assert( qv->share_msg() != 0 );

	    int len = 0; u_char * bptr = _buf;
	    qv->encode( bptr, len );
	    _service_time = (double)len / (double)_bw;
	    bytes = len;

	    type = NetStatsCollector::UNI;
	  } else if (v->GetType().Is_A(_pnni_visitor_type)) {
	    PNNIVisitor * pnni = (PNNIVisitor *)v;
	    int len = 0; u_char * bptr = _buf;

	    pnni->encode( bptr, len );
	    _service_time = (double)len / (double)_bw;
	    bytes = len;

	    if ( v->GetType().Is_A( _hello_visitor_type ) )
	      type = NetStatsCollector::Hello;
	    else if ( v->GetType().Is_A( _ds_visitor_type )   ||
		      v->GetType().Is_A( _req_visitor_type )  ||
		      v->GetType().Is_A( _ptsp_visitor_type ) ||
		      v->GetType().Is_A( _ack_visitor_type ))
	      type = NetStatsCollector::NodePeer;
	  }
	  _serviceEvent = new SimEvent( this, this, OUT_QUEUE_EVENT );

	  if ( _jitter )
	    _service_time += uniform_distribution::sample( 0.0, MAX_JITTER );

	  Deliver(_serviceEvent, _service_time);
	  _qFromB.append(v);
	}
	break;
	
      case Visitor::OTHER:
      default:
	cerr << "State ERROR: A visitor entered the Protocol from an unexpected source.\n";
	v->Suicide();
	break;
    }
    if (bytes > 0)
      theNetStatsCollector().TrafficNotification( OwnerName(), bytes, type );
  }
  return this;
}

void QueuingLinkState::Interrupt(SimEvent * event) 
{ 
  if ( event->GetCode() == IN_QUEUE_EVENT ) {
    assert( _qFromA.empty() == false );
    Visitor * v = _qFromA.pop();
    assert( v != 0 );
    
    if (v->GetType().Is_A(_pnni_visitor_type)) {
      PNNIVisitor * pnni = (PNNIVisitor *)v;
      int len = 0; u_char * bptr = _buf;
      
      pnni->encode( bptr, len );
    }
    PtoB(v);
  } else if ( event->GetCode() == OUT_QUEUE_EVENT ) {
    assert( _qFromB.empty() == false );
    Visitor * v = _qFromB.pop();
    assert( v != 0 );
    
    if (v->GetType().Is_A(_pnni_visitor_type)) {
      PNNIVisitor * pnni = (PNNIVisitor *)v;
      int len = 0; u_char * bptr = _buf;
      
      // ENCODE -> DESTROY -> DECODE -> CREATE
      // pnni->encode( bptr, len );
      // v->Suicide();
      // pnni = decode( bptr, len );
      // v = pnni;
    }
    PtoA(v);
  }
  // Either way you die ...
  delete event;
}

void QueuingLinkState::PtoB(Visitor * v)
{
  if ((v->GetType().Is_A(_port_visitor_type)) && (_portB != -1)) {
    PortVisitor * pv = (PortVisitor *)v;
    pv->SetOutPort(_portB);
    pv->SetInPort(_portB);
  }
  PassVisitorToB(v);
}

void QueuingLinkState::PtoA(Visitor * v)
{
  if ((v->GetType().Is_A(_port_visitor_type)) && (_portA != -1)) {
    PortVisitor * pv = (PortVisitor *)v;
    pv->SetOutPort(_portA);
    pv->SetInPort(_portA);
  }
  PassVisitorToA(v);
}
