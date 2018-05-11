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

#ifndef LINT
static char const _linktst_cc_rcsid_[] =
"$Id: linktst.cc,v 1.6 1998/08/06 04:05:35 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include "LinkState.h"
#include "LinkStateWrapper.h"
#include "NullState.h"
#include "PortAccessor.h"
#include "PortCreator.h"
#include "PortDataVisitor.h"
#include "PortExpander.h"
#include <FW/basics/Conduit.h>
#include <FW/behaviors/Factory.h>
#include <FW/behaviors/Protocol.h>
#include <fsm/database/Database.h>
#include <codec/pnni_ig/id.h>

#include <iostream.h>

int main(int argc, char** argv)
{
  u_char source_address[22] = {96, 160, 0x47, 0x00, 0x05, 
			       0x80, 0xff, 0xde, 0x00, 0x00, 
			       0x01, 0x00, 0x00, 0x04, 0x01,
			       0xff, 0x1c, 0x06, 0x01, 0x01, 
			       0x01, 0x00 };

  NodeID *nodeID1 = new NodeID(source_address);
  NodeID *nodeID2 = new NodeID(source_address);

  Database database(nodeID1);	/* Leaks one SimEvent.  To plug the
				 * leak, start the Kernel.  That will
				 * let the SimEvent arrive in
				 * Database::Interrupt(), where it
				 * will be deleted. 
				 */
  PortDataVisitor *portDataVisitor = new PortDataVisitor();

  LinkState *linkState = new LinkState();
  Protocol *linkProtocol = new Protocol(linkState);
  Conduit *linkConduit = new Conduit("TestLinkState", linkProtocol);

  NullState *nullState = new NullState();
  Protocol *nullProtocol = new Protocol(nullState);
  Conduit *nullConduit = new Conduit("TestNullState", nullProtocol);

  PortCreator *portCreator = new PortCreator();
  Factory *portFactory = new Factory(portCreator);
  Conduit *portConduit = new Conduit("TestPortCreator", portFactory);

  PortExpander *portExpander = new PortExpander(database, 1, nodeID2);
  PortAccessor *portAccessor1 = new PortAccessor();
  PortAccessor *portAccessor2 = new PortAccessor();
  Cluster *portCluster = new Cluster(portExpander, portAccessor1, 
  				     portAccessor2);
  Conduit *clusterConduit = new Conduit("TestPortAccessor", portCluster);

  delete linkConduit;
  delete nullConduit;
  delete portConduit;
  delete clusterConduit;

  return 0;
}
