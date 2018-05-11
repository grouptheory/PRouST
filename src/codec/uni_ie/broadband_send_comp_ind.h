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

#ifndef _BROADBAND_SEND_COMP_IND_H_
#define _BROADBAND_SEND_COMP_IND_H_
#if !defined(LINT)
static char const _broadband_send_comp_ind_h_rcsid_[] =
"$Id: broadband_send_comp_ind.h,v 1.4 1998/02/19 16:13:31 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class ie_errmsg;

/*
 * see 5.4.5.21 page 237 UNI-3.1 book
 * The purpose of this information element is to optionally indicate 
 * completion of the called party number.  this element is OPTIONAL.
 * Its length is 1 byte.
 */

class ie_broadband_send_comp_ind;

class ie_broadband_send_comp_ind : public InfoElem {

 public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const ie_broadband_send_comp_ind* himptr) const;

  int operator==(const ie_broadband_send_comp_ind& him) const;

  enum send_comp_indication {
    sending_complete = 0x21
  };

  ie_broadband_send_comp_ind(void);
  ie_broadband_send_comp_ind(const ie_broadband_send_comp_ind & rhs);
  virtual ~ie_broadband_send_comp_ind();
  
  InfoElem* copy(void) const;

  virtual int                 encode(u_char * buffer);
  virtual InfoElem::ie_status decode(u_char *, int &);
  int Length();
  char PrintSpecific(ostream & os);
};

#endif





