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
static char const _bhli_cc_rcsid_[] =
"$Id: bhli.cc,v 1.10 1998/08/06 04:03:11 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/bhli.h>
#include <codec/uni_ie/errmsg.h>

//================  Broadband Higher Layer Info ===========================

//---------------- ie_bhli ------------------------------------------------
ie_bhli::ie_bhli(bhli_type t) :InfoElem(ie_bhli_id),_bhli_type(t){}


ie_bhli::~ie_bhli(){}


enum ie_bhli::bhli_type ie_bhli::get_bhli_type(void) const 
{ return _bhli_type; }

char ie_bhli::PrintSpecific(ostream & os)
{
  os << endl << "   bhli_type::";
  switch (_bhli_type) {
  case iso:                            
    os << "iso";   break;
  case user_specific:
    os << "user_specific";   break;
  case vendor_specific_application_id: 
    os << "vendor_specific_application_id";   break;
  default: 
    os << "unknown";   break;
  }
  os << endl;
  return 0;
}

//---------------- iso_bhli -----------------------------------------------

iso_bhli::iso_bhli(u_char *buffer, int buflen) : ie_bhli(iso) {
  _iso_bhli_len = (buflen > 8 ? 8  : buflen) ;
  int i;
  if (buffer==NULL) return;
  for (i = 0; i < _iso_bhli_len; i++)
    _iso_bhli[i] = *buffer++;
  if(_iso_bhli_len < 8)
    while(i < 8)
      _iso_bhli[i++] = 0;
}

iso_bhli::iso_bhli(const iso_bhli & rhs) : ie_bhli(iso) 
{
  for (int i = 0; i < 8; i++)
    _iso_bhli[i] = rhs._iso_bhli[i];
  _iso_bhli_len = rhs._iso_bhli_len;
}

iso_bhli::iso_bhli(void) : ie_bhli(iso) {
  u_char *buffer=0L;
  int buflen=8;

  _iso_bhli_len = (buflen > 8 ? 8  : buflen) ;
  int i;
  if (buffer==NULL) return;
  for (i = 0; i < _iso_bhli_len; i++)
    _iso_bhli[i] = *buffer++;
  if(_iso_bhli_len < 8)
    while(i < 8)
      _iso_bhli[i++] = 0;
}

iso_bhli::~iso_bhli() {}

void iso_bhli::set_bhli(u_char * buffer, int buflen){
  _iso_bhli_len = (buflen > 8 ? 8  : buflen) ;
  int i;
  if (buffer==NULL) return;
  for (i = 0; i < _iso_bhli_len; i++)
    _iso_bhli[i] = *buffer++;
  if(_iso_bhli_len < 8)
    while(i < 8)
      _iso_bhli[i++] = 0;
  // invalidate cache
  MakeStale();
}


u_char* iso_bhli::get_bhli_buffer(void){
  u_char* buffer = new u_char [_iso_bhli_len];
  memcpy( (u_char*) buffer, (u_char*) _iso_bhli, _iso_bhli_len);
  return(buffer);
}


int iso_bhli::get_bhli_buflen(void) const {
  return (_iso_bhli_len);
}


int iso_bhli::operator == (const ie_bhli & bhli) const
{
  return equals(&bhli);
}


int iso_bhli::equals(const InfoElem* him) const{
  return him->equals(this);
}

int iso_bhli::equals(const iso_bhli* bhliptr) const
{
  int i;
  const iso_bhli* ib=bhliptr->get_iso_bhli();

  if(bhliptr->get_bhli_type() != _bhli_type)
    return(0);
  if(!ib)
    return(0);
  // match lengths
  if(_iso_bhli_len != ib->get_bhli_buflen())
    return(0);
  // match contents
  for(i=0; i<_iso_bhli_len;i++)
    if(_iso_bhli[i] != ib->_iso_bhli[i])
      return(0);
  return(1);
}


InfoElem* iso_bhli::copy(void) const {
  return new iso_bhli((u_char*)_iso_bhli,_iso_bhli_len);
}

InfoElem::ie_status iso_bhli::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  bhli_type bhlitype = (bhli_type)(_bhli_type); // & 0x7f);
  int i;

  if (!len)
    return (InfoElem::empty);
  // max length 9 :8 bytes (bytes 6 thru 13)+_bhli_type which is 1 byte
  if (len > 9)
    return (InfoElem::invalid_contents);
  buffer += ie_header_len;
  if ((*buffer++ & 0x80) != 0x80)
    return (InfoElem::invalid_contents);
  if (bhlitype != iso)
    return (InfoElem::invalid_contents);
  len--;
  if(len)
    {
      _iso_bhli_len = len;
      for (i = 0; i < len; i++)
	_iso_bhli[i] = *buffer++;
      // fill with zero's the remaining slots
      while( i < 8)
	_iso_bhli[i++] = 0;
    }
  return (InfoElem::ok);
}

int iso_bhli::Length()
{
  return (ie_header_len + 1 + _iso_bhli_len);
}

int iso_bhli::encode(u_char * buffer)
{
  u_char * temp;
  int len = 0;
  
  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (len = CacheLength()));
    buffer += len;
    return(len);
  }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);
  put_coding(buffer,_coding);
  put_8(temp,len,(0x80 | iso));
  for (int i = 0; i < _iso_bhli_len; i++)
    put_8(temp,len,_iso_bhli[i]);
  put_len(buffer,len);
  len += ie_header_len;
  FillCache(buffer, len);
  return (len);
}

const iso_bhli*    iso_bhli::get_iso_bhli(void)    const{
  return (const iso_bhli*)this;
}

char iso_bhli::PrintSpecific(ostream & os)
{
  ie_bhli::PrintSpecific(os);
  
  os << "   iso_bhli::0x" << ios::hex;
  for (int i = 0; i < _iso_bhli_len; i++) {
    int val = (0xFF & (int) _iso_bhli[i]);
    if (val < 16) os << "0";
    os << val;
  }
  os << ios::dec << endl;

  return 0;
}

ostream & operator << (ostream & os, iso_bhli & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}
//---------------- user_bhli ----------------------------------------------

user_bhli::user_bhli(u_char *buffer, int buflen) : 
                     ie_bhli(user_specific) 
{
    _user_bhli_len = (buflen > 8 ? 8  : buflen) ;
    int i;
    if (buffer==NULL) return;
    for (i = 0; i < _user_bhli_len; i++)
      _user_bhli[i] = *buffer++;
    if(_user_bhli_len < 8)
      while(i < 8)
	_user_bhli[i++] = 0;
}

user_bhli::user_bhli(const user_bhli & rhs) : ie_bhli(user_specific)
{
  for (int i = 0; i < 8; i++)
    _user_bhli[i] = rhs._user_bhli[i];
  _user_bhli_len = rhs._user_bhli_len;
}

user_bhli::user_bhli(void) : ie_bhli(user_specific) 
{
  u_char *buffer=0L;
  int buflen=8;

    _user_bhli_len = (buflen > 8 ? 8  : buflen) ;
    int i;
    if (buffer==NULL) return;
    for (i = 0; i < _user_bhli_len; i++)
      _user_bhli[i] = *buffer++;
    if(_user_bhli_len < 8)
      while(i < 8)
	_user_bhli[i++] = 0;
}

user_bhli::~user_bhli() { }

void user_bhli::set_bhli(u_char * buffer, int buflen)
{
  _user_bhli_len = (buflen > 8 ? 8  : buflen) ;
  int i;
  if (buffer==NULL) return;
  for (i = 0; i < _user_bhli_len; i++)
    _user_bhli[i] = *buffer++;
  if(_user_bhli_len < 8)
    while(i < 8)
      _user_bhli[i++] = 0;
  MakeStale();
}

u_char* user_bhli::get_bhli_buffer(void){
  u_char* buffer = new u_char [_user_bhli_len];
  memcpy( (u_char*) buffer, (u_char*) _user_bhli, _user_bhli_len);
  return(buffer);
}

int user_bhli::get_bhli_buflen(void) const {
  return (_user_bhli_len);
}

int user_bhli::operator == (const ie_bhli & bhli) const{
  return equals(&bhli);
}

int user_bhli::equals(const InfoElem* him) const{
  return him->equals(this);
}

int user_bhli::equals(const user_bhli* bhliptr) const
{
  int i;
  const user_bhli* ub = bhliptr->get_user_bhli();
  if(bhliptr->get_bhli_type() != _bhli_type)
    return(0);
  if(!ub)
    return(0);
  // match lengths
  if(_user_bhli_len != ub->get_bhli_buflen())
    return(0);
  // match contents
  for(i=0; i<_user_bhli_len;i++)
    if(_user_bhli[i] != ub->_user_bhli[i])
      return(0);
  return(1);
}

InfoElem* user_bhli::copy(void) const {
  return new user_bhli((u_char*)_user_bhli,_user_bhli_len);
}

InfoElem::ie_status user_bhli::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  bhli_type bhlitype = (bhli_type ) (_bhli_type); // & 0x7f);
  int i;

  if (!len)
    return (InfoElem::empty);
  // max length 9 :8 bytes (bytes 6 thru 13)+_bhli_type which is 1 byte
  if (len > 9)
    return (InfoElem::invalid_contents);
  buffer += ie_header_len;
  if ((*buffer++ & 0x80) != 0x80)
    return (InfoElem::invalid_contents);
  if(bhlitype != user_specific)
    return (InfoElem::invalid_contents);
  len--;
  if (len)
    {
      _user_bhli_len = len;
      for (i = 0; i < len; i++)
	_user_bhli[i] = *buffer++;
      // fill with zero's the remaining slots
      while( i < 8)
	_user_bhli[i++] = 0;
    }
  return (InfoElem::ok);
}

int user_bhli::Length()
{
  return (ie_header_len + 1 + _user_bhli_len);
}

int user_bhli::encode(u_char * buffer)
{
  u_char * temp;
  int len = 0;
  
  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (len = CacheLength()));
    buffer += len;
    return(len);
  }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);	
  put_coding(buffer,_coding);
  put_8(temp, len,(0x80 | user_specific));
  for (int i = 0; i < _user_bhli_len; i++)
    put_8(temp,len,_user_bhli[i]);
  put_len(buffer,len);
  len += ie_header_len;
  FillCache(buffer, len);
  return (len);
}

const user_bhli*   user_bhli::get_user_bhli(void)   const{
  return (const user_bhli*)this;
}


char user_bhli::PrintSpecific(ostream & os)
{
  ie_bhli::PrintSpecific(os);
  
  os << "   user_bhli::0x";
  for (int i = 0; i < _user_bhli_len; i++) {
    int val = (0xFF & (int) _user_bhli[i]);
    if (val < 16) os << "0";
    os << val;
  } 
  os << endl;

  return 0;
}

ostream & operator << (ostream & os, user_bhli & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}


//---------------- vendor_bhli --------------------------------------------

vendor_bhli::vendor_bhli(int oui, int app_id) 
: ie_bhli(vendor_specific_application_id),
  _oui(oui), _app_id(app_id) {}

vendor_bhli::vendor_bhli(const vendor_bhli & rhs) : 
  ie_bhli(vendor_specific_application_id), _oui(rhs._oui), _app_id(rhs._app_id) { }

vendor_bhli::vendor_bhli(void)
: ie_bhli(vendor_specific_application_id)
  {
    int oui=0;
    int app_id=0;
    _oui=oui;
    _app_id=app_id;
  }


vendor_bhli::~vendor_bhli() {}

void vendor_bhli::set_bhli(u_char * buffer, int buflen){
  return ;
  MakeStale();
}

u_char* vendor_bhli::get_bhli_buffer(void){
  return 0;
}

int vendor_bhli::get_bhli_buflen(void) const {
  return 0;
}

int vendor_bhli::operator == (const ie_bhli & bhli) const{
  return equals(&bhli);
}

int vendor_bhli::equals(const InfoElem* him) const{
  return him->equals(this);
}

int vendor_bhli::equals(const vendor_bhli* bhliptr) const
{
  if(bhliptr->get_bhli_type() != _bhli_type)
    return(0);

  const vendor_bhli *vb = bhliptr->get_vendor_bhli();
  
  if(vb->_oui != _oui)
    return(0);
  if(vb->_app_id != _app_id)
    return(0);
  return(1);
}

InfoElem* vendor_bhli::copy(void) const {
  return new vendor_bhli(_oui,_app_id);
}

InfoElem::ie_status vendor_bhli::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  bhli_type bhlitype = (bhli_type )(_bhli_type); // & 0x7f);
  int i;

  if (!len)
    return (InfoElem::empty);
  // max length 9 :8 bytes (bytes 6 thru 13)+_bhli_type which is 1 byte
  if (len > 9)
    return (InfoElem::invalid_contents);
  buffer += ie_header_len;
  if ((*buffer++ & 0x80) != 0x80)
    return (InfoElem::invalid_contents);
  if(bhlitype != vendor_specific_application_id)
    return (InfoElem::invalid_contents);
  len--;
  if(len > 4)
    {
      get_32(buffer,_oui);
      len -= 4;
    }
  if(len > 3)
    {
      get_32(buffer,_app_id);
      len -= 4;
    }
  if (len)
    return (InfoElem::invalid_contents);
  return (InfoElem::ok);
}


int vendor_bhli::Length()
{
  // vendor_specific_application_id (1) + _oui(4) + _app_id(4)
  return (ie_header_len + 9);
}

int vendor_bhli::encode(u_char * buffer)
{
  u_char * temp;
  int len = 0;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (len = CacheLength()));
    buffer += len;
    return(len);
  }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);	
  put_coding(buffer,_coding);
  put_8(temp, len,(0x80 | vendor_specific_application_id));
  put_32(temp,len,_oui);
  put_32(temp,len,_app_id);
  put_len(buffer,len);
  len += ie_header_len;
  FillCache(buffer, len);
  return (len);
}

const vendor_bhli* vendor_bhli::get_vendor_bhli(void) const{
  return (const vendor_bhli*)this;
}

char vendor_bhli::PrintSpecific(ostream & os)
{
  ie_bhli::PrintSpecific(os);
  
  os << "   oui::" << _oui << endl;
  os << "   app_id::" << _app_id << endl;

  return 0;
}

ostream & operator << (ostream & os, vendor_bhli & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}
