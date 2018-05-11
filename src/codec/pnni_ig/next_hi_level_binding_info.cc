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
static char const _nxt_hilvl_bind_info_cc_rcsid_[] =
"$Id: next_hi_level_binding_info.cc,v 1.8 1998/09/23 18:59:09 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/next_hi_level_binding_info.h>
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/addr.h>

void HexDump(const u_char * buffer, int len, ostream & os, bool nid = false);

ig_next_hi_level_binding_info::ig_next_hi_level_binding_info(u_char * nid, 
							     char * pgl) :
  InfoGroup(InfoGroup::ig_next_hi_level_binding_info_id)
{
  if (nid) {
    // parent node ID
    bcopy(nid, _parent_log_group_id, 22);
    // extract parent ATM address from parent NID
    bcopy(nid+2, _parent_lgn_atm_esa, 20);
    // extract parent PGID from parent NID
    bzero(_parent_peer_group_id, 14);
    int level = nid[0]/8;
    _parent_peer_group_id[0] = level;
    bcopy(nid+2, _parent_peer_group_id, level);
    // load the parent PGL node ID
    if (pgl)
      bcopy(pgl, _node_id_pgl_parent_pgroup, 22);
    else
      bzero(_node_id_pgl_parent_pgroup, 22);
  } else {
    bzero(_parent_log_group_id, 22);
    bzero(_parent_lgn_atm_esa, 20);
    bzero(_parent_peer_group_id, 14);
    // load the parent PGL node ID
    if (pgl)
      bcopy(pgl, _node_id_pgl_parent_pgroup, 22);
    else
      bzero(_node_id_pgl_parent_pgroup, 22);
  }
}


ig_next_hi_level_binding_info::
ig_next_hi_level_binding_info(u_char *pgid, u_char *pesa, u_char *ppgid, 
			      u_char *nid) :
  InfoGroup(InfoGroup::ig_next_hi_level_binding_info_id)
{
  if (pgid)
    bcopy(pgid, _parent_log_group_id, 22); // parent node ID 
  else
    bzero(_parent_log_group_id, 22);
  if (pesa)
    bcopy(pesa, _parent_lgn_atm_esa, 20);
  else
    bzero(_parent_lgn_atm_esa, 20);
  if (ppgid)
    bcopy(ppgid, _parent_peer_group_id, 14);
  else
    bzero(_parent_peer_group_id, 14);
  if (nid)
    bcopy(nid, _node_id_pgl_parent_pgroup, 22);
  else
    bzero(_node_id_pgl_parent_pgroup, 22);
}

ig_next_hi_level_binding_info::
ig_next_hi_level_binding_info(NodeID * pgid, Addr * pesa, PeerID * ppgid,
			      NodeID * nid) :
  InfoGroup(InfoGroup::ig_next_hi_level_binding_info_id)
{
  SetParentLogGroupID(pgid);
  SetParentLGNATMAddr(pesa);
  SetParentPeerGroupID(ppgid);
  SetNodeIDPGLParentPG(nid);
}

ig_next_hi_level_binding_info::ig_next_hi_level_binding_info(const ig_next_hi_level_binding_info & rhs)
  : InfoGroup(InfoGroup::ig_next_hi_level_binding_info_id)
{
  bcopy(rhs._parent_log_group_id,       _parent_log_group_id, 22);
  bcopy(rhs._parent_lgn_atm_esa,        _parent_lgn_atm_esa, 20);
  bcopy(rhs._parent_peer_group_id,      _parent_peer_group_id, 14);
  bcopy(rhs._node_id_pgl_parent_pgroup, _node_id_pgl_parent_pgroup, 22);
}


ig_next_hi_level_binding_info::~ig_next_hi_level_binding_info() { }

InfoGroup * ig_next_hi_level_binding_info::copy(void)
{
  return new ig_next_hi_level_binding_info(*this);
}

void ig_next_hi_level_binding_info::size(int & length)
{
  length += BINDING_INFO_HEADER; // default 84 bytes
}

u_char * ig_next_hi_level_binding_info::encode(u_char * buffer, int & buflen)
{
  int i;

  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  for (i = 0; i < 22; i++)
    *temp++ = _parent_log_group_id[i];
  buflen += 22;

  for (i = 0; i < 20; i++)
    *temp++ = _parent_lgn_atm_esa[i];
  buflen += 20;

  for (i = 0; i < 14; i++)
    *temp++ = _parent_peer_group_id[i];
  buflen += 14;

  for (i = 0; i < 22; i++)
    *temp++ = _node_id_pgl_parent_pgroup[i];
  buflen += 22;

  // 2 reserved bytes
  *temp++ = 0;
  *temp++ = 0;
  buflen += 2;

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_next_hi_level_binding_info::decode(u_char * buffer)
{
  errmsg * rval = 0;

  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF)), i;
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_next_hi_level_binding_info_id ||
      len < 80)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);

  u_char * temp = &buffer[4];

  for (i = 0; i < 22; i++)
    _parent_log_group_id[i] = *temp++;
  for (i = 0; i < 20; i++)
    _parent_lgn_atm_esa[i] = *temp++;
  for (i = 0; i < 14; i++)
    _parent_peer_group_id[i] = *temp++;
  for (i = 0; i < 22; i++)
    _node_id_pgl_parent_pgroup[i] = *temp++;
  // Two more bytes, reserved are here. need to decode
  // when they are used.

  return rval;
}

const NodeID * ig_next_hi_level_binding_info::GetParentLogGroupID(void) const 
{ return new NodeID(_parent_log_group_id); }

void ig_next_hi_level_binding_info::SetParentLogGroupID(NodeID *parent)
{
  if (parent)
    bcopy((void *)parent->GetNID(), &_parent_log_group_id[0], 22);
  else
    bzero(&_parent_log_group_id[0], 22);
}

const Addr   * ig_next_hi_level_binding_info::GetParentLGNATMAddr(void) const 
{ 
  int i = 20; Addr * rval = new NSAP_DCC_ICD_addr; 
  rval->decode((u_char *)_parent_lgn_atm_esa, i); 
  return rval; 
}

void ig_next_hi_level_binding_info::SetParentLGNATMAddr(Addr *parent)
{ 
  if (parent) 
    parent->encode(&_parent_lgn_atm_esa[0]);
  else
    bzero(&_parent_lgn_atm_esa[0], 20);
}

const PeerID * ig_next_hi_level_binding_info::GetParentPeerGroupID(void) const 
{ return new PeerID(_parent_peer_group_id); }

void ig_next_hi_level_binding_info::SetParentPeerGroupID(PeerID *parent)
{
  if (parent)
    bcopy((void *)parent->GetPID(), &_parent_peer_group_id[0], 14);
  else
    bzero(&_parent_peer_group_id[0], 14);
}

const NodeID * ig_next_hi_level_binding_info::GetNodeIDPGLParentPG(void) const 
{ return new NodeID(_node_id_pgl_parent_pgroup); }

void ig_next_hi_level_binding_info::SetNodeIDPGLParentPG(NodeID *parent)
{
  if (parent)
    bcopy((void *)parent->GetNID(), _node_id_pgl_parent_pgroup, 22);
  else
    bzero(_node_id_pgl_parent_pgroup, 22);
}

bool ig_next_hi_level_binding_info::PrintSpecific(ostream & os)
{
  os << "     Parent Log Group ID::";
  HexDump(_parent_log_group_id, 22, os, true);
  os << "     Parent LGN ATM ESA::";
  HexDump(_parent_lgn_atm_esa, 20, os);
  os << "     Parent Peer Group ID::" << (int)_parent_peer_group_id[0] << ":";
  HexDump(_parent_peer_group_id+1, 13, os);
  os << "     Node ID PGL Parent Peer Group::";
  HexDump(_node_id_pgl_parent_pgroup, 22, os, true);
  os << endl;
  return true;
}

bool ig_next_hi_level_binding_info::ReadSpecific(istream & os)
{
  // Incomplete
  return false;
}

ostream & operator << (ostream & os, ig_next_hi_level_binding_info & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return os;
}

istream & operator >> (istream & is, ig_next_hi_level_binding_info & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return is;
}

