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
// @file Configurator.cc
// @author talmage
// @version $Id: Configurator.cc,v 1.8 1999/02/19 21:22:57 marsh Exp $
//
// Purpose: Implements a MIB for switch, link, and call configuration
//

#ifndef LINT
static char const _Configurator_cc_rcsid_[] =
"$Id: Configurator.cc,v 1.8 1999/02/19 21:22:57 marsh Exp $";
#endif

#include "CallConfiguration.h"
#include "Configurator.h"
#include "LinkConfiguration.h"
#include "SwitchConfiguration.h"
#include <DS/util/String.h>
#include <FW/basics/diag.h>
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/UNI40_bbc.h>
#include <codec/uni_ie/UNI40_bbc.h>
#include <codec/uni_ie/UNI40_e2e_trans_delay.h>
#include <codec/uni_ie/UNI40_td.h>
#include <codec/uni_ie/UNI40_xqos.h>
#include <codec/uni_ie/qos_param.h>
#include <common/cprototypes.h>

#define YY_PROTO(proto) proto
typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern void PNNIConfigurator_delete_buffer YY_PROTO(( YY_BUFFER_STATE b ));

extern YY_BUFFER_STATE PNNIConfigurator_create_buffer 
	YY_PROTO(( FILE *file, int size ));

extern void PNNIConfigurator_switch_to_buffer
	YY_PROTO(( YY_BUFFER_STATE new_buffer ));
#ifndef YY_BUF_SIZE
#define YY_BUF_SIZE 16384
#endif

Configurator * Configurator::_configurator_singleton = 0;

const Configurator &theConfigurator(const char * filename)
{
  if (filename != 0) {
    if (Configurator::_configurator_singleton != 0)
      diag("fsm.configurator", DIAG_FATAL, 
	   "Attempt to re-initialize the global Configurator from file \"%s\".\n",
	   filename);

    Configurator::_configurator_singleton = new Configurator(filename);
    return *(Configurator::_configurator_singleton);
  } else {
    if (Configurator::_configurator_singleton == 0)
      diag("fsm.configurator", DIAG_FATAL, 
	   "The global Configurator has not been initialized.\n");

    return *(Configurator::_configurator_singleton);
  }
}


Configurator::Configurator(const char *filename) : _good(false)
{
  //
  // XXX This has to go in the yac file
  //

  FILE *input_file = fopen(filename, "r");
  if (input_file == 0)
    diag("fsm.configurator", DIAG_FATAL,
	 "Configurator could not open file \"%s\"\n", filename);

  YY_BUFFER_STATE buf = PNNIConfigurator_create_buffer(input_file, 
						       YY_BUF_SIZE);
  if (buf != 0) {
    PNNIConfigurator_switch_to_buffer(buf);
    int answer = PNNIConfiguratorparse((void *)this);
    PNNIConfigurator_delete_buffer(buf);
    fclose(input_file);

    // XXX Do something with answer
    _good = (answer == 0);
  }
}


Configurator::~Configurator(void)
{
  dic_item ditem;

  while (_switches.size() > 0) {
    ditem = _switches.first();

    ds_String *key = _switches.key(ditem);
    SwitchConfiguration *switchConfig = _switches.inf(ditem);

    delete key;
    delete switchConfig;

    _switches.del_item(ditem);
  }

  while (_links.size() > 0) {
    ditem = _links.first();

    ds_String *key = _links.key(ditem);
    LinkConfiguration *link = _links.inf(ditem);

    delete key;
    delete link;

    _links.del_item(ditem);
  }

  while (_calls.size() > 0) {
    ditem = _calls.first();

    ds_String *key = _calls.key(ditem);
    CallConfiguration *call = _calls.inf(ditem);

    delete key;
    delete call;

    _calls.del_item(ditem);
  }

}


bool Configurator::good(void) const
{
  return _good;
}


//////////////////////////////////////////////////////////////////////////
//
// Methods related to switches
//
//////////////////////////////////////////////////////////////////////////

int Configurator::Ports(ds_String *switchName) const
{
  int answer = -1;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->Ports();
  }

  return answer;
}


// Caller owns the pointer.
NodeID *Configurator::BaseNodeID(ds_String *switchName) const
{
  NodeID *answer = 0;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->BaseNodeID();
  }

  return answer;
}


int Configurator::PhysicalLevel(ds_String *switchName) const
{
  int answer = -1;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->PhysicalLevel();
  }

  return answer;
}


int Configurator::NextLevelDown(ds_String *switchName, int level) const
{
  int answer = -1;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->NextLevelDown(level);
  }

  return answer;
}


int Configurator::NextLevelUp(ds_String *switchName, int level) const
{
  int answer = -1;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->NextLevelUp(level);
  }

  return answer;
}


int Configurator::PeerGroupLeadershipPriority(ds_String *switchName, 
					      int level) const
{
  int answer = -1;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->PeerGroupLeadershipPriority(level);
  }

  return answer;
}


// Caller owns the pointer.
// Always returns zero.
NodeID *Configurator::PreferredPeerGroupLeader(ds_String *switchName, 
					       int level) const
{
  NodeID *answer = 0;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->PreferredPeerGroupLeader(level);
  }

  return answer;
}
  

bool Configurator::Omniscient(ds_String *switchName) const
{
  bool answer = false;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->Omniscient();
  }

  return answer;
}


// Caller owns the pointer.
ds_String *Configurator::ACACPluginPrefix(ds_String *switchName) const
{
  ds_String *answer = 0;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->ACACPluginPrefix();
  }

  return answer;
}


// Caller owns the pointer.
ds_String *Configurator::ACACPluginLibrary(ds_String *switchName) const
{
  ds_String *answer = 0;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->ACACPluginLibrary();
  }

  return answer;
}


// Caller owns the pointer.
ds_String *Configurator::LogosPluginPrefix(ds_String *switchName) const
{
  ds_String *answer = 0;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->LogosPluginPrefix();
  }

  return answer;
}


// Caller owns the pointer.
ds_String *Configurator::LogosPluginLibrary(ds_String *switchName) const
{
  ds_String *answer = 0;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->LogosPluginLibrary();
  }

  return answer;
}


// Caller owns the pointer.
ds_String *Configurator::AggregatorPluginPrefix(ds_String *switchName) const
{
  ds_String *answer = 0;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->AggregatorPluginPrefix();
  }

  return answer;
}


// Caller owns the pointer.
ds_String *Configurator::AggregatorPluginLibrary(ds_String *switchName) const
{
  ds_String *answer = 0;
  dic_item ditem = _switches.lookup(switchName);

  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    answer = s->AggregatorPluginLibrary();
  }

  return answer;
}


//////////////////////////////////////////////////////////////////////////
//
// Methods related to links
//
//////////////////////////////////////////////////////////////////////////

u_int Configurator::AggregationToken(ds_String *linkName) const
{
  int answer = -1;
  dic_item ditem = _links.lookup(linkName);

  if (ditem != 0) {
    LinkConfiguration *l = _links.inf(ditem);
    answer = l->AggregationToken();
  }

  return answer;
}


// Caller owns the pointer.
list <ig_resrc_avail_info *> *Configurator::RAIGs(ds_String *linkName) const
{
  list <ig_resrc_avail_info *> *answer = 0;
  dic_item ditem = _links.lookup(linkName);

  if (ditem != 0) {
    LinkConfiguration *l = _links.inf(ditem);
    answer = l->RAIGs();
  }

  return answer;
}


//////////////////////////////////////////////////////////////////////////
//
// Methods related to calls
//
//////////////////////////////////////////////////////////////////////////

UNI40_traffic_desc *Configurator::TrafficDescriptor(ds_String *callName) const
{
  UNI40_traffic_desc *answer = 0;
  dic_item ditem = _calls.lookup(callName);

  if (ditem != 0) {
    CallConfiguration *c = _calls.inf(ditem);
    answer = c->TrafficDescriptor();
  }

  return answer;
}


ie_bbc *Configurator::BroadbandBearerCap(ds_String *callName) const
{
  ie_bbc *answer = 0;
  dic_item ditem = _calls.lookup(callName);

  if (ditem != 0) {
    CallConfiguration *c = _calls.inf(ditem);
    answer = c->BroadbandBearerCap();
  }

  return answer;
}


ie_qos_param *Configurator::QualityOfService(ds_String *callName) const
{
  ie_qos_param *answer = 0;
  dic_item ditem = _calls.lookup(callName);

  if (ditem != 0) {
    CallConfiguration *c = _calls.inf(ditem);
    answer = c->QualityOfService();
  }

  return answer;
}


UNI40_xqos_param *
Configurator::ExtendedQualityOfService(ds_String *callName) const
{
  UNI40_xqos_param *answer = 0;
  dic_item ditem = _calls.lookup(callName);

  if (ditem != 0) {
    CallConfiguration *c = _calls.inf(ditem);
    answer = c->ExtendedQualityOfService();
  }

  return answer;
}


UNI40_e2e_transit_delay *
Configurator::EndToEndTransitDelay(ds_String *callName) const
{
  UNI40_e2e_transit_delay *answer = 0;
  dic_item ditem = _calls.lookup(callName);

  if (ditem != 0) {
    CallConfiguration *c = _calls.inf(ditem);
    answer = c->EndToEndTransitDelay();
  }

  return answer;
}


// Copies key.  Owns switch.
//
// If there is another SwitchConfiguration for the same key,
// Insert() silently deletes it and substitutes the new one.
void Configurator::Insert(ds_String *key, SwitchConfiguration *switchConfig) 
{
  dic_item ditem = _switches.lookup(key);
  if (ditem != 0) {
    SwitchConfiguration *s = _switches.inf(ditem);
    delete s;
    _switches.change_inf(ditem, switchConfig);
  } else {
    ds_String *key_copy = new ds_String(*key);
    _switches.insert(key_copy, switchConfig);
  }
}


// Copies key.  Owns link.
//
// If there is another LinkConfiguration for the same key,
// Insert() silently deletes it and substitutes the new one.
void Configurator::Insert(ds_String *key, LinkConfiguration *link) 
{
  dic_item ditem = _links.lookup(key);
  if (ditem != 0) {
    LinkConfiguration *l = _links.inf(ditem);
    delete l;
    _links.change_inf(ditem, link);
  } else {
    ds_String *key_copy = new ds_String(*key);
    _links.insert(key_copy, link);
  }
}


// Copies key.  Owns call.
//
// If there is another CallConfiguration for the same key,
// Insert() silently deletes it and substitutes the new one.
void Configurator::Insert(ds_String *key, CallConfiguration *call) 
{
  dic_item ditem = _calls.lookup(key);
  if (ditem != 0) {
    CallConfiguration *c = _calls.inf(ditem);
    delete c;
    _calls.change_inf(ditem, call);
  } else {
    ds_String *key_copy = new ds_String(*key);
    _calls.insert(key_copy, call);
  }
}

void Configurator::Print(ostream &os) const
{
  ds_String *temp = 0;
  dic_item ditem;

  forall_items(ditem, _switches) {
    ds_String *key = _switches.key(ditem);
    os << *key << ":" << endl;

    os << "Ports: " << Ports(key) << endl;

    NodeID *base = BaseNodeID(key);
    os << "BaseNodeID: " << *base << endl;
    delete base;

    int lev = PhysicalLevel(key);
    os << "Physical level: " << lev << endl;
    os << "Level	PGLPriority	PrefPGL" << endl;
    while (lev != -1) {
      int pglpriority = PeerGroupLeadershipPriority(key, lev);
      NodeID *pgl = PreferredPeerGroupLeader(key, lev);
      os << lev << "\t" << pglpriority;
      if (pgl != 0) {
	os << "\t\t" << *pgl;
	delete pgl;
      } else
	os << "\t\t0";

      os << endl;

      lev = NextLevelUp(key, lev);
    }

    os << endl;

    os << "Omniscient: ";
    if (Omniscient(key)) os << "true"; else os << "false";
    os << endl;

    temp = ACACPluginPrefix(key);
    os << "ACACPrefix: ";
    if (temp != 0) os << *temp;
    os << endl;

    delete temp;
    temp = ACACPluginLibrary(key);
    os << "ACACLibrary: ";
    if (temp != 0) os << *temp;
    os << endl;

    delete temp;
    temp = LogosPluginPrefix(key);
    os << "LogosPrefix: ";
    if (temp != 0) os << *temp;
    os << endl;

    delete temp;
    temp = LogosPluginLibrary(key);
    os << "LogosLibrary: ";
    if (temp != 0) os << *temp;
    os << endl;

    delete temp;
    temp = AggregatorPluginPrefix(key);
    os << "AggregatorPrefix: ";
    if (temp != 0) os << *temp;
    os << endl;

    delete temp;
    temp = AggregatorPluginLibrary(key);
    os << "AggregatorLibrary: ";
    if (temp != 0) os << *temp;
    os << endl;

    delete temp;

  }

  os << endl;
  os << "Links:" << endl;
  forall_items(ditem, _links) {
    list_item litem;
    ds_String *key = _links.key(ditem);
    list <ig_resrc_avail_info *> *raigs = RAIGs(key);

    os << *key << ":" << endl;
    while (raigs->size() > 0) {
      ig_resrc_avail_info *raig = raigs->pop();

      os << *raig << endl;
      delete raig;
    }
    delete raigs;
  }

  os << endl;
  os << "Calls:" << endl;
  forall_items(ditem, _calls) {
    ds_String *key = _calls.key(ditem);
    UNI40_traffic_desc *td = TrafficDescriptor(key);
    ie_bbc *bbc = BroadbandBearerCap(key);
    ie_qos_param *qos = QualityOfService(key);
    UNI40_xqos_param *xqos = ExtendedQualityOfService(key);
    UNI40_e2e_transit_delay *e2etd = EndToEndTransitDelay(key);

    os << *key << ":" << endl;
    os << "BBC:" << *bbc << endl;
    os << "E2etd: " << *e2etd << endl;
    os << "ExtQoS: " << *xqos << endl;
    os << "QoS: " << *qos << endl;
    os << "TD: " << *td << endl;
    os << endl;

    delete bbc;
    delete e2etd;
    delete xqos;
    delete qos;
    delete td;
  }

}

void ConfiguratorInsert(ds_String *key, SwitchConfiguration *switchConfig)
{
  Configurator::_configurator_singleton->Insert(key, switchConfig);
}


void ConfiguratorInsert(ds_String *key, LinkConfiguration *link)
{
  Configurator::_configurator_singleton->Insert(key, link);
}


void ConfiguratorInsert(ds_String *key, CallConfiguration *call)
{
  Configurator::_configurator_singleton->Insert(key, call);
}


