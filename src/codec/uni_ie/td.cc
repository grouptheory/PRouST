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
static char const _td_cc_rcsid_[] =
"$Id: td.cc,v 1.8 1998/08/06 23:23:19 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/td.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>

/*
 * ATM traffic descriptor: 5.4.5.6 
 */

ie_traffic_desc::ie_traffic_desc(void) : InfoElem(ie_traffic_desc_id),
   _ftpc(TPC_UNSPECIFIED), _btpc(TPC_UNSPECIFIED)
{ 
  zerof();
  zerob();
}


ie_traffic_desc::~ie_traffic_desc() {}


InfoElem* ie_traffic_desc::copy(void) const
{return new ie_traffic_desc(*this);}

// 0 is for HIGH and 1 for LOW priority (CLP = 0 or CLP = 0+1)
// peak cell rate
int ie_traffic_desc::get_FPCR_0(void) { return _FPCR_0_value;}
int ie_traffic_desc::get_BPCR_0(void) { return _BPCR_0_value;}

int ie_traffic_desc::get_FPCR_01(void) { return _FPCR_01_value;}
int ie_traffic_desc::get_BPCR_01(void) { return _BPCR_01_value;}

// sustained cell rate
int ie_traffic_desc::get_FSCR_0(void) { return _FSCR_0_value;}
int ie_traffic_desc::get_BSCR_0(void) { return _BSCR_0_value;}

int ie_traffic_desc::get_FSCR_01(void) { return _FSCR_01_value;}
int ie_traffic_desc::get_BSCR_01(void) { return _BSCR_01_value;}

// Maximum Burst Size    
int ie_traffic_desc::get_FMBS_0(void) { return _FMBS_0_value;}
int ie_traffic_desc::get_BMBS_0(void) { return _BMBS_0_value;}

int ie_traffic_desc::get_FMBS_01(void) { return _FMBS_01_value;}
int ie_traffic_desc::get_BMBS_01(void) { return _BMBS_01_value;}

u_char ie_traffic_desc::IsVariableLen(void) { return ((u_char)1); }

char ie_traffic_desc::complete(void){
  if (_ftpc && _btpc) return 1;
  else return 0;
}

enum ie_traffic_desc::VALID_TPC  ie_traffic_desc::ftpc(void) const 
   { return _ftpc; }
enum ie_traffic_desc::VALID_TPC  ie_traffic_desc::btpc(void) const 
   { return _btpc; }

void ie_traffic_desc:: set_TP1(dir d, int pcr_0, int pcr_01)
{
  switch(d)
    {
    case fw:
      zerof();
      _ftagging = 0;
      _ftpc = TPC1;
      _FPCR_0_value = pcr_0;
      _FPCR_01_value = pcr_01;
      break;
    case bw:
      zerob();
      _btpc = TPC1;
      _btagging = 0;
      _BPCR_0_value = pcr_0;
      _BPCR_01_value = pcr_01;
      break;
    default:
      break;
    }
  MakeStale();
}


void ie_traffic_desc:: set_TP2(dir d, int pcr_0, int pcr_01)
{
  switch(d)
    {
    case fw:
      zerof();
      _ftpc = TPC2;
      _ftagging = 1;
      _FPCR_0_value = pcr_0;
      _FPCR_01_value = pcr_01;
      break;
    case bw:
      zerob();
      _btpc = TPC2;
      _btagging = 1;
      _BPCR_0_value = pcr_0;
      _BPCR_01_value = pcr_01;
      break;
    default:
      break;
    }
  MakeStale();
}

void ie_traffic_desc:: set_TP3(dir d, int pcr_01,int scr_0,  int mbs_0)
{
  switch(d)
    {
    case fw:
      zerof();
      _ftpc = TPC3;
      _ftagging = 0;
      _FPCR_01_value = pcr_01;
      _FSCR_0_value = scr_0;
      _FMBS_0_value = mbs_0;
      break;
    case bw:
      zerob();
      _btpc = TPC3;
      _btagging = 0;
      _BPCR_01_value = pcr_01;
      _BSCR_0_value  = scr_0;     
      _BMBS_0_value = mbs_0;
      break;
    default:
      break;
    }
  MakeStale();
}

void ie_traffic_desc:: set_TP4(dir d, int pcr_01,int scr_0, int mbs_0)
{
  switch(d)
    {
    case fw:
      zerof();
      _ftpc = TPC4;
      _ftagging = 1;
      _FPCR_01_value = pcr_01;
      _FSCR_0_value = scr_0;
      _FMBS_0_value = mbs_0;
      break;
    case bw:
      zerob();
      _btpc = TPC4;
      _btagging = 1;
      _BPCR_01_value = pcr_01;
      _BSCR_0_value  = scr_0;     
      _BMBS_0_value = mbs_0;
      break;
    default:
      break;
    }
  MakeStale();
}

void ie_traffic_desc:: set_TP5(dir d, int pcr_01)
{
  switch(d)
    {
    case fw:
      zerof();
      _ftpc = TPC5;
      _ftagging = 0;
      _FPCR_01_value = pcr_01;
      break;
    case bw:
      zerob();
      _btpc = TPC5;
      _btagging = 0;
      _BPCR_01_value = pcr_01;
      break;
    default:
      break;
    }
  MakeStale();
}

void ie_traffic_desc:: set_TP6(dir d, int pcr_01, int scr_01, int mbs_01)
{
  switch(d)
    {
    case fw:
      zerof();
      _ftpc = TPC6;
      _ftagging = 0;
      _FPCR_01_value = pcr_01;
      _FSCR_01_value = scr_01;
      _FMBS_01_value = mbs_01;
      break;
    case bw:
      zerob();
      _btpc = TPC6;
      _btagging = 0;
      _BPCR_01_value = pcr_01;
      _BSCR_01_value  = scr_01;     
      _BMBS_01_value = mbs_01;
      break;
    default:
      break;
    }
  MakeStale();
}

// best effort always applies to both directions 

void ie_traffic_desc:: set_BE(int fpcr_01, int bpcr_01)
{      
  zerof();
  zerob();
  _ftagging = 0;
  _btagging = 0;
  _ftpc = TPC7;
  _btpc = TPC7;
  _FPCR_01_value = fpcr_01;
  _BPCR_01_value = bpcr_01;
  MakeStale();
}

int ie_traffic_desc::equals(const ie_traffic_desc *himptr) const
{
  if((_ftpc != himptr->_ftpc) || (_btpc != himptr->_btpc))
    return 0;
  // (F/B)PCR_01 is common to all tpc's
  if((_FPCR_01_value != himptr->_FPCR_01_value) || (_BPCR_01_value != himptr->_BPCR_01_value))
    return(0);

  // forward combination parameters have to match
  switch(_ftpc)
    {
    case TPC1:
    case TPC2:
      if(_FPCR_0_value != himptr->_FPCR_0_value) 
	return(0);
      break;

    case TPC3:
    case TPC4:
      if((_FSCR_0_value != himptr->_FSCR_0_value) || (_FMBS_0_value != himptr->_FMBS_0_value))
	return(0);
      // if TPC4 need to check tagging
      if(_ftpc == TPC4)
	{
	  if(_ftagging != himptr->_ftagging)
	    return(0);
	}
      break;
    case TPC6:
      if((_FSCR_01_value != himptr->_FSCR_01_value) || (_FMBS_01_value != himptr->_FMBS_01_value))
	return(0);
      break;
    case TPC7: // BE
      if(_FPCR_01_value != himptr->_FPCR_01_value) 
	return(0);
      break;
    }

  // backward combination parameters have to match
  switch(_btpc)
    {
    case TPC1:
    case TPC2:
      if(_BPCR_0_value != himptr->_BPCR_0_value) 
	return(0);
      break;

    case TPC3:
    case TPC4:
      if((_BSCR_0_value != himptr->_BSCR_0_value) || (_BMBS_0_value != himptr->_BMBS_0_value))
	return(0);
      // if TPC4 need to check tagging
      if(_btpc == TPC4)
	{
	  if(_btagging != himptr->_btagging)
	    return(0);
	}
      break;
    case TPC6:
      if((_BSCR_01_value != himptr->_BSCR_01_value) || (_BMBS_01_value != himptr->_BMBS_01_value))
	return(0);
      break;
    case TPC7: // BE
      if(_BPCR_01_value != himptr->_BPCR_01_value) 
	return(0);
      break;
    }
  return(1);
}

int ie_traffic_desc::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}

// forward and backward traffic combinations have to match
int ie_traffic_desc:: operator == (const ie_traffic_desc & tp) const
{
  // common parameters
  if((_ftpc != tp._ftpc) || (_btpc != tp._btpc))
    return 0;
  // (F/B)PCR_01 is common to all tpc's
  if((_FPCR_01_value != tp._FPCR_01_value) || (_BPCR_01_value != tp._BPCR_01_value))
    return(0);

  // forward combination parameters have to match
  switch(_ftpc)
    {
    case TPC1:
    case TPC2:
      if(_FPCR_0_value != tp._FPCR_0_value) 
	return(0);
      break;

    case TPC3:
    case TPC4:
      if((_FSCR_0_value != tp._FSCR_0_value) || (_FMBS_0_value != tp._FMBS_0_value))
	return(0);
      // if TPC4 need to check tagging
      if(_ftpc == TPC4)
	{
	  if(_ftagging != tp._ftagging)
	    return(0);
	}
      break;
    case TPC6:
      if((_FSCR_01_value != tp._FSCR_01_value) || (_FMBS_01_value != tp._FMBS_01_value))
	return(0);
      break;
    case TPC7: // BE
      if(_FPCR_01_value != tp._FPCR_01_value) 
	return(0);
      break;
    }

  // backward combination parameters have to match
  switch(_btpc)
    {
    case TPC1:
    case TPC2:
      if(_BPCR_0_value != tp._BPCR_0_value) 
	return(0);
      break;

    case TPC3:
    case TPC4:
      if((_BSCR_0_value != tp._BSCR_0_value) || (_BMBS_0_value != tp._BMBS_0_value))
	return(0);
      // if TPC4 need to check tagging
      if(_btpc == TPC4)
	{
	  if(_btagging != tp._btagging)
	    return(0);
	}
      break;
    case TPC6:
      if((_BSCR_01_value != tp._BSCR_01_value) || (_BMBS_01_value != tp._BMBS_01_value))
	return(0);
      break;
    case TPC7: // BE
      if(_BPCR_01_value != tp._BPCR_01_value) 
	return(0);
      break;
    }
  return(1);
}


int ie_traffic_desc::Length()
{
  
  if(LookupCache())
    return (CacheLength());

  int buflen = ie_header_len;
  if(_ftpc == TPC1 || _ftpc == TPC2)
    buflen += 4;
  if(_btpc == TPC1 || _btpc == TPC2)
    buflen += 4;
  // FPCR_01 and BPCR_01 common to all TPC's
  buflen += 8;
  // FSCR_0 for TPC3 or TPC4
  if(_ftpc == TPC3 || _ftpc == TPC4)
    buflen += 4;
  //  BSCR_0 for TPC3 or TPC4
  if(_btpc == TPC3 || _btpc == TPC4)
    buflen += 4;
  //  FSCR_01 for TPC6
  if (_ftpc == TPC6)
    buflen += 4;
  // BSCR_01 for TPC6
  if (_btpc == TPC6)
    buflen += 4;
  // FMBS_0
  if ((_ftpc == TPC3) || (_ftpc == TPC4))
    buflen += 4;
  // BMBS_0
  if ((_btpc == TPC3) || (_btpc == TPC4))
    buflen += 4;
  // FMBS_01
  if (_ftpc == TPC6)
    buflen += 4;
  // encode BMBS_01
  if (_btpc == TPC6)
    buflen += 4;
  //  best effort
  if ((_ftpc == TPC7) && (_btpc == TPC7))
    buflen++;
  // tagging information and TMO_id
  if (_ftagging == 1 || _btagging == 1)
    buflen += 3;
  return (buflen);
}


int ie_traffic_desc::encode(u_char * buffer)
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
  put_coding(buffer,_coding);
  // encode FPCR_0 for TPC1 or TPC2
  if(_ftpc == TPC1 || _ftpc == TPC2)
    {
      put_8(cptr,buflen,FPCR_0_id);
      put_24(cptr,buflen,_FPCR_0_value);
    }
  // encode BPCR_0 for TPC1 or TPC2
  if(_btpc == TPC1 || _btpc == TPC2)
    {
      put_8(cptr,buflen,BPCR_0_id);
      put_24(cptr,buflen,_BPCR_0_value);
    }
  // encode FPCR_01 and BPCR_01 common to all TPC's
  put_8(cptr,buflen,FPCR_01_id);
  put_24(cptr,buflen,_FPCR_01_value);
  put_8(cptr,buflen,BPCR_01_id);
  put_24(cptr,buflen,_BPCR_01_value);

  // encode FSCR_0 for TPC3 or TPC4
  if(_ftpc == TPC3 || _ftpc == TPC4)
    {
      put_8(cptr,buflen,FSCR_0_id);
      put_24(cptr,buflen,_FSCR_0_value);
    }
  // encode BSCR_0 for TPC3 or TPC4
  if(_btpc == TPC3 || _btpc == TPC4)
    {
      put_8(cptr,buflen,BSCR_0_id);
      put_24(cptr,buflen,_BSCR_0_value);
    }
  // encode FSCR_01 for TPC6
  if (_ftpc == TPC6)
    {
      put_8(cptr,buflen,FSCR_01_id);
      put_24(cptr,buflen,_FSCR_01_value);
    }
  // encode BSCR_01 for TPC6
  if (_btpc == TPC6)
    {
      put_8(cptr,buflen,BSCR_01_id);
      put_24(cptr,buflen,_BSCR_01_value);
    }
  // encode FMBS_0
  if ((_ftpc == TPC3) || (_ftpc == TPC4))
    {
      put_8(cptr,buflen,FMBS_0_id);
      put_24(cptr,buflen,_FMBS_0_value);
    }
  // encode BMBS_0
  if ((_btpc == TPC3) || (_btpc == TPC4))
    {
      put_8(cptr,buflen,BMBS_0_id);
      put_24(cptr,buflen,_BMBS_0_value);
    }
  // encode FMBS_01
  if (_ftpc == TPC6)
    {
      put_8(cptr,buflen,FMBS_01_id);
      put_24(cptr,buflen,_FMBS_01_value);
    }
  // encode BMBS_01
  if (_btpc == TPC6)
    {
      put_8(cptr,buflen,BMBS_01_id);
      put_24(cptr,buflen,_BMBS_01_value);
    }
  // encode best effort
  if ((_ftpc == TPC7) && (_btpc == TPC7))
    put_8(cptr,buflen,BE_id);
  // encode tagging information and TMO_id
  if (_ftagging == 1 || _btagging == 1) {
    put_8(cptr,buflen,TMO_id);
    *cptr = 0;
    if(_ftagging == 1)
      *cptr = 0x01;
    if(_btagging == 1)
      *cptr |= 0x02;
    cptr++;
    buflen += 2;
  }
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

InfoElem::ie_status ie_traffic_desc :: decode(u_char * buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  int tp_id;
  int tp_value;
  u_long ftpc;
  u_long btpc;
  register int i;

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  // FPCR 0+1 and BPCR 0+1 are mandatory for all TPC's
  if (len < 8)
    return (InfoElem::incomplete);
  // check coding against 00/11
  buffer += ie_header_len;
  /* at max we have 14 id's see TP_ID enumeration.
   * as we pick up the parameters we need to register them in order
   * to check whether they form a valid combination.
   */
  ftpc=0;
  btpc=0;
  for (i = 0; i < 14; i++) {
    u_long x = 0;
    if (!len)
      break;
    get_8(buffer,tp_id);
    switch (tp_id)
      {
      case  BE_id: // 1 byte long
	if (len < 1)
	  return (InfoElem::invalid_contents);
	_ftagging = 0;
	_btagging = 0;
	ftpc |= BE;
	btpc |= BE;
	len--;
	break;


      case  TMO_id: // 2 bytes long
	if (len < 2)
	  return (InfoElem::invalid_contents);
	get_8(buffer,tp_value);
	if ((tp_value & 0x02) == 0x02)
	  {
	    btpc |= TAGGING;
	    _btagging = 1;
	  }
	if ((tp_value & 0x01) == 0x01)
	  {
	    ftpc |= TAGGING;
	    _ftagging = 1;
	  }
	len -= 2;
	break;
	
      case  FPCR_0_id:// 4 bytes long
      case  BPCR_0_id:
      case  FPCR_01_id:
      case  BPCR_01_id:
      case  FSCR_0_id:
      case  BSCR_0_id:
      case  FSCR_01_id:
      case  BSCR_01_id:
      case  FMBS_0_id:
      case  BMBS_0_id:
      case  FMBS_01_id:
      case  BMBS_01_id:
	if (len < 4)
	  return (InfoElem::invalid_contents);
	// x = ntohl((buffer[0] << 16) | (buffer[1] << 8) | buffer[2]); // sjm 
	x = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
	switch(tp_id)
	  {
	  case  FPCR_0_id:
	    _FPCR_0_value = x;
	    ftpc |=PCR_0;
	    break;
	  case  FPCR_01_id:
	    _FPCR_01_value = x;
	    ftpc |=PCR_01;
	    break;
	  case  FSCR_0_id:
	    _FSCR_0_value = x;
	    ftpc |=SCR_0;
	    break;
	  case  FSCR_01_id:
	    _FSCR_01_value = x;
	    ftpc |=SCR_01;
	    break;
	  case  FMBS_0_id:
	    _FMBS_0_value  = x;
	    ftpc |=MBS_0;
	    break;
	  case  FMBS_01_id:
	    _FMBS_01_value = x;
	    ftpc |=MBS_01;
	    break;
	  case  BPCR_0_id:
	    _BPCR_0_value  = x;
	    btpc |=PCR_0;
	    break;
	  case  BPCR_01_id:
	    _BPCR_01_value = x;
	    btpc |=PCR_01;
	    break;
	  case  BSCR_0_id:
	    _BSCR_0_value  = x;
	    btpc |=SCR_0;
	    break;
	  case  BSCR_01_id:
	    _BSCR_01_value = x;
	    btpc |=SCR_01;
	    break;
	  case  BMBS_0_id:
	    _BMBS_0_value  = x;
	    btpc |=MBS_0;
	    break;
	  case  BMBS_01_id:
	    _BMBS_01_value = x;
	    btpc |=MBS_01;
	    break;
	  }
	buffer+= 3;
	len -= 4;
	break;
      default:
	return (InfoElem::invalid_contents);
      }
  }
  // if we still have data we have an error
  if (len > 0)
    return (InfoElem::invalid_contents);
  // validate the tpc's
  // check for forward combinations
  switch(ftpc) {
    case TPC1:
    case TPC2:
    case TPC3:
    case TPC4:
    case TPC5:
    case TPC6:
    case TPC7:
      _ftpc = (ie_traffic_desc::VALID_TPC)ftpc;
      break;
    default:
      return (InfoElem::invalid_contents);
  }

  switch(btpc) {
    case TPC1:
    case TPC2:
    case TPC3:
    case TPC4:
    case TPC5:
    case TPC6:
    case TPC7:
      _btpc = (ie_traffic_desc::VALID_TPC)btpc;
      break;
    default:
      return (InfoElem::invalid_contents);
  }
  return (InfoElem::ok);
}

char ie_traffic_desc::PrintSpecific(ostream & os)
{
  os << endl << "   ftpc::";
  switch (_ftpc) {
  case TPC_UNSPECIFIED: os << "TPC UNSPECIFIED " << endl; break;
  case TPC1: os << "TPC 1 " << endl; break;
  case TPC2: os << "TPC 2 " << endl; break;
  case TPC3: os << "TPC 3 " << endl; break;
  case TPC4: os << "TPC 4 " << endl; break;
  case TPC5: os << "TPC 5 " << endl; break;
  case TPC6: os << "TPC 6 " << endl; break;
  case TPC7: os << "TPC 7 " << endl; break;
  default:   os << "unknown " << endl; break;
  }
  os << "   btpc::";
  switch (_btpc) {
  case TPC_UNSPECIFIED: os << "TPC UNSPECIFIED " << endl; break;
  case TPC1: os << "TPC 1 " << endl; break;
  case TPC2: os << "TPC 2 " << endl; break;
  case TPC3: os << "TPC 3 " << endl; break;
  case TPC4: os << "TPC 4 " << endl; break;
  case TPC5: os << "TPC 5 " << endl; break;
  case TPC6: os << "TPC 6 " << endl; break;
  case TPC7: os << "TPC 7 " << endl; break;
  default:   os << "unknown " << endl; break;
  }
  os << "   forward tagging::"  << _ftagging << endl;
  os << "   backward tagging::" << _btagging << endl;
  os << "   forward peak cell rate::"   << _FPCR_0_value  << endl;
  os << "   forward peak cell rate::"   << _FPCR_01_value << endl;
  os << "   backward peak cell rate::"  << _BPCR_0_value  << endl;
  os << "   backward peak cell rate::"  << _BPCR_01_value << endl;
  os << "   forward sust. cell rate::"  << _FSCR_0_value  << endl;
  os << "   forward sust. cell rate::"  << _FSCR_01_value << endl;
  os << "   backward sust. cell rate::" << _BSCR_0_value  << endl;
  os << "   backward sust. cell rate::" << _BSCR_01_value << endl;
  os << "   forward max burst size::"   << _FMBS_0_value  << endl;
  os << "   forward max burst size::"   << _FMBS_01_value << endl;
  os << "   backward max burst size::"  << _BMBS_0_value  << endl;
  os << "   backward max burst size::"  << _BMBS_01_value << endl;
  os << "   Best Effort::" <<  _BE_value << endl;
  os << "   Traffic Management Options::" << _TMO_value << endl;

  return 0;
}

ostream & operator << (ostream & os, ie_traffic_desc & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}

