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
#if !defined(LINT)
static char const _PNNI_called_party_pvpc_cc_rcsid_[] =
"$Id: PNNI_called_party_pvpc.cc,v 1.7 1998/08/06 23:13:58 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/PNNI_called_party_pvpc.h>

PNNI_called_party_soft_pvpc::PNNI_called_party_soft_pvpc(u_char sel, int vpi, int vci) :
  InfoElem(PNNI_called_party_soft_pvpc_id), _selection_type(sel), _vpi(vpi), 
  _vci(vci), _vci_present(false)
{
  _coding = InfoElem::net;
}

PNNI_called_party_soft_pvpc::PNNI_called_party_soft_pvpc(const PNNI_called_party_soft_pvpc & rhs) :
  InfoElem(PNNI_called_party_soft_pvpc_id), _selection_type(rhs._selection_type),
  _vpi(rhs._vpi), _vci(rhs._vci), _vci_present(rhs._vci_present) { }

PNNI_called_party_soft_pvpc::~PNNI_called_party_soft_pvpc(void) { }

PNNI_called_party_soft_pvpc * PNNI_called_party_soft_pvpc::copy(void) 
{ return (new PNNI_called_party_soft_pvpc(*this)); }

InfoElem * PNNI_called_party_soft_pvpc::copy(void) const 
{ return (InfoElem *)new PNNI_called_party_soft_pvpc(this->_vpi, this->_vci); } 

int PNNI_called_party_soft_pvpc::Length()
{

  int buflen = ie_header_len;
  buflen++;
  if (_selection_type != PNNI_called_party_soft_pvpc::AnyValue)
    buflen += 3;
  if(_vci_present)
    buflen += 3;
  return buflen;
}


int PNNI_called_party_soft_pvpc::encode(u_char *buffer)
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
  put_8(temp,buflen,_selection_type);
  if (_selection_type != PNNI_called_party_soft_pvpc::AnyValue)
    {
      put_8(temp,buflen,PNNI_called_party_soft_pvpc::VPI_ident);
      put_16(temp,buflen,_vpi);
    }
  if(_vci_present)
    {
      put_8(temp,buflen,PNNI_called_party_soft_pvpc::VCI_ident);
      put_16(temp,buflen,_vci);
    }
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return buflen;
}


InfoElem::ie_status PNNI_called_party_soft_pvpc::decode(u_char *buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if(len != 7)
    return (InfoElem::invalid_contents);
  u_char *temp = buffer + ie_header_len;
  get_8(temp,_selection_type);
  int vpi_id ;
  int vci_id ;
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
}

int PNNI_called_party_soft_pvpc::operator == (const PNNI_called_party_soft_pvpc & rhs) const
{
  return (_selection_type == rhs._selection_type &&
	  _vpi == rhs._vpi &&
	  _vci == rhs._vci);
}

int PNNI_called_party_soft_pvpc::equals(const InfoElem * him) const
{
  return (him->equals(this));
}

int PNNI_called_party_soft_pvpc::equals(const PNNI_called_party_soft_pvpc * himptr) const
{
  return (*this == *himptr);
}

char PNNI_called_party_soft_pvpc::PrintSpecific(ostream & os)
{
  os << "     Selection Type::" << _selection_type << endl;
  os << "     VPI::" << _vpi << endl;
  if (_vci_present)
    os << "     VCI::" << _vci << endl;
  return 0;
}

ostream & operator << (ostream & os, PNNI_called_party_soft_pvpc & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return(os);
}



