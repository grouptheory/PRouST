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
 * Version: $Id: test1.cc,v 1.35 1998/09/01 20:47:05 mountcas Exp $
 * Purpose: Test the reading of a database from a file.
 */

#ifndef LINT
static char const _loaddb_cc_rcsid_[] =
"$Id: test1.cc,v 1.35 1998/09/01 20:47:05 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/basics/diag.h>
#include <FW/basics/FW.h>
#include <FW/interface/ShareableRegistry.h>
#include <codec/pnni_pkt/pkt_incl.h>
#include <codec/uni_ie/ie.h>
#include <codec/pnni_ig/id.h>

#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/database/Database.h>
#include <fsm/database/ExtInitDB.h>
#include <fsm/hello/HelloState.h>
#include <fsm/hello/DBtrapper.h>
#include <fsm/queue/QueueState.h>
#include <fsm/events/UsageEvent.h>

#include <FW/basics/diag.h>

extern "C" {
#include <unistd.h>
  //  char * getcwd(char * buf, int size);
};

#define QUEUE_SVC_INTERVAL 0.01

class HelloTerm : public Terminal {
public:

  HelloTerm(void) {
    UsageEvent *ue = new UsageEvent(1.0, true);
    Register(ue);
  }
  virtual ~HelloTerm() { }

  void Interrupt(SimEvent* e) {
    cout << "I was Interrupted by event " << e->GetID() << "!\n";
  }

  void Absorb(Visitor * v) {
    v->Suicide();
  }
};

class TestExpander : public Expander {
public:

  TestExpander(const char * dbfilename) {
    FILE * fp;    
    if (!(fp = fopen(dbfilename, "r"))) {
      cerr << "ERROR: Unable to open file '" << dbfilename << "' for reading." << endl;
      exit(1);
    }

    NodeID * nid = 0, * pn = 0, * ppgl = 0;
    Database * db;
    Conduit * database = ExtInitDB(fp, "Database", db, nid, pn, ppgl);

    // Starters  to inject PortUpVisitor's on the 2 Hello's
    HelloTerm * t1 = new HelloTerm();
    Adapter   * a1 = new Adapter(t1);
    Conduit   *tc1 = new Conduit("HelloTerm", a1);

    HelloState * fsm1 = new HelloState(*db, nid, 1, 1, 1, 1);
    Protocol   * pro1 = new Protocol(fsm1);
    Conduit    * hc1  = new Conduit("HelloState", pro1);

    QueueState * q1  = new QueueState(QUEUE_SVC_INTERVAL, 100);
    Protocol   * pq1 = new Protocol(q1);
    Conduit    * qc1 = new Conduit("QueueState", pq1);

    // join side A of terminal to side B of database
    Join( A_half(tc1), B_half(database) );
    // join side A of database to side A of Hello
    Join( A_half(hc1), A_half(database) );
    // join side B half of Hello to A helf of Q
    Join( B_half(hc1), A_half(qc1) );

    _queue = qc1;
    _hello = hc1;
    _term  = tc1;
    _db    = database;

    // This makes the Expander blocking so QueryInterfaces will not leave it
    SetBlocking();

    DefinitionComplete();
  }

  virtual ~TestExpander( ) { }

  virtual Conduit * GetAHalf(void) const
  { return A_half(_term); }

  virtual Conduit * GetBHalf(void) const
  { return B_half(_queue); }

private:

  Conduit * _queue;
  Conduit * _hello;
  Conduit * _term;
  Conduit * _db;
};

extern int INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME;

void main(int argc, char **argv)
{
  char path1[256];
  char path2[256];
  char filename[80];

  DiagLevel(FSM_HELLO,DIAG_DEBUG);

  if (argc >= 3) {
    strcpy(path1,argv[1]);
    strcpy(path2,argv[2]);
  } else {
    if (getcwd(path1,80) == NULL) {
      cerr << "ERROR: Unable to get CWD!" << endl;
      exit(1);
    }
    strcat(path1,"/");
    strcpy(path2, path1);
    cout << "Enter filename for DB1: ";
    cin >> filename;
    strcat(path1, filename);
    cout << "Enter filename for DB2: ";
    cin >> filename;
    strcat(path2, filename);
  }
  
  int speedvalue;
  if (argc <= 3) {
    cout << "Enter log_2 speed (-2,-1,0,1,2) or enter 666 for sim-time: ";
    cin >> filename;
    speedvalue = atoi(filename);
  } else
    speedvalue = atoi(argv[3]);

  double simtime;
  if (argc <= 4) {
    cout << "How long (seconds) to run the simulation: ";
    cin >> filename;
    simtime = atof(filename);
  } else
    simtime = atof(argv[4]);
  
  int debug;
  if (argc <= 5) {
    cout << "(0)= Run, (1)= Debug, Enter 1 or 0: ";
    cin >> filename;
    debug = atoi(filename);
  } else
    debug = atoi(argv[5]);
  
  if (argc <= 6) {
    cout << "Enter the name of the visualizer pipe: ";
    cin >> filename;
  } else
    strcpy(filename, argv[6]);
  
  VisPipe(filename);

  if (argc > 7) {
    // Set diagnostic level for everything
    DiagLevel((diag_internals::diag_level)atoi(argv[7]));
  }

  INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME = 1;

  TestExpander * te1 = new TestExpander(path1);
  Cluster      * clu = new Cluster( te1 );
  Conduit      * ex1 = new Conduit("TestExpander-1", clu);

  TestExpander * te2 = new TestExpander(path2);
                 clu = new Cluster( te2 );
  Conduit      * ex2 = new Conduit("TestExpander-2", clu);

  // trapper between the B halfs of Qs
  DBtrapper * trap = new DBtrapper();
  Protocol  * ptra = new Protocol(trap);
  Conduit   * crap = new Conduit("DBtrapper", ptra);

  // glue in the trapper between Qs
  Join( B_half( ex1 ), A_half(crap) );
  Join( B_half( ex2 ), B_half(crap) );

  theShareableRegistry().save("hfsm.vis");

  Kernel & controller = theKernel();
  controller.SetSpeed( (Kernel::Speed) speedvalue);

  // Right here it should register a stop timer.
  controller.StopIn(simtime);

  if (debug) controller.Debug();
  else controller.Run();
}
