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
 * Version: $Id: test3.cc,v 1.2 1998/08/06 04:04:54 bilal Exp $
 * Purpose: Test the reading of a database from a file.
 */

#ifndef LINT
static char const _loaddb_cc_rcsid_[] =
"$Id: test3.cc,v 1.2 1998/08/06 04:04:54 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/basics/FW.h>
#include <codec/pnni_pkt/pkt_incl.h>
#include <codec/uni_ie/ie.h>
#include <codec/pnni_ig/id.h>

#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/database/Database.h>
#include <fsm/database/InitDB.h>
#include <fsm/hello/HelloState.h>
#include <fsm/hello/DBtrapper.h>
#include <fsm/nodepeer/NodePeerState.h>

#include <fsm/queue/QueueState.h>

extern "C" {
  char * getcwd(char *, int);
};

class StartTimer : public TimerHandler {
public:
  StartTimer(Terminal *t, Visitor *v);
  ~StartTimer();
 void Callback(void);
  Terminal *_t;
  Visitor *_v;
};


class HelloTerm: public Terminal{
public:
  HelloTerm(NodeID *, NodeID *);
  ~HelloTerm();
  void Interrupt(SimEvent* e) {
    cout << "I was Interrupted by event " << e->GetID() << "!\n";
  }

  void Absorb(Visitor * v) {
    v->Suicide();
  }
  
  StartTimer *_go1;
};


StartTimer::StartTimer(Terminal *t, Visitor *v):TimerHandler(t,1.0),
  _t(t),_v(v){};
StartTimer::~StartTimer(){};

void StartTimer::Callback(void)
{
  _t->Inject(_v);
}

HelloTerm::HelloTerm(NodeID * myNode, NodeID * remNode)
{
 // Agg token 0, port id 1
  FastUNIVisitor *v = new FastUNIVisitor(myNode, remNode, 1, 0x05, 0, FastUNIVisitor::FastUNIUplinkResponse);
  v->SetMSGType(FastUNIVisitor::FastUNIUplinkResponse);
  _go1 = new StartTimer(this,v);
  Register(_go1);
}

HelloTerm::~HelloTerm(){};

void main(int argc, char *argv[])
{
  SVCHelloState::primal_type pt = SVCHelloState::primal_state;
  LgnHelloState::primal_type lpt = LgnHelloState::primal_state;

  char path1[256];
  char path2[256];
  char path3[256];
  char path4[256];
  char filename[80];
  FILE *fp;

  Conduit *dc1 = 0;
  Conduit *dc2 = 0;
  Database *db1 = 0;
  Database *db2 = 0; 

  NodeID *n1 = 0;
  NodeID *n2 = 0;
  PeerGroupID *pgid1 = 0;
  PeerGroupID *pgid2 = 0;
  NodeID *pn1 = 0;
  NodeID *pn2 = 0;
  u_int p1 = 1;
  u_int p2 = 2;
  u_short cver = 1;
  u_short nver = 1;
  u_short over = 1;
  int vpi = 0; 
  int vci = 78;

  if (argc >= 5) {
    strcpy(path1, argv[1]);
    strcpy(path2, argv[2]);
    strcpy(path3, argv[3]);
    strcpy(path4, argv[4]);
  } else {
    if (!getcwd(path1,80)) {
      cout << "can't get CWD \n";
      exit(1);
    }
    strcat(path1,"/");
    strcpy(path2,path1);
    strcpy(path3, path1);
    strcpy(path4, path1);
    cout << "Enter filename for DB1: ";
    cin >> filename;
    strcat(path1,filename);
    cout << "Enter filename for DB2: ";
    cin >> filename;
    strcat(path2,filename);
    cout << "Enter filename1 for Uplinks: ";
    cin >> filename; 
    strcat(path3, filename);
    cout << "Enter filename2 for Uplinks: ";
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
    cout << "How long (seconds) to run the simulation: ";
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

  NodeID * ppgl1 = 0;

  if (!(fp = fopen(path1,"r"))) {
    cout << "can't open file '" << path1 << "' for reading." << endl;
    exit(1);
  }
  dc1 = InitDB(fp, "DB1", db1, n1, pn1, ppgl1);

  if (!(fp = fopen(path2, "r"))) {
    cout << "can't open file '" << path2 << "' for reading." << endl;
    exit(1);
  }
  NodeID* ppgl2=0;
  dc2 = InitDB(fp, "DB2", db2, n2, pn2,ppgl2);



  SVCHelloState *fsm1 = new SVCStateLinkDown(*db1, pt,n1, 0,vpi,vci,cver,nver,over);
  Protocol * pro1 = new Protocol(fsm1);
  Conduit *hc1 = new Conduit("SVH1", pro1);
  fsm1->SetName("SVH1");

  SVCHelloState *fsm2 = new SVCStateLinkDown(*db2, pt,n2,n1,vpi,vci,cver,nver,over);
  Protocol * pro2 = new Protocol(fsm2);
  Conduit *hc2 = new Conduit("SVH2", pro2);
  fsm1->SetName("SVH2");

  LgnHelloState *fsm3 = new LgnStateLinkDown(lpt,n1, 0); // aggr 0 for now 
  Protocol * pro3 = new Protocol(fsm3);
  Conduit *hc3 = new Conduit("LVH1", pro3);
  fsm3->SetName("LVH1");
  
  LgnHelloState *fsm4 = new LgnStateLinkDown(lpt, n2, 0);
  Protocol * pro4 = new Protocol(fsm4);
  Conduit *hc4 = new Conduit("LVH4", pro4);
  fsm4->SetName("LVH2");
  
  NodePeerState *np1 = new NodePeerState(n1,n2,db1);
  Protocol * np_pro1 = new Protocol(np1);
  Conduit      *npc1 = new Conduit("NP1", np_pro1);

  NodePeerState *np2 = new NodePeerState(n2,n1,db2);
  Protocol * np_pro2 = new Protocol(np2);
  Conduit      *npc2 = new Conduit("NP2", np_pro2);


 QueueState* q1=new QueueState(0.0, 100);
 Protocol * pq1 = new Protocol(q1);
 Conduit *qc1 = new Conduit("Queue1", pq1);

 QueueState* q2=new QueueState(0.0, 100);
 Protocol * pq2 = new Protocol(q2);
  Conduit *qc2 = new Conduit("Queue2", pq2);
  
  // Starters  to inject PortUpVisitor's on the 2 Hello's
  HelloTerm *t1 = new HelloTerm(n1, n2 );
  Adapter *a1 = new Adapter(t1);
  Conduit *tc1 = new Conduit("Term1",a1);

  HelloTerm *t2 = new HelloTerm(n2, n1);
  Adapter *a2 = new Adapter(t2);
  Conduit *tc2 = new Conduit("Term2",a2);

  // join side A of terminals to side B of database
  Join( A_half(tc1), B_half(dc1) );
  Join( A_half(tc2), B_half(dc2) );

  // join side A of databases to side B of NPs
  Join( B_half(npc1), A_half(dc1) );
  Join( B_half(npc2), A_half(dc2) );

  // join side A side of the NPs to side B of LGN's
  Join( B_half(hc3), A_half(npc1) );
  Join( B_half(hc4), A_half(npc2) );


  // join side A half of LGN's to B half of SVC Hello's
  Join(A_half(hc3), B_half(hc1));
  Join(A_half(hc4), B_half(hc2));

 // join side A half of  SVC hello's to A half of Q's 
  Join( A_half(hc1), A_half(qc1) );
  Join( A_half(hc2), A_half(qc2) );

//    Join( A_half(hc1), A_half(hc2) );

  // trapper between the B halfs of Qs

  DBtrapper* trap = new DBtrapper();
  Protocol * ptrap = new Protocol(trap);
  Conduit * ctrap = new  Conduit("dbvisitor_trap",ptrap);

// glue in the trapper between Qs

  Join( B_half(qc1), A_half(ctrap) );
  Join( B_half(qc2), B_half(ctrap) );
//    Join( B_half(qc1), B_half(qc2));

  if (!(fp = fopen(path3,"r"))) {
    cout << "can't open file '" << path3 << "' for reading." << endl;
    exit(1);
  }
//  InitDB(fp, "upl1", db1, n1, pn1, ppgl1, 1);

  if (!(fp = fopen(path4,"r"))) {
    cout << "can't open file '" << path4 << "' for reading." << endl;
    exit(1);
  }

//  InitDB(fp, "upl2", db2, n2, pn2, ppgl2, 1);

  Kernel& controller=theKernel();
  controller.SetSpeed( (Kernel::Speed) speedvalue);

  controller.StopIn(simtime); // stop in 100 seconds!

  if (debug) controller.Debug();
  else controller.Run();

  cout << "Simulation ended (\ab\ae\ae\ae\a\e\ae\ae\ae\ap\a!), Press CTRL-C to exit out." << endl;
  system("sleep 9999");
}
