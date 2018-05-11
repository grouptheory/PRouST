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
static char const _hello_cc_rcsid_[] =
"$Id: hello.cc,v 1.24 1999/01/06 16:44:09 battou Exp battou $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_pkt/hello.h>
#include <codec/pnni_ig/aggregation_token.h>
#include <codec/pnni_ig/nodal_hierarchy_list.h>
#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/lgn_horizontal_link_ext.h>


HelloPkt::HelloPkt(u_char * n, u_char * a, u_char * p, u_char * r, int pid, int rpid, sh_int hint) :
  PNNIPkt(PNNIPkt::hello), _flags(0), _port_id(pid), _rport_id(rpid), _hello_interval(hint),
  _nod_lst(0), _agg_tok(0), _upl_nfo(0), _lgn_ext(0)
{
  if (n)
  {
    _node_id = new NodeID(n);
  }
  else
    _node_id = 0;

  if (a)
  {
     int i = 20;
     Addr * _atm_end_sys_addr = new NSAP_DCC_ICD_addr;
     _atm_end_sys_addr->decode(a, i);
     // _addr only for time being. Once Addr is coded properly
     // we can get depend on _atm_end_sys_addr. Only use of
     // _addr will be in encoding
  }
  else
    _atm_end_sys_addr = 0;
  if (p)
    _peer_group_id = new PeerID(p);
  else
    _peer_group_id = 0;
  if (r)
   _remote_node_id = new NodeID(r);
  else
   _remote_node_id = 0;
}

HelloPkt::HelloPkt(const HelloPkt & hp) :
  PNNIPkt(PNNIPkt::hello), _flags(hp._flags), _port_id(hp._port_id), 
  _rport_id(hp._rport_id), _hello_interval(hp._hello_interval),
  _nod_lst(0), _agg_tok(0), _upl_nfo(0), _lgn_ext(0)
{
  if(hp._node_id)
   _node_id = hp._node_id->copy();
  else
   _node_id = 0;
  if(hp._atm_end_sys_addr)
    _atm_end_sys_addr = hp._atm_end_sys_addr->copy();
  else
    _atm_end_sys_addr = 0;
  if(hp._peer_group_id)
   _peer_group_id = hp._peer_group_id->copy();
  else
   _peer_group_id = 0;
  if(hp._remote_node_id)
   _remote_node_id = hp._remote_node_id->copy();
  else
   _remote_node_id = 0;

  if (hp._nod_lst)
    _nod_lst = (ig_nodal_hierarchy_list *)hp._nod_lst->copy();
  else
    _nod_lst = 0;
  if (hp._agg_tok)
    _agg_tok = (ig_aggregation_token *)hp._agg_tok->copy();
  else
    _agg_tok = 0;
  if (hp._upl_nfo)
    _upl_nfo = (ig_uplink_info_attr *)hp._upl_nfo->copy();
  else
    _upl_nfo = 0;
  if (hp._lgn_ext)
    _lgn_ext = (ig_lgn_horizontal_link_ext *)hp._lgn_ext->copy();
  else
   _lgn_ext = 0;
}

HelloPkt::HelloPkt(const NodeID * nid, const NodeID * rnid,
                   int pid, int rpid, sh_int hint):
   PNNIPkt(PNNIPkt::hello), _flags(0), _port_id(pid), _rport_id(rpid),
   _hello_interval(hint), _nod_lst(0), _agg_tok(0), _upl_nfo(0), _lgn_ext(0)
{
  if (nid)
  {
    _node_id          = nid->copy();
    _peer_group_id    = nid->GetPeerGroup();
    _atm_end_sys_addr = nid->GetAddr();
  }
  else
  {
    _node_id          = 0;
    _peer_group_id    = 0;
    _atm_end_sys_addr = 0;
  }

  if (rnid)
     _remote_node_id = rnid->copy();
  else
    _remote_node_id = 0;
}

HelloPkt::~HelloPkt() 
{
  if(_node_id) delete _node_id;
  if(_peer_group_id) delete _peer_group_id;
  if(_atm_end_sys_addr) delete _atm_end_sys_addr;
  if(_remote_node_id) delete _remote_node_id;
  if (_agg_tok) delete _agg_tok;
  if (_nod_lst) delete _nod_lst;
  if (_upl_nfo) delete _upl_nfo;
  if (_lgn_ext) delete _lgn_ext;
}

PNNIPkt * HelloPkt::copy(void) const
{
  HelloPkt * rval = new HelloPkt(*this);
  return rval;
}

void HelloPkt::size(int & length)
{
  length = FIXED_HELLO_HEADER; // default pkt header + hello specific with
            // no other optional elements
  if(_agg_tok){  _agg_tok->size(length); }
  if(_nod_lst){  _nod_lst->size(length); }
  if(_upl_nfo){  _upl_nfo->size(length); }
  if(_lgn_ext){  _lgn_ext->size(length); }
}

u_char * HelloPkt::encode(u_char * buffer, int & buflen)
{
  int i;

  buflen = 0;
  u_char * temp = buffer + pkt_header_len;

  *temp++ = (_flags >> 8) & 0xFF;
  *temp++ = (_flags & 0xFF);
  buflen += 2;

  u_char * _nid = (u_char *)_node_id->GetNID();
  for (i = 0; i < 22; i++)
    *temp++ = _nid[i];
  buflen += 22;

  u_char * _addr = (u_char *)_node_id->GetATM();
  for (i = 0; i < 20; i++)
    *temp++ = _addr[i];
  buflen += 20;

  u_char * _pgid = (u_char *)_peer_group_id->GetPID();
  for (i = 0; i < 14; i++)
    *temp++ = _pgid[i];
  buflen += 14;

  if(_remote_node_id)
  {
    u_char * _rnid = (u_char *)_remote_node_id->GetNID();
    for (i = 0; i < 22; i++)
      *temp++ = _rnid[i];
  }
  else
  {
    for (i = 0; i < 22; i++)
     *temp++ = 0;
  }
  buflen += 22;

  *temp++ = (_port_id >> 24) & 0xFF;
  *temp++ = (_port_id >> 16) & 0xFF;
  *temp++ = (_port_id >>  8) & 0xFF;
  *temp++ = (_port_id & 0xFF);

  *temp++ = (_rport_id >> 24) & 0xFF;
  *temp++ = (_rport_id >> 16) & 0xFF;
  *temp++ = (_rport_id >>  8) & 0xFF;
  *temp++ = (_rport_id & 0xFF);

  *temp++ = (_hello_interval >> 8) & 0xFF;
  *temp++ = (_hello_interval & 0xFF);
  // Reserved
  *temp++ = 0;
  *temp++ = 0;

  buflen += 12;
  int len = 0;
  if (_agg_tok)
    temp = _agg_tok->encode(temp, len);
  buflen += len;
  len = 0;
  if (_nod_lst)
    temp = _nod_lst->encode(temp, len);
  buflen += len;
  len = 0;
  if (_upl_nfo)
    temp = _upl_nfo->encode(temp, len);
  buflen += len;
  len = 0;
  if (_lgn_ext)
    temp = _lgn_ext->encode(temp, len);
  buflen += len;
  encode_pkt_header(buffer, buflen);
  return temp;
}


errmsg * HelloPkt::decode(const u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF)), i;
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != PNNIPkt::hello || len < 96)
    {
      return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);
    }
  decode_pkt_header(buffer);
  len -= 8;

  u_char * temp = (u_char *)buffer + pkt_header_len;

  _flags  = (*temp++ << 8) & 0xFF00;
  _flags |= (*temp++ & 0xFF);
  len -= 2;

  u_char _nid[22];
  for (i = 0; i < 22; i++)
    _nid[i] = *temp++;
  len -= 22;
  _node_id = new NodeID(_nid);

  u_char _addr[20];
  for (i = 0; i < 20; i++)
    _addr[i] = *temp++;
  len -= 20;
  _atm_end_sys_addr = new NSAP_DCC_ICD_addr;
  _atm_end_sys_addr->decode(_addr, i);

  u_char _pg[14];
  for (i = 0; i < 14; i++)
    _pg[i] = *temp++;
  _peer_group_id = new PeerID(_pg);
  len -= 14;

  u_char _rnid[22];
  for (i = 0; i < 22; i++)
    _rnid[i] = *temp++;

  u_char tmp[22];
  bzero((void *)tmp, 22);
  if((memcmp((void *)tmp, (void *)_rnid, 22) == 0))
     _remote_node_id = 0;
  else
     _remote_node_id = new NodeID(_rnid);


  len -= 22;

  u_long portid;
  portid  = (*temp++ << 24) & 0xFF000000;
  portid |= (*temp++ << 16) & 0x00FF0000;
  portid |= (*temp++ <<  8) & 0x0000FF00;
  portid |= (*temp++ & 0xFF);
  _port_id = portid;
  len -= 4;

 u_long rportid;
  rportid  = (*temp++ << 24) & 0xFF000000;
  rportid |= (*temp++ << 16) & 0x00FF0000;
  rportid |= (*temp++ <<  8) & 0x0000FF00;
  rportid |= (*temp++ & 0xFF);
 _rport_id = rportid;
  len -= 4;

  _hello_interval  = (*temp++ <<  8) & 0xFF00;
  _hello_interval |= (*temp++ & 0xFF);
  len -= 2;

  // reserved
  sh_int reserved  = (*temp++ <<  8) & 0xFF00;
         reserved |= (*temp++ & 0xFF);
  len -= 2;

  if (len < 0) // len should be >= 0 at this point
    {
      return new errmsg(errmsg::invalid_contents, errmsg::empty_ig, type);
    }

  errmsg * rval = 0;
  while (len > 0) {
    
    int type = (((temp[0] << 8) & 0xFF00) | (temp[1] & 0xFF));
    int tlen = (((temp[2] << 8) & 0xFF00) | (temp[3] & 0xFF));
    
    switch (type)
      {
      case InfoGroup::ig_aggregation_token_id:
        // Allocate the ig
        _agg_tok =  new ig_aggregation_token;
        rval = _agg_tok->decode(temp);
        break;
      case InfoGroup::ig_nodal_hierarchy_list_id:
        _nod_lst = new ig_nodal_hierarchy_list;
        rval = _nod_lst->decode(temp);
        break;
      case InfoGroup::ig_uplink_info_attr_id:
        _upl_nfo = new ig_uplink_info_attr;
        rval = _upl_nfo->decode(temp);
        break;
      case InfoGroup::ig_lgn_horizontal_link_ext_id:
        _lgn_ext  = new ig_lgn_horizontal_link_ext;
        rval = _lgn_ext->decode(temp);
        break;

    default:
      return new errmsg(errmsg::invalid_contents, errmsg::empty_ig, type);
    }
    if (rval) return rval;

    len  -= tlen + 4;
    temp += tlen + 4;
  }
  return rval;
}

void HelloPkt::SetNodeID(u_char *n)
{
  if (n)
   _node_id = new NodeID(n);
}

void HelloPkt::SetATMEndSysAddr(u_char *a)
{
  if (a)
  {
    int i = 20;
    _atm_end_sys_addr = new NSAP_DCC_ICD_addr;
    _atm_end_sys_addr->decode(a, i);
  }
}

void HelloPkt::SetPeerGroupID(u_char *p)
{
  if (p)
    _peer_group_id = new PeerID(p);
}

void HelloPkt::SetRemoteNodeID(u_char *r)
{
  if (r)
   _remote_node_id = new NodeID(r);
}

void HelloPkt::SetAggregationToken(ig_aggregation_token * a) 
{
   delete _agg_tok;
   _agg_tok = new ig_aggregation_token(*(a));
}

void HelloPkt::SetNodalHierarchyList(ig_nodal_hierarchy_list * n) 
{ 
  delete _nod_lst;
  _nod_lst = new ig_nodal_hierarchy_list(*(n)); 
}

void HelloPkt::SetUplinkInfoAttr(ig_uplink_info_attr * u)
{ 
  delete _upl_nfo;
  _upl_nfo = new ig_uplink_info_attr(*(u)); 
}

void HelloPkt::SetLGNHorizontalLinkExt(const ig_lgn_horizontal_link_ext * l) 
{ 
  delete _lgn_ext;
_lgn_ext = (ig_lgn_horizontal_link_ext *)((ig_lgn_horizontal_link_ext *)l)->copy();
}

const NodeID * HelloPkt::GetNodeID(void) const
{ return _node_id;}

const Addr * HelloPkt::GetATMEndSysAddr(void) const
{
  return _atm_end_sys_addr;
}
    
const PeerGroupID * HelloPkt::GetPeerGroupID(void) const
{
  return _peer_group_id;
}

const NodeID * HelloPkt::GetRemoteNodeID(void) const
{
  return _remote_node_id;
}

void    HelloPkt::SetPortID(int p) 
{ _port_id = p; }

int     HelloPkt::GetPortID(void) const 
{ return _port_id; }

void    HelloPkt::SetRemotePortID(int p) 
{ _rport_id = p; }

int     HelloPkt::GetRemotePortID(void) const 
{ return _rport_id; }

void    HelloPkt::SetHelloInterval(sh_int i) 
{ _hello_interval = i; }

int     HelloPkt::GetHelloInterval(void) const 
{ return _hello_interval; }

const ig_aggregation_token * HelloPkt::GetAggregationToken(void) const 
{ return _agg_tok; }

const ig_nodal_hierarchy_list * HelloPkt::GetNodalHierarchyList(void) const 
{ return _nod_lst; }

const ig_uplink_info_attr * HelloPkt::GetUplinkInfoAttr(void) const 
{ return _upl_nfo; }

const ig_lgn_horizontal_link_ext * HelloPkt::GetLGNHorizontalLinkExt(void) const 
{ return _lgn_ext; }

bool HelloPkt::PrintSpecific(ostream & os) const
{
  os << "     Flags: " << _flags << endl;
  os << "     Node ID:  ";
  os << *_node_id  << endl;
  os << "     System Address: ";
  os << *_atm_end_sys_addr << endl;
  os << "     Peer Group ID:  ";
  os << *_peer_group_id << endl;
  os << "     Remote Node ID: ";
  if(_remote_node_id)
  os << *_remote_node_id << endl;
  os << "     Port ID:        " << _port_id << endl;
  os << "     Remote Port ID: " << _rport_id << endl;
  os << "     Hello Interval: " << _hello_interval << " secs" << endl;

  if (_agg_tok)
    os << *(_agg_tok);
  if (_nod_lst)
    os << *(_nod_lst);
  if (_upl_nfo)
    os << *(_upl_nfo);
  if (_lgn_ext)
    os << *(_lgn_ext);
  return true;
}

ostream & operator << (ostream & os, const HelloPkt & pkt)
{
  pkt.PrintGeneral(os);
  pkt.PrintSpecific(os);
  return os;
}


void PrintSysAddress(const u_char * buffer, int len, ostream & os)
{
  const u_char * buf = buffer;

  // Print it like this.
  // 47.0091810000000061705A8301.0061705A8301.00
  if (buf) {
    for (int i = 0; i < len; i++) {
      int val = (int)(buf[i]);
      if (val < 16) 
        os << "0";
      os << hex << val;
      if ((i == 0) || (i == 12) || (i == 18)) os << ".";
    }
  } else 
    os << "Invalid message" << endl;
  os << dec << endl;
}

void PrintNodeID(const u_char * buffer, int len, ostream & os)
{
  const u_char * buf = buffer;

  // Print it like this.
  // 56:160:47.0091810000000061705A8301.0061705A8301.00
  if (buf) {
    for (int i = 0; i < len; i++) {
      int val = (int)(buf[i]);
      if (val < 16) 
        os << "0";
      os << hex << val;
      if ((i == 0) || (i == 1)) os << ":";
      else if ((i == 2) || (i == 14) || (i == 20)) os << ".";
    }
  } else 
    os << "Invalid message" << endl;
  os << dec << endl;
}

void PrintPeerGroupID(const u_char * buffer, int len, ostream & os)
{
  const u_char * buf = buffer;

  // Print it like this.
  // 56:47.0091.8100.0000.0000.0000.0000
  if (buf) {
    for (int i = 0; i < len; i++) {
      int val = (int)(buf[i]);
      if (val < 16) 
        os << "0";
      os << hex << val;
      if (i == 0) os << ":";
      else if ((i  ) && i != len-1) os << ".";
    }
  } else 
    os << "Invalid message" << endl;
  os << dec << endl;
}
