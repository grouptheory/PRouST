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
static char const _UNI40_e2e_trans_delay_cc_rcsid_[] =
"$Id: UNI40_e2e_trans_delay.cc,v 1.8 1999/02/12 16:48:50 talmage Exp $";
#endif

#include <common/cprototypes.h>
#include <codec/uni_ie/UNI40_e2e_trans_delay.h>
#include <netinet/in.h>

UNI40_e2e_transit_delay::UNI40_e2e_transit_delay(int cumulative, int maximum, int net_gen_ind_set) : 
   InfoElem(UNI40_e2e_transit_delay_id), _cumulative(cumulative), _maximum(maximum),_net_gen_ind_set(net_gen_ind_set) { }

UNI40_e2e_transit_delay::UNI40_e2e_transit_delay(const UNI40_e2e_transit_delay & etd) :
   InfoElem(UNI40_e2e_transit_delay_id), _cumulative(etd._cumulative), _maximum(etd._maximum),_net_gen_ind_set(etd._net_gen_ind_set) 
{ }

UNI40_e2e_transit_delay & UNI40_e2e_transit_delay::operator =(const UNI40_e2e_transit_delay & rhs)
{
  _cumulative  = rhs._cumulative;
  _maximum     = rhs._maximum;
  _net_gen_ind_set = rhs._net_gen_ind_set;
  return (*this);
}

int UNI40_e2e_transit_delay::operator == (const UNI40_e2e_transit_delay & rhs) const
{
  return equals(&rhs);
}

int UNI40_e2e_transit_delay::equals(const UNI40_e2e_transit_delay * rhs) const
{
  return ((_cumulative == rhs->_cumulative) && (_maximum == rhs->_maximum) &&
	  (_net_gen_ind_set == rhs->_net_gen_ind_set));
}

int UNI40_e2e_transit_delay::equals(const InfoElem * rhs) const
{
  return rhs->equals(this);
}

int UNI40_e2e_transit_delay::Length()
{
  int buflen = ie_header_len;
  buflen += 6;
  if(_net_gen_ind_set == 1)
    buflen++;
  return buflen;
}

int UNI40_e2e_transit_delay::encode(u_char * buffer)
{
  u_char * temp = 0;
  int buflen = 0;
  
  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);	
  put_coding(buffer,0x60);
  // first the identifier
  put_8(temp,buflen,cumulative_id);
  put_16(temp,buflen,_cumulative);
  put_8(temp,buflen,maximum_id);
  put_16(temp,buflen,_maximum);
  if(_net_gen_ind_set == 1)
    put_8(temp,buflen,net_gen_ind);
  put_len(buffer, buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return buflen;
}

// this IE length is 7 bytes not including the header length
InfoElem::ie_status UNI40_e2e_transit_delay::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  int len = ntohs((buffer[2] << 8) | (buffer[3]));
  // buffer[1]: bit8 must be 1 and coding must be 11
  if(((buffer[1] & 0xE0) >> 5) != 0x07)
    return InfoElem::invalid_contents;
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len < 6)
    return (InfoElem::incomplete);
  if(len != 6 && len != 7)
    return InfoElem::invalid_contents;
  u_char *temp = buffer + ie_header_len;
  int cid;
  get_8(temp,cid);
  if(cid != cumulative_id)
    return InfoElem::invalid_contents;
  get_16(temp,_cumulative);
  get_8(temp,cid);
  if(cid != maximum_id)
    return InfoElem::invalid_contents;
  get_16(temp,_maximum);
  if(len == 7)
    {
      identifiers  ngind;
      get_8tc(temp,ngind, enum UNI40_e2e_transit_delay::identifiers);
      if(ngind != net_gen_ind)
	return InfoElem::invalid_contents;
    }
  return (InfoElem::ok);
}

UNI40_e2e_transit_delay * UNI40_e2e_transit_delay::copy(void)
{ return new UNI40_e2e_transit_delay(*this); }

InfoElem * UNI40_e2e_transit_delay::copy(void) const
{ return (InfoElem *) new UNI40_e2e_transit_delay(this->_cumulative, this->_maximum,this->_net_gen_ind_set); }

char UNI40_e2e_transit_delay::PrintSpecific(ostream &os)
{
  os << "cumulative: " << _cumulative;
  os << " maximum: " << _maximum;
  os << " net_gen_ind: " << _net_gen_ind_set << endl;

  return 0;
}

ostream & operator << (ostream & os, UNI40_e2e_transit_delay & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return os;
}

