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
static char const _malloc_test_cc_rcsid_[] =
"$Id: malloc-test.cc,v 1.2 1998/08/06 04:04:54 bilal Exp $";
#endif
#include <common/cprototypes.h>
#include <fsm/database/Database.h>
#include <fsm/database/InitDB.h>

class Conduit;
class Database;
class NodeID;
class PeerGroupID;

main(int argc, char *argv[])
{
  FILE *fp;

  Conduit  *dc = 0;
  Database *db = 0;
  NodeID   *n = 0;
  NodeID   *pn = 0;
  NodeID   *ppgl=0;
  PeerGroupID *pgid = 0;

  if (argc < 3) {
    cout << "usage: " << argv[0] << " db1.txt db2.txt db3.txt" << endl;
    exit(0);
  }

  for (int i = 0; i < 3; i++) {
    if (!(fp = fopen(argv[i + 1], "r"))) {
      printf("can't open file %s for reading\n", argv[i + 1]);
      exit(1);
    }
    dc = InitDB(fp, "DB", db, n, pn, ppgl);
  }
  cout << "Done." << endl;
}
