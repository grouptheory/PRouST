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

/* -*- C++ -*-
 * @file ResourceManager.cc
 * @author talmage 
 * @version $Id: ResourceManager.cc,v 1.110 1999/02/19 21:22:48 marsh Exp $
 *
 * BUGS:
 */
#ifndef LINT
static char const _ResourceManager_cc_rcsid[] =
"$Id: ResourceManager.cc,v 1.110 1999/02/19 21:22:48 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <stdio.h>
#include <assert.h>

#include "ResourceManager.h"
#include <DS/util/String.h>
#include <FW/basics/Conduit.h>
#include <FW/behaviors/Protocol.h>
#include <fsm/config/Configurator.h>
#include <fsm/database/Database.h>
#include <sim/acac/ACAC.h>
#include <sim/logos/Logos.h>
#include <sim/router/RouteControl.h>
#include <sim/aggregator/Aggregator.h>
#include <sim/leadership/Leadership.h>
#include <fsm/omni/Omni.h>

#define CONSULT_OMNI 1

/*
 * Constructor
 *
 * int numPorts The number of ports in the switch.
 *
 * NodeID *& myNode Address of the switch.  If you don't
 * supply dbFileName, below, then you must supply myNode.  If you
 * supply dbFileName, then CAC() fills in myNode.
 *
 * NodeID *& myParentNode Address of the switch's parent.
 * CAC() fills this in if you supply dbFileName.  CAC() ignores it if
 * you don't supply dbFileName.
 *
 * NodeID *& myPreferredPeerGroupLeader Address of the
 * switch's peer group leader.  * CAC() fills this in if you supply
 * dbFileName.  CAC() ignores it if you don't supply dbFileName.
 *
 * Database*& myDatabase Address of the switch's database.
 * CAC() creates myDatabase.
 *
 * const char * dbFileName Optional name of the file that contains 
 * the initial contents of the database.  If it's zero, then you must supply
 * myNode. 
 */
ResourceManager::ResourceManager(ds_String *key, NodeID *& myNode) :
  _acacPluginState(0), 
  _routeControl(0), _logos(0), _leadership(0),
  _aggregator(0), _database(0), _acac(0), _router(0), _gcac(0), 
  _leader(0), _agg(0), _db(0)
{ 
  Conduit  * prev = 0;
  Protocol * pro = 0;
  FILE     * fp = 0;

  bool   omni_mode = theConfigurator().Omniscient(key);

  ds_String * acacPluginPrefix = theConfigurator().ACACPluginPrefix(key);
  ds_String * logosPluginPrefix = theConfigurator().LogosPluginPrefix(key);
  ds_String * aggregatorPluginPrefix = theConfigurator().AggregatorPluginPrefix(key);

  //
  // Whether myNode comes from the caller or from the database, 
  // it had better not be zero.  the ResourceManager doesn't 
  // know what to do if it's zero.
  //
  assert(myNode);

  //
  // Create the empty Database first.
  // By some magic, this guarantees that it can tell each Factory about
  // itself before anyone else tells anything about itself.
  //
  _database = new Database(key, myNode);
  _db = new Conduit("Database", _database);

  PeerID * mypgid = myNode->GetPeerGroup();

  // Create ACAC 
  _acacPluginState = new ACAC(new NodeID(*myNode), 
			      theConfigurator().Ports(key),
			      _database->PTSERefreshInterval(),
			      acacPluginPrefix);
  _acac = new Conduit("CallAdmissionControl", _acacPluginState);
  
  // Create the RouteControl
  _routeControl = new RouteControl(myNode);
  _router = new Conduit("RouteControl", _routeControl);

  _logos = new Logos( myNode, logosPluginPrefix, omni_mode );
  _gcac = new Conduit("Logos", _logos);

  // Create the Aggregator
  _aggregator = new Aggregator(*myNode, *mypgid, aggregatorPluginPrefix->chars());
  _agg = new Conduit("Aggregator", _aggregator);

  // Create the Leadership
  _leadership = new Leadership(myNode);
  _leader = new Conduit("Leadership", _leadership);

  //
  // Join the Conduits of the CAC expander together
  // The A side of ACAC is the A side of ResourceManager.  
  // The B side of ACAC is connected to the A side of Logos.
  // The B side of Logos is connected to the A side of the database.
  // The B side of the database is the B side of ResourceManager.
  //
  // A-ACAC-B <---> A-RouteControl-B <---> A-Logos-B <---> A-Leadership-B 
  //          <---> A-Aggregator-B <---> A-Database-B
  //
  Join(B_half(_acac),   A_half(_router));
  Join(B_half(_router), A_half(_gcac));
  Join(B_half(_gcac),   A_half(_leader));
  Join(B_half(_leader), A_half(_agg));
  Join(B_half(_agg),    A_half(_db));

  delete mypgid;

  delete acacPluginPrefix;
  delete logosPluginPrefix;
  delete aggregatorPluginPrefix;

  DefinitionComplete();
}


ResourceManager::~ResourceManager()
{
  delete _acac;
  delete _router;
  delete _gcac;
  delete _leader;
  delete _agg;
  delete _db;
}


/*
 * The A half of the CAC Expander is the A side of _aResourceManager.
 */
Conduit * ResourceManager::GetAHalf(void) const
{
  return A_half(_acac);
}


/*
 * The B half of the CAC Expander is the B side of _db, the database.
 */
Conduit * ResourceManager::GetBHalf(void) const
{
  return B_half(_db);
}

/**
 * Permit the printing of any CAC object on an ostream.
 *
 * @param os the ostream on which to print
 *
 * @param cac the CAC object to print
 *
 * @return the ostream
 */
ostream & operator << (ostream & os, ResourceManager & rm)
{
  os << "ResourceManager " << &rm << " " << rm.OwnerName() << endl;

  return os;
}
