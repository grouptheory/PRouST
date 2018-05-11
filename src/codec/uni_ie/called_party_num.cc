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
#ifndef LINT
static char const _called_party_num_cc_rcsid_[] =
"$Id: called_party_num.cc,v 1.12 1998/08/06 04:03:14 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/errmsg.h>
#include <codec/uni_ie/called_party_num.h>

/*
 * Called party number 5.4.5.11 in UNI-3.1 page 219
 * This IE is used to identify the called party of a call.
 *  since we have a pointer to an address we need a copy 
 *  of the address object first.
 */

ie_called_party_num::ie_called_party_num(Addr * dst_addr) :
  InfoElem(ie_called_party_num_id), _dst_addr(0)
{
  if (dst_addr)
    _dst_addr = dst_addr->copy();
}

ie_called_party_num::ie_called_party_num(void) :
  InfoElem(ie_called_party_num_id), _dst_addr(0) {}

ie_called_party_num::ie_called_party_num(const ie_called_party_num &dst_addr) :
  InfoElem(dst_addr), _dst_addr(0)
{
  if (dst_addr._dst_addr)
    _dst_addr = dst_addr._dst_addr->copy();
}

ie_called_party_num::~ie_called_party_num(void) 
{
  if (_dst_addr)
    delete _dst_addr;
  _dst_addr=0L;
}

Addr* ie_called_party_num::get_addr(void)
{ return _dst_addr; }

InfoElem* ie_called_party_num::copy(void) const
{
  ie_called_party_num* answer = new ie_called_party_num(_dst_addr);
  return answer;
}

u_char IsVariableLen(void)
{
  return ((u_char)1);
}

int ie_called_party_num::encode(u_char * buffer)
{
  u_char * temp;
  int buflen = 0;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  int type_of_num = 0;
  if (_dst_addr)
    type_of_num = _dst_addr->get_addressing_type();
  temp = buffer + ie_header_len;
  put_id(buffer,_id);
  put_coding(buffer,_coding);
  put_8(temp,buflen,(0x80 | type_of_num));
  if (_dst_addr)
    buflen += _dst_addr->encode(temp);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}


int ie_called_party_num::Length()
{
  int  buflen = ie_header_len;
  buflen++;
  if (_dst_addr)
    buflen += _dst_addr->Length();
  return (buflen);
}


InfoElem::ie_status ie_called_party_num::decode(u_char *buffer, int & id)
{
  int nid;
  InfoElem::ie_status rval;
  id = buffer[0];
  int len = get_len(buffer);
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if ((len < 1) || (len > 21))
    return (InfoElem::incomplete);
  buffer += 4;
  // bit 8 of byte 5 should be 1
  if ((0x80 & *buffer) != 0x80)
    return (InfoElem::invalid_contents);
  len--;
  if(!len)
    return (InfoElem::invalid_contents);
  // type and plan are combined in c
  int c;
  get_7(buffer,c);
  E164_addr *tel;
  switch(c) {
    case Addr::International: // 15 digit telephone number
      if (len > E164_MAX_SIZE)
	return (InfoElem::invalid_contents);
      tel = new E164_addr;
      tel->decode(buffer,len);
      _dst_addr = tel;
      break;
    case Addr::Unknown: // NSAP based address (20 bytes)
      if (len != 20)
	return (InfoElem::invalid_contents);
      NSAP_addr * nsap;
      switch(*buffer) {
	case NSAP_addr::DCC_ATM:
	case NSAP_addr::ICD_ATM:
	  nsap = new NSAP_DCC_ICD_addr((NSAP_addr::afi_type) *buffer);
	  // fill in the object 
	  if (rval = nsap->decode(buffer,nid))
	    return rval;
	  _dst_addr = nsap;
	  break;
	case NSAP_addr::E164_ATM:
	  nsap = new NSAP_E164_addr((NSAP_addr::afi_type) *buffer);
	  // fill in the object 
	  if (rval = nsap->decode(buffer,nid))
	    return rval;
	  _dst_addr = nsap;
	  break;
	default:
	  cout << "value is " << *buffer << endl
	       << "which is not " << NSAP_addr::DCC_ATM << endl
	       << "which is not " << NSAP_addr::ICD_ATM << endl
	       << "which is not " << NSAP_addr::E164_ATM << endl;
	  return (InfoElem::invalid_contents);
	  break;
      }
      break;
    default:
      return (InfoElem::invalid_contents);
      break;
  }
  return (InfoElem::ok);
}


int ie_called_party_num::equals(const InfoElem *him) const
{
  return him->equals(this);
}


int ie_called_party_num::equals(const ie_called_party_num * himptr) const
{
  if (_dst_addr == 0L && himptr->_dst_addr == 0L)
    return 1;
  if (_dst_addr == 0L || himptr->_dst_addr == 0L)
    return 0;
  return (*_dst_addr == *(himptr->_dst_addr));
}


int ie_called_party_num::operator == (const ie_called_party_num & dst) const
{
  return equals(&dst);
//  return(*_dst_addr == *(dst._dst_addr));
}

char ie_called_party_num::PrintSpecific(ostream & os)
{
  // call Addr's overloaded operator
  
  os << endl << "   destination addr::";
  if (_dst_addr != 0L) // os << *(_dst_addr);
    _dst_addr->PrintSpecific(os);
  else os << "0x00" << endl;
  return 0;
}

ostream & operator << (ostream & os, ie_called_party_num & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}

