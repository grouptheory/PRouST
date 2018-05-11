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

/*
 * File: loaddb.cc
 * Author: battou
 * Version: $Id: dbtest2.cc,v 1.2 1998/08/06 04:04:43 bilal Exp $
 * Purpose: Test the reading of a database from a file.
 */

#ifndef LINT
static char const _dbtest_cc_rcsid_[] =
"$Id: dbtest2.cc,v 1.2 1998/08/06 04:04:43 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/basics/FW.h>
#include <codec/pnni_pkt/pkt_incl.h>
#include <codec/uni_ie/ie.h>
#include <codec/pnni_ig/id.h>

#include <fsm/database/Database.h>
#include <fsm/database/InitDB.h>

extern void mallocbug(int);
void f(int argc, char **argv);
 
void main(int argc, char **argv)
{
  f(argc,argv);
}

void f(int argc, char **argv)
{
  FILE     * fp;
  Conduit  * dbConduit1 = 0;
  Database * db1 = 0;
  NodeID   * ppgl1 = 0;
  NodeID   * n1 = 0;
  NodeID   * pn1 = 0;

  Conduit  * dbConduit2 = 0;
  Database * db2 = 0;
  NodeID   * ppgl2 = 0;
  NodeID   * n2 = 0;
  NodeID   * pn2 = 0;

  NodeID   * ppgl3 = 0;
  NodeID   * n3 = 0;
  NodeID   * pn3 = 0;

  NodeID   * ppgl4 = 0;
  NodeID   * n4 = 0;
  NodeID   * pn4 = 0;

  if (!(fp = fopen(argv[1],"r"))) {
    cout << "ERROR: Unable to open file '" << argv[1] << "' for reading." << endl;
    exit(1);
  }
  cout << "Opening '" << argv[1] << "'." << endl;

  dbConduit1 = InitDB(fp, argv[1], db1, n1, pn1, ppgl1, 0, 96);

if (!(fp = fopen(argv[2],"r"))) {
    cout << "ERROR: Unable to open file '" << argv[1] << "' for reading." << endl;
    exit(1);
  }

  dbConduit2 = InitDB(fp, argv[2], db2, n2, pn2, ppgl2, 0, 96);

if (!(fp = fopen(argv[3],"r"))) {
    cout << "ERROR: Unable to open file '" << argv[1] << "' for reading." << endl;
    exit(1);
  }

  InitDB(fp, "db1.txt", db1, n3, pn3, ppgl3, 1, 72);

if (!(fp = fopen(argv[4],"r"))) {
    cout << "ERROR: Unable to open file '" << argv[1] << "' for reading." << endl;
    exit(1);
  }

  InitDB(fp, "db2.txt", db2, n4, pn4, ppgl4, 1, 72);

  DatabaseSumPkt * dsl1 = db1->GetDatabaseSummary(96); 

  cout << "The Database summary for the level 96 db1 \n" << endl; 
  if(dsl1){
   cout << *dsl1 << endl;
  }
  else{
   cout << "ERROR without Level 1 Summary for db1 \n" << endl;
  }
  
  DatabaseSumPkt * dsl2 = db1->GetDatabaseSummary(72);

  if(dsl2){
    cout << "The Database summary for the level 72 db1 \n" << endl;
    cout << *dsl2 << endl;
  }
  else{
   cout << "ERROR without Level 2 Summary for db1 \n" << endl;
  }

  dsl1 = db2->GetDatabaseSummary(96);
  
  if(dsl1){
    cout << "The Database summary for the level 96 db2 \n" << endl;
    cout << *dsl1 << endl;
  }
  else{
   cout << "ERROR without Level 1 Summary for db2 \n" << endl;
  }

    dsl2 = db2->GetDatabaseSummary(72);

  if(dsl2){ 
    cout << "The Database summary for the level 72 db2 \n" << endl;
    cout << *dsl2 << endl;
  }
  else{
    cout << "ERROR without Level 2 Summary for db2  \n" << endl;
  }

}
