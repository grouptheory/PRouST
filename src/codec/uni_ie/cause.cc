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
static char const _cause_cc_rcsid_[] =
"$Id: cause.cc,v 1.10 1998/08/06 04:03:16 bilal Exp $";
#endif
#include <common/cprototypes.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>
#include <codec/uni_ie/broadband_repeat_ind.h>

/*
 * rc: refrence to a ie_cause
 */
ie_cause::ie_cause(const ie_cause & rc) : InfoElem(ie_cause_id),
   _location(rc._location),
   _cause_value(rc._cause_value),
   _diagnostic_len(rc._diagnostic_len)
{
  // allocate diagnostics and copy it
  _diagnostic_info = new u_char[_diagnostic_len];
  bcopy((char *)rc._diagnostic_info,(char *) _diagnostic_info,_diagnostic_len);
}

ie_cause::ie_cause(ie_cause::CauseValues cv, Location loc) :
  InfoElem(ie_cause_id),
  _cause_value(cv),
  _location(loc),
  _diagnostic_info(0),
  _diagnostic_len(0),
  _num_diagnostics(1) {}

ie_cause::ie_cause(CauseValues cv, Location loc, int len, u_char * d) :
  InfoElem(ie_cause_id),
  _cause_value(cv),
  _location(loc),
  _diagnostic_info(d),
  _diagnostic_len(len),
  _num_diagnostics(1) {}

/*
 * clean list of ie_causes
 */
ie_cause::~ie_cause()
{
  delete [] _diagnostic_info; _diagnostic_info=0L;
}

InfoElem* ie_cause::copy(void) const
{
  u_char *temp = 0;

  if (_diagnostic_len) {
    temp = new u_char[_diagnostic_len];
    bcopy((char *)_diagnostic_info,(char *)temp,_diagnostic_len);
  }
  return (new ie_cause(_cause_value,_location,_diagnostic_len,temp));
}


int ie_cause::equals(const InfoElem* him) const
{
  return (him->equals(this));
}

int ie_cause::equals(const ie_cause* himptr) const
{
  ie_cause him = *himptr;

  if (_cause_value != him._cause_value)
    return(0);
  /* here everything has to match */
  if (_cause_value == recovery_on_timer_expiry) {
    if (_diagnostic_len != him._diagnostic_len)
      return(0);
    for (int i = 0; i < _diagnostic_len; i++)
      if (_diagnostic_info[i] != him._diagnostic_info[i])
	return(0);
  }
  return(1);
}

/*
 * Need to add check for ie_cause list and diagnostic_info
 *
 */
int ie_cause::operator == (const ie_cause & cr) const
{
  return equals(&cr);
}

int ie_cause::Length()
{
  ie_cause * cptr = this;
  int buflen = 0;
  int len = 0; // keeps total length same as buflen if 1 IE
  u_char *hptr;

  if (LookupCache())
    return (CacheLength());
  int i  = NumInList();
  if(IsFirstOfMany())
    {
      ie_broadband_repeat_ind bri;
      len = bri.Length();
  }
  while (i-- > 0)
    {
      buflen = ie_header_len;
      // location and cause value
      buflen += 2;
      // diagnostics if any
      if (cptr->_diagnostic_len)
	buflen += cptr->_diagnostic_len;
      len += buflen;
      cptr = (ie_cause *)cptr->get_next_ie();
    }
  return (len);
}



int ie_cause::encode(u_char * buffer)
{
  u_char * temp;
  ie_cause * cptr = this;
  int buflen = 0;
  int len = 0; // keeps total length same as buflen if 1 IE
  u_char *hptr;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  int i  = NumInList();
  temp = buffer;
  // Encodes itself and all appended causes
  if (IsFirstOfMany()) {
    ie_broadband_repeat_ind bri;
    temp += (buflen = bri.encode(temp));
  }
  len += buflen;
  while (i-- > 0) {
    buflen = 0;
    hptr = temp; // header starts here
    put_id(hptr,_id);
    // see note 5 top of page 230 and reset coding standard appropriately
    if(cptr && (cptr->_cause_value == user_rejects_all_calls))
      hptr[1] = 0xe0;
    else
      put_coding(hptr,_coding);
    temp += ie_header_len; // body starts here
    // encode location and cause value
    put_8(temp,buflen,(0x80 | cptr->_location));
    put_8(temp,buflen,(0x80 | cptr->_cause_value));
    // encode diagnostics if any
    if (cptr->_diagnostic_len) {
      bcopy((char *)cptr->_diagnostic_info,(char *)temp, cptr->_diagnostic_len);
      temp += cptr->_diagnostic_len;
      buflen += cptr->_diagnostic_len;
    }
    put_len(hptr,buflen);
    buflen += ie_header_len;
    len += buflen;
    cptr = (ie_cause *)cptr->get_next_ie();
  }
  FillCache(buffer,len);
  return (len);
}

/*
 * The fixed part of this IE has length (4+2) = 6.
 * The max. total length is 6 + MAX_DIAGNOSTIC_LEN
 */

InfoElem::ie_status ie_cause::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  int len = get_len(buffer), coding;

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len < 2 || len > MAX_DIAGNOSTIC_LEN + 2)
    return (InfoElem::incomplete);
  if ((coding = buffer[1] & 0x60) != InfoElem::itu)
    return (InfoElem::invalid_contents);
  buffer += ie_header_len;
  if ((*buffer & 0x80) != 0x80)
    return (InfoElem::invalid_contents);

  // extract location and check it
  Location loc = (ie_cause::Location)(*buffer & 0x8f);
  switch(loc) {
  case user:
  case local_private_network:
  case local_public_network:
  case transit_network:
  case remote_private_network:
  case remote_public_network:
  case international_network:
  case network_beyond_interworking_pt:
    _location = (ie_cause::Location) loc;
    break;
  default:
    return (InfoElem::invalid_contents);
    break;
  }
  buffer++;
  len--;
  // check bit8 of cause value
  if ((*buffer & 0x80) != 0x80)
    return (InfoElem::invalid_contents);

  // extract cause value and check it
  int cv = *buffer++ & 0x7f;
  switch(cv) {
    case unallocated_address:
    case no_route_to_specified_transit_network:
    case no_route_to_destination:
    case normal_call_clearing:
    case user_busy:
    case no_user_responding:
    case call_rejected:
    case address_changed:
    case user_rejects_all_calls:
    case destination_out_of_order:
    case invalid_address_format:
    case response_to_status_enquiry:
    case normal_unspecified:
    case requested_VPCI_VCI_unavailable:
    case VPCI_VCI_assignment_failure:
    case user_cell_rate_unavailable:
    case network_out_of_order:
    case temporary_failure:
    case access_information_discarded:
    case no_VPCI_VCI_available:
    case resource_unavailable_unspecified:
    case qos_unavailable:
    case peer_group_leader_changed:
    case bearer_capability_not_authorized:
    case bearer_capability_not_presently_available:
    case service_or_option_not_unavailable_unspecified:
    case bearer_capability_not_implemented:
    case unsupported_combination_of_traffic_parameters:
    case aal_parameters_cannot_be_supported:
    case invalid_call_reference_value:
    case identified_channel_doesnt_exist:
    case incompatible_destination:
    case invalid_endpoint_reference:
    case invalid_transit_network_selection:
    case too_many_pending_add_party_requests:
    case mandatory_information_element_is_missing:
    case message_type_nonexistent_or_not_implemented:
    case information_element_nonexistent_or_not_implemented:
    case invalid_information_element_contents:
    case message_not_compatible_with_call_state:
    case recovery_on_timer_expiry:
    case incorrect_message_length:
    case protocol_error_unspecified:
      _cause_value = (ie_cause::CauseValues)cv;
    break;
  default:
    return (InfoElem::invalid_contents);
  }
  len--;
  if ((_cause_value == user_rejects_all_calls) && coding != InfoElem::net) {
    return (InfoElem::invalid_contents);
  }
  // got diagnostics
  if(len)
    {
      _diagnostic_len = len ;
      if (_diagnostic_info)
	{
	  delete _diagnostic_info;
	  _diagnostic_info=0L;
	}
      _diagnostic_info = new u_char[_diagnostic_len];
      bcopy((char *)buffer, (char *)_diagnostic_info, _diagnostic_len);
    }
  return (InfoElem::ok);
}


u_char ie_cause::IsVariableLen(void)
{
  return ((u_char)1);
}

enum ie_cause::Location ie_cause::get_location(void) {return _location;}

void ie_cause::set_location(Location loc) 
{
  _location = loc;
  MakeStale();
}
  
enum ie_cause::CauseValues ie_cause::get_cause_value(void) 
{
  return _cause_value;
}

void ie_cause::set_cause_value(CauseValues cv) 
{
  _cause_value = cv;
  MakeStale();
}

// max of two causes in list
int ie_cause::max_repeats() { return 2;}

char ie_cause::PrintSpecific(ostream & os)
{
  os << endl << "   location::";
  switch (_location) { 
  case user: os << "user "<< endl; break;
  case local_private_network:  os << "local private network "<< endl;  break;
  case local_public_network:   os << "local public network "<< endl;   break;
  case transit_network:        os << "transit network "<< endl;        break;
  case remote_private_network: os << "remote private network "<< endl; break;
  case remote_public_network:  os << "remove public network "<< endl;  break;
  case international_network:  os << "international network " << endl; break;
  case network_beyond_interworking_pt: 
    os << "network beyond interworking point "<< endl; break;
  default: os << "unknown "<< endl; break;
  }
  os << "   cause value::";
  switch (_cause_value) {
  case unallocated_address: 
    os << "unallocated address "<< endl; break;
  case no_route_to_specified_transit_network: 
    os << "no route to specified transit network "<< endl; break;
  case no_route_to_destination: 
    os << "no route to destination "<< endl; break;
  case normal_call_clearing: 
    os << "normal call clearing "<< endl; break;
  case user_busy: 
    os << "user busy "<< endl; break;
  case no_user_responding: 
    os << "no user responding "<< endl; break;
  case call_rejected: 
    os << "call rejected "<< endl; break;
  case address_changed: 
    os << "address changed "<< endl; break;
  case user_rejects_all_calls: 
    os << "user rejects all calls "<< endl; break;
  case destination_out_of_order: 
    os << "destination out of order "<< endl; break;
  case invalid_address_format: 
    os << "invalid address format "<< endl; break;
  case response_to_status_enquiry: 
    os << "response to status enquiry "<< endl; break;
  case normal_unspecified: 
    os << "normal unspecified "<< endl; break;
  case requested_VPCI_VCI_unavailable: 
    os << "requested VPCI VCI unavailable "<< endl; break;
  case VPCI_VCI_assignment_failure: 
    os << "VPCI/VCI assignment failure "<< endl; break;
  case user_cell_rate_unavailable: 
    os << "user cell rate unavailable "<< endl; break;
  case network_out_of_order: 
    os << "network out of order "<< endl; break;
  case temporary_failure: 
    os << "temporary failure "<< endl; break;
  case access_information_discarded: 
    os << "access information discarded "<< endl; break;
  case no_VPCI_VCI_available: 
    os << "no VPCI/VCI available "<< endl; break;
  case resource_unavailable_unspecified: 
    os << "resource unavailable/unsupported "<< endl; break;
  case qos_unavailable: 
    os << "QOS unavailable "<< endl; break;
  case peer_group_leader_changed:
    os << "Peer Group Leader Changed" << endl; break;
  case bearer_capability_not_authorized: 
    os << "bearer capability not authorized "<< endl; break;
  case bearer_capability_not_presently_available: 
    os << "bearer capability not presently available "<< endl; break;
  case service_or_option_not_unavailable_unspecified: 
    os << "service or option not unavailable/unspecified "<< endl; break;
  case bearer_capability_not_implemented: 
    os << "bearer capability not implemented "<< endl; break;
  case unsupported_combination_of_traffic_parameters: 
    os << "unsupported combination of traffic parameters "<< endl; break;
  case aal_parameters_cannot_be_supported: 
    os << "aal parameters cannot be supported "<< endl; break;
  case invalid_call_reference_value: 
    os << "invalid call reference value "<< endl; break;
  case identified_channel_doesnt_exist: 
    os << "identified channel doesn't exist "<< endl; break;
  case incompatible_destination: 
    os << "incompatible destination "<< endl; break;
  case invalid_endpoint_reference: 
    os << "invalid endpoint reference "<< endl; break;
  case invalid_transit_network_selection: 
    os << "invalid transit network selection "<< endl; break;
  case too_many_pending_add_party_requests: 
    os << "too many pending add party requests "<< endl; break;
  case mandatory_information_element_is_missing: 
    os << "mandatory information element is missing "<< endl; break;
  case message_type_nonexistent_or_not_implemented: 
    os << "message type nonexistent or not implemented "<< endl; break;
  case information_element_nonexistent_or_not_implemented: 
    os << "information element nonexistent or not implemented "<< endl; break;
  case invalid_information_element_contents: 
    os << "invalid information element contents "<< endl; break;
  case message_not_compatible_with_call_state: 
    os << "message not compatible with call state "<< endl; break;
  case recovery_on_timer_expiry: 
    os << "recovery on timer expiry "<< endl; break;
  case incorrect_message_length: 
    os << "incorrect message length "<< endl; break;
  case protocol_error_unspecified: 
    os << "protocol error unspecified "<< endl; break;
  default: os << "unknown "<< endl; break;
  }
  os << "   num diagnostics::" << _num_diagnostics << endl;

  if (_diagnostic_len > 0) {
    os << "   diagnostic info::0x";
    for (int i = 0; i < _diagnostic_len; i++) {
      int val = (0xFF & (int) _diagnostic_info[i]);
      if (val < 16) os << "0";
      os << hex << val << dec;
    }
    os << endl;
  }

  return 0;
}

ostream & operator << (ostream & os, ie_cause & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}


