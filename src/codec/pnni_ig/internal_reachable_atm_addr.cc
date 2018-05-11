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
static char const _int_reach_atm_addr_cc_rcsid_[] =
"$Id: internal_reachable_atm_addr.cc,v 1.10 1998/09/24 13:37:28 mountcas Exp $";
#endif
#include <common/cprototypes.h>

extern "C" {
#include <math.h>
};
#include <codec/pnni_ig/internal_reachable_atm_addr.h>
#include <codec/pnni_ig/resrc_avail_info.h>

ig_internal_reachable_atm_addr::ig_internal_reachable_atm_addr(int pid, 
	   u_char ad_scope, u_char addrnfolen) :
  InfoGroup(InfoGroup::ig_internal_reachable_atm_addr_id), _flags(0), _pid(pid),
  _ad_scope(ad_scope), _addr_info_len(addrnfolen), _addr_info_cnt(0),
  _padding(0) { }

ig_internal_reachable_atm_addr::ig_internal_reachable_atm_addr(const ig_internal_reachable_atm_addr & rhs)
  : InfoGroup(InfoGroup::ig_internal_reachable_atm_addr_id), _flags(rhs._flags), 
    _pid(rhs._pid), _ad_scope(rhs._ad_scope), _addr_info_len(rhs._addr_info_len), 
    _addr_info_cnt(rhs._addr_info_cnt), _padding(rhs._padding)
{
  if (rhs._prefixes.empty() == false) {
    list_item li;
    forall_items(li, rhs._prefixes) {
      _prefixes.append( new AddrPrefixCont( *(rhs._prefixes.inf(li)) ) );
    }
  }
  if (rhs._outgoing_raigs.empty() == false) {
    list_item li;
    forall_items(li, rhs._outgoing_raigs) {
      _outgoing_raigs.append( (ig_resrc_avail_info *)rhs._outgoing_raigs.inf(li)->copy() );
    }
  }
  if (rhs._incoming_raigs.empty() == false) {
    list_item li;
    forall_items(li, rhs._incoming_raigs) {
      _incoming_raigs.append( (ig_resrc_avail_info *)rhs._incoming_raigs.inf(li)->copy() );
    }
  }
}

ig_internal_reachable_atm_addr::~ig_internal_reachable_atm_addr()
{
  AddrPrefixCont * aptr;
  ig_resrc_avail_info *raig_ptr;

  list_item li;
  forall_items(li, _prefixes) {
    if (aptr = _prefixes.inf(li)) 
      delete aptr;
  }
  _prefixes.clear();
  forall_items(li, _outgoing_raigs) { 
    if (raig_ptr = _outgoing_raigs.inf(li))
      delete raig_ptr;
  }
  _outgoing_raigs.clear();

  forall_items(li, _incoming_raigs) {
    if (raig_ptr = _outgoing_raigs.inf(li))
      delete raig_ptr;
  }
  _incoming_raigs.clear();
}

InfoGroup * ig_internal_reachable_atm_addr::copy(void)
{
  return new ig_internal_reachable_atm_addr(*this);
}

void ig_internal_reachable_atm_addr::size(int & length)
{
  list_item li;
  length += INT_REACH_HEADER; // Initial 16 bytes.
  forall_items(li, _prefixes)
  {
    (_prefixes.inf(li))->size(length);
  }
  length += 4 - (int)(fmod(fmod((double)(_addr_info_cnt * _addr_info_len), 4.0),
 4.0));
  li = 0;
  forall_items(li, _outgoing_raigs)
  {
    (_outgoing_raigs.inf(li))->size(length);
  }

  li = 0;
  forall_items(li, _incoming_raigs)
  {
    (_incoming_raigs.inf(li))->size(length);
  }
}

int ig_internal_reachable_atm_addr::equals(const InfoGroup * other) const
{
  int rval = InfoGroup::equals(other);
  if (!rval) {
    ig_internal_reachable_atm_addr * rhs = (ig_internal_reachable_atm_addr *)other;
    if ((_flags < rhs->_flags) ||
	((_flags == rhs->_flags) && (_pid < rhs->_pid)) ||
	((_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope < rhs->_ad_scope)) ||
	((_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope == rhs->_ad_scope) &&
	 (_addr_info_len < rhs->_addr_info_len)) ||
	(_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope == rhs->_ad_scope) &&
	 (_addr_info_len == rhs->_addr_info_len) && (_addr_info_cnt < rhs->_addr_info_cnt))
      return -1;
    else if ((_flags > rhs->_flags) ||
	((_flags == rhs->_flags) && (_pid > rhs->_pid)) ||
	((_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope > rhs->_ad_scope)) ||
	((_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope == rhs->_ad_scope) &&
	 (_addr_info_len > rhs->_addr_info_len)) ||
	(_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope == rhs->_ad_scope) &&
	 (_addr_info_len == rhs->_addr_info_len) && (_addr_info_cnt > rhs->_addr_info_cnt))
      return 1;
  }
  return rval;
}


u_char * ig_internal_reachable_atm_addr::encode(u_char * buffer, int & buflen)
{
  int i;

  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  *temp++ = (_flags >> 8) & 0xFF;
  *temp++ = (_flags & 0xFF);

  temp += 2; // Reserved

  *temp++ = (_pid >> 24) & 0xFF;
  *temp++ = (_pid >> 16) & 0xFF;
  *temp++ = (_pid >>  8) & 0xFF;
  *temp++ = (_pid & 0xFF);

  *temp++ = _ad_scope;

  *temp++ = _addr_info_len;

  *temp++ = (_addr_info_cnt >> 8) & 0xFF;
  *temp++ = (_addr_info_cnt & 0xFF);

  buflen  += 12;

  list_item li;
  forall_items(li, _prefixes)
  {
    AddrPrefixCont * aptr = _prefixes.inf(li);
    int tlen = 0;
    temp = aptr->encode(temp, tlen);
    buflen += tlen;
  }

  _padding = 4 - (int)(fmod(fmod((double)(_addr_info_cnt * _addr_info_len), 4.0),4.0));
  for(i = 0 ; i < _padding ; i++)
    *temp++ = 0;

  buflen += _padding;

  forall_items(li, _outgoing_raigs)
  {
    ig_resrc_avail_info * rptr = _outgoing_raigs.inf(li);
    int tlen = 0;
    temp = rptr->encode(temp, tlen);
    buflen += tlen;
  }

  forall_items(li, _incoming_raigs)
  {
    ig_resrc_avail_info * rptr = _incoming_raigs.inf(li);
    int tlen = 0;
    temp = rptr->encode(temp, tlen);
    buflen += tlen;
  }

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_internal_reachable_atm_addr::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_internal_reachable_atm_addr_id ||
      len < 12)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);
  u_char *temp = &buffer[4];

  _flags  = (*temp++ << 8) & 0xFF00;
  _flags |= (*temp++ & 0xFF);
  temp += 2; // Reserved
  _pid  = (*temp++ << 24) & 0xFF000000;
  _pid |= (*temp++ << 16) & 0x00FF0000;
  _pid |= (*temp++ <<  8) & 0x0000FF00;
  _pid |= (*temp++ & 0xFF);
  _ad_scope = (*temp++);
  _addr_info_len = (*temp++);
  _addr_info_cnt  = (*temp++ << 8) & 0xFF00;
  _addr_info_cnt |= (*temp++ & 0xFF);
  int counter = _addr_info_cnt;
  len -= 12;

  while (len > 0 && counter--)
  {
    u_char plen;
    u_char * reachable = new u_char [((int)_addr_info_len -1)];
    bzero(reachable, ((int)_addr_info_len -1));

    plen = *temp++;

    int bytelen  = ((int)plen + 7)/8;
    for (int i = 0; i < bytelen; i++)
    {
      reachable[i] = *temp++;
    }

    temp += ((int)_addr_info_len -1) - bytelen;

    len -= _addr_info_len ;

    // Assuming right now
    // and also the rechable address comes with zero's till
    // ail -1 after the preflen bits.
    // can be resolved to bit specific accuracy
    // by anding the last byte with 00, 80, 40, 20, 08,04, 02, later
    AddrPrefixCont * adCont = new  AddrPrefixCont(plen, reachable, _addr_info_len);
   _prefixes.append(adCont);
    delete [] reachable;
  }

  _padding = 4 - (int)(fmod((fmod((double)(_addr_info_len * _addr_info_cnt), 4.0)), 4.0));

  temp += _padding;
  len  -= _padding;

  errmsg * rval = 0;

  while (len > 0)
  {
    int type = (((temp[0] << 8 ) & 0xFF00) | ( temp[1] & 0xFF ));
    int tlen = (((temp[2] << 8 ) & 0xFF00) | (temp[3] & 0xFF));

    if(type == (InfoGroup::ig_outgoing_resource_avail_id))
    {
      ig_resrc_avail_info * ig = new ig_resrc_avail_info(ig_resrc_avail_info::outgoing);

      rval = ig->decode(temp);
      _outgoing_raigs.append(ig);
    }
    else if(type == (InfoGroup::ig_incoming_resource_avail_id))
    {
      ig_resrc_avail_info * ig = new ig_resrc_avail_info(ig_resrc_avail_info::incoming);
      rval = ig->decode(temp);
      _incoming_raigs.append(ig);
    }
    else
    {
      return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);
    }

    if(rval) return rval;

    len  -= tlen + 4;
    temp += tlen + 4;
  }

  return rval;
}

void ig_internal_reachable_atm_addr::AddPrefix(u_char plen, u_char *reachable)
{
  AddrPrefixCont * aptr = new AddrPrefixCont(plen, reachable, _addr_info_len);
  _prefixes.append(aptr);
  _addr_info_cnt = _prefixes.length();
}

bool ig_internal_reachable_atm_addr::RemPrefix(u_char plen, u_char *reachable)
{
  AddrPrefixCont * aptr, addr(plen, reachable, _addr_info_len);

  list_item li;
  forall_items(li, _prefixes) {
    aptr = _prefixes.inf(li);
    if (aptr->equals(addr)) {
      _prefixes.del_item(_prefixes.search(aptr));
      delete aptr;
      _addr_info_cnt = _prefixes.length();
      return true;
    }
  }
  return false;
}

const list<struct AddrPrefixCont *> & ig_internal_reachable_atm_addr::GetPrefixes(void) const
{ return _prefixes; }

const int    ig_internal_reachable_atm_addr::GetPID(void) const { return _pid; }
const u_char ig_internal_reachable_atm_addr::GetAddrScope(void) const { return _ad_scope; }
const u_char ig_internal_reachable_atm_addr::GetAddrInfoLen(void) const { return _addr_info_len; }
const sh_int ig_internal_reachable_atm_addr::GetAddrInfoCnt(void) const { return _addr_info_cnt; }

const list<ig_resrc_avail_info *> & ig_internal_reachable_atm_addr::GetOutGoingRAIGS(void) const
{
  return _outgoing_raigs;
}

const list<ig_resrc_avail_info *> & ig_internal_reachable_atm_addr::GetInComingRAIGS(void) const
{
  return _incoming_raigs;
}

void  ig_internal_reachable_atm_addr::AddOutGoingRAIG(ig_resrc_avail_info *p)
{
  _outgoing_raigs.append(p);
}

void ig_internal_reachable_atm_addr::AddInComingRAIG(ig_resrc_avail_info *p)
{
  _incoming_raigs.append(p);
}

bool ig_internal_reachable_atm_addr::PrintSpecific(ostream & os)
{
  os.setf(ios::hex);
  os << "    Flags::" << _flags << endl;
  os.setf(ios::dec);
  os << "    PID::" << _pid << endl;
  os << "    AD Scope::" << _ad_scope << endl;
  os << "    Addr Info Len::" << _addr_info_len << endl;
  os << "    Addr Info Count::" << _addr_info_cnt << endl;
  AddrPrefixCont * aptr;
  list_item li;
  forall_items(li, _prefixes) {
    if (aptr = _prefixes.inf(li))
      aptr->PrintSpecific(os);
  }
  return true;
}

bool ig_internal_reachable_atm_addr::ReadSpecific(istream & os)
{
  return false;
}

ostream & operator << (ostream & os, ig_internal_reachable_atm_addr & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

istream & operator >> (istream & is, ig_internal_reachable_atm_addr & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return (is);
}
