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
static char const _DBKey_cc_rcsid_[] =
"$Id: DBKey.cc,v 1.36 1999/01/14 15:54:00 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <iostream.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/DBKey.h>

// ---------------------- Key Container --------------------
DBKey::DBKey(InfoGroup::ig_id type, const u_char * orig, 
	     int id, int seq, const u_char * peer, short ttl) 
  : _seq_num(seq), _id(id), _type(type), _rem_life_time(ttl)
{
  if (orig)
    bcopy((unsigned char *)orig, _originator_id, 22);
  else
    bzero(_originator_id, 22);

  if (peer)
    bcopy((unsigned char *)peer, _peergroup_id, 14);
  else
    bzero(_peergroup_id, 14);
}

DBKey::DBKey(InfoGroup::ig_id type, const NodeID * orig,
	     int id, int seq, const PeerGroupID *pid, short ttl) 
  :  _seq_num(seq), _id(id), _type(type), _rem_life_time(ttl)
{
  if (orig && orig->GetNID())
    bcopy((unsigned char *)(orig->GetNID()), _originator_id, 22);
  else
    bzero(_originator_id, 22);

  if (pid)
    bcopy((unsigned char *)(pid->GetPGID()), _peergroup_id, 14);
  else
    bzero(_peergroup_id, 14);
}


//----------------------------------------------------------
DBKey::DBKey(const DBKey & other) : _seq_num(other._seq_num), 
  _id(other._id), _type(other._type), 
  _rem_life_time(other._rem_life_time)
{
  bcopy((unsigned char *)other._originator_id, _originator_id, 22);
  bcopy((unsigned char *)other._peergroup_id, _peergroup_id, 14);
}


//----------------------------------------------------------
DBKey::~DBKey( ) { }

//----------------------------------------------------------
const int DBKey::GetSN(void) const { return _seq_num; }

//----------------------------------------------------------
const int DBKey::GetID(void) const { return _id; }

//------------------------------------------------------------
const short int DBKey::GetTTL(void) const { return _rem_life_time; }

//----------------------------------------------------------
const u_char * DBKey::GetOrigin(void) const
{  return _originator_id;  }

//----------------------------------------------------------
const u_char * DBKey::GetPeer(void) const
{  return _peergroup_id;  }

//----------------------------------------------------------
bool DBKey::has_same_originator(const DBKey & other) const
{  
  return (!memcmp((char *)_originator_id, (char *)other._originator_id, 22));  
}

//----------------------------------------------------------
bool DBKey::has_same_originator(const NodeID * origin) const
{  
  NodeID nid(_originator_id);
  return (nid == *origin);
}

bool DBKey::has_same_originator(const u_char * origin) const
{  
  return (!memcmp((char *)_originator_id, (char *)origin, 22));  
}

//----------------------------------------------------------
// Equality depends ONLY on ID and NodeID of the database Keys
int operator == (const DBKey & lhs, const DBKey & rhs)
{
  return (lhs._id == rhs._id && 
	  !memcmp((void *)(lhs._originator_id), (void *)(rhs._originator_id), 22));
}

//----------------------------------------------------------
int operator < (const DBKey & lhs, const DBKey & rhs)
{
   return ( (lhs._seq_num < rhs._seq_num) ||
            ((lhs._seq_num == rhs._seq_num) &&
             ((lhs._rem_life_time != DBKey::ExpiredAge) &&
              (rhs._rem_life_time == DBKey::ExpiredAge))));
}

//----------------------------------------------------------
int operator > (const DBKey & lhs, const DBKey & rhs)
{
  if (lhs._id > rhs._id)
    return 1;
  if ((lhs._id == rhs._id) &&
      (memcmp((char *)(lhs._originator_id), (char *)(rhs._originator_id), 22) > 0))
    return 1;
  return 0;
}

//----------------------------------------------------------
DBKey DBKey::operator = (const DBKey him)
{
  _seq_num = him._seq_num;
  _type    = him._type;
  _id      = him._id;
  _rem_life_time = him._rem_life_time;
  bcopy(_originator_id, (u_char *)(him._originator_id), 22);
  return *this;
}

void DBKey::Print(char * out) const
{
  NodeID nid(_originator_id);

  sprintf(out, "DBKey (%d:%s) ID: %d, seq#: %d\n"
	       "\tOrigin: %s ", (int)_type,
	  IGTypeToName(_type), _id, _seq_num, 
	  nid.Print());
}

//----------------------------------------------------------
ostream & operator << (ostream & os, const DBKey & him)
{
  char buf[256];
  him.Print(buf);
  os << buf;
}

//----------------------------------------------------------
int compare(DBKey const & lhs, DBKey const & rhs)
{
 int nodecmp = memcmp((void *)(lhs._originator_id), (void *)(rhs._originator_id), 22);

 if (nodecmp != 0)
   return nodecmp;

 if ( lhs._id < rhs._id )
   return -1;
 if ( lhs._id > rhs._id )
   return 1;

 return 0;
}

int compare(DBKey *const & lhs, DBKey *const & rhs)
{
  return compare(*lhs, *rhs);
}

const char * IGTypeToName(InfoGroup::ig_id id)
{
  switch (id) {
    case InfoGroup::ig_aggregation_token_id:
      return "aggregation token"; break;
    case InfoGroup::ig_nodal_hierarchy_list_id:
      return "nodal hierarchy list"; break;
    case InfoGroup::ig_uplink_info_attr_id:
      return "uplink information attribute"; break;
    case InfoGroup::ig_lgn_horizontal_link_ext_id:
      return "LGN horizontal link extension"; break;
    case InfoGroup::ig_ptse_id:
      return "PTSE"; break;
    case InfoGroup::ig_nodal_state_params_id:
      return "nodal state parameters"; break;
    case InfoGroup::ig_nodal_info_group_id:
      return "nodal info group"; break;
    case InfoGroup::ig_outgoing_resource_avail_id:
      return "outgoing resource avail"; break;
    case InfoGroup::ig_incoming_resource_avail_id:
      return "incoming resource avail"; break;
    case InfoGroup::ig_optional_gcac_params_id:
      return "optional GCAC parameters"; break;
    case InfoGroup::ig_next_hi_level_binding_info_id:
      return "next higher level binding info"; break;
    case InfoGroup::ig_internal_reachable_atm_addr_id:
      return "internal reachable ATM address"; break;
    case InfoGroup::ig_exterior_reachable_atm_addr_id:
      return "exterior reachable ATM address"; break;
    case InfoGroup::ig_horizontal_links_id:
      return "horizontal links"; break;
    case InfoGroup::ig_uplinks_id:
      return "uplinks"; break;
    case InfoGroup::ig_transit_net_id_id:
      return "transit network ID"; break;
    case InfoGroup::ig_nodal_ptse_ack_id: 
      return "nodal PTSE ack"; break;
    case InfoGroup::ig_nodal_ptse_summary_id:
      return "nodal PTSE summary"; break;
    case InfoGroup::ig_req_ptse_header_id:
      return "request PTSE header"; break;
    case InfoGroup::ig_system_capabilities_id:
      return "system capabilities"; break;
    case InfoGroup::ig_unknown_id:
      return "unknown IG"; break;
  default:
      return "unknown IG"; break;
  }
  return "unknown IG"; 
}

int DBKey::GetOrigin(u_char * rval) const
{
  if (rval) {
    memcpy(rval, _originator_id, 22);
    return 0;
  }
  return -1;
}

int DBKey::GetPeer(u_char * rval) const
{
  if (rval) {
    memcpy(rval, _peergroup_id, 22);
    return 0;
  }
  return -1;
}

int DBKey::GetOrigin(NodeID & rval) const
{
  NodeID tmp(_originator_id);
  rval = tmp;
  return 0;
}

int DBKey::GetPeer(PeerID & rval) const
{
  PeerID tmp(_peergroup_id);
  rval = tmp;
  return 0;
}

int DBKey::GetSeqNum(int & rval) const
{
  rval = _seq_num;
  return 0;
}

int DBKey::GetID(int & rval) const
{
  rval = _id;
  return 0;
}

int DBKey::GetTimeToLive(int & rval) const
{
  rval = _rem_life_time;
  return 0;
}

int DBKey::GetType(InfoGroup::ig_id & rval) const
{
  rval = _type;
  return 0;
}
