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
static char const _UNI40_xqos_cc_rcsid_[] =
"$Id: UNI40_xqos.cc,v 1.7 1998/08/06 23:26:29 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/UNI40_xqos.h>

UNI40_xqos_param::UNI40_xqos_param(origin o, fb_ind fb, int fv,int bv,int fcv, int bcv,int fclr,int bclr) : 
  InfoElem(UNI40_xqos_param_id), _o(o),
  _fv(fv),_bv(bv), _fcv(fcv), _bcv(bcv) ,_fclr(fclr),_bclr(bclr)
{
  _fb = both;
}

UNI40_xqos_param::UNI40_xqos_param(const UNI40_xqos_param & rhs) : InfoElem(UNI40_xqos_param_id),
  _o(rhs._o), _fv(rhs._fv), _bv(rhs._bv), _fcv(rhs._fcv), _bcv(rhs._bcv), _fclr(rhs._fclr), _bclr(rhs._bclr),
  _fb(rhs._fb)
{
}

UNI40_xqos_param::UNI40_xqos_param(origin o, fb_ind fb, int v, int cv, int clr) : InfoElem(UNI40_xqos_param_id)
{
  _o = o;
  if(fb == forw)
    {
      _fb = forw;
      _fv = v;
      _fcv = cv;
      _fclr = clr;
    }
  else
    {
      _fb = back;
      _bv = v;
      _bcv = cv;
      _bclr = clr;
    }
}


UNI40_xqos_param::~UNI40_xqos_param() {}


UNI40_xqos_param * UNI40_xqos_param::copy(void)
{ return (new UNI40_xqos_param(_o,_fb,_fv,_bv,_fcv,_bcv,_fclr,_bclr)); }

InfoElem * UNI40_xqos_param::copy(void) const
{ return (InfoElem *)(new UNI40_xqos_param(_o,_fb,_fv,_bv,_fcv,_bcv,_fclr,_bclr)); }

int UNI40_xqos_param::equals(const UNI40_xqos_param *himptr) const
{
  return ((himptr->_fb == _fb) && 
	  (himptr->_fv == _fv) && 
          (himptr->_bv == _bv) &&
          (himptr->_fcv == _fcv) &&
          (himptr->_bcv == _bcv) &&
          (himptr->_fclr == _fclr) &&
	  (himptr->_bclr == _bclr));
}


int UNI40_xqos_param::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}


int UNI40_xqos_param::operator == (const UNI40_xqos_param & rs) const
{
  return ((rs._fb == _fb) && 
          (rs._fv == _fv) &&
          (rs._bv == _bv) &&

          (rs._fcv == _fcv) &&
          (rs._bcv == _bcv) &&

          (rs._fclr == _fclr) &&
          (rs._bclr == _bclr));
}

  
int UNI40_xqos_param::Length()
{
  int buflen = ie_header_len;
  buflen++;
  switch(_fb)
    {
    case forw:
    case back:
      // cdv 4
      // cumul cdv 4
      // clr 2
      buflen += 10;
      break;
    case both:
      buflen += 20;
      break;
    }
  return (buflen);
}


int UNI40_xqos_param::encode(u_char * buffer)
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
  put_coding(buffer,0x60);
  put_8(temp,buflen,_o);
  switch(_fb)
    {
    case forw:
      // cdv
      put_8(temp,buflen,AFP2PCDV);
      put_24(temp,buflen,_fv);
      // cumul cdv
      put_8(temp,buflen,CFP2PCDV);
      put_24(temp,buflen,_fcv);
      // clr
      put_8(temp,buflen,AFCLR);
      put_8(temp,buflen,_fclr);
      break;

    case back:
      // cdv
      put_8(temp,buflen,ABP2PCDV);
      put_24(temp,buflen,_bv);
      //cumul cdv
      put_8(temp,buflen,CBP2PCDV);
      put_24(temp,buflen,_bcv);
      // clr
      put_8(temp,buflen,ABCLR);
      put_8(temp,buflen,_bclr);
      break;

    case both:
      put_8(temp,buflen,AFP2PCDV);
      put_24(temp,buflen,_fv);

      put_8(temp,buflen,ABP2PCDV);
      put_24(temp,buflen,_bv);

      put_8(temp,buflen,CFP2PCDV);
      put_24(temp,buflen,_fcv);

      put_8(temp,buflen,CBP2PCDV);
      put_24(temp,buflen,_bcv);

      put_8(temp,buflen,AFCLR);
      put_8(temp,buflen,_fclr);

      put_8(temp,buflen,ABCLR);
      put_8(temp,buflen,_bclr);
      break;
    }
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

/*
 * This information element has length 1 bytes
 */
InfoElem::ie_status UNI40_xqos_param::decode(u_char * buffer, int & id)
{
  int x;
  short len = get_len(buffer);
  id = buffer[0];
  // buffer[1]: bit8 must be 1 and coding must be 11
  if(((buffer[1] & 0xE0) >> 5) != 0x07)
    return InfoElem::invalid_contents;
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  buffer += ie_header_len;
  get_8tc(buffer,_o, enum UNI40_xqos_param::origin);
  len--;
  for (int i =0; i < 6; i++) {
    if (!len)
      break;
    get_8(buffer,x);
    len--;
    switch (x) {
    case AFP2PCDV:
      if (len < 3)
	return InfoElem::invalid_contents;
      get_24(buffer,_fv);
      len -= 3;
      _fb = (enum UNI40_xqos_param::fb_ind)((int)_fb | forw);
      break;
    case ABP2PCDV:
      if (len < 3)
	return InfoElem::invalid_contents;
      get_24(buffer,_bv);
      len -= 3;
      _fb = (enum UNI40_xqos_param::fb_ind)((int)_fb | back);
      break;
    case CFP2PCDV:
      if ((_fb & forw) == 0)
	return InfoElem::invalid_contents;
      else {
	if (len < 3)
	  return InfoElem::invalid_contents;
	get_24(buffer,_fcv);
	len -= 3;
      }
      break;
    case CBP2PCDV:
      if ((_fb & back) == 0)
	return InfoElem::invalid_contents;
      else {
	if (len < 3)
	  return InfoElem::invalid_contents;
	get_24(buffer,_bcv);
	len -= 3;
      }
      break;
    case AFCLR:
      if ((_fb & forw) == 0)
	return InfoElem::invalid_contents;
      else {
	get_8(buffer,_fclr);
	len--;
      }
      break;
    case ABCLR:
      if ((_fb & back) == 0)
	return InfoElem::invalid_contents;
      else {
	get_8(buffer,_bclr);
	len--;
      }
      break;
    default:
      return (InfoElem::invalid_contents);
    }
  }
  if (len > 0)
    return (InfoElem::invalid_contents);
  return (InfoElem::ok);
}

int UNI40_xqos_param::get_forw_cdv(){ return _fv;}
int UNI40_xqos_param::get_back_cdv(){ return _bv;}
int UNI40_xqos_param::get_forw_cumul_cdv(){ return _fcv;}
int UNI40_xqos_param::get_back_cumul_cdv(){ return _bcv;}
int UNI40_xqos_param::get_forw_clr(){ return _fclr;}
int UNI40_xqos_param::get_back_clr(){ return _bclr;}

UNI40_xqos_param::origin UNI40_xqos_param::get_origin(){ return _o;}

void UNI40_xqos_param::set_forw_cdv(int val)
{
  _fv = val;
  _fb = (enum UNI40_xqos_param::fb_ind)((int)_fb | forw);
  MakeStale();
}

void UNI40_xqos_param::set_back_cdv(int val)
{
  _bv = val;
  _fb = (enum UNI40_xqos_param::fb_ind)((int)_fb | back);
  MakeStale();
}

void UNI40_xqos_param::set_forw_cumul_cdv(int val)
{
  _fcv = val;
  _fb = (enum UNI40_xqos_param::fb_ind)((int)_fb | forw);
  MakeStale();
}

void UNI40_xqos_param::set_back_cumul_cdv(int val)
{
  _bcv = val;
  _fb = (enum UNI40_xqos_param::fb_ind)((int)_fb | back);
  MakeStale();
}

void UNI40_xqos_param::set_forw_clr(int val)
{
  _fclr = val;
  _fb = (enum UNI40_xqos_param::fb_ind)((int)_fb | forw);
  MakeStale();
}

void UNI40_xqos_param::set_back_clr(int val)
{
  _bclr = val;
  _fb = (enum UNI40_xqos_param::fb_ind)((int)_fb | back);
  MakeStale();
}

void UNI40_xqos_param::set_origin(origin o)
{
  _o = o;
  MakeStale();
}


char UNI40_xqos_param::PrintSpecific(ostream & os)
{
   os << endl << "    origin: " << (_o ? (_o == inet ? "inet" : "error") : "user") << endl;
   switch(_fb) {
    case forw:
      os << "    forw cdv: " << dec <<  _fv << endl;
      os << "    forw cumul cdv: " << dec << _fcv << endl;
      os << "    forw clr: " << dec << _fclr << endl;
      break;
    case back:
      os << "    back cdv: " << dec << _fv << endl;
      os << "    back cumul cdv: " << dec << _fcv << endl;
      os << "    back clr: " << dec << _fclr << endl;
      break;
    case both:
      os << "    forw cdv: " << dec << _fv << endl;
      os << "    forw cumul cdv: " << dec << _fcv << endl;
      os << "    forw clr: " << dec << _fclr << endl;

      os << "    back cdv: " << dec << _fv << endl;
      os << "    back cumul cdv: " << dec << _fcv << endl;
      os << "    back clr: " << dec << _fclr << endl;
      break;
   }
   os << endl;
   return 0;
}

ostream & operator << (ostream & os, UNI40_xqos_param & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}






