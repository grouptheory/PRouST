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
#ifndef __RCCHelloState_cc__
#define __RCCHelloState_cc__

#ifndef LINT
static char const _RCCHelloState_cc_rcsid_[] =
"$Id: RCCHelloState.cc,v 1.100 1999/03/05 16:44:12 mountcas Exp $";
#endif

#include <fsm/hello/Hello_DIAG.h>
#include <fsm/hello/RCCHelloState.h>
#include <common/cprototypes.h>

#include <FW/basics/diag.h>
#include <FW/basics/Log.h>

#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/lgn_horizontal_link_ext.h>
#include <codec/pnni_pkt/hello.h>
#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/cause.h>

#include <fsm/hello/HelloTimers.h>
#include <fsm/hello/HelloVisitor.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/nodepeer/NPVisitors.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/netstats/NetStatsCollector.h>

#include <fsm/hello/DelayedSendTimer.h>

#include <DS/containers/list.h>

// ---------------------- SVCC Hello FSM --------------------------------
VisitorType * RCCHelloState::_horlink_vistype = 0;
VisitorType * RCCHelloState::_hello_vistype = 0;
VisitorType * RCCHelloState::_fastuni_vistype = 0;
VisitorType * RCCHelloState::_npstate_vistype = 0;
InternalRCCHelloState * RCCHelloState::_StateLinkDown   = 0;
InternalRCCHelloState * RCCHelloState::_StateAttempt    = 0;
InternalRCCHelloState * RCCHelloState::_State1WayInside = 0;
InternalRCCHelloState * RCCHelloState::_State2WayInside = 0;
int                     RCCHelloState::_ref_count       = 0;

void AllocTheStates(void) 
{
  if (!RCCHelloState::_StateLinkDown)
    RCCHelloState::_StateLinkDown = new SVCStateLinkDown();
  if (!RCCHelloState::_StateAttempt)
    RCCHelloState::_StateAttempt = new SVCStateAttempt();
  if (!RCCHelloState::_State1WayInside)
    RCCHelloState::_State1WayInside = new SVCState1WayInside();
  if (!RCCHelloState::_State2WayInside)
    RCCHelloState::_State2WayInside = new SVCState2WayInside();

  // Allocs the VisitorTypes too
  if (!RCCHelloState::_horlink_vistype)
    RCCHelloState::_horlink_vistype = (VisitorType *)QueryRegistry(HLINK_VISITOR_NAME);
  if (!RCCHelloState::_hello_vistype)
    RCCHelloState::_hello_vistype = (VisitorType *)QueryRegistry(HELLO_VISITOR_NAME);
  if (!RCCHelloState::_fastuni_vistype)
    RCCHelloState::_fastuni_vistype = (VisitorType *)QueryRegistry(FAST_UNI_VISITOR_NAME);
  if (!RCCHelloState::_npstate_vistype)
    RCCHelloState::_npstate_vistype = (VisitorType *)QueryRegistry(NPSTATE_VISITOR_NAME);
}

void DeallocTheStates(void)
{
  if (RCCHelloState::_StateLinkDown) {
    delete RCCHelloState::_StateLinkDown;
    RCCHelloState::_StateLinkDown = 0;
  }
  if (RCCHelloState::_StateAttempt) {
    delete RCCHelloState::_StateAttempt;
    RCCHelloState::_StateAttempt = 0;
  }
  if (RCCHelloState::_State1WayInside) {
    delete RCCHelloState::_State1WayInside;
    RCCHelloState::_State1WayInside = 0;
  }
  if (RCCHelloState::_State2WayInside) {
    delete RCCHelloState::_State2WayInside;
    RCCHelloState::_State2WayInside = 0;
  }
}

RCCHelloState::RCCHelloState( const NodeID * nid, 
			      const NodeID * unid , int vpi, int vci, 
			      u_short nver, u_short over, int cref) :
  _vpi(vpi), _vci(vci), _cver(nver), _nver(nver),
  _over(over), _SVCUp(false), _SVCInitiated(false), _SetupReceived(false),
  _UplinkReceived(false), _NPToFullState(false), _LocPortID(0xffffffff), 
  _RemNodeID(0L), _RemPeerGroupID(0L), _RemPortID(0), _LocNodeID(0L),
  _HelloTimer(0L), _InactivityTimer(0L), _IntegrityTimer(0L), 
  _HLInactivityTimer(0L), _booting(true), _InitLGNSVCTimer(0L), 
  _RetryLGNSVCTimer(0L), _cref(cref), _Cause53(false), _UpNodeID(0L), 
  _CalledParty(true)
{
  DIAG(FSM_HELLO_RCC, DIAG_DEBUG, cout << "Instantiating SVCHello for " 
       << *nid << endl);

  AllocTheStates();
  _cs = _StateLinkDown;

  SetLocNodeID(nid);
  assert(_LocNodeID != 0);

  // This PeerGroup
  _LocPeerGroupID = _LocNodeID->GetPeerGroup();
  assert(_HLE = new ig_lgn_horizontal_link_ext());

  // build the UpNodeID 
  if (unid) {
    SetUpNodeID(unid);
    SetRemNodeID(unid);
    assert(_UpNodeID != 0);
    assert(_RemNodeID != 0);

    if (*_LocNodeID < *_UpNodeID)
      _CalledParty = true;
    else
      _CalledParty = false;
  }

  assert(_HelloTimer      = new RCCHelloTimer(this, RCCHelloInterval));
  assert(_InactivityTimer = 
	 new RCCInactivityTimer(this, 1.0*RCCHelloInterval*RCCInactivityFactor));

  if (_CalledParty)
    _IntegrityTimer = new RCCIntegrityTimer(this, RCCCalledIntegrityTime);
  else
    _IntegrityTimer = new RCCIntegrityTimer(this, RCCCallingIntegrityTime);
  assert(_IntegrityTimer != 0);

  assert(_HLInactivityTimer = new RCCInactivityTimer(this, RCCHLinkInactivityTime));
  assert(_InitLGNSVCTimer   = new RCCInitTimer(this, RCCInitLgnSVCTime));
  assert(_RetryLGNSVCTimer  = new RetryLGNSVCTimer(this, RCCRetryLgnSVCTime));
  _ref_count++;
}

RCCHelloState::~RCCHelloState() 
{
  assert(_LocNodeID != 0);
  delete _LocNodeID;
  assert(_LocPeerGroupID != 0);
  delete _LocPeerGroupID;
  assert(_HelloTimer != 0);
  delete _HelloTimer;
  assert(_InactivityTimer != 0);
  delete _InactivityTimer;
  assert(_IntegrityTimer != 0);
  delete _IntegrityTimer;
  assert(_HLInactivityTimer != 0);
  delete _HLInactivityTimer;
 
  if (_HLE)             delete _HLE;
  if (_RemNodeID)       delete _RemNodeID;
  if (_RemPeerGroupID)  delete _RemPeerGroupID;
  if (_UpNodeID)        delete _UpNodeID;

  // Clean up the memory used by the states
  if (--_ref_count == 0)
    DeallocTheStates();
}

void RCCHelloState::Dump(void) 
{  Dump(cout);  }

void RCCHelloState::Dump(ostream & os)
{
  DIAG(FSM_HELLO_RCC, DIAG_DEBUG, os <<
       "  NodeID:     " << *_LocNodeID << endl <<
       "  PGID:       " << *_LocPeerGroupID << endl <<
       "  UpNodeID in Uplinks: " << *_UpNodeID << endl <<
       "  Current State: " << _cs << endl <<
       "  State Link Down: " << _StateLinkDown << endl <<
       "  State Attempt: " << _StateAttempt << endl <<
       "  State 1WayIn: " << _State1WayInside << endl <<
       "  State 2WayIn: " << _State2WayInside << endl;
       if (_CalledParty)  os << "  CalledParty" << endl; 
       else               os << "  CallingParty" << endl;
       if (_SVCUp)        os << "  SVC is Up **" << endl;
       else               os << "  SVC is Down" << endl; 
       if (!_CalledParty && _SVCInitiated)
         os << "  Initiated SVC *" << endl;
       else if (!_CalledParty && !_SVCInitiated)
         os << "  SVC NotInitiated  " << endl;
       else if (_CalledParty && _SetupReceived)
         os << "  Setup Received *" << endl;
       else 
         os << "  Setup not Received" << endl;
       if (_UplinkReceived)
         os << "  An Uplink has been received *" << endl;
       else
         os << "  No Uplink is received " << endl;
       if ( _NPToFullState != false )
         os << "  NodePeerFSM is in FULL state ***" << endl;
       else
         os << "  NodePeerFSM is not in FULL state " << endl;
       os << "  VPI: " << _vpi << " VCI: " << _vci << endl <<
       "  SVCC PortID: " << _LocPortID << endl <<
       "  Current Version: " << _cver << endl;
       if (_RemNodeID) {
	 os << "  Remote NodeID in Hello: " ; 
	 os << *_RemNodeID << endl; 
       }
       os << "  Remote PGID in Hello: ";
       if (_RemPeerGroupID) 
         os << *_RemPeerGroupID << endl;
       else
         os << "  NULL" << endl;
       os << "  Remote SVC PortID: " << _RemPortID  << endl <<
       "  CREF for the call: " << hex << _cref << endl);
}

const InternalRCCHelloState * RCCHelloState::GetCS(void) const
{  return _cs;  }

void RCCHelloState::Interrupt(SimEvent * se)
{
  diag(FSM_HELLO_RCC, DIAG_DEBUG, "%s, My god, why am I being interrupted? \n",
       OwnerName());
}

State * RCCHelloState::Handle(Visitor * v)
{
  assert(_cs != 0);

  _cs->Handle(this, v);
  return this;
}

const NodeID * RCCHelloState::GetLocNodeID(void) const
{  return _LocNodeID;  }

const NodeID * RCCHelloState::GetRemNodeID(void) const
{  return _RemNodeID;  }

const NodeID * RCCHelloState::GetUpnodeID(void) const
{  return _UpNodeID;  }

void RCCHelloState::SetLocNodeID(const NodeID * n)
{
  if (_LocNodeID)
    delete _LocNodeID;
  _LocNodeID = (n ? n->copy() : 0);
}

void RCCHelloState::SetRemNodeID(const NodeID * n)
{
  if (_RemNodeID)
    delete _RemNodeID;
  _RemNodeID = (n ? n->copy() : 0);
}

void RCCHelloState::SetUpNodeID(const NodeID * n)
{
  if (_UpNodeID)
    delete _UpNodeID;
  _UpNodeID = (n ? n->copy() : 0);
}

void   RCCHelloState::SetCver(u_short cver) 
{  _cver = cver;  }

void RCCHelloState::SetNver(u_short nver) 
{  _nver = nver;  }

void RCCHelloState::SetOver(u_short over) 
{  _over = over;  }

void  RCCHelloState::SendHello(void)
{
  DIAG(FSM_HELLO_RCC, DIAG_DEBUG, cout <<
       OwnerName() << "::SendHello RemNodeID is " << *_UpNodeID << 
       " RemPort is " << _RemPortID << endl);

  // Get the Hellopkt ready
  HelloPkt * hp = new HelloPkt(GetLocNodeID(), GetRemNodeID(), 0xffffffff,0xffffffff, RCCHelloInterval);  
  assert(hp != 0);

  hp->SetLGNHorizontalLinkExt(GetLGNHLE()); 

  // Set the elements for routing the visitor
  HelloVisitor * v = new HelloVisitor(HelloVisitor::SVCHello, hp);
  assert(v != 0);

  v->SetLoggingOn();             // ???? BILAL TODO What about setting aggtok?

  v->SetSourceNID(_LocNodeID);
  v->SetDestNID(_UpNodeID);

  // This should get it through the DataForwarder
  v->SetInVP(_vpi);
  v->SetInVC(_vci);
  v->SetCREF(_cref);
  v->SetInPort(0);
  // AB 01/21/99
  const int lp = 0xffffffff;
  v->SetRPID(lp);
  v->SetPID(lp);
  PassVisitorToA(v);
}

RCCHelloState::RCCHelloTransitions RCCHelloState::GetEventType(Visitor * v)
{
  RCCHelloTransitions rval = Invalid;

  DIAG(FSM_HELLO_RCC, DIAG_DEBUG, cout <<
       OwnerName() << " " << *_LocNodeID << " received a " << 
       v->GetType() << " (" << v << ") " << endl;
       if (v->GetType().Is_A(_fastuni_vistype)) {
	 FastUNIVisitor * fuv = (FastUNIVisitor *)v;
	 if (fuv->GetSourceNID())
	   cout << "from " << *(fuv->GetSourceNID()) << endl;
	 if (fuv->GetDestNID())
	   cout << "to " << *(fuv->GetDestNID()) << endl;
       });

  // HelloVisitor
  if (v->GetType().Is_A(_hello_vistype)) {
    HelloVisitor * hv = (HelloVisitor *)v;
    // could get a Hello before the uplink PTSE and I exist because a
    // SETUP came in
    if (_UplinkReceived) {
      if (hv->GetOutVP() != _vpi || hv->GetOutVC()  != _vci) {
	diag(FSM_HELLO_RCC, DIAG_ERROR, 
	     "%s: Bad (vpi,vci) or SVCC not to this SVCC (%d,%d).\n", 
	     OwnerName(), _vpi, _vci);
	return Invalid;
      }
      /*
       * PortID in Hello must match 0xFFFFFFFF
       * PGID   in hello must match local PGID 
       * NodeID in Hello must match local NID (which was the UpNodeID
       * recieved in PTSE)
       */
      const HelloPkt * hp = hv->GetHelloPkt();
      
      if (hp->GetPortID() != 0xffffffff) {
	diag(FSM_HELLO_RCC, DIAG_DEBUG, 
	     "%s: HelloMisMatch port id is equal to %x.\n", 
	     OwnerName(), hp->GetPortID());
	rval = HelloMisMatch;
      } else if (!_LocPeerGroupID->equals(hp->GetPeerGroupID())) {
	DIAG(FSM_HELLO_RCC, DIAG_DEBUG, cout <<
	     OwnerName() << ": HelloMisMatch, PeerGroupIDs do not match." <<
	     "Mine: " << *_LocPeerGroupID << endl << "His:  ";
	     if (hp->GetPeerGroupID())
	       cout << *hp->GetPeerGroupID();
	     else
	       cout << "(Null)";
	     cout << endl);
	rval = HelloMisMatch;
      } else if (!_UpNodeID->equals(hp->GetNodeID())) {
	diag(FSM_HELLO_RCC, DIAG_DEBUG, 
	     "%s: HelloMisMatch: NodeID in hp different from UpNodeID in PTSE.\n"
	     , OwnerName());
	rval = HelloMisMatch;
      } else if (hp->GetRemoteNodeID() == 0) {
	// WayInside1 & WayInside2
	diag(FSM_HELLO_RCC, DIAG_DEBUG, "%s: OneWayInside.\n", OwnerName());
	rval = WayInside1;
      } else if (_LocNodeID->equals(hp->GetRemoteNodeID())) {
	diag(FSM_HELLO_RCC, DIAG_DEBUG, "%s: TwoWayInside.\n", OwnerName());
	CancelTimer(RCCIntegrityTIMER);
	rval = WayInside2;
      } else
	rval = HelloMisMatch;
    }
    return rval;
  }

  // FastUNI Visitor
  if (v->GetType().Is_A(_fastuni_vistype)) {
    FastUNIVisitor * fu = (FastUNIVisitor *)v;

    diag(FSM_HELLO_RCC, DIAG_DEBUG, "%s received a FastUNIVisitor::%s (%x).\n",
	 OwnerName(), fu->PrintMSGType(), v);

    switch (fu->GetMSGType()) {
      case FastUNIVisitor::FastUNISetup:
      case FastUNIVisitor::FastUNILHI:
        rval = SVCSetup; break;
      case FastUNIVisitor::FastUNIConnect: 
        rval = SVCConnect; break;
      case FastUNIVisitor::RemotePGLChanged:
        rval = RemotePGLChanged; break;
      case FastUNIVisitor::PGLChanged:
        rval = PGLChanged; break;
      case FastUNIVisitor::FastUNIRelease:
        rval = SVCRelease; break;
      case FastUNIVisitor::FastUNISetupFailure:
	rval = SVCFailure; break;
      case FastUNIVisitor::FastUNILHIRebinder:
	{
	  // Why bother doing this when you can use passthrough?
	  //	  FastUNIVisitor * fuv = new FastUNIVisitor(*fu);
	  //	  PassVisitorToB(fuv);
	  //	  rval = Invalid; 
	  rval = passthru;
	}
        break;
      default:
        rval = passthru; break;
    }
    return rval;
  }

  // NPStateVisitor
  if (v->GetType().Is_A(_npstate_vistype)) {
    diag(FSM_HELLO_RCC, DIAG_DEBUG, "%s received an NPStateVisitor (%x).\n", 
	 OwnerName(), v);
    
    NPStateVisitor *npv = (NPStateVisitor *)v;
    
    switch (npv->GetVT()) {
      case NPStateVisitor::FullState:
	_NPToFullState = true;
	// The buck stops here
	rval = KillVisitor;
        break;
      case NPStateVisitor::BadPTSEReq:
      case NPStateVisitor::DSMismatch:
        _NPToFullState = false;
        rval = BadNeighbor;
      default:
	rval = Invalid;
        break;
    }
    return rval;
  }

  // HorLinkVisitor (from LgnHello)
  if (v->GetType().Is_A(_horlink_vistype)) {
    HorLinkVisitor * hlv = (HorLinkVisitor *)v;

    if (hlv->GetVT() == HorLinkVisitor::LgnHello) {
      // Extract the AggTok, Local Port, Remote Port
      int AggTok = hlv->GetAgg(), 
	  LocalPort = hlv->GetLocalPort(), 
	  RemotePort = hlv->GetRemotePort();

      AddLgnHLinkExtInfo(AggTok, LocalPort, RemotePort);
    }
    // Kill it.
    return Invalid;
  }
  return passthru;
}

void RCCHelloState::do_Hp(Visitor * v, int code)
{
  HelloVisitor * hv = (HelloVisitor *)v;
  SVCCPortInfoVisitor * spf = 0;
  const HelloPkt * hp = 0;

  diag(FSM_HELLO_RCC, DIAG_DEBUG, "%s: Doing SVCCHello Protocol %d.\n", 
       OwnerName(), code);

  switch (code) {
    case 0:
      break;

    case 1:      
      SendHello();
      RegisterTimer(RCCHelloTIMER);
      break;

    case 2:
      RegisterTimer(RCCInactivityTIMER);

      assert(hv != 0);
      hp = hv->GetHelloPkt();

      _RemPortID      = hp->GetPortID();
      SetRemNodeID(hp->GetNodeID());
      _RemPeerGroupID = hp->GetPeerGroupID()->copy();
      // if((_->_cver = v->_nver) > __nver)
      // _cver = _nver;
      SendHello();

      // restart hello timer
      RegisterTimer(RCCHelloTIMER);
      break;

    case 3:
      do_Hp(v,2);
      // To the Lgn NodePeerState machine AddPort
      spf = new SVCCPortInfoVisitor(true, _vpi, _vci);  
      PassVisitorToB(spf);
      CancelTimer(RCCIntegrityTIMER);
      break;

    case 4:  // Called in OneWayInside when we reach TwoWayInside
      // restart the inactivity timer
      RegisterTimer(RCCInactivityTIMER);
      // notify all of the LgnHellos of the RemoteNodeID
      // HOW?!?!?!?!?!?!?!?!?!?!?
      // call the NetStatsCollector with type Hello_Up
      theNetStatsCollector().ReportNetEvent("Hello_Up",
					    OwnerName(),
					    _RemNodeID ? _RemNodeID->Print() : "Logical-TwoWayInside", 
					    _LocNodeID);
      // invoke the corresponding NP fsm with AddPort
      spf = new SVCCPortInfoVisitor(true, _vpi, _vci);  
      PassVisitorToB(spf);
      // cancel the integrity timer
      CancelTimer(RCCIntegrityTIMER);

      if ( _CalledParty ) {
	// Contact the DB and let it know that the SVC was setup properly
	const NodeID * rnid = GetRemNodeID();
	
	DBHelloInterface * dbface = (DBHelloInterface *)QueryInterface( "Database" );
	assert( dbface != 0 && dbface->good() );
	// make sure we have it for a bit .....
	dbface->Reference();
	if (dbface->good())
	  dbface->SVCCIsUp(rnid);
	// ........ give it back
	dbface->Unreference();
      }
      break;

    case 5:
    case 6:
    case 7:
    case 11:
    case 13:
    case 14:
    case 17:
    case 19:
    case 20:
      diag(FSM_HELLO_RCC, DIAG_FATAL, "Illegal case!\n");
      // exit(1);
      break;

    case 8:
      CancelTimer(RCCInactivityTIMER);
      ClearDS();
      SendHello();
      // restart hello timer
      RegisterTimer(RCCHelloTIMER);
      break;

    case 9:
      CancelTimer(RCCHelloTIMER);
      CancelTimer(RCCInactivityTIMER);
      ClearDS();
      break;

    case 10:
      RegisterTimer(RCCInactivityTIMER);
      SendHello();
      CancelTimer(RCCHelloTIMER);
      RegisterTimer(RCCHelloTIMER);
      // invoke the NP fsm with DropPort
      _NPToFullState = false;
      spf = new SVCCPortInfoVisitor(false, _vpi, _vci);  
      PassVisitorToB(spf);
      break;

    case 12:
      RegisterTimer(RCCInactivityTIMER);
      // Process the HLE IG if DB is in Full state
      if ( _NPToFullState != false ) {
	const ig_lgn_horizontal_link_ext * hle = 
	  hv->GetHelloPkt()->GetLGNHorizontalLinkExt();
	
	if ( ! hle ) {
          diag(FSM_HELLO_RCC, DIAG_FATAL, 
	       "%s: HelloVisitor (%x) does not include the mandatory HLE IG.\n",
	       OwnerName(), hv);
	}
	assert( hle != 0 );

	ParseLgnExtLinks( hle );
	CancelTimer( HLInactivityTIMER );
	RegisterTimer( HLInactivityTIMER );

	SendLgnHLinkExtRequest( );
      } 
      break;

    case 15:
      SendHello();
      RegisterTimer(RCCHelloTIMER);
      break;

    case 16:
      // Invoke the NP fsm with DropPort by sending it a PortDownVisitor.
      _NPToFullState = false;

      // call the NetStatsCollector with type Hello_Down
      theNetStatsCollector().ReportNetEvent("Hello_Down",
					    OwnerName(),
					    _RemNodeID ? _RemNodeID->Print() : "Logical-HelloMisMatch", 
					    _LocNodeID);

      spf = new SVCCPortInfoVisitor(false, _vpi, _vci);
      PassVisitorToB(spf);
      do_Hp(v,8);
      break;

    case 18:
      // Invoke the NP fsm with DropPort by sending it a PortDownVisitor.
      _NPToFullState = false; 

      // call the NetStatsCollector with type Hello_Down
      theNetStatsCollector().ReportNetEvent("Hello_Down",
					    OwnerName(),
					    _RemNodeID ? _RemNodeID->Print() : "Logical-HelloMisMatch", 
					    _LocNodeID);

      spf = new SVCCPortInfoVisitor(false, _vpi, _vci); 
      PassVisitorToB(spf);
      do_Hp(v,9);
      break;
  }
}

void RCCHelloState::StartSVCIntegrityTimer(void)
{
  if (_SVCUp || _SVCInitiated || _SetupReceived) {
    if (!_IntegrityTimer->IsRunning())
      RegisterTimer(RCCIntegrityTIMER);
  } else {
    if (_IntegrityTimer->IsRunning()) {
      CancelTimer(RCCIntegrityTIMER);
      DIAG(FSM_HELLO_RCC, DIAG_WARNING, 
	   cout << "In State: " << OwnerName() << 
	   "SVCInegrity timer on when SVC is not present " << endl);
    } 
  }
}

void RCCHelloState::StopSVCIntegrityTimer(void)
{
  // ensure that you change the state to LinkDown
  // before you reestablish any svc here.
  if (_IntegrityTimer->IsRunning())
    CancelTimer(RCCIntegrityTIMER);
}

void RCCHelloState::SetVPI(int vpi)
{ _vpi = vpi; }

void RCCHelloState::SetVCI(int vci)
{ _vci = vci; }

void RCCHelloState::SendLinkUp(void)
{
  HorLinkVisitor * hlv = new HorLinkVisitor(HorLinkVisitor::HLinkUp,
					    _LocNodeID, _RemNodeID, 0, -1, -1,
					    _LocPortID, _RemPortID, true);
  PassVisitorToB(hlv);
}

void RCCHelloState::SendLinkDown(void)
{
  HorLinkVisitor * hlv = new HorLinkVisitor(HorLinkVisitor::HLinkDown,
					    _LocNodeID, _RemNodeID, 0, -1, -1,
					    _LocPortID, _RemPortID, true);
  PassVisitorToB(hlv);
}

const ig_lgn_horizontal_link_ext * RCCHelloState::GetLGNHLE(void) const
{ return _HLE; }

void RCCHelloState::SetSVCInitiated(bool val)
{ _SVCInitiated = val; }

void RCCHelloState::SetSetupReceived(bool val)
{ 
  _SetupReceived = val; 
}

void RCCHelloState::SetCREF(int cref)
{ _cref = cref; }

void RCCHelloState::SetPort(u_int port)
{ _port = port; }

void RCCHelloState::SetSVCUp(bool val)
{ _SVCUp = val; }

void RCCHelloState::SetCause53(bool val)
{ _Cause53 = val; }

void RCCHelloState::ParseLgnExtLinks( const ig_lgn_horizontal_link_ext * hle )
{
  // need to check the HLE IG for all expected Aggr tokes.
  // if present, send a HorLinkVisitor of type LgnHello,
  // otherwise HelloMismatch type to force the HL fsm to attempt state.
  HorLinkVisitor * hlv = 0;

  list<ig_lgn_horizontal_link_ext::HLinkCont *> lhle_list = _HLE->GetLinks();
  list<ig_lgn_horizontal_link_ext::HLinkCont *> rhle_list = hle->GetLinks();

  ig_lgn_horizontal_link_ext::HLinkCont * rhptr = 0; 
          
  // Iterate through the containers within OUR LGN HLE
  list_item lhle_li;
  forall_items(lhle_li, lhle_list) {
    ig_lgn_horizontal_link_ext::HLinkCont * lhptr = lhle_list.inf(lhle_li);
    assert( lhptr != 0 );
    // locate the rhptr with the same aggregation token
    list_item rhle_li = rhle_list.search(lhptr);

    if (rhle_li != 0) {
      rhptr = rhle_list.inf(rhle_li);
      // Our remote is his Local Port
      lhptr->_remote_lgn_port = rhptr->_local_lgn_port; 
      // AB 01/21/99: his remote is our local and -1 means ZERO for the hello FSMs
      if (rhptr->_remote_lgn_port == -1)
	rhptr->_remote_lgn_port = 0;
      // Inform the appropriate Lgn Hello FSM that we received its information
      hlv = new HorLinkVisitor(HorLinkVisitor::LgnHello,
			       0, 0, 0, -1, 
			       // We share the agg token
			       rhptr->_aggregation_token,
			       // His remote should == our local
			       rhptr->_remote_lgn_port,
			       // His local should == our remote
			       rhptr->_local_lgn_port);
      PassVisitorToB(hlv);
    } else {
      // Inform mismatch to appropriate Lgn Hello as it is not announced
      // by the remote node.
      hlv = new HorLinkVisitor(HorLinkVisitor::HelloMismatch, 0, 0, 0, -1,
                               lhptr->_aggregation_token);
      PassVisitorToB(hlv);
    }
  }
}

void RCCHelloState::SendInducedUpLink(FastUNIVisitor * fu)
{
  if ( fu->GetMSGType() == FastUNIVisitor::FastUNISetup )
    if (!_UplinkReceived)
      _UplinkReceived = true; 

  // Either this is the FastUNILHI OR (FastUNISetup AND we are Calling Party)
  if (( fu->GetMSGType() == FastUNIVisitor::FastUNILHI ) ||
      ( fu->GetMSGType() == FastUNIVisitor::FastUNISetup &&
	! _CalledParty )) {

    ig_lgn_horizontal_link_ext:: HLinkCont * HCont
      = _HLE->FindHLinkCont( fu->GetAgg() );

    if ( HCont == 0 )
      _HLE->AddHLink(fu->GetAgg(), -1, 0L);
    else {
      int HlCount = 0;
      HlCount = HCont->GetUplinkCount();
      HlCount++;
      HCont->SetUplinkCount(HlCount);
    }
    // Send a HorLink Visitor to the AggToken Mux
    const NodeID * loc = GetLocNodeID(), 
                 * rem = GetUpnodeID(), 
                 * bor = fu->GetBorder();
    int bp = fu->GetBorderPort(), agg = fu->GetAgg();
    HorLinkVisitor * hlv = new HorLinkVisitor(HorLinkVisitor::AddInducedLink,
					      loc, rem, bor, bp, agg,
					      -1);
    DelayedSendTimer * dst = new DelayedSendTimer(this, hlv, Visitor::B_SIDE, 
						  0.0);
    Register(dst);
  }
}

void RCCHelloState::SendDropInducedUpLink(FastUNIVisitor *fu)
{
  HorLinkVisitor * hlv = 0; 
  ig_lgn_horizontal_link_ext:: HLinkCont * HCont 
                       = _HLE->FindHLinkCont(fu->GetAgg());

  const NodeID * bor = fu->GetBorder();;
  int bp = fu->GetBorderPort(), agg = fu->GetAgg();

  if (HCont) {
    int UplinkCount = HCont->GetUplinkCount();
    if (--UplinkCount == 0)  {
      _HLE->DelHLink(fu->GetAgg());
      if (!_HLE->GetHLinkCount()) {
        if (_UplinkReceived)
          _UplinkReceived = false;
      }
      hlv = new HorLinkVisitor(HorLinkVisitor::DropLastInducedLink,
			       0, 0, bor, bp, agg);
    } else {
      HCont->SetUplinkCount(UplinkCount);
      hlv = new HorLinkVisitor(HorLinkVisitor::DropInducedLink,
			       0, 0, bor, bp, agg);
    }
    PassVisitorToB(hlv);
  } else {
    diag(FSM_HELLO_RCC, DIAG_ERROR, "%s  No Uplink AggrToken %d before \n",
                          OwnerName(), fu->GetAgg());
  }
}

void RCCHelloState::BroadCastLinkDownToHLinks(void)
{
  HorLinkVisitor * hlv = 0;
  hlv = new HorLinkVisitor(HorLinkVisitor::DropALLLinks, (int)0, (int)0, 
			   (int)0, true);
  hlv->SetToBroadCast(true);
  PassVisitorToB(hlv);
}

void RCCHelloState::BroadCastMismatchToHLinks(void)
{
  HorLinkVisitor * hlv = 0;
  hlv = new HorLinkVisitor(HorLinkVisitor::HelloMismatch, (int)0, (int)0, 
			   (int)0, true);
  hlv->SetToBroadCast(true);
  PassVisitorToB(hlv);
}

void RCCHelloState::BroadCastBadNeighborToHLinks(void)
{
  HorLinkVisitor * hlv = 0;
  hlv = new HorLinkVisitor(HorLinkVisitor::BadNeighbor, (int)0, (int)0, 
			   (int)0, true);
  hlv->SetToBroadCast(true);
  PassVisitorToB(hlv);
}

void RCCHelloState::SendSetupRequest(void)
{
  DBHelloInterface * dbi = (DBHelloInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  dbi->ReinitiateSVC( _LocNodeID, _UpNodeID );
  dbi->Unreference();
}

void RCCHelloState::SendReleaseRequest(void)
{
  FastUNIVisitor * fu = new FastUNIVisitor(_LocNodeID, _UpNodeID,
					   _port, _vci, -1, _cref,
                                           FastUNIVisitor::FastUNIRelease);
  fu->SetCREF(_cref);
  fu->SetInVP(_vpi);
  fu->SetInVC(_vci);
  fu->SetInPort(_port);

  if (_Cause53) { 
    ie_cause * cause = 
      new ie_cause(ie_cause::peer_group_leader_changed, 
		   ie_cause::local_private_network);
    fu->SetCause(cause);
  } 
  PassVisitorToB(fu);
}

void RCCHelloState::RegisterTimer(RCCHelloState::RCCTimers type)
{
  switch (type) {
    case RCCInactivityTIMER:
      assert(_InactivityTimer);
      Register(_InactivityTimer);
      break;
    case RCCHelloTIMER:
      assert(_HelloTimer);
      Register(_HelloTimer);
      break;
    case RCCIntegrityTIMER:
      assert(_IntegrityTimer);
      Register(_IntegrityTimer);
      break;
    case HLInactivityTIMER:
      assert(_HLInactivityTimer);
      Register(_HLInactivityTimer);
      break;
    case InitLGNSVCTIMER:
      assert(_InitLGNSVCTimer);
      Register(_InitLGNSVCTimer);
      break;
    case RetryLGNSVCTIMER:
      assert(_RetryLGNSVCTimer);
      Register(_RetryLGNSVCTimer);
      break;
    default:
      diag(FSM_HELLO_RCC, DIAG_ERROR, "%s, Illegal Timer type given \n", 
	   OwnerName()); 
  }
}

void RCCHelloState::CancelTimer(RCCHelloState::RCCTimers type)
{
  switch (type) {
    case RCCInactivityTIMER:
      assert(_InactivityTimer);
      Cancel(_InactivityTimer);
      break;
    case RCCHelloTIMER:
      assert(_HelloTimer);
      Cancel(_HelloTimer);
      break;
    case RCCIntegrityTIMER:
      assert(_IntegrityTimer);
      Cancel(_IntegrityTimer);
      break;
    case HLInactivityTIMER:
      assert(_HLInactivityTimer);
      Cancel(_HLInactivityTimer);
      break;
    case InitLGNSVCTIMER:
      assert(_InitLGNSVCTimer);
      Cancel(_InitLGNSVCTimer);
      break;
    case RetryLGNSVCTIMER:
      assert(_RetryLGNSVCTimer);
      Cancel(_RetryLGNSVCTimer);
      break;
    default:
      diag(FSM_HELLO_RCC, DIAG_ERROR, "%s, Illegal Timer type given \n", 
	   OwnerName());
  }
}

void RCCHelloState::ChangeState(RCCHelloState::StateID to)
{
  switch (to) {
    case LinkDownState:
      _cs = _StateLinkDown;
      StopSVCIntegrityTimer();
      break;
    case AttemptState:
      _cs = _StateAttempt;
      StartSVCIntegrityTimer();
      break;
    case Way1InsideState:
      _cs = _State1WayInside;
      StartSVCIntegrityTimer();
      break;
    case Way2InsideState:
      _cs = _State2WayInside;
      StopSVCIntegrityTimer();
      break;
    default:
      diag(FSM_HELLO_RCC, DIAG_ERROR, "Illegal State.\n");
      break;
  }
}

void RCCHelloState::ClearDS(void)
{
  // Look for other variables
  _NPToFullState = false;
  ClearRemotePortInHLE();
  delete _RemNodeID;
  _RemNodeID = 0;
  delete _RemPeerGroupID;
  _RemPeerGroupID = 0;
  _RemPortID = 0;
}

void RCCHelloState::ClearRemotePortInHLE(void)
{
  const list<ig_lgn_horizontal_link_ext::HLinkCont *> * links = 
    _HLE->ShareLinks();

  list_item li; 
  forall_items(li, *links) {
    ig_lgn_horizontal_link_ext::HLinkCont * hCont = links->inf(li);  
    hCont->_remote_lgn_port = 0L;
  }
}

void RCCHelloState::AddLgnHLinkExtInfo(int agg, int lport, int rport)
{
  // AB 01/21/99 : again for Hello FSMs -1 means 0 that is Unknown
  if (rport == -1)
    rport = 0;
  _HLE->AddHLink(agg, lport, rport);
}

void RCCHelloState::SendLgnHLinkExtRequest(void)
{
  HorLinkVisitor * hlv = new HorLinkVisitor(HorLinkVisitor::LgnHello,
					    // Agg Token is -1 here, so it should be broadcast
					    0, 0, 0, -1, -1, -1, -1,
					    true);
  // When everything is -1 it signifies a request for information
  PassVisitorToB(hlv);
}

// -------------------------- Internal SVC Hello State ------------------------
InternalRCCHelloState::InternalRCCHelloState()  {  }

InternalRCCHelloState::~InternalRCCHelloState() { }

void InternalRCCHelloState::Handle(RCCHelloState * s, Visitor * v)
{
  switch (s->GetEventType(v)) {
    case RCCHelloState::WayInside1:
      RcvWayInside1(s,v);
      v->Suicide();
      break;
    case RCCHelloState::WayInside2:
      RcvWayInside2(s,v);
      v->Suicide();
      break;
    case RCCHelloState::HelloMisMatch:
      RcvHelloMisMatch(s,v);
      v->Suicide();
      break;  
    case RCCHelloState::SVCSetup:
      {
	int send = RcvSVCSetup( s, v );
	RcvNewUpLink( s, v );
	if ( send != fsm_error )
	  PassThru( s, v );
	else
	  v->Suicide();
      }
      //      if (RcvSVCSetup(s,v)) {
      //	AppendEventToLog("SVCSetup");
      //	RcvNewUpLink(s,v);
      //	PassThru(s, v);
      //      } else 
      //	v->Suicide();
      break;
    case RCCHelloState::SVCConnect: 
      RcvSVCConnect(s,v);
      PassThru(s, v);
      break;
    case RCCHelloState::SVCRelease:
      if (IsCalledParty(s))
        RcvCalledSVCRelease(s, v);
      else
        RcvCallingSVCRelease(s, v);
      break;
    case RCCHelloState::SVCFailure:
      RcvSVCFailure(s, v);
      PassThru(s, v);
      break;
    case RCCHelloState::RemotePGLChanged:
      RcvRemotePGLChange(s,v);
      v->Suicide();
      break;
    case RCCHelloState::PGLChanged:
      RcvPGLChange(s, v);
      v->Suicide();
      break;
    case RCCHelloState::AnUpLinkDown:
      /* This isn't around anymore,
       * try SVCRelease
       */
      RcvAnUpLinkDown(s,v);
      v->Suicide();
      break;
    case RCCHelloState::BadNeighbor:
      RcvBadNeighbor(s,v);
      v->Suicide();
      break;
    case RCCHelloState::passthru:
      PassThru(s, v);
      break;
    case RCCHelloState::Invalid:
    case RCCHelloState::KillVisitor:
      v->Suicide();
      break;
    default:
      diag(FSM_HELLO_RCC, DIAG_DEBUG, "Unknown return of event");
      PassThru(s, v);
      break;
  }
}

void InternalRCCHelloState::do_Hp(RCCHelloState *s, Visitor *v, int code)
{
  s->do_Hp(v, code);
}

bool InternalRCCHelloState::IsSVCUp(RCCHelloState *s)
{
  return s->_SVCUp; 
}

bool InternalRCCHelloState::IsSVCInitiated(RCCHelloState *s)
{
  return s->_SVCInitiated;
}

bool InternalRCCHelloState::IsSetupReceived(RCCHelloState *s)
{
  return s->_SetupReceived;
}

bool InternalRCCHelloState::IsCalledParty(RCCHelloState *s)
{
  return s->_CalledParty;
}

void InternalRCCHelloState::SetCallingParty(RCCHelloState * s)
{
  s->_CalledParty = false;
}

bool InternalRCCHelloState::IsUplinksReceived(RCCHelloState *s)
{
  return s->_UplinkReceived;
}

const NodeID  * InternalRCCHelloState::GetLocNodeID(RCCHelloState *s) const
{
  return s->_LocNodeID;
}

const NodeID  * InternalRCCHelloState::GetRemNodeID(RCCHelloState *s) const
{
  return s->_RemNodeID;
}

const NodeID  * InternalRCCHelloState::GetUpNodeID(RCCHelloState *s) const
{
  return s->_UpNodeID;
}

void InternalRCCHelloState::SetSVCInitiated(RCCHelloState *s, bool val)
{
  s->SetSVCInitiated(val); 
}
 
void InternalRCCHelloState::SetSetupReceived(RCCHelloState *s, bool val)
{
  s->SetSetupReceived(val);
}

void InternalRCCHelloState::SetCREF(RCCHelloState *s, int cref)
{
  s->SetCREF(cref);
}

void InternalRCCHelloState::SetPort(RCCHelloState *s, u_int port)
{ s->SetPort(port); }

void InternalRCCHelloState::SetSVCUp(RCCHelloState *s, bool val)
{
 s->SetSVCUp(val);
}

void InternalRCCHelloState::SendInducedUpLink(RCCHelloState *s, 
					      FastUNIVisitor *fu)
{
  s->SendInducedUpLink(fu);
}

void InternalRCCHelloState::SendDropInducedUpLink(RCCHelloState *s,
                                                  FastUNIVisitor *fu)
{
  s->SendDropInducedUpLink(fu); 
}

void InternalRCCHelloState::StartSVCIntegrityTimer(RCCHelloState *s)
{
  s->StartSVCIntegrityTimer();
}

const char * InternalRCCHelloState::OwnerName(RCCHelloState *s)
{
  return s->OwnerName();
}

void InternalRCCHelloState::SetVPVC(RCCHelloState *s, int vpi, int vci)
{
  s->SetVPI(vpi);
  s->SetVCI(vci);
}

void InternalRCCHelloState::SetCause53(RCCHelloState *s, bool val)
{
  s->SetCause53(val);
}

bool InternalRCCHelloState::GetCause53(RCCHelloState *s)
{
  return s->_Cause53;
}

int InternalRCCHelloState::RcvLinkUp(RCCHelloState *s, Visitor *v)
{
  DIAG(FSM_HELLO_RCC, DIAG_WARNING, cout << 
       OwnerName(s)  << "A LinkUp event in state " << CurrentStateName() 
       << endl;);
  return fsm_error;
} 

int InternalRCCHelloState::RcvWayInside1(RCCHelloState *s, Visitor *v)
{
  DIAG(FSM_HELLO_RCC, DIAG_WARNING, 
       cout << "Received 1wayInside in " << CurrentStateName() << endl;);
  return fsm_error;
}

int InternalRCCHelloState::RcvWayInside2(RCCHelloState *s, Visitor *v)
{
  DIAG(FSM_HELLO_RCC, DIAG_WARNING, 
       cout << "Received 2wayInside in " << CurrentStateName() << endl;);
  return fsm_error;
}

int InternalRCCHelloState::RcvHelloMisMatch(RCCHelloState *s, Visitor *v)
{
  return fsm_error;
}

int InternalRCCHelloState::RcvLinkDown(RCCHelloState *s, Visitor *v)
{
  DIAG(FSM_HELLO_RCC, DIAG_WARNING,
       cout << "Received LinkDown in " << CurrentStateName() << endl;);
  return fsm_error;
}

int InternalRCCHelloState::RcvSVCFailure(RCCHelloState * s, Visitor * v)
{
  FastUNIVisitor * fu = (FastUNIVisitor *)v;

  if (IsSVCInitiated(s) && 
      (GetRemNodeID(s) && GetRemNodeID(s)->equals(fu->GetDestNID()))) {
    SetSVCInitiated(s, false);
  }
  return fsm_ok;
}

// If this is the first time return 1, otherwise return 0
int InternalRCCHelloState::RcvSVCSetup(RCCHelloState * s, Visitor * v)
{
  FastUNIVisitor * fu = (FastUNIVisitor *)v;

  // Check if we have already initiated the SVCC
  if (!IsSVCInitiated(s) && 
      fu->GetMSGType() == FastUNIVisitor::FastUNISetup ) {
    // We have not tried to setup the SVCC yet, are we the called party?
    if (IsCalledParty(s)) {
      // We are the called party
      if (GetLocNodeID(s)->equals(fu->GetDestNID())) {
	SetPort(s, fu->GetInPort());
	SetCREF(s, fu->GetCREF());
	SetVPVC(s, fu->GetInVP(), fu->GetInVC());
        SetSetupReceived(s, true);
        StartSVCIntegrityTimer(s);
      } else {
        DIAG(FSM_HELLO_RCC, DIAG_ERROR, cout
	     <<  OwnerName(s) << "Wrong Setup received with CREF: " 
	     << fu->GetCREF() << " in state " << CurrentStateName() 
	     << endl);
      } 
    } else {
      // We initiated the SVCC setup
      if (GetLocNodeID(s)->equals(fu->GetSourceNID())) {
	SetPort(s, fu->GetOutPort());
	SetSVCInitiated(s, true);
	SetCREF(s, fu->GetCREF()); 
	SendInducedUpLink(s, fu);
	StartSVCIntegrityTimer(s);
	SetCallingParty(s);
      } else {
        DIAG(FSM_HELLO_RCC, DIAG_ERROR, cout
	     <<  OwnerName(s) << " Wrong Setup received with CREF: "
	     << fu->GetCREF() << endl << "in state" << CurrentStateName() 
	     << endl);
      }
    }
  } else 
    return fsm_error;
  return fsm_ok;
}

int InternalRCCHelloState::RcvCallingSVCRelease(RCCHelloState *s, Visitor * v)
{
   FastUNIVisitor * fu = (FastUNIVisitor *)v;
   if (!IsSVCUp(s)) {
     if (fu->GetSourceNID()->equals(GetLocNodeID(s))) {
       if (IsSVCInitiated(s)) {
         ie_cause * cause = 0;
         cause = fu->GetCause();
         if (cause && (cause->get_cause_value() == 53)) {
	   // case 1: We sent Setup to remote and before SVCUp we
	   //         had Integritytimer Expire, We send release
	   //         back fu going to remote
	   // case 2: If cause = 53, we send setup,& then our node
	   //         is not pgl. Fu going to remote
	   SetSVCInitiated(s, 0L);
	   RegisterTimer(s, RCCHelloState::RetryLGNSVCTIMER);
         }
         PassThru(s,v);
       } else {
         // Case 3 : We Sent Request for Setup and Acac/Logos Sent Release
         //            to us. visitor is to us so is killed

         RegisterTimer(s, RCCHelloState::RetryLGNSVCTIMER);
         v->Suicide();
       }
     } else {
       SetSVCInitiated(s, 0L);

       ie_cause * cause = 0;
       cause = fu->GetCause();

       if (cause && (cause->get_cause_value() == 53)) {
	 // Case 4:  Remote End switch rejected our setuprequest as PGL changed
	 //            there.
         s->SetCause53(true);
       } else {
	 // case 5: A middle switch or remote end switch rejected our setup
	 //           request for some constraints other than 53.
         RegisterTimer(s, RCCHelloState::RetryLGNSVCTIMER);
       }
       RcvLinkDown(s, v);
       PassThru(s, v);

       if (GetCause53(s)) {
         diag(FSM_HELLO_RCC, DIAG_DEBUG, 
	      "%s, SVCC Release due to Remote PGL Change \n", 
	      OwnerName(s));
         HandleCause53(s, false);
       }
     }
   } else {
     if (fu->GetSourceNID()->equals(GetLocNodeID(s))) {
       // Case 6: We send release when SVCIntegrity timer
       //         expires or HelloMismatch. We need to 
       //         send SVC request Again

       // Case 7: When Our Node Is not pgl, need to kill fsm.
       SetSVCUp(s, 0L);
       SetSVCInitiated(s, 0L);
       PassThru(s, v);
     } else {
       ie_cause * cause = 0;
       cause = fu->GetCause();
       
       if (cause && (cause->get_cause_value() == 53)) {
	 // case 8: Remote Node sent release as it is no more
	 //          pgl
	 s->SetCause53(true);
       }

       SetSVCUp(s, 0L);
       SetSVCInitiated(s, 0L);

       RcvLinkDown(s, v);
       PassThru(s, v);

       if (!GetCause53(s)) {
         // Case 9: Remote Node sent fsm for some reason.
         //        Send Setup request again 
         SendSetupRequest(s);
       } else {
	 diag(FSM_HELLO_RCC, DIAG_DEBUG, 
	      "%s, SVCC Release due to Remote PGL Change \n",
	      OwnerName(s));
	 HandleCause53(s, false);
       }
     }
   }
   return fsm_ok;
}
          
int InternalRCCHelloState::RcvCalledSVCRelease(RCCHelloState *s, Visitor * v)
{
  FastUNIVisitor *fu = (FastUNIVisitor *)v;

  if (IsSVCUp(s)) {
    if (fu->GetSourceNID()->equals(GetLocNodeID(s))) {
      // Case 6: We send release when SVCIntegrity timer
      //         expires or HelloMismatch. We need to 
      //         send SVC request Again

      // Case 7: When Our Node Is not pgl, need to kill fsm.
      SetSVCUp(s, 0L);
      SetSetupReceived(s, 0L);
      PassThru(s, v);
    } else {
      ie_cause * cause = 0;
      cause = fu->GetCause();

      if (cause && (cause->get_cause_value() == 53)) {
        // case 8: Remote Node sent release as it is no more
        //          pgl
        s->SetCause53(true);
      }
      SetSVCUp(s, 0L);
      SetSetupReceived(s, 0L);

      RcvLinkDown(s, v);
      PassThru(s, v);

      if (GetCause53(s)) {
	diag(FSM_HELLO_RCC, DIAG_DEBUG, 
	     "%s, Killing the LogicalSVCexp cluster",
	     OwnerName(s)); 
        HandleCause53(s, false);
      } 
    }
  } else {
    diag(FSM_HELLO_RCC, DIAG_ERROR, 
	 "%s, Release in Down state in Called Party \n", 
	 s->OwnerName()); 
  }
  return fsm_ok;
}

int InternalRCCHelloState::RcvSVCConnect(RCCHelloState *s, Visitor *v)
{
  FastUNIVisitor * fu = (FastUNIVisitor *)v;

  if (IsSVCUp(s)) {
    diag(FSM_HELLO_RCC, DIAG_WARNING, "%s Connect Again when SVC is up \n", 
	 OwnerName(s));
    return fsm_error;
  }

  // If the source NodeID of the Connect is us, we are the called party
  if (GetLocNodeID(s)->equals(fu->GetSourceNID())) { 
    if (IsCalledParty(s)) {
      SetSVCUp(s, true);
      SetCREF(s, fu->GetCREF());
      SetVPVC(s, fu->GetInVP(), fu->GetInVC()); 
      RcvLinkUp(s, v);
      s->SendLinkUp();
    } else {
      DIAG(FSM_HELLO_RCC, DIAG_WARNING, cout << 
	   "Calling Party sending SVCconnect in state " << 
	   CurrentStateName() << endl);
    }
  } // else we are the calling party
  else if (!IsCalledParty(s)) {
    SetSVCUp(s, true);
    SetVPVC(s, fu->GetOutVP(), fu->GetOutVC());
    RcvLinkUp(s, v);
    // We setup the SVCC, so we know the Remote Node (who coincidentally is the Source of the Connect).
    fu->SetSourceNID(s->GetRemNodeID());
    s->SendLinkUp();
  } else {
    DIAG(FSM_HELLO_RCC, DIAG_WARNING, cout << 
	 OwnerName(s) <<" Called Party receiving SVCconnect " << endl <<
	 " in state " << CurrentStateName() << endl);
  }
  return fsm_ok;
}

int InternalRCCHelloState::RcvNewUpLink(RCCHelloState *s, Visitor *v)
{
  FastUNIVisitor * fu = (FastUNIVisitor *)v;

  if (!IsCalledParty(s)) {
    if (!IsSVCInitiated(s))  
      RegisterTimer(s, RCCHelloState::InitLGNSVCTIMER);
  }
  SendInducedUpLink(s, fu);
  return fsm_ok;
}

int  InternalRCCHelloState::RcvRemotePGLChange(RCCHelloState *s, Visitor *v)
{
  FastUNIVisitor *fu = (FastUNIVisitor *)v;
  SendDropInducedUpLink(s, fu);
  if (GetCause53(s))
    HandleCause53(s, false);
  return fsm_ok;
}

int InternalRCCHelloState::RcvPGLChange(RCCHelloState *s,  Visitor *v)
{
  SetCause53(s, true);
  RcvLinkDown(s, 0);

  if (IsCalledParty(s)) {
    if (IsSetupReceived(s)) {
      SendReleaseRequest(s);
    }
  } else {
    if (IsSVCInitiated(s)) {
      SendReleaseRequest(s);
    }
  }
  HandleCause53(s, true);

  return fsm_ok;
}


int InternalRCCHelloState::RcvAnUpLinkDown(RCCHelloState *s, Visitor *v)
{    
  FastUNIVisitor *fu = (FastUNIVisitor *)v;
  SendDropInducedUpLink(s, fu);
  if (GetCause53(s))
    HandleCause53(s, false);
  return fsm_ok;
}

int InternalRCCHelloState::RcvBadNeighbor(RCCHelloState *s, Visitor *v)
{
  return fsm_error;
}

void InternalRCCHelloState::RegisterTimer(RCCHelloState * s, 
					  RCCHelloState::RCCTimers type)
{
  s->RegisterTimer(type);
}

int InternalRCCHelloState::ExpHelloTimer(RCCHelloState * s)
{
  // attempt, 1way, 2way
  s->do_Hp(0, 15);
  return fsm_ok;
}

int InternalRCCHelloState::ExpInactivityTimer(RCCHelloState *s)
{
  DIAG(FSM_HELLO_RCC, DIAG_WARNING, cout << 
       "Inactivity timer expired in " << CurrentStateName() << endl);
  return fsm_ok;
}

int InternalRCCHelloState::ExpIntegrityTimer(RCCHelloState *s)
{
  if (IsCalledParty(s)) { 
    if (IsSetupReceived(s)) {
      RcvLinkDown(s, 0);
      SendReleaseRequest(s); 
    } else
      diag(FSM_HELLO_RCC, DIAG_WARNING, 
           "%s, Integrity Timer Expired when SVC is not up \n", OwnerName(s)); 
  } else {
    if (IsSVCInitiated(s)) {
      RcvLinkDown(s, 0);
      SendReleaseRequest(s); 
      SendSetupRequest(s);
    } else 
      diag(FSM_HELLO_RCC, DIAG_WARNING, 
	   "%s Integrity timer expired when we SVC not initiated\n", 
	   OwnerName(s)); 
  } 
  return fsm_ok;
}

int InternalRCCHelloState::ExpInitLGNSVCTimer(RCCHelloState *s)
{
  static int i = 0;

  if (s->_UplinkReceived && !i) 
    SendSetupRequest(s);
  i++;
  return fsm_ok;
} 

int InternalRCCHelloState::ExpRetryLGNSVCTimer(RCCHelloState *s)
{
  static int i = 0;

  if (s->_UplinkReceived && !i)
    SendSetupRequest(s);
  i++;
  return fsm_ok;
}

void InternalRCCHelloState::ChangeState(RCCHelloState *s, 
					RCCHelloState::StateID type)
{
  s->ChangeState(type);
}

void InternalRCCHelloState::PassThru(RCCHelloState *s, Visitor * v)
{
  s->PassThru(v);
}
    
void InternalRCCHelloState::ClearDS(RCCHelloState *s)
{
  s->ClearDS();
}

void InternalRCCHelloState::SendSetupRequest(RCCHelloState *s)
{
  s->SendSetupRequest();
}

void InternalRCCHelloState::SendReleaseRequest(RCCHelloState *s)
{
  s->SendReleaseRequest();
}

void InternalRCCHelloState::BroadCastLinkDownToHLinks(RCCHelloState *s)
{
  s->BroadCastLinkDownToHLinks();
}

void InternalRCCHelloState::BroadCastMismatchToHLinks(RCCHelloState *s)
{
  s->BroadCastMismatchToHLinks();
}

void InternalRCCHelloState::BroadCastBadNeighborToHLinks(RCCHelloState *s)
{
  s->BroadCastBadNeighborToHLinks();
}

void InternalRCCHelloState::HandleCause53(RCCHelloState *s, bool LocalPGLDown)
{
  if ((!LocalPGLDown) &&
      (IsUplinksReceived(s) || !GetCause53(s)))
    return;
  
  diag(FSM_HELLO_RCC, DIAG_WARNING, 
       "Taking Down the Cluster Having the Conduit, %s\n",
       OwnerName(s)); 

  ClusterKillerVisitor * KillerVisitor =  new ClusterKillerVisitor();
  PassVisitorToA(s, KillerVisitor); 
}

void InternalRCCHelloState::PassVisitorToA(RCCHelloState *s, Visitor * v)
{
  s->PassVisitorToA(v); 
}

//------------- SVCLinkDownState-----------------------------------------
SVCStateLinkDown::SVCStateLinkDown() { }

SVCStateLinkDown::~SVCStateLinkDown() { }

int SVCStateLinkDown::RcvLinkUp(RCCHelloState *s, Visitor *v)
{
  ChangeState(s, RCCHelloState::AttemptState);
  if (!IsCalledParty(s)) {
    do_Hp(s, v, 1);
  }
  return fsm_ok;
}

int SVCStateLinkDown::RcvLinkDown(RCCHelloState *s, Visitor *v)
{
  if (GetCause53(s))
    BroadCastBadNeighborToHLinks(s);
  return fsm_ok;
}

int SVCStateLinkDown::ExpHelloTimer(RCCHelloState *s)
{
  DIAG(FSM_HELLO_RCC, DIAG_WARNING, cout << 
       "Hello Timer Expired in " << CurrentStateName() << endl);
  return fsm_error;
}
   

//------------------------SVCAttemptState------------------------------------
SVCStateAttempt::SVCStateAttempt() { }

SVCStateAttempt::~SVCStateAttempt() { }

int SVCStateAttempt::RcvWayInside1(RCCHelloState *s, Visitor *v)
{
  ChangeState(s, RCCHelloState::Way1InsideState);
  do_Hp(s, v, 2);
  return fsm_ok;
}

int SVCStateAttempt::RcvWayInside2(RCCHelloState *s, Visitor *v)
{
  ChangeState(s, RCCHelloState::Way2InsideState);
  do_Hp(s, v, 3);
  return fsm_ok;
}

int SVCStateAttempt::RcvHelloMisMatch(RCCHelloState *s, Visitor *v)
{
  // Required as need to inform Hlinks also happens
  // when we port or PG sent by remote node is 
  // different from what we have(in lowlevel pgl,
  // portid accepted irrespective of the value where as
  // a different pg leads 2 1wayoutside, so Mismatch
  // in hello is not possible 
  // Broadcasting Mismatch makes sense
  // Imagining that SVCh went to 2way then svc released
  // due to some error and then we get svc back up
  // & Mismatch. The HLinks could be in 2-way and
  // now they need to generate Mismatch.
  BroadCastMismatchToHLinks(s);

  if (IsCalledParty(s)) {
    ChangeState(s, RCCHelloState::AttemptState);
    do_Hp(s, v, 8);
  } else {
    RcvLinkDown(s, v);
    SendReleaseRequest(s);
    SendSetupRequest(s);
  }
  return fsm_ok;
}

int SVCStateAttempt::RcvLinkDown(RCCHelloState *s, Visitor *v)
{
  ChangeState(s, RCCHelloState::LinkDownState);
  do_Hp(s, v, 9);
  if (GetCause53(s)) 
    BroadCastBadNeighborToHLinks(s);
  return fsm_ok;
}


//-------------------SVC1WayState---------------------------------------------
SVCState1WayInside::SVCState1WayInside() { }

SVCState1WayInside::~SVCState1WayInside() { }

int SVCState1WayInside::RcvWayInside1(RCCHelloState *s, Visitor *v)
{
  do_Hp(s, v, 12);
  return fsm_ok;
}

int SVCState1WayInside::RcvWayInside2(RCCHelloState *s, Visitor  *v)
{
  ChangeState(s, RCCHelloState::Way2InsideState);
  do_Hp(s, v, 4);
  return fsm_ok;
}

int SVCState1WayInside::RcvHelloMisMatch(RCCHelloState *s, Visitor *v)
{
  BroadCastMismatchToHLinks(s);

  if (IsCalledParty(s)) {
    ChangeState(s, RCCHelloState::AttemptState);
    do_Hp(s, v, 8);
  } else {
    RcvLinkDown(s, v);
    SendReleaseRequest(s);
    SendSetupRequest(s);
  }
  return fsm_ok;
}

int SVCState1WayInside::RcvLinkDown(RCCHelloState *s, Visitor *v)
{
  ChangeState(s, RCCHelloState::LinkDownState);
  do_Hp(s, v, 9);
  if (GetCause53(s))
    BroadCastBadNeighborToHLinks(s);
  return fsm_ok;
}

int SVCState1WayInside::ExpInactivityTimer(RCCHelloState *s)
{
  ChangeState(s, RCCHelloState::AttemptState);
  do_Hp(s, 0, 8);
  return fsm_ok;
}

//-------------------SVC2WayState------------------------------------------
SVCState2WayInside::SVCState2WayInside() { }

SVCState2WayInside::~SVCState2WayInside() { }
 
int SVCState2WayInside::RcvWayInside1(RCCHelloState *s, Visitor *v)
{
  ChangeState(s, RCCHelloState::Way1InsideState);
  do_Hp(s, v, 10);
  return fsm_ok;
}

int SVCState2WayInside::RcvWayInside2(RCCHelloState *s, Visitor  *v)
{
  do_Hp(s, v, 12);
  return fsm_ok;
}

int SVCState2WayInside::RcvHelloMisMatch(RCCHelloState *s, Visitor *v)
{
  BroadCastMismatchToHLinks(s);

  if (IsCalledParty(s)) {
    ChangeState(s, RCCHelloState::AttemptState);
    do_Hp(s, v, 16);
  } else {
    RcvLinkDown(s, v);
    SendReleaseRequest(s);
    SendSetupRequest(s);
  }
  return fsm_ok;
}

int SVCState2WayInside::RcvLinkDown(RCCHelloState *s, Visitor *v)
{
  ChangeState(s, RCCHelloState::LinkDownState);
  do_Hp(s, v, 18);
  if (GetCause53(s))
    BroadCastBadNeighborToHLinks(s);
  return fsm_ok;
}

int SVCState2WayInside::ExpInactivityTimer(RCCHelloState *s)
{
  ChangeState(s, RCCHelloState::AttemptState);
  do_Hp(s, 0, 16);
  return fsm_ok;
}

int SVCState2WayInside::RcvBadNeighbor(RCCHelloState *s, Visitor *v)
{
  // BadNeighbor received only in 2-way state here
  // when NP has DS mismatch or the BadPTSERequest
  BroadCastBadNeighborToHLinks(s);
  return fsm_ok;
}

#endif // __RCCHelloState_cc__
