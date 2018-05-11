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
#ifndef LINT
static char const _linktst_cc_rcsid_[] =
"$Id: linktst.cc,v 1.5 1999/02/10 16:33:06 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "Logos.h"
#include <iostream.h>
#include <fsm/database/Database.h>

int main(int argc, char ** argv)
{
  u_char source_address[22] = {96, 160, 0x47, 0x00, 0x05, 
			       0x80, 0xff, 0xde, 0x00, 0x00, 
			       0x01, 0x00, 0x00, 0x04, 0x01,
			       0xff, 0x1c, 0x06, 0x01, 0x01, 
			       0x01, 0x00 };
  
  NodeID *sourceNode1 = new NodeID(source_address);
  NodeID *sourceNode2 = new NodeID(source_address);
  
  Logos *logos1 = 0;
  Logos *logos2 = 0;
  
  cout << "Creating logos with a NodeID *" << endl;
  logos1 = new Logos(sourceNode2);
  cout << "Deleting it" << endl;
  delete logos1;

  cout << "Creating logos with a Database *" << endl;
  logos2 = new Logos(sourceNode1);
  cout << "Deleting it" << endl;
  delete logos2;

  exit(0);
}
