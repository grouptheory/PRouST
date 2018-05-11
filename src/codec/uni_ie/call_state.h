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
#ifndef _CALL_STATE_H_
#define _CALL_STATE_H_

#ifdef LINT
static char const _call_state_h_rcsid_[] =
"$Id: call_state.h,v 1.7 1998/05/13 15:28:46 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class ie_errmsg;

/* see 5.4.5.10 page 218 in UNI-3.1 book
 * Call State describes the current state of a call or
 * the global state of the interface.
 * the length is 1 bytes.
 */


class ie_call_state : public InfoElem {

 public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const ie_call_state* himptr) const;

  enum call_state_values {
    u0_null                      = 0x00,
    u1_call_initiated            = 0x01,
    u2_overlap_sending           = 0x02,
    u3_out_going_call_proceeding = 0x03,
    u4_call_delivered            = 0x04,
    u6_call_present              = 0x06,
    u7_call_received             = 0x07,
    u8_connect_request           = 0x08,
    u9_incoming_call_proceeding  = 0x09,
    u10_active                   = 0x0a,
    u11_release_request          = 0x0b,
    u12_release_indication       = 0x0c,
    u25_overlap_receiving        = 0x19,
    rest1_restart_request        = 0x3d,
    rest2_restart                = 0x3e
  };
  
  ie_call_state(call_state_values state_value);
  ie_call_state(const ie_call_state & rhs);
  ie_call_state(void);

  virtual ~ie_call_state();

  InfoElem* copy(void) const;

  int operator == (const ie_call_state & rs) const;
  
  virtual int encode(u_char * buffer);
  virtual InfoElem::ie_status decode(u_char *, int &);

  call_state_values get_state_value(void);
  void set_state_value(call_state_values state_value);
  int Length();
  char PrintSpecific(ostream & os);

  friend ostream & operator << (ostream & os, ie_call_state & x);

private:

  call_state_values _state_value;
};

#endif

