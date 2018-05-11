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
static char const _Forwarder_cc_rcsid_[] =
"$Id: Forwarder.cc,v 1.191 1999/02/10 19:17:36 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/diag.h>
#include <FW/kernel/SimEvent.h>
#include <codec/pnni_ig/id.h>
#include <codec/q93b_msg/generic_q93b_msg.h>
#include <codec/q93b_msg/setup.h>
#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/UNI40_bbc.h>
#include <codec/uni_ie/called_party_num.h>
#include <codec/uni_ie/calling_party_num.h>
#include <codec/uni_ie/ie_base.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/election/ElectionVisitor.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/LinkUpVisitor.h>
#include <fsm/visitors/BorderVisitor.h>
#include <fsm/visitors/PortDownVisitor.h>
#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/visitors/PortVisitor.h>
#include <fsm/visitors/VPVCVisitor.h>
#include <fsm/forwarder/Fab.h>
#include <fsm/forwarder/Forwarder.h>
#include <fsm/forwarder/VCAllocator.h>
#include <fsm/forwarder/ForwarderInterfaces.h>

// These need to be changed to per port sometime in the future.
//   Perhaps DF should maintain them since it maintains other
//   port specific information.
const int SIGNALLING_VP = 0;
const int SIGNALLING_VC = 5;

#define  CREF_FLAG  0x800000

// --------------- TEST CODE ------------------
#ifndef OPTIMIZE
#define CHECK checkFabrics()
void Forwarder::checkFabrics(void)
{
  dic_item di;

  forall_items(di, _FabFabMap) {
    list<Fab *> * lst = _FabFabMap.inf(di);
    lst->size();  // This runs invariant checking on the list
  }
  _FabFabMap.size();             // This runs invariant checking on the dictionary
  _FabAddrMap.size();            // This runs invariant checking on the dictionary
  _Port2_CREF2Fab_Map.size();    // This runs invariant checking on the dictionary
  _setup_in_progress_Map.size(); // This runs invariant checking on the dictionary

  forall_items(di, _Port2_CREF2Fab_Map) {
    dictionary<unsigned int, Fab *> * CREF2Fab = _Port2_CREF2Fab_Map.inf(di);
    int port = _Port2_CREF2Fab_Map.key(di);

    dic_item di2;
    forall_items(di2, *CREF2Fab) {
      Fab * f = CREF2Fab->inf(di2);

      assert( f->GetCREF() == CREF2Fab->key(di2) &&
	      f->GetPort() == port );
    }
  }
}
#else
#define CHECK
#endif
// --------------- TEST CODE ------------------

const VisitorType * Forwarder::_fastunitype    = 0;
const VisitorType * Forwarder::_electiontype   = 0;
const VisitorType * Forwarder::_portuptype     = 0;
const VisitorType * Forwarder::_linkuptype     = 0;
const VisitorType * Forwarder::_portdowntype   = 0;
const VisitorType * Forwarder::_borderuptype   = 0;
const VisitorType * Forwarder::_borderdowntype = 0;
const VisitorType * Forwarder::_vpvctype       = 0;
const VisitorType * Forwarder::_porttype       = 0;

//----------------------------------------------------------------------------
Forwarder::Forwarder( const NodeID * nid )
  : _LocNodeID(0), _nports(0), _lowestCREF(1),
    _DATA_DELAY(0.0), _CTRL_DELAY(0.01)
{
  assert( nid != 0 );
  _LocNodeID = new NodeID( *nid );

  // These are the visitor types we need to be sensitive to
  if (_fastunitype  == 0)
    _fastunitype     = QueryRegistry(FAST_UNI_VISITOR_NAME);
  if (_electiontype  == 0)
    _electiontype    = QueryRegistry(ELECTION_VISITOR_NAME);
  if (_portuptype   == 0)
    _portuptype      = QueryRegistry(PORT_UP_VISITOR_NAME);
  if (_borderuptype == 0)
    _borderuptype    = QueryRegistry(BORDER_UP_VISITOR_NAME);
  if (_linkuptype   == 0)
    _linkuptype      = QueryRegistry(LINK_UP_VISITOR_NAME);
  if (_portdowntype == 0)
    _portdowntype    = QueryRegistry(PORT_DOWN_VISITOR_NAME);
  if (_borderdowntype == 0)
    _borderdowntype  = QueryRegistry(BORDER_DOWN_VISITOR_NAME);
  if (_vpvctype     == 0)
    _vpvctype        = QueryRegistry(VPVC_VISITOR_NAME);
  if (_porttype     == 0)
    _porttype        = QueryRegistry(PORT_VISITOR_NAME);

  Interface * iface = new ForwarderDefaultInterface(this);
  AddPermission("*", iface);
}

Forwarder::~Forwarder()
{
  // must .. clean out .. the data structs .... too weak
}

//----------------------------------------------------------------------------
void Forwarder::Absorb(Visitor * v)
{
  CHECK;

  VisitorType   vt = v->GetType();

  if (vt.Is_A(_vpvctype)) {
    // we have a VPVCVisitor
    VPVCVisitor * vis = (VPVCVisitor *)v;
    int iport = vis->GetInPort();
    int ivci  = vis->GetInVC();
    int ivpi  = vis->GetInVP();
    
    if (vt.Is_A(_fastunitype)) {
      // this is a fast uni visitor
      FastUNIVisitor * fv = (FastUNIVisitor*)v;

      if ( fv->GetSharedMessage() != 0 ) {
	// It MUST have a message otherwise the Translator will barf
	if (iport > 0) 
	  HandleFUVFromPhysical(fv, iport, ivpi, ivci);
	else 
	  HandleFUVFromControl(fv, iport, ivpi, ivci);
	v = fv;
      } else {
	SendToControl( v );
	v = 0;
      }
    } else if (vt.Is_A(_electiontype)) {
      ElectionVisitor * ev = (ElectionVisitor *)v;
      if ( ev->GetOutPort() == 0 )
	SendToControl( v );
      else
	SendToPhysical( v, ev->GetOutPort() );
      v= 0;
    } else if ((ivpi == 0) && (ivci == 18)) {
      // for PNNI on the physical level
      if (iport != 0) 
	HandlePNNIFromPhysical(vis, iport, ivpi, ivci);
      else 
	HandlePNNIFromControl(vis, iport, ivpi, ivci);
      v = vis;
    } else {
      //  data on an SVC, potentially for PNNI on a logical level
      if (iport != 0) 
	HandleDataFromPhysical(vis, iport, ivpi, ivci);
      else 
	HandleDataFromControl(vis, iport, ivpi, ivci);
      v = vis;
    }
  } else if (vt.Is_A(_portuptype) || vt.Is_A(_borderuptype)) { 
    // PortUpVisitor to the Control port
    PortUpVisitor * pu = (PortUpVisitor *)v;
    int port = pu->GetInPort();
    if (!_vcallocs.lookup(port)) {
      _vcallocs.insert(port, new VCAllocator());
      _nports++;
    }
    SendToControl( v );
    v = 0;
  } else if (vt.Is_A(_portdowntype) || vt.Is_A(_borderdowntype)) { 
    // PortDownVisitor to the Control port
    PortUpVisitor * pu = (PortUpVisitor *)v;
    dic_item di;
    int port = pu->GetInPort();
    if (di = _vcallocs.lookup(port)) {
      VCAllocator * vca = _vcallocs.inf(di);
      _vcallocs.del_item(di);
      delete vca;
      _nports--;
    }
    SendToControl( v );
    v = 0;
  }

  // left overs
  if (v) {
    DIAG("fsm.forwarder", DIAG_FATAL, // DIAG_WARNING, 
	 cout << OwnerName()
	 << ": Ack!  I cannot handle a(n) " << v->GetType()
	 << " (" << hex << v << dec << ")! " << endl);
    v->Suicide();
  }
  CHECK;
}

//----------------------------------------------------------------------------
// FUV coming from physical
void Forwarder::HandleFUVFromPhysical(FastUNIVisitor *& fv,
				      int iport, int ivpi, int ivci)
{
  InfoElem ** ie;
  generic_q93b_msg * msg = (generic_q93b_msg *)fv->GetSharedMessage();
  assert(msg != 0);
  u_int icref = msg->get_crv(), ocref = msg->get_crv();
  Fab * in = 0;  
  dic_item di = 0;
  dictionary<u_int, Fab *> * CREF2Fab = 0;
  Addr * iaddr = 0;

  switch (msg->type()) {
    case header_parser::setup_msg: 
      {
	ie = msg->get_ie_array();
	assert(ie[InfoElem::ie_called_party_num_ix]); // mandatory
	// set destination to called party in fv
	Addr * our_lgn_addr =
	  ((ie_called_party_num*)
	   (ie[InfoElem::ie_called_party_num_ix]))->get_addr();
	
	NodeID my_logical_nodeid(our_lgn_addr);
	// Here we set the destination NodeID to OUR logical nodeID.
	fv->SetDestNID(&my_logical_nodeid);
	//
	//     Addr class needs to support this kind of equality
	//     if(ie[ie_called_party_num_ix]->equals(_LocNodeID->GetAddr()))
	//     In the meantime we have:
	NSAP_DCC_ICD_addr * myAddr = (NSAP_DCC_ICD_addr*)(_LocNodeID->GetAddr()),
	                  * hisAddr = (NSAP_DCC_ICD_addr *)(ie[InfoElem::ie_called_party_num_ix]);

	DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
	assert( dbi != 0 && dbi->good() );
	dbi->Reference();
	bool IAmLogicalNode = dbi->LogicalNodeIsMe( fv->GetDestNID() );
	dbi->Unreference();
	if ( hisAddr->equals(myAddr) || IAmLogicalNode ) {
	  // we are the final switch
	  
	  // allocate terminal (ovpi,ovci) : guaranteed
	  int ovpi  = 0;
	  int ovci  = RequestNewVC(0);
	  int oport = 0;
	  // Perhaps we should check to see if the icref has the flag set first?
	  ocref = (NextPort0CREF() | CREF_FLAG);
	  Addr * oaddr = our_lgn_addr->copy();
	  if (fv->GetSourceNID())
	    iaddr = (fv->GetSourceNID())->GetAddr();
	  
	  // Set the out VP/VC
	  fv->SetOutVP(ovpi);
	  fv->SetOutVC(ovci);
	  // Since we're the destination, also set the in VP/VC
	  fv->SetInVP(ovpi);
	  fv->SetInVC(ovci);
	  fv->SetCREF(ocref);

	  // register the final binding:
	  // (iport,ivci,ivpi,icref,0, <<-->> oport,ovci,ovpi,ocref,oaddr)
	  Bind(iport, ivpi, ivci, icref, iaddr, 
	       oport, ovpi, ovci, ocref, oaddr);
	  
	  assert(ie[InfoElem::ie_broadband_bearer_cap_ix]); // mandatory
	  ie_bbc::conn_type t =
	    ((ie_bbc *)(ie[InfoElem::ie_broadband_bearer_cap_ix]))->get_conn_type();
	  // This is required now since the other block was commented in Bind
	  if (t == ie_bbc::p2p)
	    Bind(oport, ovpi, ovci, ocref, oaddr, 
		 iport, ivpi, ivci, icref, iaddr);
	} else {
	  // intermediate switch
	  // save input Fab
	  Fab * fab = new Fab(iport, ivpi, ivci, icref, iaddr);
	  _setup_in_progress_Map.insert(fv, fab);
	}
	delete myAddr;
      }
      break;
    
    case header_parser::call_proceeding_msg:
      //
      // set _ocref to complement of msg cref, check if the (ivci, ivpi)
      // match the pair allocated, if not solidify the binding
      //
      assert(di = _Port2_CREF2Fab_Map.lookup(iport));
      assert(CREF2Fab = _Port2_CREF2Fab_Map.inf(di));
      assert(di = CREF2Fab->lookup(icref));     // get the binding associated
      assert(in = CREF2Fab->inf(di));

      if (in->GetVC() < 0) {
	// First return the tempory VC allocated for this fabric
	ReturnVC(iport, -(in->GetVC()));
	// Then modify the bindings
	Rebind(in, ivpi, ivci);
      }
      break;

    case header_parser::release_msg:
      if (OriginatedCall(iport, icref)) {
	// Find the CREF for the last leg and hammer it into the FUV
	Fab inFab(iport, ivpi, ivci, icref);
	dic_item di = _FabFabMap.lookup(&inFab);
	assert(di != 0);
	list<Fab *> * outFabs = _FabFabMap.inf(di);
	assert(outFabs != 0);
	fv->SetCREF(outFabs->head()->GetCREF());
        UnBind(iport, icref);  /* even in the face of crankback this
				* can be handled properly.  */
      } else {
      //   lookup the bindings for this call ....  hopefully it's p2p
      //   so there are only two mappings.  in -> out, out -> in
      //   remove the half of the binding that the release came in on
      //   (the 'out' bindings).  remove the out -> in binding since
      //   that will be taken care of when the pending -> permanent
      //   place the 'in' bindings in the pending setups map
      }
      // Releases from the physical ports go directly to the control
      // port for processing.
      break;

    case header_parser::connect_msg:
      // Sorry, but this has to be forwarded to the end point
      Forward((VPVCVisitor *&)fv);
      break;

    default:
      diag("fsm.forwarder", DIAG_ERROR,
	   "%s: FastUNIVisitor (%x) carrying unsupported message type (%d).\n",
	   OwnerName(), fv, msg->type());
      break;
  }

  CHECK;
  if (fv) {
    SendToControl( fv );
    fv = 0;
  }
}

// FUV coming from ACAC
void Forwarder::HandleFUVFromControl(FastUNIVisitor *& fv,
				     int iport, int ivpi, int ivci)
{ 
  generic_q93b_msg * msg = (generic_q93b_msg *)fv->GetSharedMessage();

  // Unfortunately we may receive some special purpose FUVs from
  //   the control port -- these FUVs may NEVER leave the switch.
  assert(msg != 0);
  u_long icref = msg->get_crv(), ocref = msg->get_crv();
  int  oport = fv->GetOutPort();
  bool pending = false;
  Addr * iaddr = (fv->GetSourceNID() ? fv->GetSourceNID()->GetAddr() : 0), 
       * oaddr = (fv->GetDestNID() ? fv->GetDestNID()->GetAddr() : 0);
  dic_item di = 0;
  
  switch (msg->type()) 
    {
    case header_parser::setup_msg:
      {
	// allocate temporary (ovpi,ovci)
	int ovpi  = 0;
	int ovci  = -1 * RequestNewVC(oport);  // negative means temporary
	
	dic_item di;
	// It is a setup message, so either it is in progress, or it is
	//   being setup by us.
	if (di = _setup_in_progress_Map.lookup(fv)) {
	  // Since the setup is in progress we are not the originator
	  Fab * fab = _setup_in_progress_Map.inf(di);
	  iport = fab->GetPort();
	  ivpi  = fab->GetVP();
	  ivci  = fab->GetVC();
	  icref = fab->GetCREF();
	  if (iaddr) delete iaddr;
	  iaddr = fab->GetAddr();
	  // TODO what about _dest - is this really 0
	  _setup_in_progress_Map.del_item(di);
	  delete fab;
	} else if (iport == 0) {
	  //
	  // If this switch originated the call then ACAC in the
	  // Control port set the iport to zero.  ACAC also chose the
	  // input vpi, vci, and call reference.
	  //
	  assert(ivpi == fv->GetInVP());
	  assert(ivci == fv->GetInVC());	    
	} else {
	  DIAG("fsm.forwarder", DIAG_FATAL, cout << *_LocNodeID << ": " <<
	       "Ummm, what the hell is going on?  A setup seems " << 
	       "to have bypassed me on the way in!" << endl);
	}
	// Solidify the bindings for the Visitor
	Bind(iport, ivpi, ivci, icref, iaddr, 
	     oport, ovpi, ovci, ocref, oaddr);
	
	InfoElem ** ie = msg->get_ie_array();
	assert(ie[InfoElem::ie_broadband_bearer_cap_ix]); // mandatory
	ie_bbc::conn_type t =
	  ((ie_bbc *)(ie[InfoElem::ie_broadband_bearer_cap_ix]))->get_conn_type();
	// This is required now since the other block was commented in Bind
	if (t == ie_bbc::p2p)
	  Bind(oport, ovpi, ovci, ocref, oaddr,
	       iport, ivpi, ivci, icref, iaddr); 
      }
    break;
      
    case header_parser::release_msg: 
      {
	if (iaddr) delete iaddr;
	if (oaddr) delete oaddr;
	// This is necessary to morph failures into releases, otherwise
	//  ACAC and pals get pissy.
	if (fv->GetMSGType() == FastUNIVisitor::FastUNISetupFailure) {
	  pending = true;
	  generic_q93b_msg * tmp = fv->TakeMessage();
	  fv->SetMSGType(FastUNIVisitor::FastUNIRelease);
	  fv->SetMessage(tmp);
	}
	
	// Its a release from Control - ACAC agreed to store the original 
	// incoming port for this Visitor in its outPort - 
	// so swap In and Out ports
	fv->SetInPort(fv->GetOutPort());
	fv->SetInVP(  fv->GetOutVP());   // added by BK 2.27.98
	fv->SetInVC(  fv->GetOutVC());   // added by BK 2.27.98
	
	if (pending)
	  di = _setup_in_progress_Map.lookup(fv);
	  
	// Special case for Releases because they must be forwarded in both directions
	ForwardRelease(fv); 
	
	if (di) {
	  Fab * tmp = _setup_in_progress_Map.inf(di); 
	  _setup_in_progress_Map.del_item(di);
	  delete  tmp;
	} else {
	  // Remove the binding information for this call
	  //
	  // Use the oport because iport is zero.  oport was set to the
	  // Right Value by ACAC when he released the call.
	  if (!UnBind(oport, icref)) {
	    DIAG("fsm.forwarder", DIAG_FATAL, cout << *_LocNodeID <<
		 ": Failure to unbind pending information for Port " << 
		 oport << " CREF 0x" << hex << icref << dec << endl);
	  }
	} 
      }
    break;
    
    case header_parser::connect_msg:
      if (iaddr) delete iaddr;
      if (oaddr) delete oaddr;
      DIAG("fsm.forwarder", DIAG_DEBUG, cout << *_LocNodeID <<
	   ": received a Connect Message destined for " << *(fv->GetDestNID()) << endl);
      Forward((VPVCVisitor *&)fv);
      break;
      
    default: // Shouldn't get call procs (or anything else) from control
      DIAG("fsm.forwarder", DIAG_FATAL, cout << *_LocNodeID << 
	   ": FastUNIVisitor (0x" << hex << fv << dec << ") carrying " <<
	   "unsupported message type (" << msg->type() << endl);
      break;
    }
  CHECK;
  // register binding: (iport,ivpi,ivci)->(oport,ovpi,ovci)
  // register icref->(iport,ivpi,ivci)
  // register ocref->(oport,ovpi,ovci)
  if (fv) {
    if ( fv->GetOutPort() == 0 )
      SendToControl( fv );
    else
      SendToPhysical( fv, fv->GetOutPort() );
  }
  fv = 0;
}

//-----------------------------------------------------------------------
// coming from Control Port - 0
// iport set by originator
void Forwarder::HandlePNNIFromControl(VPVCVisitor *& vis,
				      int iport, int ivpi, int ivci)
{
  CHECK;
  assert(vis->GetOutPort() != 0);
  SendToPhysical( vis, vis->GetOutPort() );
  vis = 0;
}

// PNNI coming from physical ports
// iport must already be in the visitor
void Forwarder::HandlePNNIFromPhysical(VPVCVisitor *& vis,
				       int iport, int ivpi, int ivci)
{
  CHECK;
  assert(vis->GetDestNID()); // inserted by hello
  SendToControl( vis );
  vis = 0;
}

//-------------------------------------------------------------------------
// Data coming from Control
void Forwarder::HandleDataFromControl(VPVCVisitor *& vis,
				     int iport, int ivpi, int ivci)
{
  Forward(vis);
} 

// data coming from physical port
void Forwarder::HandleDataFromPhysical(VPVCVisitor *& vis,
				       int iport, int ivpi, int ivci)
{
  // hammering of DestNID for oport==0 is done in Forward();
  Forward(vis);
}

//-------------------------------------------------------------------------
int Forwarder::RequestNewVC(int port, int request)
{
  int answer = 0;

  dic_item di = _vcallocs.lookup(port);
  if (!di) { di = _vcallocs.insert(port, new VCAllocator()); }
  if (request != -1) answer = _vcallocs.inf(di)->RequestVCI(request);
  else answer = _vcallocs.inf(di)->GetNewVCI();

  return answer;
}

bool Forwarder::ReturnVC(int port, int vc)
{
  dic_item di = _vcallocs.lookup(port);
  if (!di) { return false; }
  _vcallocs.inf(di)->ReturnVCI(vc);
  return true;
}

VCAllocator * Forwarder::GetSharedPortZeroVCAllocator(void)
{
  int i = 0;
  dic_item di = _vcallocs.lookup(i);
  if (!di) di = _vcallocs.insert(i, new VCAllocator());
  return _vcallocs.inf(di);
}


//---------------------------------------------------------------------------
bool Forwarder::Bind(Fab * in, Fab * out)
{
  return Bind(in->GetPort(),  in->GetVP(),  in->GetVC(),  
	      in->GetCREF(),  in->GetAddr(),
	      out->GetPort(), out->GetVP(), 
	      out->GetVC(), out->GetCREF(), out->GetAddr());
}

bool Forwarder::Bind(int iport, int ivpi, int ivci, 
		     int icref, const Addr* iaddr,
		     int oport, int ovpi, int ovci, 
		     int ocref, const Addr* oaddr)
{
  // the ports, vp's, and crefs MUST be above zero, vc's may be negative, I think
  assert(iport >= 0 && oport >= 0 &&
	 ivpi  >= 0 && ovpi  >= 0 &&
	 icref >= 0 && ocref >= 0);

  dic_item di;
  Fab * in  = new Fab(iport, ivpi, ivci, icref, (Addr *)iaddr);
  Fab * out = new Fab(oport, ovpi, ovci, ocref, (Addr *)oaddr);

  DIAG("fsm.forwarder", DIAG_INFO,
       cout << *_LocNodeID << ": Binding " 
       << *in << " <--> " << *out << endl);

  DIAG("fsm.forwarder", DIAG_DEBUG, PrintBindings(cout));

  if (!(di = _FabFabMap.lookup(in)))
    di = _FabFabMap.insert(in, new list<Fab *>);
  list<Fab *> * outputlist = _FabFabMap.inf(di);
  if (!(outputlist->search(out)))
    outputlist->append(out);
  else {
    // Place your break point in here
    diag("fsm.forwarder", DIAG_ERROR, 
	 "%s: Attempted to insert duplicate entry in outbound fabric list.\n",
	 OwnerName());
  }

  if (oaddr) _FabAddrMap.insert(in,  (Addr *)oaddr);

  dictionary<u_int,Fab *> * CREF2Fab = 0;

  if (!(di = _Port2_CREF2Fab_Map.lookup(iport))) 
    di = _Port2_CREF2Fab_Map.insert(iport, new dictionary<u_int, Fab *> );
  assert( (CREF2Fab = _Port2_CREF2Fab_Map.inf(di)) != 0 );
  assert( CREF2Fab->lookup(icref) == 0 );
  CREF2Fab->insert(icref, in);

#ifdef ADD_REVERSE_BINDINGS
  // Add the reverse binding here as well -- THIS NEEDS TO BE FIXED FOR P2MP
  if (!(di = _FabFabMap.lookup(out)))
    di = _FabFabMap.insert(out, new list<Fab *>);
  outputlist = _FabFabMap.inf(di);
  if (!(outputlist->search(in)))
    outputlist->append(in);
  else {
    // Place your break point in here
    diag("fsm.forwarder", DIAG_ERROR, 
	 "%s: Attempted to insert duplicate entry in outbound fabric list.\n",
	 OwnerName());
  }

  if (iaddr) _FabAddrMap.insert(out, (Addr *)iaddr);

  if (!(di = _Port2_CREF2Fab_Map.lookup(oport)))
    di = _Port2_CREF2Fab_Map.insert(oport, new dictionary<u_int,Fab *> );
  assert( (CREF2Fab = _Port2_CREF2Fab_Map.inf(di)) != 0 );
  assert( CREF2Fab->lookup(ocref) == 0 );
  CREF2Fab->insert(ocref, out);

#endif
  // In the case of crankback we may already have a binding which we
  // need to replace with the new binding.  - NOT FINISHED YET 11am
  // 2-6-98
  return true;
}

bool Forwarder::Rebind(Fab * in, int ivpi, int ivci)
{
  // Then attempt to look it up in the _FabFabMap
  dic_item di;
  assert(di = _FabFabMap.lookup(in));
  dic_item di2 = _FabAddrMap.lookup(in);

  // How do we know this is one we need to change?  It could be the reverse!
  in->SetVC(ivci);
  in->SetVP(ivpi);
  // How does it look up correctly if we've already changed it?
  //     I guess all the pointers are shared ... confusing!
  Fab * fin = _FabFabMap.key(di);
  fin->SetVC(ivci);
  fin->SetVP(ivpi);
  // Now update the outgoing bindings
  list<Fab *> * flist = _FabFabMap.inf(di);
  list_item li;
  forall_items(li, *flist) {
    Fab * out = flist->inf(li);
    
    dic_item di2;
    if (di2 = _FabFabMap.lookup(out)) {
      list<Fab *> * reverse = _FabFabMap.inf(di2);
      
      Fab * rin = reverse->head();

      rin->SetVC(ivci);
      rin->SetVP(ivpi);
     
      // Make sure that the Fab * in this list matches in
      assert(rin->equals(*in));
    }
    DIAG("fsm.forwarder", DIAG_INFO,
	 cout << *_LocNodeID << ": ReBinding " 
	 << *in << " <--> " << *out << endl);
  }
  // If we've already changed it how the heck does it look up correctly?
  if (di2) {
    Fab * ain = _FabAddrMap.key(di2);
    ain->SetVC(ivci);
    ain->SetVP(ivpi);
  }

  return true;
}


// This will remove the bindings in both directions
//   If this is part of a p2mp call the caller must 
//   be holding on to the other items
bool Forwarder::UnBindBothDirections(Fab & in, Fab & out)
{
  CHECK;
  bool rval = true;
  dic_item di;

  DIAG("fsm.forwarder", DIAG_INFO, cout << *_LocNodeID << ": Removing binding " << in << endl);
  DIAG("fsm.forwarder", DIAG_DEBUG, PrintBindings(cout));

  // Remove from FabAddrMap
  if (di = _FabAddrMap.lookup(&in)) {
    Addr * addr = _FabAddrMap.inf(di);
    if (addr) delete addr;
    _FabAddrMap.del_item(di);
  } // we let this slide since calling party addr isn't always required

  // Remove from FabFabMap
  if (di = _FabFabMap.lookup(&in)) {
    list<Fab *> * lst = _FabFabMap.inf(di);
    if (lst->size() == 1)
      delete lst;
    _FabFabMap.del_item(di);
  } else
    rval = false;

  if (di = _Port2_CREF2Fab_Map.lookup(in.GetPort())) {
    dic_item di2;
    dictionary<u_int, Fab *> * CREF2Fab = _Port2_CREF2Fab_Map.inf(di);
    if (di2 = CREF2Fab->lookup(in.GetCREF()))
      CREF2Fab->del_item(di2);
    if (CREF2Fab->empty()) {
      delete CREF2Fab;
      _Port2_CREF2Fab_Map.del_item(di);
    }
  }

  // ----- now handle the outgoing bindings
  DIAG("fsm.forwarder", DIAG_INFO, cout << *_LocNodeID << ": Removing binding " << out << endl);

  // Remove from FabAddrMap
  if (di = _FabAddrMap.lookup(&out)) {
    Addr * addr = _FabAddrMap.inf(di);
    if (addr) delete addr;
    _FabAddrMap.del_item(di);
  } // we let this slide since calling party addr isn't always required

  // Remove from FabFabMap
  if (di = _FabFabMap.lookup(&out)) {
    list<Fab *> * lst = _FabFabMap.inf(di);
    if (lst->size() == 1)
      delete lst;
    _FabFabMap.del_item(di);
  } else
    rval = false;

  if (di = _Port2_CREF2Fab_Map.lookup(out.GetPort())) {
    dic_item di2;
    dictionary<u_int, Fab *> * CREF2Fab = _Port2_CREF2Fab_Map.inf(di);
    if (di2 = CREF2Fab->lookup(out.GetCREF()))
      CREF2Fab->del_item(di2);
    if (CREF2Fab->empty()) {
      delete CREF2Fab;
      _Port2_CREF2Fab_Map.del_item(di);
    }
  }

  CHECK;
  return rval;
}

bool Forwarder::UnBind(Fab * oneSide)
{
  CHECK;
  dic_item di;
  bool rval = false;

  list<Fab *> * givenList = 0;
  // Locate the list of bindings for the given side,
  if (di = _FabFabMap.lookup(oneSide))
    givenList = _FabFabMap.inf(di);

  Fab * firstItem = givenList->head();
  list<Fab *> * secondList = 0;
  // Locate the list of bindings for the reverse side,
  if (di = _FabFabMap.lookup(firstItem))
    secondList = _FabFabMap.inf(di);

  list<Fab *> * iterator = (givenList->size() >= secondList->size() ? givenList : secondList);
  Fab * itItem = (givenList->size() >= secondList->size() ? oneSide : firstItem);

  assert(iterator->empty() == false);

  // Iterate over the list with the most elements
  list_item li, nli;
  for (li = iterator->first(); li; li = nli) {
    nli = iterator->next(li);
    
    Fab * tmpItem = iterator->inf(li);
    // call UnBindBothDirections on each pair in the list
    rval = UnBindBothDirections(*itItem, *tmpItem);
    delete tmpItem;
  }
  // Cleanup the longest list, all others should've been cleaned up by UnBindBothDirections
  //  delete iterator;
  // Cleanup the key Fab *
  delete itItem;

  CHECK;
  // return true if all UnBinds were successful
  return rval;
}


// Remove the bindings for both directions in all three maps.  This
// method calls UnBind(Fab *) and removes mapping from
// _Port2_CREF2Fab_Map
bool Forwarder::UnBind(int port, u_int cref)
{
  CHECK;
  bool rval = false;
  dic_item di;

  if (di = _Port2_CREF2Fab_Map.lookup(port)) {
    dictionary<u_int, Fab *> * CREF2Fab = _Port2_CREF2Fab_Map.inf(di);

    dic_item di2;
    if (di2 = CREF2Fab->lookup(cref)) {
      Fab * in_fab = CREF2Fab->inf(di2);

      rval = UnBind(in_fab);
    }
  }
  CHECK;
  return rval;
}

// -------------- helper that forwards a VPVCVisitor  ---------------
void Forwarder::ForwardRelease(FastUNIVisitor *& release)
{
  bool sent = false;
  int port = release->GetInPort();
  u_int cref = release->GetCREF();

  dic_item di;
  // If not then it MUST be in the 'setup_in_progress_Map'
  if ( (di = _setup_in_progress_Map.lookup(release)) != 0 ) {
    Fab * out_fab = _setup_in_progress_Map.inf(di);

    release->SetOutPort(out_fab->GetPort());
    // release->SetOutVP  (out_fab->GetVP());
    // release->SetOutVC  (out_fab->GetVC());
    release->SetOutVP(SIGNALLING_VP);
    release->SetOutVC(SIGNALLING_VC);
    release->SetCREF(out_fab->GetCREF());
    Addr * addr = out_fab->GetAddr();
    if (addr) {
      NodeID nid(addr);
      release->SetDestNID(&nid);
      delete addr;
    }
    if ( release->GetOutPort() == 0 )
      SendToControl( release );
    else
      SendToPhysical( release, release->GetOutPort() );
    sent = true;
  } else
  // First check to see if the binding has already been set properly
  if ((di = _Port2_CREF2Fab_Map.lookup(port)) != 0) {
    dictionary<unsigned int, Fab *> * CREF2Fab = _Port2_CREF2Fab_Map.inf(di);

    if ( (di = CREF2Fab->lookup(cref)) != 0 ) {
      Fab * out_fab = CREF2Fab->inf(di);

      if (out_fab->GetPort() != release->GetInPort()) {
	FastUNIVisitor * VisToSend = (FastUNIVisitor *)(release->duplicate());

	VisToSend->SetOutPort(out_fab->GetPort());
	// For assistance in debugging, uncomment these
	//  release->SetOutVP(out_fab->GetVP());
	//  release->SetOutVC(out_fab->GetVC());
	VisToSend->SetOutVP(SIGNALLING_VP);
	VisToSend->SetOutVC(SIGNALLING_VC);
	VisToSend->SetCREF(out_fab->GetCREF());
	Addr * addr = out_fab->GetAddr();
	if (addr) {
	  NodeID nid(addr);
	  VisToSend->SetDestNID(&nid);
	  delete addr;
	}
	if ( VisToSend->GetOutPort() == 0 )
	  SendToControl( VisToSend );
	else
	  SendToPhysical( VisToSend, VisToSend->GetOutPort() );
	sent = true;
      } else
      // Now that we've found the correct Fabric we must locate the inverse of it ...
      if ( (di = _FabFabMap.lookup(out_fab)) != 0 ) {
	list<Fab *> * flist = _FabFabMap.inf(di);

	int count = flist->length();
	list_item li;
	forall_items(li, *flist) {
	  count--;
	  Fab * fabalicious = flist->inf(li);

	  FastUNIVisitor * VisToSend = release;
	  if (count)
	    VisToSend = (FastUNIVisitor *)(release->duplicate());

	  VisToSend->SetOutPort(fabalicious->GetPort());
	  // For assistance in debugging, uncomment these
	  //  release->SetOutVP(out_fab->GetVP());
	  //  release->SetOutVC(out_fab->GetVC());
	  VisToSend->SetOutVP(SIGNALLING_VP);
	  VisToSend->SetOutVC(SIGNALLING_VC);
	  VisToSend->SetCREF(fabalicious->GetCREF());
	  Addr * addr = fabalicious->GetAddr();
	  if (addr) {
	    NodeID nid(addr);
	    VisToSend->SetDestNID(&nid);
	    delete addr;
	  }
	  if ( VisToSend->GetOutPort() == 0 )
	    SendToControl( VisToSend );
	  else
	    SendToPhysical( VisToSend, VisToSend->GetOutPort() );
	  sent = true;
	}
      }
    }
  }

  assert( sent != false );  // The Fabric must be found above
  release = 0;
}

void Forwarder::Forward(VPVCVisitor *& vis)
{
  CHECK;
  bool sent = false;
  // save arrival information
  Fab in_fab(vis->GetInPort(), vis->GetInVP(), 
	     vis->GetInVC(), vis->GetCREF());
  
  // General Case
  dic_item di;
  if (di = _FabFabMap.lookup(&in_fab)) {
    Fab * key = _FabFabMap.key(di);
    list<Fab *> * lptr = _FabFabMap.inf(di);

    int count = lptr->size();
    // iterate over all the outgoing fabrics
    list_item li;
    forall_items(li, *lptr) {
      count--;

      assert( lptr->inf(li) );
      Fab out_fab(*(lptr->inf(li)));

      VPVCVisitor * VisToSend = vis;
      // Check if we need to send a copy
      if (count)
	VisToSend = (VPVCVisitor *)(vis->duplicate());

      // Restore information about arrival
      // This stuff should still be good, but what the heck, cycles are cheap ...
      VisToSend->SetInPort(key->GetPort());
      VisToSend->SetInVP(key->GetVP());
      VisToSend->SetInVC(key->GetVC());
      Addr * addr = key->GetAddr();
      if (addr) {
	NodeID nid(addr);
	// Don't worry SetSourceNID makes a copy
	VisToSend->SetSourceNID(&nid);
	delete addr;
      }

      // Set departure information from Fab in routing table
      int oport = -1;
      VisToSend->SetOutPort(oport = out_fab.GetPort());
      VisToSend->SetOutVP(out_fab.GetVP());
      VisToSend->SetOutVC(out_fab.GetVC());
      addr = out_fab.GetAddr();
      if (addr) {
	NodeID nid(addr);
	VisToSend->SetDestNID(&nid);
	delete addr;
      }

      if (oport == 0) {  // We are the destination switch
	dic_item dia = _FabAddrMap.lookup(key);
	// We MUST have this in there, otherwise the Destination NSAP Mux will fail
	assert(dia != 0);

	Addr * myaddr_from_call = _FabAddrMap.inf(dia);
	unsigned char id[22];
	myaddr_from_call->encode(id+2);
	// be damn careful around here
	id[0]  = id[21];
	id[21] = 0x0;

	DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface("Database");
	assert( dbi != 0 && dbi->good() );
	dbi->Reference();
	int level = dbi->NextLevelDown( id[0] );
	dbi->Unreference();
	if ( level == -1 )
	  id[1] = 160;
	else
	  id[1] = level;
	NodeID my_logical_nodeid(id);
	// It was always doing this to the original, but I've changed it - mountcas
	VisToSend->SetDestNID(&my_logical_nodeid);
      }

      CHECK;
      VisToSend->SetCREF(out_fab.GetCREF());
      DIAG("fsm.forwarder", DIAG_DEBUG, 
	   cout << OwnerName() << ": Sending out " << VisToSend->GetType() <<
	   " (" << VisToSend << ") from " << *(key) << " to " << out_fab << endl);
      // Send Visitor out.
      if ( VisToSend->GetOutPort() == 0 )
	SendToControl( VisToSend );
      else
	SendToPhysical( VisToSend, VisToSend->GetOutPort() );
      sent = true;
    }
  } else if (vis->GetType().Is_A(QueryRegistry(FAST_UNI_VISITOR_NAME))) {
    // Specific Case for Setups
    FastUNIVisitor * fv = (FastUNIVisitor *)vis;

    if (di = _setup_in_progress_Map.lookup(fv)) {
      Fab * out_fab = _setup_in_progress_Map.inf(di);

      vis->SetOutPort(out_fab->GetPort());
      vis->SetOutVP  (out_fab->GetVP());
      vis->SetOutVC  (out_fab->GetVC());
      vis->SetCREF   (out_fab->GetCREF());
      // This is taken care of in the HandleFUVfromControl
      // _setup_in_progress_Map.del_item(di);
      // delete out_fab;
      CHECK;
      if ( vis->GetOutPort() == 0 )
	SendToControl( vis );
      else
	SendToPhysical( vis, vis->GetOutPort() );
      sent = true;
    }
  }

  if (!sent) {
    // This should be a DIAG_FATAL because if you took the time to pass a 
    // Visitor up to the Forwarder, then you sure as hell want it 
    // forwarded somewhere, right?
    DIAG("fsm.forwarder", DIAG_WARNING, cout << *_LocNodeID <<
	 ": ***** NO VALID DEPARTURE INFO FOR " << vis->GetType() << 
	 " (" << hex << vis << dec << ")! " << in_fab << " *****" << endl;
	 PrintBindings(cout));
    // This is fatal for FUVs
    assert( vis->GetType().Is_A(_fastunitype) == false );
    vis->CleanLog();
    vis->Suicide();
  }
  CHECK;
  vis = 0;
}

void Forwarder::Interrupt(SimEvent * e) 
{ 
  // if its a DATA_QUEUE_EVENT
  if ( e->GetCode() == DATA_QUEUE_EVENT ) {
    assert( ! _dataQueue.empty() );
    Visitor * v = _dataQueue.pop();
    delete e;
    Inject( v );

  // if its a CTRL_QUEUE_EVENT
  } else if ( e->GetCode() == CTRL_QUEUE_EVENT ) {
    assert( ! _ctrlQueue.empty() );
    Visitor * v = _ctrlQueue.pop();
    delete e;
    Inject( v );
  }
}

void Forwarder::NotifyOfPort(int port)
{
  if (!_vcallocs.lookup(port)) {
    _vcallocs.insert(port, new VCAllocator());
    _nports++;
  } // else, umm you already told me about this damn port ... fool
}

void Forwarder::PrintBindings(ostream & os)
{
  os << "DataForwarder " << *_LocNodeID << " " << OwnerName() << endl;

  dic_item di;
  forall_items(di, _FabFabMap) {
    Fab * infab = _FabFabMap.key(di);
    list<Fab *> * flist = _FabFabMap.inf(di);

    os << *infab << " --> ";
    list_item li;
    forall_items(li, *flist) {
      Fab * outfab = flist->inf(li);
      os << *outfab << " ";
    }
    os << endl;
  }
}

void Forwarder::PrintInfo(ostream & os)
{
  os << "Forwarder " << *_LocNodeID << " " << OwnerName() << endl;

  dic_item di;
  forall_items(di, _Port2_CREF2Fab_Map) {
    dictionary<unsigned int, Fab *> * CREF2Fab = _Port2_CREF2Fab_Map.inf(di);
    int port = _Port2_CREF2Fab_Map.key(di);

    if (CREF2Fab != 0) {
      dic_item di2;
      forall_items(di2, *CREF2Fab) {
	u_int cref = CREF2Fab->key(di2);
	Fab * f = CREF2Fab->inf(di2);
	
	os << "[" << port << "," << hex << (int)cref << dec <<"] --> " << *f << endl;
      }
    }
  }
}

bool Forwarder::OriginatedCall(int port, u_int crv)
{
  bool rval = false;
  dic_item di1, di2;

  if (di1 = _Port2_CREF2Fab_Map.lookup(port)) {
    dictionary<u_int, Fab *> * CREF2Fab = _Port2_CREF2Fab_Map.inf(di1);

    if (di2 = CREF2Fab->lookup(crv)) {
      Fab * in = CREF2Fab->inf(di2);

      assert( port == in->GetPort() );

      // We should succeed in this first if block if we really are the
      // originator
      if (di1 = _FabAddrMap.lookup(in)) {
	// This logic in the binding helps us out here ...
	// if (iaddr) _FabAddrMap.insert(out, (Addr *)iaddr);
	// since when the release comes in the in-bindings are 
	// actually the out-bindings of the original setup.
	Addr * ad = _FabAddrMap.inf(di1);
	if (_LocNodeID->equals(ad))
	  rval = true;
      }
      // This may not be right -- this could return true if we are the
      // called party
      if (di1 = _FabFabMap.lookup(in)) {
	list<Fab *> * flist = _FabFabMap.inf(di1);
	list_item li;
	forall_items(li, *flist) {
	  Fab * out = flist->inf(li);
	  
	  if (di2 = _FabAddrMap.lookup(in)) {
	    Addr * ad = _FabAddrMap.inf(di2);
	    if (_LocNodeID->equals(ad))
	      rval = true;
	  }

	}
      }
    }
  }
  return rval;
}

u_int Forwarder::NextPort0CREF(void)
{
  if (_freeCREF.empty()) {
    for (u_int i = _lowestCREF; i < _lowestCREF + 25; i++)
      _freeCREF.append(i);
    _lowestCREF += 25;
  }
  u_int rval = _freeCREF.pop();
  _usedCREF.append(rval);
  return rval;
}

void  Forwarder::ReturnPort0CREF(u_int cref)
{
  list_item li = _usedCREF.lookup(cref);
  //  assert( li != 0 );
  if (li) {
    _usedCREF.del_item(li);
    _freeCREF.append(cref);
  }
}

void Forwarder::SendToControl( Visitor * v )
{
  if (v->GetType().Is_A(_porttype)) {
    PortVisitor * pv = (PortVisitor *)v;
    pv->SetOutPort( 0 );
  }

  SimEvent * se = new SimEvent( this, this, CTRL_QUEUE_EVENT );
  _ctrlQueue.append( v );
  Deliver( se, _CTRL_DELAY );
}

void Forwarder::SendToPhysical( Visitor * v, int p )
{

  if (v->GetType().Is_A(_porttype)) {
    PortVisitor * pv = (PortVisitor *)v;
    pv->SetOutPort( p );
  }

  SimEvent * se = new SimEvent( this, this, DATA_QUEUE_EVENT );
  _dataQueue.append( v );
  Deliver( se, _DATA_DELAY );
}
