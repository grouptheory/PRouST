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
// @file Configurator.h
// @author talmage
// @version $Id: Configurator.h,v 1.6 1999/02/19 21:22:57 marsh Exp $
//
// Purpose: Implements a MIB for switch, link, and call configuration.
// Access the global singleton Configurator through theConfigurator().
//

#ifndef __CONFIGURATOR_H__
#define __CONFIGURATOR_H__

#ifndef LINT
static char const _configurator_h_rcsid_[] =
"$Id: ";
#endif

#include <DS/containers/dictionary.h>
#include <DS/containers/list.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <ostream.h>

class CallConfiguration;
class LinkConfiguration;
class NodeID;
class ds_String;
class SwitchConfiguration;
class UNI40_e2e_transit_delay;
class UNI40_traffic_desc;
class UNI40_xqos_param;
class ie_bbc;
class ie_qos_param;

class Configurator {
public:
  friend int PNNIConfiguratorparse(void *);

  friend void ConfiguratorInsert(ds_String *key, 
				 SwitchConfiguration *switchConfig);
  friend void ConfiguratorInsert(ds_String *key, LinkConfiguration *link);
  friend void ConfiguratorInsert(ds_String *key, CallConfiguration *call);

  // Returns the configurator singleton, possibly constructing it from
  // the file named by filename.  Aborts if filename doesn't exist or
  // if filename is zero and the singleton hasn't been initialized.
  friend const Configurator &theConfigurator(const char *filename = 0);

  // Aborts if filename is zero or if it can't be opened.
  Configurator(const char *filename);
  ~Configurator(void);

  // Returns the correctness of the Configurator.  true indicates that
  // the Configurator is ready for use.  false indicates that the
  // Configurator is not ready for use, perhaps because the input file
  // contained errors.
  bool good(void) const;

  //
  // Methods for Configuration of switches
  //
  int Ports(ds_String *switchName) const;

  // Caller owns the pointer.
  NodeID *BaseNodeID(ds_String *switchName) const;

  // -1 indicates no answer
  int PhysicalLevel(ds_String *switchName) const;

  // -1 indicates no lower level
  int NextLevelDown(ds_String *switchName, int level) const;

  // -1 indicates no higher level
  int NextLevelUp(ds_String *switchName, int level) const;

  // -1 indicates no leadership priority
  int PeerGroupLeadershipPriority(ds_String *switchName, int level) const;

  // Caller owns the pointer.
  // Always returns zero.
  NodeID *PreferredPeerGroupLeader(ds_String *switchName, int level) const;
  
  // Returns true if the switch uses the global network topology database.
  // Returns false otherwise.
  bool Omniscient(ds_String *switchName) const;

  // caller owns the pointer
  ds_String *ACACPluginPrefix(ds_String *switchName) const;

  // caller owns the pointer
  ds_String *ACACPluginLibrary(ds_String *switchName) const;

  // caller owns the pointer
  ds_String *LogosPluginPrefix(ds_String *switchName) const;

  // caller owns the pointer
  ds_String *LogosPluginLibrary(ds_String *switchName) const;

  // caller owns the pointer
  ds_String *AggregatorPluginPrefix(ds_String *switchName) const;

  // caller owns the pointer
  ds_String *AggregatorPluginLibrary(ds_String *switchName) const;

  //
  // Methods for Configuration of links
  //

  //
  // Returns the value of the link's aggregation token.
  //
  // -1 indicates no aggregation token.
  //
  u_int AggregationToken(ds_String *linkName) const;

  // Caller owns the pointer.
  // 
  // 0 indicates no RAIGs.
  //
  list <ig_resrc_avail_info *> *RAIGs(ds_String *linkName) const;


  //
  // Methods for Configuration of calls
  //

  UNI40_traffic_desc *TrafficDescriptor(ds_String *callName) const;
  ie_bbc *BroadbandBearerCap(ds_String *callName) const;
  ie_qos_param *QualityOfService(ds_String *callName) const;
  UNI40_xqos_param *ExtendedQualityOfService(ds_String *callName) const;
  UNI40_e2e_transit_delay *EndToEndTransitDelay(ds_String *callName) const;

  //
  // Display everything in the Configurator
  //
  void Print(ostream &os) const;

  //
  // Insert information into the Configurator
  //

  // Copies key.  Owns switch.
  //
  // If there is another SwitchConfiguration for the same key,
  // Insert() silently deletes it and substitutes the new one.
  void Insert(ds_String *key, SwitchConfiguration *switchConfig);

  // Copies key.  Owns link.
  //
  // If there is another LinkConfiguration for the same key,
  // Insert() silently deletes it and substitutes the new one.
  void Insert(ds_String *key, LinkConfiguration *link);

  // Copies key.  Owns call.
  //
  // If there is another CallConfiguration for the same key,
  // Insert() silently deletes it and substitutes the new one.
  void Insert(ds_String *key, CallConfiguration *call);

private:
  bool _good;
  static Configurator * _configurator_singleton;

  // Do not define this method under penalty of death!
  //  The sneaky compiler attempts to define this method for us,
  //  and causes quite a few problems by stealing pointers.
  Configurator(const Configurator & rhs);

  dictionary <ds_String *, SwitchConfiguration *> _switches;
  dictionary <ds_String *, LinkConfiguration *> _links;
  dictionary <ds_String *, CallConfiguration *> _calls;
};

#endif
