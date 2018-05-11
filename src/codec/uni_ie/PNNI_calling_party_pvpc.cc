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

// -*-C++-*-
#ifndef LINT
static char const _PNNI_calling_party_pvpc_cc_rcsid_[] =
"$Id: PNNI_calling_party_pvpc.cc,v 1.6 1998/08/06 04:03:06 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/PNNI_calling_party_pvpc.h>
#include <codec/uni_ie/PNNI_called_party_pvpc.h>

int PNNI_calling_party_soft_pvpc::Length()
{
  return (ie_header_len + 6);
}

int PNNI_calling_party_soft_pvpc::encode(u_char *buffer)
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
  put_coding(buffer,_coding);
  put_8(temp,buflen,PNNI_called_party_soft_pvpc::VPI_ident);
  put_16(temp,buflen,_vpi);
  put_8(temp,buflen,PNNI_called_party_soft_pvpc::VCI_ident);
  put_16(temp,buflen,_vci);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

InfoElem::ie_status PNNI_calling_party_soft_pvpc::decode(u_char *buffer, int & id)
{
  u_char * temp = buffer + ie_header_len;
  id = buffer[0];
  // buffer[1]: bit8 must be 1 and coding must be 11
  if(((buffer[1] & 0xE0) >> 5) != 0x07)
    return InfoElem::invalid_contents;
  short len = get_len(buffer);
  if (id != _id)
    return InfoElem::bad_id;
  if (!len)
    return InfoElem::empty;
  if(len != 6)
    return InfoElem::invalid_contents;
  int vpi_id;
  int vci_id;
   get_8(temp,vpi_id);
  if(vpi_id != 0x81)
    return (InfoElem::invalid_contents);
  get_16(temp,_vpi);
  if(_vpi > 4095)
     return (InfoElem::invalid_contents);
  get_8(temp,vci_id);
  if(vci_id != 0x82)
    return (InfoElem::invalid_contents);
  get_16(temp,_vci);
  return (InfoElem::ok);
  return InfoElem::ok;
}

int PNNI_calling_party_soft_pvpc::operator == (const PNNI_calling_party_soft_pvpc & rhs) const
{
  return (_vpi == rhs._vpi && _vci == rhs._vci);
}

int PNNI_calling_party_soft_pvpc::equals(const InfoElem * him) const
{
  return him->equals(this);
}

int PNNI_calling_party_soft_pvpc::equals(const PNNI_calling_party_soft_pvpc * himptr) const
{
  return (*this == *himptr);
}

char PNNI_calling_party_soft_pvpc::PrintSpecific(ostream & os)
{
  os << "     VPI::" << _vpi << endl;
  os << "     VCI::" << _vci << endl;
  return 0;
}

ostream & operator << (ostream & os, PNNI_calling_party_soft_pvpc & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}

