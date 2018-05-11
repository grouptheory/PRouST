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
static char const _syscap_cc_rcsid_[] =
"$Id: system_capabilities.cc,v 1.5 1998/09/24 13:22:13 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/system_capabilities.h>

ig_system_capabilities::ig_system_capabilities(sh_int length, int oui, u_char *info) :
  InfoGroup(InfoGroup::ig_system_capabilities_id), _content_length(length),
  _ieee_oui(oui), _padding_sz(0)
{
  int size = _content_length - 3;

  _info = new u_char [size];
  bcopy(info, _info, size);
  _padding_sz = (4 - ((5 + size)    ))    ;
}

ig_system_capabilities::ig_system_capabilities(const ig_system_capabilities & rhs)
  : InfoGroup(InfoGroup::ig_system_capabilities_id), _info(0),
    _content_length(rhs._content_length), _ieee_oui(rhs._ieee_oui),
    _padding_sz(rhs._padding_sz)
{
  if (rhs._info) {
    _info = new u_char [rhs._content_length - 3];
    bcopy(rhs._info, _info, rhs._content_length - 3);
    // padding size should've been set above
  }
}

ig_system_capabilities::~ig_system_capabilities() 
{ 
  if (_info)
    delete [] _info;
}

InfoGroup * ig_system_capabilities::copy(void)
{
  return new ig_system_capabilities(*this);
}

void ig_system_capabilities::size(int & length)
{
  length += FIXED_SYS_CAP_CONTENT; // without Sys Cap Inf + Padding 9
  // Need to update for syscapinfo and Padding
}

u_char * ig_system_capabilities::encode(u_char * buffer, int & buflen)
{
  int i;

  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  *temp++ = (_content_length >> 8) & 0xFF;
  *temp++ = (_content_length & 0xFF);

  *temp++ = (_ieee_oui >> 16) & 0xFF;
  *temp++ = (_ieee_oui >>  8) & 0xFF;
  *temp++ = (_ieee_oui & 0xFF);

  buflen += 5;

  for (i = 0; i < (_content_length - 3); i++)
    *temp++ = _info[i];
  buflen += (_content_length - 3);

  for (i = 0; i < _padding_sz; i++)
    *temp++ = 0;
  buflen += _padding_sz;

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_system_capabilities::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_system_capabilities_id ||
      len  < 5)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);
  u_char * temp = &buffer[4];

  _content_length  = (*temp++ << 8) & 0xFF00;
  _content_length |= (*temp++ & 0xFF);

  _ieee_oui  = (*temp++ << 16) & 0xFF0000;
  _ieee_oui |= (*temp++ <<  8) & 0x00FF00;
  _ieee_oui |= (*temp++ & 0xFF);
  len -= 5;

  if (_info)
    delete [] _info;
  _info = new u_char [_content_length - 3];
  if (len < (_content_length - 3))
    return new errmsg(errmsg::invalid_contents, errmsg::none, type);
  for (int i = 0; i < (_content_length - 3); i++)
    _info[i] = *temp++;
  len -= (_content_length - 3);
  _padding_sz = (4 - ((5 + (_content_length - 3))    ))    ;
  if (len < _padding_sz)
    return new errmsg(errmsg::invalid_contents, errmsg::none, type);
  temp += _padding_sz;
  return 0;
}

const sh_int ig_system_capabilities::GetContentLength(void) const { return _content_length; }
const    int ig_system_capabilities::GetIEEEOUI(void) const { return _ieee_oui; }
const u_char * ig_system_capabilities::GetInfo(void) const { return _info; }
const u_char ig_system_capabilities::GetPaddingSize(void) const { return _padding_sz; }

bool ig_system_capabilities::PrintSpecific(ostream & os)
{
  os << "    Length::" << _content_length << endl;
  os << "    OUI::" << _ieee_oui << endl;
  os << "    Info::";
  for (int i = 0; i < (_content_length - 3); i++)
    os << _info[i];
  os << endl;
  return true;
}

bool ig_system_capabilities::ReadSpecific(istream & is)
{
  char buf[80], *temp = buf;

  is.getline(buf, 80);
  while (*temp && !isdigit(*temp))
    temp++;
  _content_length = atoi(temp);

  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _ieee_oui = atoi(temp);

  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _info = new u_char [(_content_length - 3)];
  for (int i =0; i < (_content_length - 3); i ++)
    is >> _info[i];
  _padding_sz = (4 - (5 + (_content_length - 3))    )    ;
  return true;
}

ostream & operator << (ostream & os, ig_system_capabilities & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return os;
}

istream & operator >> (istream & is, ig_system_capabilities & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return is;
}
