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
static char const _transit_net_id_cc_rcsid_[] =
"$Id: transit_net_id.cc,v 1.13 1998/09/24 13:38:46 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/transit_net_id.h>

void HexDump(const u_char * buffer, int len, ostream & os, bool nid = false);

ig_transit_net_id::ig_transit_net_id(u_char net_id_data, u_char *net_id, int net_id_len) :
  InfoGroup(InfoGroup::ig_transit_net_id_id), _TNS_length(0),
  _network_id_data(net_id_data), _network_id(0),
  _net_id_length(net_id_len), _padding(0)
{
  if (net_id) {
    assert(net_id_len > 0);
    _network_id = new u_char [ _net_id_length ];
    bcopy(net_id, _network_id, _net_id_length);
  }
  _TNS_length = 1 + _net_id_length;
  _padding = (4 - ((3 + _net_id_length)));
}

ig_transit_net_id::ig_transit_net_id(const ig_transit_net_id & rhs) 
  : InfoGroup(InfoGroup::ig_transit_net_id_id), _TNS_length(rhs._TNS_length),
    _network_id_data(rhs._network_id_data), _network_id(0), 
    _net_id_length(rhs._net_id_length), _padding(rhs._padding)
{ 
  if (rhs._network_id) {
    _network_id = new u_char [ _net_id_length ];
    bcopy(rhs._network_id, _network_id, _net_id_length);
  }
}

ig_transit_net_id::~ig_transit_net_id() 
{ 
  if (_network_id)
    delete [] _network_id;
}

InfoGroup * ig_transit_net_id::copy(void)
{
  return new ig_transit_net_id(*this);
}

void ig_transit_net_id::size(int & length)
{
 length += FIXED_TRANSITID_CONTENT; //content w/o Netinfo + padding  7
                                    // Add these when reqd
}

u_char * ig_transit_net_id::encode(u_char *buffer, int & buflen)
{
  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  *temp++ = (_TNS_length >> 8) & 0xFF;
  *temp++ = (_TNS_length & 0xFF);

  *temp++ = _network_id_data;

  buflen += 3;

  for (int i = 0; i < _net_id_length; i++)
    *temp++ = _network_id[i];
  buflen  += _net_id_length;

  for (int j = 0; j < _padding; j++)
    *temp++ = 0;
  buflen += _padding;

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_transit_net_id::decode(u_char *buffer)
{
  int type = ((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF);
  int len  = ((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF);

  if (len < 3 || (type != InfoGroup::ig_transit_net_id_id))
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);

  u_char * temp = &buffer[4];
  _TNS_length  = (*temp++ << 8) & 0xFF00;
  _TNS_length |= (*temp++ & 0xFF);

  _network_id_data = *temp++;

  len -= 3;

  _net_id_length = _TNS_length - 1;
  if (_network_id)
    delete [] _network_id;
  _network_id = new u_char [ _net_id_length ];
  for (int i = 0; i < _net_id_length; i++)
    _network_id[i] = *temp++, len--;

  // Now count the padding
  _padding = (4 - ((3 + _net_id_length)    ))    ;
  len -= _padding;

  if (len < 0)
    return new errmsg(errmsg::invalid_contents, errmsg::none, type);
  else
    return 0;
}

const sh_int  ig_transit_net_id::GetTNSLength(void) const   { return _TNS_length; }
const u_char  ig_transit_net_id::GetNetworkData(void) const { return _network_id_data; }
const u_char *ig_transit_net_id::GetNetworkID(void) const   { return _network_id; }
const sh_int  ig_transit_net_id::GetNetIDLength(void) const { return _net_id_length; }
const sh_int  ig_transit_net_id::GetPadding(void) const     { return _padding; }

bool ig_transit_net_id::PrintSpecific(ostream& os)
{
  os << "     TNS Length::" << _TNS_length << endl;
  os << "     Network ID Data::" << _network_id_data << endl;
  os << "     Network ID::";
  HexDump(_network_id, _net_id_length, os);
  os << endl;
  
  return true;
}

bool ig_transit_net_id::ReadSpecific(istream& is)
{
  return false;
}

ostream & operator << (ostream & os, ig_transit_net_id & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

istream & operator >> (istream & is, ig_transit_net_id & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return (is);
}
