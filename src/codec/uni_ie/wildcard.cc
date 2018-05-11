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

#ifndef _IE_WILDCARD_CC_
#define _IE_WILDCARD_CC_
#if !defined(LINT)
static char const _wildcard_cc_rcsid_[] =
"$Id: wildcard.cc,v 1.7 1998/09/28 09:12:00 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/wildcard.h>

ostream & operator << (ostream & os, ie_wildcard & x)
{
  os << "Wildcard\n";
  return os;
}

int ie_wildcard::equals(const InfoElem* him) const {return 1; }
int ie_wildcard::equals(const ie_wildcard* himptr) { return 1; }
int ie_wildcard::equals(const E164_addr* x) const { return 1; }
int ie_wildcard::equals(const NSAP_addr* x) const { return 1; }
int ie_wildcard::equals(const NSAP_DCC_ICD_addr* x) const { return 1; }
int ie_wildcard::equals(const NSAP_E164_addr* x) const { return 1; }
int ie_wildcard::equals(const iso_bhli* x) const { return 1; }
int ie_wildcard::equals(const user_bhli* x) const { return 1; }
int ie_wildcard::equals(const vendor_bhli* x) const { return 1; }
int ie_wildcard::equals(const ie_blli* x) const { return 1; }
int ie_wildcard::equals(const ie_bbc* x) const { return 1; }
// int ie_wildcard::equals(const A_Bbc* x) const { return 1; }
// int ie_wildcard::equals(const C_Bbc* x) const { return 1; }
// int ie_wildcard::equals(const X_Bbc* x) const { return 1; }
int ie_wildcard::equals(const aal5 * x) const { return 1; }
int ie_wildcard::equals(const ie_broadband_repeat_ind* x) const { return 1; }
int ie_wildcard::equals(const ie_broadband_send_comp_ind* x) const { return 1; }
int ie_wildcard::equals(const ie_call_state* x) const { return 1; }
int ie_wildcard::equals(const ie_called_party_num* x) const { return 1; }
int ie_wildcard::equals(const ie_called_party_subaddr* x) const { return 1; }
int ie_wildcard::equals(const ie_calling_party_num* x) const { return 1; }
int ie_wildcard::equals(const ie_calling_party_subaddr* x) const { return 1; }
int ie_wildcard::equals(const ie_cause* x) const { return 1; }
int ie_wildcard::equals(const ie_conn_id* x) const { return 1; }
int ie_wildcard::equals(const ie_end_pt_ref* x) const { return 1; }
int ie_wildcard::equals(const ie_end_pt_state* x) const { return 1; }
int ie_wildcard::equals(const ie_qos_param* x) const { return 1; }
int ie_wildcard::equals(const ie_restart_ind* x) const { return 1; }
int ie_wildcard::equals(const ie_traffic_desc* x) const { return 1; }
int ie_wildcard::equals(const ie_transit_net_sel* x) const { return 1; }


ie_wildcard::ie_wildcard(void):InfoElem(ie_unknown_id) {}

ie_wildcard::ie_wildcard(const ie_wildcard & c):InfoElem(c) {}

ie_wildcard::~ie_wildcard() {}

InfoElem* ie_wildcard::copy(void) const { return new ie_wildcard(); }

int ie_wildcard::operator == (const ie_wildcard & cv) const {return 1;}

int ie_wildcard::encode(u_char * buf)  { return 0; }

InfoElem::ie_status ie_wildcard::decode(u_char *, int & id) { id = 0; return InfoElem::ok; }

u_char ie_wildcard::IsVariableLen(void) {return 0;}

char ie_wildcard::PrintSpecific(ostream & os) { os << "Wildcard\n"; return 0; }

#endif
