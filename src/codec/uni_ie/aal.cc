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
static char const _aal_cc_rcsid_[] =
"$Id: aal.cc,v 1.12 1998/08/06 23:18:25 marsh Exp $";
#endif
#include <common/cprototypes.h>
#include <codec/uni_ie/aal.h>
#include <codec/uni_ie/errmsg.h>

class aal5;

//================  AAL ===================================================
ie_aal::ie_aal(AAL_TYPE aal_type) :
  InfoElem(ie_aal_param_id),
  _aal_type(aal_type)
{
}


ie_aal::ie_aal(void):
  InfoElem(ie_aal_param_id),
  _aal_type((AAL_TYPE) 0)
{
}


ie_aal::~ie_aal() 
{
}


int ie_aal::operator == (const ie_aal& aal) const
{
    return (_aal_type == aal._aal_type);
}



enum ie_aal::AAL_TYPE ie_aal::get_aal_type(void) const 
{
  return _aal_type;
}

u_char ie_aal::IsVariableLen(void)
{
  return ((u_char)1);
}


aal5::aal5(void) : 
      ie_aal(aal5_type), _sscs_set(0), _sscs(sscs_null),
      _fm_cpcs_sdu_size_set(0),
      _bm_cpcs_sdu_size_set(0)
{
}


aal5::aal5(aal5_sscs sscs, int fm_sdu_size, int bm_sdu_size) :
  ie_aal(aal5_type),
  _sscs_set(1),
  _fm_cpcs_sdu_size_set(1),
  _bm_cpcs_sdu_size_set(1),
  _sscs(sscs),
  _fm_cpcs_sdu_size(fm_sdu_size),
  _bm_cpcs_sdu_size(bm_sdu_size)
{
}


aal5::aal5(const aal5 & old) : ie_aal(aal5_type), _sscs_set(old._sscs_set),
  _fm_cpcs_sdu_size_set(old._fm_cpcs_sdu_size_set),
  _bm_cpcs_sdu_size_set(old._bm_cpcs_sdu_size_set),
  _sscs(old._sscs),
  _fm_cpcs_sdu_size(old._fm_cpcs_sdu_size),
  _bm_cpcs_sdu_size(old._bm_cpcs_sdu_size)
{
}


aal5::~aal5() 
{
  // This isn't returning memory back somehow -- check out insight
}

const aal5 * aal5::get_aal5_type(void) const 
{
  return this;
}


enum aal5::aal5_sscs  aal5::get_sscs(void)
{
  return _sscs;
}


void aal5::set_sscs(aal5_sscs sscs)
{
  _sscs = sscs; 
  _sscs_set = 1;
  MakeStale();
};


int aal5::get_fm_cpcs_sdu_size(void) 
{ 
  return( _fm_cpcs_sdu_size_set?_fm_cpcs_sdu_size : -1); 
}


void aal5::set_fm_cpcs_sdu_size(int size) 
{ 
  _fm_cpcs_sdu_size = size;   
  _fm_cpcs_sdu_size_set = 1;
  MakeStale();
}


int aal5::get_bm_cpcs_sdu_size(void) 
{ 
  return( _bm_cpcs_sdu_size_set?_bm_cpcs_sdu_size : -1); 
}


void aal5::set_bm_cpcs_sdu_size(int size) 
{ 
  _bm_cpcs_sdu_size = size;   
  _bm_cpcs_sdu_size_set = 1;
  MakeStale();
}


int aal5::operator == (const ie_aal& aal) const
{
  return equals(&aal);
}


int aal5::equals(const InfoElem* him) const
{
  return him->equals(this);
}


int aal5::equals(const aal5* himptr) const
{
  if (_fm_cpcs_sdu_size_set) {
    if ((!himptr->_fm_cpcs_sdu_size_set) || 
        (_fm_cpcs_sdu_size != himptr->_fm_cpcs_sdu_size))
      return(0);
    if ((!himptr->_bm_cpcs_sdu_size_set) || !_bm_cpcs_sdu_size_set ||
	(_bm_cpcs_sdu_size != himptr->_bm_cpcs_sdu_size))
      return(0);
  } else {
    if (!himptr->_fm_cpcs_sdu_size_set)
      return(0);
    if (_bm_cpcs_sdu_size_set || himptr->_bm_cpcs_sdu_size_set)
      return(0);
  }
  if (_sscs_set)
    if ((!himptr->_sscs_set) || (_sscs != himptr->_sscs))
      return(0);
  return(1);
}

InfoElem* aal5::copy(void) const
{
  aal5 * temp = new aal5(*this);
  return ((ie_aal *) temp);
}


int aal5::Length()
{
  int  buflen = ie_header_len;
  buflen++;
  if (_fm_cpcs_sdu_size_set) 
    buflen += 6;
  if(_sscs_set)
    buflen += 2;
  return (buflen);
}


int aal5 :: encode(u_char * buffer)
{
  u_char * temp;
  int len = 0;

  if ((temp = LookupCache()) != 0L)
    {
      bcopy((char *)temp, (char *)buffer, (len = CacheLength()));
      buffer+= len;
      return(len);
    }
  temp = buffer + ie_header_len;
  // fill in part of the header
  put_id(buffer,_id);
  put_coding(buffer,_coding);

  put_8(temp,len, aal5_type);
  if (_fm_cpcs_sdu_size_set  || _bm_cpcs_sdu_size_set)
    {
      if(_fm_cpcs_sdu_size_set != _bm_cpcs_sdu_size_set)
	{
	  return(InfoElem::invalid_contents);
	}
    }
  if (_fm_cpcs_sdu_size_set) 
    {
      put_8(temp, len, fm_cpcs_sdu_size_id);
      put_16(temp, len, _fm_cpcs_sdu_size);
      put_8(temp,len, bm_cpcs_sdu_size_id);
      put_16(temp,len, _bm_cpcs_sdu_size);
    }
  if(_sscs_set)
    {
      put_8(temp, len,  sscs_id);
      put_8(temp, len,  _sscs);
    }
  // fill in the length now that we have it
  put_len(buffer,len);
  len += ie_header_len;
  FillCache(buffer, len);
  return (len);
}


InfoElem::ie_status aal5 :: decode(u_char * buffer, int & id)
{
  U8BitValue x;
  int i;
  u_char *temp = buffer + ie_header_len + 1;
  u_short fm_cpcs_sdu_size;
  u_short bm_cpcs_sdu_size;
  cout << "Decoding AAL5 IE:" << endl;
  id = buffer[0];
  short len = get_len(buffer);
  cout << "len = " << dec << len << endl;
  if (0 == len)
    {
      cout << "  empty AAL5" << endl;
      return (InfoElem::empty);
    }

  len--;
  if (len != 2 && len != 8 && len != 10)
    {
      cout << " AAL5 length is neither 2 nor 8 nor 10 " << endl;
      return (InfoElem::invalid_contents);
    }
  // at max we expect 4 octect groups
  for (i = 0; (i < 4) && (len != 0); i++)
    {
      if (len < 2)
	{
	  cout << " AAL5 length is less than  2 : len = " << len << endl;
	  return (InfoElem::invalid_contents);
	}
      get_8(temp,x);
      switch(x)
	{
	case fm_cpcs_sdu_size_id:
	  if (len < 3)
	    return (InfoElem::invalid_contents);
	   get_16(temp,_fm_cpcs_sdu_size);
	  _fm_cpcs_sdu_size_set = true;
	  len -= 3;
	  break;

	case bm_cpcs_sdu_size_id:
	  if (len < 3)
	    return (InfoElem::invalid_contents);
	   get_16(temp,_bm_cpcs_sdu_size);
	  _bm_cpcs_sdu_size_set = 1;
	  len -= 3;
	  break;
	case atm_mode_id: // UNI 3.0 compatibility
	  get_8(temp,x);
	  len -=2;
	  break;
	case sscs_id:
	  get_8(temp,x);
	  switch (x)
	    {
	    case sscs_null:
	    case sscs_assured:
	    case sscs_non_assured:
	    case sscs_frame_relay:
	      _sscs = (aal5::aal5_sscs) x;
	      _sscs_set = 1;
	      len -= 2;
	      break;
	    default:
	      cout << " AAL5 sscs type is unknown" << endl;
	      return (InfoElem::invalid_contents);
	    }
	  break;
	default:
	  return (InfoElem::invalid_contents);
	}
    }
  if (len)
    return (InfoElem::invalid_contents);
  if (_fm_cpcs_sdu_size_set != _bm_cpcs_sdu_size_set)
    {
      cout << " _fm_cpcs_sdu_size_set != _bm_cpcs_sdu_size_set " << endl;
      return (InfoElem::invalid_contents);
    }
  return (InfoElem::ok);
}


char aal5::PrintSpecific(ostream& os)
{
  os << endl;    
  if (_sscs_set){
    switch(_sscs){
    case sscs_null:
      os << "   sscs-type: sscs_null " << endl;
      break;
    case sscs_assured:
      os << "   sscs-type: sscs_assured "<< endl;
      break;
    case sscs_non_assured:
      os << "   sscs-type: sscs_non_assured "<< endl;
      break;
    case sscs_frame_relay:
      os << "   sscs-type: sscs_frame_relay "<< endl;
      break;
    default:
      break;
    }
  } else os << "   sscs-type: (unspecified) "<< endl;
  
  if (_fm_cpcs_sdu_size_set) 
    os << "   foward-sdu-size: " << _fm_cpcs_sdu_size<< endl;
  else  
    os << "   foward-sdu-size: (unspecified) " << endl;
  
  if (_bm_cpcs_sdu_size_set) 
    os << "   backward-sdu-size: " << _bm_cpcs_sdu_size << endl;
  else  
    os << "   backward-sdu-size: (unspecified) " << endl;
  
  return(0);
}


ostream& operator << (ostream& os, aal5& x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}










