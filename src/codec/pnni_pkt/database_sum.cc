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
static char const _database_sum_cc_rcsid_[] =
"$Id: database_sum.cc,v 1.16 1999/01/06 00:36:17 battou Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_pkt/database_sum.h>
#include <codec/pnni_ig/nodal_ptse_summary.h>

DatabaseSumPkt::DatabaseSumPkt(int seq) : 
  PNNIPkt(PNNIPkt::database_sum), _flags(0), _ds_seq_num(seq) { }

DatabaseSumPkt::~DatabaseSumPkt() 
{ 
  list_item li;
  forall_items(li, _list) {
    ig_nodal_ptse_summary * ptr = _list.inf(li);
    delete ptr; 
  }
}

PNNIPkt * DatabaseSumPkt::copy(void) const
{
  DatabaseSumPkt * rval = new DatabaseSumPkt(_ds_seq_num);
  rval->_flags = _flags;
  
  list_item li;
  forall_items(li, _list) {
    ig_nodal_ptse_summary * ptr = _list.inf(li);
    rval->AddNodalPTSESum((ig_nodal_ptse_summary *) ptr->copy());
  }
  return rval;
}

void DatabaseSumPkt::size(int & length)
{
  length = FIXED_SUMMARY_HEADER; // Summary fixed header 16 bytes
  list_item li; 
  forall_items(li, _list)
  {
   (_list.inf(li))->size(length);
  }
}

u_char * DatabaseSumPkt::encode(u_char * buffer, int & len)
{
  ig_nodal_ptse_summary * ptr;

  len = 0;
  u_char * temp = buffer + pkt_header_len;

  *temp++ = (_flags >> 8) & 0xFF;
  *temp++ = (_flags & 0xFF);
  // reserved
  *temp++ = 0;
  *temp++ = 0;
  *temp++ = (_ds_seq_num >> 24) & 0xFF;
  *temp++ = (_ds_seq_num >> 16) & 0xFF;
  *temp++ = (_ds_seq_num >>  8) & 0xFF;
  *temp++ = (_ds_seq_num & 0xFF);
  len += 8;

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

errmsg * DatabaseSumPkt::decode(const u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != PNNIPkt::database_sum ||
      len < 12)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_pkt_header(buffer);
  len -= 8; // Four in header excepting type & length

  u_char * temp = (u_char *)buffer + pkt_header_len;

  _flags  = (*temp++ << 8) & 0xFF00;
  _flags |= (*temp++ & 0xFF);
  temp += 2; // reserved

  _ds_seq_num  = (*temp++ << 24) & 0xFF000000;
  _ds_seq_num |= (*temp++ << 16) & 0x00FF0000;
  _ds_seq_num |= (*temp++ <<  8) & 0x0000FF00;
  _ds_seq_num |= (*temp++ & 0xFF);

  len -= 8;

  errmsg * rval = 0;
  ig_nodal_ptse_summary * ptr;

  while (len > 0)
  {
    int tlen = (((temp[2] << 8) & 0xFF00) | (temp[3] & 0xFF));

    ptr = new ig_nodal_ptse_summary;
    rval = ptr->decode(temp);
    _list.append(ptr);

    if(rval) return rval; // Error Scenario

    len -= tlen + 4;
    temp += tlen + 4;
  }

  return rval;
}

void DatabaseSumPkt::AddNodalPTSESum(ig_nodal_ptse_summary * s)
{
  if (!_list.search(s))
    _list.append(s);
}

bool DatabaseSumPkt::RemNodalPTSESum(ig_nodal_ptse_summary * s)
{
  ig_nodal_ptse_summary cont(*s), *ptr;
  list_item li;
  forall_items(li, _list) {
    ptr = _list.inf(li);
    if (ptr->equals(cont)) {
      _list.del_item(li);
      delete ptr;
      return true;
    }
  }
  return false;
}

const list<ig_nodal_ptse_summary *> & DatabaseSumPkt::GetNodalSummaries(void) const 
{ return _list; }

void DatabaseSumPkt::SetBit(dsflags f) 
{ _flags |= f; } 

bool DatabaseSumPkt::IsSet(dsflags f) const
{ return (_flags & f); }

void DatabaseSumPkt::RemBit(dsflags f) 
{ _flags &= ~f; }

const int DatabaseSumPkt::GetSequenceNum(void) const 
{ return _ds_seq_num; }

void      DatabaseSumPkt::SetSequenceNum(int sn) 
{ _ds_seq_num = sn; }

bool DatabaseSumPkt::PrintSpecific(ostream & os) const
{
  ig_nodal_ptse_summary * ptr;
  os << "     Flags::" << hex << _flags << dec << endl;
  os << "     Sequence Number::" << _ds_seq_num << endl;
  list_item li;
  forall_items(li, _list) {
    ptr = _list.inf(li);
    if (ptr)
      os << *(ptr);
  }
  os << endl;
  return true;
}

ostream & operator << (ostream & os, const DatabaseSumPkt & pkt)
{
  pkt.PrintGeneral(os);
  pkt.PrintSpecific(os);
  return os;
}

