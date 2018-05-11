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
static char const _uplinks_cc_rcsid_[] =
"$Id: uplinks.cc,v 1.19 1998/09/24 13:39:03 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/addr.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/uplink_info_attr.h>

ig_uplinks::ig_uplinks(sh_int flags, u_char *rnid, u_char *cpid, 
		       int lport, int atok, u_char *aesa) :
  InfoGroup(InfoGroup::ig_uplinks_id), _flags(flags), _local_port_id(lport),
  _aggregation_token(atok)
{
  if (rnid)
    bcopy(rnid, _remote_higher_level_node_id, 22);
  else
    bzero(_remote_higher_level_node_id, 22);
  if (cpid)
    bcopy(cpid, _common_peer_group_id, 14);
  else
    bzero(_common_peer_group_id, 14);
  if (aesa)
    bcopy(aesa, _atm_end_sys_address_upnode, 20);
  else
    bzero(_atm_end_sys_address_upnode, 20);
}

ig_uplinks::ig_uplinks(sh_int flags, NodeID *rnid, PeerID *cpid, 
		       int lport, int atok, Addr *aesa) :
  InfoGroup(InfoGroup::ig_uplinks_id), _flags(flags), _local_port_id(lport),
  _aggregation_token(atok)
{
  if (rnid)
    bcopy((void *)rnid->GetNID(), _remote_higher_level_node_id, 22);
  else
    bzero(_remote_higher_level_node_id, 22);
  if (cpid)
    bcopy((u_char *)cpid->GetPID(), _common_peer_group_id, 14);
  else
    bzero(_common_peer_group_id, 14);
  if (aesa)
    aesa->encode(_atm_end_sys_address_upnode);
  else
    bzero(_atm_end_sys_address_upnode, 20);
}

ig_uplinks::ig_uplinks(const ig_uplinks & rhs) 
  : InfoGroup(InfoGroup::ig_uplinks_id), _flags(rhs._flags),
    _local_port_id(rhs._local_port_id), 
    _aggregation_token(rhs._aggregation_token)
{
  bcopy(rhs._remote_higher_level_node_id, _remote_higher_level_node_id, 22);
  bcopy(rhs._common_peer_group_id,        _common_peer_group_id,        14);
  bcopy(rhs._atm_end_sys_address_upnode,  _atm_end_sys_address_upnode,  20);

  if (rhs._list.empty() == false) {
    list_item li;
    forall_items(li, rhs._list) {
      AddIG( rhs._list.inf(li)->copy() );
    }
  }
}

ig_uplinks::~ig_uplinks() { }

InfoGroup * ig_uplinks::copy(void)
{
  return new ig_uplinks(*this);
}

void ig_uplinks::size(int & length)
{
  list_item li;
  length += FIXED_UPLINKS_CONTENT; // w/o optional elements 72
  forall_items(li, _list)
  {
    (_list.inf(li))->size(length);
  }
}

int ig_uplinks::equals(const InfoGroup * other) const
{
  int rval = InfoGroup::equals(other);
  if (!rval) {
    ig_uplinks * rhs = (ig_uplinks *)other;
    if ((_flags < rhs->_flags) ||
	((_flags == rhs->_flags) && 
	 (memcmp(_remote_higher_level_node_id, rhs->_remote_higher_level_node_id, 22) < 0)) ||
	((_flags == rhs->_flags) && 
	 !memcmp(_remote_higher_level_node_id, rhs->_remote_higher_level_node_id, 22) &&
	 (memcmp(_common_peer_group_id, rhs->_common_peer_group_id, 14) < 0)) ||
	((_flags == rhs->_flags) && 
	 !memcmp(_remote_higher_level_node_id, rhs->_remote_higher_level_node_id, 22) &&
	 !memcmp(_common_peer_group_id, rhs->_common_peer_group_id, 14) &&
	 (_local_port_id < rhs->_local_port_id)) ||
	((_flags == rhs->_flags) && 
	 !memcmp(_remote_higher_level_node_id, rhs->_remote_higher_level_node_id, 22) &&
	 !memcmp(_common_peer_group_id, rhs->_common_peer_group_id, 14) &&
	 (_local_port_id == rhs->_local_port_id) && (_aggregation_token < rhs->_aggregation_token)) ||
	((_flags == rhs->_flags) && 
	 !memcmp(_remote_higher_level_node_id, rhs->_remote_higher_level_node_id, 22) &&
	 !memcmp(_common_peer_group_id, rhs->_common_peer_group_id, 14) &&
	 (_local_port_id == rhs->_local_port_id) && (_aggregation_token == rhs->_aggregation_token) &&
	 (memcmp(_atm_end_sys_address_upnode, rhs->_atm_end_sys_address_upnode, 20) < 0)))
      return -1;
    else if ((_flags > rhs->_flags) ||
	((_flags == rhs->_flags) && 
	 (memcmp(_remote_higher_level_node_id, rhs->_remote_higher_level_node_id, 22) > 0)) ||
	((_flags == rhs->_flags) && 
	 !memcmp(_remote_higher_level_node_id, rhs->_remote_higher_level_node_id, 22) &&
	 (memcmp(_common_peer_group_id, rhs->_common_peer_group_id, 14) > 0)) ||
	((_flags == rhs->_flags) && 
	 !memcmp(_remote_higher_level_node_id, rhs->_remote_higher_level_node_id, 22) &&
	 !memcmp(_common_peer_group_id, rhs->_common_peer_group_id, 14) &&
	 (_local_port_id > rhs->_local_port_id)) ||
	((_flags == rhs->_flags) && 
	 !memcmp(_remote_higher_level_node_id, rhs->_remote_higher_level_node_id, 22) &&
	 !memcmp(_common_peer_group_id, rhs->_common_peer_group_id, 14) &&
	 (_local_port_id == rhs->_local_port_id) && (_aggregation_token > rhs->_aggregation_token)) ||
	((_flags == rhs->_flags) && 
	 !memcmp(_remote_higher_level_node_id, rhs->_remote_higher_level_node_id, 22) &&
	 !memcmp(_common_peer_group_id, rhs->_common_peer_group_id, 14) &&
	 (_local_port_id == rhs->_local_port_id) && (_aggregation_token == rhs->_aggregation_token) &&
	 (memcmp(_atm_end_sys_address_upnode, rhs->_atm_end_sys_address_upnode, 20) > 0)))
      return 1;
  }
  return rval;
}

u_char * ig_uplinks::encode(u_char * buffer, int & buflen)
{
  int i;

  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  *temp++ = (_flags >> 8) & 0xFF;
  *temp++ = (_flags & 0xFF);

  // Reserved
  *temp++ = 0;
  *temp++ = 0;

  buflen += 4;

  for (i = 0; i < 22; i++)
    *temp++ = _remote_higher_level_node_id[i];
  buflen += 22;

  for (i = 0; i < 14; i++)
    *temp++ = _common_peer_group_id[i];
  buflen += 14;

  *temp++ = (_local_port_id >> 24) & 0xFF;
  *temp++ = (_local_port_id >> 16) & 0xFF;
  *temp++ = (_local_port_id >>  8) & 0xFF;
  *temp++ = (_local_port_id & 0xFF);

  *temp++ = (_aggregation_token >> 24) & 0xFF;
  *temp++ = (_aggregation_token >> 16) & 0xFF;
  *temp++ = (_aggregation_token >>  8) & 0xFF;
  *temp++ = (_aggregation_token & 0xFF);

  buflen += 8;

  for (i = 0; i < 20; i++)
    *temp++ = _atm_end_sys_address_upnode[i];
  buflen += 20;

  list_item li;
  forall_items(li, _list)
  {
    InfoGroup * ptr = _list.inf(li);
    int tlen= 0;
    temp = ptr->encode(temp, tlen);
    buflen += tlen;
  }

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_uplinks::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF)), i;
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_uplinks_id ||
      len < 68)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);
  u_char * temp = &buffer[4];

  _flags  = (*temp++ << 8) & 0xFF00;
  _flags |= (*temp++ & 0xFF);

  temp += 2; // Reserved

  for (i = 0; i < 22; i++)
    _remote_higher_level_node_id[i] = *temp++;

  for (i = 0; i < 14; i++)
    _common_peer_group_id[i] = *temp++;

  _local_port_id  = (*temp++ << 24) & 0xFF000000;
  _local_port_id |= (*temp++ << 16) & 0x00FF0000;
  _local_port_id |= (*temp++ <<  8) & 0x0000FF00;
  _local_port_id |= (*temp++ & 0xFF);

  _aggregation_token  = (*temp++ << 24) & 0xFF000000;
  _aggregation_token |= (*temp++ << 16) & 0x00FF0000;
  _aggregation_token |= (*temp++ <<  8) & 0x0000FF00;
  _aggregation_token |= (*temp++ & 0xFF);

  for (i = 0; i < 20; i++)
    _atm_end_sys_address_upnode[i] = *temp++;

  len -= 68;

  errmsg * rval = 0;

  while (len > 0) {
    int type = (((temp[0] << 8 ) & 0xFF00) | ( temp[1] & 0xFF ));
    int tlen = (((temp[2] << 8 ) & 0xFF00) | (temp[3] & 0xFF));

    if (type == (InfoGroup::ig_outgoing_resource_avail_id)) {
      InfoGroup * ig = (InfoGroup *)(new ig_resrc_avail_info(ig_resrc_avail_info::outgoing));
      rval = ig->decode(temp);
      _list.append(ig);
    } else if (type == (InfoGroup::ig_uplink_info_attr_id)) {
      InfoGroup * ig = (InfoGroup *)(new ig_uplink_info_attr());
      rval = ig->decode(temp);
      _list.append(ig);
    } else {
      return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);
    }

    if (rval) return rval;
    
    len  -= tlen + 4;
    temp += tlen + 4;
  }
  
  return rval;
}

void ig_uplinks::AddIG(InfoGroup * ptr) 
{ _list.append(ptr); }

const list<InfoGroup *> & ig_uplinks::GetIGs(void) const 
{ return _list; }

const list<InfoGroup *> * ig_uplinks::ShareIGs(void) const 
{ return &_list; }

NodeID * ig_uplinks::GetRemoteID(void) const 
{ 
  if (_remote_higher_level_node_id)
    return new NodeID(_remote_higher_level_node_id); 
  else return 0;
}

PeerID * ig_uplinks::GetCommonPGID(void) const 
{ 
  if (_common_peer_group_id)
    return new PeerID(_common_peer_group_id); 
  else return 0; 
}

const int ig_uplinks::GetLocalPID(void) const 
{ return _local_port_id; }

const int ig_uplinks::GetAggTok(void) const 
{ return _aggregation_token; }

Addr   * ig_uplinks::GetATMAddr(void) const 
{ 
  int i = 20; Addr * rval = new NSAP_DCC_ICD_addr; 
  rval->decode((u_char *)_atm_end_sys_address_upnode, i); 
  return rval; 
}
  
bool ig_uplinks::PrintSpecific(ostream & os)
{
  os << "    Flags::" << _flags << endl;
  os << "    Remote Higher Level Node ID::";
  NodeID rnid(_remote_higher_level_node_id);
  os << rnid << endl;

  os << "    Common Peer Group ID::";
  PeerID cpid(_common_peer_group_id);
  os << cpid << endl;

  os << "    ATM End System Address of Upnode::";
  char tmp[50]; tmp[0] = '\0';
  for (int i = 0; i < 20; i++)
    sprintf(tmp, "%s%02x", tmp, (int)_atm_end_sys_address_upnode[i]);
  os << tmp << endl;

  os << "    Local Port ID::" << _local_port_id << endl;
  os << "    Aggregation Token::" << _aggregation_token << endl;
  return true;
}

// This should be able to read in the output of PrintSpecific
// so make sure the variables are read in the same order they are
// printed.
bool ig_uplinks::ReadSpecific(istream & is)
{
  char buf[80], *temp = buf;

  is.getline(buf, 80);
  while (*temp && !isdigit(*temp))
    temp++;
  _flags = atoi(temp);

  is.getline(buf, 80);
  // How to extract the remote_higher_level_node_id?
  is.getline(buf, 80);
  // How to extract the common_peer_group_id?
  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _local_port_id = atoi(temp);

  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _aggregation_token = atoi(temp);

  is.getline(buf, 80);
  // how to extract the atm_end_sys_address_upnode?
  return true;
}

ostream & operator << (ostream & os, ig_uplinks & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

istream & operator >> (istream & is, ig_uplinks & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return (is);
}
