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
static char const _nodal_info_cc_rcsid_[] =
"$Id: nodal_info_group.cc,v 1.19 1998/09/23 19:00:56 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/next_hi_level_binding_info.h>
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/addr.h>

void HexDump(const u_char * buffer, int len, ostream & os, bool nid = false);

ig_nodal_info_group::ig_nodal_info_group(u_char *atm_addy, u_char prior, 
					 u_char flags, u_char *pref_pgl, 
					 ig_next_hi_level_binding_info * 
					 higher) :
  InfoGroup(InfoGroup::ig_nodal_info_group_id), 
  _leadership_priority(prior), _nodal_flags(flags),
  _next_higher_lvl(higher)
{ 
  if (atm_addy)
    bcopy(atm_addy, _atm_address, 20);
  else
    bzero((char *)_atm_address, 20);
  if (pref_pgl)
    bcopy(pref_pgl, _pref_peer_group_leader, 22);
  else
    bzero((char *)_pref_peer_group_leader, 22);
}

ig_nodal_info_group::ig_nodal_info_group(Addr * atm_addy, u_char prior, 
					 u_char flags, NodeID * pref_pgl, 
					 ig_next_hi_level_binding_info * 
					 higher) :
  InfoGroup(InfoGroup::ig_nodal_info_group_id), 
  _leadership_priority(prior), _nodal_flags(flags),
  _next_higher_lvl(higher)
{ 
  if (atm_addy)
    atm_addy->encode(_atm_address);
  else
    bzero((char *)_atm_address, 20);
  if (pref_pgl)
    bcopy((void *)pref_pgl->GetNID(), _pref_peer_group_leader, 22);
  else
    bzero((char *)_pref_peer_group_leader, 22);
}

ig_nodal_info_group::ig_nodal_info_group(const ig_nodal_info_group & rhs) 
  : InfoGroup(InfoGroup::ig_nodal_info_group_id), 
    _leadership_priority(rhs._leadership_priority), 
    _nodal_flags(rhs._nodal_flags), _next_higher_lvl(0) 
{
  bcopy(rhs._atm_address, _atm_address, 20);
  bcopy(rhs._pref_peer_group_leader, _pref_peer_group_leader, 22);
  if (rhs._next_higher_lvl)
    _next_higher_lvl = (ig_next_hi_level_binding_info *)rhs._next_higher_lvl->copy();
}
 
ig_nodal_info_group::~ig_nodal_info_group() 
{ 
  if (_next_higher_lvl) delete _next_higher_lvl;
}

InfoGroup * ig_nodal_info_group::copy(void)
{
  return new ig_nodal_info_group(*this);
}

void ig_nodal_info_group::size(int & length)
{
  length  += NODAL_INFO_HEADER; // without Next higher level binding info 48
  if(_next_higher_lvl)
    _next_higher_lvl->size(length);
}

u_char * ig_nodal_info_group::encode(u_char * buffer, int & buflen)
{
  int i;

  buflen = 0;
  u_char * temp = buffer + ig_header_len;


  for (i = 0; i < 20; i++)
    *temp++ = _atm_address[i];
  buflen += 20;

  *temp++ = _leadership_priority;
  *temp++ = _nodal_flags;
  buflen += 2;

  for (i = 0; i < 22; i++)
    *temp++ = _pref_peer_group_leader[i];
  buflen += 22;

  if (_next_higher_lvl) {
    int tlen = 0;
    temp = _next_higher_lvl->encode(temp, tlen);
    buflen += tlen;
  }

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_nodal_info_group::decode(u_char * buffer)
{
  int i;
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (len < 44 || type != InfoGroup::ig_nodal_info_group_id)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);

  u_char * temp = &buffer[4];

  for (i = 0; i < 20; i++)
    _atm_address[i] = *temp++;
  len -= 20;

  _leadership_priority = *temp++;
  _nodal_flags = *temp++;
  len -= 2;

  for (i = 0; i < 22; i++)
    _pref_peer_group_leader[i] = *temp++;
  len -= 22;

  errmsg * rval = 0;

  if (len < 0)
    return new errmsg(errmsg::incomplete_ig, errmsg::none, type);

  else if (len > 0)
  {
    int type = (((temp[0] << 8) & 0xFF00) | (temp[1] & 0xFF));
    int tlen = (((temp[2] << 8) & 0xFF00) | (temp[3] & 0xFF));

    _next_higher_lvl = new ig_next_hi_level_binding_info;
    rval = _next_higher_lvl->decode(temp);

    len -= tlen + 4;
  }
  if(len < 0 || len > 0)
   return new errmsg(errmsg::incomplete_ig, errmsg::none, type);
  else
   return rval;
}
  
bool ig_nodal_info_group::PrintSpecific(ostream & os)
{
  os << "     ATM Address::";
  HexDump(_atm_address, 20, os);
  os << "     Leadership Priority::" << hex << (int)_leadership_priority 
     << endl;
  os << "     Nodal Flags::" << hex << (int)_nodal_flags << dec << endl;
  os << "     Pref. Peer Group Leader::" << (int)_pref_peer_group_leader[0]
     << ":" << (int)_pref_peer_group_leader[1] << ":";
  HexDump(_pref_peer_group_leader+2, 20, os);
  os << dec << endl;
  if (_next_higher_lvl)
    os << *(_next_higher_lvl) << endl;
  return true;
}

bool ig_nodal_info_group::ReadSpecific(istream & os)
{
  return false;
}

ostream & operator << (ostream & os, ig_nodal_info_group & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

istream & operator >> (istream & is, ig_nodal_info_group & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return is;
}

const ig_next_hi_level_binding_info * 
ig_nodal_info_group::GetNextHigherLevel(void) const 
{ 
  return _next_higher_lvl; 
}

void 
ig_nodal_info_group::SetNextHigherLevel(ig_next_hi_level_binding_info *nhl)
{
  if (_next_higher_lvl != nhl) 
    delete _next_higher_lvl;
  _next_higher_lvl = nhl;
}

const Addr   * ig_nodal_info_group::GetATMAddress(void) const 
{ 
  int i = 20; 
  Addr * rval = new NSAP_DCC_ICD_addr; 
  rval->decode((u_char *)_atm_address, i); 
  return rval;
}

int ig_nodal_info_group::GetATM(Addr * rval) const
{
  if (rval) {
    int i = 20;
    rval->decode((u_char *)_atm_address, i);
    return 0;
  }
  return -1;
}

const u_char   ig_nodal_info_group::GetLeadershipPriority(void) const 
{ 
  return _leadership_priority; 
}

const u_char   ig_nodal_info_group::GetNodalFlags(void) const 
{ 
  return _nodal_flags; 
}

const NodeID * ig_nodal_info_group::GetPreferredPGL(void) const 
{ 
  return new NodeID(_pref_peer_group_leader); 
}

int ig_nodal_info_group::GetPreferredPGL(NodeID & rval) const
{
  NodeID tmp(_pref_peer_group_leader);
  rval = tmp;
  return 0;
}
