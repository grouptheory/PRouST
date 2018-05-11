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

#ifndef _CALLING_PARTY_NUM_H_
#define _CALLING_PARTY_NUM_H_
#if !defined(LINT)
static char const _calling_party_num_h_rcsid_[] =
"$Id: calling_party_num.h,v 1.3 1998/02/19 01:48:25 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

/* see 5.4.5.13 page 222 in UNI-3.1 book
 * Calling party number
 * this is required for IP over ATM see RFC1755 section 5 page 71
 */

class Addr;
class ie_errmsg;

class ie_calling_party_num : public InfoElem {

 public:

  int equals(const InfoElem* him) const;
  int equals(const ie_calling_party_num* himptr) const;

  enum presentation_indicator {
    presentation_allowed    = 0x00, // default
    presentation_restricted = 0x20,
    addr_not_available      = 0x40,
    reserved                = 0x60
  };

  enum screening_indicator {
    user_provided_not_screened        = 0x00, // default
    user_provided_verified_and_passed = 0x01,
    user_provided_verified_and_failed = 0x02,
    network_provided                  = 0x03
  };

  ie_calling_party_num(Addr *src_addr,
                  presentation_indicator presentation = presentation_allowed,
                  screening_indicator screening = user_provided_not_screened);

  ie_calling_party_num(const ie_calling_party_num & src_addr);

  virtual ~ie_calling_party_num();

  virtual InfoElem* copy(void) const;
  

  Addr* get_addr(void);
  presentation_indicator get_presentation(void);
  screening_indicator get_screening(void);


  virtual int encode(u_char * buffer);

  virtual InfoElem::ie_status decode(u_char *, int &);
  
  int operator == (const ie_calling_party_num & src) const;

  ie_calling_party_num(void);

  char PrintSpecific(ostream & os);
  friend ostream & operator << (ostream & os, ie_calling_party_num & x);
  int Length();
private:

  presentation_indicator _presentation;
  screening_indicator    _screening;
  int _send;
  Addr * _src_addr;
};


#endif
