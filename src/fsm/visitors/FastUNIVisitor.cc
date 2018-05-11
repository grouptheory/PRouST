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
static char const _FastUNIVisitor_cc_rcsid_[] =
"$Id: FastUNIVisitor.cc,v 1.2 1999/02/19 13:31:19 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "FastUNIVisitor.h"
#include <FW/basics/diag.h>
#include <FW/actors/State.h>
#include <fsm/nni/Q93bVisitors.h>
#include <fsm/visitors/QueueVisitor.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/q93b_msg/setup.h>
#include <codec/q93b_msg/connect.h>
#include <codec/q93b_msg/release.h>
#include <codec/uni_ie/PNNI_crankback.h>
#include <codec/uni_ie/UNI40_td.h>
#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/UNI40_bbc.h>
#include <codec/uni_ie/called_party_num.h>
#include <codec/uni_ie/calling_party_num.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/conn_id.h>
#include <codec/uni_ie/qos_param.h>
#include <codec/uni_ie/PNNI_designated_transit_list.h>

#include <fstream.h>

#define MAKE_VPI(vpvc) (((vpvc) & 0x00ff0000) >> 16)
#define MAKE_VCI(vpvc) ((vpvc) & 0x0000ffff)

int PrintNodeList(ostream & os, const list<DTLContainer *> *lptr)
{
  int i = 0;

  if (!lptr) return i;

  list_item li;
  for (li = lptr->first(); li; li = lptr->succ(li), i++) {
    DTLContainer * dtc = lptr->inf(li);
    os << *(dtc->GetNID()) << " " << dtc->GetPort() << endl;
  }
  return i;
}


int PrintNodeListBackwards(ostream & os, const list<DTLContainer *> *lptr)
{
  int i = 0;

  if (!lptr) return i;

  for (list_item li = lptr->last(); li; li = lptr->pred(li)) {
    i++;
    DTLContainer * dtc = lptr->inf(li);
    os << *(dtc->GetNID()) << " " << dtc->GetPort() << endl;
  }
  return i;
}


FastUNIVisitor::~FastUNIVisitor() 
{ 
  while (!_dtl.empty())
    delete _dtl.pop();
  
  while (!_hops.empty())
    delete _hops.pop();

  if (_lst != 0) {
    while (! _lst->empty())
      delete _lst->pop();
    delete _lst;
  }

  if (_border)
    delete _border;

  if (_dtl_filename != 0)
    delete [] _dtl_filename;

  if (_message)
    delete _message;
}

void FastUNIVisitor::on_death(void) const
{
  DIAG("fsm.visitors..dumplog", DIAG_INFO, 
       if (DumpLog(cout)) cout << endl; 
      );

  DIAG("fsm.visitors", DIAG_INFO,
       if (_dtl_filename != 0) {
	 if ( strlen(_dtl_filename) == 0 )
	   cout << *this;
	 else {
	   ofstream os(_dtl_filename);
	   os << *this;
	   os.close();
	 }
       }
      );
}

// Constructor
FastUNIVisitor::FastUNIVisitor(const NodeID * Source, const NodeID * Destination,
			       int port, int vpvc, int agg,
			       int cref, uni_message type,
			       char * dtl_filename,
			       u_long bandwidth)
  : VPVCVisitor(_my_type, port, agg, vpvc, Source, Destination), 
    _message_type(type), _lst(0), _bandwidth(bandwidth),
    _border(0), _border_port(-1), _state(0), _uplink(0), 
    _dtl_filename(0), _message(0), _tunneling_payload(0),
    _is_for_svcc(false), _lp(-1)
{
  //  _my_type.derived_from(QueueOverrideType());
  _message_type = type;

  if (dtl_filename != 0) {
    _dtl_filename = new char [ strlen(dtl_filename) + 1 ];
    strcpy(_dtl_filename, dtl_filename);
  }
  BuildTheQ93bMessage(_message_type, cref);
}

FastUNIVisitor::FastUNIVisitor(const FastUNIVisitor & rhs) 
  : VPVCVisitor(rhs), _message_type(rhs._message_type), 
    _border(0), _border_port(rhs._border_port), 
    _state(rhs._state), _uplink(0), _dtl_filename(0), 
    _message(0), _lst(0), _tunneling_payload(0),
    _is_for_svcc(false), _lp(rhs._lp)
{ 
   if (rhs._dtl_filename != 0) _dtl_filename = strdup(rhs._dtl_filename);
   if (rhs._message != 0)
     _message = rhs._message->copy();

   if (rhs._border)
     _border = rhs._border->copy();

   if (rhs._lst != 0) {
     _lst = new list<PNNI_crankback *>;
     list_item li;
     forall_items(li, *(rhs._lst)) {
       _lst->append(rhs._lst->inf(li)->copy());
     }
   }
}

Visitor * FastUNIVisitor::dup(void) const
{
  return new FastUNIVisitor(*this);
}

FastUNIVisitor & FastUNIVisitor::operator = (const FastUNIVisitor & rhs)
{
  _message_type = rhs._message_type;

  if (_border)
    delete _border;
  if (rhs._border)
    _border = rhs._border->copy();
  else
    _border = 0;

  _border_port = rhs._border_port;

  if (rhs._state)
    _state = rhs._state;
  else
    _state = 0;

  if (_uplink)
    delete _uplink;
  if (rhs._uplink)
    _uplink = rhs._uplink;
  else
    _uplink = 0;

  if (_dtl_filename)
    delete [] _dtl_filename;
  if (rhs._dtl_filename) {
    _dtl_filename = new char [ strlen(rhs._dtl_filename) + 1 ];
    strcpy(rhs._dtl_filename, _dtl_filename);
  } else
    _dtl_filename = 0;
  
  list_item li;
  while (!_dtl.empty())
    delete _dtl.pop();
  
  forall_items(li, rhs._dtl) {
    _dtl.append(rhs._dtl.inf(li)->copy());
  }

  while (!_hops.empty())
    delete _hops.pop();

  forall_items(li, rhs._hops) {
    _hops.append(rhs._hops.inf(li)->copy());
  }

  if ( _lst != 0 ) {
    while (! _lst->empty())
      delete _lst->pop();
    delete _lst;
    _lst = 0;
  }

  if (rhs._lst) {
    if ( ! _lst ) _lst = new list<PNNI_crankback *>;
    forall_items(li, *rhs._lst) {
      _lst->append(rhs._lst->inf(li)->copy());
    }
  }

  if (_message)
    delete _message;
  if (rhs._message)
    _message = rhs._message->copy();
  else
    _message = 0;

  _lp = rhs._lp;

  return (FastUNIVisitor &)(*(VPVCVisitor *)this = (VPVCVisitor &)rhs);
}


// --- DTL methods ---
const DTLContainer * FastUNIVisitor::Peek(void) const
{
  DTLContainer * rval = 0;
  list_item li;
  if (li = _dtl.first())
    rval = _dtl.inf(li);
  return rval;
}

DTLContainer * FastUNIVisitor::Pop(void)
{
  DTLContainer * rval = 0;

  if (!_dtl.empty()) {
    rval = _dtl.pop();
    if (rval != 0)  // this will no longer work since we cannot carry it across switches
      _hops.push(rval->copy());
  }
  return rval;
}

void FastUNIVisitor::Push(const NodeID * nid, int port)
{ _dtl.push(new DTLContainer(nid, port)); }

void FastUNIVisitor::Append(const NodeID * nid, int port)
{ _dtl.append(new DTLContainer(nid, port)); }

void FastUNIVisitor::Push(const list<NodeID *> * lptr)
{
  for (list_item li = lptr->last(); li; li = lptr->pred(li))
    _dtl.push(new DTLContainer( lptr->inf(li) ));
}

void FastUNIVisitor::Push(const list<DTLContainer *> * lptr)
{
  for (list_item li = lptr->last(); li; li = lptr->pred(li))
    _dtl.push( lptr->inf(li) );
}

void FastUNIVisitor::ClearDTL(void)
{
  while (!_dtl.empty())
    delete _dtl.pop();
}
// --- End of DTL methods ---

NodeID * FastUNIVisitor::GetBorder(void) const     { return _border; }
State  * FastUNIVisitor::GetState(void) const      { return _state; }
int      FastUNIVisitor::GetBorderPort(void) const { return _border_port; }

void     FastUNIVisitor::SetBorder(NodeID * nid) { _border = nid; }
void     FastUNIVisitor::SetBorderPort(int port) { _border_port = port; }
void     FastUNIVisitor::SetState(State * state) { _state = state; }

//
// Change the type of the FastUNIVisitor.  Delete the
// _tunneling_payload Q93bVisitor.  This prevents confusion in
// Forwarder and other submodules that give priority to
// _tunneling_payload over _message_type.
//
void     FastUNIVisitor::SetMSGType(FastUNIVisitor::uni_message msg) 
{ 
  _message_type = msg;
  unsigned int cref = (_message ? _message->get_crv() 
		       : (unsigned int)-1);

  Q93bVisitor * tp = TakeTunnelingPayload();
  if (tp != 0) tp->Suicide();

  // This will delete _message
  BuildTheQ93bMessage(msg, cref);
}

FastUNIVisitor::uni_message FastUNIVisitor::GetMSGType(void) const 
{
  return _message_type; 
}

const char * FastUNIVisitor::PrintMSGType(void) const
{
  const char * rval = 0;
  switch (_message_type) {
    case FastUNITunneling:
      rval = "Tunneling";
      break;
    case FastUNISetup:
      rval = "Setup";
      break;
    case FastUNICallProceeding:
      rval = "CallProceeding";
      break;
    case FastUNIRelease:
      rval = "Release";
      break;
    case FastUNIConnect:
      rval = "Connect";
      break;
    case FastUNILHI:
      rval = "LgnHelloInstantiator";
      break;
    case FastUNILHD:
      rval = "LgnHelloDestructor";
      break;
    case FastUNILHIRebinder:
      rval = "LgnHelloRebinder";
      break;
    case FastUNISetupFailure:
      rval = "SetupFailure";
      break;
    case FastUNIUplinkResponse:
      rval = "UplinkResponse";
      break;
    case RemotePGLChanged:
      rval = "RemotePGLChanged";
      break;
    case PGLChanged:
      rval = "PGLChanged";
      break;
    case NewUplink:
      rval = "NewUplink";
      break;
    case UplinkDown:
      rval = "UplinkDown";
      break;
    case UpGrade:
      rval = "Upgrade";
      break;
    case Unknown:
    default:
      rval = "Unknown";
      break;
  }
  return rval;
}

const VisitorType FastUNIVisitor::GetType(void) const
{
  return VisitorType(GetClassType()); 
}

ig_uplinks * FastUNIVisitor::GetULIG(void) const
{
  return _uplink;
}

void         FastUNIVisitor::SetULIG(ig_uplinks * ulig)
{
  _uplink = ulig;
}

q93b_setup_message * FastUNIVisitor::GetSetup(void) const
{
  if (_message_type == FastUNISetup ||
      _message_type == FastUNIUplinkResponse)
    return (q93b_setup_message *)_message;
  return 0;
}

// After calling this you MUST remeber to change _message_type to either
//  FastUNISetup or FastUNIUplinkResponse !!
void FastUNIVisitor::SetSetup(q93b_setup_message * msg) 
{
  //
  // Don't delete _message if it points to the same address as msg.
  // That would be bad.  msg would become a dangling pointer.
  //
  if (_message && msg && _message != msg) {
    delete _message;
    _message = msg;
  }
  // Don't allow the message to be set to 0
}

// After calling this you MUST remeber to change _message_type to either
//  FastUNISetup or FastUNIUplinkResponse !!
void FastUNIVisitor::SetSetup(u_long bandwidth, int flags) 
{
  Addr *tempAddr = 0;
  _bandwidth = bandwidth;
  InfoElem * ie_array[num_ie];
  for (int i = 0; i < num_ie; i++) ie_array[i] = 0;

  // the 1st four mandatory IE's don't exist in our implementation ...
  ie_array[InfoElem::ie_traffic_desc_ix] = new UNI40_traffic_desc();
  ((UNI40_traffic_desc *)
   ie_array[InfoElem::ie_traffic_desc_ix])->set_BE(_bandwidth, _bandwidth);

  // next BBC A
  ie_array[InfoElem::ie_broadband_bearer_cap_ix] = 
    new ie_bbc(ie_bbc::BCOB_A, ie_bbc::not_clipped, ie_bbc::p2p, ie_bbc::ATC_Absent);
    // A_Bbc(ie_broadband_bearer_cap::not_clipped, 
    //       ie_broadband_bearer_cap::p2p);

  // next Called Party number
  tempAddr = GetDestNID()->GetAddr();
  ie_array[InfoElem::ie_called_party_num_ix] =
    new ie_called_party_num(tempAddr);

  delete tempAddr;

  // next Calling Party number
  tempAddr = GetSourceNID()->GetAddr();
  ie_array[InfoElem::ie_calling_party_num_ix] = 
    new ie_calling_party_num(tempAddr);

  delete tempAddr;

  // next Connection Identifier
  ie_array[InfoElem::ie_conn_identifier_ix] =
    new ie_conn_id(GetOutVP(), GetOutVC());

  // next QoS Parameter
  ie_array[InfoElem::ie_qos_param_ix] = 
    new ie_qos_param(ie_qos_param::qos0, ie_qos_param::qos0);

  u_long cref = _message->get_crv();
  delete _message;

  _message = new q93b_setup_message(ie_array, cref, flags);
}

q93b_release_message * FastUNIVisitor::GetRelease(void) const
{
  if (_message_type == FastUNIRelease ||
      _message_type == FastUNISetupFailure)
    return (q93b_release_message *)_message;
  return 0;
}

// After calling this you MUST remeber to change _message_type to either
//  FastUNISetupFailure or FastUNIRelease !!
void FastUNIVisitor::SetRelease(q93b_release_message * msg)
{
  //
  // Don't delete _message if it points to the same address as msg.
  // That would be bad.  msg would become a dangling pointer.
  //
  if (_message != msg)
    delete _message;
  _message = msg;
}

// After calling this you MUST remeber to change _message_type to either
//  FastUNISetupFailure or FastUNIRelease !!
void FastUNIVisitor::SetRelease(const ie_cause * c, 
				const PNNI_crankback * crank, 
				int flags)
{
  ie_cause * cause = GetCause();
  PNNI_crankback * pc = GetCrankback();

  InfoElem * ie_array[num_ie];
  bzero(ie_array, sizeof(InfoElem *) * num_ie);
  // for (int i = 0; i < num_ie; i++) ie_array[i] = 0;

  //
  // If we already have a cause, don't put the same object
  // into ie_array.  If we did that, we'd have a dangling
  // pointer.
  //
  if (cause != c)
    delete ie_array[InfoElem::ie_cause_ix];
  ie_array[InfoElem::ie_cause_ix] = (InfoElem *)c;

  if (pc != crank)
    delete ie_array[InfoElem::PNNI_crankback_ix];
  ie_array[InfoElem::PNNI_crankback_ix] = (InfoElem *)crank;

  u_long cref = (u_long)-1;
  if (_message != 0)
    cref = _message->get_crv();

  delete _message;

  _message = new q93b_release_message(ie_array, cref, flags);
}

// You'd best not delete this on me!
PNNI_crankback * FastUNIVisitor::GetCrankback(void) const
{
  if (_message && (_message_type == FastUNIRelease ||
		   _message_type == FastUNISetupFailure)) {
    return (PNNI_crankback *)_message->ie(InfoElem::PNNI_crankback_ix);
  }
  return 0;
}

//
// FastUNIVisitor thinks that it owns the crankback.
//
void FastUNIVisitor::SetCrankback(const PNNI_crankback * crank)
{
  if (_message_type == FastUNIRelease ||
      _message_type == FastUNISetupFailure) {
    ie_cause * cause = GetCause();
    ie_cause * new_cause = 0;

    //
    // Send a copy of cause to SetRelease() because cause is just a
    // pointer to an IE in _message and SetRelease() deletes _message
    // before creating a new one.
    //
    if (cause != 0) new_cause = (ie_cause *)(cause->copy());

    SetRelease(new_cause, crank);

    // delete _crankback;
    // _crankback = crank;
  }
}


ie_cause * FastUNIVisitor::GetCause(void) const
{
  if (_message && (_message_type == FastUNIRelease ||
		   _message_type == FastUNISetupFailure)) {
    return (ie_cause *)_message->ie(InfoElem::ie_cause_ix);
    // return _cause;
  }
  return 0;
}


//
// FastUNIVisitor thinks that it owns the cause.
//
void FastUNIVisitor::SetCause(const ie_cause * cause)
{
  if (_message_type == FastUNIRelease ||
      _message_type == FastUNISetupFailure) {
    PNNI_crankback * crank = GetCrankback();

    SetRelease(cause, crank);
    // delete _cause;
    // _cause = cause;
  }
}


list<PNNI_crankback *> * FastUNIVisitor::GetCrankbackList(void)
{
  list<PNNI_crankback *> * rval = _lst;
  _lst = 0;
  return rval;
}

void FastUNIVisitor::SetCrankbackList(const list<PNNI_crankback *> * lst)
{
  list_item li; 
  if (lst != 0 && lst->empty() == false) {
    if ( _lst != 0 ) {
      while ( _lst->empty() == false )
	delete _lst->pop();
      _lst->clear();
    } else
      _lst = new list<PNNI_crankback *>;

    forall_items(li, *lst) {
      PNNI_crankback * pc = lst->inf(li);
      // Copy all the items out of lst
      _lst->append(pc->copy());
    }
  }
}

ostream & operator << (ostream & os, const FastUNIVisitor & fuv)
{
  os << fuv.GetType() << ": ";
  switch (fuv._message_type) {
  case FastUNIVisitor::Unknown: 
    os << "Unknown";
    break;

  case FastUNIVisitor::FastUNITunneling:
    os << "Tunneling";
    break;

  case FastUNIVisitor::FastUNISetup: 
    os << "Setup";
    break;

  case FastUNIVisitor::FastUNICallProceeding:
    os << "CallProceeding";
    break;

  case FastUNIVisitor::FastUNIRelease: 
    os << "Release";
    break;

  case FastUNIVisitor::FastUNIConnect: 
    os << "Connect";
    break;

  case FastUNIVisitor::FastUNIUplinkResponse: 
    os << "UplinkResponse";
    break;

  case FastUNIVisitor::FastUNISetupFailure: 
    os << "SetupFailure";
    break;

  case FastUNIVisitor::RemotePGLChanged:
    os << "RemotePGLChanged";
    break;

  case FastUNIVisitor::PGLChanged:
    os << "PGLChanged";
    break;

  case FastUNIVisitor::NewUplink:
    os << "NewUplink";
    break;

  case FastUNIVisitor::UplinkDown:
    os << "UplinkDown";
    break;

  case FastUNIVisitor::UpGrade:
    os << "Upgrade";
    break;

  default:
    os << "Something new " << (int)(fuv._message_type) 
       << " (0x" << hex << (int)(fuv._message_type) << ")";
  }

  os << endl;
#ifdef LATER
  os << "Message: ";
  if (fuv._message)
    os << *(fuv._message) << endl;
  else
    os << "(Null)" << endl;
#endif
  os << "========================================" << endl;

  os << "Source:      ";
  if (fuv.GetSourceNID() != 0) os << *(fuv.GetSourceNID()) << endl;
  else os << "0" << endl;

  os << "Destination: " ;
  if (fuv.GetDestNID() != 0) cout << *(fuv.GetDestNID()) << endl;
  else os << "0" << endl;

  os << "Visited these nodes:" << endl;
  if (!PrintNodeListBackwards(os, &(fuv._hops)))
    os << "  Didn't visit any nodes yet." << endl;
  os << "++++++++++++++++++++++++++++++++++++++++" << endl
     << "Nodes remaining on DTL:" << endl;
  if (!PrintNodeList(os, &(fuv._dtl)))
    os << "  No nodes remain on the DTL." << endl;
  os << "========================================" << endl;
  return os;
}

void FastUNIVisitor::PrintDTL(ostream & os) const
{
  PrintNodeList(os, &_dtl);
}

bool FastUNIVisitor::SetTunnelingPayload(Q93bVisitor*& v)
{
  _tunneling_payload=v;
  v=0;
  return true;
}

Q93bVisitor* FastUNIVisitor::TakeTunnelingPayload(void)
{
  Q93bVisitor * ans = _tunneling_payload;
  _tunneling_payload = 0;
  return ans;
}


const generic_q93b_msg * FastUNIVisitor::GetSharedMessage(void) const
{
  return _message;
}

generic_q93b_msg* FastUNIVisitor::TakeMessage(void)
{
  generic_q93b_msg * ans = _message;
  _message = 0;
  return ans;
}

bool FastUNIVisitor::SetMessage(generic_q93b_msg *& msg)
{
  // If you think this should be uncommented speak with Bilal
  if (_message) delete _message;

  _message = msg;
  msg = 0;

  return true;
}

void   FastUNIVisitor::SetCREF(u_long cref) 
{ 
  if (_message) _message->set_crv(cref); 
}

u_long FastUNIVisitor::GetCREF(void) const 
{
  if (_message) 
    return _message->get_crv(); 

  // else we die
  diag("fsm.visitors", DIAG_FATAL, 
       "Request for CREF from FastUNIVisitor %x which doesn't have a message!\n", this);
  // we won't reach this point, don't worry
  return 0;
}

bool FastUNIVisitor::IsForSVCC(void) const { return _is_for_svcc; }

void FastUNIVisitor::SetForSVCC(bool s) { _is_for_svcc = s; }

void FastUNIVisitor::BuildTheQ93bMessage(uni_message type, u_long cref)
{
  InfoElem * ie_array[num_ie];
  for (int i = 0; i < num_ie; i++) 
    ie_array[i] = 0;

  if (_message)
    delete _message, _message = 0;

  switch (type) {
    case FastUNIConnect:
      ie_array[InfoElem::ie_conn_identifier_ix] = 
	new ie_conn_id(_out_vp, _out_vc);
      _message = new q93b_connect_message(ie_array, cref, 0);
      break;

    case FastUNICallProceeding:
      diag("fsm.visitors.fastuni", DIAG_DEBUG, 
	   "Automated generation of the Q93b call proceeding message hasn't been written yet.\n");
      break;

    case FastUNIRelease:
      ie_array[InfoElem::ie_cause_ix] = 
	new ie_cause(ie_cause::normal_unspecified, ie_cause::user);

      // crankback is optional
      //      ie_array[PNNI_crankback_ix] = 0;

      // notification indicator and progress indicator are mandatory
      //      ie_array[UNI40_notification_id_ix] = 0;
      //      ie_array[progress_indicator_ix] = 0;
      _message = new q93b_release_message(ie_array, cref, 0);
      break;

    case FastUNISetupFailure:
      diag("fsm.visitors.fastuni", DIAG_DEBUG, 
	   "Automated generation of the Q93b setup failure message hasn't been written yet.\n");
      break;

    case FastUNISetup:
      assert(GetDestNID());
      // the 1st four mandatory IE's don't exist in our implementation ...
      ie_array[InfoElem::ie_traffic_desc_ix] = new UNI40_traffic_desc();
      ((UNI40_traffic_desc *)
       ie_array[InfoElem::ie_traffic_desc_ix])->set_BE(_bandwidth, _bandwidth);
      // next BBC C
      ie_array[InfoElem::ie_broadband_bearer_cap_ix] = 
	new ie_bbc(ie_bbc::BCOB_C, ie_bbc::not_clipped, ie_bbc::p2p, ie_bbc::ATC_Absent);
	//	new C_Bbc(ie_broadband_bearer_cap::not_clipped, 
	//		  ie_broadband_bearer_cap::p2p);
      // next Called Party number
      { 
	Addr * tempAddr = GetDestNID()->GetAddr();
	ie_array[InfoElem::ie_called_party_num_ix] = 
	  new ie_called_party_num(tempAddr);
	delete tempAddr;
      }
      if (GetSourceNID()) {
	// next Calling Party number
	Addr * tempAddr = GetSourceNID()->GetAddr();
	ie_array[InfoElem::ie_calling_party_num_ix] = 
	  new ie_calling_party_num(tempAddr);
	delete tempAddr;
      }
      // next Connection Identifier
      ie_array[InfoElem::ie_conn_identifier_ix] = 
	new ie_conn_id(_out_vp, _out_vc);
      // next QoS Parameter
      ie_array[InfoElem::ie_qos_param_ix] = 
	new ie_qos_param(ie_qos_param::qos0, ie_qos_param::qos0);
      _message = new q93b_setup_message(ie_array, cref, 0);
      break;

    default:
      // Since we deleted it at the beginning we must reset it
      _message = 0;
      break;
  }
}

void FastUNIVisitor::SetLogicalPort(int lp)
{  _lp = lp;  }

int  FastUNIVisitor::GetLogicalPort(void) const
{  return _lp;  }

void FastUNIVisitor::Fill_Subtype_Name(char * buf) const 
{
  sprintf(buf, "%s", PrintMSGType());
}
