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
static char const _qos_param_cc_rcsid_[] =
"$Id: qos_param.cc,v 1.7 1998/08/06 04:03:19 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/errmsg.h>
#include <codec/uni_ie/qos_param.h>

// QOS class information element 
ie_qos_param::ie_qos_param(qos_indicator fqos_class, qos_indicator bqos_class) : InfoElem(ie_qos_param_id),
  _fqos_class(fqos_class),_bqos_class(bqos_class) { }

ie_qos_param::ie_qos_param(const ie_qos_param & rhs) : InfoElem(ie_qos_param_id),
    _fqos_class(rhs._fqos_class), _bqos_class(rhs._bqos_class) { }

ie_qos_param::~ie_qos_param() { }


InfoElem* ie_qos_param::copy(void) const
{ return (new ie_qos_param(_fqos_class,_bqos_class)); }


int ie_qos_param::equals(const ie_qos_param *himptr) const
{
  return (himptr->_fqos_class == _fqos_class) && 
    (himptr->_bqos_class == _bqos_class);
}


int ie_qos_param::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}


int ie_qos_param::operator == (const ie_qos_param & rs) const
{
  return (rs._fqos_class == _fqos_class) && (rs._bqos_class == _bqos_class);
}

  
int ie_qos_param::Length()
{
  return (ie_header_len + 2);
}

  
int ie_qos_param::encode(u_char * buffer)
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
  put_8(temp,buflen,_fqos_class);
  put_8(temp,buflen,_bqos_class);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

/*
 * This information element has length 1 bytes
 */
InfoElem::ie_status ie_qos_param::decode(u_char * buffer, int & id)
{
  int fqos, bqos;
  id = buffer[0];
  short len = get_len(buffer);
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 2)
    return (InfoElem::incomplete);
  buffer += ie_header_len;
  get_8(buffer,fqos);
  get_8(buffer,bqos);
  // Encoding enforcement
  InfoElem::coding_std fc,bc;
  if(fqos == qos0)
    fc=InfoElem::itu;
  else
    fc=InfoElem::net;
  if(bqos == qos0)
    bc=InfoElem::itu;
  else
    bc=InfoElem::net;
  if ((fc!=bc) && (fc!=_coding))
    return (InfoElem::invalid_contents);
  switch(fqos) {
    case qos0:
    case qos1:
    case qos2:
    case qos3:
    case qos4:
      _fqos_class = (qos_indicator )fqos;
      break;
    default:
      return (InfoElem::invalid_contents);
      break;
  }
  switch(bqos) {
    case qos0:
    case qos1:
    case qos2:
    case qos3:
    case qos4:
      _bqos_class = (qos_indicator )bqos;
      return (InfoElem::ok);
      break;
    default:
      return (InfoElem::invalid_contents);
      break;
  }
  return (InfoElem::ok);
}

enum ie_qos_param::qos_indicator ie_qos_param::get_fqos_class(void) 
{return _fqos_class;}

enum ie_qos_param::qos_indicator ie_qos_param::get_bqos_class(void) 
{return _bqos_class;}

void ie_qos_param::set_fqos_class(qos_indicator fqos_class)
{
  _fqos_class = fqos_class;
  MakeStale();
}

void ie_qos_param::set_bqos_class(qos_indicator bqos_class)
{
  _bqos_class = bqos_class;
  MakeStale();
}

ie_qos_param::ie_qos_param(void) : InfoElem(ie_qos_param_id),
  _fqos_class(qos0),_bqos_class(qos0) {}

char ie_qos_param::PrintSpecific(ostream & os)
{
  os << endl << "   fqos::";
  switch (_fqos_class) {
  case qos0: os << "qos 0 " << endl; break;
  case qos1: os << "qos 1 " << endl; break;
  case qos2: os << "qos 2 " << endl; break;
  case qos3: os << "qos 3 " << endl; break;
  case qos4: os << "qos 4 " << endl; break;
  }
  os << "   bqos::";
  switch (_bqos_class) {
  case qos0: os << "qos 0 " << endl; break;
  case qos1: os << "qos 1 " << endl; break;
  case qos2: os << "qos 2 " << endl; break;
  case qos3: os << "qos 3 " << endl; break;
  case qos4: os << "qos 4 " << endl; break;
  }

  return 0;
}

ostream & operator << (ostream & os, ie_qos_param & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}
