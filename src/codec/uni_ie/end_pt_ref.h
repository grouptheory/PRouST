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
#ifndef _END_PT_REF_H_
#define _END_PT_REF_H_

#ifndef LINT
static char const _end_pt_ref_h_rcsid_[] =
"$Id: end_pt_ref.h,v 1.4 1998/02/19 01:31:29 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>


class ie_errmsg;
/*
 * see 5.4.8.1 page 239 UNI-3.1 book
 * This is an ATM Forum ie. The purpose of this information
 * element is to identify an endpoint a point-to-multipoint
 * connection. The ie with end-point-ref-value of 0 is allways
 * used for the first party. Non-zero values identify subsequent
 * parties. Its length is 3 bytes.
 */

class ie_end_pt_ref : public InfoElem {

 public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const ie_end_pt_ref* himptr) const;

  enum endpoint_ref_type {
    locally_defined = 0x00
  };

  enum endpoint_ref_flag {
    from_endpoint_ref_side = 0x00,
    to_endpoint_ref_side   = 0x80
  };


  ie_end_pt_ref(int eprv);
  ie_end_pt_ref(const ie_end_pt_ref & rhs);

  virtual ~ie_end_pt_ref();

  InfoElem* copy(void) const;

  int operator == (const ie_end_pt_ref & rie) const;
  
  virtual int                 encode(u_char * buffer);
  virtual InfoElem::ie_status decode(u_char *, int &);

  int  get_epr_value(void);
  void set_epr_value(int eprv);

  ie_end_pt_ref(void);

  virtual char PrintSpecific(ostream & os);
  friend ostream & operator << (ostream & os, ie_end_pt_ref & x);
  int Length();
private:

  int _endpoint_ref_value;
};

#endif
