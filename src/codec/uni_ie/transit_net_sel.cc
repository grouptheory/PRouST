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
static char const _transit_net_sel_cc_rcsid_[] =
"$Id: transit_net_sel.cc,v 1.7 1998/08/06 04:03:20 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/transit_net_sel.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>

extern "C" {
#include <memory.h>
};

/*
 * network_id_type, network_id_plan and network_id must match.
 */
ie_transit_net_sel::ie_transit_net_sel(char *net_id) : 
  InfoElem(ie_transit_net_sel_id),
  _network_id_type(national_network),
  _network_id_plan(carrier_id_code),
  _network_id_len(NET_ID_LEN)
{
  if (net_id != 0)
    for (int i  = 0; i < NET_ID_LEN; i++)
      _network_id[i] = net_id[i];
  else memset(_network_id, 0, NET_ID_LEN);
}

ie_transit_net_sel::ie_transit_net_sel(const ie_transit_net_sel & rhs) :
  InfoElem(ie_transit_net_sel_id), _network_id_type(rhs._network_id_type),
  _network_id_plan(rhs._network_id_plan), _network_id_len(rhs._network_id_len)
{
  for (int i = 0; i < NET_ID_LEN; i++)
    _network_id[i] = rhs._network_id[i];
}

ie_transit_net_sel::~ie_transit_net_sel() {}


InfoElem* ie_transit_net_sel::copy(void) const
{ return new ie_transit_net_sel((char*)_network_id);}

enum ie_transit_net_sel::network_id_type ie_transit_net_sel::get_net_id_type(void) 
{ return _network_id_type; }

int ie_transit_net_sel::set_net_id_type(network_id_type net_id_type)
    {
      switch(net_id_type)
	{
	case national_network:
	  _network_id_type = net_id_type;
	  return(0);
	  break;
	default:
	  return(-1);
	}
    }
  
int ie_transit_net_sel::get_network_id_len(void) {return _network_id_len;}
  
int ie_transit_net_sel::equals(const InfoElem* him) const
{
  return him->equals(this);
}

int ie_transit_net_sel::equals(const ie_transit_net_sel* himptr) const
{
  if(_network_id_type != himptr->_network_id_type ||
     _network_id_plan != himptr->_network_id_plan ||
     _network_id_len != himptr->_network_id_len)
    return(0);
  for(int i = 0; i < _network_id_len; i++)
    if(himptr->_network_id[i] != _network_id[i])
      return(0);
  return(1);
}

int ie_transit_net_sel::operator == (const ie_transit_net_sel & rie) const
{
  return equals(&rie);
}


int ie_transit_net_sel::Length()
{
  return (ie_header_len + _network_id_len + 1);
}


int ie_transit_net_sel::encode(u_char * buffer)
{
  u_char * temp;
  int buflen = 0;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);	
  put_coding(buffer,_coding);
  put_8(temp,buflen,(0x80 | _network_id_type  | _network_id_plan)); 
  for (int i = 0; i < _network_id_len; i++)
    put_8(temp,buflen,_network_id[i]);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

/*
 * This information element has length 2 bytes
 */
InfoElem::ie_status ie_transit_net_sel::decode(u_char * buffer, int & id)
{
         id = buffer[0];
  short len = get_len(buffer); 

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 5)
    return (InfoElem::incomplete);
  buffer += ie_header_len;
  if ((*buffer & 0x80) != 0x80)
    return (InfoElem::invalid_contents);
  int net_id_type = (*buffer & 0x70);
  switch(net_id_type) {
    case national_network:
      _network_id_type = (ie_transit_net_sel::network_id_type)net_id_type;
      break;
    default:
      return (InfoElem::invalid_contents);
      break;
  }
  int net_id_plan = (*buffer & 0x0f);
  switch(net_id_plan) {
    case carrier_id_code:
      _network_id_plan = (ie_transit_net_sel::network_id_plan)net_id_plan;
      return (InfoElem::ok);
      break;
    default:
      return (InfoElem::invalid_contents);
      break;
  }
  return (InfoElem::ok);
}

char ie_transit_net_sel::PrintSpecific(ostream & os)
{
  os << endl << "   network id type::";
  switch (_network_id_type) {
  case national_network: os << "national network " << endl; break;
  default: os << "unknown " << endl; break;
  }
  os << "   network id plan::";
  switch (_network_id_plan) {
  case carrier_id_code: os << "carrier id code " << endl; break;
  default: os << "unknown " << endl; break;
  }
  if (_network_id_len > 0) {
    os << "   network id::";
    special_print((u_char *)_network_id, _network_id_len, os);
  }
  return 0;
}

ostream & operator << (ostream & os, ie_transit_net_sel & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}

