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
static char const _ptsp_cc_rcsid_[] =
"$Id: ptsp.cc,v 1.43 1999/01/14 21:48:26 battou Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_pkt/ptsp.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/id.h>

PTSPPkt::PTSPPkt(const u_char * nid, const u_char * pid) 
  : PNNIPkt(PNNIPkt::ptsp)
{
  if (nid)
    bcopy((unsigned char *)nid, _orig_node_id, 22);
  else
    bzero(_orig_node_id, 22);
  if (pid)
    bcopy((unsigned char *)pid, _orig_peer_group_id, 14);
  else
    bzero(_orig_peer_group_id, 14);
}

PTSPPkt::~PTSPPkt()
{
  list_item li;
  forall_items(li, _elements) {
    ig_ptse * ptr = _elements.inf(li);
    if (ptr)
      ptr->UnReference();
  }
  _elements.clear();
}

// Deep copy
PNNIPkt * PTSPPkt::copy(void) const
{
  return copy(false);
}

PNNIPkt * PTSPPkt::copy(bool nondeep) const
{
  PTSPPkt * rval = new PTSPPkt(_orig_node_id, _orig_peer_group_id);

  list_item li;
  // Copy over all elements regardless.
  forall_items(li, _elements) {
    ig_ptse * ptr = _elements.inf(li);
    if (nondeep && (ptr->GetTTL() == DBKey::ExpiredAge))
      rval->_elements.append(ptr);
    else
      rval->_elements.append((ig_ptse *)ptr->copy());
  }
  return rval;
}

void PTSPPkt::size(int & length)
{
  list_item li;
  length = FIXED_PTSP_HEADER; // PNNI header + PTSP specific, 44 bytes
  forall_items(li, _elements) {
    (_elements.inf(li))->size(length);
  }
}

// computes the common part of the PTSE checksum which includes
// the node ID and peer-group ID of the PTSP

static  long common_checksum(u_short *buf)
{
  register long sum = 0;
  int count = 18; // 22 for node-d + 14 for peer-group id expressed in shorts as 18
  while (count--)
    {
      sum += *buf++;
      if ( sum & 0xffff0000 )
	{
	  // carry occurred, wrap around
	  sum &= 0xffff;
	  sum++;
	}
    }
  return sum;
}

static void fill_in_checksum(long c1, u_short *buf, int len)
{
  register long sum = c1;
  int count = len/2;
  int i;
  buf[8] = 0; // set checksum field to zero
  for(i = 0; i < count; i++)
    {
      if(i == 9)
	continue; // skip the PTSE remaining lifetime
      sum += buf[i];
      if ( sum & 0xffff0000 )
	{
	  // carry occurred, wrap around
	  sum &= 0xffff;
	  sum++;
	}
    }
  buf[8] = ~(sum & 0xffff);
}

u_char * PTSPPkt::encode(u_char * buffer, int & buflen)
{
  int i;

  buflen = 0;
  u_char * temp = buffer + pkt_header_len;

  for (i = 0; i < 22; i++)
    *temp++ = _orig_node_id[i];
  buflen += 22;

  for (i = 0; i < 14; i++)
    *temp++ = _orig_peer_group_id[i];
  buflen += 14;

  // do partial checksum of node id & peer-group id
  u_short foo[18];
  bcopy((void *)&_orig_node_id[0],(void *)&foo[0],22);
  bcopy((void *)&_orig_peer_group_id[0],(void *)&foo[11],14);
  long c1 =  common_checksum(foo);
  ig_ptse * ptr;
  list_item li;
  forall_items(li, _elements) {
    ptr = _elements.inf(li);
    int len = 0;
    u_short *buf = (u_short *)temp; // save start of encoded ig_ptse
    assert((int )buf%2 == 0);
    temp = ptr->encode(temp, len);
    // do the PTSE checksum
    fill_in_checksum(c1, buf, len);
    buflen += len;
  }
  encode_pkt_header(buffer, buflen);
  return temp;
}

errmsg * PTSPPkt::decode(const u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF)), i;
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != PNNIPkt::ptsp || len < 40)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_pkt_header(buffer);
  len -= 8;

  u_char * temp = (u_char *)buffer + pkt_header_len;

  for (i = 0; i < 22; i++)
    _orig_node_id[i] = *temp++;
  len -= 22;

  if (len < 14)
    return new errmsg(errmsg::incomplete_ig, errmsg::none, type);

  for (i = 0; i < 14; i++)
    _orig_peer_group_id[i] = *temp++;
  len -= 14;

  errmsg * rval = 0;

  while (len > 0) {
    int tlen = (((temp[2] << 8) & 0xFF00) | (temp[3] & 0xFF));
    
    ig_ptse * ptr = new ig_ptse(InfoGroup::ig_unknown_id, -1, -1);
    rval  = ptr->decode(temp);
    _elements.append(ptr);

    if(rval) return rval;

    len  -= tlen + 4;
    temp += tlen + 4;
  }

  return rval;
}

bool PTSPPkt::AddPTSE(ig_ptse * p)
{
  if (p && !_elements.search(p)) {
    _elements.append(p);
    // sandeep  Aug 18 below
    // p->Reference();
    return true;
  }
  return false;
}

bool PTSPPkt::RemPTSE(ig_ptse * p)
{
  ig_ptse *ptr;

  list_item li;
  forall_items(li, _elements) {
    ptr = _elements.inf(li);
    if (ptr->equals(*p)) {
      _elements.del_item(li);
      ptr->UnReference();
      return true;
    }
  }
  return false;
}

const u_char * PTSPPkt::GetOID(void) const  
{ return _orig_node_id; }

const u_char * PTSPPkt::GetPGID(void) const 
{ return _orig_peer_group_id; }

const list<ig_ptse *> & PTSPPkt::GetElements(void) const 
{ return _elements; }

void  PTSPPkt::AddElement(ig_ptse * ptse) 
{ _elements.append(ptse); }

void PTSPPkt::RemElement(ig_ptse * ptse)
{
  list_item li = _elements.search(ptse);
  _elements.del_item(li);
}

void PTSPPkt::ClearElements()
{ _elements.clear(); }

void PTSPPkt::ClearExpiredPtses()
{
  // Clear Dead ptse items
  list<ig_ptse *> list_dead_ptse;
  list_item li;
  forall_items(li, _elements) {
    ig_ptse * lPtse = _elements.inf(li);
    if (lPtse->GetTTL() == DBKey::ExpiredAge)
      list_dead_ptse.append(lPtse);
  }
  
  if (!(list_dead_ptse.empty())) {
    forall_items(li, list_dead_ptse) {
      ig_ptse * lPtse = list_dead_ptse.inf(li);
      RemElement(lPtse);
    }
  }
}

int   PTSPPkt::GetSize(void)
{ 
  return _elements.size(); 
}

bool PTSPPkt::PrintSpecific(ostream & os) const
{
  NodeID nid(_orig_node_id);
  PeerID pgid(_orig_peer_group_id);
  os << "NodeID: " << nid << endl;
  os << pgid << endl;
  
  list_item li;
  forall_items(li, _elements) {
    ig_ptse * ptr = _elements.inf(li);
    os << *(ptr);
  }
  os << endl;
  return true;
}

bool operator == ( const PTSPPkt & lhs, const PTSPPkt & rhs)
{
  int nodecmp = memcmp((u_char *)lhs._orig_node_id, (u_char *)rhs._orig_node_id,
		       22);
  int pgcmp = memcmp((u_char *)lhs._orig_peer_group_id, (u_char *)rhs._orig_peer_group_id, 14);
  if ((nodecmp == 0) && (pgcmp == 0)) return true;
  return false;
}

ostream & operator << (ostream & os, const PTSPPkt & pkt)
{
  pkt.PrintGeneral(os);
  pkt.PrintSpecific(os);
  return os;
}

int compare(PTSPPkt *const & lhs, PTSPPkt *const & rhs)
{
  int nodecmp = memcmp(lhs->_orig_node_id, rhs->_orig_node_id, 22);
  int pgcmp = memcmp(lhs->_orig_peer_group_id, rhs->_orig_peer_group_id, 14);
  if (pgcmp < 0) return -1;
  if (pgcmp > 0) return 1;
  if (nodecmp < 0) return -1;
  if (nodecmp > 0) return 1;
  return 0;
}
