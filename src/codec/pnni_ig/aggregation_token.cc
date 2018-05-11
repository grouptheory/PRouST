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
static char const _agg_tok_cc_rcsid_[] =
"$Id: aggregation_token.cc,v 1.5 1998/09/23 18:45:25 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/aggregation_token.h>

ig_aggregation_token::ig_aggregation_token(long token) 
  : InfoGroup(InfoGroup::ig_aggregation_token_id),
  _aggregation_token(token) { }

ig_aggregation_token::ig_aggregation_token(const ig_aggregation_token & tok) : 
  InfoGroup(InfoGroup::ig_aggregation_token_id), 
  _aggregation_token(tok._aggregation_token) { }

ig_aggregation_token::~ig_aggregation_token() { }

InfoGroup * ig_aggregation_token::copy(void)
{
  // Copy ctor works in this case ...
  ig_aggregation_token * rval = new ig_aggregation_token(*this);
  return rval;
}

void ig_aggregation_token::size(int & length)
{
  length += SIZE_OF_AGG_TOKEN_IE; // 8 bytes
}

int ig_aggregation_token::equals(const InfoGroup * other) const
{
  int rval = InfoGroup::equals(other);

  if (!rval) {
    // We're still equal, so other must be an agg tok
    ig_aggregation_token * rhs = (ig_aggregation_token *)other;
    if (_aggregation_token < rhs->_aggregation_token)
      rval = -1;
    else if (_aggregation_token > rhs->_aggregation_token)
      rval = 1;
  }
  return rval;
}


u_char * ig_aggregation_token::encode(u_char *buffer, int & buflen)
{
  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  *temp++ = (_aggregation_token >> 24) & 0xFF;
  *temp++ = (_aggregation_token >> 16) & 0xFF;
  *temp++ = (_aggregation_token >>  8) & 0xFF;
  *temp++ = (_aggregation_token & 0xFF);

  buflen += 4;
  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_aggregation_token::decode(u_char *buffer)
{
  errmsg * rval = 0;

  decode_ig_header(buffer);

  int len = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));

  if (len < 4 || type != InfoGroup::ig_aggregation_token_id)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  u_char * temp = &buffer[4];

  _aggregation_token  = (*temp++ << 24) & 0xFF000000;
  _aggregation_token |= (*temp++ << 16) & 0x00FF0000;
  _aggregation_token |= (*temp++ <<  8) & 0x0000FF00;
  _aggregation_token |= (*temp++ & 0xFF);

  return rval;
}
      
bool ig_aggregation_token::PrintSpecific(ostream& os)
{
  os << "    Aggregation Token::" << _aggregation_token << endl;
  return true;
}

bool ig_aggregation_token::ReadSpecific(istream& is)
{
  char buf[80], *temp = buf;

  is.getline(buf, 80);

  while (*temp && !isdigit(*temp))
    temp++;

  if (*temp)
    _aggregation_token = atol(temp);
  else
    return false;
  return true;
}

ostream & operator << (ostream & os, ig_aggregation_token & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

istream & operator >> (istream & is, ig_aggregation_token & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return (is);
}
