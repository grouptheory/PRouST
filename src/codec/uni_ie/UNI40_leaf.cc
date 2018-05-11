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
static char const _UNI40_leaf_cc_rcsid_[] =
"$Id: UNI40_leaf.cc,v 1.10 1998/08/06 23:17:37 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>
#include <codec/uni_ie/UNI40_leaf.h>

// LIJ Call Identifier
UNI40_lij_call_id::UNI40_lij_call_id(u_char type, int value) :
       InfoElem(UNI40_leaf_call_id), _type(type), _value(value) { }

UNI40_lij_call_id::UNI40_lij_call_id(const UNI40_lij_call_id & lij) :
       InfoElem(lij), 
       _type(lij._type), 
       _value(lij._value) { }

UNI40_lij_call_id & UNI40_lij_call_id::operator = (const UNI40_lij_call_id & right)
{
   if (this == &right)
     return *this;

   _type  = right._type;
   _value = right._value;

   return *this;
}

int UNI40_lij_call_id::Length()
{
  return (ie_header_len + 5);
}

int UNI40_lij_call_id::encode(u_char *buffer)
{
  u_char * cptr;
  int buflen = 0;

  if ((cptr = LookupCache()) != 0L) {
    bcopy((char *)cptr, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  cptr = buffer + ie_header_len;
  put_id(buffer,_id);	
  put_coding(buffer,0x60);
  // see sec 6.1.2.1
  put_8(cptr,buflen,(_type & 0x8F));
  put_32(cptr,buflen,_value);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

InfoElem::ie_status UNI40_lij_call_id::decode(u_char *buffer, int & id)
{
  id = buffer[0];
  // buffer[1]: bit8 must be 1 and coding must be 11
  if(((buffer[1] & 0xE0) >> 5) != 0x07)
    return InfoElem::invalid_contents;
  short len = get_len(buffer);
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len < 5)
    return (InfoElem::incomplete);
  if(len != 5)
    return InfoElem::invalid_contents;
  u_char *temp =  buffer + ie_header_len;
  get_8(temp,_type);
  _type &= 0x0F; 
  get_32(temp,_value);
  return (InfoElem::ok); 
}


UNI40_lij_call_id *UNI40_lij_call_id::copy(void) 
{
  return (new UNI40_lij_call_id(*this));
}


int UNI40_lij_call_id::operator == (const UNI40_lij_call_id & lp) const
{
  return equals(&lp);
}


int UNI40_lij_call_id::equals(const UNI40_lij_call_id * himptr) const
{
  return ((_type == himptr->_type) && (_value == himptr->_value));
}


int UNI40_lij_call_id::equals(const InfoElem * him) const
{
  return (him->equals(this));
}


char UNI40_lij_call_id::PrintSpecific(ostream & os)
{
  os << endl << "   type::" << ios::hex << (int)_type << ios::dec;
  os << endl << "   value::" << ios::hex << _value << ios::dec << endl;
  return 0;
}


ostream & operator << (ostream & os, UNI40_lij_call_id & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}


// LIJ Parameters
int UNI40_lij_params::encode(u_char *buffer)
{
  u_char * cptr = buffer;
  int buflen;

  if ((cptr = LookupCache()) != 0L) {
    bcopy((char *)cptr, (char *)buffer, (buflen = CacheLength()));
    cptr = buffer + buflen;
  } else {
    cptr = buffer + ie_header_len;
    // see sec 6.1.2.2
    *cptr++ = (u_char) (_screening_ind & 0x8F);

    buflen = 1 + ie_header_len;
    // encode the header now that we know the total length
    encode_ie_header(buffer, _id, 1);
    FillCache(buffer, buflen);
  }
  return (buflen);
}

int UNI40_lij_params::Length()
{
  return (ie_header_len + 1);
}


InfoElem::ie_status UNI40_lij_params::decode(u_char *buffer, int & id)
{
  short len = get_len(buffer);
  id = buffer[0];
  //      id = ntohs(buffer[0]); // sjm
  
  if (id != _id)
    return InfoElem::bad_id;
  if (!len)
    return InfoElem::empty;
  if (len != 1)
    return InfoElem::invalid_contents;

  _screening_ind = (buffer[4] & 0x0F);

  return InfoElem::ok; 
}


UNI40_lij_params *UNI40_lij_params::copy(void) 
{
  return (new UNI40_lij_params(*this));
}


int UNI40_lij_params::operator == (const UNI40_lij_params & lp) const
{
  return equals(&lp);
}


int UNI40_lij_params::equals(const UNI40_lij_params * himptr) const
{
  return (_screening_ind == himptr->_screening_ind);
}


int UNI40_lij_params::equals(const InfoElem * him) const
{
  return (him->equals(this));
}


char UNI40_lij_params::PrintSpecific(ostream & os)
{
  os << endl
     << "   screening indicator::" << ios::hex << (int)_screening_ind << dec
     << endl;
  return 0;
}


ostream & operator << (ostream & os, UNI40_lij_params & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}



// Leaf Sequence Number
int UNI40_leaf_sequence_num::encode(u_char *buffer)
{
  u_char * cptr = buffer;
  int buflen;

  if ((cptr = LookupCache()) != 0L) {
    bcopy((char *)cptr, (char *)buffer, (buflen = CacheLength()));
    cptr = buffer + buflen;
  } else {
    cptr = buffer + ie_header_len;
    // see sec 6.1.2.3
    *cptr++ = (u_char) ((_seq_num >> 24) & 0xFF);
    *cptr++ = (u_char) ((_seq_num >> 16) & 0xFF);
    *cptr++ = (u_char) ((_seq_num >>  8) & 0xFF);
    *cptr++ = (u_char) (_seq_num & 0xFF);

    buflen = 4 + ie_header_len;
    // encode the header now that we know the total length
    encode_ie_header(buffer, _id, 4);
    FillCache(buffer, buflen);
  }
  return (buflen);
}


int UNI40_leaf_sequence_num::Length()
{
  return (4 + ie_header_len);
}


InfoElem::ie_status UNI40_leaf_sequence_num::decode(u_char *buffer, int & id)
{
  short len = get_len(buffer);
       id = buffer[0];
       //       id = ntohs(buffer[0]);

  if (id != _id)
    return InfoElem::bad_id;
  if (!len)
    return InfoElem::empty;
  if (len != 4)
    return InfoElem::invalid_contents;

  _seq_num  = (int)((buffer[4] << 24) & 0xFF000000);
  _seq_num |= (int)((buffer[5] << 16) & 0x00FF0000);
  _seq_num |= (int)((buffer[6] <<  8) & 0x0000FF00);
  _seq_num |= (int) (buffer[7] & 0xFF);

  return InfoElem::ok; 
}


UNI40_leaf_sequence_num *UNI40_leaf_sequence_num::copy(void) 
{
  return (new UNI40_leaf_sequence_num(*this));
}


int UNI40_leaf_sequence_num::operator == (const UNI40_leaf_sequence_num & lp) const
{
  return equals(&lp);
}


int UNI40_leaf_sequence_num::equals(const UNI40_leaf_sequence_num * himptr) const
{
  return (_seq_num == himptr->_seq_num);
}


int UNI40_leaf_sequence_num::equals(const InfoElem * him) const
{
  return (him->equals(this));
}


char UNI40_leaf_sequence_num::PrintSpecific(ostream & os)
{
  os << endl
     << "   sequence number::" << ios::hex << _seq_num << ios::dec <<endl;
  return 0;
}


ostream & operator << (ostream & os, UNI40_leaf_sequence_num & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}


