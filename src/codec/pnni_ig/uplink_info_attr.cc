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
static char const _uplink_info_cc_rcsid_[] =
"$Id: uplink_info_attr.cc,v 1.5 1998/09/24 13:27:58 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/pnni_ig/resrc_avail_info.h>

ig_uplink_info_attr::ig_uplink_info_attr(long seq_num) : 
  InfoGroup(InfoGroup::ig_uplink_info_attr_id), _seq_num(seq_num) { }

ig_uplink_info_attr::ig_uplink_info_attr(const ig_uplink_info_attr & rhs) : 
  InfoGroup(InfoGroup::ig_uplink_info_attr_id), _seq_num(rhs._seq_num) 
{ 
  if (rhs._list.empty() == false) {
    list_item li;
    forall_items(li, rhs._list) {
      AddIG( rhs._list.inf(li)->copy() );
    }
  }
}

ig_uplink_info_attr::~ig_uplink_info_attr() { }

InfoGroup * ig_uplink_info_attr::copy(void)
{
  return new ig_uplink_info_attr(*this);
}

void ig_uplink_info_attr::size(int & length)
{   
  length += ULIA_HEADER; // deafult ig header  8 
  list_item li;
  forall_items(li, _list)
  {
    (_list.inf(li))->size(length);
  }
}

u_char * ig_uplink_info_attr::encode(u_char * buffer, int & buflen)
{
  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  *temp++ = (_seq_num >> 24) & 0xFF;
  *temp++ = (_seq_num >> 16) & 0xFF;
  *temp++ = (_seq_num >>  8) & 0xFF; 
  *temp++ = (_seq_num & 0xFF);
  buflen  += 4;

  list_item li;
  forall_items(li, _list) {
    int tlen = 0;
    InfoGroup * ptr = _list.inf(li);
    temp = ptr->encode(temp, tlen);
    buflen += tlen;
  }
  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_uplink_info_attr::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_uplink_info_attr_id || len < 4)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);

  u_char * temp = buffer + ig_header_len;

  _seq_num  = (*temp++ << 24) & 0xFF000000;
  _seq_num |= (*temp++ << 16) & 0x00FF0000;
  _seq_num |= (*temp++ <<  8) & 0x0000FF00;
  _seq_num |= (*temp++ & 0xFF);
  len -= 4;

  errmsg * rval = 0;

  while(len > 0)
  {
     int type = (((temp[0] << 8 ) & 0xFF00) | ( temp[1] & 0xFF ));
     int tlen = (((temp[2] << 8 ) & 0xFF00) | (temp[3] & 0xFF));

     if(type == (InfoGroup::ig_outgoing_resource_avail_id))
     {
       InfoGroup * ig = (InfoGroup *)(new ig_resrc_avail_info(ig_resrc_avail_info::outgoing));
       rval = ig->decode(temp);
       _list.append(ig);
     }
     else
     {
       return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);
     }

    if(rval) return rval;

     len  -= tlen + 4;
     temp += tlen + 4;
  }

  return rval;
}

void  ig_uplink_info_attr::AddIG(InfoGroup * ptr)                 { _list.append(ptr); }
const list<InfoGroup *> & ig_uplink_info_attr::GetIGs(void) const { return _list; }
const long ig_uplink_info_attr::GetSequenceNum(void) const        { return _seq_num; }

bool ig_uplink_info_attr::PrintSpecific(ostream & os)
{
  os << "     Sequence Number::" << _seq_num << endl;
  list_item li;
  forall_items(li, _list) {
    InfoGroup * ptr = _list.inf(li);
    os << *ptr << endl;
  }
  return true;
}

bool ig_uplink_info_attr::ReadSpecific(istream & is)
{
  char buf[80];
  char * temp = buf;
  is.getline(buf, 80);

  while (*temp && !isdigit(*temp))
    temp++;

  if (temp && isdigit(*temp))
    _seq_num = atol(temp);
  else 
    return false;
  return true;
}

ostream & operator << (ostream & os, ig_uplink_info_attr & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}
