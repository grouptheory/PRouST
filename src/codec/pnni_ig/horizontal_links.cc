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
static char const _horizon_links_cc_rcsid_[] =
"$Id: horizontal_links.cc,v 1.9 1998/09/23 18:49:44 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/id.h>

ig_horizontal_links::ig_horizontal_links(sh_int flags, u_char *rnode, int rport, int lport, int atok) :
  InfoGroup(InfoGroup::ig_horizontal_links_id), _flags(flags), _remote_port_id(rport), 
  _local_port_id(lport), _aggregation_token(atok)
{
  if (rnode)
    bcopy(rnode, _remote_node_id, 22);
  else
    bzero(_remote_node_id, 22);
}

ig_horizontal_links::ig_horizontal_links(const ig_horizontal_links & rhs) 
  : InfoGroup(InfoGroup::ig_horizontal_links_id), _flags(rhs._flags), 
    _remote_port_id(rhs._remote_port_id), _local_port_id(rhs._local_port_id), 
    _aggregation_token(rhs._aggregation_token)
{
  bcopy(rhs._remote_node_id, _remote_node_id, 22);
  if (rhs._outgoing_raigs.empty() == false) {
    list_item li;
    forall_items(li, rhs._outgoing_raigs) {
      _outgoing_raigs.append( (ig_resrc_avail_info *)rhs._outgoing_raigs.inf(li)->copy() );
    }
  }
}

ig_horizontal_links::ig_horizontal_links(sh_int flags, NodeID *rnode, int rport, int lport, int atok) :
  InfoGroup(InfoGroup::ig_horizontal_links_id), _flags(flags), _remote_port_id(rport), 
  _local_port_id(lport), _aggregation_token(atok)
{
  if (rnode)
    bcopy((void *)rnode->GetNID(), _remote_node_id, 22);
  else
    bzero(_remote_node_id, 22);
}

ig_horizontal_links::~ig_horizontal_links() 
{ 
  // Deletes all of its raigs on death
  list_item li;
  forall_items(li, _outgoing_raigs) {
    delete _outgoing_raigs.inf(li);
  }
  _outgoing_raigs.clear();
}

InfoGroup * ig_horizontal_links::copy(void)
{
  return new ig_horizontal_links(*this);
}

void ig_horizontal_links::size(int & length)
{
  list_item li;
  length += HLINK_HEADER; // Fixed header 40

  forall_items(li, _outgoing_raigs)
  {
    (_outgoing_raigs.inf(li))->size(length);
  }
}

int ig_horizontal_links::equals(const InfoGroup * other) const
{
  int rval = InfoGroup::equals(other);

  if (!rval) {
    ig_horizontal_links * rhs = (ig_horizontal_links *)other;
    if ((_flags < rhs->_flags) ||
	((_flags == rhs->_flags) && memcmp(_remote_node_id, rhs->_remote_node_id, 22) < 0) ||
	((_flags == rhs->_flags) && !memcmp(_remote_node_id, rhs->_remote_node_id, 22) &&
	 (_remote_port_id < rhs->_remote_port_id)) ||
	((_flags == rhs->_flags) && !memcmp(_remote_node_id, rhs->_remote_node_id, 22) &&
	 (_remote_port_id == rhs->_remote_port_id) && (_local_port_id < rhs->_local_port_id)) ||
	((_flags == rhs->_flags) && !memcmp(_remote_node_id, rhs->_remote_node_id, 22) &&
	 (_remote_port_id == rhs->_remote_port_id) && (_local_port_id == rhs->_local_port_id) &&
	 (_aggregation_token < rhs->_aggregation_token)))
      return -1;
    else if ((_flags > rhs->_flags) ||
	     ((_flags == rhs->_flags) && memcmp(_remote_node_id, rhs->_remote_node_id, 22) > 0) ||
	     ((_flags == rhs->_flags) && !memcmp(_remote_node_id, rhs->_remote_node_id, 22) &&
	      (_remote_port_id > rhs->_remote_port_id)) ||
	     ((_flags == rhs->_flags) && !memcmp(_remote_node_id, rhs->_remote_node_id, 22) &&
	      (_remote_port_id == rhs->_remote_port_id) && (_local_port_id > rhs->_local_port_id)) ||
	     ((_flags == rhs->_flags) && !memcmp(_remote_node_id, rhs->_remote_node_id, 22) &&
	      (_remote_port_id == rhs->_remote_port_id) && (_local_port_id == rhs->_local_port_id) &&
	      (_aggregation_token > rhs->_aggregation_token)))
      return 1;
  }
  return rval;
}


u_char * ig_horizontal_links::encode(u_char * buffer, int & buflen)
{
  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  *temp++ = (_flags >> 8) & 0xFF;
  *temp++ = (_flags & 0xFF);
  buflen += 2;

  for (int i = 0; i < 22; i++)
    *temp++ = _remote_node_id[i];
  buflen += 22;

  *temp++ = (_remote_port_id >> 24) & 0xFF;
  *temp++ = (_remote_port_id >> 16) & 0xFF;
  *temp++ = (_remote_port_id >>  8) & 0xFF;
  *temp++ = (_remote_port_id & 0xFF);
  buflen += 4;

  *temp++ = (_local_port_id >> 24) & 0xFF;
  *temp++ = (_local_port_id >> 16) & 0xFF;
  *temp++ = (_local_port_id >>  8) & 0xFF;
  *temp++ = (_local_port_id & 0xFF);
  buflen += 4;

  *temp++ = (_aggregation_token >> 24) & 0xFF;
  *temp++ = (_aggregation_token >> 16) & 0xFF;
  *temp++ = (_aggregation_token >>  8) & 0xFF;
  *temp++ = (_aggregation_token & 0xFF);
  buflen += 4;

  list_item li;
  forall_items(li, _outgoing_raigs)
  {
    ig_resrc_avail_info * rptr = _outgoing_raigs.inf(li);
    int tlen = 0;
    temp = rptr->encode(temp, tlen);
    buflen += tlen;
  }

  encode_ig_header(buffer, buflen);
  return temp;
}
  
errmsg * ig_horizontal_links::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_horizontal_links_id ||
      len < 36)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);

  u_char * temp = &buffer[4];

  _flags  = (*temp++ << 8) & 0xFF00;
  _flags |= (*temp++ & 0xFF);

  for (int i = 0; i < 22; i++)
    _remote_node_id[i] = *temp++;

  _remote_port_id  = (*temp++ << 24) & 0xFF000000;
  _remote_port_id |= (*temp++ << 16) & 0x00FF0000;
  _remote_port_id |= (*temp++ <<  8) & 0x0000FF00;
  _remote_port_id |= (*temp++ & 0xFF);

  _local_port_id  = (*temp++ << 24) & 0xFF000000;
  _local_port_id |= (*temp++ << 16) & 0x00FF0000;
  _local_port_id |= (*temp++ <<  8) & 0x0000FF00;
  _local_port_id |= (*temp++ & 0xFF);

  _aggregation_token  = (*temp++ << 24) & 0xFF000000;
  _aggregation_token  = (*temp++ << 16) & 0x00FF0000;
  _aggregation_token  = (*temp++ <<  8) & 0x0000FF00;
  _aggregation_token  = (*temp++ & 0xFF);

  len -= 36;

  errmsg * rval = 0;
  ig_resrc_avail_info * raig;

  while (len > 0)
  {
    int type = (((temp[0] << 8) & 0xFF00) | (temp[1] & 0xFF));
    int tlen = (((temp[2] << 8) & 0xFF00) | (temp[3] & 0xFF));

    if (type != InfoGroup::ig_outgoing_resource_avail_id)
      return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

    raig = new ig_resrc_avail_info(ig_resrc_avail_info::outgoing);
    rval = raig->decode(temp);
    _outgoing_raigs.append(raig);

    if(rval) return rval;

    len  -= tlen + 4;
    temp += tlen + 4;
  }

  return rval;
}

bool ig_horizontal_links::PrintSpecific(ostream & os)
{
  os << hex << "    Flags::" << _flags << endl;
  os << dec << "    Remote Node ID::" << (int)(_remote_node_id[0]) << ":" << (int)(_remote_node_id[1]) << ":";
  for (int i = 2; i < 22; i++) {
    os.width(2); os.fill('0');
    os << hex << (int)(_remote_node_id[i]);
  }
  os << dec << endl;
  os << "    Remote Port ID::" << _remote_port_id << endl;
  os << "    Local Port ID::" << _local_port_id << endl;
  os << "    Aggregation Token::" << _aggregation_token << endl;
  return true;
}

bool ig_horizontal_links::ReadSpecific(istream & is)
{
  char buf[80], *temp = buf;

  is.getline(buf, 80);

  while (*temp && !isdigit(*temp))
    temp++;
  _flags = atoi(temp);

  is.getline(buf, 80);
  // How to extract the _remote_node_id ?

  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _remote_port_id = atoi(temp);

  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _local_port_id = atoi(temp);

  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _aggregation_token = atoi(temp);
  return true;
}

ostream & operator << (ostream & os, ig_horizontal_links & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

istream & operator >> (istream & is, ig_horizontal_links & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return is;
}

// The pointer is mine now, don't touch!
void ig_horizontal_links::AddRAIG(ig_resrc_avail_info * ptr)
{
  _outgoing_raigs.append(ptr);
}

const list<ig_resrc_avail_info *> & ig_horizontal_links::GetRAIGS(void) const 
{ return _outgoing_raigs; }

const NodeID * ig_horizontal_links::GetRemoteID(void) const 
{ return new NodeID(_remote_node_id); }

int ig_horizontal_links::GetRemoteID(NodeID & rval) const
{  
  NodeID tmp(_remote_node_id);
  rval = tmp;
  return 0;
}

const int      ig_horizontal_links::GetRemotePID(void) const
{ return _remote_port_id; }

const int      ig_horizontal_links::GetLocalPID(void) const 
{ return _local_port_id; }

const int      ig_horizontal_links::GetAggTok(void) const 
{ return _aggregation_token; }
