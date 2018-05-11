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
static char const _nodal_ptse_summary_cc_rcsid_[] =
"$Id: nodal_ptse_summary.cc,v 1.17 1998/09/24 13:31:58 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/nodal_ptse_summary.h>
#include <codec/pnni_ig/DBKey.h>
#include <codec/pnni_ig/id.h>

ig_nodal_ptse_summary::ig_nodal_ptse_summary(u_char *onode, u_char *opeer) :
  InfoGroup(InfoGroup::ig_nodal_ptse_summary_id), _ptse_summary_count(0)
{
  if (onode)
    bcopy(onode, _originating_node, 22);
  else
    bzero(_originating_node, 22);
  if (opeer)
    bcopy(opeer, _orig_peer_group, 14);
  else
    bzero(_orig_peer_group, 14);
}

ig_nodal_ptse_summary::ig_nodal_ptse_summary(NodeID * onode, PeerID * opeer) :
  InfoGroup(InfoGroup::ig_nodal_ptse_summary_id), _ptse_summary_count(0)
{
  if (onode)
    bcopy((void *)onode->GetNID(), _originating_node, 22);
  else
    bzero(_originating_node, 22);
  if (opeer)
    bcopy((u_char *)opeer->GetPID(), _orig_peer_group, 14);
  else
    bzero(_orig_peer_group, 14);
}

ig_nodal_ptse_summary::ig_nodal_ptse_summary(const ig_nodal_ptse_summary & rhs)
  : InfoGroup(InfoGroup::ig_nodal_ptse_summary_id), _ptse_summary_count(0)
{
  bcopy(rhs._originating_node, _originating_node, 22);
  bcopy(rhs._orig_peer_group,  _orig_peer_group,  14);

  if (rhs._list.empty() == false) {
    list_item li;
    forall_items(li, rhs._list) {
      SumContainer * sptr = rhs._list.inf(li);
      AddSum(sptr->_ptse_type, sptr->_ptse_id, sptr->_ptse_seq, 
	     sptr->_ptse_checksum, sptr->_ptse_rem_life);
    }
  }
}

ig_nodal_ptse_summary::~ig_nodal_ptse_summary()
{ 
  list_item li;
  forall_items(li, _list) {
    SumContainer * sptr = _list.inf(li); 
    delete sptr; 
  }
}

InfoGroup * ig_nodal_ptse_summary::copy(void)
{
  return new ig_nodal_ptse_summary(*this);
}

void ig_nodal_ptse_summary::size(int & length)
{
 length += DS_SUMMARY_HEADER;  // default w/o any summary 44
 length +=  _ptse_summary_count*DS_SUMMARY; // Each of the summary size 16
}

u_char * ig_nodal_ptse_summary::encode(u_char * buffer, int & buflen)
{
  int i;

  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  for (i = 0; i < 22; i++)
    *temp++ = _originating_node[i];
  buflen += 22;

  for (i = 0; i < 14; i++)
    *temp++ = _orig_peer_group[i];
  buflen += 14;

  // Reserved
  *temp++ = 0;
  *temp++ = 0;
  *temp++ = (_ptse_summary_count >> 8) & 0xFF;
  *temp++ = (_ptse_summary_count & 0xFF);
  buflen += 4;

  list_item li;
  forall_items(li, _list) {
    SumContainer * sptr = _list.inf(li);
    *temp++ = (sptr->_ptse_type >> 8) & 0xFF;
    *temp++ = (sptr->_ptse_type & 0xFF);

    // Reserved
    *temp++ = 0;
    *temp++ = 0;

    *temp++ = (sptr->_ptse_id >> 24) & 0xFF;
    *temp++ = (sptr->_ptse_id >> 16) & 0xFF;
    *temp++ = (sptr->_ptse_id >>  8) & 0xFF;
    *temp++ = (sptr->_ptse_id & 0xFF);

    *temp++ = (sptr->_ptse_seq >> 24) & 0xFF;
    *temp++ = (sptr->_ptse_seq >> 16) & 0xFF;
    *temp++ = (sptr->_ptse_seq >>  8) & 0xFF;
    *temp++ = (sptr->_ptse_seq & 0xFF);

    *temp++ = (sptr->_ptse_checksum >> 8) & 0xFF;
    *temp++ = (sptr->_ptse_checksum & 0xFF);

    *temp++ = (sptr->_ptse_rem_life >> 8) & 0xFF;
    *temp++ = (sptr->_ptse_rem_life & 0xFF);

    buflen += 16;
  }
  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_nodal_ptse_summary::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF)), i;
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_nodal_ptse_summary_id || len < 40)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);

  u_char * temp = &buffer[4];

  for (i = 0; i < 22; i++)
    _originating_node[i] = *temp++;
  len -= 22;

  for (i = 0; i < 14; i++)
    _orig_peer_group[i] = *temp++;
  len -= 14;

  // Reserved
  temp += 2;
  _ptse_summary_count  = (*temp++ << 8) & 0xFF00;
  _ptse_summary_count |= (*temp++ & 0xFF);
  len -= 4;

  if (len < (_ptse_summary_count * 16))
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  int counter = _ptse_summary_count;
  for (i = 0; i < counter; i++)
  {
    int t, i, s, c, l;

    t  = (*temp++ << 8) & 0xFF00;
    t |= (*temp++ & 0xFF);
    temp+= 2;

    i  = (*temp++ << 24) & 0xFF000000;
    i |= (*temp++ << 16) & 0x00FF0000;
    i |= (*temp++ <<  8) & 0x0000FF00;
    i |= (*temp++ & 0xFF);

    s  = (*temp++ << 24) & 0xFF000000;
    s |= (*temp++ << 16) & 0x00FF0000;
    s |= (*temp++ <<  8) & 0x0000FF00;
    s |= (*temp++ & 0xFF);

    c  = (*temp++ << 8) & 0xFF00;
    c |= (*temp++ & 0xFF);

    l  = (*temp++ << 8) & 0xFF00;
    l |= (*temp++ & 0xFF);

    AddSum(t, i, s, c, l);
    len -= 16;
  }
  if (len != 0)
    return new errmsg(errmsg::incomplete_ig, errmsg::none, type);
  else
    return 0;
}
  
void ig_nodal_ptse_summary::AddSum(sh_int t, int i, int s, sh_int c, sh_int l)
{
  SumContainer *sptr = new SumContainer(t, i, s, c, l);
  _list.append(sptr);
  _ptse_summary_count = _list.size();
}

bool ig_nodal_ptse_summary::RemSum(sh_int t, int i, int s, sh_int c, sh_int l)
{
  SumContainer cont(t, i, s, c, l);

  list_item li;
  forall_items(li, _list) {
    SumContainer * sptr = _list.inf(li);
    if (sptr->equals(cont)) {
      _list.del_item(_list.search(sptr));
      delete sptr;
      _ptse_summary_count = _list.size();
      return true;
    }
  }
  return false;
}

int ig_nodal_ptse_summary::equals(ig_nodal_ptse_summary & rhs)
{ 
  return (!memcmp(_originating_node, rhs._originating_node, 22) &&
	  !memcmp(_orig_peer_group, rhs._orig_peer_group, 14) &&
	  _ptse_summary_count == rhs._ptse_summary_count);
}

const list<SumContainer *> & ig_nodal_ptse_summary::GetContainers(void) const 
{ return _list; }

const NodeID * ig_nodal_ptse_summary::GetOrigin(void) const 
{ return new NodeID(_originating_node); }

const PeerID * ig_nodal_ptse_summary::GetPeer(void) const 
{ return new PeerID(_orig_peer_group); }

const sh_int   ig_nodal_ptse_summary::GetPTSESumCnt(void) const
{ return _ptse_summary_count; }

bool ig_nodal_ptse_summary::PrintSpecific(ostream & os)
{
  int i;
  NodeID nid(_originating_node); 
  PeerID pgid(_orig_peer_group);
  os << "NodeID: " << nid << endl;
  os << pgid << endl;
  list_item li;
  forall_items(li, _list) {
    SumContainer * sptr = _list.inf(li);
    os << "     PTSE Type::"     << sptr->_ptse_type << endl;
    os << "     PTSE ID::"       << sptr->_ptse_id << endl;
    os << "     PTSE Seq. Num::" << sptr->_ptse_seq << endl;
    os << "     PTSE Checksum::" << sptr->_ptse_checksum << endl;
    os << "     PTSE Remaining Life::" << sptr->_ptse_rem_life << endl;
  }
  return true;
}

bool ig_nodal_ptse_summary::ReadSpecific(istream & os)
{
  char buf[80], *temp = buf;
  // Incomplete
  return false;
}

ostream & operator << (ostream & os, ig_nodal_ptse_summary & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return os;
}

istream & operator >> (istream & is, ig_nodal_ptse_summary & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return is;
}

SumContainer::SumContainer(sh_int type, int id, int seq, sh_int check, sh_int life) :
    _ptse_type(type), _ptse_id(id), _ptse_seq(seq), _ptse_checksum(check),
    _ptse_rem_life(life) { }

int compare(SumContainer *const & lhs, SumContainer *const & rhs)
{
  if((lhs->_ptse_id == rhs->_ptse_id) &&
     (lhs->_ptse_seq == rhs->_ptse_seq) &&
     (((lhs->_ptse_rem_life == DBKey::ExpiredAge)&&
      (rhs->_ptse_rem_life == DBKey::ExpiredAge)) ||
      ((lhs->_ptse_rem_life != DBKey::ExpiredAge) &&
       (rhs->_ptse_rem_life != DBKey::ExpiredAge)))) return 0;
  else if((lhs->_ptse_id < rhs->_ptse_id) ||
         ((lhs->_ptse_id == rhs->_ptse_id) &&
          (lhs->_ptse_seq < rhs->_ptse_seq)) ||
         ((lhs->_ptse_id == rhs->_ptse_id) &&
          (lhs->_ptse_seq == rhs->_ptse_seq) &&
          ((lhs->_ptse_rem_life != DBKey::ExpiredAge) &&
           (rhs->_ptse_rem_life == DBKey::ExpiredAge)))) return -1;
  return 1;

}

int compare(ig_nodal_ptse_summary *const & lhs, ig_nodal_ptse_summary *const & rhs)
{
  int nodecmp = memcmp(lhs->_originating_node, rhs->_originating_node, 22);
  int pgcmp = memcmp(lhs->_orig_peer_group, rhs->_orig_peer_group, 14);
  if(pgcmp < 0) return -1;
  if(pgcmp > 0) return 1;
  if(nodecmp < 0) return -1;
  if(nodecmp > 0) return 1;
  return 0;
}
