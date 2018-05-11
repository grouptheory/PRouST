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
static char const _conn_id_cc_rcsid_[] =
"$Id: conn_id.cc,v 1.11 1998/08/06 04:03:16 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/conn_id.h>
#include <codec/uni_ie/errmsg.h>

// explicit indication of VPCI and exclusive VPCI and exclusive VCI
ie_conn_id::ie_conn_id(long vpi, long vci) :InfoElem(ie_conn_identifier_id),
  _vpi(vpi), _vci(vci),_vp_associated(1),_pref_exclusive(0) { }

ie_conn_id::ie_conn_id(long vpi, long vci, int vp_associated, int pref_exclusive) :InfoElem(ie_conn_identifier_id),
  _vpi(vpi), _vci(vci),_vp_associated(vp_associated),_pref_exclusive(pref_exclusive) { }

ie_conn_id::~ie_conn_id() { }

ie_conn_id::ie_conn_id(const ie_conn_id & rhs) : InfoElem(ie_conn_identifier_id),
  _vpi(rhs._vpi), _vci(rhs._vci) { }

InfoElem* ie_conn_id::copy(void) const {return new ie_conn_id(_vpi,_vci);}

int ie_conn_id::equals(const ie_conn_id *himptr) const
{
  return ((_vpi == himptr->_vpi) && (_vci == himptr->_vci));
}


int ie_conn_id::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}

int ie_conn_id::operator == (const ie_conn_id & cid) const
{return ((_vpi == cid._vpi) && (_vci == cid._vci));}

long ie_conn_id::get_vci(void) { return _vci;}
long ie_conn_id::get_vpi(void) { return _vpi;}
void ie_conn_id::set_vci(long v) { _vci = v; MakeStale();}
void ie_conn_id::set_vpi(long v) { _vpi = v; MakeStale();}

ie_conn_id::ie_conn_id(void):InfoElem(ie_conn_identifier_id), _vpi(0),_vci(0){}

int ie_conn_id::GetVPassociated()
{
  return _vp_associated;
}

int ie_conn_id::GetPrefExclusive()
{
  return _pref_exclusive;
}


int ie_conn_id::Length()
{
  return (ie_header_len + 5);
}

int ie_conn_id::encode(u_char * buffer)
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
  put_8(temp,buflen,(0x80 | (_vp_associated << 3) | _pref_exclusive));
  put_8(temp,buflen,0);
  put_8(temp,buflen,(0xff & _vpi));
  // u_short vci = htons(_vci); // sjm
  put_16(temp,buflen,_vci);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

/*
 * This information element has length 5 bytes
 */
InfoElem::ie_status ie_conn_id::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 5)
    return (InfoElem::incomplete);
  buffer += 4;
  if ((*buffer & 0x80) != 0x80)
    return (InfoElem::invalid_contents);

  // bits 5,4 or fifth byte must be either 0 OR 1
  _vp_associated = (*buffer & 0x18) >> 4;
  if(_vp_associated != 0 && _vp_associated != 1)
    return (InfoElem::invalid_contents);

  // bits 3,2,1 or fifth byte must be either 0 OR 1
  _pref_exclusive = *buffer++ & 0x07;
  if(_pref_exclusive != 0 && _pref_exclusive != 1)
    return (InfoElem::invalid_contents);

  get_16(buffer,_vpi);
  get_16(buffer,_vci);
  return (InfoElem::ok);
}

char ie_conn_id::PrintSpecific(ostream & os)
{
   os << endl;
   os << "   vpi::" << _vpi << endl;
   os << "   vci::" << _vci << endl;
   return 0;
}

ostream & operator << (ostream & os, ie_conn_id & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}









