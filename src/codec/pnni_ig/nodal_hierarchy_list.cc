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
static char const _nodal_list_cc_rcsid_[] =
"$Id: nodal_hierarchy_list.cc,v 1.19 1998/10/05 20:00:41 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/nodal_hierarchy_list.h>
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/addr.h>

Level::Level(u_char * l, u_char * a, u_char * p)
{
  if (l)
    bcopy(l, _log_node_id, 22);
  else
    bzero((char *)_log_node_id, 22);
  if (a)
    bcopy(a, _atm_end_sys_addr, 20);
  else
    bzero((char *)_atm_end_sys_addr, 20);
  if (p)
    bcopy(p, _peer_group_id, 14);
  else
    bzero((char *)_peer_group_id, 14);
}

Level::Level(const Level & rhs)
{
  bcopy((void *)rhs._log_node_id, _log_node_id, 22);
  bcopy((void *)rhs._atm_end_sys_addr, _atm_end_sys_addr, 20);
  bcopy((void *)rhs._peer_group_id, _peer_group_id, 14);
}

Level::Level(NodeID * nid, Addr * atm, PeerID * pid)
{
  if (nid)
    bcopy((u_char *)nid->GetNID(), _log_node_id, 22);
  else
    bzero(_log_node_id, 22);
  if (atm)
    atm->encode(_atm_end_sys_addr);
  else
    bzero((char *)_atm_end_sys_addr, 20);
  if (pid)
    bcopy((u_char *)pid->GetPID(), _peer_group_id, 14);
  else
    bzero((char *)_peer_group_id, 14);
}

u_char * Level::encode(u_char * buffer, int & buflen)
{
  int i = 0;
  u_char * temp = buffer;

  for (i = 0; i < 22; i++)
    *temp++ = _log_node_id[i];
  for (i = 0; i < 20; i++)
    *temp++ = _atm_end_sys_addr[i];
  for (i = 0; i < 14; i++)
    *temp++ = _peer_group_id[i];
  buflen += (22 + 20 + 14);
  return temp;
}

Level * Level::copy(void)
{
  Level * rval = new Level(_log_node_id, _atm_end_sys_addr, _peer_group_id);
  return rval;
}

int Level::equals(Level & rhs)
{
  return (!memcmp(_log_node_id, rhs._log_node_id, 22) &&
	  !memcmp(_atm_end_sys_addr, rhs._atm_end_sys_addr, 20) &&
	  !memcmp(_peer_group_id, rhs._peer_group_id, 14) );
}

PeerID * Level::GetPID(void) 
{ 
  return new PeerID(_peer_group_id); 
}

NodeID * Level::GetNID(void) 
{ 
  return new NodeID(_log_node_id);  
}

Addr   * Level::GetATM(void) 
{ 
  int i = 20; Addr * rval = new NSAP_DCC_ICD_addr; rval->decode(_atm_end_sys_addr, i); return rval; 
}

bool Level::PrintSpecific(ostream & os)
{
  int i;
  NodeID nid(_log_node_id);
  os << "     Log NodeID: " << nid << endl;
  os << "     ATM End Sys Addr::";
  char buf[41]; buf[0] = '\0';
  for (i = 0; i < 20; i++)
    sprintf(buf, "%s%s%x", buf, (_atm_end_sys_addr[i] < 0x10 ? "0" : ""), (int)_atm_end_sys_addr[i]);
  buf[40] = '\0';
  os << buf << endl;
  PeerGroupID pgid(_peer_group_id);
  os << "     PeerGroupID: " << pgid;
  return true;
}

int operator > (const Level & lhs, const Level & rhs)
{
  if (((int)lhs._log_node_id[0]) > ((int)rhs._log_node_id[0]))
    return 1;
  return 0;
}

int compare(Level *const & lhs, Level *const & rhs)
{
  return (!memcmp(lhs->_log_node_id, rhs->_log_node_id, 22) &&
	  !memcmp(lhs->_atm_end_sys_addr, rhs->_atm_end_sys_addr, 20) &&
	  !memcmp(lhs->_peer_group_id, rhs->_peer_group_id, 14));
}

// ----------------------------------------------------------------------
ig_nodal_hierarchy_list::ig_nodal_hierarchy_list(long seq_num) :
  InfoGroup(InfoGroup::ig_nodal_hierarchy_list_id), 
  _seq_num(seq_num), _level_count(0), _levels(0) { }

ig_nodal_hierarchy_list::ig_nodal_hierarchy_list(const ig_nodal_hierarchy_list & list) :
  InfoGroup(InfoGroup::ig_nodal_hierarchy_list_id), 
  _seq_num(list._seq_num), _level_count(0), _levels(0)
{
  if (list._levels && list._levels->empty() == false) {
    _levels = NewLev();
  
    list_item li;
    forall_items(li, *(list._levels)) {
      Level * lptr = (list._levels)->inf(li); 
      Level *tmp = lptr->copy();
      AddLevel(tmp);
      delete tmp;
    }
    _level_count = _levels->size();
  }
}

ig_nodal_hierarchy_list::~ig_nodal_hierarchy_list() 
{ 
  if (_levels) {
    list_item li;
    forall_items(li, *_levels) {
      Level * lptr = _levels->inf(li);
      delete lptr; 
    }
    delete _levels;
  }
}

InfoGroup * ig_nodal_hierarchy_list::copy(void)
{
  return new ig_nodal_hierarchy_list(*this);
}

void ig_nodal_hierarchy_list::size(int & length)
{
  length += NHL_HEADER; // ig header + nodal_hierarchy specific 12
  length += _level_count*NHL_CONTENT; // ( nodID+ATM Addr+ PGID)(22+20+14) 56
}

u_char * ig_nodal_hierarchy_list::encode(u_char *buffer, int & buflen)
{
  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  *temp++ = (_seq_num >> 24) & 0xFF;
  *temp++ = (_seq_num >> 16) & 0xFF;
  *temp++ = (_seq_num >>  8) & 0xFF;
  *temp++ = (_seq_num & 0xFF);
  buflen += 4;

  // Reserved
  *temp++ = 0;
  *temp++ = 0;
  buflen += 2;

  // Level count
  *temp++ = (_level_count >> 8) & 0xFF;
  *temp++ = (_level_count & 0xFF);
  buflen += 2;

  if (_levels) {
    list_item li;
    forall_items(li, *_levels) {
      Level * lptr = _levels->inf(li);
      int tlen = 0;
      temp = lptr->encode(temp, tlen);
      buflen += tlen;
    }
  }
  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_nodal_hierarchy_list::decode(u_char *buffer)
{
  int i = 0;

  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));

  if (len < 8 || type != InfoGroup::ig_nodal_hierarchy_list_id)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);

  u_char * temp = &buffer[4];

  _seq_num  = (*temp++ << 24) & 0xFF000000;
  _seq_num |= (*temp++ << 16) & 0x00FF0000;
  _seq_num |= (*temp++ <<  8) & 0x0000FF00;
  _seq_num |= (*temp++ & 0xFF);

  // skip reserved bytes
  *temp++;
  *temp++;

  // level count
  _level_count  = (*temp++ << 8) & 0xFF00;
  _level_count |= (*temp++ & 0xFF);

  len -= 8;

  int counter = _level_count;

  for (int j = 0; j < counter; j++) {
    u_char lognid[22], aesa[20], pgid[14];

    for (i = 0; i < 22; i++)
      lognid[i] = *temp++;
    for (i = 0; i < 20; i++)
      aesa[i] = *temp++;
    for (i = 0; i < 14; i++)
      pgid[i] = *temp++;
    AddLevel(lognid, aesa, pgid);

    len -= 56;
  }

  if ( len < 0 || len > 0)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);
  else
   return 0;
}

void ig_nodal_hierarchy_list::AddLevel(u_char *log_id, u_char *atm_addr, u_char *peer_id)
{
  if (!IsInLevels(log_id, atm_addr, peer_id)) {
    Level * lptr = new Level(log_id, atm_addr, peer_id);
    AddLevel(lptr);
    delete lptr;
  }
}

void ig_nodal_hierarchy_list::AddLevel(NodeID * nid, Addr * atm, PeerID * pid)
{
  if (!IsInLevels(nid, atm, pid)) {
    Level * lptr = new Level(nid, atm, pid);
    AddLevel(lptr);
    delete lptr;
  }
}

void ig_nodal_hierarchy_list::AddLevel(Level * ptr)
{
  bool inserted = false;
  Level * lptr = new Level(ptr->_log_node_id, ptr->_atm_end_sys_addr, ptr->_peer_group_id);

  if (!_levels) 
    _levels = NewLev(); // new list<Level *>;
  // The below code sorts the Levels based on the level of the NodeID in descending order
  // such as 96, 88, 80. ...
  list_item li;
  for (li = _levels->first(); li; li = _levels->succ(li)) {
    Level * lev = _levels->inf(li);
    if (*lptr > *lev) {
      inserted = true;
      // _levels->insert(lptr, li, list<Level *>::before);
      _levels->insert(lptr, li, before);
      break;
    }
  }
  if (!inserted) 
    _levels->append(lptr);
  
  _level_count = _levels->length();
}

bool ig_nodal_hierarchy_list::IsInLevels(u_char *log_id, u_char * atm_id, u_char * peer_id)
{
  if (!_levels)
    return false;

  Level lcont(log_id, atm_id, peer_id);

  list_item li;
  forall_items(li, *_levels) {
    Level * lptr = _levels->inf(li);
    if (lptr->equals(lcont))
      return true;
  }
  return false;
}

bool ig_nodal_hierarchy_list::IsInLevels(NodeID * nid, Addr * atm, PeerID * pid)
{
  if (!_levels)
    return false;

  Level lcont(nid, atm, pid);

  list_item li;
  forall_items(li, *_levels) {
    Level * lptr = _levels->inf(li);
    if (lptr->equals(lcont))
      return true;
  }
  return false;
}

bool ig_nodal_hierarchy_list::DelLevel(u_char * l, u_char * a, u_char * p)
{
  if (!_levels)
    return false;
  
  Level lcont(l, a, p);

  list_item li;
  forall_items(li, *_levels) {
    Level * lptr = _levels->inf(li);
    if (lptr->equals(lcont)) {
      _levels->del_item(_levels->search(lptr));
      delete lptr;
      return true;
    }
  }
  return false;
}

const list<Level *> & ig_nodal_hierarchy_list::GetLevels(void) const 
{ 
  return *_levels; 
}

// Clients had best not dick me over on this one, I'm giving you a ptr to my innerds!
const list<Level *> * ig_nodal_hierarchy_list::ShareLevels(void) const
{
  return _levels;
}

bool ig_nodal_hierarchy_list::PrintSpecific(ostream& os)
{
  os << "     Sequence Num::" << _seq_num << endl;
  os << "     Level Count::"  << _level_count << endl;
  if (_levels && _levels->empty() != false) {
    list_item li;
    forall_items(li, *_levels) {
      Level * lptr = _levels->inf(li); 
      lptr->PrintSpecific(os);
    }
  }

  return true;
}

bool ig_nodal_hierarchy_list::ReadSpecific(istream& is)
{
  return false;
}

const long   ig_nodal_hierarchy_list::GetSequenceNum(void) const 
{ 
  return _seq_num; 
}

const sh_int ig_nodal_hierarchy_list::GetLevelCount(void) const 
{ 
  return _level_count; 
}

// go thru the list and find the least common peer group
Level * ig_nodal_hierarchy_list::FindCommonPGID(ig_nodal_hierarchy_list * nhl)
{
  assert(nhl);
    
  const list<Level *> * this_lvls = ShareLevels();
  const list<Level *> * othr_lvls = nhl->ShareLevels();

  list_item this_li, othr_li;
  forall_items(this_li, *this_lvls) {
    PeerID * this_pid = this_lvls->inf(this_li)->GetPID();  // this makes a copy (see line 81)
    
    forall_items(othr_li, *othr_lvls) {
      PeerID * othr_pid = othr_lvls->inf(othr_li)->GetPID();  // this makes a copy (see line 81)

      if (this_pid->equals(othr_pid)) {
	delete othr_pid;
	delete this_pid;
	return (Level *)(this_lvls->inf(this_li));
      }
      delete othr_pid;
    }
    delete this_pid;
  }

  return 0;
}

ostream & operator << (ostream & os, ig_nodal_hierarchy_list & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

istream & operator >> (istream & is, ig_nodal_hierarchy_list & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return (is);
}

