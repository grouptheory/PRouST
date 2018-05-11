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
static char const _test1_cc_rcsid_[] =
"$Id: test_level.cc,v 1.3 1998/08/06 04:05:05 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>

#include <FW/basics/FW.h>

#include <codec/pnni_pkt/pkt_incl.h>
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/ie.h>

#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/hello/HelloState.h>
#include <fsm/queue/QueueState.h>
#include <fsm/hello/DBtrapper.h>
#include <fsm/database/Database.h>
#include <fsm/database/InitDB.h>
#include <fsm/nodepeer/NodePeerState.h>

#include <FW/basics/diag.h>

extern "C" {
#include <stdio.h>
#include <sys/types.h>
  char * getcwd(char *, int);
  caddr_t sbrk(int);
};

void ss(void){
  printf("sbrk=%x\n",sbrk(0));
}

#define QUEUE_SVC_INTERVAL 1.0

class StartTimer : public TimerHandler {
public:

  StartTimer(Terminal *t, Visitor *v) : TimerHandler(t,0), _t(t),_v(v) { }

  ~StartTimer() { }

  void Callback(void) { _t->Inject(_v); }
  Terminal *_t;
  Visitor  *_v;
};


class HelloTerm : public Terminal {
public:

  HelloTerm() {
    Visitor * v = new PortUpVisitor();
    _go1 = new StartTimer(this,v);
    Register(_go1);
  }
  ~HelloTerm() { delete _go1; }
  void Interrupt(SimEvent* e) {
    cout << "I was Interrupted by event " << e->GetID() << "!\n";
  }

  void Absorb(Visitor * v) {
    v->Suicide();
  }
  StartTimer *_go1;
};


// NodeID: 72:160:47.00-05-80-ff-de-00-00-01-00-00-04-03.ff-1c-06-92-00-01.00
int main(int argc, char **argv)
{
#if 0
  // Let's see if Insight is earning its keep ...
  char x[10];
  char * y = x + 40;
  // it only picks up the line below ...
  strcpy(y, "123456789012345678901234567890");
#endif

  // free( malloc( 900000 ) ); // abracadabra!

  NodeID * n1 = 0, * n2 = 0, * n3 = 0, * n4 = 0, * pn1 = 0, * pn2 = 0,
           * pn3 = 0, * pn4 =0;
  PeerID * p1 = 0, * p2 = 0;
  NodeID * ppgl2=0, * ppgl1 = 0, *ppgl3 = 0, * ppgl4 = 0;

  HelloState::primal_type pt = HelloState::primal_state;
  char path1[256], path2[256], path3[256], path4[256], filename[80];
  FILE * fp;


  if (argc >= 5) {
    strcpy(path1, argv[1]);
    strcpy(path2, argv[2]);
    strcpy(path3, argv[3]);
    strcpy(path4, argv[4]);
  } else {
    if (getcwd(path1,80) == NULL) {
      cerr << "can't get CWD!" << endl;
      exit(1);
    }
    strcat(path1, "/");
    strcpy(path2, path1);
    strcpy(path3, path1);
    strcpy(path4, path1);
    cout << "Enter the name of the level 1 initialization file for the first Database: ";
    cin >> filename;
    strcat(path1, filename);
    cout << "Enter the name of the level 1 initialization file for the second Database: ";
    cin >> filename;
    strcat(path2, filename);

    cout << "Enter the name of the level 2 initialization file for the first Database: ";
    cin >> filename;
    strcat(path3, filename);

    cout << "Enter the name of the level 2 initialization file for the second Database: ";
    cin >> filename;
    strcat(path4, filename);
  
  }
  
  int speedvalue;
  if (argc <= 5) {
    cout << "Enter log_2 speed (-2,-1,0,1,2) or enter 666 for sim-time: ";
    cin >> filename;
    speedvalue = atoi(filename);
  } else
    speedvalue = atoi(argv[5]);

  double simtime;
  if (argc <= 6) {
    cout << "How long (in seconds) to run the simulation: ";
    cin >> filename;
    simtime = atof(filename);
  } else
    simtime = atof(argv[6]);
  
  int debug;
  if (argc <= 7) {
    cout << "(0)= Run, (1)= Debug, Enter 1 or 0: ";
    cin >> filename;
    debug = atoi(filename);
  } else
    debug = atoi(argv[7]);
  
  if (argc <= 8) {
    cout << "Enter the name of the visualizer pipe: ";
    cin >> filename;
  } else
    strcpy(filename, argv[8]);
  
  VisPipe(filename);

  if (argc > 9) {
    // Set diagnostic level for everything
    DiagLevel((diag_internals::diag_level)atoi(argv[7]));
  }

  if (!(fp = fopen(path1,"r"))) {
    cerr << "ERROR: Unable to open file " << path1 << " for reading." << endl;
    exit(1);
  }
  Database   * db1, * db2;
  const char * db1name = "DB1";
  Conduit    * dc1 = InitDB(fp, db1name, db1, n1, pn1, ppgl1, 0, 96);

  if (!(fp = fopen(path2,"r"))) {
    cerr << "ERROR: Unable to open file " << path2 << " for reading." << endl;
    exit(1);
  }
  const char * db2name = "DB2";
  Conduit    * dc2 = InitDB(fp, db2name, db2, n2, pn2, ppgl2, 0, 96);

  if (!(fp = fopen(path3,"r"))) {
    cerr << "ERROR: Unable to open file " << path3 << " for reading." << endl;
    exit(1);
  }

  InitDB(fp, db1name, db1, n3, pn3, ppgl3, 1, 72);

  if (!(fp = fopen(path4,"r"))) {
    cerr << "ERROR: Unable to open file " << path4 << " for reading." << endl;
    exit(1);
  }

  InitDB(fp, db1name, db2, n4, pn4, ppgl4, 1, 72);

  // Starters  to inject PortUpVisitor's on the 2 Hello's
  HelloTerm *t1 = new HelloTerm();
  Adapter   *a1 = new Adapter(t1);
  Conduit  *tc1 = new Conduit("Term1",a1);

  HelloTerm *t2 = new HelloTerm();
  Adapter   *a2 = new Adapter(t2);
  Conduit  *tc2 = new Conduit("Term2",a2);

  NodePeerState *np1 = new NodePeerState(n1,n2,db1);
  Protocol * np_pro1 = new Protocol(np1);
  Conduit      *npc1 = new Conduit("NP1", np_pro1);

  NodePeerState *np2 = new NodePeerState(n2,n1,db2);
  Protocol * np_pro2 = new Protocol(np2);
  Conduit      *npc2 = new Conduit("NP2", np_pro2);


  NodePeerState *np3 = new NodePeerState(n3,n4,db1);
  Protocol * np_pro3 = new Protocol(np3);
  Conduit      *npc3 = new Conduit("NP3", np_pro3);

  NodePeerState *np4 = new NodePeerState(n4,n3,db2);
  Protocol * np_pro4 = new Protocol(np4);
  Conduit      *npc4 = new Conduit("NP4", np_pro4);

  QueueState* q1=new QueueState(QUEUE_SVC_INTERVAL, 100);
  Protocol * pq1 = new Protocol(q1);
  Conduit *qc1 = new Conduit("Queue1", pq1);

  QueueState* q2=new QueueState(QUEUE_SVC_INTERVAL, 100);
  Protocol * pq2 = new Protocol(q2);
  Conduit *qc2 = new Conduit("Queue2", pq2);
  
  // join side A of terminals to side B of database
  Join( A_half(tc1), B_half(dc1) );
  Join( A_half(tc2), B_half(dc2) );

  // join side A of Database to B side of NP level 1s 
  Join(A_half(dc1), B_half(npc1) );
  Join(A_half(dc2), B_half(npc2) );

  // join side A half of NP level 1 's to B half of NP level 2's
  Join( B_half(npc3), A_half(npc1) );
  Join( B_half(npc4), A_half(npc2) );

  // join side A half of NP level 2's to A half of Q's
  Join( A_half(npc3), A_half(qc1) );
  Join( A_half(npc4), A_half(qc2) );

  // trapper between the B halfs of Qs
  DBtrapper* trap = new DBtrapper();
  Protocol * ptrap = new Protocol(trap);
  Conduit * ctrap = new  Conduit("dbvisitor_trap",ptrap);

  // glue in the trapper between Qs
  Join( B_half(qc1), A_half(ctrap) );
  Join( B_half(qc2), B_half(ctrap) );

  Kernel & controller = theKernel();
  controller.SetSpeed( (Kernel::Speed) speedvalue);
  controller.StopIn(simtime); 

  if (debug) 
    controller.Debug();
  else 
    controller.Run();

  cout << "Simulation ended (\a\abeep), Press CTRL-C to exit out.\n";
  // system("sleep 9999");
}
