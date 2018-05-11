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
static char const _UNI40_bbc_rcsid_[] =
"$Id: UNI40_bbc.cc,v 1.3 1998/09/28 16:52:02 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <codec/uni_ie/UNI40_bbc.h>

ie_bbc::ie_bbc(Bbc_class bbc, clipping clip,conn_type ctype, trans_caps atc)
  : InfoElem(ie_broadband_bearer_cap_id), _qos_class(bbc), _clip(clip),
    _conn_type(ctype), _atc(atc) { }

ie_bbc::ie_bbc(const ie_bbc &him)
  : InfoElem(him)
{
  _qos_class = him._qos_class;
  _clip = him._clip;
  _conn_type = him._conn_type;
  _atc = him._atc;
}

ie_bbc::ie_bbc(void) 
  : InfoElem(ie_broadband_bearer_cap_id) { }

ie_bbc::~ie_bbc() { }

int ie_bbc::Length(void)
{
  return (ie_header_len+3);
}

enum ie_bbc::Bbc_class 
ie_bbc::get_qos_class(void) const
{return _qos_class;}

enum ie_bbc::clipping  
ie_bbc::get_clipping(void) const
{return _clip;}

enum ie_bbc::conn_type 
ie_bbc::get_conn_type(void) const
{return _conn_type;}

void  ie_bbc::set_qos_class(Bbc_class qos_class) 
{
  _qos_class = qos_class;
  MakeStale();
}

void  ie_bbc::set_clipping(clipping clip) 
{
  _clip = clip;
  MakeStale();
}

void  ie_bbc::set_conn_type(conn_type ctype) 
{
  _conn_type = ctype;
  MakeStale();
}


ie_bbc::trans_caps ie_bbc::get_ATC(void) const
{
  return _atc;
}

void   ie_bbc::set_ATC(ie_bbc::trans_caps atc)
{
  _atc = atc;
  MakeStale();
}


char ie_bbc::PrintSpecific(ostream & os)
{
  os << endl << "   qos_class::";
  switch (_qos_class) {
    case BCOB_A: os << "BCOB_A ";  break;
    case BCOB_C: os << "BCOB_C ";  break;
    case BCOB_X: os << "BCOB_X ";  break;
    case BCOB_VP: os << "BCOB_VP ";  break;
    default:     os << "unknown "; break;
  }
  os << endl;
  os << "ATC: " << _atc << endl;

  os << endl << "   clip::";
  switch (_clip) {
    case not_clipped: os << "not clipped "; break;
    case clipped:     os << "clipped ";     break;
    default:          os << "unknown ";     break;
  }
  os << endl << "   conn_type::";
  switch (_conn_type) {
    case p2p:  os << "point to point ";      break;
    case p2mp: os << "point to multi-point ";break;
    default:   os << "unknown ";             break;
  }
  os << endl;
  return 0;
}

int ie_bbc::equals(const ie_bbc * himptr) const
{
  if (_qos_class != himptr->_qos_class)
    return(0);
  if (_atc != himptr->_atc)
    return(0);
  if (_clip != himptr->_clip)
    return(0);
  if (_conn_type != himptr->_conn_type)
    return(0);
  return 1;
}


int ie_bbc::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}

InfoElem * ie_bbc::copy(void) const {
  return new ie_bbc(_qos_class, _clip, _conn_type,_atc);
}

int ie_bbc::encode(u_char * buffer)
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
  put_len(buffer,3);
  put_8(temp,buflen, _qos_class);
  put_8(temp,buflen, (0x80 |_atc));
  put_8(temp,buflen, (0x80 | _clip | _conn_type));
  buflen += ie_header_len;
  FillCache(buffer, buflen); 
  return (buflen);
}

InfoElem::ie_status ie_bbc::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  Bbc_class bbc;
  clipping clip;
  conn_type ctype;
  u_char atc;

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 3)
    return (InfoElem::invalid_contents);

  buffer += ie_header_len;

  bbc = (ie_bbc::Bbc_class )*buffer++;
  atc = *buffer++;
  clip = (ie_bbc::clipping ) (*buffer & 0x60);
  ctype = (ie_bbc::conn_type )(*buffer++ & 0x03);

  if ((bbc & 0x80) == 0x80)
    {
      return (InfoElem::invalid_contents);
    }
  int bbc_int = (int) bbc;
  bbc_int &= 0x1F;
  bbc = (ie_bbc::Bbc_class) bbc_int;

  switch(bbc)
    {
    case BCOB_A:
    case BCOB_C:
    case BCOB_X:
    case BCOB_VP:
      _qos_class = bbc;
      break;
    default:
      return (InfoElem::invalid_contents);      
    }

  if ((atc & 0x80) != 0x80)
    {
      return (InfoElem::invalid_contents);
    }
  atc &= 0x7F; // clear bit 8
  switch(atc)
    {
    case  CBR:
    case  CBR_4:
    case  CBR_6:
    case  CBR_CLR:
    case  RT_VBR:
    case  RT_VBR_1:
    case  RT_VBR_CLR:
    case  NonRT_VBR:
    case  NonRT_VBR_0:
    case  NonRT_VBR_2:
    case  NonRT_VBR_8:
    case  NonRT_VBR_CLR:
    case  ABR:
      _atc = (trans_caps)atc;
      break;
    default:
      return (InfoElem::invalid_contents);
    }

  int clip_int = (int) clip;
  clip_int &= 0x60;
  clip = (ie_bbc::clipping) clip_int;

  switch(clip) 
    {
    case not_clipped:
    case clipped:
      _clip = clip;
      break;
    default:
      return (InfoElem::invalid_contents);
    }
  
  int ctype_int = (int) ctype;
  ctype_int &= 0x03;
  ctype = (ie_bbc::conn_type) ctype_int;

  switch (ctype)
    {
    case p2p:
    case p2mp:
      _conn_type = ctype;
      break;
    default:
      return (InfoElem::invalid_contents);
    }
  return (InfoElem::ok);
}
