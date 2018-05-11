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
 * File: SwitchExpander.h
 * Author: 
 * Version: $Id: SwitchExpander.h,v 1.37 1999/02/19 21:22:53 marsh Exp $
 * Purpose: Creates Switches with an arbitrary number of Ports.
 * BUGS:
 */


#ifndef __SWITCH_EXPANDER_H__
#define __SWITCH_EXPANDER_H__
#ifndef LINT
static char const _SwitchExpander_h_rcsid_[] =
"$Id: SwitchExpander.h,v 1.37 1999/02/19 21:22:53 marsh Exp $";
#endif

#include <DS/containers/list.h>
#include <FW/actors/Expander.h>
#include <sim/ctrl/Control.h>

class Addr;
class Creator;
class NodeID;
class ds_String;

//
// A SwitchExpander models a switch with an arbitrary number of ports.
//
// By default, the switch has no ports and it uses a PortCreator object
// to build new ports.  The switch adds ports on receipt of the 
// PortInstallerVisitor object.
//
// If the constructor is given a non-zero pointer to a Creator object,
// it uses that object in the Port Factory.  Otherwise, it creates its 
// own PortCreator for the Port Factory.
//
// If the constructor is given the number of ports for the switch, it
// builds them by sending one PortInstallerVisitor per port into the
// switch.
//
// The constructor sets the NodeID of the switch (myNode), its parent
// (myParentNode), and its peer group leader (myPreferredPeerGroupLeader).
//
// The constructor also sets the Conduit of the Terminal at the bottom
// of the Control cluster.  The Conduit supplied in the
// controlTerminal parameter is used if it is non-zero.  Otherwise,
// the constructor supplies the default DebugTerminal.  This is useful
// for inserting Visitors into Control to test the Database, Logos,
// and ACAC.
//
class SwitchExpander : public Expander {
public:

  SwitchExpander(ds_String *key, 
		 ds_String *name,
		 NodeID *& myNode,
		 Creator *portCreator,
		 Conduit *& controlTerminal);

  virtual Conduit * GetAHalf(void) const;
  virtual Conduit * GetBHalf(void) const;

  const NodeID * GetAddress(void) const { return _cptr->GetAddress(); }

protected:

  virtual ~SwitchExpander();

private:
  Conduit * _control;
  Control * _cptr;
  Conduit * _data_forwarder;	
  Conduit * _top_mux;
  Conduit * _port_factory;
  Conduit * _bot_mux;
};

#endif
