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
static char const _end_pt_ref_cc_rcsid_[] =
"$Id: end_pt_ref.cc,v 1.8 1998/08/06 04:03:16 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/end_pt_ref.h>
#include <codec/uni_ie/errmsg.h>

//***************************************************************************
// see 5.4.8.1 page 239 UNI-3.1 book
// This is an ATM Forum ie. The purpose of this information
// element is to identify an endpoint of a point-to-multipoint
// connection. The ie with end-point-ref-value of 0 is always
// used for the first party. Non-zero values identify subsequent
// parties. Its length is 3 bytes.
//***************************************************************************

ie_end_pt_ref::ie_end_pt_ref(int eprv) : InfoElem(ie_end_pt_ref_id),
  _endpoint_ref_value(eprv) { }

ie_end_pt_ref::ie_end_pt_ref(const ie_end_pt_ref & rhs) : InfoElem(ie_end_pt_ref_id),
    _endpoint_ref_value(rhs._endpoint_ref_value) { }

ie_end_pt_ref::~ie_end_pt_ref() {}


InfoElem* ie_end_pt_ref::copy(void) const
    {return new ie_end_pt_ref(_endpoint_ref_value);};

int ie_end_pt_ref::equals(const ie_end_pt_ref *himptr) const
{
  return (himptr->_endpoint_ref_value == _endpoint_ref_value);
}


int ie_end_pt_ref::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}


int ie_end_pt_ref::operator == (const ie_end_pt_ref & rie) const
    {return (rie._endpoint_ref_value == _endpoint_ref_value);}
  
int  ie_end_pt_ref::get_epr_value(void) { return _endpoint_ref_value; }

void ie_end_pt_ref::set_epr_value(int eprv) 
{ 
  _endpoint_ref_value = eprv;
  MakeStale();
}

ie_end_pt_ref::ie_end_pt_ref(void) : InfoElem(ie_end_pt_ref_id),
  _endpoint_ref_value(0) {}

int ie_end_pt_ref::Length()
{
  return (ie_header_len + 3);
}


int ie_end_pt_ref::encode(u_char * buffer)
{
  u_char * temp;
  int buflen = 0;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);
  put_coding(buffer,_coding);
  put_8(temp,buflen,locally_defined); 
  // u_short endpoint_ref_value = htons(_endpoint_ref_value); // sjm
  put_16(temp,buflen,_endpoint_ref_value);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

InfoElem::ie_status ie_end_pt_ref::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 3)
    return (InfoElem::incomplete);

  buffer += ie_header_len;
  int epr_type;
  get_8(buffer,epr_type);
  if (epr_type != locally_defined)
    return (InfoElem::invalid_contents);
  if (((*buffer & 0x80) != to_endpoint_ref_side) && ((*buffer & 0x80) != from_endpoint_ref_side))
    return (InfoElem::invalid_contents);
  get_16(buffer,_endpoint_ref_value);
  return (InfoElem::ok);
}


char ie_end_pt_ref::PrintSpecific(ostream & os)
{
  os << endl << "   endpoint reference::" << _endpoint_ref_value << endl;
  return 0;
}

ostream & operator << (ostream & os, ie_end_pt_ref & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}





