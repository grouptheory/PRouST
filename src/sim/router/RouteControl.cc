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
static char const _RouteControl_cc_rcsid_[] =
"$Id: RouteControl.cc,v 1.79 1999/02/09 14:34:33 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "RouteControl.h"
#include <sim/router/CREFEvent.h>
#include <FW/basics/diag.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/database/Database.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/PNNI_designated_transit_list.h>
#include <codec/q93b_msg/generic_q93b_msg.h>

const VisitorType * RouteControl::_fast_uni_type = 0;

RouteControl::RouteControl(const NodeID * nid) 
  : _myNode(0)
{ 
  assert( nid != 0 );
  _myNode = new NodeID( *nid );
  
  if (!_fast_uni_type)
    _fast_uni_type = QueryRegistry(FAST_UNI_VISITOR_NAME);
}

RouteControl::~RouteControl() { }

State * RouteControl::Handle(Visitor * v)
{
  bool rval = true;

  DIAG("sim.routecontrol", DIAG_DEBUG, cout <<
       OwnerName() << " received " << v->GetType() << " (" << v << ") ";
       if (v->GetType().Is_A(_fast_uni_type))
         cout << ((FastUNIVisitor *)v)->PrintMSGType();
       cout << endl);
  
  // It must be a FastUNIVisitor
  if (v->GetType().Is_A(_fast_uni_type)) {
    FastUNIVisitor * fuv = (FastUNIVisitor *)v;

    // and must have come from SideB (a.k.a. Logos)
    if (VisitorFrom(v) == Visitor::B_SIDE)
      rval = SetupFromInside(fuv);
    // came from SideA (a.k.a. ACAC, a.k.a. Outside the switch, a.k.a. the real world ...
    else if (VisitorFrom(v) == Visitor::A_SIDE)
      rval = SetupFromOutside(fuv);
    // else it came from beyond ... Oooo, scary.
  }
  if (rval)
    PassThru(v);
  return this;
}

bool RouteControl::SetupFromOutside(FastUNIVisitor * fuv)
{
  // Handle Setups
  if (fuv->GetMSGType() == FastUNIVisitor::FastUNISetup ||
      fuv->GetMSGType() == FastUNIVisitor::FastUNIUplinkResponse) {
    // Check to see if it requires DTL expansion
    const DTLContainer * dtl = fuv->Peek();
    NodeID * topNode = (NodeID *)(dtl ? dtl->GetNID() : 0);

    assert(topNode != 0);

    DatabaseInterface * iface = (DatabaseInterface*)QueryInterface( "Database" );
    assert( iface != 0 && iface->good() );
    iface->Reference();
    bool IAmLogicalNode = iface->LogicalNodeIsMe(fuv->GetDestNID());
    iface->Unreference();
    
    // We are on top of the DTL, send it back to ACAC (A side)
    if ((topNode && _myNode->equals(topNode)) ||
	// OR we are the destination
	(_myNode->equals(fuv->GetDestNID()) ||
	 (IAmLogicalNode))) {
      if (topNode && _myNode->equals(topNode)) {
	diag("sim.routecontrol", DIAG_DEBUG, 
	     "%s FastUNIVisitor %x does not require DTL expansion, "
	     "sending back up to Admission Control.\n", OwnerName(), fuv);
      } else {
	// We are the destination.
	DIAG("sim.routecontrol", DIAG_DEBUG, cout
	     << OwnerName() << " FastUNIVisitor " << fuv
	     << " has arrived at its destination " << *_myNode
	     << " sending back up to Admission Control." << endl;);
      }
      PassVisitorToA(fuv);
      return false;
    }
    // else it requires DTL expansion
    // Set the SimEvent on this Visitor so we recognize it on the way back from Logos
    CREFEvent * se = new CREFEvent(this, this, 0);
    // Save it in its pristine state.
    MessageContainer * mc = new MessageContainer(fuv);
    _pending_setups.insert(se->GetKey(), mc);
    fuv->SetReturnEvent(se);
  } else if (fuv->GetMSGType() == FastUNIVisitor::FastUNISetupFailure ||
	     fuv->GetMSGType() == FastUNIVisitor::FastUNIRelease) {
    // handle failures/releases coming from ACAC
    dic_item di;
    
    // Do I have a saved Setup message for this Release/Failure?
    if (di = _saved_setups.lookup(fuv->GetCREF())) {
      MessageContainer * mc = _saved_setups.inf(di);
      PNNI_crankback * pc = 0;
      // Does the Failure/Release have crankback information?
      if (pc = fuv->GetCrankback()) {
	// Store the PNNI_crankback
	mc->AddCrankback(pc);
	// Store the Release's incoming port, cref, vp, vc
	  // Use the oport because iport is zero.  oport was set to the
	  // Right Value by ACAC when he released the call.
	int iport = fuv->GetOutPort(), icref = fuv->GetCREF(), 
	    ivp = fuv->GetInVP(), ivc = fuv->GetInVC();
	// Morph the Failure into a Setup for Logos
	*fuv = *(mc->GetSetupMessage());

	// otherwise the bindings will not be set correctly in the Forwarder
	// Set the in fields correctly
	fuv->SetInPort(iport);
	fuv->SetCREF(icref);
	fuv->SetInVP(ivp);
	fuv->SetInVC(ivc);

	// Needed for SVCs and won't harm DF
	fuv->SetOutVP(ivp);
	fuv->SetOutVC(ivc);
	// Add the list of crankbacks to the Setup message
	fuv->SetCrankbackList(mc->GetCrankbacks());

	// call the NetStatsCollector with type Crankback

	DIAG("sim.routecontrol", DIAG_DEBUG, cout
	     << OwnerName() << " Attempting to crankback (" << fuv << ") " << *fuv << endl;);

	// Send it to Logos
	PassVisitorToB(fuv);
      } else {
	DIAG("sim.routecontrol", DIAG_DEBUG, cout << OwnerName() << 
	     " Removing saved Setup for " << fuv->GetType() << " (" << 
	     fuv << ")" << " and sending the Setup (" << mc->GetSetupMessage() << 
	     ") to ACAC for Release." << endl);
	// There is no crankback IE in this Failure, but I've obviously added to its
	//  DTL in the past.  So remove all the stored information for this Setup.
	MessageContainer * saved = _saved_setups.inf(di);
	_saved_setups.del_item(di);
	// Pass it back to ACAC so the call can be properly released.
	PassVisitorToA(fuv);
	delete saved;
      }
    } else {
      // This failure/release was not saved by me, so check in the pending setups
      SimEvent * se = 0;
      if (se = fuv->GetReturnEvent()) {
	CREFEvent * ce = (CREFEvent *)se;
	// I think my return event is still stuck in the Visitor
	if (di = _pending_setups.lookup(ce->GetKey())) {
	  // I did add to the DTL, let's see if there's crankback information ...
	  if (fuv->GetCrankback()) {
	    // There is, let Logos try and crank it back
	    MessageContainer * mc = _pending_setups.inf(di);
	    // Store the PNNI_crankback
	    mc->AddCrankback(fuv->GetCrankback());
	    // Morph the Failure into a Setup for Logos
	    *fuv = *(mc->GetSetupMessage());
	    // Add the list of crankbacks to the Setup message
	    fuv->SetCrankbackList(mc->GetCrankbacks());
	    // This is so I can continue to recognize it
	    fuv->SetReturnEvent(ce);

	    // call the NetStatsCollector with type Crankback

	    DIAG("sim.routecontrol", DIAG_INFO, cout
		 << OwnerName() << " Attempting to crankback (" << fuv << ") " << *fuv << endl;);
	    // Pass it on to Logos
	    PassVisitorToB(fuv);
	  } else {
	    // no crankback, so just clean up the pending list
	    delete _pending_setups.inf(di);
	    _pending_setups.del_item(di);
	    //  and send it back to ACAC unharmed
	    PassVisitorToA(fuv);
	  }
	} else {
	  // guess it wasn't my return event ... wierd
	  fuv->SetReturnEvent(se);

	  DIAG("sim.routecontrol", DIAG_DEBUG, 
	       cout << OwnerName() << " has no information pending for " 
	       << fuv->GetType() << " (" << fuv << ")" 
	       << ", sending it back up toward Admission Control." 
	       << endl;);

	  PassVisitorToA(fuv);
	}
      } else {
	// I must've not added to the DTL
	PassVisitorToA(fuv);
      }
    }
    return false;
  } // else I don't know what kind of FastUNIVisitor this is, so just pass it thru

  return true; 
}

bool RouteControl::SetupFromInside(FastUNIVisitor * fuv)
{
  dic_item di;
  SimEvent * se;

  if (fuv->GetMSGType() == FastUNIVisitor::FastUNISetup ||
      fuv->GetMSGType() == FastUNIVisitor::FastUNIUplinkResponse) {
    // if it came to us with a return event then we've seen it before
    if (se = fuv->GetReturnEvent()) {
      CREFEvent * ce = (CREFEvent *)se;
      // I've seen this before, let it slide
      if (di = _pending_setups.lookup(ce->GetKey()))
	fuv->SetReturnEvent(ce);
      else {
	CREFEvent * new_se = new CREFEvent(this, this, 0);
	// I haven't seen this before, so save it - But wait, who's SimEvent is that?!?!?!
	MessageContainer * mc = new MessageContainer(fuv, se);
	// We haven't saved pending information for this Setup yet ... so do it now.
	_pending_setups.insert(new_se->GetKey(), mc);
	fuv->SetReturnEvent(new_se);

	DIAG("sim.routecontrol", DIAG_DEBUG, 
	     cout << OwnerName() << " Saving information for " << fuv->GetType() 
	     << " (" << fuv << ").  After receiving it from Logos." 
	     << endl << "[" << new_se->GetKey() << "] " << *mc;);
      }
    } else {
      // no return event, but perhaps we've seen this Setup before
      if (!(di = _saved_setups.lookup(fuv->GetCREF()))) {
	// not in saved setups, so add it to pending
	MessageContainer * mc = new MessageContainer(fuv);
	CREFEvent * ce = new CREFEvent(this, this, 0);
	fuv->SetReturnEvent(ce);
	// Save a copy of the Setup
	_pending_setups.insert(ce->GetKey(), mc);
	mc->KillSetupsDTL();

	DIAG("sim.routecontrol", DIAG_DEBUG, 
	     cout << OwnerName() << " Saving information for " << fuv->GetType() << " (" << fuv << ")." 
	     << endl << "[" << ce->GetKey() << "] " << *mc;);
      } else {
	// it's in the saved setups dictionary ...
	MessageContainer * mc = _saved_setups.inf(di);
	CREFEvent * ce = new CREFEvent(this, this, 0);
	// This will enable us to update the CREF value
	fuv->SetReturnEvent(ce);
	mc->KillSetupsDTL();

	// Move it to a new location (from saved to pending)
	_saved_setups.del_item(di);
	_pending_setups.insert(ce->GetKey(), mc);

	DIAG("sim.routecontrol", DIAG_DEBUG, 
	     cout << OwnerName() << " Saving information for " << fuv->GetType() << " (" << fuv << ")." 
	     << endl << "[" << ce->GetKey() << "] " << *mc;);
      }
    }
  } else if (fuv->GetMSGType() == FastUNIVisitor::FastUNISetupFailure ||
	     fuv->GetMSGType() == FastUNIVisitor::FastUNIRelease) {
    // A release coming from Logos means we cannot do anymore here ...
    if (di = _saved_setups.lookup(fuv->GetCREF())) {
      // We have saved information that needs to be released
      delete _saved_setups.inf(di);
      _saved_setups.del_item(di);
    } else if (se = fuv->GetReturnEvent()) {
      CREFEvent * ce = (CREFEvent *)se;
      if (di = _pending_setups.lookup(ce->GetKey())) {
	delete _pending_setups.inf(di);
	_pending_setups.del_item(di);
      } else // not my SimEvent
	fuv->SetReturnEvent(se);
    }
    // no return event and no saved information, just let it pass through
  }
  return true;
}

void RouteControl::Interrupt(SimEvent * e) 
{ 
  if (e->GetCode() == CREF_EVENT_CODE) {
    CREFEvent * ce = (CREFEvent *)e;
    int CREF = ce->GetCREF();

    dic_item di;
    if (di = _pending_setups.lookup(ce->GetKey())) {
      if (CREF) {
	// The call was accepted so move it over to saved setups
	MessageContainer * mc = _pending_setups.inf(di);
	mc->SetCREF(CREF);
	DIAG("sim.routecontrol", DIAG_DEBUG, 
	     cout << OwnerName() << " Moving pending information to saved information."
	     << endl << "[" << CREF << "] " << *mc;);
	SimEvent * tse = mc->GetSimEvent();
	if (tse) {
	  if (tse->GetCode() == CREF_EVENT_CODE) {
	    CREFEvent * sce = (CREFEvent *)tse;
	    sce->SetCREF(CREF);
	  } else
	    tse->SetCode(CREF);
	  Deliver(tse);
	}
	_saved_setups.insert(CREF, mc);
      } else
	delete _pending_setups.inf(di);

      // now clean out the item from the pending list
      _pending_setups.del_item(di);
    }
    // else it's not in my pending list, so I have no idea why I was sent this SimEvent
  }
  // else it's not a CREFEvent and I don't deal with other types of events.
  delete e;
}

void RouteControl::SaveSetup(int CREF, FastUNIVisitor * fuv)
{
  dic_item di;
  PNNI_crankback * cb = fuv->GetCrankback();
  if (di = _saved_setups.lookup(CREF)) {
    // It's in there, so add crankback
    MessageContainer * mc = _saved_setups.inf(di);
    mc->AddCrankback(cb);
  } else {
    // Allocate new MessageContainer for it
    MessageContainer * mc = new MessageContainer(fuv);
    mc->AddCrankback(cb);
    DIAG("sim.routecontrol", DIAG_DEBUG, 
	 cout << OwnerName() << " Saving information for " << fuv->GetType() << " (" << fuv << ")." 
	 << endl);
    _saved_setups.insert(CREF, mc);
  }
}

// -------------------------------------------------------------------------
RouteControl::MessageContainer::MessageContainer(const FastUNIVisitor * fuv, SimEvent * se) 
  : _setup(0), _crankbacks(0), _se(se)
{
  assert(fuv);

  //  generic_q93b_msg * msg = ((FastUNIVisitor *)fuv)->TakeMessage();
  _setup = new FastUNIVisitor(*fuv);
  generic_q93b_msg * copy = ((generic_q93b_msg *)fuv->GetSharedMessage())->copy();
  //  generic_q93b_msg * copy = msg->copy();
  _setup->SetMessage(copy);
  //  ((FastUNIVisitor *)fuv)->SetMessage(msg);
}

RouteControl::MessageContainer::MessageContainer(const RouteControl::MessageContainer & rhs) 
  : _setup(0), _crankbacks(0), _se(rhs._se)
{
  assert(rhs._setup);

  _setup = new FastUNIVisitor(*rhs._setup);

  // copy the list of crankbacks
  if (rhs._crankbacks) {
    _crankbacks = new list<PNNI_crankback *>;
    list_item li;
    forall_items(li, *(rhs._crankbacks)) {
      PNNI_crankback * cb = rhs._crankbacks->inf(li);
      _crankbacks->append(cb->copy());
    }
  }
}

RouteControl::MessageContainer::~MessageContainer( )
{
  _setup->Suicide();
  RemoveCrankbacks();
}

SimEvent * RouteControl::MessageContainer::GetSimEvent(void)
{
  SimEvent * se = _se;
  _se = 0;
  return se;
}

void RouteControl::MessageContainer::AddCrankback(const PNNI_crankback * c)
{
  if (!_crankbacks)
    _crankbacks = new list<PNNI_crankback *>;
  
  _crankbacks->append((PNNI_crankback *)c->copy());
}

void RouteControl::MessageContainer::RemoveCrankbacks(void)
{
  if (_crankbacks) {
    list_item li;
    forall_items(li, *_crankbacks) {
      delete _crankbacks->inf(li);
    }
    _crankbacks->clear();
    delete _crankbacks;
  }
}

const list<PNNI_crankback *> * RouteControl::MessageContainer::GetCrankbacks(void) const
{
  return _crankbacks;
}

const FastUNIVisitor * RouteControl::MessageContainer::GetSetupMessage(void) const
{
  return _setup;
}

void RouteControl::MessageContainer::KillSetupsDTL(void)
{
  _setup->ClearDTL();
}

void RouteControl::MessageContainer::SetCREF(int crv)
{
  _setup->SetCREF(crv);
}

ostream & operator << (ostream & os, const RouteControl::MessageContainer & mc)
{
  const FastUNIVisitor * setup = mc.GetSetupMessage();
  const list<PNNI_crankback *> * crankbacks = mc.GetCrankbacks();

  if (setup) os << *setup << endl;
  else os << "Setup: (Null)" << endl;
  if (crankbacks) {
    list_item li;
    forall_items(li, *crankbacks)
      os << "Crankback: " << *(crankbacks->inf(li)) << endl;
  }
  return os;
}
