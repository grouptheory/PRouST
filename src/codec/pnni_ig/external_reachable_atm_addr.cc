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
static char const _ext_reach_atm_addr_cc_rcsid_[] =
"$Id: external_reachable_atm_addr.cc,v 1.11 1998/09/24 13:36:51 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/external_reachable_atm_addr.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/transit_net_id.h>

#include <math.h>	// fmod()

ig_external_reachable_atm_addr::ig_external_reachable_atm_addr(int pid, 
	   u_char ad_scope, u_char addrnfolen) :
  InfoGroup(InfoGroup::ig_internal_reachable_atm_addr_id), _flags(0), _pid(pid),
  _ad_scope(ad_scope), _addr_info_len(addrnfolen), _addr_info_cnt(0), _padding(0){ }

ig_external_reachable_atm_addr::ig_external_reachable_atm_addr(const ig_external_reachable_atm_addr & rhs) 
  : InfoGroup(InfoGroup::ig_internal_reachable_atm_addr_id), _flags(rhs._flags), 
    _pid(rhs._pid), _ad_scope(rhs._ad_scope), _addr_info_len(rhs._addr_info_len), 
    _addr_info_cnt(rhs._addr_info_cnt), _padding(rhs._padding) { }

ig_external_reachable_atm_addr::~ig_external_reachable_atm_addr()
{
  AddrPrefixCont * aptr;
  ig_resrc_avail_info *raig_ptr;
  ig_transit_net_id *trans_ptr;

  list_item li;
  forall_items(li, _prefixes) {
    aptr = _prefixes.inf(li);
    delete aptr;
  }

  forall_items(li, _incoming_raigs) {
    if (raig_ptr = _incoming_raigs.inf(li))
      delete raig_ptr;
  }

  forall_items(li, _outgoing_raigs) {
    if (raig_ptr = _outgoing_raigs.inf(li))
      delete raig_ptr;
  }

  forall_items(li, _transit_net_ids) {
    if (trans_ptr = _transit_net_ids.inf(li))
      delete trans_ptr;
  }

}

InfoGroup * ig_external_reachable_atm_addr::copy(void)
{
  ig_external_reachable_atm_addr * rval = 
    new ig_external_reachable_atm_addr(_pid, _ad_scope, _addr_info_len);

  rval->_flags = _flags;
  rval->_addr_info_cnt = _addr_info_cnt;
  rval->_padding = _padding;

  list_item li;
  forall_items(li, _prefixes) {
    AddrPrefixCont * aptr = _prefixes.inf(li);
    rval->AddPrefix(aptr->_prefix_len, aptr->_reachable_addr_prefix);
  }

  li = 0;
  forall_items(li, _outgoing_raigs)
  {
    ig_resrc_avail_info * res = _outgoing_raigs.inf(li);
    rval->AddOutGoingRAIG((ig_resrc_avail_info *)res->copy());
  }

  li = 0;
  forall_items(li, _incoming_raigs)
  {
    ig_resrc_avail_info * res = _incoming_raigs.inf(li);
    rval->AddInComingRAIG((ig_resrc_avail_info *)res->copy());
  }

  li = 0;
  forall_items(li, _transit_net_ids)
  {
   ig_transit_net_id * trans = _transit_net_ids.inf(li);
   rval->AddTransitNetID((ig_transit_net_id *)trans->copy());
  }
  return rval;
}

void ig_external_reachable_atm_addr::size(int & length)
{
  list_item li;
  length += EXT_REACH_ADDR_HEADER; // Initial 16 bytes.

  forall_items(li, _prefixes)
  {
   (_prefixes.inf(li))->size(length);
  }

  length += 4 - (int)(fmod((fmod((double)(_addr_info_len * _addr_info_cnt), 4.0)), 4.0));

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

  li = 0;
  forall_items(li, _transit_net_ids)
  {
    (_transit_net_ids.inf(li))->size(length);
  }
}

int ig_external_reachable_atm_addr::equals(const InfoGroup * other) const
{
  int rval = InfoGroup::equals(other);

  if (!rval) {
    ig_external_reachable_atm_addr * rhs = (ig_external_reachable_atm_addr *)other;
    if ((_flags < rhs->_flags) ||
	((_flags == rhs->_flags) && (_pid < rhs->_pid)) ||
	((_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope < rhs->_ad_scope)) ||
	((_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope == rhs->_ad_scope) && 
	 (_addr_info_len < rhs->_addr_info_len)) ||
	((_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope == rhs->_ad_scope) && 
	 (_addr_info_len == rhs->_addr_info_len) && (_addr_info_cnt < rhs->_addr_info_cnt)))
      rval = -1;
    else if ((_flags > rhs->_flags) ||
	((_flags == rhs->_flags) && (_pid > rhs->_pid)) ||
	((_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope > rhs->_ad_scope)) ||
	((_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope == rhs->_ad_scope) && 
	 (_addr_info_len > rhs->_addr_info_len)) ||
	((_flags == rhs->_flags) && (_pid == rhs->_pid) && (_ad_scope == rhs->_ad_scope) && 
	 (_addr_info_len == rhs->_addr_info_len) && (_addr_info_cnt > rhs->_addr_info_cnt)))
      rval = 1;
  }
  return rval;
}


u_char * ig_external_reachable_atm_addr::encode(u_char * buffer, int & buflen)
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

 _padding =
   4 - (int)(fmod((fmod((double)(_addr_info_len * _addr_info_cnt), 4.0)), 4.0));

  for(i =0 ; i < _padding ; i++)
    *temp++ = 0;

  buflen += _padding;

  li = 0;
  forall_items(li, _outgoing_raigs)
  {
    ig_resrc_avail_info * rptr = _outgoing_raigs.inf(li);
    int tlen = 0;
    temp = rptr->encode(temp, tlen);
    buflen += tlen;
  }

  li = 0;
  forall_items(li, _incoming_raigs)
  {
    ig_resrc_avail_info * rptr = _incoming_raigs.inf(li);
    int tlen = 0;
    temp = rptr->encode(temp, tlen);
    buflen += tlen;
  }

  li = 0;
  forall_items(li, _transit_net_ids)
  {
    ig_transit_net_id * tptr = _transit_net_ids.inf(li);
    int tlen = 0;
    temp = tptr->encode(temp, tlen);
    buflen += tlen;
  }

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_external_reachable_atm_addr::decode(u_char * buffer)
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
  len -= _padding;

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
    else if(type == (InfoGroup::ig_transit_net_id_id))
    {
      ig_transit_net_id * ig = new ig_transit_net_id(0, 0, 0);
      rval = ig->decode(temp);
      _transit_net_ids.append(ig);
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

void ig_external_reachable_atm_addr::AddPrefix(u_char plen, u_char *reachable)
{
  AddrPrefixCont * aptr = new AddrPrefixCont(plen, reachable, _addr_info_len);
  _prefixes.append(aptr);
  _addr_info_cnt = _prefixes.length();
}

bool ig_external_reachable_atm_addr::RemPrefix(u_char plen, u_char *reachable)
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

const list<struct AddrPrefixCont *> & ig_external_reachable_atm_addr::GetPrefixes(void) const 
{ return _prefixes; }

const int    ig_external_reachable_atm_addr::GetPID(void) const { return _pid; }
const u_char ig_external_reachable_atm_addr::GetAddrScope(void) const { return _ad_scope; }
const u_char ig_external_reachable_atm_addr::GetAddrInfoLen(void) const { return _addr_info_len; }
const sh_int ig_external_reachable_atm_addr::GetAddrInfoCnt(void) const { return _addr_info_cnt; }

const list<ig_resrc_avail_info *> & ig_external_reachable_atm_addr::GetOutGoingRAIGS(void) const
{ return _outgoing_raigs; }

const list<ig_resrc_avail_info *> & ig_external_reachable_atm_addr::GetInComingRAIGS(void) const
{ return _incoming_raigs; }

void  ig_external_reachable_atm_addr::AddOutGoingRAIG(ig_resrc_avail_info *p) { _outgoing_raigs.append(p); }

void  ig_external_reachable_atm_addr::AddInComingRAIG(ig_resrc_avail_info *p) { _incoming_raigs.append(p); }


const list<ig_transit_net_id *> & ig_external_reachable_atm_addr::GetTransitNetID(void) const 
{ return _transit_net_ids; }

void  ig_external_reachable_atm_addr::AddTransitNetID(ig_transit_net_id *p) 
{ _transit_net_ids.append(p); }

bool ig_external_reachable_atm_addr::PrintSpecific(ostream & os)
{
  os.setf(ios::hex);
  os << "    Flags::" << _flags << endl;
  os.setf(ios::dec);
  os << "    PID::" << _pid << endl;
  os << "    AD Scope::" << _ad_scope << endl;
  os << "    Addr Info Len::" << _addr_info_len << endl;
  os << "    Addr Info Count::" << _addr_info_cnt << endl;
  list_item li;
  forall_items(li, _prefixes) {
    AddrPrefixCont * aptr = _prefixes.inf(li);
    aptr->PrintSpecific(os);
  }
  return true;
}

bool ig_external_reachable_atm_addr::ReadSpecific(istream & os)
{
  return false;
}

ostream & operator << (ostream & os, ig_external_reachable_atm_addr & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

istream & operator >> (istream & is, ig_external_reachable_atm_addr & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return (is);
}

int compare(AddrPrefixCont *const & lhs, AddrPrefixCont *const & rhs)
{
  int length = ((lhs->_prefix_len/8) + (lhs->_prefix_len         ? 1 : 0));
  return (lhs->_prefix_len == rhs->_prefix_len &&
          lhs->_ail == rhs->_ail &&
          !memcmp(lhs->_reachable_addr_prefix, rhs->_reachable_addr_prefix,
                length));
}

AddrPrefixCont::AddrPrefixCont(u_char plen, u_char *reach, u_char ail) : 
        _prefix_len(plen), _ail(ail)
{ 
  _reachable_addr_prefix = new u_char [ail-1];

  int bytelen = (plen + 7) / 8;
  if (reach) {
    bcopy(reach, _reachable_addr_prefix, bytelen); 
    bzero(_reachable_addr_prefix + bytelen, (8 * (ail-1) - plen) / 8);
  } else
    bzero(_reachable_addr_prefix, ail-1);
}

AddrPrefixCont::AddrPrefixCont(const AddrPrefixCont & rhs)
  : _prefix_len(rhs._prefix_len), _pad_len(rhs._pad_len),
    _ail(rhs._ail), _reachable_addr_prefix(0)
{
  if (rhs._reachable_addr_prefix) {
    _reachable_addr_prefix = new u_char [ _ail - 1 ];
    bcopy(rhs._reachable_addr_prefix, _reachable_addr_prefix, (_prefix_len + 7) / 8);
    bzero(_reachable_addr_prefix + ((_prefix_len + 7) / 8), (8 * (_ail - 1) - _prefix_len) / 8);
  }
}

void AddrPrefixCont::size(int & length)
{
  length +=  _ail; // prefix length (1 byte + (ail -1) rechable addr length)
}


bool AddrPrefixCont::PrintSpecific(ostream & os)
{
  os << "     Reachable Addr Prefix::";
  for (int i = 0; i < _prefix_len; i++)
    os << _reachable_addr_prefix[i];
  os << endl;
  return true;
}

u_char * AddrPrefixCont::encode(u_char * buffer, int & buflen)
{
  int i;

  buflen = 0;

  *buffer++ = _prefix_len;

  for (i = 0; i < (_ail-1); i++)
    *buffer++ = _reachable_addr_prefix[i];

  buflen += _ail;

  return buffer;
}

int AddrPrefixCont::equals(AddrPrefixCont & rhs)
{
  return (_prefix_len == rhs._prefix_len &&
	  !memcmp(_reachable_addr_prefix, rhs._reachable_addr_prefix, _ail-1) &&
	  _pad_len == rhs._pad_len && _ail == rhs._ail);
}
