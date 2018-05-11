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
static char const _ptse_ack_cc_rcsid_[] =
"$Id: ptse_ack.cc,v 1.21 1999/01/07 19:45:04 battou Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_pkt/ptse_ack.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/nodal_ptse_ack.h>

PTSEAckPkt::PTSEAckPkt() : PNNIPkt(PNNIPkt::ptse_ack) { }

PTSEAckPkt::~PTSEAckPkt() 
{ 
  if (!_list.empty()) {
    list_item li;
    forall_items(li, _list)
      delete _list.inf(li);
    _list.clear();
  }
}

PNNIPkt * PTSEAckPkt::copy(void) const
{
  PTSEAckPkt * rval = new PTSEAckPkt();
  list_item li;
  forall_items(li, _list) {
    rval->AddNodalPTSEAck((ig_nodal_ptse_ack *) (_list.inf(li))->copy());
  }
  return rval;
}

void PTSEAckPkt::size(int & length)
{
  list_item li; 
  length = pkt_header_len; // PNNIPkt Header 8 bytes

  forall_items(li, _list)
  {
    (_list.inf(li))->size(length); 
  }
}

u_char * PTSEAckPkt::encode(u_char * buffer, int & len)
{
  ig_nodal_ptse_ack *ptr;

  len = 0;
  u_char * temp = buffer + pkt_header_len;

  list_item li;
  forall_items(li, _list) {
    ptr = _list.inf(li); 
    int tlen = 0;
    temp = ptr->encode(temp, tlen);
    len += tlen;
  }
  encode_pkt_header(buffer, len);
  return temp;
}

errmsg * PTSEAckPkt::decode(const u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != PNNIPkt::ptse_ack)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);


  decode_pkt_header(buffer);
  len -= 8;

  u_char * temp = (u_char *)buffer + pkt_header_len;

  errmsg * rval = 0;
  ig_nodal_ptse_ack * ptr;

  while (len > 0)
  {
    int tlen = (((temp[2] << 8) & 0xFF00) | (temp[3] & 0xFF));

    ptr = new ig_nodal_ptse_ack;
    rval = ptr->decode(temp);
    _list.append(ptr);

    if(rval) return rval;

    len  -= tlen + 4;
    temp += tlen + 4;
  }
  return rval;
}

void PTSEAckPkt::AddNodalPTSEAck(ig_nodal_ptse_ack *a)
{
  ig_nodal_ptse_ack * ptr = new ig_nodal_ptse_ack(*(a));
  _list.append(ptr);
}

void PTSEAckPkt::AddNodalPTSEAck(const u_char * onid, ig_ptse * rPtse)
{
  if (_list.empty()) {
    ig_nodal_ptse_ack * lAhr = new ig_nodal_ptse_ack((u_char *)onid);
    lAhr->AddAck(rPtse->GetID(), rPtse->GetSN(),
		 rPtse->GetCS(), rPtse->GetTTL());
    // This uses the copy ctor ...
    AddNodalPTSEAck(lAhr);
    delete lAhr;
    return;
  }
  ig_nodal_ptse_ack * lAhr = Search(onid);
  if (lAhr) {
    lAhr->AddAck(rPtse->GetID(), rPtse->GetSN(),
		 rPtse->GetCS(), rPtse->GetTTL());
  } else {
    ig_nodal_ptse_ack * lAhr = new ig_nodal_ptse_ack((u_char *)onid);
    lAhr->AddAck(rPtse->GetID(), rPtse->GetSN(),
		 rPtse->GetCS(), rPtse->GetTTL());
    
    AddNodalPTSEAck(lAhr);
    delete lAhr;
  }
}

bool PTSEAckPkt::RemNodalPTSEAck(ig_nodal_ptse_ack *a)
{
  ig_nodal_ptse_ack * ptr, cont(*(a));

  list_item li;
  forall_items(li, _list) {
    ptr = _list.inf(li);
    if (ptr) {
      if (ptr->equals(cont)) {
        _list.del_item(_list.search(ptr));
        delete ptr;
        return true;
      }
    }
  }
  return false;
}

ig_nodal_ptse_ack * PTSEAckPkt::Search(const u_char * r_orig_node_id)
{
  ig_nodal_ptse_ack * lptrAhr;
  list_item li;
  forall_items(li, _list) {
    lptrAhr = _list.inf(li);
    if (lptrAhr->equals(r_orig_node_id))  
      return lptrAhr;
  }
  return 0;
}

const list<ig_nodal_ptse_ack *> & PTSEAckPkt::GetNodalPTSEAcks(void) const 
{ return _list; }

bool PTSEAckPkt::PrintSpecific(ostream & os) const
{
  ig_nodal_ptse_ack * ptr;
  if (!(_list.empty())) {
    list_item li;
    forall_items(li, _list) {
      if (ptr = _list.inf(li))
	os << *(ptr);
    }
    os << endl;
  }
  return true;
}

ostream & operator << (ostream & os, const PTSEAckPkt & pkt)
{
  pkt.PrintGeneral(os);
  pkt.PrintSpecific(os);
  return os;
}
