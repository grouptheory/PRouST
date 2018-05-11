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
static char const _req_ptse_header_cc_rcsid_[] =
"$Id: req_ptse_header.cc,v 1.22 1998/09/24 13:18:39 mountcas Exp battou $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/req_ptse_header.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/DBKey.h>
#include <codec/pnni_ig/id.h>

ig_req_ptse_header::ig_req_ptse_header(const u_char * orig) :
  InfoGroup(InfoGroup::ig_req_ptse_header_id), _ptse_request_count(0)
{
  if (orig)
    bcopy((char *)orig, _originating_node_id, 22);
  else
    bzero(_originating_node_id, 22);
}

ig_req_ptse_header::ig_req_ptse_header(const NodeID * orig) :
  InfoGroup(InfoGroup::ig_req_ptse_header_id), _ptse_request_count(0)
{
  if (orig)
    bcopy((void *)orig->GetNID(), _originating_node_id, 22);
  else
    bzero(_originating_node_id, 22);
}

ig_req_ptse_header::ig_req_ptse_header(const ig_req_ptse_header & rhs)
  : InfoGroup(InfoGroup::ig_req_ptse_header_id), _ptse_request_count(0)
{
  bcopy(rhs._originating_node_id, _originating_node_id, 22);
  if (rhs._list.empty() == false) {
    list_item li;
    forall_items(li, rhs._list) {
      ReqContainer * rc = rhs._list.inf(li);
      AddReqContainer(rc->_ptse_id, rc->_seq_num, rc->_rem_life_time);
    }
  }
}


ig_req_ptse_header::~ig_req_ptse_header()
{
  if (!_list.empty()) {
    list_item li;
    forall_items(li, _list) 
      delete _list.inf(li);

    _list.clear();
  }
}

InfoGroup * ig_req_ptse_header::copy(void)
{
  return new ig_req_ptse_header(*this);
}

void ig_req_ptse_header::size(int & length)
{
  length += REQUEST_HEADER; // 28
  length += _ptse_request_count*REQUEST_CONTENT; // For each summary count 4
}

u_char * ig_req_ptse_header::encode(u_char * buffer, int & buflen)
{
  int i;

  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  for (i = 0; i < 22; i++)
    *temp++ = _originating_node_id[i];
  buflen += 22;

  *temp++ = (_ptse_request_count >> 8) & 0xFF;
  *temp++ = (_ptse_request_count & 0xFF);
  buflen += 2;

  list_item li;
  forall_items(li, _list)
  {
    ReqContainer * lRcont = _list.inf(li);
    *temp++ = (lRcont->_ptse_id >> 24) & 0xFF;
    *temp++ = (lRcont->_ptse_id >> 16) & 0xFF;
    *temp++ = (lRcont->_ptse_id >>  8) & 0xFF;
    *temp++ = (lRcont->_ptse_id & 0xFF);
    buflen += 4;
  }

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_req_ptse_header::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF)), i;
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_req_ptse_header_id || len < 28)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);
  u_char * temp = &buffer[4];

  for (i = 0; i < 22; i++)
    _originating_node_id[i] = *temp++;
  len -= 22;

  _ptse_request_count  = (*temp++ << 8) & 0xFF00;
  _ptse_request_count |= (*temp++ & 0xFF);
  len -= 2;

  if (len < _ptse_request_count)
    return new errmsg(errmsg::incomplete_ig, errmsg::none, type);

  while (len > 0)
  {
    int ptse_id  = (*temp++ << 24) & 0xFF000000;
    ptse_id |= (*temp++ << 16) & 0x00FF0000;
    ptse_id |= (*temp++ <<  8) & 0x0000FF00;
    ptse_id |= (*temp++ & 0xFF);
    len -= 4;

    if (len > 0) {
      ReqContainer * rRcont = new ReqContainer(ptse_id);  // 1/13/99
      _list.append(rRcont);
    }
  }
  return 0;
}

void ig_req_ptse_header::RemReqContainer(ReqContainer * lRcont)
{
  list_item li;
  if (li = _list.search(lRcont))
    _list.del_item(li);
}

ReqContainer * ig_req_ptse_header::Search(int r_ptse_id)
{
  list_item li;
  forall_items(li, _list) {
    ReqContainer * lptrRcont = _list.inf(li);
    if (lptrRcont->_ptse_id == r_ptse_id) 
      return lptrRcont;
  }
  return 0L;
}
  
bool ig_req_ptse_header::PrintSpecific(ostream & os)
{
  NodeID nid(_originating_node_id); 
  os << "NodeID: " << nid << endl;
  os << "   PTSE Request Count::" << _ptse_request_count << endl;
  os << "   Requested IDs::";
  list_item li;
  forall_items(li, _list) {
    ReqContainer * lRcont = _list.inf(li);
    os << "Request id::" << lRcont->_ptse_id << endl;
    os << "Request ptse_seq::" << lRcont->_seq_num << endl;
    os << "Request ptse lifetime::" << lRcont->_rem_life_time << endl;
  }
  os << endl;
  return true;
}

bool ig_req_ptse_header::ReadSpecific(istream & is)
{
  char buf[80], *temp = buf;

  is.getline(buf, 80);
  while (*temp && !isdigit(*temp))
    temp++;
  for (int i = 0; i < 22; i++)
    is >> _originating_node_id[i];

  is.getline(buf, 80);
  while (*temp && !isdigit(*temp))
    temp++;
  _ptse_request_count = atoi(temp);

  is.getline(buf, 80);
  while (*temp && !isdigit(*temp))
    temp++;
  // _ptse_id = atoi(temp);
  return true;
}

ostream & operator << (ostream & os, ig_req_ptse_header & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return os;
}

istream & operator >> (istream & is, ig_req_ptse_header & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return is;
}

int compare(ig_req_ptse_header *const & lhs, ig_req_ptse_header *const & rhs)
{
 return(memcmp(lhs->_originating_node_id, rhs->_originating_node_id, 22));
}

bool ReqContainer::equals(const ReqContainer * ptr)
{
  return (_ptse_id == ptr->_ptse_id &&
	  _seq_num == ptr->_seq_num &&
	  _rem_life_time == ptr->_rem_life_time);
}

int compare(ReqContainer *const & lhs, ReqContainer *const & rhs)
{
  if((lhs->_ptse_id == rhs->_ptse_id) &&
     (lhs->_seq_num == rhs->_seq_num) &&
     (((lhs->_rem_life_time == DBKey::ExpiredAge)&&
      (rhs->_rem_life_time == DBKey::ExpiredAge)) ||
      ((lhs->_rem_life_time != DBKey::ExpiredAge) &&
       (rhs->_rem_life_time != DBKey::ExpiredAge)))) return 0;
  else if((lhs->_ptse_id < rhs->_ptse_id) ||
         ((lhs->_ptse_id == rhs->_ptse_id) &&
          (lhs->_seq_num < rhs->_seq_num)) ||
         ((lhs->_ptse_id == rhs->_ptse_id) &&
          (lhs->_seq_num == rhs->_seq_num) &&
          ((lhs->_rem_life_time != DBKey::ExpiredAge) &&
           (rhs->_rem_life_time == DBKey::ExpiredAge)))) return -1;
  return 1;

}


bool operator > (const ReqContainer & lhs, const ig_ptse & rhs){
  if(lhs._seq_num >  rhs.GetSN()) return true;
  if((lhs._seq_num == rhs.GetSN()) &&
     ((lhs._rem_life_time == DBKey::ExpiredAge) &&
     !(rhs.GetTTL() == DBKey::ExpiredAge))) return true;
    return false;
}

int ig_req_ptse_header::equals(ig_req_ptse_header & rhs)
{
  if(memcmp(_originating_node_id, rhs._originating_node_id, 22) == 0) return 1;
  return 0;
}

void ig_req_ptse_header::AddReqContainer(ReqContainer *lRcont)
{ 
  _list.append(lRcont); _ptse_request_count = _list.size(); 
}

void ig_req_ptse_header::AddReqContainer(int id, u_int seq, sh_int life)
{
  list_item li;
  ReqContainer * nReq = new ReqContainer(id, seq, life);
  if (li = _list.search(nReq)) {
    delete _list.inf(li);
    _list.del_item(li);
  }
  _list.append(nReq); 
  _ptse_request_count = _list.size(); 
}

bool  ig_req_ptse_header::equals(const u_char * r_orig_node_id)
{
  if (memcmp(_originating_node_id, r_orig_node_id, 22) == 0) 
    return 1;
  return 0; 
}

const NodeID * ig_req_ptse_header::GetOrigin(void) const 
{ 
  return new NodeID(_originating_node_id); 
}

const list<ReqContainer *> & ig_req_ptse_header::GetReqSummary(void) const 
{ return _list; }
