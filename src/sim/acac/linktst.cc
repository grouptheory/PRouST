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
"$Id: linktst.cc,v 1.19 1999/02/19 21:22:59 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include "ACAC.h"
#include "pluggable_port_info.h"
#include <FW/basics/Conduit.h>
#include <DS/containers/memory.h>
#include <sim/ctrl/ResourceManager.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/uni_ie/addr.h>
#include <fsm/config/Configurator.h>
#include <fsm/database/Database.h>
#include <iostream.h>

extern void DeleteFW(void);

int main(int argc, char** argv)
{
  u_char remote_address[22] = {96, 160, 0x47, 0x00, 0x05, 
			       0x80, 0xff, 0xde, 0x00, 0x00, 
			       0x01, 0x00, 0x00, 0x04, 0x01,
			       0xff, 0x1c, 0x06, 0x01, 0x01, 
			       0x01, 0x00 };
  Addr *anAddr = 0;
  NodeID *aNodeID = 0;
  NodeID *aPeerGroupID = 0;
  NodeID *anotherNodeID = 0;
  Database * aDatabase = 0;
  pluggable_port_info *aPort;
  ResourceManager *aCAC;

  if (argc < 2) {
    cout << "Usage: " << argv[0] << " visualizer_pipe_name" << endl;
    cout << "Using /dev/null instead." << endl;
  }
  VisPipe(((argc > 1) ? argv[1] : "/dev/null"));
  const Configurator &globalConfig = theConfigurator("Configurator.ACAC");

  assert(globalConfig.good());

  ds_String *key = new ds_String("ACAC-Test1");

  NodeID *remoteNode = new NodeID(remote_address);
  
  cout << "Creating a pluggable_port_info" << endl;
  aPort = new pluggable_port_info(0, false, pluggable_port_info::unspecified, 
			NPStateVisitor::NonFullState, 33, 1, 
			2, remoteNode, 0, 0);
  cout << *aPort << endl;
  
  cout << "Creating a CAC expander" << endl;
  aCAC = new ResourceManager(key, remoteNode);
  Conduit *ACACConduit = new Conduit("ACAC", aCAC);

  cout << *aCAC << endl;
  
  cout << "Deleting a pluggable_port_info" << endl;
  delete aPort;
  
  cout << "Deleting a CAC expander" << endl;
  delete ACACConduit;
  
  cout << "Deleting a NodeID" << endl;
  delete remoteNode;
  
  cout << "Deleting the things returned by the CAC constructor...";
  
  cout << "NodeID...";
  delete aNodeID;
  
  cout << "PeerID...";
  delete aPeerGroupID;
  
  cout << "PGL's NodeID" << endl;
  delete anotherNodeID;
  
  DeleteFW();
  return 0;
}

