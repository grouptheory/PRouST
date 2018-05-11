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
 * File: main.cc
 * Author: mountcas
 * Version: $Id: linktst.cc,v 1.2 1999/02/19 21:22:55 marsh Exp $
 *
 * Usage: linktst input_file [debug]
 *
 * where debug is any string.  If it's specified, the program runs
 * in debug mode, spewing forth great gobs and chunks of internal
 * parser information.
 */
#ifndef LINT
static char const _main_cc_rcsid_[] =
"$Id: linktst.cc,v 1.2 1999/02/19 21:22:55 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <DS/util/String.h>
#include <FW/basics/diag.h>

#if YYDEBUG != 0
extern int PNNIConfiguratordebug;
#endif

#include "Configurator.h"

int main(int argc, char** argv)
{
  int answer = 1;

  if (argc >= 2) {

#if YYDEBUG != 0
    if (argc > 2) PNNIConfiguratordebug = 1;
#endif

    Configurator *theConfigurator = new Configurator(argv[1]);
    if (theConfigurator->good()) {
      answer = 0;

      theConfigurator->Print(cout);

      ds_String *search1 = new ds_String("Test1");
      cout << endl;
      cout << "Looking for " << *search1 << endl;
      int result = theConfigurator->Ports(search1);
      cout << "It has " << result << " ports.  Is that right?" << endl;
    }

    delete theConfigurator;
  }

  return answer;
}
