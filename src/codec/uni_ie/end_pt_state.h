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
#ifndef _END_PT_STATE_H_
#define _END_PT_STATE_H_

#ifndef LINT
static char const _end_pt_state_h_rcsid_[] =
"$Id: end_pt_state.h,v 1.8 1998/05/15 21:39:37 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class ie_errmsg;
/*
 * see 5.4.8.2 Endpoint State, page 239 UNI-3.1 book
 * This is an ATM Forum ie. The purpose of this information
 * element is to indicate the state of an endpoint in a point-to-multipoint
 * connection.
 * Its length is 1 byte.
 */

class ie_end_pt_state : public InfoElem {
  friend ostream & operator << (ostream & os, ie_end_pt_state & x);
public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const ie_end_pt_state* himptr) const;

  enum party_state {
    p_null                = 0x00,
    add_party_initiated   = 0x01,
    party_alert_delivered = 0x04,
    add_party_received    = 0x06,
    party_alert_received  = 0x07,
    drop_party_initiated  = 0x0b,
    drop_party_received   = 0x0c,
    p_active              = 0x0a
  };

  ie_end_pt_state(party_state state);
  ie_end_pt_state(const ie_end_pt_state & rhs);
  ie_end_pt_state(void);

  virtual ~ie_end_pt_state();

  InfoElem* copy(void) const;

  int operator == (const ie_end_pt_state & rie) const;
  
  virtual int encode(u_char * buffer);
  virtual InfoElem::ie_status decode(u_char *, int &);

  party_state  get_epr_state(void);
  void set_epr_state(party_state state);
  
  virtual char PrintSpecific(ostream & os);
  int Length();
private:

  party_state _epr_party_state;
};

#endif

