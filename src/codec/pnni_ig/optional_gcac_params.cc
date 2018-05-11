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
static char const _opt_gcac_params_cc_rcsid_[] =
"$Id: optional_gcac_params.cc,v 1.5 1998/09/24 13:14:14 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/optional_gcac_params.h>

ig_optional_gcac_params::ig_optional_gcac_params(int crm, int vf) : 
  InfoGroup(InfoGroup::ig_optional_gcac_params_id), _cell_rate_margin(crm),
  _variance_factor(vf) { }

ig_optional_gcac_params::ig_optional_gcac_params(const ig_optional_gcac_params & rhs) 
  : InfoGroup(InfoGroup::ig_optional_gcac_params_id),
    _cell_rate_margin(rhs._cell_rate_margin), _variance_factor(rhs._variance_factor) { }

ig_optional_gcac_params::~ig_optional_gcac_params() { }

InfoGroup * ig_optional_gcac_params::copy(void)
{
  return new ig_optional_gcac_params(*this);
}

void ig_optional_gcac_params::size(int & length)
{
  length += GCAC_CONTENT; // default size of gcac 12
}

u_char * ig_optional_gcac_params::encode(u_char * buffer, int & buflen)
{
  buflen = 0;
  u_char * temp = buffer + ig_header_len; 

  *temp++ = (_cell_rate_margin >> 24) & 0xFF;
  *temp++ = (_cell_rate_margin >> 16) & 0xFF;
  *temp++ = (_cell_rate_margin >>  8) & 0xFF;
  *temp++ = (_cell_rate_margin & 0xFF);

  *temp++ = (_variance_factor >> 24) & 0xFF;
  *temp++ = (_variance_factor >> 16) & 0xFF;
  *temp++ = (_variance_factor >>  8) & 0xFF;
  *temp++ = (_variance_factor & 0xFF);

  buflen += 8;

  encode_ig_header(buffer, buflen); 
  return temp;
}

errmsg * ig_optional_gcac_params::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_optional_gcac_params_id ||
      len < 8)
    return new errmsg(errmsg::incomplete_ig, errmsg::wrong_id, type);

  decode_ig_header(buffer);
  u_char * temp = &buffer[4];

  _cell_rate_margin  = (*temp++ << 24) & 0xFF000000;
  _cell_rate_margin |= (*temp++ << 16) & 0x00FF0000;
  _cell_rate_margin |= (*temp++ <<  8) & 0x0000FF00;
  _cell_rate_margin |= (*temp++ & 0xFF);

  _variance_factor  = (*temp++ << 24) & 0xFF000000;
  _variance_factor |= (*temp++ << 16) & 0x00FF0000;
  _variance_factor |= (*temp++ <<  8) & 0x0000FF00;
  _variance_factor |= (*temp++ & 0xFF);

  return 0;
}

const int ig_optional_gcac_params::GetCellRateMargin(void) const
{ return _cell_rate_margin; }

const int ig_optional_gcac_params::GetVarianceFactor(void) const
{ return _variance_factor; }

bool ig_optional_gcac_params::PrintSpecific(ostream & os)
{
  os << "     Cell Rate Margin::" << _cell_rate_margin << endl;
  os << "     Variance Factor::" << _variance_factor << endl;
  os << endl;
  return true;
}

bool ig_optional_gcac_params::ReadSpecific(istream & is)
{
  char buf[80], *temp = buf;

  is.getline(buf, 80);
  while (*temp && !isdigit(*temp))
    temp++;
  _cell_rate_margin = atoi(temp);
  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _variance_factor = atoi(temp);
  return true;
}

ostream & operator << (ostream & os, ig_optional_gcac_params & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return os;
}

istream & operator >> (istream & is, ig_optional_gcac_params & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return is;
}
