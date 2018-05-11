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
static char const _ptse_req_cc_rcsid_[] =
"$Id: ptse_req.cc,v 1.22 1999/01/06 00:38:35 battou Exp battou $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_pkt/ptse_req.h>
#include <codec/pnni_ig/req_ptse_header.h>

PTSEReqPkt::PTSEReqPkt() : PNNIPkt(PNNIPkt::ptse_req) { }

PTSEReqPkt::~PTSEReqPkt() 
{ 
  ig_req_ptse_header * ptr; 
  list_item li;
  forall_items(li, _list) {
    ptr = _list.inf(li);
    delete ptr; 
  }
  _list.clear();
}

PNNIPkt * PTSEReqPkt::copy(void) const
{
  PTSEReqPkt * rval = new PTSEReqPkt();
 
 list_item li;
  forall_items(li, _list) {
    rval->AddNodalPTSEReq((ig_req_ptse_header *) (_list.inf(li))->copy());
  }
  return rval;
}

void PTSEReqPkt::size(int  & length) 
{
  length = pkt_header_len; // PNNI Pkt header
  list_item li;
  forall_items(li, _list)
  {
    (_list.inf(li))->size(length);
  }
}

u_char * PTSEReqPkt::encode(u_char * buffer, int & len)
{
  ig_req_ptse_header * ptr;

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

errmsg * PTSEReqPkt::decode(const u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != PNNIPkt::ptse_req)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_pkt_header(buffer);
  len -= 8;

  u_char * temp = (u_char *)buffer + pkt_header_len;

  errmsg * rval = 0;
  ig_req_ptse_header * ptr;

  while (len > 0) {
    int ttype = (((temp[0] << 8) & 0xFF00) | (temp[1] & 0xFF));
    assert( ttype==513 );

    int tlen = (((temp[2] << 8) & 0xFF00) | (temp[3] & 0xFF));

    ptr = new ig_req_ptse_header;
    rval = ptr->decode(temp);
    AddNodalPTSEReq(ptr);

    if(rval) return rval;

    len  -= tlen + 4;
    temp += tlen + 4;
  }
  return rval;
}

void PTSEReqPkt::AddNodalPTSEReq(ig_req_ptse_header * p)
{
  if (!_list.search(p))
    _list.append(p);
}

void  PTSEReqPkt::AddMoreReq(PTSEReqPkt * sRpkt)
{
  ig_req_ptse_header * rRhr, *lRhr;
  ReqContainer *rRcont, *lRCont;
  list_item li;
  forall_items(li, sRpkt->_list) {
    rRhr = (sRpkt->_list).inf(li);
    list_item ri;
    ri = _list.search(rRhr);
    lRhr = _list.inf(ri);
    if (lRhr) {
      list_item lli;
      forall_items(lli, rRhr->GetReqSummary()) {
	rRcont = (rRhr->GetReqSummary()).inf(lli);
        if (((list<ReqContainer *>)lRhr->GetReqSummary()).search(rRcont)) 
	  continue;
	lRhr->AddReqContainer(rRcont);
	// rRhr->del_item(lli);
      }
    } else {
      AddNodalPTSEReq(rRhr);
      sRpkt->_list.del_item(li);
    }
  }
}


bool PTSEReqPkt::RemNodalPTSEReq(ig_req_ptse_header *p)
{
  ig_req_ptse_header * ptr, cont(*(p));
  list_item li;
  forall_items(li, _list) {
    ptr = _list.inf(li);
    if (ptr->equals(cont)) {
      _list.del_item(_list.search(ptr));
      delete ptr;
      return true;
    }
  }
  return false;
}

ig_req_ptse_header * PTSEReqPkt::Search(const u_char * rnid)
{
  ig_req_ptse_header * lRhr;
  list_item li;
  forall_items(li, _list) {
    lRhr = _list.inf(li);
    if (lRhr->equals(rnid)) {
      return lRhr;
    }
  }
  return 0;
}

const list<ig_req_ptse_header *> & PTSEReqPkt::GetHeaders(void) const
{ return _list; }

bool PTSEReqPkt::PrintSpecific(ostream & os) const
{
  ig_req_ptse_header * ptr;
  list_item li;
  forall_items(li, _list) {
    if (ptr = _list.inf(li))
      os << *(ptr);
  }
  os << endl;
  return true;
}

ostream & operator << (ostream & os, const PTSEReqPkt & pkt)
{
  pkt.PrintGeneral(os);
  pkt.PrintSpecific(os);
  return os;
}
