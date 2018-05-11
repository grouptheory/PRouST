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
// @file SwitchConfiguration.cc
// @author talmage
// @version $Id: SwitchConfiguration.cc,v 1.5 1999/02/19 21:22:58 marsh Exp $
//
// Purpose: Describes the configuration of a switch.
//

#ifndef LINT
static char const _SwitchConfiguration_cc_rcsid_[] =
"$Id: SwitchConfiguration.cc,v 1.5 1999/02/19 21:22:58 marsh Exp $";
#endif

#include "SwitchConfiguration.h"
#include <codec/pnni_ig/id.h>
#include <DS/containers/sortseq.h>
#include <DS/util/String.h>



// Owns all pointers
SwitchConfiguration::SwitchConfiguration(ds_String *name, 
					 int ports, 
					 NodeID *base,
					 sortseq<int, SwitchLevelInfo *> *levels, 
					 bool Omniscient, 
					 ds_String *acacPrefix, 
					 ds_String *acacLibrary, 
					 ds_String *logosPrefix, 
					 ds_String *logosLibrary, 
					 ds_String *aggregatorPrefix, 
					 ds_String *aggregatorLibrary):
  _name(name), _ports(ports), _baseNodeID(base), _levels(levels),
  _isOmniscient(Omniscient), 
  _acacPluginPrefix(acacPrefix), _acacPluginLibrary(acacLibrary),
  _logosPluginPrefix(logosPrefix),   _logosPluginLibrary(logosLibrary), 
  _aggregatorPluginPrefix(aggregatorPrefix), 
  _aggregatorPluginLibrary(aggregatorLibrary)
{
}


// Deletes all pointers
SwitchConfiguration::~SwitchConfiguration(void)
{
  delete _name;
  delete _baseNodeID;
  delete _acacPluginPrefix;
  delete _logosPluginPrefix;
  delete _aggregatorPluginPrefix;

  if (_levels != 0) {
    while (_levels->size() > 0) {
      dic_item ditem = _levels->first();

      SwitchLevelInfo *level = _levels->inf(ditem);

      delete level;

      _levels->del_item(ditem);
    }

    delete _levels;
  }
}

// caller owns the pointer
ds_String *SwitchConfiguration::Name(void)
{
  ds_String *answer = 0;
  if (_name != 0) answer = new ds_String(*_name);

  return answer;
}

int SwitchConfiguration::Ports(void)
{
  return _ports;
}

  // caller owns the pointer
NodeID *SwitchConfiguration::BaseNodeID(void)
{
  NodeID *answer = 0;
  if (_baseNodeID != 0) answer = new NodeID(*_baseNodeID);

  return answer;
}

int SwitchConfiguration::PhysicalLevel(void)
{
  int answer = -1;

  if (_levels->size() > 0) {
    seq_item sitem = _levels->max();

    if (sitem != 0) {
      SwitchLevelInfo *level_info = _levels->inf(sitem);
      answer = level_info->Level();
    }
  }
  return answer;
}

int SwitchConfiguration::NextLevelDown(int level)
{
  int answer = -1;
  if (_levels->size() != 0) {
    seq_item sitem = _levels->lookup(level);

    if (sitem != 0) {
      seq_item next_level_down = _levels->next(sitem);

      if (next_level_down != 0) {
	SwitchLevelInfo *level_info = _levels->inf(next_level_down);
      answer = level_info->Level();
      }
    }
  }

  return answer;
}

int SwitchConfiguration::NextLevelUp(int level)
{
  int answer = -1;
  if (_levels->size() != 0) {
    seq_item sitem = _levels->lookup(level);

    if (sitem != 0) {
      seq_item next_level_up = _levels->prev(sitem);
      
      if (next_level_up != 0) {
	SwitchLevelInfo *level_info = _levels->inf(next_level_up);
	answer = level_info->Level();
      }
    }
  }

  return answer;
}

int SwitchConfiguration::PeerGroupLeadershipPriority(int level)
{
  int answer = -1;
  if (_levels->size() != 0) {
    seq_item sitem = _levels->lookup(level);
 
    if (sitem != 0) {
      SwitchLevelInfo *level_info = _levels->inf(sitem);
      answer = level_info->PeerGroupLeadershipPriority();
    }
  }

  return answer;
}

// caller owns the pointer
NodeID *SwitchConfiguration::PreferredPeerGroupLeader(int level)
{
 NodeID *answer = 0;
  if (_levels->size() != 0) {
    seq_item sitem = _levels->lookup(level);
 
    if (sitem != 0) {
      SwitchLevelInfo *level_info = _levels->inf(sitem);
      answer = level_info->PreferredPeerGroupLeader();
    }
  }

  return answer;
}



// Owns all pointers
SwitchLevelInfo::SwitchLevelInfo(int level, int pgl_priority, 
				 NodeID *preferredPGL) :
  _level(level), _pgl_priority(pgl_priority), _preferred_pgl(preferredPGL)
{
}


// Deletes all pointers
SwitchLevelInfo::~SwitchLevelInfo(void)
{
  delete _preferred_pgl;
}

  
int SwitchLevelInfo::Level(void)
{
  return _level;
}


int SwitchLevelInfo::PeerGroupLeadershipPriority(void)
{
  return _pgl_priority;
}


// Caller owns the pointer
NodeID *SwitchLevelInfo::PreferredPeerGroupLeader(void)
{
  NodeID *answer = 0;

  if (_preferred_pgl != 0) answer = new NodeID(*_preferred_pgl);

  return answer;
}


bool SwitchConfiguration::Omniscient(void)
{
  return _isOmniscient;
}


// caller owns the pointer
ds_String *SwitchConfiguration::ACACPluginPrefix(void)
{
  ds_String *answer = 0;
  if (_name != 0) answer = new ds_String(*_acacPluginPrefix);

  return answer;
}


// caller owns the pointer
ds_String *SwitchConfiguration::ACACPluginLibrary(void)
{
  ds_String *answer = 0;
  if (_name != 0) answer = new ds_String(*_acacPluginLibrary);

  return answer;
}


// caller owns the pointer
ds_String *SwitchConfiguration::LogosPluginPrefix(void)
{
  ds_String *answer = 0;
  if (_name != 0) answer = new ds_String(*_logosPluginPrefix);

  return answer;
}


// caller owns the pointer
ds_String *SwitchConfiguration::LogosPluginLibrary(void)
{
  ds_String *answer = 0;
  if (_name != 0) answer = new ds_String(*_logosPluginLibrary);

  return answer;
}


// caller owns the pointer
ds_String *SwitchConfiguration::AggregatorPluginPrefix(void)
{
  ds_String *answer = 0;
  if (_name != 0) answer = new ds_String(*_aggregatorPluginPrefix);

  return answer;
}


// caller owns the pointer
ds_String *SwitchConfiguration::AggregatorPluginLibrary(void)
{
  ds_String *answer = 0;
  if (_name != 0) answer = new ds_String(*_aggregatorPluginLibrary);

  return answer;
}

