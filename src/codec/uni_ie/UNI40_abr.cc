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
static char const _UNI40_abr_cc_rcsid_[] =
"$Id: UNI40_abr.cc,v 1.8 1998/08/06 23:14:32 marsh Exp $";
#endif
#include <common/cprototypes.h>
#include <iostream.h>
#include <codec/uni_ie/UNI40_abr.h>

// ABR Additional Parameters
int UNI40_abr_param::Length()
{
  return (ie_header_len + 10);
}

int UNI40_abr_param::encode(u_char * buffer)
{
  u_char * cptr = buffer;
  int buflen = 0;

  if ((cptr = LookupCache()) != 0L) {
    bcopy((char *)cptr, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  cptr = buffer + ie_header_len;
  put_id(buffer,_id);	
  put_coding(buffer,0x60);
  // see sec 10.1.2.1
  put_8(cptr,buflen,fwd_param_rec_id);
  put_32(cptr,buflen,_fwd_param);
  put_8(cptr,buflen,bwd_param_rec_id);
  put_32(cptr,buflen,_bwd_param);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return buflen;
}

InfoElem::ie_status UNI40_abr_param::decode(u_char * buffer, int  & id) 
{
  u_char *temp = buffer + ie_header_len;
  id = (buffer[0]);
  // buffer[1]: bit8 must be 1 and coding must be 11
  if(((buffer[1] & 0xE0) >> 5) != 0x07)
    return InfoElem::invalid_contents;
  int len = (buffer[2] << 8) | (buffer[3]);
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 10)
    return (InfoElem::incomplete);
  int aid;
  get_8(temp,aid);
  if(aid != 0xc2)
    return InfoElem::invalid_contents;
  get_32(temp,_fwd_param);
  get_8(temp,aid);
  if(aid != 0xc3)
    return InfoElem::invalid_contents;
  get_32(temp,_bwd_param);
  return InfoElem::ok; 
}


int UNI40_abr_param::operator == (const UNI40_abr_param & right) const
{
  return equals(&right);
}

int UNI40_abr_param::equals(const InfoElem * him) const
{
  return him->equals(this);
}

int UNI40_abr_param::equals(const UNI40_abr_param * himptr) const
{
  return (_fwd_param == himptr->_fwd_param &&
          _bwd_param == himptr->_bwd_param);
}

char UNI40_abr_param::PrintSpecific(ostream & os)
{
  os << endl;
  os << "   forward additional parameters record::"
     << ios::hex << _fwd_param << ios::dec << endl;
  os << "   backward additional parameters record::"
     << ios::hex << _bwd_param << ios::dec << endl;
  return 0;
}

ostream & operator << (ostream & os, UNI40_abr_param & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}


// ABR Setup Parameters
int UNI40_abr_setup::encode(u_char *buffer)
{
  u_char * cptr = buffer;
  int buflen = 0;

  if ((cptr = LookupCache()) != 0L) {
    bcopy((char *)cptr, (char *)buffer, (buflen = CacheLength()));
    cptr = buffer + buflen;
  } else {
    cptr = buffer + ie_header_len;
    // see sec 10.1.2.2
    if (_flags & fwd_icr) {
      *cptr++ = (u_char) forward_init_cell_rate_id;
      *cptr++ = (u_char) ((_fwd_icr >> 16) & 0xFF);
      *cptr++ = (u_char) ((_fwd_icr >>  8) & 0xFF);
      *cptr++ = (u_char)  (_fwd_icr & 0xFF);
      buflen += 4;	
    }
    if (_flags & bwd_icr) {
      *cptr++ = (u_char) backward_init_cell_rate_id;
      *cptr++ = (u_char) ((_bwd_icr >> 16) & 0xFF);
      *cptr++ = (u_char) ((_bwd_icr >>  8) & 0xFF);
      *cptr++ = (u_char)  (_bwd_icr & 0xFF);
      buflen += 4;
    }
    if (_flags & fwd_tbe) {
      *cptr++ = (u_char) forward_trans_buf_exp_id;
      *cptr++ = (u_char) ((_fwd_tbe >> 16) & 0xFF);
      *cptr++ = (u_char) ((_fwd_tbe >>  8) & 0xFF);
      *cptr++ = (u_char)  (_fwd_tbe & 0xFF);
      buflen += 4;    
    }
    if (_flags & bwd_tbe) {
      *cptr++ = (u_char) backward_trans_buf_exp_id;
      *cptr++ = (u_char) ((_bwd_tbe >> 16) & 0xFF);
      *cptr++ = (u_char) ((_bwd_tbe >>  8) & 0xFF);
      *cptr++ = (u_char)  (_bwd_tbe & 0xFF);
      buflen += 4;    
    }
    if (_flags & cum_rtt) {
      *cptr++ = (u_char) cumulative_fixed_rtt_id;
      *cptr++ = (u_char) ((_cum_rtt >> 16) & 0xFF);
      *cptr++ = (u_char) ((_cum_rtt >>  8) & 0xFF);
      *cptr++ = (u_char)  (_cum_rtt & 0xFF);
      buflen += 4;    
    }
    if (_flags & fwd_rif) {
      *cptr++ = (u_char) _fwd_rif;
      *cptr++ = (u_char) forward_rate_incr_id;
      buflen += 2;
    }
    if (_flags & bwd_rif) {
      *cptr++ = (u_char) _bwd_rif;
      *cptr++ = (u_char) backward_rate_incr_id;
      buflen += 2;
    }
    if (_flags & fwd_rdf) {
      *cptr++ = (u_char) _fwd_rdf;
      *cptr++ = (u_char) forward_rate_decr_id;
      buflen += 2;
    }
    if (_flags & bwd_rdf) {
      *cptr++ = (u_char) _bwd_rdf;
      *cptr++ = (u_char) backward_rate_decr_id;
      buflen += 2;
    }
    // encode the header now that we know the total length
    put_id(buffer,_id);
    put_coding(buffer,_coding);
    put_len(buffer, buflen);
    buflen += ie_header_len;
    FillCache(buffer, buflen);
  }
  return buflen;
}

int UNI40_abr_setup::Length()
{
  int buflen = ie_header_len;
  if (_flags & fwd_icr)
    buflen += 4;	
  if (_flags & bwd_icr)
    buflen += 4;
  if (_flags & fwd_tbe)
    buflen += 4;    
  if (_flags & bwd_tbe) 
    buflen += 4;    
  if (_flags & cum_rtt)
    buflen += 4;    
  if (_flags & fwd_rif)
    buflen += 2;
  if (_flags & bwd_rif)
    buflen += 2;
  if (_flags & fwd_rdf) 
    buflen += 2;
  if (_flags & bwd_rdf)
    buflen += 2;
  return buflen;
}


InfoElem::ie_status UNI40_abr_setup::decode(u_char *buffer, int & id)
{
  int i = 4;
  u_char temp;

   id = buffer[0];
  short len = get_len(buffer);

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);

  // reset the flags
  _flags = 0;

  // don't know what we're reading until we check the identifier...
  while (len-- > 1) {
    // temp = buffer[i++], len--;
    switch (buffer[i++]) {
      case forward_init_cell_rate_id:
        _fwd_icr  = (int)((buffer[i++] << 16) & 0x00FF0000);
        _fwd_icr |= (int)((buffer[i++] <<  8) & 0x0000FF00);
        _fwd_icr |= (int) (buffer[i++] & 0xFF);
	_flags |= fwd_icr;
        len -= 3;
        break;
      case backward_init_cell_rate_id:
	_bwd_icr  = (int)((buffer[i++] << 16) & 0x00FF0000);
	_bwd_icr |= (int)((buffer[i++] <<  8) & 0x0000FF00);
	_bwd_icr |= (int) (buffer[i++] & 0xFF);
	_flags |= bwd_icr;
        len -= 3;
	break;
      case forward_trans_buf_exp_id:
	_fwd_tbe  = (int)((buffer[i++] << 16) & 0x00FF0000);
	_fwd_tbe |= (int)((buffer[i++] <<  8) & 0x0000FF00);
	_fwd_tbe |= (int) (buffer[i++] & 0xFF);
	_flags |= fwd_tbe;
	len -= 3;
	break;
      case backward_trans_buf_exp_id:
	_bwd_tbe  = (int)((buffer[i++] << 16) & 0x00FF0000);
	_bwd_tbe |= (int)((buffer[i++] <<  8) & 0x0000FF00);
	_bwd_tbe |= (int) (buffer[i++] & 0xFF);
	_flags |= bwd_tbe;
	len -= 3;
	break;
      case cumulative_fixed_rtt_id:
	_cum_rtt  = (int)((buffer[i++] << 16) & 0x00FF0000);
	_cum_rtt |= (int)((buffer[i++] <<  8) & 0x0000FF00);
	_cum_rtt |= (int) (buffer[i++] & 0xFF);
	_flags |= cum_rtt;
	len -= 3;
	break;
      case forward_rate_incr_id:
	_fwd_rif = buffer[i++];
        _flags |= fwd_rif;
	len--;
	break;
      case backward_rate_incr_id:
	_bwd_rif = buffer[i++];
        _flags |= bwd_rif;
	len--;
	break;
      case forward_rate_decr_id:
	_fwd_rdf = buffer[i++];
        _flags |= fwd_rdf;
	len--;
	break;
      case backward_rate_decr_id:
	_bwd_rdf = buffer[i++];
        _flags |= bwd_rdf;
	len--;
	break;
      default:
	return InfoElem::invalid_contents;
	break;
    }
  }
  return InfoElem::ok; 
}


int UNI40_abr_setup::operator == (const UNI40_abr_setup & right) const
{
  return equals(&right);
}

int UNI40_abr_setup::equals(const InfoElem * him) const
{
  return him->equals(this);
}

int UNI40_abr_setup::equals(const UNI40_abr_setup * himptr) const
{
  return ((_fwd_icr == himptr->_fwd_icr) &&
          (_bwd_icr == himptr->_bwd_icr) &&
	  (_fwd_tbe == himptr->_fwd_tbe) &&
	  (_bwd_tbe == himptr->_bwd_tbe) &&
	  (_cum_rtt == himptr->_cum_rtt) &&
	  (_fwd_rif == himptr->_fwd_rif) &&
	  (_bwd_rif == himptr->_bwd_rif) &&
	  (_fwd_rdf == himptr->_fwd_rdf) &&
	  (_bwd_rdf == himptr->_bwd_rdf) &&
	  (_flags == himptr->_flags));
}

char UNI40_abr_setup::PrintSpecific(ostream & os)
{
  if (_flags & fwd_icr)
    os << endl << "   forward initial cell rate::" << hex << _fwd_icr << dec;
  if (_flags & bwd_icr)
    os << endl << "   backward initial cell rate::"<< hex << _bwd_icr << dec;
  if (_flags & fwd_tbe)
    os << endl
       << "   forward transient buffer exposure::" << hex << _fwd_tbe << dec;
  if (_flags & bwd_tbe)
    os << endl
       << "   backward transient buffer exposure::" << hex << _bwd_tbe << dec;
  if (_flags & cum_rtt)
    os << endl
       << "   cumulative RM fixed round trip time::" << hex << _cum_rtt << dec;
  if (_flags & fwd_rif)
    os << endl
       << "   forward rate increase factor::" << ios::hex << _fwd_rif << dec;
  if (_flags & bwd_rif)
    os << endl
       << "   backward rate increase factor::" << ios::hex << _bwd_rif << dec;
  if (_flags & fwd_rdf)
    os << endl
       << "   forward rate decrease factor::" << ios::hex << _fwd_rdf << dec;
  if (_flags & bwd_rdf)
    os << endl
       << "   backward rate decrease factor::" << ios::hex << _bwd_rdf << dec;
  os << endl;

  return 0;
}


ostream & operator << (ostream & os, UNI40_abr_setup & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}


