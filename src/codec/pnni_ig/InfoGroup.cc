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
#ifndef __IG_C__
#define __IG_C__

#ifndef LINT
static char const _ig_cc_rcsid_[] =
"$Id: InfoGroup.cc,v 1.14 1999/01/28 16:24:15 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/InfoGroup.h>

// Length of IG header in bytes - note: Packet Header is 8 bytes
const int InfoGroup::ig_header_len = 4;

InfoGroup::InfoGroup(InfoGroup::ig_id id) : _id(id), _length(0) { }

InfoGroup::~InfoGroup() { _id = ig_unknown_id; _length = -1; }

InfoGroup::ig_id   InfoGroup::GetId(void) const { return _id; }

void    InfoGroup::SetId(InfoGroup::ig_id id) { _id = id; }

int     InfoGroup::GetLength(void) const { return _length; }

void    InfoGroup::SetLength(int length) { _length = length; }

bool is_ig_valid(enum InfoGroup::ig_id x)
{
    return ((igid_to_index(x) != -1));
}

void Print_IG_name(InfoGroup::ig_ix ix, ostream& os)
{
  switch (ix) {
    case InfoGroup::ig_aggregation_token_ix:
      os << "ig_aggregation_token";
      break;
    case InfoGroup::ig_nodal_hierarchy_list_ix:
      os << "ig_nodal_hierarchy_list";
      break;
    case InfoGroup::ig_uplink_info_attr_ix:
      os << "ig_uplink_info_attribute";
      break;
    case InfoGroup::ig_lgn_horizontal_link_ext_ix:
      os << "ig_LGN_horizontal_link_extension";
      break;
    case InfoGroup::ig_ptse_ix:
      os << "ig_PTSE";
      break;
    case InfoGroup::ig_nodal_state_params_ix:
      os << "ig_nodal_state_paramters";
      break;
    case InfoGroup::ig_nodal_info_group_ix:
      os << "ig_nodal_information_group";
      break;
    case InfoGroup::ig_outgoing_resource_avail_ix:
      os << "ig_outgoing_resource_availability";
      break;
    case InfoGroup::ig_incoming_resource_avail_ix:
      os << "ig_incoming_resource_availability";
      break;
    case InfoGroup::ig_optional_gcac_params_ix:
      os << "ig_optional_GCAC_parameters";
      break;
    case InfoGroup::ig_next_hi_level_binding_info_ix:
      os << "ig_next_higher_level_binding_information";
      break;
    case InfoGroup::ig_internal_reachable_atm_addr_ix:
      os << "ig_internal_reachable_atm_address";
      break;
    case InfoGroup::ig_exterior_reachable_atm_addr_ix:
      os << "ig_exterior_reachable_atm_address";
      break;
    case InfoGroup::ig_horizontal_links_ix:
      os << "ig_horizontal_links";
      break;
    case InfoGroup::ig_uplinks_ix:
      os << "ig_uplinks";
      break;
    case InfoGroup::ig_transit_net_id_ix:
      os << "ig_transit_network_ID";
      break;
    case InfoGroup::ig_nodal_ptse_ack_ix:
      os << "ig_nodal_PTSE_ack";
      break;
    case InfoGroup::ig_nodal_ptse_summary_ix:
      os << "ig_nodal_PTSE_summaries";
      break;
    case InfoGroup::ig_req_ptse_header_ix:
      os << "ig_requested_PTSE_header";
      break;
    case InfoGroup::ig_system_capabilities_ix:
      os << "ig_system_capabilities";
      break;
    case InfoGroup::ig_unknown_ix:
    default:
      os << "ig_unknown";
      break;
    }
}

const char * IG2Name(InfoGroup::ig_id id)
{
  switch (id) {
    case InfoGroup::ig_aggregation_token_id:
      return "ig_aggregation_token";
      break;
    case InfoGroup::ig_nodal_hierarchy_list_id:
      return "ig_nodal_hierarchy_list";
      break;
    case InfoGroup::ig_uplink_info_attr_id:
      return "ig_uplink_info_attribute";
      break;
    case InfoGroup::ig_lgn_horizontal_link_ext_id:
      return "ig_LGN_horizontal_link_extension";
      break;
    case InfoGroup::ig_ptse_id:
      return "ig_PTSE";
      break;
    case InfoGroup::ig_nodal_state_params_id:
      return "ig_nodal_state_paramters";
      break;
    case InfoGroup::ig_nodal_info_group_id:
      return "ig_nodal_information_group";
      break;
    case InfoGroup::ig_outgoing_resource_avail_id:
      return "ig_outgoing_resource_availability";
      break;
    case InfoGroup::ig_incoming_resource_avail_id:
      return "ig_incoming_resource_availability";
      break;
    case InfoGroup::ig_optional_gcac_params_id:
      return "ig_optional_GCAC_parameters";
      break;
    case InfoGroup::ig_next_hi_level_binding_info_id:
      return "ig_next_higher_level_binding_information";
      break;
    case InfoGroup::ig_internal_reachable_atm_addr_id:
      return "ig_internal_reachable_atm_address";
      break;
    case InfoGroup::ig_exterior_reachable_atm_addr_id:
      return "ig_exterior_reachable_atm_address";
      break;
    case InfoGroup::ig_horizontal_links_id:
      return "ig_horizontal_links";
      break;
    case InfoGroup::ig_uplinks_id:
      return "ig_uplinks";
      break;
    case InfoGroup::ig_transit_net_id_id:
      return "ig_transit_network_ID";
      break;
    case InfoGroup::ig_nodal_ptse_ack_id:
      return "ig_nodal_PTSE_ack";
      break;
    case InfoGroup::ig_nodal_ptse_summary_id:
      return "ig_nodal_PTSE_summaries";
      break;
    case InfoGroup::ig_req_ptse_header_id:
      return "ig_requested_PTSE_header";
      break;
    case InfoGroup::ig_system_capabilities_id:
      return "ig_system_capabilities";
      break;
    case InfoGroup::ig_unknown_id:
    default:
      return "ig_unknown";
      break;
  }
}

enum InfoGroup::ig_id index_to_igid(enum InfoGroup::ig_ix ix)
{
  switch (ix) {
    case InfoGroup::ig_aggregation_token_ix:
      return InfoGroup::ig_aggregation_token_id;
      break;
    case InfoGroup::ig_nodal_hierarchy_list_ix:
      return InfoGroup::ig_nodal_hierarchy_list_id;
      break;
    case InfoGroup::ig_uplink_info_attr_ix:
      return InfoGroup::ig_uplink_info_attr_id;
      break;
    case InfoGroup::ig_lgn_horizontal_link_ext_ix:
      return InfoGroup::ig_lgn_horizontal_link_ext_id;
      break;
    case InfoGroup::ig_ptse_ix:
      return InfoGroup::ig_ptse_id;
      break;
    case InfoGroup::ig_nodal_state_params_ix:
      return InfoGroup::ig_nodal_state_params_id;
      break;
    case InfoGroup::ig_nodal_info_group_ix:
      return InfoGroup::ig_nodal_info_group_id;
      break;
    case InfoGroup::ig_outgoing_resource_avail_ix:
      return InfoGroup::ig_outgoing_resource_avail_id;
      break;
    case InfoGroup::ig_incoming_resource_avail_ix:
      return InfoGroup::ig_incoming_resource_avail_id;
      break;
    case InfoGroup::ig_optional_gcac_params_ix:
      return InfoGroup::ig_optional_gcac_params_id;
      break;
    case InfoGroup::ig_next_hi_level_binding_info_ix:
      return InfoGroup::ig_next_hi_level_binding_info_id;
      break;
    case InfoGroup::ig_internal_reachable_atm_addr_ix:
      return InfoGroup::ig_internal_reachable_atm_addr_id;
      break;
    case InfoGroup::ig_exterior_reachable_atm_addr_ix:
      return InfoGroup::ig_exterior_reachable_atm_addr_id;
      break;
    case InfoGroup::ig_horizontal_links_ix:
      return InfoGroup::ig_horizontal_links_id;
      break;
    case InfoGroup::ig_uplinks_ix:
      return InfoGroup::ig_uplinks_id;
      break;
    case InfoGroup::ig_transit_net_id_ix:
      return InfoGroup::ig_transit_net_id_id;
      break;
    case InfoGroup::ig_nodal_ptse_ack_ix:
      return InfoGroup::ig_nodal_ptse_ack_id;
      break;
    case InfoGroup::ig_nodal_ptse_summary_ix:
      return InfoGroup::ig_nodal_ptse_summary_id;
      break;
    case InfoGroup::ig_req_ptse_header_ix:
      return InfoGroup::ig_req_ptse_header_id;
      break;
    case InfoGroup::ig_system_capabilities_ix:
      return InfoGroup::ig_system_capabilities_id;
      break;
    case InfoGroup::ig_unknown_ix:
    default:
      return InfoGroup::ig_unknown_id;
      break;
    }
}

enum InfoGroup::ig_ix igid_to_index(enum InfoGroup::ig_id id)
{
  switch (id) {
    case InfoGroup::ig_aggregation_token_id:
      return InfoGroup::ig_aggregation_token_ix;
      break;
    case InfoGroup::ig_nodal_hierarchy_list_id:
      return InfoGroup::ig_nodal_hierarchy_list_ix;
      break;
    case InfoGroup::ig_uplink_info_attr_id:
      return InfoGroup::ig_uplink_info_attr_ix;
      break;
    case InfoGroup::ig_lgn_horizontal_link_ext_id:
      return InfoGroup::ig_lgn_horizontal_link_ext_ix;
      break;
    case InfoGroup::ig_ptse_id:
      return InfoGroup::ig_ptse_ix;
      break;
    case InfoGroup::ig_nodal_state_params_id:
      return InfoGroup::ig_nodal_state_params_ix;
      break;
    case InfoGroup::ig_nodal_info_group_id:
      return InfoGroup::ig_nodal_info_group_ix;
      break;
    case InfoGroup::ig_outgoing_resource_avail_id:
      return InfoGroup::ig_outgoing_resource_avail_ix;
      break;
    case InfoGroup::ig_incoming_resource_avail_id:
      return InfoGroup::ig_incoming_resource_avail_ix;
      break;
    case InfoGroup::ig_optional_gcac_params_id:
      return InfoGroup::ig_optional_gcac_params_ix;
      break;
    case InfoGroup::ig_next_hi_level_binding_info_id:
      return InfoGroup::ig_next_hi_level_binding_info_ix;
      break;
    case InfoGroup::ig_internal_reachable_atm_addr_id:
      return InfoGroup::ig_internal_reachable_atm_addr_ix;
      break;
    case InfoGroup::ig_exterior_reachable_atm_addr_id:
      return InfoGroup::ig_exterior_reachable_atm_addr_ix;
      break;
    case InfoGroup::ig_horizontal_links_id:
      return InfoGroup::ig_horizontal_links_ix;
      break;
    case InfoGroup::ig_uplinks_id:
      return InfoGroup::ig_uplinks_ix;
      break;
    case InfoGroup::ig_transit_net_id_id:
      return InfoGroup::ig_transit_net_id_ix;
      break;
    case InfoGroup::ig_nodal_ptse_ack_id:
      return InfoGroup::ig_nodal_ptse_ack_ix;
      break;
    case InfoGroup::ig_nodal_ptse_summary_id:
      return InfoGroup::ig_nodal_ptse_summary_ix;
      break;
    case InfoGroup::ig_req_ptse_header_id:
      return InfoGroup::ig_req_ptse_header_ix;
      break;
    case InfoGroup::ig_system_capabilities_id:
      return InfoGroup::ig_system_capabilities_ix;
      break;
    case InfoGroup::ig_unknown_id:
    default:
      return InfoGroup::ig_unknown_ix;
      break;
    }
}

void InfoGroup::encode_ig_header(u_char * buffer, int & len)
{
  u_char * temp = buffer;
  *temp++ = (_id >> 8) & 0xFF;
  *temp++ = (_id & 0xFF);    
  len += 4; 
  *temp++ = (len >> 8) & 0xFF;
  *temp++ = (len & 0xFF);
}

errmsg * InfoGroup::decode_ig_header(u_char * buffer)
{
  errmsg * rval = 0;

  _length = (((buffer[2] >> 8) & 0xFF00) | (buffer[3] & 0xFF));

  return rval;
}

bool InfoGroup::PrintGeneral(ostream& os)
{
  os << "! ";

  switch (_id) {
    case ig_aggregation_token_id:
      os << "ig_aggregation_token: ";
      break;
    case ig_nodal_hierarchy_list_id:
      os << "ig_nodal_hierarchy_list: ";
      break;
    case ig_uplink_info_attr_id:
      os << "ig_uplink_info_attr: ";
      break;
    case ig_lgn_horizontal_link_ext_id:
      os << "ig_lgn_horizontal_link_ext: ";
      break;
    case ig_ptse_id:
      os << "ig_ptse: ";
      break;
    case ig_nodal_state_params_id:
      os << "ig_nodal_state_params: ";
      break;
    case ig_nodal_info_group_id:
      os << "ig_nodal_info_group: ";
      break;
    case ig_outgoing_resource_avail_id:
      os << "ig_outgoing_resource_avail: ";
      break;
    case ig_incoming_resource_avail_id:
      os << "ig_incoming_resource_avail: ";
      break;
    case ig_optional_gcac_params_id:
      os << "ig_optional_gcac_params: ";
      break;
    case ig_next_hi_level_binding_info_id:
      os << "ig_next_hi_level_binding_info: ";
      break;
    case ig_internal_reachable_atm_addr_id:
      os << "ig_internal_reachable_atm_addr: ";
      break;
    case ig_exterior_reachable_atm_addr_id:
      os << "ig_exterior_reachable_atm_addr: ";
      break;
    case ig_horizontal_links_id:
      os << "ig_horizontal_links: ";
      break;
    case ig_uplinks_id:
      os << "ig_uplinks: ";
      break;
    case ig_transit_net_id_id:
      os << "ig_transit_net_id: ";
      break;
    case ig_nodal_ptse_ack_id:
      os << "ig_nodal_ptse_ack: ";
      break;
    case ig_nodal_ptse_summary_id:
      os << "ig_nodal_ptse_summary: ";
      break;
    case ig_req_ptse_header_id:
      os << "ig_req_ptse_header: ";
      break;
    case ig_system_capabilities_id:
      os << "ig_system_capabilities: ";
      break;
    case ig_unknown_id:
    default:
      os << "ig_unknown: ";
      break;
  }
  os << endl;
  return true;
}

ostream & operator << (ostream & os, InfoGroup & x) 
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return(os);
}

istream & operator >> (istream & is, InfoGroup & ig)
{
  ig.ReadGeneral(is);
  return(is);
}

bool InfoGroup::ReadGeneral(istream & is)
{
  const int TEMPSTR = 255;
  char  tmp[TEMPSTR], ch;
  int   i = 0;

  while ((is.get(ch)) && (ch == '!')) 
    i++;

  while ((is.get(ch)) && (ch != '!'))
    tmp[i++] = ch;
  tmp[i]=0;

  while((is.get(ch))&&(ch=='!')) i++;

  if (!strcmp("ig_aggregation_token: ",tmp)) this->_id = ig_aggregation_token_id;
  else if (!strcmp("ig_nodal_hierarchy_list: ", tmp)) this->_id = ig_nodal_hierarchy_list_id;
  else if (!strcmp("ig_uplink_info_attr: ", tmp)) this->_id = ig_uplink_info_attr_id;
  else if (!strcmp("ig_lgn_horizontal_link_ext: ", tmp)) this->_id = ig_lgn_horizontal_link_ext_id;
  else if (!strcmp("ig_ptse: ", tmp)) this->_id = ig_ptse_id;
  else if (!strcmp("ig_nodal_state_params: ", tmp)) this->_id = ig_nodal_state_params_id;
  else if (!strcmp("ig_nodal_info_group: ", tmp)) this->_id = ig_nodal_info_group_id;
  else if (!strcmp("ig_outgoing_resource_avail: ", tmp)) this->_id = ig_outgoing_resource_avail_id;
  else if (!strcmp("ig_incoming_resource_avail: ", tmp)) this->_id = ig_incoming_resource_avail_id;
  else if (!strcmp("ig_optional_gcac_params: ", tmp)) this->_id = ig_optional_gcac_params_id;
  else if (!strcmp("ig_next_hi_level_binding_info: ", tmp)) this->_id = ig_next_hi_level_binding_info_id;
  else if (!strcmp("ig_internal_reachable_atm_addr: ", tmp)) this->_id = ig_internal_reachable_atm_addr_id;
  else if (!strcmp("ig_exterior_reachable_atm_addr: ", tmp)) this->_id = ig_exterior_reachable_atm_addr_id;
  else if (!strcmp("ig_horizontal_links: ", tmp)) this->_id = ig_horizontal_links_id;
  else if (!strcmp("ig_uplinks: ", tmp)) this->_id = ig_uplinks_id;
  else if (!strcmp("ig_transit_net_id: ", tmp)) this->_id = ig_transit_net_id_id;
  else if (!strcmp("ig_nodal_ptse_ack: ", tmp)) this->_id = ig_nodal_ptse_ack_id;
  else if (!strcmp("ig_nodal_ptse_summary: ", tmp)) this->_id = ig_nodal_ptse_summary_id;
  else if (!strcmp("ig_req_ptse_header: ", tmp)) this->_id = ig_req_ptse_header_id;
  else return false;

  if (this->_id) this->ReadSpecific(is);
  
  return (true);
}

bool InfoGroup::ReadSpecific(istream& is)
{  return false;  }

void HexDump(const u_char * buffer, int len, ostream & os, bool nid)
{
  const u_char * buf = buffer;
  int   i = 0;

  if (nid && buf) {
    os << (int)buf[i++] << ":";
    os << (int)buf[i++] << ":";
  }

  if (buf) {
    for (; i < len; i++) {
      int val = (int)(buf[i]);
      if (val < 16) 
	os << "0";
      os << hex << (int)val;
      // if ((i   ) == 1) os << " ";
    }
  } else
    os << "Invalid message.";
  os << dec << endl;
}

int compare(InfoGroup * const & lhs, InfoGroup * const & rhs)
{
  if ((long)lhs < (long)rhs)
    return -1;
  if ((long)lhs > (long)rhs)
    return 1;
  return 0;
}

int InfoGroup::equals(const InfoGroup * other) const
{
  if ((_id < other->_id) ||
      ((_id == other->_id) && (_length < other->_length)))
    return -1;
  if ((_id > other->_id) ||
      ((_id == other->_id) && (_length > other->_length)))
    return 1;
  return 0;
}

InfoGroup::ig_id name2ig(const char * name)
{
  if ( !strcasecmp( name, "aggregation token" ) )
    return InfoGroup::ig_aggregation_token_id;
  if ( !strcasecmp( name, "nodal hierarchy list" ) )
    return InfoGroup::ig_nodal_hierarchy_list_id;
  if ( !strcasecmp( name, "uplink info" ) )
    return InfoGroup::ig_uplink_info_attr_id;
  if ( !strcasecmp( name, "lgn horiztonal link ext" ) )
    return InfoGroup::ig_lgn_horizontal_link_ext_id;
  if ( !strcasecmp( name, "ptse" ) )
    return InfoGroup::ig_ptse_id;
  if ( !strcasecmp( name, "nodal state params" ) )
    return InfoGroup::ig_nodal_state_params_id;
  if ( !strcasecmp( name, "nodal info group" ) )
    return InfoGroup::ig_nodal_info_group_id;
  if ( !strcasecmp( name, "outgoing resource avail" ) )
    return InfoGroup::ig_outgoing_resource_avail_id;
  if ( !strcasecmp( name, "incoming resource avail" ) )
    return InfoGroup::ig_incoming_resource_avail_id;
  if ( !strcasecmp( name, "optional gacac params" ) )
    return InfoGroup::ig_optional_gcac_params_id;
  if ( !strcasecmp( name, "next higher level binding info" ) )
    return InfoGroup::ig_next_hi_level_binding_info_id;
  if ( !strcasecmp( name, "internal reachable atm addr" ) )
    return InfoGroup::ig_internal_reachable_atm_addr_id;
  if ( !strcasecmp( name, "exterior reachable atm addr" ) )
    return InfoGroup::ig_exterior_reachable_atm_addr_id;
  if ( !strcasecmp( name, "horiztonal link" ) )
    return InfoGroup::ig_horizontal_links_id;
  if ( !strcasecmp( name, "uplink" ) )
    return InfoGroup::ig_uplinks_id;
  if ( !strcasecmp( name, "transit net id" ) )
    return InfoGroup::ig_transit_net_id_id;
  if ( !strcasecmp( name, "nodal ptse ack" ) )
    return InfoGroup::ig_nodal_ptse_ack_id;
  if ( !strcasecmp( name, "nodal ptse summary" ) )
    return InfoGroup::ig_nodal_ptse_summary_id;
  if ( !strcasecmp( name, "req ptse header" ) )
    return InfoGroup::ig_req_ptse_header_id;
  if ( !strcasecmp( name, "system caps" ) )
    return InfoGroup::ig_system_capabilities_id;
  return InfoGroup::ig_unknown_id;
}

#endif
