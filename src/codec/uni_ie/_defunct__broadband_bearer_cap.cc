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
static char const _broadband_bearer_cap_cc_rcsid_[] =
"$Id: _defunct__broadband_bearer_cap.cc,v 1.1 1998/09/28 09:12:00 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/broadband_bearer_cap.h>
#include <codec/uni_ie/errmsg.h>

//================  Broadband Bearer Capability ===========================
//---------------- ie_broadband_bearer_cap --------------------------------
ie_broadband_bearer_cap::ie_broadband_bearer_cap(Bbc_class bbc, clipping clip,
						 conn_type ctype)
  : InfoElem(ie_broadband_bearer_cap_id), _qos_class(bbc), _clip(clip),
    _conn_type(ctype), _atc(-1) { }

ie_broadband_bearer_cap::ie_broadband_bearer_cap(Bbc_class qos_class)
  : InfoElem(ie_broadband_bearer_cap_id), _qos_class(qos_class), 
    _clip(ie_broadband_bearer_cap::not_clipped), 
    _conn_type(ie_broadband_bearer_cap::p2p), _atc(-1) { }

ie_broadband_bearer_cap::ie_broadband_bearer_cap(const ie_broadband_bearer_cap 
						 &him)
  : InfoElem(him)
{
  _qos_class = him._qos_class;
  _clip = him._clip;
  _conn_type = him._conn_type;
  _atc = him._atc;
}

ie_broadband_bearer_cap::~ie_broadband_bearer_cap() { }

int ie_broadband_bearer_cap::Length(void)
{
  int rval = 0;

  switch (_qos_class) {
    case BCOB_A:
    case BCOB_C:
      rval = (ie_header_len+2);
      break;
    case BCOB_X:
    case BCOB_VP:
      rval = (ie_header_len+3);
      break;
  }
  return rval;
}

enum ie_broadband_bearer_cap::Bbc_class 
ie_broadband_bearer_cap::get_qos_class(void) const
{return _qos_class;}

enum ie_broadband_bearer_cap::clipping  
ie_broadband_bearer_cap::get_clipping(void) const
{return _clip;}

enum ie_broadband_bearer_cap::conn_type 
ie_broadband_bearer_cap::get_conn_type(void) const
{return _conn_type;}

void  ie_broadband_bearer_cap::set_qos_class(Bbc_class qos_class) 
{
  _qos_class = qos_class;
  MakeStale();
}

void  ie_broadband_bearer_cap::set_clipping(clipping clip) 
{
  _clip = clip;
  MakeStale();
}

void  ie_broadband_bearer_cap::set_conn_type(conn_type ctype) 
{
  _conn_type = ctype;
  MakeStale();
}


char ie_broadband_bearer_cap::get_ATC(void) const
{
  return _atc;
}

void   ie_broadband_bearer_cap::set_ATC(char atc)
{
  if (atc != CBR && atc != CBR_CLR && atc != RT_VBR &&
      atc != RT_VBR_CLR && atc != NonRT_VBR && atc != NonRT_VBR_CLR &&
      atc != ABR && atc != 0 && atc != 1 && atc != 2 && atc != 4 &&
      atc != 6 && atc != 8)
    _atc = -1;
  else
    _atc = atc;
}


char ie_broadband_bearer_cap::PrintSpecific(ostream & os)
{
  os << endl << "   qos_class::";
  switch (_qos_class) {
    case BCOB_A: os << "BCOB_A ";  break;
    case BCOB_C: os << "BCOB_C ";  break;
    case BCOB_X: os << "BCOB_X ";  break;
    case BCOB_VP: os << "BCOB_VP ";  break;
    default:     os << "unknown "; break;
  }
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

//---------------- A_Bbc --------------------------------------------------
A_Bbc::A_Bbc(clipping clip, conn_type ctype) :
  ie_broadband_bearer_cap(BCOB_A,clip,ctype) { }

A_Bbc::A_Bbc(void) : ie_broadband_bearer_cap(BCOB_A) { }

A_Bbc::A_Bbc(const A_Bbc &him) : ie_broadband_bearer_cap(him) { 
}
A_Bbc::~A_Bbc() { }

InfoElem * A_Bbc::copy(void) const {return new A_Bbc(*this);}

int A_Bbc::equals(const A_Bbc *himptr) const
{
  if (_qos_class != himptr->_qos_class)
    return(0);
  if (_clip != himptr->_clip)
    return(0);
  if (_conn_type != himptr->_conn_type)
    return(0);
  return 1;
}


int A_Bbc::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}


int A_Bbc::operator == (const A_Bbc & a_bbc) const
{
  if (_qos_class != a_bbc._qos_class)
    return(0);
  if (_clip != a_bbc._clip)
    return(0);
  if (_conn_type != a_bbc._conn_type)
    return(0);
  return 1;
} 

const ie_broadband_bearer_cap * A_Bbc::get_bbc_class(void) const {return this;}


int A_Bbc::encode(u_char * buffer)
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
  put_8(temp,buflen, (0x80 | _qos_class));
  put_8(temp,buflen, (0x80 | _clip | _conn_type));
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen); 
  return (buflen);
}

InfoElem::ie_status A_Bbc::decode(u_char * buffer, int & id)
{
         id = buffer[0];
  short len = get_len(buffer);
  Bbc_class bbc;
  clipping clip;
  conn_type ctype;

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 2)
    return (InfoElem::invalid_contents);
  buffer += ie_header_len;
  
  bbc = (Bbc_class)(*buffer & 0x1F);
  if (bbc != BCOB_A)
    return (InfoElem::invalid_contents);
  if ((*buffer++ & 0x80) != 0x80)
    return (InfoElem::invalid_contents);
  // parsing susceptibility to clipping
  clip = ((clipping )((*buffer) & 0x60));
  switch (clip) {
    case not_clipped: // 0x00
    case clipped:     // 0x20
      _clip = (ie_broadband_bearer_cap::clipping) clip;
      break;
    default:
      return (InfoElem::invalid_contents);
  }
  ctype = (conn_type )((*buffer) & 0x03);
  switch (ctype) {
    case p2p:
    case p2mp:
      _conn_type = (ie_broadband_bearer_cap::conn_type)ctype;
      break;
    default:
      return (InfoElem::invalid_contents);
  }
  return (InfoElem::ok);
} 

char A_Bbc::PrintSpecific(ostream & os)
{
  ie_broadband_bearer_cap::PrintSpecific(os);
  return 0;
}

ostream & operator << (ostream & os, A_Bbc & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);

  return (os);
}

//---------------- C_Bbc --------------------------------------------------
C_Bbc::C_Bbc(clipping clip,
	conn_type ctype) : ie_broadband_bearer_cap(BCOB_C,clip,ctype) {} 

C_Bbc::C_Bbc(void):ie_broadband_bearer_cap(BCOB_C) {}

C_Bbc::C_Bbc(const C_Bbc &him) : ie_broadband_bearer_cap(him) {}

InfoElem* C_Bbc::copy(void) const {return new C_Bbc(*this);}

C_Bbc::~C_Bbc() {}

int C_Bbc::equals(const C_Bbc *himptr) const
{
  if(_qos_class != himptr->_qos_class)
    return(0);
  if(_clip != himptr->_clip)
    return(0);
  if(_conn_type != himptr->_conn_type)
    return(0);
  return 1;

}


int C_Bbc::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}


int C_Bbc::operator == (const C_Bbc & c_bbc) const
{
  if(_qos_class != c_bbc._qos_class)
    return(0);
  if(_clip != c_bbc._clip)
    return(0);
  if(_conn_type != c_bbc._conn_type)
    return(0);
  return 1;
} 

const ie_broadband_bearer_cap * C_Bbc::get_bbc_class(void) const {return this;}

int C_Bbc::encode(u_char * buffer)
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
  put_8(temp,buflen, (0x80 | _qos_class));
  put_8(temp,buflen,(0x80 | _clip | _conn_type));
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen); 
  return (buflen);
}

InfoElem::ie_status C_Bbc::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  Bbc_class bbc;
  clipping clip;
  conn_type ctype;

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 2)
    return(InfoElem::invalid_contents);
  buffer += ie_header_len;
  bbc = (Bbc_class )(*buffer & 0x1F);
  if (bbc != BCOB_C)
    return(InfoElem::invalid_contents);
  if ((*buffer++ & 0x80) != 0x80)
    return(InfoElem::invalid_contents);
  // parsing susceptibility to clipping
  clip = ((clipping )((*buffer) & 0x60));
  switch(clip) {
    case not_clipped:
    case clipped:
      _clip = (ie_broadband_bearer_cap::clipping) clip;
      break;
    default:
      return(InfoElem::invalid_contents);
  }
  ctype = (conn_type )((*buffer) & 0x03);
  switch (ctype) {
    case p2p:
    case p2mp:
      _conn_type = (ie_broadband_bearer_cap::conn_type)ctype;
      break;
    default:
      return(InfoElem::invalid_contents);
  }
  return (InfoElem::ok);
} 


char C_Bbc::PrintSpecific(ostream & os)
{
  ie_broadband_bearer_cap::PrintSpecific(os);
  return 0;
}

ostream & operator << (ostream & os, C_Bbc & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}

//---------------- X_Bbc --------------------------------------------------
X_Bbc::X_Bbc(clipping clip, conn_type ctype, traffic_type tp, timing_req tr) 
  : ie_broadband_bearer_cap(BCOB_X,clip,ctype), _tp(tp),_tr(tr) { } 

X_Bbc::X_Bbc(void) : ie_broadband_bearer_cap(BCOB_X), 
    _tp(X_Bbc::no_tp), _tr(X_Bbc::no_timing) { }

X_Bbc::X_Bbc(const X_Bbc &him) : ie_broadband_bearer_cap(him) 
{
  _tp = him._tp;
  _tr = him._tr;
}

X_Bbc::~X_Bbc() {}


enum X_Bbc::traffic_type X_Bbc::get_traffic_type(void) {return _tp;}

enum X_Bbc::timing_req X_Bbc::get_timing_req(void) {return _tr;}


int X_Bbc::equals(const X_Bbc *himptr) const
{
  if(_qos_class != himptr->_qos_class)
    return(0);
  if(_clip != himptr->_clip)
    return(0);
  if(_conn_type != himptr->_conn_type)
    return(0);
  if(_tp != himptr->_tp)
    return(0);
  if(_tr != himptr->_tr)
    return(0);
  return 1;
}


int X_Bbc::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}


int X_Bbc::operator == (const X_Bbc & x_bbc) const
{
  if(_qos_class != x_bbc._qos_class)
    return(0);
  if(_clip != x_bbc._clip)
    return(0);
  if(_conn_type != x_bbc._conn_type)
    return(0);
  if(_tp != x_bbc._tp)
    return(0);
  if(_tr != x_bbc._tr)
    return(0);
  return 1;
} 

InfoElem*  X_Bbc::copy(void) const {return new X_Bbc(*this);}

const ie_broadband_bearer_cap *  X_Bbc::get_bbc_class(void) const {return this;}

int X_Bbc::encode(u_char * buffer)
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
  put_8(temp,buflen,(0x00 | _qos_class));
  put_8(temp,buflen,(0x80 | _tp | _tr));
  put_8(temp,buflen,(0x80 | _clip | _conn_type));
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

// this class has the extra byte 5a

InfoElem::ie_status X_Bbc::decode(u_char * buffer, int & id)
{
       id = buffer[0];
  short len = get_len(buffer);
  Bbc_class bbc;
  clipping clip;
  conn_type ctype;
  traffic_type tp;
  timing_req tr;

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 3)
    return (InfoElem::invalid_contents);
  buffer += ie_header_len;
  // qos_class must be BCOB_X
  bbc = (Bbc_class )(*buffer & 0x1F);
  if (bbc != BCOB_X)
    return (InfoElem::invalid_contents);
  // parsing byte 5.a
  if ((*buffer++ & 0x80) == 0x80)
    return (InfoElem::invalid_contents);

  // notice comparisons are done in place no need to shift
   tp = (traffic_type )((*buffer) & 0x1c);
  switch(tp) {
    case no_tp:
    case CBR:
    case VBR:
      _tp = (X_Bbc::traffic_type)tp;
      break;
    default:
      return (InfoElem::invalid_contents);
  }
  // parsing timing requirements
  tr = (timing_req )((*buffer++) & 0x03);
  switch (tr) {
    case no_timing:
    case end_to_end_required:
    case end_to_end_not_required:
    case reserved:
      _tr = (X_Bbc::timing_req)tr;
      break;
    default:
      return (InfoElem::invalid_contents);
  }

  // parsing byte 6
  if ((*buffer & 0x80) != 0x80)
    return (InfoElem::invalid_contents);

  // parsing susceptibility to clipping
  clip = ((clipping )((*buffer) & 0x60));
  switch(clip) {
    case not_clipped:
    case clipped:
      _clip = (ie_broadband_bearer_cap::clipping) clip;
      break;
    default:
      return (InfoElem::invalid_contents);
  }
  ctype = (conn_type )((*buffer) & 0x03);
  switch (ctype) {
    case p2p:
    case p2mp:
      _conn_type = (ie_broadband_bearer_cap::conn_type)ctype;
      break;
    default:
      return (InfoElem::invalid_contents);
  }
  return (InfoElem::ok);
} 


char X_Bbc::PrintSpecific(ostream & os)
{
  ie_broadband_bearer_cap::PrintSpecific(os);

  os << "   traffic_type::";
  switch (_tp) {
  case no_tp: os << "no type "; break;
  case CBR:   os << "CBR ";     break;
  case VBR:   os << "VBR ";     break;
  default:    os << "unknown "; break;
  }
  os << endl << "   timing_req::";
  switch (_tr) {
  case no_timing:               os << "no timing ";          break;
  case end_to_end_required:     os << "end to end req ";     break;
  case end_to_end_not_required: os << "end to end not req "; break;
  case reserved:                os << "reserved ";           break;
  default:                      os << "unknown ";            break;
  }
  os << endl;
  return 0;
}

ostream & operator << (ostream & os, X_Bbc & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}


//---------------- VP_Bbc --------------------------------------------------
VP_Bbc::VP_Bbc(clipping clip, conn_type ctype, traffic_type tp, timing_req tr) 
  : ie_broadband_bearer_cap(BCOB_X,clip,ctype), _tp(tp),_tr(tr) { } 

VP_Bbc::VP_Bbc(void) : ie_broadband_bearer_cap(BCOB_X), 
    _tp(VP_Bbc::no_tp), _tr(VP_Bbc::no_timing) { }

VP_Bbc::VP_Bbc(const VP_Bbc &him) : ie_broadband_bearer_cap(him)
{
  _tp = him._tp;
  _tr = him._tr;
}

VP_Bbc::~VP_Bbc() {}


enum VP_Bbc::traffic_type VP_Bbc::get_traffic_type(void) {return _tp;}

enum VP_Bbc::timing_req VP_Bbc::get_timing_req(void) {return _tr;}


int VP_Bbc::equals(const VP_Bbc *himptr) const
{
  if(_qos_class != himptr->_qos_class)
    return(0);
  if(_clip != himptr->_clip)
    return(0);
  if(_conn_type != himptr->_conn_type)
    return(0);
  if(_tp != himptr->_tp)
    return(0);
  if(_tr != himptr->_tr)
    return(0);
  return 1;
}


int VP_Bbc::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}


int VP_Bbc::operator == (const VP_Bbc & VP_Bbc) const
{
  if(_qos_class != VP_Bbc._qos_class)
    return(0);
  if(_clip != VP_Bbc._clip)
    return(0);
  if(_conn_type != VP_Bbc._conn_type)
    return(0);
  if(_tp != VP_Bbc._tp)
    return(0);
  if(_tr != VP_Bbc._tr)
    return(0);
  return 1;
} 

InfoElem*  VP_Bbc::copy(void) const {return new VP_Bbc(*this);}

const ie_broadband_bearer_cap *  VP_Bbc::get_bbc_class(void) const {return this;}

int VP_Bbc::encode(u_char * buffer)
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
  put_8(temp,buflen,(0x00 | _qos_class));
  put_8(temp,buflen,(0x80 | _tp | _tr));
  put_8(temp,buflen,(0x80 | _clip | _conn_type));
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

// this class has the extra byte 5a

InfoElem::ie_status VP_Bbc::decode(u_char * buffer, int & id)
{
       id = buffer[0];
  short len = get_len(buffer);
  Bbc_class bbc;
  clipping clip;
  conn_type ctype;
  traffic_type tp;
  timing_req tr;

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 3)
    return (InfoElem::invalid_contents);
  buffer += ie_header_len;
  // qos_class must be BCOB_X
  bbc = (Bbc_class )(*buffer & 0x1F);
  if (bbc != BCOB_X)
    return (InfoElem::invalid_contents);
  // parsing byte 5.a
  if ((*buffer++ & 0x80) == 0x80)
    return (InfoElem::invalid_contents);

  // notice comparisons are done in place no need to shift
   tp = (traffic_type )((*buffer) & 0x1c);
  switch(tp) {
    case no_tp:
    case CBR:
    case VBR:
      _tp = (VP_Bbc::traffic_type)tp;
      break;
    default:
      return (InfoElem::invalid_contents);
  }
  // parsing timing requirements
  tr = (timing_req )((*buffer++) & 0x03);
  switch (tr) {
    case no_timing:
    case end_to_end_required:
    case end_to_end_not_required:
    case reserved:
      _tr = (VP_Bbc::timing_req)tr;
      break;
    default:
      return (InfoElem::invalid_contents);
  }

  // parsing byte 6
  if ((*buffer & 0x80) != 0x80)
    return (InfoElem::invalid_contents);

  // parsing susceptibility to clipping
  clip = ((clipping )((*buffer) & 0x60));
  switch(clip) {
    case not_clipped:
    case clipped:
      _clip = (ie_broadband_bearer_cap::clipping) clip;
      break;
    default:
      return (InfoElem::invalid_contents);
  }
  ctype = (conn_type )((*buffer) & 0x03);
  switch (ctype) {
    case p2p:
    case p2mp:
      _conn_type = (ie_broadband_bearer_cap::conn_type)ctype;
      break;
    default:
      return (InfoElem::invalid_contents);
  }
  return (InfoElem::ok);
} 


char VP_Bbc::PrintSpecific(ostream & os)
{
  ie_broadband_bearer_cap::PrintSpecific(os);

  os << "   traffic_type::";
  switch (_tp) {
  case no_tp: os << "no type "; break;
  case CBR:   os << "CBR ";     break;
  case VBR:   os << "VBR ";     break;
  default:    os << "unknown "; break;
  }
  os << endl << "   timing_req::";
  switch (_tr) {
  case no_timing:               os << "no timing ";          break;
  case end_to_end_required:     os << "end to end req ";     break;
  case end_to_end_not_required: os << "end to end not req "; break;
  case reserved:                os << "reserved ";           break;
  default:                      os << "unknown ";            break;
  }
  os << endl;
  return 0;
}

ostream & operator << (ostream & os, VP_Bbc & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}
