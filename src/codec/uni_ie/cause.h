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

#ifndef _IE_CAUSE_H_
#define _IE_CAUSE_H_
#if !defined(LINT)
static char const _cause_h_rcsid_[] =
"$Id: cause.h,v 1.8 1998/04/08 15:01:40 mountcas Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

/** see 5.4.5.15 page 225 in UNI-3.1 book
   The ie_cause information element gives the reason for generating certain
   error  messages, and gives diagnostic information
    (u_char *diagnostic_info) for
   procedural errors and gives the location (see Location values) of the
   originator.
   Both ie_cause and diagnostic_info can be repeated respectively (num_causes,
   num_diagnostics).
 */

class ie_cause : public InfoElem {
  friend ostream & operator << (ostream & os, ie_cause & x);

public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const ie_cause* himptr) const;

  enum CauseValues {
    unspecified_or_unknow                 = 0,
    unallocated_address                   = 1,
    no_route_to_specified_transit_network = 2,
    no_route_to_destination               = 3,
    normal_call_clearing                  = 16,
    user_busy                             = 17,
    no_user_responding                    = 18,
    call_rejected                         = 21,
    address_changed                       = 22,
    user_rejects_all_calls                = 23,
    destination_out_of_order              = 27,
    invalid_address_format                = 28,
    response_to_status_enquiry            = 30,
    normal_unspecified                    = 31,
    requested_VPCI_VCI_unavailable        = 35,
    VPCI_VCI_assignment_failure           = 36,
    user_cell_rate_unavailable            = 37,
    network_out_of_order                  = 38,
    temporary_failure                     = 41,
    access_information_discarded          = 43,
    no_VPCI_VCI_available                 = 45,
    resource_unavailable_unspecified      = 47,
    qos_unavailable                       = 49,
    peer_group_leader_changed             = 53,
    bearer_capability_not_authorized      = 57,
    bearer_capability_not_presently_available      = 58,
    service_or_option_not_unavailable_unspecified  = 63,
    bearer_capability_not_implemented     = 65,
    unsupported_combination_of_traffic_parameters  = 73,
    aal_parameters_cannot_be_supported    = 78,
    invalid_call_reference_value          = 81,
    identified_channel_doesnt_exist       = 82,
    incompatible_destination              = 88,
    invalid_endpoint_reference            = 89,
    invalid_transit_network_selection     = 91, 
    too_many_pending_add_party_requests   = 92,
    mandatory_information_element_is_missing       = 96,
    message_type_nonexistent_or_not_implemented    = 97,
    information_element_nonexistent_or_not_implemented  = 99,
    invalid_information_element_contents   = 100,
    message_not_compatible_with_call_state = 101,
    recovery_on_timer_expiry               = 102,
    incorrect_message_length               = 104,
    protocol_error_unspecified             = 111
    };

  enum Location {
    user                                = 0x80,
    local_private_network               = 0x81,
    local_public_network                = 0x82,
    transit_network                     = 0x83,
    remote_private_network              = 0x84,
    remote_public_network               = 0x85,
    international_network               = 0x87,
    network_beyond_interworking_pt      = 0x8a
    };

  ie_cause(CauseValues cv = ie_cause::normal_unspecified, Location loc = user);

  ie_cause(CauseValues cv, Location loc, int len, u_char * d);

  ie_cause(const ie_cause & c);

  virtual ~ie_cause();

  InfoElem* copy(void) const;

  int operator == (const ie_cause & cv) const;
  int in_cause_list(const ie_cause & cv) const;

  virtual int encode(u_char * buf);
  virtual InfoElem::ie_status  decode(u_char *, int &);

  virtual u_char IsVariableLen(void);
  
  Location get_location(void);
  void set_location(Location loc);
  
  CauseValues get_cause_value(void);
  void set_cause_value(CauseValues cv);

  int max_repeats();
  int Length();
 private:
  virtual char PrintSpecific(ostream & os);

  Location     _location;
  CauseValues  _cause_value;
  int          _diagnostic_len;
  int          _num_diagnostics;
  u_char      *_diagnostic_info;
  // InfoElem keeps track of multiple causes
};

#endif
