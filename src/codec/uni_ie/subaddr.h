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
#ifndef _SUBADDR_H_
#define _SUBADDR_H_

#ifndef LINT
static char const _subaddr_h_rcsid_[] =
"$Id: subaddr.h,v 1.6 1998/02/19 01:14:43 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class Addr;

/*
 * both ie_called_party_subaddr and calling_party_subaddr will inherit
 * from this class. this is an NSAP based address that is either
 * NSAP_DCC_ICD_addr or NSAP_E164_addr.
 * Since this is not an IE we need the information element of either the
 * called/calling subaddr that is ie_called_party_subaddr_id OR
 * ie_calling_party_subaddr_id in addition to a pointer
 * the one of NSAP_DCC_ICD_addr or NSAP_E164_addr IEs.
 * Keeps:
 *  a) subaddress type
 *  b) ptr to an address object
 *  c) length of encoded subaddress
 *  d) ptr to the encoded address 
 */

class subaddress  : public InfoElem {
  friend class ie_called_party_subaddr;
  friend class calling_party_subaddr;
  friend ostream & operator <<(ostream & os, subaddress & x);
public:
  enum subaddr_type {
    nsap_even = 0x80,
    nsap_odd  = 0x88,
    atm_even  = 0x90,
    atm_odd   = 0x98
  };

  subaddress(ieid  id, Addr *aptr);
  subaddress(const subaddress & him);

  virtual ~subaddress();

  virtual int                 encode(u_char * buffer);
  virtual InfoElem::ie_status decode(u_char *, int &);

  virtual u_char IsVariableLen(void);

  int operator == (const subaddress & sub) const;
  subaddr_type get_subaddr_type(void);
  void set_subaddr_type(subaddr_type atype);
  int Length();
protected:

  virtual char PrintSpecific(ostream & os);
  subaddress(ieid id);

  subaddr_type _subaddr_type;
  Addr *_subaddr_ptr;
};

#endif

