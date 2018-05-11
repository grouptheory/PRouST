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
static char const _lgn_horiz_link_cc_rcsid_[] =
"$Id: lgn_horizontal_link_ext.cc,v 1.12 1999/01/25 14:26:41 mountcas Exp bilal $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/lgn_horizontal_link_ext.h>

ostream & operator << (ostream & os, ig_lgn_horizontal_link_ext & ig);
istream & operator >> (istream & is, ig_lgn_horizontal_link_ext & ig);

ig_lgn_horizontal_link_ext::ig_lgn_horizontal_link_ext(void) : 
  InfoGroup(InfoGroup::ig_lgn_horizontal_link_ext_id), _hlink_count(0) { }

ig_lgn_horizontal_link_ext::ig_lgn_horizontal_link_ext(const ig_lgn_horizontal_link_ext & rhs) : 
  InfoGroup(InfoGroup::ig_lgn_horizontal_link_ext_id), _hlink_count(0) 
{
  if (rhs._hlinks.empty() == false) {
    list_item li;
    forall_items(li, rhs._hlinks) {
      HLinkCont * hptr = (rhs._hlinks).inf(li);
      AddHLink(hptr->_aggregation_token, 
	       hptr->_local_lgn_port, 
	       hptr->_remote_lgn_port);
    }
  }
}
 
ig_lgn_horizontal_link_ext::~ig_lgn_horizontal_link_ext()
{  
  list_item li;
  forall_items(li, _hlinks) {
    HLinkCont * hptr = _hlinks.inf(li);
    delete hptr; 
  }
  _hlinks.clear();
}

InfoGroup * ig_lgn_horizontal_link_ext::copy(void)
{
  return new ig_lgn_horizontal_link_ext(*this);
}

void ig_lgn_horizontal_link_ext::size(int & length)
{  
 length += LGN_EXT_HEADER;                // 8
 length += _hlink_count * AGGR_PORT_INFO; // 12
}

u_char * ig_lgn_horizontal_link_ext::encode(u_char * buffer, int & buflen)
{
  HLinkCont * hptr;

  buflen = 0;
  u_char * temp = buffer + ig_header_len;
    
  // First two bytes are reserved
  *temp++ = 0;
  *temp++ = 0;

  // HLink Count
  *temp++ = (_hlink_count >> 8) & 0xFF;
  *temp++ = (_hlink_count & 0xFF);

  buflen += 4;

  list_item li;
  forall_items(li, _hlinks)
  {
    hptr = _hlinks.inf(li);
    int tlen =0;
    temp = hptr->encode(temp, tlen);
    buflen += tlen;
  }

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_lgn_horizontal_link_ext::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_lgn_horizontal_link_ext_id || len < 4)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);

  u_char * temp = &buffer[4];

  // Skip past the reserved bytes
  *temp++;
  *temp++;
  len -= 2;

  _hlink_count  = (*temp++ << 8) & 0xFF00;
  _hlink_count |= (*temp++ & 0xFF);
  len -= 2;

  int counter = _hlink_count;
  for (int i = 0; i < counter; i++)
  {
    int a, l, r;

    a  = (*temp++ << 24) & 0xFF000000;
    a |= (*temp++ << 16) & 0x00FF0000;
    a |= (*temp++ <<  8) & 0x0000FF00;
    a |= (*temp++ & 0xFF);

    l  = (*temp++ << 24) & 0xFF000000;
    l |= (*temp++ << 16) & 0x00FF0000;
    l |= (*temp++ <<  8) & 0x0000FF00;
    l |= (*temp++ & 0xFF);

    r  = (*temp++ << 24) & 0xFF000000;
    r |= (*temp++ << 16) & 0x00FF0000;
    r |= (*temp++ <<  8) & 0x0000FF00;
    r |= (*temp++ & 0xFF);

    len -= 12;

    AddHLink(a, l, r);
  }

  if (len < 0)
    return new errmsg(errmsg::incomplete_ig, errmsg::none, type);
  else
    return 0;
}

void ig_lgn_horizontal_link_ext::AddHLink(int a, int l, int r)
{
  bool found = false;
  list_item li;

  forall_items(li, _hlinks) {
    HLinkCont * hptr = _hlinks.inf(li);
    if (a == hptr->_aggregation_token) {
      hptr->_local_lgn_port  = l;
      hptr->_remote_lgn_port = r;
      found = true;
      break;
    }
  }

  if (!found) {
    HLinkCont * hptr = new HLinkCont(a, l, r);
    _hlinks.append(hptr);
    // couldn't we just increment this?
    _hlink_count = _hlinks.length();
  }
}

bool ig_lgn_horizontal_link_ext::DelHLink(int a, int l, int r)
{
  HLinkCont hcont(a, l, r);

  list_item li;
  forall_items(li, _hlinks) {
    HLinkCont * hptr = _hlinks.inf(li);
    if (hptr->equals(hcont)) {
      _hlinks.del_item(_hlinks.search(hptr));
      // couldn't we just decrement this?
      _hlink_count = _hlinks.length();
      return true;
    }
  }
  return false;
}

ig_lgn_horizontal_link_ext:: HLinkCont * 
ig_lgn_horizontal_link_ext::FindHLinkCont(int aggrToken)
{
  HLinkCont hCont(aggrToken, 0, 0);
  list_item li;
  if (li = _hlinks.search(&hCont))
   return _hlinks.inf(li);
  return 0L;
}

bool ig_lgn_horizontal_link_ext::PrintSpecific(ostream & os)
{
  os << "     HLink Count::" << _hlink_count << endl;
  os << "     HLinks::" << endl;
  list_item li;
  forall_items(li, _hlinks) {
    HLinkCont * hptr = _hlinks.inf(li);
    hptr->Print(os);
  }
  return true;
}

bool ig_lgn_horizontal_link_ext::ReadSpecific(istream & is)
{
  char buf[80], *temp = buf;

  is.getline(buf, 80);
  while (*temp && !isdigit(*temp))
    temp++;
  _hlink_count = atoi(temp);
  // Skip  HLinks:: line
  is.getline(buf, 80);
  int counter = _hlink_count;
  for (int i = 0; i < counter; i++) {
    is.getline(buf, 80);
    temp = buf;
    while (*temp && !isdigit(*temp))
      temp++;
    int a = atoi(temp);
    is.getline(buf, 80);
    temp = buf;
    while (*temp && !isdigit(*temp))
      temp++;
    int l = atoi(temp);
    is.getline(buf, 80);
    temp = buf;
    while (*temp && !isdigit(*temp))
      temp++;
    int r = atoi(temp);
    AddHLink(a, l, r);
  }
  return true;
}
  
u_char * ig_lgn_horizontal_link_ext::HLinkCont::encode(u_char * buffer, int & buflen)
{
  buflen = 0;

  *buffer++ = (_aggregation_token >> 24) & 0xFF;
  *buffer++ = (_aggregation_token >> 16) & 0xFF;
  *buffer++ = (_aggregation_token >>  8) & 0xFF;
  *buffer++ = (_aggregation_token & 0xFF);

  *buffer++ = (_local_lgn_port >> 24) & 0xFF;
  *buffer++ = (_local_lgn_port >> 16) & 0xFF;
  *buffer++ = (_local_lgn_port >>  8) & 0xFF;
  *buffer++ = (_local_lgn_port & 0xFF);

  *buffer++ = (_remote_lgn_port >> 24) & 0xFF;
  *buffer++ = (_remote_lgn_port >> 16) & 0xFF;
  *buffer++ = (_remote_lgn_port >>  8) & 0xFF;
  *buffer++ = (_remote_lgn_port & 0xFF);

  buflen += 12;

  return buffer;
}

bool ig_lgn_horizontal_link_ext::HLinkCont::Print(ostream & os)
{
  os << "     Aggregation token::" << _aggregation_token << endl;
  os << "     Local LGN port::" << _local_lgn_port << endl;
  os << "     Remote LGN port::" << _remote_lgn_port << endl;
  return true;
}

bool ig_lgn_horizontal_link_ext::HLinkCont::Read(istream & is)
{
  char buf[80], *temp = buf;

  is.getline(buf, 80);

  while (*temp && !isdigit(*temp))
    temp++;
  _aggregation_token = atoi(temp);

  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _local_lgn_port = atoi(temp);

  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _remote_lgn_port = atoi(temp);

  return true;
}

int compare(ig_lgn_horizontal_link_ext::HLinkCont * const & lhs,
	    ig_lgn_horizontal_link_ext::HLinkCont * const & rhs)
{
  if (lhs->_aggregation_token < rhs->_aggregation_token) return -1;
  if (rhs->_aggregation_token < lhs->_aggregation_token) return +1;
  return 0;
}

ostream & operator << (ostream & os, ig_lgn_horizontal_link_ext & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

const list<ig_lgn_horizontal_link_ext::HLinkCont *> & ig_lgn_horizontal_link_ext::GetLinks(void) const 
{ return _hlinks; }

const list<ig_lgn_horizontal_link_ext::HLinkCont *> * ig_lgn_horizontal_link_ext::ShareLinks(void) const 
{ return &_hlinks; }

const sh_int ig_lgn_horizontal_link_ext::GetHLinkCount(void)
{ return _hlinks.size(); }

ig_lgn_horizontal_link_ext::HLinkCont::HLinkCont(int a, int l, int r) :
  _aggregation_token(a), _local_lgn_port(l),
  _remote_lgn_port(r), _uplinks_count(1)
{ }

ig_lgn_horizontal_link_ext::HLinkCont::~HLinkCont() { }

int  ig_lgn_horizontal_link_ext::HLinkCont::GetAggTok(void) const
{ return _aggregation_token; }

int  ig_lgn_horizontal_link_ext::HLinkCont::GetLocalPort(void) const
{ return _local_lgn_port; }

int ig_lgn_horizontal_link_ext::HLinkCont:: GetRemotePort(void) const
{ return _remote_lgn_port; }

int  ig_lgn_horizontal_link_ext::HLinkCont::GetUplinkCount(void) const
{return _uplinks_count; }

void ig_lgn_horizontal_link_ext::HLinkCont::SetUplinkCount(int new_count)
{ _uplinks_count = new_count; }

int ig_lgn_horizontal_link_ext::HLinkCont::equals(HLinkCont & rhs)
{
  return ((_aggregation_token == rhs._aggregation_token) &&
	  (_local_lgn_port == rhs._local_lgn_port) &&
	  (_remote_lgn_port == rhs._remote_lgn_port));
}
