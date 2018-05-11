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
static char const _Translator_cc_rcsid_[] =
"$Id: Translator.cc,v 1.63 1998/10/22 16:55:37 mountcas Exp $";
#endif

#include <fsm/nni/Translator.h>
#include <FW/basics/diag.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/visitors/PortDownVisitor.h>
#include <fsm/visitors/PortVisitor.h>
#include <fsm/nni/Q93bVisitors.h>
#include <codec/pnni_ig/id.h>
#include <codec/q93b_msg/q93b_msg.h>
#include <codec/uni_ie/ie.h>
#include <fsm/forwarder/VCAllocator.h>
#include <common/cprototypes.h>

bool match(Q93bVisitor::Q93bVisitorType qvt, FastUNIVisitor::uni_message fut);

// -------------------- Static Members ---------------------
const VisitorType * Translator::_fastunitype  = 0;
const VisitorType * Translator::_portuptype   = 0;
const VisitorType * Translator::_portdowntype = 0;
const VisitorType * Translator::_q93btype      = 0;


// -------------------- Constructor --------------------
Translator::Translator(void) : 
  State(), _master(false), _port_active(false)
{
  // These are the visitor types we need to be sensitive to
  if (_fastunitype  == 0)
    _fastunitype     = QueryRegistry(FAST_UNI_VISITOR_NAME);
  if (_portuptype   == 0)
    _portuptype      = QueryRegistry(PORT_UP_VISITOR_NAME);
  if (_portdowntype == 0)
    _portdowntype    = QueryRegistry(PORT_DOWN_VISITOR_NAME);
  if (_q93btype      == 0)
    _q93btype         = QueryRegistry(Q93B_VISITOR_NAME);

  _vcpool = new VCAllocator();
}

//----------------------------------------------------
Translator::~Translator(void)
{
  _vcpool->UnReference();
}

bool Translator::HandlePortUpFromInside(PortUpVisitor * pv)
{
  _port_active = true;
  // determine mastership here
  assert(pv->GetDestNID() && pv->GetSourceNID());
  // you got to do what you got to do : cast away constness
  // Captain's Log 1-29-98: make these NodeIDs const once jack fixes NIDcmp
  NodeID * lnid = (NodeID *)pv->GetDestNID();
  NodeID * rnid = (NodeID *)pv->GetSourceNID();

  if (lnid->NIDcmp(rnid) > 0)
    _master = true;
  else
    _master = false;

  pv->Suicide();
  return false;
}

// This converts incoming Q93bVisitors to FastUNIVisitors
bool Translator::HandleQ93bFromOutside(Q93bVisitor * qv)
{
  Q93bVisitor    * new_qv = 0L;     // for responses to incoming v
  FastUNIVisitor * fuv = 0;         // converting the FastUNI Visitor
  InfoElem * ie[num_ie];
  bzero(ie, sizeof(InfoElem *) * num_ie);
  generic_q93b_msg * m = 0L;
  NodeID *from = 0;
  NodeID *to = 0;
  int port = qv->get_port();
  int vc = qv->get_vci();
  int vp = qv->get_vpi();
  int crv = qv->get_crv();
  generic_q93b_msg * msg = qv->take_msg();
  assert(msg != 0);
  InfoElem ** ies = msg->get_ie_array();
  ie_end_pt_ref * epr_ie = 
    (ie_end_pt_ref *)ies[InfoElem::ie_end_pt_ref_ix];

  // Reach into the message and pull out the Called Party,
  //  use this as the Destination NodeID.
  ie_called_party_num * dest = 
    (ie_called_party_num *)msg->ie(InfoElem::ie_called_party_num_ix);

  if (dest) to = new NodeID(dest->get_addr());

  // Reach into the message and pull out the Calling Party,
  //  use this as the Source NodeID, if not present leave blank.
  ie_calling_party_num * src = 
    (ie_calling_party_num *)msg->ie(InfoElem::ie_calling_party_num_ix);

  if (src) from = new NodeID(src->get_addr());

  switch (qv->GetVT()) {
    case Q93bVisitor::setup_ind:
      // we need to make a call_proc_reqVisitor and send it down
      // this same stack
      if (vc == -1) {
	// we get to choose the new VCI
	vc = _vcpool->GetNewVCI();
      } else { // vc != -1
	if (! _vcpool->RequestVCI((unsigned int)vc)) {
	  // We have a collision
	  if (_master) // Since we're the master simply allocate another one.
	    vc = _vcpool->GetNewVCI();
	  else {
	    // if he is asking for Exclusive VCI send a
	    // ReleaseComplete with CrankBack IE with cause
	    // "no VCI available"
	  }
	}
      }
      if (vp == -1) vp = 0;
      // build the call_proceeding message
      // include an end_pt_ref IE if one was included in the setup
      if (epr_ie)
	ie[InfoElem::ie_end_pt_ref_ix] = epr_ie->copy();
      // the ie_conn_id IE is mandatory 
      ie[InfoElem::ie_conn_identifier_ix] = new ie_conn_id(vp, vc, 1, 0);
      m  = new q93b_call_proceeding_message(ie,crv,1);
      new_qv = new call_procVisitor(m, Q93bVisitor::call_proc_req);
      // DT- vpi and vci used to be set above.  new_qv is zero there.
      new_qv->set_vpi(vp);
      new_qv->set_vci(vc);
      // Added 12:13pm 1-29-98 SPM ...
      new_qv->set_crv(crv);
      new_qv->set_port(port);
      // ... Added 12:13pm 1-29-98 SPM
      PassVisitorToB(new_qv); /* Pass the Call Proceeding back to the
			       * previous switch
			       */
      //
      // We don't know the out port yet, so we specify 0.  The Control
      // port's routing department (ACAC, RouteControl, Logos) will
      // determine the out port.  We know the in port.
      //
      // We don't know the out vpvc yet, so we specify -1.  We know
      // the in vpi and vci because we chose them, above, or because
      // the other switch is master and he told us what to use.
      //
      fuv = new FastUNIVisitor(from, to, 0, -1, 0, crv,
			       FastUNIVisitor::FastUNISetup);
      fuv->SetInVP(vp);
      fuv->SetInVC(vc);
      fuv->SetInPort(port);
      break;

    case Q93bVisitor::call_proc_ind:
      fuv = new FastUNIVisitor(from, to, port, vc, 0, crv,
			     FastUNIVisitor::FastUNICallProceeding);
      fuv->SetInVP(vp);
      fuv->SetInVC(vc);
      break;
    case Q93bVisitor::connect_ind:
      fuv = new FastUNIVisitor(from, to, port, vc, 0, crv,
			     FastUNIVisitor::FastUNIConnect);
      fuv->SetInVP(vp);
      fuv->SetInVC(vc);
      fuv->SetInPort(port);
      break;
    case Q93bVisitor::release_ind: {
      // need to send a ReleaseCompReq down this stack
      new_qv = new release_compVisitor(msg->copy(), Q93bVisitor::release_comp_ind);
      new_qv->set_port(port);
      new_qv->set_crv(crv);
      PassVisitorToB(new_qv);
      // Continue translating this release into a FUV
      fuv = new FastUNIVisitor(from, to, port, vc, 0, crv,
			       FastUNIVisitor::FastUNIRelease);
      }
      break;
	    
    case Q93bVisitor::add_party_ind:
      // need to send an AddpartyAckReq down this stack
      break;
	    
    case Q93bVisitor::status_enq_ind:
    case Q93bVisitor::status_ind:
    case Q93bVisitor::add_party_comp_ind:
    case Q93bVisitor::add_party_ack_ind:
    case Q93bVisitor::add_party_rej_ind:
    case Q93bVisitor::drop_party_ind:
    case Q93bVisitor::drop_party_comp_ind:
    case Q93bVisitor::drop_party_ack_ind:
    case Q93bVisitor::leaf_setup_ind:
    case Q93bVisitor::restart_ind:
    case Q93bVisitor::restart_comp_ind:
    case Q93bVisitor::restart_ack_ind:
    case Q93bVisitor::setup_comp_ind:
    case Q93bVisitor::connect_ack_ind:
      fuv = new FastUNIVisitor(from, to, port, vc, 0, crv,
			       FastUNIVisitor::FastUNITunneling);
      break;

    case Q93bVisitor::release_comp_ind:	
    default: // Don't pass on
      fuv = 0;
      break;
  }
	
  if (fuv) {
    // Reach into the message and pull out the DTL,
    //  set the FUV with the DTL, if present.
    PNNI_designated_transit_list * dtl = (PNNI_designated_transit_list *)
      msg->ie(InfoElem::PNNI_designated_transit_list_ix);
    if (dtl) {
      list_item li;
      const list<DTLContainer *> theDTL = dtl->GetDTL();
      forall_items(li, theDTL) {
	DTLContainer * dc = theDTL.inf(li);
	fuv->Append(dc->GetNID(), dc->GetPort()); 
      }
      delete dtl;    
      InfoElem ** ies = msg->get_ie_array();
      ies[InfoElem::PNNI_designated_transit_list_ix] = 0;
    }
    // Finish up the FUV
    fuv->SetMessage(msg);
    // CONVERT QV BACK INTO A SetupReq
    qv->TurnIntoReq();
    fuv->SetTunnelingPayload(qv);
    // see above
    fuv->SetInPort(port);
    PassVisitorToA(fuv);
    // Is qv leaked here?
    // NO -- it is tunneled in fuv and will carry its journey to the final 
    // destination
  } else {
    // Cleanup
    qv->Suicide();
    delete msg;
  }
  delete from;
  delete to;
  return false;
}

// This converts FastUNIVisitors to Q93bVisitors
bool Translator::HandleFastUNIFromInside(FastUNIVisitor * fuv)
{
  generic_q93b_msg * msg = fuv->TakeMessage();
  Q93bVisitor * qv = fuv->TakeTunnelingPayload();
  int vpi = fuv->GetOutVP(), vci = fuv->GetOutVC();
  assert(msg != 0);

#if 0
  if (vpi == -1 || vci == -1) {
    // remove the IE conn ID
    InfoElem ** ies = msg->get_ie_array();
    // Shouldn't we delete it first?
    ies[InfoElem::ie_conn_identifier_ix] = 0;
  }
#endif
  if (qv && match(qv->GetVT(), fuv->GetMSGType())) {
    // Copy over the DTL from the FUV into the Q93b setup message
    PNNI_designated_transit_list * dtl = 0;
    DTLContainer * dtc = 0;
    while ((dtc = fuv->Pop())) {
      if (!dtl) dtl = new PNNI_designated_transit_list();
      dtl->AddToDTL(dtc->GetNID(), dtc->GetPort());
      delete dtc;
    }
    if (dtl) {
      InfoElem ** ies = msg->get_ie_array();
      ies[InfoElem::PNNI_designated_transit_list_ix] = dtl;
    }
    // End of DTL copy
    qv->set_msg(msg);
    qv->set_port(fuv->GetOutPort());
    qv->set_vpi(vpi);
    qv->set_vci(vci);
    // Send it on its way
    PassVisitorToB(qv);
  } else {
    switch (fuv->GetMSGType()) {
      case FastUNIVisitor::FastUNISetup:
	qv = new setupVisitor(msg, Q93bVisitor::setup_req);
	break;
      case FastUNIVisitor::FastUNIRelease:
	qv = new releaseVisitor(msg, Q93bVisitor::release_req);
	break;
      case FastUNIVisitor::FastUNIConnect:
	qv = new connectVisitor(msg, Q93bVisitor::connect_req);
	break;
      case FastUNIVisitor::FastUNICallProceeding:
	qv = new call_procVisitor(msg, Q93bVisitor::call_proc_req);
	break;
      case FastUNIVisitor::FastUNIUplinkResponse:
      case FastUNIVisitor::FastUNISetupFailure:
      case FastUNIVisitor::FastUNITunneling:
      default:
	DIAG("fsm.nni.translator", DIAG_DEBUG, cout << 
	     "Translator (" << this << ") doesn't know how "
	     "to handle a FastUNIVisitor of type " << fuv->GetMSGType() <<
	     endl);
	break;
    }
    if (qv) {
      // Copy over the DTL from the FUV into the Q93b setup message
      PNNI_designated_transit_list * dtl = 0;
      DTLContainer * dtc = 0;
      while ((dtc = fuv->Pop())) {
	if (!dtl) dtl = new PNNI_designated_transit_list();
	dtl->AddToDTL(dtc->GetNID(), dtc->GetPort());
	delete dtc;
      }
      if (dtl) {
	InfoElem ** ies = msg->get_ie_array();
	ies[InfoElem::PNNI_designated_transit_list_ix] = dtl;
      }
      // End of DTL copy
      qv->set_port(fuv->GetOutPort());
      qv->set_vpi(vpi);
      qv->set_vci(vci);
      PassVisitorToB(qv);
    } else {
      fuv->Suicide();
      delete msg;
    }
  }
  return false;
}

//----------------------------------------------------
State * Translator::Handle(Visitor * v)
{
  VisitorType inc = v->GetType();
  DIAG("fsm.nni.translator", DIAG_DEBUG, cout <<
       "*** " << OwnerName() << " (" << this << ") *** received " << 
       v->GetType();
       if (inc.Is_A(_fastunitype)) {
	 switch (((FastUNIVisitor *)v)->GetMSGType()) {
	 case FastUNIVisitor::FastUNISetup: 
	   cout << "::Setup"; break;
	 case FastUNIVisitor::FastUNICallProceeding: 
	   cout << "::CallProceeding"; break;
	 case FastUNIVisitor::FastUNIRelease:
	   cout << "::Release"; break;
	 case FastUNIVisitor::FastUNITunneling:
	   cout << "::Tunneling"; break;
	 case FastUNIVisitor::FastUNIConnect:
	   cout << "::Connect"; break;
	 case FastUNIVisitor::FastUNIUplinkResponse:
	   cout << "::UplinkResponse"; break;
	 case FastUNIVisitor::FastUNISetupFailure:
	   cout << "::SetupFailure"; break;
	 case FastUNIVisitor::RemotePGLChanged:
	   cout << "::RemotePGLChanged"; break;
	 case FastUNIVisitor::PGLChanged:
	   cout << "::PGLChanged"; break;
	 case FastUNIVisitor::NewUplink:
	   cout << "::NewUplink"; break;
	 case FastUNIVisitor::UplinkDown:
	   cout << "::UplinkDown"; break;
	 case FastUNIVisitor::UpGrade:
	   cout << "::UpGrade"; break;
	 default: break;
	 }
       }
       cout << endl);

  bool passon = true;

  switch (VisitorFrom(v)) {
    case Visitor::B_SIDE:
      // ----- Q93bVisitor -----
      if (inc.Is_A(_q93btype)) 
	passon = HandleQ93bFromOutside((Q93bVisitor *)v);
      break;

    case Visitor::A_SIDE:
      // ----- PortUpVisitor -----
      if (inc.Is_A(_portuptype))
	passon = HandlePortUpFromInside((PortUpVisitor *)v);
      // ----- PortDownVisitor -----
      else if (inc.Is_A(_portdowntype))
	_port_active = false;
      // ----- FastUNIVisitor -----
      else if (inc.Is_A(_fastunitype))
	passon = HandleFastUNIFromInside((FastUNIVisitor *)v);
      break;
      
    default:
      break;
  }
  if (passon)
    PassThru(v);

  // Always return this
  return this;
}

//----------------------------------------------------
void Translator::Interrupt(SimEvent * event)
{
  // does nothing
}

//----------------------------------------------------
bool Translator::IsPortActive(void)
{
  return _port_active;
}

bool match(Q93bVisitor::Q93bVisitorType qvt, FastUNIVisitor::uni_message fut)
{
  if ((qvt == Q93bVisitor::setup_req ||
       qvt == Q93bVisitor::setup_ind) &&
      (fut == FastUNIVisitor::FastUNISetup ||
       fut == FastUNIVisitor::FastUNIUplinkResponse))
    return true;
  if ((qvt == Q93bVisitor::connect_req ||
       qvt == Q93bVisitor::connect_ind) &&
      (fut == FastUNIVisitor::FastUNIConnect))
    return true;
  if ((qvt == Q93bVisitor::call_proc_req ||
       qvt == Q93bVisitor::call_proc_ind) &&
      (fut == FastUNIVisitor::FastUNICallProceeding))
    return true;
  if ((qvt == Q93bVisitor::release_req ||
       qvt == Q93bVisitor::release_ind) &&
      (fut == FastUNIVisitor::FastUNIRelease))
    return true;
  return false;
}
