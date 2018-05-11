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
static char const _UNI40_generic_cc_rcsid_[] =
"$Id: UNI40_generic.cc,v 1.10 1998/08/06 04:03:07 bilal Exp $";
#endif
#include <common/cprototypes.h>
// Generic Identifier Transport IE see UNI 4.0 2.1.1

#include <codec/uni_ie/UNI40_generic.h>


UNI40_generic_id::UNI40_generic_id(id_related_std_app type, id_types id,
				   u_char *val, int n)
  : InfoElem(UNI40_generic_id_id), _idr(type), _id_type(id), _repeats(n)
{ 
  if (val != 0) {
    _id_value = new u_char [strlen((char *)val)]; 
    int i;
    for (i = 0; val[i] != 0; ++i) 
      _id_value[i] = val[i]; 
    _id_value[i] = 0;
  } else _id_value = val;
}

UNI40_generic_id::UNI40_generic_id(const UNI40_generic_id & g) :
 	InfoElem(UNI40_generic_id_id), _idr(g._idr), 
               _id_type(g._id_type), _repeats(g._repeats)
{ 
  int i;
  _id_value = new u_char [strlen((char *)g._id_value)];
  for (i = 0; g._id_value[i] != 0; ++i)
    _id_value[i] = g._id_value[i];
  _id_value[i] = 0;
}

UNI40_generic_id::~UNI40_generic_id( ) { delete [] _id_value; }
UNI40_generic_id & UNI40_generic_id::operator = (const UNI40_generic_id & rhs)
{
  int i;
  _idr = rhs._idr;
  _id_type = rhs._id_type;
  _repeats = rhs._repeats;
  if (_id_value != 0)
    delete [] _id_value;
  if (rhs._id_value == 0)
    _id_value = 0;
  else {
    _id_value = new u_char [strlen((char *)rhs._id_value)];
    for (i = 0; rhs._id_value[i] != 0; ++i)
      _id_value[i] = rhs._id_value[i];
    _id_value[i] = 0;
  }
  return (*this);
}

int UNI40_generic_id::operator == (const UNI40_generic_id & rhs) const
{
   return equals(&rhs);
}

int UNI40_generic_id::equals(const UNI40_generic_id * rhs) const
{
  return ((_idr == rhs->_idr) &&
          (_id_type == rhs->_id_type) &&
	  (_repeats == rhs->_repeats) &&
	  (!strcmp((char *)_id_value, (char *)rhs->_id_value)));
}

int UNI40_generic_id::equals(const InfoElem * rhs) const
{
  return rhs->equals(this);
}


int UNI40_generic_id::Length()
{
  int      buflen = ie_header_len;
  buflen++; // for the Identifier Related Standard/Applications
  int  n = _repeats;
  while (n-- > 0)
    {
      buflen += 2;
      if(_id_value)
	buflen += strlen((char *)_id_value);
    }
  return buflen;
}

int UNI40_generic_id::encode(u_char * buffer)
{
  u_char * temp = buffer;
  int      buflen = 0;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    return(buflen);
  }
  put_id(buffer,_id);
  put_coding(buffer,0x60);
  temp = buffer + ie_header_len;
  // encode the Identifier Related Standard/Applications
  put_8(temp,buflen,_idr);
  int  n = _repeats;
  while (n-- > 0) {
    put_8(temp,buflen,_id_type);
    // If the pointer is NULL write a zero and end
    if (!_id_value)
      *temp++ = 0x00;
    else {
      *temp++ = (u_char)(strlen((char *)_id_value) & 0xFF);
      
      for (int i = 0; i < strlen((char *)_id_value); ++i) {
	*temp++ = _id_value[i];
	buflen++;
      }
    }
    buflen++;
  }
  put_len(buffer,buflen);
  FillCache(buffer, buflen);
  return buflen;
}

InfoElem::ie_status UNI40_generic_id::decode(u_char * buffer, int & id)
{
       id = buffer[0];
  int len = (buffer[2] << 8) | (buffer[3]);
  
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);

  buffer += ie_header_len;
  // read in the identifer related standard
  _idr = (id_related_std_app) *buffer++;
  len--;
  // id type
  _id_type = (id_types)*buffer++;
  len--;
  // id value length
  u_short vlen = *buffer++;
  len--;
  _id_value = new u_char [vlen];
  for (int i = 0; i < vlen; ++i, --len)
    _id_value[i] = *buffer++;
  
  if (len < 0)
    return (InfoElem::incomplete);
  return InfoElem::ok;
}

char UNI40_generic_id::PrintSpecific(ostream & os)
{
  os << endl << "   Application::";
  switch (_idr) {
    case DSMCC: os << "Digital Storage Media Command & Control " << endl;
      break;
    case H245: os << "Recommendation H.245 " << endl;
      break;
    default: os << "Error " << endl;
      break;
  }
  os << "   Identifier Type::";
  switch (_id_type) {
    case Session: os << "Session " << endl;   break;
    case Resource: os << "Resource " << endl; break;
    default: os << "Error " << endl;          break;
  }
  os << "   Identifier Value::";
  special_print(_id_value, strlen((char *)_id_value), os);
  os << endl;
  return 0;
}

ostream & operator << (ostream & os, UNI40_generic_id & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}


UNI40_generic_id * UNI40_generic_id::copy(void)
{ return new UNI40_generic_id(*this); }
InfoElem * UNI40_generic_id::copy(void) const 
{ 
  return (InfoElem *)(new UNI40_generic_id(this->_idr, this->_id_type,
					   this->_id_value, this->_repeats)); 
}

UNI40_generic_id::id_related_std_app UNI40_generic_id::getRelated(void) const
{ return _idr; }
UNI40_generic_id::id_types           UNI40_generic_id::getType(void) const
{ return _id_type; }
u_char            *UNI40_generic_id::getValue(void) const { return _id_value; }

void               UNI40_generic_id::setRelated(id_related_std_app rel)
{ _idr = rel; }
void               UNI40_generic_id::setType(id_types type)
{ _id_type = type; }
void               UNI40_generic_id::setRepeats(int n) { _repeats = n; }

int UNI40_generic_id::max_repeats(void) { return 3; }
