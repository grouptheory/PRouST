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
"$Id: linktst.cc,v 1.11 1999/02/19 21:22:52 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include "SwitchFunctions.h"
#include <DS/util/String.h>
#include <FW/basics/Conduit.h>
#include <codec/pnni_ig/id.h>
#include <iostream.h>
#include <fsm/config/Configurator.h>
#include <sim/port/LinkStateWrapper.h>

int main(int argc, char** argv)
{
  int answer = 0;
  NodeID *switch1Node = 0;
  Conduit *switch1Control = 0;

  NodeID *switch2Node = 0;
  Conduit *switch2Control = 0;

  LinkStateWrapper *LinkStateWrappers[9];
  int i = 0;

  Conduit *switch1 = 0;
  Conduit *switch2 = 0;

  if (argc < 3) {
    cout << "Usage: " << argv[0] << "configuration_file_name visualizer_pipe_name" << endl;
    answer = 1;
  }
  else {
    const Configurator & globalConfig = theConfigurator((const char *)argv[1]);
    VisPipe(argv[2]);

    cout << "Creating switch1" << endl;
    ds_String *switch1Key = new ds_String("Switch1");
    NodeID *switch1NodeID = globalConfig.BaseNodeID(switch1Key);
    switch1 = MakeSwitch(switch1Key, switch1Key, switch1NodeID,
			 switch1Control);

    cout << "Creating switch2" << endl;
    ds_String *switch2Key = new ds_String("Switch2");
    NodeID *switch2NodeID = globalConfig.BaseNodeID(switch2Key);
    switch2 = MakeSwitch(switch2Key, switch2Key, switch2NodeID, 
			 switch2Control);

    ds_String *forwardKey = new ds_String("Switch1-Switch2");
    ds_String *backwardKey = new ds_String("Switch2-Switch1");

    for (i=0; i < 9; i++) {
      cout << "Linking switch1 port " << i + 1 
	   << " and switch2 port " << i + 1;

      if ((LinkStateWrappers[i] = LinkSwitches(switch1, i + 1, switch2, i + 1,
					       forwardKey, backwardKey))
	   != 0) cout << " SUCCEEDED!" << endl;
      else cout << " FAILED!" << endl;

    }

    for (i = 0; i < 9; i++) {
      cout << "Unlinking switch1 port " << i + 1 
	   << " and switch2 port " << i + 1;
      if (UnlinkSwitches(LinkStateWrappers[i])) cout << " SUCCEEDED!" << endl;
      else cout << " FAILED!" << endl;
    }

    delete forwardKey;
    delete backwardKey;

    cout << "Deleting switch1" << endl;
    delete switch1;
    delete switch1Node;

    cout << "Deleting switch2" << endl;
    delete switch2;
    delete switch2Node;

  }

  //  memory_clear();
  memory_kill();
  exit(answer);
}
