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
 * Version: $Id: test1.cc,v 1.3 1998/08/06 04:05:36 bilal Exp $
 * Purpose: Test the reading of a database from a file.
 */

#ifndef LINT
static char const _loaddb_cc_rcsid_[] =
"$Id: test1.cc,v 1.3 1998/08/06 04:05:36 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/basics/FW.h>
#include <codec/pnni_pkt/pkt_incl.h>
#include <codec/uni_ie/ie.h>
#include <codec/pnni_ig/id.h>

#include <fsm/visitors/LinkUpVisitor.h>
#include <fsm/database/Database.h>
#include <fsm/database/ExtInitDB.h>
#include <fsm/hello/HelloState.h>
#include <fsm/hello/DBtrapper.h>
#include <sim/port/LinkState.h>
#include <fsm/events/UsageEvent.h>

#include <FW/basics/diag.h>

extern "C" {
#include <assert.h>
  char * getcwd(char * buf, int size);
};

#define QUEUE_SVC_INTERVAL 0.0

class HelloTerm: public Terminal{
public:
  HelloTerm() 
	{ UsageEvent *ue = new UsageEvent(1.0);
	  Register(ue);
	}
  ~HelloTerm() {}
  void Interrupt(SimEvent* e) {
    cout << "I was Interrupted by event " << e->GetID() << "!\n";
  }

  void Absorb(Visitor * v) {
    v->Suicide();
  }
};

extern int INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME;

main(int argc, char *argv[])
{
  HelloState::primal_type pt = HelloState::primal_state;
  char path1[256];
  char path2[256];
  char filename[80];
  FILE * fp;
  Conduit *dc1 = 0;
  Conduit *dc2 = 0;
  Database *db1 = 0;
  Database *db2 = 0; 

  NodeID *n1 = 0;
  NodeID *n2 = 0;
  NodeID *pn1 = 0;
  NodeID *pn2 = 0;

  u_int p1 = 1;
  u_int p2 = 2;
  u_short cver = 1;
  u_short nver = 1;
  u_short over = 1;

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

  if (!(fp = fopen(path1, "r"))) {
    cerr << "ERROR: Unable to open file '" << path1 << "' for reading." << endl;
    exit(1);
  }

  DiagLevel(FSM_HELLO, DIAG_DEBUG);
  DiagLevel(CODEC, DIAG_ERROR);
  DiagLevel(FW, DIAG_ERROR);
  DiagLevel(SIM, DIAG_DEBUG);
  

  INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME=1;

  NodeID* ppgl1=0;
  dc1 = ExtInitDB(fp, "DB1", db1, n1, pn1, ppgl1);

  if (!(fp = fopen(path2, "r"))) {
    cerr << "ERROR: Unable to open file '" << path2 << "' for reading." << endl;
    exit(1);
  }

  NodeID * ppgl2=0;
  dc2 = ExtInitDB(fp, "DB2", db2, n2, pn2, ppgl2);

  // Starters  to inject PortUpVisitor's on the 2 Hello's
  HelloTerm *t1 = new HelloTerm();
  Adapter *a1 = new Adapter(t1);
  Conduit *tc1 = new Conduit("Term1",a1);

  HelloTerm *t2 = new HelloTerm();
  Adapter *a2 = new Adapter(t2);
  Conduit *tc2 = new Conduit("Term2",a2);

  HelloState *fsm1 = new StateLinkDown(*db1, pt,n1,p1,cver,nver,over);
  Protocol * pro1 = new Protocol(fsm1);
  Conduit *hc1 = new Conduit("H1", pro1);
  fsm1->SetName("H1");

  HelloState *fsm2 = new StateLinkDown(*db2, pt,n2,p2,cver,nver,over);
  Protocol * pro2 = new Protocol(fsm2);
  Conduit *hc2 = new Conduit("H2", pro2);
  fsm2->SetName("H2");
  
  PurifyingLinkState * l=new PurifyingLinkState(1,1);
  Protocol * pl = new Protocol(l);
  Conduit *lc = new Conduit("Link", pl);

  // join side A of terminals to side B of database
  Join( A_half(tc1), B_half(dc1) );
  Join( A_half(tc2), B_half(dc2) );

  // join side A of databases to side A of Hellos
  Join( A_half(hc1), A_half(dc1) );
  Join( A_half(hc2), A_half(dc2) );

  // join side B half of Hellos to A helf of Qs
  Join( B_half(hc1), A_half(lc) );
  Join( B_half(hc2), B_half(lc) );

  LinkUpVisitor * luv1 = new LinkUpVisitor(p1);
  LinkUpVisitor * luv2 = new LinkUpVisitor(p2);

  luv1->SetLoggingOn();
  luv2->SetLoggingOn();

  B_half(hc1)->Accept(luv1);
  B_half(hc2)->Accept(luv2);

  Kernel& controller=theKernel();
  controller.SetSpeed( (Kernel::Speed) speedvalue);

  controller.StopIn(simtime); // stop in 100 seconds!

  if (debug) controller.Debug();
  else controller.Run();

  //  cout << "Simulation ended (beep), Press CTRL-C to exit out.\n";
  // system("sleep 9999");
}
