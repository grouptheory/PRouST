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

#if !defined(LINT)
static char const _calling_party_subaddr_cc_rcsid_[] =
"$Id: calling_party_subaddr.cc,v 1.7 1998/08/06 23:21:06 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/calling_party_subaddr.h>
#include <codec/uni_ie/subaddr.h>

/*
 * we need to first check whether we are pointing to an address
 * object. Then copy the encoded address and its length and type.
 */

ie_calling_party_subaddr::ie_calling_party_subaddr(Addr *addr) :
  subaddress(ie_calling_party_subaddr_id, addr) 
{
}

ie_calling_party_subaddr::ie_calling_party_subaddr(const 
						   ie_calling_party_subaddr 
						   &subaddr) :
  subaddress(subaddr) {};

ie_calling_party_subaddr::~ie_calling_party_subaddr() 
{ }

InfoElem* ie_calling_party_subaddr::copy(void) const
{
  Addr *subaddr = 0L;

  if (_subaddr_ptr) {
    subaddr = _subaddr_ptr->copy();
    return(new ie_calling_party_subaddr(subaddr));
  } else
    return(new ie_calling_party_subaddr());
}



int ie_calling_party_subaddr::operator == (const ie_calling_party_subaddr & src) const
{
  return equals(&src);
}


int ie_calling_party_subaddr::equals(const InfoElem* him) const
{
  return him->equals(this);
}


int ie_calling_party_subaddr::equals(const ie_calling_party_subaddr* himptr) const
{
  // Use equality oper from subaddr
  return ((*((class subaddress*)this)) == *((class subaddress*)himptr));
}

Addr* ie_calling_party_subaddr::get_subaddr(void) { return _subaddr_ptr; }

int ie_calling_party_subaddr::Length()
{ return subaddress::Length(); }

int ie_calling_party_subaddr::encode(u_char * buffer)
{ return subaddress::encode(buffer); }

InfoElem::ie_status ie_calling_party_subaddr::decode(u_char * buffer, int & id)
{ return subaddress::decode(buffer, id); }

ie_calling_party_subaddr::ie_calling_party_subaddr(void) : subaddress(ie_calling_party_subaddr_id){}

int ie_calling_party_subaddr::max_repeats(void) { return 2; }
