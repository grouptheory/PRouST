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
// @file SwitchConfiguration.h
// @author talmage
// @version $Id: SwitchConfiguration.h,v 1.4 1999/02/19 21:22:58 marsh Exp $
//
// Purpose: Describes the configuration of a switch.
//
#ifndef __SWITCHCONFIGURATION_H__
#define __SWITCHCONFIGURATION_H__

#ifndef LINT
static char const _SwitchConfiguration_h_rcsid_[] =
"$Id: SwitchConfiguration.h,v 1.4 1999/02/19 21:22:58 marsh Exp $";
#endif

#include <DS/containers/sortseq.h>

class NodeID;
class ds_String;
class SwitchLevelInfo;

class SwitchConfiguration {
public:

  // Owns all pointers
  SwitchConfiguration(ds_String *name, int ports, NodeID *base,
		      sortseq<int, SwitchLevelInfo *> *levels, 
		      bool Omniscient,
		      ds_String *acacPrefix, ds_String *acacLibrary,
		      ds_String *logosPrefix, ds_String *logosLibrary, 
		      ds_String *aggregatorPrefix, ds_String *aggregatorLibrary);

  // Deletes all pointers
  ~SwitchConfiguration(void);

  // caller owns the pointer
  ds_String *Name(void);

  int Ports(void);

  // caller owns the pointer
  NodeID *BaseNodeID(void);

  int PhysicalLevel(void);

  int NextLevelDown(int level);

  int NextLevelUp(int level);

  int PeerGroupLeadershipPriority(int level);

  // caller owns the pointer
  NodeID *PreferredPeerGroupLeader(int level);

  // Returns true if the switch uses the global network topology database.
  // Returns false otherwise.
  bool Omniscient(void);

  // caller owns the pointer
  ds_String *ACACPluginPrefix(void);

  // caller owns the pointer
  ds_String *ACACPluginLibrary(void);

  // caller owns the pointer
  ds_String *LogosPluginPrefix(void);

  // caller owns the pointer
  ds_String *LogosPluginLibrary(void);

  // caller owns the pointer
  ds_String *AggregatorPluginPrefix(void);

  // caller owns the pointer
  ds_String *AggregatorPluginLibrary(void);

private:
  ds_String *_name;
  int _ports;
  NodeID *_baseNodeID;
  sortseq<int, SwitchLevelInfo *> *_levels;
  bool _isOmniscient;
  ds_String *_acacPluginPrefix;
  ds_String *_acacPluginLibrary;
  ds_String *_logosPluginPrefix;
  ds_String *_logosPluginLibrary;
  ds_String *_aggregatorPluginPrefix;
  ds_String *_aggregatorPluginLibrary;
};


class SwitchLevelInfo {
public:
  // Owns all pointers
  SwitchLevelInfo(int level, int pgl_priority, NodeID *preferredPGL);

  // Deletes all pointers
  ~SwitchLevelInfo(void);
  
  int Level(void);
  int PeerGroupLeadershipPriority(void);

  // Caller owns the pointer
  NodeID *PreferredPeerGroupLeader(void);

private:
  int _level;
  int _pgl_priority;
  NodeID *_preferred_pgl;
};


#endif
