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
 * File: SwitchFunctions.cc
 * Author: talmage
 * Version: $Id: HostFunctions.cc,v 1.13 1999/02/19 21:22:52 marsh Exp $
 * Purpose: Shortcuts to creating and connecting switches.
 * BUGS:
 */

#if !defined(LINT)
static char const rcsid[] =
"$Id: HostFunctions.cc,v 1.13 1999/02/19 21:22:52 marsh Exp $";
#endif

#include <DS/util/String.h>
#include <FW/behaviors/Cluster.h>
#include <FW/basics/diag.h>
#include <FW/basics/Conduit.h>
#include <codec/pnni_ig/id.h>
#include <sim/port/LinkState.h>
#include <sim/port/LinkStateWrapper.h>
#include <sim/port/PortAccessor.h>
#include <sim/port/PortCreator.h>
#include <sim/switch/StrikeVisitor.h>
#include <sim/switch/SwitchExpander.h>
#include <sim/switch/HostFunctions.h>


//
// Create a switch with name 'name' and some number of ports.
//
Conduit *MakeHost(const ds_String *key, ds_String *name, 
		  Addr *& myAddress)
{
  Conduit *answer = 0;
#if 0
  Conduit *controlTerminal = 0;
  NodeID * myNode = 0;
  NodeID * myParentNode = 0; 
  NodeID * myPreferredPeerGroupLeader = 0;

  if (name == 0) name = "HOST";

  DIAG(SIM, DIAG_DEBUG, cout << "Making host " << name << endl;)

  Accessor     * ac = new PortAccessor();
  Creator      * uc = new PortCreator(0);  // This is a bug!

  Expander     * ne = new SwitchExpander(key, name, myNode, 0, 
					 controlTerminal);
  Cluster      * cl = new Cluster(ne, 0, ac);
  answer =new Conduit(name, cl);

  if (myNode != 0) myAddress = myNode->GetAddr();

  //
  // Tell the Factory to stop creating ports in the host.
  //
  if (answer != 0) {
    StrikeVisitor *sv = new StrikeVisitor();
    sv->SetLoggingOn();
    DIAG(SIM, DIAG_DEBUG, 
	 cout << endl <<"Sending StrikeVisitor to host " << name << endl;)
    A_half(answer)->Accept(sv);
    DIAG(SIM, DIAG_DEBUG, 
	 cout << endl << "Sent StrikeVisitor to host " << name << endl;)
  }

#endif

  return answer;
}
