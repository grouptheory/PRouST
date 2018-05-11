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
static char const _nodal_ptse_ack_cc_rcsid_[] =
"$Id: nodal_ptse_ack.cc,v 1.20 1998/09/23 19:03:38 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/nodal_ptse_ack.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/DBKey.h>
#include <codec/pnni_ig/id.h>

ig_nodal_ptse_ack::ig_nodal_ptse_ack(const u_char *nid) :
  InfoGroup(InfoGroup::ig_nodal_ptse_ack_id), _ack_count(0)
{
  if (nid)
    bcopy((void *)nid, (void *)_node_id, 22);
  else
    bzero(_node_id, 22);
}

ig_nodal_ptse_ack::ig_nodal_ptse_ack(NodeID * nid) :
  InfoGroup(InfoGroup::ig_nodal_ptse_ack_id), _ack_count(0)
{
  if (nid)
    bcopy((void *)nid->GetNID(), _node_id, 22);
  else
    bzero(_node_id, 22);
}

ig_nodal_ptse_ack::ig_nodal_ptse_ack(const ig_nodal_ptse_ack & rhs)
  : InfoGroup(InfoGroup::ig_nodal_ptse_ack_id), _ack_count(0)
{
  if (rhs._list.empty() == false) {
    list_item li;
    forall_items(li, rhs._list) {
      AckContainer * ackp = rhs._list.inf(li);
      AddAck(ackp->_ptse_id, ackp->_ptse_seq, ackp->_ptse_checksum, ackp->_ptse_rem_life);
    }
  }
}

ig_nodal_ptse_ack::~ig_nodal_ptse_ack()
{
#if 0
  list_item li;
  forall_items(li, _list) {
    AckContainer * aptr = _list.inf(li);
    delete aptr;
  }
#endif
}

InfoGroup * ig_nodal_ptse_ack::copy(void)
{
  return new ig_nodal_ptse_ack(*this);
}

void ig_nodal_ptse_ack::size(int & length)
{
  length += ACK_HEADER; // w/o any ackcontainers - 28
  length  += _ack_count*ACK_CONTAINER; // each of the container is 12 bytes
}

u_char * ig_nodal_ptse_ack::encode(u_char * buffer, int & buflen)
{
  int i;
  AckContainer *aptr;

  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  for (i = 0; i < 22; i++)
    *temp++ = _node_id[i];
  buflen += 22;

  *temp++ = (_ack_count >> 8) & 0xFF;
  *temp++ = (_ack_count & 0xFF);
  buflen += 2;

  list_item li;
  forall_items(li, _list)
  {
    aptr = _list.inf(li);
    *temp++ = (aptr->_ptse_id >> 24) & 0xFF;
    *temp++ = (aptr->_ptse_id >> 16) & 0xFF;
    *temp++ = (aptr->_ptse_id >>  8) & 0xFF;
    *temp++ = (aptr->_ptse_id & 0xFF);

    *temp++ = (aptr->_ptse_seq >> 24) & 0xFF;
    *temp++ = (aptr->_ptse_seq >> 16) & 0xFF;
    *temp++ = (aptr->_ptse_seq >>  8) & 0xFF;
    *temp++ = (aptr->_ptse_seq & 0xFF);

    *temp++ = (aptr->_ptse_checksum >> 8) & 0xFF;
    *temp++ = (aptr->_ptse_checksum & 0xFF);

    *temp++ = (aptr->_ptse_rem_life >> 8) & 0xFF;
    *temp++ = (aptr->_ptse_rem_life & 0xFF);

    buflen += 12;
  }

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_nodal_ptse_ack::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF)), i;
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_nodal_ptse_ack_id)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);

  u_char * temp = &buffer[4];

  for (i = 0; i < 22; i++)
    _node_id[i] = *temp++;
  len -= 22;

  if (len < 0)
    return new errmsg(errmsg::incomplete_ig, errmsg::none, type);

  _ack_count  = (*temp++ << 8) & 0xFF00;
  _ack_count |= (*temp++ & 0xFF);
  len -= 2;

  if (len < 0)
    return new errmsg(errmsg::incomplete_ig, errmsg::none, type);

  errmsg * rval = 0;

  int counter = _ack_count;
  for (i = 0; i < counter; i++)
  {
    int id = 0 , seq = 0, check = 0, life = 0;

    id  = (*temp++ << 24) & 0xFF000000;
    id |= (*temp++ << 16) & 0x00FF0000;
    id |= (*temp++ <<  8) & 0x0000FF00;
    id |= (*temp++ & 0xFF);
    len -= 4;
    if (len < 0)
      return new errmsg(errmsg::incomplete_ig, errmsg::none, type);

    seq  = (*temp++ << 24) & 0xFF000000;
    seq |= (*temp++ << 16) & 0x00FF0000;
    seq |= (*temp++ <<  8) & 0x0000FF00;
    seq |= (*temp++ & 0xFF);
    len -= 4;
    if (len < 0)
      return new errmsg(errmsg::incomplete_ig, errmsg::none, type);

    check |= (*temp++ <<  8) & 0xFF00;
    check |= (*temp++ & 0xFF);

    life |= (*temp++ <<  8) & 0xFF00;
    life |= (*temp++ & 0xFF);

    len -= 4;

    if (len < 0)
      return new errmsg(errmsg::incomplete_ig, errmsg::none, type);

    AddAck(id, seq, check, life);
  }
  return 0;
}
  
void ig_nodal_ptse_ack::AddAck(int id, int seq, sh_int checksum, sh_int rem_life)
{
  // RemAck will remove it if it's already there
  RemAck(id, seq, checksum, rem_life);

  _list.append(new AckContainer(id, seq, checksum, rem_life));
  _ack_count = _list.size();
}

bool ig_nodal_ptse_ack::RemAck(int id, int seq, sh_int checksum, sh_int rem_life)
{
  AckContainer cont(id, seq, checksum, rem_life);

  list_item li;
  forall_items(li, _list) {
    AckContainer * aptr = _list.inf(li);
    if (aptr && aptr->equals(cont)) {
      _list.del_item(_list.search(aptr));
      delete aptr;
      _ack_count = _list.size();
      return true;
    }
  }
  return false;
}


int ig_nodal_ptse_ack::equals(ig_nodal_ptse_ack & rhs) 
{
  if (memcmp(_node_id, rhs._node_id, 22) == 0)
    return  1; 
  return 0;
}

bool ig_nodal_ptse_ack::equals(const u_char * r_orig_node_id)
{  
  if (memcmp(_node_id, r_orig_node_id, 22) == 0) 
    return 1;
  return 0;
}

const list<AckContainer *> & ig_nodal_ptse_ack::GetAcks(void) const { return _list; }
 
const NodeID * ig_nodal_ptse_ack::GetNodeID(void) const { return new NodeID(_node_id); }
const sh_int   ig_nodal_ptse_ack::GetAckCount(void) const { return _ack_count; }
const u_char * ig_nodal_ptse_ack::GetOID() const { return _node_id;}

bool ig_nodal_ptse_ack::PrintSpecific(ostream & os)
{
  int i;
  AckContainer * aptr;

  NodeID nid(_node_id);
  os << "NodeID: " << nid << endl;
  os << "    AckCount::" << _ack_count << endl;
  list_item li;
  forall_items(li, _list) {
    aptr = _list.inf(li);
    os << "    PTSE ID::" << aptr->_ptse_id << endl;
    os << "    PTSE Sequence Num::" << aptr->_ptse_seq << endl;
    os << "    PTSE Checksum::" << aptr->_ptse_checksum << endl;
    os << "    PTSE Remaining Lifetime::" << aptr->_ptse_rem_life << endl;
  }
  return true;
}

bool ig_nodal_ptse_ack::ReadSpecific(istream & is)
{
  char buf[80], *temp = buf;
  // incomplete
  return false;
}

ostream & operator << (ostream & os, ig_nodal_ptse_ack & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return os;
}

int operator == ( ig_ptse & lhs, AckContainer & rhs)
{
   return((lhs.GetID() ==  rhs._ptse_id ) &&
          (lhs.GetSN()  == rhs._ptse_seq) &&
          (lhs.GetCS() == rhs._ptse_checksum) &&
          (lhs.GetTTL() == rhs._ptse_rem_life) );
}

istream & operator >> (istream & is, ig_nodal_ptse_ack & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return is;
}

AckContainer::AckContainer(int id, int seq, sh_int checksum, sh_int remlife) :
  _ptse_id(id), _ptse_seq(seq), _ptse_checksum(checksum),  _ptse_rem_life(remlife)
{ }

int compare(AckContainer *const & lhs,
	    AckContainer *const & rhs)
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

int compare(ig_nodal_ptse_ack *const & lhs, ig_nodal_ptse_ack *const & rhs)
{
           return(memcmp(lhs->_node_id, rhs->_node_id, 22));
}
