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

#ifndef _IE_WILDCARD_H_
#define _IE_WILDCARD_H_
#if !defined(LINT)
static char const _wildcard_h_rcsid_[] =
"$Id: wildcard.h,v 1.4 1998/09/28 09:12:00 bilal Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class ie_wildcard : public InfoElem{
  friend ostream & operator << (ostream & os, ie_wildcard & x);

public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const ie_wildcard* himptr);
  virtual int equals(const E164_addr* x) const;
  virtual int equals(const NSAP_addr* x) const;
  virtual int equals(const NSAP_DCC_ICD_addr* x) const;
  virtual int equals(const NSAP_E164_addr* x) const;
  virtual int equals(const iso_bhli* x) const;
  virtual int equals(const user_bhli* x) const;
  virtual int equals(const vendor_bhli* x) const;
  virtual int equals(const ie_blli* x) const;
  virtual int equals(const ie_bbc* x) const;
  //  virtual int equals(const A_Bbc* x) const;
  //  virtual int equals(const C_Bbc* x) const;
  //  virtual int equals(const X_Bbc* x) const;
  virtual int equals(const aal5 * x) const;
  virtual int equals(const ie_broadband_repeat_ind* x) const;
  virtual int equals(const ie_broadband_send_comp_ind* x) const;
  virtual int equals(const ie_call_state* x) const;
  virtual int equals(const ie_called_party_num* x) const;
  virtual int equals(const ie_called_party_subaddr* x) const;
  virtual int equals(const ie_calling_party_num* x) const;
  virtual int equals(const ie_calling_party_subaddr* x) const;
  virtual int equals(const ie_cause* x) const;
  virtual int equals(const ie_conn_id* x) const;
  virtual int equals(const ie_end_pt_ref* x) const;
  virtual int equals(const ie_end_pt_state* x) const;
  virtual int equals(const ie_qos_param* x) const;
  virtual int equals(const ie_restart_ind* x) const;
  virtual int equals(const ie_traffic_desc* x) const;
  virtual int equals(const ie_transit_net_sel* x) const;

  ie_wildcard(void);

  ie_wildcard(const ie_wildcard & c);

  virtual ~ie_wildcard();

  InfoElem* copy(void) const;

  int operator == (const ie_wildcard & cv) const;

  virtual int                 encode(u_char * buf);
  virtual InfoElem::ie_status decode(u_char *, int &);

  virtual u_char IsVariableLen(void);

private:

  virtual char PrintSpecific(ostream & os);
};

#endif
