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
static char const _addr_cc_rcsid_[] =
"$Id: addr.cc,v 1.28 1998/10/22 15:08:13 mountcas Exp $";
#endif
#include <common/cprototypes.h>
#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/errmsg.h>
#include <iostream.h>
#include <sys/types.h>

Addr::Addr(Addressing_type addr_type) : 
  Cache(), _addr_type(addr_type), _valid(1)  { }

Addr::~Addr() { }

int compare(Addr * const & x, Addr * const & y)
{
  if (x->equals(y)) 
    return 0;
  // either their equal or the first one is bigger?!?!?!  Is this right?
  return (1);
}

int Addr::operator == (const Addr & addr2) const 
{ 
  return (equals(&addr2)); 
}

int Addr::Length(void)
{
  if (_addr_type == International)
    return(E164_MAX_SIZE);
  return(20);
}  

enum Addr::Addressing_type Addr::get_addressing_type(void) const
{
    return _addr_type;
}

int Addr::equals(const Addr * him) const
{ 
  if (!him)
    return 0;
  return ((_addr_type == him->_addr_type) ? 1 : 0);
}

int Addr::equals(const Addr & him) const
{ 
  return equals(&him);
}

char Addr::IsValid(void) const { return (_valid); }

void Addr::TogValid(void) { _valid = !_valid; }


char Addr::PrintSpecific(ostream & os)
{
  os << "! Addressing_type::";
  switch (_addr_type) {
  default:
  case Unknown: os << "Unknown " << endl;   break;
  case International: os << "International " << endl;   break;
  }
  return 0;
}

u_char Addr::IsVariableLen(void) const
{
  return ((u_char)0);
}

NSAP_addr::NSAP_addr() : Addr((Addressing_type)Unknown), _afi(DCC_ATM), _sel(0) 
{ 
  bcopy("000000",_esi,6);
}

NSAP_addr::NSAP_addr(afi_type afi,u_char *esi, int sel) : 
           Addr((Addressing_type)Unknown), _afi(afi), _sel(sel) 
{
  bcopy(esi,_esi,6);
}

NSAP_addr::NSAP_addr(afi_type afi): Addr((Addressing_type) Unknown), 
                                    _afi(afi), _sel(0)
{ 
  bcopy("000000",_esi,6);
}

NSAP_addr::~NSAP_addr() {}

NSAP_addr::afi_type NSAP_addr::get_afi_type(void) const 
{ return _afi; }

bool NSAP_addr::IsAnycast()
{
  if((_afi == DCC_ATM_ANYCAST) || (_afi == ICD_ATM_ANYCAST) || (_afi == E164_ATM_ANYCAST))
    return true;
  return 0;
}

void NSAP_addr::set_afi_type(NSAP_addr::afi_type type) { _afi = type; }

int operator == (const NSAP_addr & addr1, const NSAP_addr & addr2) 
{ 
  return addr1.equals(&addr2); 
}

int NSAP_addr::equals(const NSAP_addr * him) const
{ 
  if (him && Addr::equals(him) && 
      (_afi == him->_afi) &&
      (_sel == him->_sel) &&
      !memcmp(_esi, him->_esi, 6)) 
    return 1;
  return 0;
}

int NSAP_addr::equals(const NSAP_addr & him) const
{ 
  return equals(&him);
}

char NSAP_addr::PrintSpecific(ostream & os)
{
  // Something wierd is happening in here.
  // Insight says FUNC_BAD iostream.h : 101
  // ostream& operator<<(__manip func) {(*func)(*this); return *this;}
  // Function pointer is not a function: (void *) func
  // line 101.
  Addr::PrintSpecific(os);  

  os << "   afi_type::";
  switch (_afi) {
  case DCC_ATM:  os << "DCC_ATM " << endl;   break;
  case ICD_ATM:  os << "ICD_ATM " << endl;   break;
  case E164_ATM: os << "E164_ATM "<< endl;   break;
  default:       os << "Unknown " << endl;   break;
  }

  os << "   esi::0x";
  if (_esi != 0L)
    for (int i = 0; i < 6; i++) {
      u_int val = (u_int) _esi[i];
      if (val < 16) os << "0";
      os << hex << val << dec;
    }
  else os << "00";
  os << " sel::";
  if (_sel < 16) os << "0";
  os << hex << _sel << dec << endl;

  return 0;
}

E164_addr::E164_addr(u_char *E164_addr,int E164_addr_size) :
           Addr((Addressing_type)Unknown),_E164_addr_size(E164_addr_size) 
{
  bcopy(E164_addr,_E164_addr,6);
}

E164_addr::E164_addr(const E164_addr & addr) : Addr(addr.get_addressing_type())
{
  _E164_addr_size = addr._E164_addr_size;
  bcopy((u_char *)addr._E164_addr, _E164_addr, 6);
}

E164_addr::E164_addr(E164_addr *addr) : Addr(addr->get_addressing_type())
{
  _E164_addr_size = addr->_E164_addr_size;
  bcopy(addr->_E164_addr, _E164_addr, 6);
}

E164_addr::~E164_addr() {}

int E164_addr::equals(const InfoElem * him) const 
{
  if (!him)
    return 0;
  return him->equals(this);
}

int E164_addr::equals(const E164_addr * himptr) const 
{
  if (himptr && Addr::equals(himptr) && 
      (_E164_addr_size == himptr->_E164_addr_size) &&
      (!memcmp(_E164_addr, himptr->_E164_addr, _E164_addr_size))) 
    return 1;
  return 0;
}

int operator == (const E164_addr& addr1, const E164_addr& addr2)
{ 
  return addr1.equals(&addr2); 
}

int E164_addr::equals(const E164_addr & him) const
{ 
  return equals(&him);
}

Addr * E164_addr::copy(void) const
{
  if (this != 0L) return new E164_addr(*this);
  else return new E164_addr;
}

char E164_addr::PrintSpecific(ostream & os) 
{
  Addr::PrintSpecific(os);  

  if (_E164_addr_size) {
    os << "   E164_addr::";
    for (int i = 0; i < _E164_addr_size; i++)
      os << hex << _E164_addr[i] << dec;
    os << endl << endl;
  }
  return 0;
}

ostream & operator << (ostream & os, E164_addr & x)
{
  // x.PrintSpecific(os);
  for (int i = 0; i < x._E164_addr_size; i++)
    os << hex << (int)(x._E164_addr[i]) << dec;
  os << dec;
  return (os);
}


NSAP_DCC_ICD_addr::NSAP_DCC_ICD_addr(afi_type afi, u_char *dcc_icd, 
				     u_char *ho_dsp,
				     u_char *esi, int sel) : 
				     NSAP_addr(afi,esi,sel) 
{
  bcopy(dcc_icd, _dcc_icd, 4);
  bcopy(ho_dsp, _ho_dsp, 10);
}


NSAP_DCC_ICD_addr::NSAP_DCC_ICD_addr(NSAP_DCC_ICD_addr * addr) : NSAP_addr()
{
  _afi = addr->_afi;
  _sel = addr->_sel;
  bcopy(addr->_esi, _esi, 6);
  bcopy(addr->_dcc_icd, _dcc_icd, 4);
  bcopy(addr->_ho_dsp, _ho_dsp, 10);
}

NSAP_DCC_ICD_addr::NSAP_DCC_ICD_addr(const NSAP_DCC_ICD_addr & addr) : NSAP_addr()
{
  _afi = addr._afi;
  _sel = addr._sel;
  bcopy((u_char *)addr._esi, _esi, 6);
  bcopy((u_char *)addr._dcc_icd, _dcc_icd, 4);
  bcopy((u_char *)addr._ho_dsp, _ho_dsp, 10);
}

NSAP_DCC_ICD_addr::~NSAP_DCC_ICD_addr() { }

NSAP_DCC_ICD_addr::NSAP_DCC_ICD_addr(afi_type afi) : NSAP_addr(afi)  { }


NSAP_DCC_ICD_addr::NSAP_DCC_ICD_addr(char *string) : NSAP_addr()
{
  int len = 20, i;
  // string should be atleast 20 bytes
  char *    temp = string;

  // Skip the 0x if it's there
  if (*temp == '0' && *(temp+1) == 'x')
    temp += 2;
  
  if (*temp == '3' && *(temp+1) == '9')
    _afi = NSAP_addr::DCC_ATM;
  else if (*temp == '4' && *(temp+1) == '7')
    _afi = NSAP_addr::ICD_ATM;
  else
    Addr::TogValid();   // address is now invalid

  // Skip past the addr type
  temp += 2;
  len  -= 2;

  // Skip any dots
  if (*temp == '.')
    temp++;

  // Get dcc/icd
  for (i = 0; i < 4; i++, len--)
    _dcc_icd[i] = *temp++;

  if (*temp == '.')
    temp++;

  // Get _ho_dsp[10]
  for (i = 0; i < 10; i++) {

    if (*temp == '.')
      temp++;
      
    if (*temp <= 0x39)
      _ho_dsp[i] = (((*(temp++) - '0') << 4) & 0xF0);
    else // Must be hex letter
      _ho_dsp[i] = (((*(temp++) - 'a' + 0x0a) << 4) & 0xF0);
    // next part of ho_dsp[i]
    if (*temp <= 0x39)
      _ho_dsp[i] |= ((*(temp++) - '0') & 0x0F);
    else // Must be hex letter
      _ho_dsp[i] |= ((*(temp++) - 'a' + 0x0a) & 0x0F);
  }
  if (*temp == '.')
    temp++;
  // Get ESI
  for (i = 0; i < 6; i++) {
    if (*temp == '.')
      temp++;

    if (*temp <= 0x39)
      _esi[i] = (((*(temp++) - '0') << 4) & 0xF0);
    else // Must be hex letter
      _esi[i] = (((*(temp++) - 'a' + 0x0a) << 4) & 0xF0);
    // low nibble of esi[i]
    if (*temp <= 0x39)
      _esi[i] |= ((*(temp++) - '0') & 0x0F);
    else // Must be hex letter
      _esi[i] |= ((*(temp++) - 'a' + 0x0a) & 0x0F);
  }
  if (*temp == '.')
    temp++;

  // Get SEL
  if (*temp <= 0x39)
    _sel = (((*(temp++) - '0') << 4) & 0xF0);
  else // Must be hex letter
    _sel = (((*(temp++) - 'a' + 0x0a) << 4) & 0xF0);
  // low nibble of sel
  if (*temp <= 0x39)
    _sel |= ((*(temp++) - '0') & 0x0F);
  else // Must be hex letter
    _sel |= ((*(temp++) - 'a' + 0x0a) & 0x0F);
}



Addr *   NSAP_DCC_ICD_addr::copy(void) const
{
  if (this != 0L) return new NSAP_DCC_ICD_addr(*this);
  else return new NSAP_DCC_ICD_addr;
}


NSAP_addr * NSAP_DCC_ICD_addr::nsap_addr_copy(void) const
{
  return new NSAP_DCC_ICD_addr(*this);
}


int NSAP_DCC_ICD_addr::equals(const NSAP_DCC_ICD_addr & him) const
{ 
  return equals(&him);
}

int NSAP_DCC_ICD_addr::equals(const InfoElem * him) const
{
  return equals(him);
}

int NSAP_DCC_ICD_addr::equals(const NSAP_DCC_ICD_addr * him) const
{
  if (him && NSAP_addr::equals(him) && 
      (!memcmp(_dcc_icd, him->_dcc_icd, 4)) &&
      (!memcmp(_ho_dsp, him->_ho_dsp, 10))) 
    return 1;
  return 0;
}

int NSAP_DCC_ICD_addr::operator == (const NSAP_DCC_ICD_addr & addr2) const
{ 
  return (equals(&addr2)); 
}



/*
 *  NSAP addresses:
 *  0) [IDP][DSP]: IDP= Initial Domain Part DSP=Domain Specific Part
 *  1) [ [AFI] [IDI] ] [ [HO-DSP][ID][SEL] ]
 *      AFI=Authority and Format Identifier
 *      IDI=Initial Domain Identifier
 *      HO-DSP= High Order DSP
 *      ID = System Identifier
 *      SEL= NSAP Selector
 *
 *   {IDP,HO-DSP} is the area address 
 *
 *    GOSIP version 2 structure of the NSAP (rfc1237)
 *   
 *    {AFI} {IDI} {DFI} {AA}     {RSVD} {RD}   {Area} {ID} {SEL} fields
 *    {1}   {2}   {1}   {3}      {2}    {2}    {2}    {6}  {1}   sizes in bytes
 *    {47}  {0005}{80h} {??????} {0000} {0000} {0000}            values for US
 *               
 *    DFI=DSP Format Identifier
 *    AA=Administrative Authority
 *    RD= Routing Domain Identifier
 *  
 *    {AFI,IDI,DFI,AA} represent an adminstartive  prefix 
 *    administrative prefix + {RSVD,RD} represent a routing domain prefix
 *    routing domain prefix + {Area} represent an area address
 *
 */
int NSAP_DCC_ICD_addr::encode(u_char * buffer)
{
  u_char * cptr;
  int buflen = 0;
  register int i;

  if ((cptr = LookupCache()) != 0L) {
    bcopy((char *)cptr, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  cptr = buffer;
  // encode the AFI: 1 byte long force it to be 0x47 for now
  *cptr++ = (u_char) _afi;

  /* encode the DCC_ICD 4 bytes long: codes are in ISO 3166 and the
   * digits are in BCD
   * we assume the A3 ISO 3166 that is _dcc_icd[3] is zero.
   * so the ISO 3166 code for USA is 840 stored as 08-04-00-00 in
   * dccicd and will be encoded as 84-0F in 2 bytes.
   * it is left-justified and padded with 0xf see 5.1.3.1.1.1 page 161 UNI-3.1
   */
  for(i = 0; i < 4; i++) {
    u_char temp = _dcc_icd[i];
    if (!temp)
      temp = 0x0f;
    else
      temp = temp - '0';
    if ((i == 0) || (i == 2))
      *cptr = (u_char)(temp << 4);
    else
      *cptr++ |= (u_char)(temp);
  }
  // encode the ho-dsp as 0's for now, need to read rfc1237 or ISO 8348
  for (i = 0; i < 10 ; i++)
    *cptr++ = (u_char )_ho_dsp[i];
  // encode the ESI
  for(i = 0; i < 6 ;i++)
    *cptr++ = (u_char)_esi[i];
  *cptr++ = (u_char)_sel;
  buflen = 20;
  FillCache(buffer, buflen);
  return (buflen);
}

InfoElem::ie_status NSAP_DCC_ICD_addr::decode(u_char * buffer, int & id)
{
  u_char * ptr = buffer;
  register int i;
  id = -1;

  // get the afi
  _afi = (NSAP_addr::afi_type)*ptr++;

  if ((_afi != DCC_ATM) && (_afi != ICD_ATM))
    _afi = DCC_ATM;

  // get DCC/ICD
  for (i = 0; i < 4; i++) {
    u_char temp;
    if((i == 0) || (i == 2))
      temp = (u_char)((*ptr & 0xf0)>> 4);
    else
      temp = (u_char)(*ptr++ & 0x0f);
    if (temp == 0x0f)
      _dcc_icd[i] = 0;
    else
      _dcc_icd[i] = temp + '0';
  }
  // get the ho-dsp we don't interpret it for now
  for (i = 0; i < 10 ; i++)
    _ho_dsp[i] = *ptr++;
  //get the ESI, we don't interpret it for now
  for (i = 0; i < 6 ; i++)
    _esi[i] = *ptr++;
  //get the SEL
  _sel = *ptr;
  return (InfoElem::ok);
}


char NSAP_DCC_ICD_addr::PrintSpecific(ostream & os)
{
  int i;
  NSAP_addr::PrintSpecific(os);

  os << "   dcc_icd::0x";
  for (i = 0; i < 4; i++) {
    os << _dcc_icd[i];
  }  
  os << endl;

  os << "   ho_dsp::0x";
  for (i = 0; i < 10; i++) {
    u_int val = 0xFF & (u_int) _ho_dsp[i];
    if (val < 16) os << "0";
    os << hex << val << dec;
  }
  os << endl;
  return 0;
}

ostream & operator << (ostream & os, NSAP_DCC_ICD_addr & x)
{
  int i;
  // No print general since these are not derived from InfoElem
  // x.PrintSpecific(os);
  os << hex << (int)(x._afi) << ":"; // reset to dec at end
  for (i = 0; i < 6; i++) {
    if (x._esi[i] < 16) os << "0";
    os << (int)(x._esi[i]);
  }
  os << ":";
  for (i = 0; i < 4; i++) {
    if (x._dcc_icd[i] < 16) os << "0";
    os << (int)(x._dcc_icd[i]);
  }
  os << ":";
  for (i = 0; i < 10; i++) {
    if (x._ho_dsp[i] < 16) os << "0";
    os << (int)(x._ho_dsp[i]);
  }
  os << dec;
  return (os);
}


int NSAP_E164_addr::equals(const InfoElem * him) const 
{
  return him->equals(this);
}


int NSAP_E164_addr::equals(const NSAP_E164_addr * him) const 
{
  if (him && NSAP_addr::equals(him) &&
      (_E164_addr_size == him->_E164_addr_size) &&
      (!memcmp(_E164_addr, him->_E164_addr, _E164_addr_size)))
    return 1;
  return 0;
}


int operator == (const NSAP_E164_addr & addr1, const NSAP_E164_addr & addr2)
{
  return addr1.equals(&addr2); 
}


NSAP_E164_addr::NSAP_E164_addr(afi_type afi, u_char *ho_dsp, u_char *idi, 
			       int idilen,
			       u_char *esi, int sel): NSAP_addr(afi,esi,sel)
{
  // What about buffer, buflen and dcc_icd?
}

NSAP_E164_addr::NSAP_E164_addr(afi_type afi) : 
         NSAP_addr(afi), _E164_addr_size(0)
{
  bcopy("0000", _ho_dsp, 4);
  bcopy("00000000", _E164_addr, 8);
}

NSAP_E164_addr::NSAP_E164_addr(NSAP_E164_addr * addr) : 
         NSAP_addr(addr->get_afi_type())
{
  _E164_addr_size = addr->_E164_addr_size;
  bcopy(addr->_ho_dsp, _ho_dsp, 4);
  bcopy(addr->_E164_addr, _E164_addr, 8);
}

NSAP_E164_addr::NSAP_E164_addr(const NSAP_E164_addr & addr) : 
  NSAP_addr(addr.get_afi_type())
{
  _E164_addr_size = addr._E164_addr_size;
  bcopy((u_char *)addr._ho_dsp, _ho_dsp, 4);
  bcopy((u_char *)addr._E164_addr, _E164_addr, 8);
}

NSAP_E164_addr::~NSAP_E164_addr() {}

Addr * NSAP_E164_addr::copy(void) const
{
  if (this != 0L) return new NSAP_E164_addr(*this);
  else return new NSAP_E164_addr;
}

NSAP_addr * NSAP_E164_addr::nsap_addr_copy(void) const
{
  return new NSAP_E164_addr(*this);
}

int NSAP_E164_addr::equals(const NSAP_E164_addr & him) const
{ 
  return equals(&him);
}


/*
 * NSAP_E164_addr encoding and decoding methods
 * see 5.1.3.1.1.3 for left and right paddings
 */

int NSAP_E164_addr::encode(u_char * buffer)
{
  return (0);
}

InfoElem::ie_status NSAP_E164_addr::decode(u_char * buffer, int & id)
{
  id = -1;
  return (InfoElem::ok);
}


char NSAP_E164_addr::PrintSpecific(ostream & os)
{
  int i;
  NSAP_addr::PrintSpecific(os);
  os << "   E164_addr::0x" << hex;
  if (_E164_addr != 0L)
    for (i = 0; i < _E164_addr_size; i++) {
      u_int val = (u_int) _E164_addr[i];
      if (val < 16) os << "0";
      os << hex << val;
    }
  else os << "00";
  os << endl << "   ho_dsp::0x";
  if (_ho_dsp != 0L)
    for (i = 0; i < 4; i++) {
      u_int val = 0xFF & (u_int) _ho_dsp[i];
      if (val < 16) os << "0";
      os << hex << val;
    }
  else os << "00";
  os << dec << endl;
  
  return 0;
}


ostream & operator << (ostream & os, NSAP_E164_addr & x)
{
  // Not derived from InfoElem (i.e. no PrintGeneral(os)
  x.PrintSpecific(os);
  return (os);
}

/*
 * E.164 address
 */

E164_addr::E164_addr() : Addr(International), _E164_addr_size(0)
{
  for (int i = 0; i < E164_MAX_SIZE; i ++)
    _E164_addr[i] = 0;
}

void E164_addr::set_E164_addr(char *buffer, int buflen)
{
  char * temp = buffer;

  _E164_addr_size = 0;
  for (int i = 0; i < buflen ; i++) {
    if (_E164_addr_size == E164_MAX_SIZE)
      break;
    if (*temp == '\0')
      break;
    if((*temp == '-') || (*temp == ' '))
      continue;
    // accept only digits ?
    _E164_addr[_E164_addr_size++] = *temp;
    temp++;
  }
  MakeStale();
}

int E164_addr::encode(u_char * buffer)
{
  u_char * temp;
  int buflen;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    temp = buffer + buflen;
  } else {
    temp = buffer;

    for (int i = 0; i < _E164_addr_size ; i++)
      *temp++ = (0x7f & _E164_addr[i]);

    buflen = _E164_addr_size;
    FillCache(buffer, buflen);
  }
  return (buflen);
}

InfoElem::ie_status E164_addr::decode(u_char * buffer, int & id)
{
  u_char *temp = buffer;
  id = -1;
  _E164_addr_size = E164_MAX_SIZE;
  for(int i = 0; i < _E164_addr_size; i++)
    _E164_addr[i] = *temp++;
  return (InfoElem::ok);
}

// takes a string specifying an address of type DCC, ICD or E.164
//  and returns a ptr of Addr to it.
//
// Strings must be in the following format:
//   0x47.0005.80.ffe100.0000.f205.0992.00204810276a.00 -- herman ATM NSAP
//   0x47.0005.80.ffe100.0000.f215.0d2b.002048102788.00 -- bashful ATM NSAP
//   0x47.0005.80.ffe100.0000.f215.0d2b.002048010417.00 -- dos ATM NSAP
// The dots are not necessary, but ALL hexadecimal numbers must be in
//  lower case, and the 0x prefix is not necessary either.
//
// Strings may also be in the form produced by encode().
//
Addr * newAddr(char *string)
{
  u_char *               temp = (u_char *)string;
  NSAP_addr::afi_type  type;
  int                  n, i;
  Addr *answer = 0;
  int ignored = 20;
  InfoElem::ie_status err;

  // Skip the 0x if it's there
  if (*temp == '0' && *(temp+1) == 'x')
    temp += 2;
  
  //
  // Original flavor -- all printable, ascii characters
  //
  if (*temp == '3' && *(temp+1) == '9')
    type = NSAP_addr::DCC_ATM;
  else if (*temp == '4' && *(temp+1) == '7')
    type = NSAP_addr::ICD_ATM;
  else if (*temp == '4' && *(temp+1) == '5')
    type = NSAP_addr::E164_ATM;
  else if ((*temp == 'B' && *(temp+1) == 'D') ||
	   (*temp == 'b' && *(temp+1) == 'D') ||
	   (*temp == 'B' && *(temp+1) == 'd') ||
	   (*temp == 'b' && *(temp+1) == 'd'))
    type = NSAP_addr::DCC_ATM_ANYCAST;
  else if ((*temp == 'C' && *(temp+1) == '5') ||
	   (*temp == 'c' && *(temp+1) == '5'))
    type = NSAP_addr::ICD_ATM_ANYCAST;
  else if ((*temp == 'C' && *(temp+1) == '3') ||
	   (*temp == 'c' && *(temp+1) == '3'))
    type = NSAP_addr::E164_ATM_ANYCAST;

  //
  // New flavor, as produced by encode()/
  //
  else if ((*temp == 0x39) ||	// DCC_ATM
	   (*temp == 0x47) ||	// ICD_ATM
	   (*temp == 0xbd) ||	// DCC_ATM_ANYCAST
	   (*temp == 0xc5)	// ICD_ATM_ANYCAST
	   ) {
    answer = new NSAP_DCC_ICD_addr();
    err = answer->decode(temp, ignored);
    if (err == InfoElem::ok)
      return answer;
    else { 
      delete answer;
      return 0;
    }
  //		
  } else if ((*temp == 0x45) ||	// E164_ATM
	     (*temp == 0xc3)	// E164_ATM_ANYCAST
	     ) {
    answer = new NSAP_E164_addr();
    err = answer->decode(temp, ignored);
    if (err == InfoElem::ok)
      return answer;
    else { 
      delete answer;
      return 0;
    }
  } else return 0L;

  // Skip past the addr type
  temp += 2;
  // Skip any dots
  if (*temp == '.')
    temp++;

  // Get dcc/icd or e.164 (dcc_icd holds both)
  u_char dcc_icd[8];

  if (type == NSAP_addr::DCC_ATM || type == NSAP_addr::ICD_ATM)
    n = 4;
  else
    n = 8;

  for (i = 0; i < n; i++)
    dcc_icd[i] = *temp++;

  if (*temp == '.')
    temp++;

  // Get HO-DSP (10 or 4)
  u_char ho_dsp[10];

  if (type == NSAP_addr::DCC_ATM || type == NSAP_addr::ICD_ATM)
    n = 10;
  else 
    n = 4;
  
  for (i = 0; i < n; i++) {
    if (*temp == '.')
      temp++;
      
    if (*temp <= 0x39)
      ho_dsp[i] = (((*(temp++) - '0') << 4) & 0xF0);
    else // Must be hex letter
      ho_dsp[i] = (((*(temp++) - 'a' + 0x0a) << 4) & 0xF0);
    // next part of ho_dsp[i]
    if (*temp <= 0x39)
      ho_dsp[i] |= ((*(temp++) - '0') & 0x0F);
    else // Must be hex letter
      ho_dsp[i] |= ((*(temp++) - 'a' + 0x0a) & 0x0F);
  }
  if (*temp == '.')
    temp++;
  // Get ESI
  u_char esi[6];

  for (i = 0; i < 6; i++) {
    if (*temp == '.')
      temp++;

    if (*temp <= 0x39)
      esi[i] = (((*(temp++) - '0') << 4) & 0xF0);
    else // Must be hex letter
      esi[i] = (((*(temp++) - 'a' + 0x0a) << 4) & 0xF0);
    // low nibble of esi[i]
    if (*temp <= 0x39)
      esi[i] |= ((*(temp++) - '0') & 0x0F);
    else // Must be hex letter
      esi[i] |= ((*(temp++) - 'a' + 0x0a) & 0x0F);
  }
  if (*temp == '.')
    temp++;

  // Get SEL
  int sel;  
  if (*temp <= 0x39)
    sel = (((*(temp++) - '0') << 4) & 0xF0);
  else // Must be hex letter
    sel = (((*(temp++) - 'a' + 0x0a) << 4) & 0xF0);
  // low nibble of sel
  if (*temp <= 0x39)
    sel |= ((*(temp++) - '0') & 0x0F);
  else // Must be hex letter
    sel |= ((*(temp++) - 'a' + 0x0a) & 0x0F);

  if (type == NSAP_addr::DCC_ATM || type == NSAP_addr::ICD_ATM)
    return (new NSAP_DCC_ICD_addr(type, (u_char*)dcc_icd, (u_char*)ho_dsp, (u_char*)esi, sel));
  else // E.164 
    return (new NSAP_E164_addr(type, (u_char*)dcc_icd, (u_char*)ho_dsp, 4, (u_char*)esi, sel));
}


