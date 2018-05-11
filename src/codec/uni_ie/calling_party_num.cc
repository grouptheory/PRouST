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
static char const _calling_party_num_cc_rcsid_[] =
"$Id: calling_party_num.cc,v 1.10 1998/08/06 04:03:15 bilal Exp $";
#endif
#include <common/cprototypes.h>
#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/calling_party_num.h>
#include <codec/uni_ie/errmsg.h>

/*
 * Calling party number 5.4.5.13 in UNI-3.1 page 222
 * This IE is used to identify the origin of a call.
 * optional IE.
 */
ie_calling_party_num::ie_calling_party_num(Addr *src_addr,
  presentation_indicator presentation, screening_indicator screening) :
  InfoElem(ie_calling_party_num_id),
  _presentation(presentation),_screening(screening), _src_addr(0)
{
  if (presentation == presentation_allowed)
    _send = 1;
  else
    _send = 0;

  if (src_addr) {
    switch (src_addr->get_addressing_type()) {
    case Addr::Unknown: // NSAP
      switch (((NSAP_addr *)src_addr)->get_afi_type()) {
      case NSAP_addr::DCC_ATM:
      case NSAP_addr::ICD_ATM:
	_src_addr = new NSAP_DCC_ICD_addr((NSAP_DCC_ICD_addr *)src_addr);
	break;
      case NSAP_addr::E164_ATM:
	_src_addr = new NSAP_E164_addr((NSAP_E164_addr *)src_addr);
	break;
      default:
	_src_addr = 0L;
	break;
      }
      break;
    case Addr::International: // E164
      _src_addr = new E164_addr((E164_addr *)src_addr);
      break;
    default:
      // Error!
      _src_addr = 0L;
      break;
    }
  }
}


ie_calling_party_num::ie_calling_party_num(const ie_calling_party_num 
					   &src_addr) :
  InfoElem(src_addr), _presentation(src_addr._presentation),
  _screening(src_addr._screening), _send(src_addr._send),
  _src_addr(0)
{
  if (src_addr._src_addr)
    _src_addr = src_addr._src_addr->copy();
}
  

ie_calling_party_num::~ie_calling_party_num() {
  delete _src_addr;
  _src_addr=0L;
}

InfoElem* ie_calling_party_num::copy(void) const
{
  Addr * addr;
  InfoElem *answer = 0;

  if(_src_addr)
    addr = _src_addr->copy();
  else
    addr = 0;

  answer =  new ie_calling_party_num(addr,_presentation,_screening);
  delete addr;

  return answer;
}


Addr* ie_calling_party_num::get_addr(void)
{ return _src_addr; }

enum ie_calling_party_num::presentation_indicator ie_calling_party_num::get_presentation(void)
{ return _presentation; }

enum ie_calling_party_num::screening_indicator ie_calling_party_num::get_screening(void)       
{ return _screening; }


// if we have a _src_addr and _presentation is not restricted we include it.

int ie_calling_party_num::encode(u_char * buffer)
{
  u_char * temp;
  u_char * cptr;
  int addrlen;
  int buflen = 0;

  if ((temp = LookupCache()) != 0L) {
      bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
      buffer += buflen;
      return(buflen);
    }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);
  put_coding(buffer,_coding);
  // number type depends on the address
  if (_src_addr)
    put_8(temp,buflen, _src_addr->get_addressing_type());
  put_8(temp,buflen,(0x80 | _presentation | _screening));
  if (_src_addr) {
    if (_presentation != presentation_restricted) {
      int len = _src_addr->encode(temp);
      buflen += len;
      temp += len;
    }
  }
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}



int ie_calling_party_num::Length()
{
  int buflen = ie_header_len;
  // number type depends on the address
  if (_src_addr)
    buflen++;
  buflen++;
  if(_src_addr && (_presentation != presentation_restricted))
    buflen += _src_addr->Length();
  return (buflen);
}

/*
 * not counting the header:
 *   max length is 22 bytes: 20 bytes of an NSAP _src_addr + bytes 5 and 5a.
 *   min length is 1 byte (byte 5)
 */
InfoElem::ie_status ie_calling_party_num::decode(u_char *buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  int nid;
  InfoElem::ie_status rval;
  int addr_type = *(buffer+4) & 0x7f;
  int ind_5a = *(buffer+4) & 0x80;

  buffer += 4;
  if (id != _id)
    return (InfoElem::bad_id);
  if ((len < 1) || (len > 22))
    return (InfoElem::incomplete);
  len--;
  if (!ind_5a) {
    len--; buffer++;
    _presentation = (presentation_indicator ) (((*buffer) & 0x60) >> 5);
    _screening    = (screening_indicator )((*buffer) & 0x03);
  }
  if (!len) {
    // min case that is no calling number
    _src_addr = 0;
    return (InfoElem::ok);
  }
  E164_addr *tel;
  buffer++;

  switch (addr_type) {
    case Addr::International: // 15 digit telephone number
      if (len > E164_MAX_SIZE)
	return (InfoElem::invalid_contents);
      tel = new E164_addr;
      int y;
      if (rval = tel->decode(buffer,y))
        return rval;
      _src_addr = tel;
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
	  _src_addr = nsap;
	  break;
	case NSAP_addr::E164_ATM:
	  nsap = new NSAP_E164_addr((NSAP_addr::afi_type) *buffer);
	  // fill in the object 
	  if (rval = nsap->decode(buffer,nid))
            return rval;
	  _src_addr = nsap;
	  break;

	default:
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


int ie_calling_party_num::operator == (const ie_calling_party_num & src) const
{
  return equals(&src);
}


int ie_calling_party_num::equals(const InfoElem* him) const
{
  return him->equals(this);
}

int ie_calling_party_num::equals(const ie_calling_party_num* himptr) const
{
    if (*_src_addr == *(himptr->_src_addr)) return 1;
    else return 0;
}

ie_calling_party_num::ie_calling_party_num(void) :InfoElem(ie_calling_party_num_id),
  _src_addr(0),_presentation(presentation_allowed),
  _screening(user_provided_not_screened)
{
}

char ie_calling_party_num::PrintSpecific(ostream & os)
{
  os << endl << "   presentation_indicator::";
  switch (_presentation) {
  case presentation_allowed: os << "presentation allowed " << endl; break;
  case presentation_restricted: os << "presentation restricted" << endl; break;
  case addr_not_available:  os << "addr not available " << endl; break;
  case reserved: os << "reserved " << endl; break;
  default: os << "unknown " << endl; break;
  }
  os << "   screening_indicator::";
  switch (_screening) {
  case user_provided_not_screened: 
    os << "user provided not screened" << endl; break;
  case user_provided_verified_and_passed:  
    os << "user provided verified and passed" << endl; break;
  case user_provided_verified_and_failed:  
    os << "user provided verified and failed" << endl; break;
  case network_provided: os << "network provided" << endl; break;
  default:  os << "unknown" << endl; break;
  }
  os << "   send::" << _send << endl;
  os << "   source addr::"; // << *(_src_addr) << endl;
  if (_src_addr)
    _src_addr->PrintSpecific(os);
  else os << "0x0";
  os << endl;
  return 0;
}

ostream & operator << (ostream & os, ie_calling_party_num & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}

