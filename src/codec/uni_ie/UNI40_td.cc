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
static char const _UNI40_td_cc_rcsid_[] =
"$Id: UNI40_td.cc,v 1.20 1998/08/06 23:18:07 marsh Exp $";
#endif
#include <common/cprototypes.h>
///
#include <codec/uni_ie/UNI40_td.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>

/*
 * ATM traffic descriptor: 5.4.5.6 
 */

UNI40_traffic_desc::UNI40_traffic_desc(void) : InfoElem(ie_traffic_desc_id),
   _ftpc(TPC_UNSPECIFIED), _btpc(TPC_UNSPECIFIED)
{ 
  zerof();
  zerob();
}


UNI40_traffic_desc::UNI40_traffic_desc(const UNI40_traffic_desc & him) : 
  InfoElem(him), _ftpc(him._ftpc), _btpc(him._btpc), 
  _ftagging(him._ftagging), _btagging(him._btagging), 
  _FPCR_0_value(him._FPCR_0_value), _BPCR_0_value(him._BPCR_0_value), 
  _FPCR_01_value(him._FPCR_01_value), _BPCR_01_value(him._BPCR_01_value), 
  _FSCR_0_value(him._FSCR_0_value), _BSCR_0_value(him._BSCR_0_value), 
  _FSCR_01_value(him._FSCR_01_value), _BSCR_01_value(him._BSCR_01_value), 
  _FMBS_0_value(him._FMBS_0_value), _BMBS_0_value(him._BMBS_0_value), 
  _FMBS_01_value(him._FMBS_01_value), _BMBS_01_value(him._BMBS_01_value), 
  _BE_value(him._BE_value), _TMO_value(him._TMO_value), 
  _FMCR_01_value(him._FMCR_01_value), _BMCR_01_value(him._BMCR_01_value)
{
}


UNI40_traffic_desc::~UNI40_traffic_desc() {}

UNI40_traffic_desc * UNI40_traffic_desc::copy(void) 
{return new UNI40_traffic_desc(*this);}

InfoElem * UNI40_traffic_desc::copy(void) const
{ return (InfoElem *)(new UNI40_traffic_desc(*this)); }

u_char UNI40_traffic_desc::IsVariableLen(void) const
{ return 1; }

// 0 is for HIGH and 1 for LOW priority (CLP = 0 or CLP = 0+1)
// peak cell rate
int UNI40_traffic_desc::get_FPCR_0(void) const { return _FPCR_0_value;}
int UNI40_traffic_desc::get_BPCR_0(void) const { return _BPCR_0_value;}

int UNI40_traffic_desc::get_FPCR_01(void) const { return _FPCR_01_value;}
int UNI40_traffic_desc::get_BPCR_01(void) const { return _BPCR_01_value;}

// sustained cell rate
int UNI40_traffic_desc::get_FSCR_0(void) const { return _FSCR_0_value;}
int UNI40_traffic_desc::get_BSCR_0(void) const { return _BSCR_0_value;}

int UNI40_traffic_desc::get_FSCR_01(void) const { return _FSCR_01_value;}
int UNI40_traffic_desc::get_BSCR_01(void) const { return _BSCR_01_value;}

// Maximum Burst Size    
int UNI40_traffic_desc::get_FMBS_0(void) const { return _FMBS_0_value;}
int UNI40_traffic_desc::get_BMBS_0(void) const { return _BMBS_0_value;}

int UNI40_traffic_desc::get_FMBS_01(void) const { return _FMBS_01_value;}
int UNI40_traffic_desc::get_BMBS_01(void) const { return _BMBS_01_value;}

// ABR Min Cell Rate
int UNI40_traffic_desc::get_FMCR_01(void) const { return _FMCR_01_value; }
int UNI40_traffic_desc::get_BMCR_01(void) const { return _BMCR_01_value; }

char UNI40_traffic_desc::complete(void)
{
  if (_ftpc && _btpc) return 1;
  else return 0;
}

enum UNI40_traffic_desc::VALID_TPC  UNI40_traffic_desc::ftpc(void) const 
   { return _ftpc; }
enum UNI40_traffic_desc::VALID_TPC  UNI40_traffic_desc::btpc(void) const 
   { return _btpc; }

bool UNI40_traffic_desc::IsSet(enum PNNI_crankback::Dir direction, 
			       enum UNI40_traffic_desc::TPC_FLAGS flags) const
{
  bool answer = false;

  if (direction == PNNI_crankback::forward) {
    // Assume that the answer is true until the flags and the _ftpc
    // contradict each other.
    answer = true;

    if ((flags & PCR_0) && 
	((_ftpc != TPC1) &&
	 (_ftpc != TPC2))) answer = false;

    if ((flags & PCR_01) && 
	((_ftpc != TPC1) &&
	 (_ftpc != TPC2) &&
	 (_ftpc != TPC3) &&
	 (_ftpc != TPC4) &&
	 (_ftpc != TPC5) &&
	 (_ftpc != TPC6) &&
	 (_ftpc != TPC7) &&
	 (_ftpc != TPC8))) answer = false;

    if ((flags & SCR_0) && 
	((_ftpc != TPC3) &&
	 (_ftpc != TPC4))) answer = false;

    if ((flags & SCR_01) && 
	((_ftpc != TPC6))) answer = false;

    if ((flags & MBS_0) && 
	((_ftpc != TPC3) &&
	 (_ftpc != TPC4))) answer = false;

    if ((flags & MBS_01) && 
	((_ftpc != TPC6))) answer = false;

    if ((flags & BE) && 
	((_ftpc != TPC7) &&
	 (_ftpc != TPC8))) answer = false;

    if ((flags & TAGGING) && 
	((_ftpc != TPC2) &&
	 (_ftpc != TPC4))) answer = false;

    if ((flags & MCR_01) && 
	((_ftpc != TPC8))) answer = false;

  } else {
    // Assume that the answer is true until the flags and the _btpc
    // contradict each other.
    answer = true;

    if ((flags & PCR_0) && 
	((_btpc != TPC1) &&
	 (_btpc != TPC2))) answer = false;

    if ((flags & PCR_01) && 
	((_btpc != TPC1) &&
	 (_btpc != TPC2) &&
	 (_btpc != TPC3) &&
	 (_btpc != TPC4) &&
	 (_btpc != TPC5) &&
	 (_btpc != TPC6) &&
	 (_btpc != TPC7) &&
	 (_btpc != TPC8))) answer = false;

    if ((flags & SCR_0) && 
	((_btpc != TPC3) &&
	 (_btpc != TPC4))) answer = false;

    if ((flags & SCR_01) && 
	((_btpc != TPC6))) answer = false;

    if ((flags & MBS_0) && 
	((_btpc != TPC3) &&
	 (_btpc != TPC4))) answer = false;

    if ((flags & MBS_01) && 
	((_btpc != TPC6))) answer = false;

    if ((flags & BE) && 
	((_btpc != TPC7) &&
	 (_btpc != TPC8))) answer = false;

    if ((flags & TAGGING) && 
	((_btpc != TPC2) &&
	 (_btpc != TPC4))) answer = false;

    if ((flags & MCR_01) && 
	((_btpc != TPC8))) answer = false;

  }
  return answer;
}


void UNI40_traffic_desc:: set_TP1(dir d, int pcr_0, int pcr_01)
{
  switch (d) {
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


void UNI40_traffic_desc:: set_TP2(dir d, int pcr_0, int pcr_01)
{
  switch (d) {
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

void UNI40_traffic_desc:: set_TP3(dir d, int pcr_01,int scr_0,  int mbs_0)
{
  switch (d) {
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

void UNI40_traffic_desc:: set_TP4(dir d, int pcr_01,int scr_0, int mbs_0)
{
  switch (d) {
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

void UNI40_traffic_desc:: set_TP5(dir d, int pcr_01)
{
  switch (d) {
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

void UNI40_traffic_desc:: set_TP6(dir d, int pcr_01, int scr_01, int mbs_01)
{
  switch (d) {
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
void UNI40_traffic_desc:: set_BE(int fpcr_01, int bpcr_01)
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

// BE with MCR
void UNI40_traffic_desc:: set_BE(int fpcr_01, int bpcr_01, 
				 int fmcr_01, int bmcr_01)
{      
  zerof();
  zerob();
  _ftagging = 0;
  _btagging = 0;
  _ftpc = TPC8;
  _btpc = TPC8;
  _FPCR_01_value = fpcr_01;
  _BPCR_01_value = bpcr_01;
  _FMCR_01_value = fmcr_01;
  _BMCR_01_value = bmcr_01; 
  MakeStale();
}


bool UNI40_traffic_desc::get_BE(void) const
{
  return ( ((_ftpc == TPC7) && (_btpc == TPC7)) || 
	   ((_ftpc == TPC8) && (_btpc == TPC8)) )  ;
}


// forward and backward traffic combinations have to match
int UNI40_traffic_desc::equals(const UNI40_traffic_desc *himptr) const
{
  if ((_ftpc != himptr->_ftpc) || (_btpc != himptr->_btpc))
    return 0;

  // (F/B)PCR_01 is common to all tpc's
  if ((_FPCR_01_value != himptr->_FPCR_01_value) 
      || (_BPCR_01_value != himptr->_BPCR_01_value))
    return(0);

  // forward combination parameters have to match
  switch (_ftpc) {
    case TPC1:
    case TPC2:
      if(_FPCR_0_value != himptr->_FPCR_0_value) 
	return(0);
      break;
    case TPC3:
    case TPC4:
      if ((_FSCR_0_value != himptr->_FSCR_0_value) 
          || (_FMBS_0_value != himptr->_FMBS_0_value))
	return(0);
      // if TPC4 need to check tagging
      if (_ftpc == TPC4) {
	if (_ftagging != himptr->_ftagging)
	  return(0);
      }
      break;
    case TPC6:
      if ((_FSCR_01_value != himptr->_FSCR_01_value) || 
          (_FMBS_01_value != himptr->_FMBS_01_value))
	return(0);
      break;
    case TPC7: // BE
      if (_FPCR_01_value != himptr->_FPCR_01_value) 
	return(0);
      break;
    case TPC8:
      if (_FPCR_01_value != himptr->_FPCR_01_value)
        return(0);
      if (_FMCR_01_value != himptr->_FMCR_01_value)
        return(0);
      break;
    }

  // backward combination parameters have to match
  switch (_btpc) {
    case TPC1:
    case TPC2:
      if (_BPCR_0_value != himptr->_BPCR_0_value) 
	return(0);
      break;
    case TPC3:
    case TPC4:
      if ((_BSCR_0_value != himptr->_BSCR_0_value) || 
          (_BMBS_0_value != himptr->_BMBS_0_value))
	return(0);
      // if TPC4 need to check tagging
      if (_btpc == TPC4)
	{
	  if (_btagging != himptr->_btagging)
	    return(0);
	}
      break;
    case TPC6:
      if ((_BSCR_01_value != himptr->_BSCR_01_value) || 
          (_BMBS_01_value != himptr->_BMBS_01_value))
	return(0);
      break;
    case TPC7: // BE
      if (_BPCR_01_value != himptr->_BPCR_01_value) 
	return(0);
      break;
    case TPC8:
      if (_BPCR_01_value != himptr->_BPCR_01_value)
        return(0);
      if (_BMCR_01_value != himptr->_BMCR_01_value)
        return(0);
      break;
    }
  return(1);
}


int UNI40_traffic_desc::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}

int UNI40_traffic_desc:: operator == (const UNI40_traffic_desc & tp) const
{
   return equals(&tp);
}

int UNI40_traffic_desc::Length()
{
  int buflen = ie_header_len;
  // FPCR_0 for TPC1 or TPC2
  if(_ftpc == TPC1 || _ftpc == TPC2)
    buflen += 4;
  // BPCR_0 for TPC1 or TPC2
  if (_btpc == TPC1 || _btpc == TPC2)
    buflen += 4;
  //  FPCR_01 common to all TPC's
  buflen += 4;
  // BPCR_01 common to all TPC's
  buflen += 4;
  // FSCR_0 for TPC3 or TPC4
  if (_ftpc == TPC3 || _ftpc == TPC4)
    buflen += 4;
  // BSCR_0 for TPC3 or TPC4
  if (_btpc == TPC3 || _btpc == TPC4)
    buflen += 4;
  // FSCR_01 for TPC6
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
  // BMBS_01
  if (_btpc == TPC6)
    buflen += 4;
  // best effort
  if ((_ftpc == TPC7) && (_btpc == TPC7))
    buflen++;
  // tagging information and TMO_id
  if (_ftagging == 1 || _btagging == 1)
    buflen += 2;
  // ABR MCR on TPC8
  if ((_ftpc == TPC8) && (_btpc == TPC8))
    buflen += 8;
  return (buflen);
}


int UNI40_traffic_desc::encode(u_char * buffer)
{
  u_char * cptr;
  int buflen = 0;

  if ((cptr = LookupCache()) != 0L) {
    bcopy((char *)cptr, (char *)buffer, (buflen = CacheLength()));
    return(buflen);
  }
  cptr = buffer + ie_header_len;
  put_id(buffer,_id);	
  

  // put_coding(buffer,0x60);    // REFUSED BY SWITCH
  put_coding(buffer, _coding);   // HACKEROO Bilal -- Fri Jul 31 00:22:17 EDT 1998


  // encode FPCR_0 for TPC1 or TPC2
  if (_ftpc == TPC1 || _ftpc == TPC2) {
    put_8(cptr,buflen,FPCR_0_id);
    put_24(cptr,buflen,_FPCR_0_value);
  }
  // encode BPCR_0 for TPC1 or TPC2
  if (_btpc == TPC1 || _btpc == TPC2) {
    put_8(cptr,buflen,BPCR_0_id);
    put_24(cptr,buflen,_BPCR_0_value);
  }
  // encode FPCR_01 common to all TPC's
  put_8(cptr,buflen,FPCR_01_id);
  put_24(cptr,buflen,_FPCR_01_value);
  // encode BPCR_01 common to all TPC's
  put_8(cptr,buflen,BPCR_01_id);
  put_24(cptr,buflen,_BPCR_01_value);
  // encode FSCR_0 for TPC3 or TPC4
  if (_ftpc == TPC3 || _ftpc == TPC4) {
    put_8(cptr,buflen,FSCR_0_id);
    put_24(cptr,buflen,_FSCR_0_value);
  }
  // encode BSCR_0 for TPC3 or TPC4
  if (_btpc == TPC3 || _btpc == TPC4) {
    put_8(cptr,buflen,BSCR_0_id);
    put_24(cptr,buflen,_BSCR_0_value);
  }
  // encode FSCR_01 for TPC6
  if (_ftpc == TPC6) {
    put_8(cptr,buflen,FSCR_01_id);
    put_24(cptr,buflen,_FSCR_01_value);
  }
  // encode BSCR_01 for TPC6
  if (_btpc == TPC6) {
    put_8(cptr,buflen,BSCR_01_id);
    put_24(cptr,buflen,_BSCR_01_value);
  }
  // encode FMBS_0
  if ((_ftpc == TPC3) || (_ftpc == TPC4)) {
    put_8(cptr,buflen,FMBS_0_id);
    put_24(cptr,buflen,_FMBS_0_value);
  }
  // encode BMBS_0
  if ((_btpc == TPC3) || (_btpc == TPC4)) {
    put_8(cptr,buflen,BMBS_0_id);
    put_24(cptr,buflen,_BMBS_0_value);
  }
  // encode FMBS_01
  if (_ftpc == TPC6) {
    put_8(cptr,buflen,FMBS_01_id);
    put_24(cptr,buflen,_FMBS_01_value);
  }
  // encode BMBS_01
  if (_btpc == TPC6) {
    put_8(cptr,buflen,BMBS_01_id);
    put_24(cptr,buflen,_BMBS_01_value);   
  }
  // encode best effort
  if ((_ftpc == TPC7) && (_btpc == TPC7)) {
    put_8(cptr,buflen,BE_id);
  }
  // encode tagging information and TMO_id
  if (_ftagging == 1 || _btagging == 1) {
    put_8(cptr,buflen,TMO_id);
    *cptr = 0;
    if (_ftagging == 1)
      *cptr  = 0x01;
    if (_btagging == 1)
      *cptr |= 0x02;
    cptr++;
    buflen++;
  }
  // encode ABR MCR on TPC8
  if ((_ftpc == TPC8) && (_btpc == TPC8)) {
    put_8(cptr,buflen,FMCR_01_id);
    put_24(cptr,buflen,_FMCR_01_value);
    put_8(cptr,buflen,BMCR_01_id);
    put_24(cptr,buflen,_BMCR_01_value);
  }
  put_len(buffer, buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

InfoElem::ie_status UNI40_traffic_desc::decode(u_char * buffer, int & id)
{
  int tp_id, tp_value;
  u_long ftpc, btpc;
  register int i;
  short len = get_len(buffer);

  id = buffer[0];
  // buffer[1]: bit8 must be 1 and coding must be 11
  if (((buffer[1] & 0xE0) >> 5) != 0x07)
    return InfoElem::invalid_contents;
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  // FPCR 0+1 and BPCR 0+1 are mandatory for all TPC's
  if (len < 8)
    return (InfoElem::incomplete);
  buffer += ie_header_len;

  /* at max we have 16 id's see TP_ID enumeration.
   * as we pick up the parameters we need to register them in order
   * to check whether they form a valid combination.
   */
  ftpc=0;
  btpc=0;
  for (i = 0; i < 16; i++) {
    if (!len)
      break;
    tp_id = *buffer++;
    switch (tp_id) {
      case  BE_id:	  
	if (len < 1)
	  return (InfoElem::invalid_contents);
	_ftagging = 0;
	_btagging = 0;
	ftpc |= BE;
	btpc |= BE;
	len--;
	break;

      case  TMO_id: 
	if (len < 2)
	  return (InfoElem::invalid_contents);
	tp_value = *buffer++;
	if ((tp_value & 0x02) == 0x02) {
	  btpc |= TAGGING;
	  _btagging = 1;
	}
	if ((tp_value & 0x01) == 0x01) {
	  ftpc |= TAGGING;
	  _ftagging = 1;
	}
	len -= 2;
	break;
	
      case  FPCR_0_id:
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
      case  FMCR_01_id:
      case  BMCR_01_id:
	if (len < 4)
	  return (InfoElem::invalid_contents);
	//tp_value = ntohl((buffer[0] << 16) | (buffer[1] << 8) | buffer[2]);
	tp_value = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
	switch (tp_id) {
	case  FPCR_0_id:
	  _FPCR_0_value = tp_value;
	  ftpc |=PCR_0;
	  break;
	case  FPCR_01_id:
	  _FPCR_01_value =tp_value; 
	  ftpc |=PCR_01;
	  break;
	case  FSCR_0_id:
	  _FSCR_0_value = tp_value;
	  ftpc |= SCR_0;
	  break;
	case  FSCR_01_id:
	  _FSCR_01_value = tp_value;
	  ftpc |=SCR_01;
	  break;
	case  FMBS_0_id:
	  _FMBS_0_value  =tp_value;
	  ftpc |=MBS_0;
	  break;
	case  FMBS_01_id:
	  _FMBS_01_value =tp_value;
	  ftpc |=MBS_01;
	  break;
	case  BPCR_0_id:
	  _BPCR_0_value  =tp_value;
	  btpc |=PCR_0;
	  break;
	case  BPCR_01_id:
	  _BPCR_01_value =tp_value;
	  btpc |=PCR_01;
	  break;
	case  BSCR_0_id:
	  _BSCR_0_value  =tp_value;
	  btpc |=SCR_0;
	  break;
	case  BSCR_01_id:
	  _BSCR_01_value =tp_value;
	  btpc |=SCR_01;
	  break;
	case  BMBS_0_id:
	  _BMBS_0_value  =tp_value;
	  btpc |=MBS_0;
	  break;
	case  BMBS_01_id:
	  _BMBS_01_value =tp_value;
	  btpc |=MBS_01;
	  break;
	case  FMCR_01_id:
	  _FMCR_01_value =tp_value;
	  ftpc |=MCR_01;
	case  BMCR_01_id:
	  _BMCR_01_value = tp_value;
	  btpc |=MCR_01;
	}
	buffer += 3;
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
    case TPC8:
      _ftpc = (UNI40_traffic_desc::VALID_TPC)ftpc;
      break;
    default:
      return (InfoElem::invalid_contents);
      break;
  }
  switch(btpc) {
    case TPC1:
    case TPC2:
    case TPC3:
    case TPC4:
    case TPC5:
    case TPC6:
    case TPC7:
    case TPC8:
      _btpc = (UNI40_traffic_desc::VALID_TPC)btpc;
      break;
    default:
      return (InfoElem::invalid_contents);
      break;
  }
  return (InfoElem::ok);
}

char UNI40_traffic_desc::PrintSpecific(ostream & os)
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
  case TPC8: os << "TPC 8 " << endl; break;
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
  case TPC8: os << "TPC 8 " << endl; break;
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
  os << "   forward ABR minimum cell rate::" << _FMCR_01_value << endl;
  os << "   backward ABR minimum cell rate::" << _BMCR_01_value << endl;
  os << "   Best Effort::" <<  _BE_value << endl;
  os << "   Traffic Management Options::" << _TMO_value << endl;

  return 0;
}

void UNI40_traffic_desc::zerof(void)
{
  _ftagging=0;
  _FPCR_0_value=0;
  _FPCR_01_value=0;
  _FSCR_0_value=0;
  _FSCR_01_value=0;
  _FMBS_0_value=0;
  _FMBS_01_value=0;
  _BE_value=0;
  _TMO_value=0;
  _FMCR_01_value = 0;
}


void UNI40_traffic_desc::zerob(void)
{
  _btagging=0;
  _BPCR_0_value=0;
  _BPCR_01_value=0;
  _BSCR_0_value=0;
  _BSCR_01_value=0;
  _BMBS_0_value=0;
  _BMBS_01_value=0;
  _BE_value=0;
  _TMO_value=0;
  _BMCR_01_value = 0;
}


ostream & operator << (ostream & os, UNI40_traffic_desc & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}


// Alternative ATM TD IE
UNI40_alt_traffic_desc::UNI40_alt_traffic_desc(void) : UNI40_traffic_desc()
{ _id = UNI40_alt_traff_desc_id; }

UNI40_alt_traffic_desc::UNI40_alt_traffic_desc(const UNI40_alt_traffic_desc & him) : 
  UNI40_traffic_desc(him)
{
}


UNI40_alt_traffic_desc::~UNI40_alt_traffic_desc(void) {}

int  
UNI40_alt_traffic_desc::operator == (const UNI40_alt_traffic_desc & rhs) const
{
  return equals(&rhs);
}

int  UNI40_alt_traffic_desc::equals(const InfoElem * rhs) const
{
  return rhs->equals(this);
}



UNI40_alt_traffic_desc * UNI40_alt_traffic_desc::copy(void)
{return new UNI40_alt_traffic_desc(*this);}

InfoElem * UNI40_alt_traffic_desc::copy(void) const
{ return (InfoElem *)(new UNI40_alt_traffic_desc(*this)); }


ostream & operator << (ostream & os, UNI40_alt_traffic_desc & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}



// Minimum Traffic Descriptor

UNI40_min_traffic_desc::~UNI40_min_traffic_desc(void) {};

UNI40_min_traffic_desc::UNI40_min_traffic_desc(void) : 
  InfoElem(UNI40_min_traff_desc_id), 
  _bits(NONE), _FPCR_0(0), _BPCR_0(0), 
  _FPCR_01(0), _BPCR_01(0), 
  _FAMCR_01(0), _BAMCR_01(0) 
{
}

UNI40_min_traffic_desc::UNI40_min_traffic_desc(const UNI40_min_traffic_desc & mtd) :
   InfoElem(mtd), 
   _bits(mtd._bits), _FPCR_0(mtd._FPCR_0), _BPCR_0(mtd._BPCR_0),
   _FPCR_01(mtd._FPCR_01), _BPCR_01(mtd._BPCR_01),
   _FAMCR_01(mtd._FAMCR_01), _BAMCR_01(mtd._BAMCR_01){ }

UNI40_min_traffic_desc & 
UNI40_min_traffic_desc::operator = (const UNI40_min_traffic_desc & rhs)
{
  _bits    = rhs._bits;
  _FPCR_0  = rhs._FPCR_0;
  _BPCR_0  = rhs._BPCR_0;
  _FPCR_01 = rhs._FPCR_01;
  _BPCR_01 = rhs._BPCR_01;
  _FAMCR_01= rhs._FAMCR_01;
  _BAMCR_01= rhs._BAMCR_01;
  return *this;
}


int 
UNI40_min_traffic_desc::operator == (const UNI40_min_traffic_desc & rhs) const
{
  return equals(&rhs);
}


int  UNI40_min_traffic_desc::equals(const UNI40_min_traffic_desc * rhs) const
{
  return ((_bits == rhs->_bits) &&
          (_FPCR_0 == rhs->_FPCR_0) &&
	  (_BPCR_0 == rhs->_BPCR_0) &&
	  (_FPCR_01 == rhs->_FPCR_01) &&
	  (_BPCR_01 == rhs->_BPCR_01) &&
	  (_FAMCR_01 == rhs->_FAMCR_01) &&
	  (_BAMCR_01 == rhs->_BAMCR_01));
}


int UNI40_min_traffic_desc::equals(const InfoElem * rhs) const
{
  return rhs->equals(this);
}


UNI40_min_traffic_desc * UNI40_min_traffic_desc::copy(void) 
{ return new UNI40_min_traffic_desc(*this); }

  ///
InfoElem * UNI40_min_traffic_desc::copy(void) const 
{ 
  UNI40_min_traffic_desc *mtdp = new UNI40_min_traffic_desc;
  (*mtdp) = (*this); 
  return (InfoElem *)mtdp; 
}


int UNI40_min_traffic_desc::encode(u_char * buffer)
{
  u_char * temp;
  int buflen = 0;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    temp = buffer + buflen;
  } else {
    temp = buffer + ie_header_len;

    if (_bits & FPCR_0) {
      *temp++ = 0x82;
      *temp++ = ((_FPCR_0 >> 16) & 0xFF);
      *temp++ = ((_FPCR_0 >>  8) & 0xFF);
      *temp++ = (_FPCR_0 & 0xFF);
      buflen += 4;
    }
    if (_bits & BPCR_0) {
      *temp++ = 0x83;
      *temp++ = ((_BPCR_0 >> 16) & 0xFF);
      *temp++ = ((_BPCR_0 >>  8) & 0xFF);
      *temp++ = (_BPCR_0 & 0xFF);
      buflen += 4;
    }
    if (_bits & FPCR_01) {
      *temp++ = 0x84;
      *temp++ = ((_FPCR_01 >> 16) & 0xFF);
      *temp++ = ((_FPCR_01 >>  8) & 0xFF);
      *temp++ = (_FPCR_01 & 0xFF);
      buflen += 4;
    }
    if (_bits & BPCR_01) {
      *temp++ = 0x85;
      *temp++ = ((_BPCR_01 >> 16) & 0xFF);
      *temp++ = ((_BPCR_01 >>  8) & 0xFF);
      *temp++ = (_BPCR_01 & 0xFF);
      buflen += 4;
    }
    if (_bits & FAMCR_01) {
      *temp++ = 0x92;
      *temp++ = ((_FAMCR_01 >> 16) & 0xFF);
      *temp++ = ((_FAMCR_01 >>  8) & 0xFF);
      *temp++ = (_FAMCR_01 & 0xFF);
      buflen += 4;
    }
    if (_bits & BAMCR_01) {
      *temp++ = 0x93;
      *temp++ = ((_BAMCR_01 >> 16) & 0xFF);
      *temp++ = ((_BAMCR_01 >>  8) & 0xFF);
      *temp++ = (_BAMCR_01 & 0xFF);
      buflen += 4;
    }
    put_id(buffer, _id);
    put_coding(buffer, _coding);
    put_len(buffer, buflen);
    // encode_ie_header(buffer, _id, buflen);
    buflen += ie_header_len;
    FillCache(buffer, buflen);
  }
  return buflen;
}


InfoElem::ie_status UNI40_min_traffic_desc::decode(u_char * buffer, int & id)
{
  //       id = ntohs(buffer[0]); // sjm
       id = buffer[0];
  short len = get_len(buffer);

  if (id != _id)
    return InfoElem::bad_id;
  if (!len)
    return InfoElem::empty;

  buffer += 4;
  _bits = 0;
  
  while (len-- > 0) {
    switch (*buffer++) {
      case 0x82:   // FPCR_0
        _bits   |= FPCR_0;
	_FPCR_0  = (*buffer++ << 16) & 0x00FF0000;
        _FPCR_0 |= (*buffer++ <<  8) & 0x0000FF00;
	_FPCR_0 |= (*buffer++ & 0xFF);
        len -= 3;
        break;
      case 0x83:
	_bits   |= BPCR_0;
	_BPCR_0  = (*buffer++ << 16) & 0x00FF0000;
	_BPCR_0 |= (*buffer++ <<  8) & 0x0000FF00;
	_BPCR_0 |= (*buffer++ & 0xFF);
	len -= 3;
	break;
      case 0x84:
	_bits    |= FPCR_01;
	_FPCR_01  = (*buffer++ << 16) & 0x00FF0000;
	_FPCR_01 |= (*buffer++ <<  8) & 0x0000FF00;
	_FPCR_01 |= (*buffer++ & 0xFF);
	len -= 3;
	break;
      case 0x85:
	_bits    |= BPCR_01;
	_BPCR_01  = (*buffer++ << 16) & 0x00FF0000;
	_BPCR_01 |= (*buffer++ <<  8) & 0x0000FF00;
	_BPCR_01 |= (*buffer++ & 0xFF);
	len -= 3;
	break;
      case 0x92:
	_bits    |= FAMCR_01;
	_FAMCR_01  = (*buffer++ << 16) & 0x00FF0000;
	_FAMCR_01 |= (*buffer++ <<  8) & 0x0000FF00;
	_FAMCR_01 |= (*buffer++ & 0xFF);
	len -= 3;
	break;
      case 0x93:
	_bits    |= BAMCR_01;
	_BAMCR_01  = (*buffer++ << 16) & 0x00FF0000;
	_BAMCR_01 |= (*buffer++ <<  8) & 0x0000FF00;
	_BAMCR_01 |= (*buffer++ & 0xFF);
	len -= 3;
	break;
      default:
        return InfoElem::invalid_contents;
	break;
    }
  }
  return InfoElem::ok;
}


u_char UNI40_min_traffic_desc::IsVariableLen(void) const
{ return 1; }

void UNI40_min_traffic_desc::set_FPCR_0(int fpcr) 
{
  _FPCR_0  = fpcr; 
  setBit(FPCR_0); 
}

///
void UNI40_min_traffic_desc::set_BPCR_0(int bpcr)  
{ 
  _BPCR_0  = bpcr; 
  setBit(BPCR_0);
 }

///
void UNI40_min_traffic_desc::set_FPCR_01(int fpcr) 
{
  _FPCR_01 = fpcr;
  setBit(FPCR_01);
 }

///
void UNI40_min_traffic_desc::set_BPCR_01(int bpcr) 
{
  _BPCR_01 = bpcr;
  setBit(BPCR_01);
}


void UNI40_min_traffic_desc::set_ForwardABR(int abr)  
{
  _FAMCR_01 = abr; 
  setBit(FAMCR_01); 
}

///
void UNI40_min_traffic_desc::set_BackwardABR(int abr) 
{
  _BAMCR_01 = abr; 
  setBit(BAMCR_01); 
}


// 0 is for HIGH and 1 for LOW priority (CLP = 0 or CLP = 0+1)
// peak cell rate
int UNI40_min_traffic_desc::get_FPCR_0(void) const { return _FPCR_0;}

int UNI40_min_traffic_desc::get_BPCR_0(void) const { return _BPCR_0;}

int UNI40_min_traffic_desc::get_FPCR_01(void) const { return _FPCR_01;}

int UNI40_min_traffic_desc::get_BPCR_01(void) const { return _BPCR_01;}

int UNI40_min_traffic_desc::get_ForwardABR(void) const { return _FAMCR_01;}

int UNI40_min_traffic_desc::get_BackwardABR(void) const { return _BAMCR_01;}


///
void UNI40_min_traffic_desc::reset(void) 
{ _bits = 0; }

int UNI40_min_traffic_desc::Length()
{
  int buflen = ie_header_len;
  if (isSet(FPCR_0)) 
    buflen += 4;
  if (isSet(BPCR_0)) 
    buflen += 4;
  if (isSet(FPCR_01)) 
    buflen += 4;
  if (isSet(BPCR_01)) 
    buflen += 4;
  if (isSet(FAMCR_01)) 
    buflen += 4;
  if (isSet(BAMCR_01)) 
    buflen += 4;
  return buflen;
}


char UNI40_min_traffic_desc::PrintSpecific(ostream & os)
{
  if (_bits & FPCR_0)
    os << "   FPCR_0::" << ios::hex << _FPCR_0 << ios::dec << endl;
  if (_bits & BPCR_0)
    os << "   BPCR_0::" << ios::hex << _BPCR_0 << ios::dec << endl;
  if (_bits & FPCR_01)
    os << "   FPCR_01::" << ios::hex << _FPCR_01 << ios::dec << endl;
  if (_bits & BPCR_01)
    os << "   BPCR_01::" << ios::hex << _BPCR_01 << ios::dec << endl;
  if (_bits & FAMCR_01)
    os << "   Forward ABR Minimum Cell Rate::"
       << ios::hex << _FAMCR_01 << ios::dec << endl;
  if (_bits & BAMCR_01)
    os << "   Backward ABR Minimum Cell Rate::"
       << ios::hex << _BAMCR_01 << ios::dec << endl;
  return 0;
}


bool UNI40_min_traffic_desc::IsSet(enum UNI40_min_traffic_desc::td_bitv flags) const
{
  return isSet(flags);
}

void UNI40_min_traffic_desc::setBit(td_bitv bit) 
{ _bits |= bit; }

///
int  UNI40_min_traffic_desc::isSet(td_bitv bit) const
{ return _bits & bit; }


ostream & operator << (ostream & os, UNI40_min_traffic_desc & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}


