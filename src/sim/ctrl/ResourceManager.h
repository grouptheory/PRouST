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
 * @version $Id: ResourceManager.h,v 1.47 1999/02/19 21:22:48 marsh Exp $
 *
 * BUGS:
 */
#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#ifndef LINT
static char const _ResourceManager_h_rcsid_[] =
"$Id: ResourceManager.h,v 1.47 1999/02/19 21:22:48 marsh Exp $";
#endif

#include <iostream.h>
#include <FW/actors/Expander.h>

class ACAC;
class Aggregator;
class Conduit;
class Database;
class Leadership;
class Logos;
class NodeID;
class RouteControl;
class SimEvent;
class ds_String;
class VCAllocator;

class ResourceManager : public Expander {
  friend ostream & operator << (ostream & os, ResourceManager & rm);
public:

  ResourceManager(ds_String * key, NodeID *& myNode);

  virtual ~ResourceManager();

  Conduit * GetAHalf(void) const;	// _acac, side A
  Conduit * GetBHalf(void) const;	// _db, side B

private:

  ACAC		        * _acacPluginState;
  RouteControl          * _routeControl;
  Logos                 * _logos;
  Leadership            * _leadership;
  Aggregator            * _aggregator;
  Database              * _database;

  Conduit * _acac;	// Actual Call Admission Control
  Conduit * _router;    // RouteControl
  Conduit * _gcac;      // Logos
  Conduit * _leader;    // Leadership
  Conduit * _agg;       // Aggregator
  Conduit * _db;        // Database
};

#endif // __CACRTR_H__
