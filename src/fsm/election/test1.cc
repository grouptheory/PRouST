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
 * Version: $Id: test1.cc,v 1.11 1998/08/06 04:04:46 bilal Exp $
 * Purpose: Test the reading of a database from a file.
 */

#ifndef LINT
static char const _loaddb_cc_rcsid_[] =
"$Id: test1.cc,v 1.11 1998/08/06 04:04:46 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/basics/diag.h>

#include <fsm/election/ElectionState.h>
#include <fsm/election/ElectionVisitor.h>
#include <fsm/hello/HelloState.h>
#include <codec/pnni_pkt/pkt_incl.h>
#include <codec/pnni_ig/id.h>

#include <fsm/election/test1.h>
#include <fsm/nodepeer/NodePeerState.h>
#include <fsm/database/Database.h>
#include <fsm/database/InitDB.h>
#include <fsm/hello/DBtrapper.h>
#include <fsm/visitors/LinkUpVisitor.h>
#include <fsm/queue/QueueState.h>

extern "C" {
  char * getcwd(char *, int);
};

void HelloTerm::Absorb(Visitor * v) 
{
  const VisitorType *vt2;
  const VisitorType vt1 = v->GetType();
  vt2 = QueryRegistry(ELECTION_VISITOR_NAME);
  if (vt2 && vt1.Is_A(vt2)) {
    delete vt2;
    if (((ElectionVisitor *)v)->GetPurpose() == ElectionVisitor::ToOriginateANodalinfo) {
      ElectionVisitor * ev = (ElectionVisitor *)v;
      u_char node[22];
      u_char * pg = 0;
      bcopy((u_char *)ev->GetNode()->GetNID(), node, 22);
      if (ev->GetPGL()) {
        pg = new u_char[22];
        cout << "ABSORB: The PGL is : "<< *ev->GetPGL() << endl;
        bcopy((u_char *)ev->GetPGL()->GetNID(), pg, 22);
      } 
      
      ig_nodal_info_group * nig = new ig_nodal_info_group(node, (u_char)ev->GetPriority(), ((u_char)00), pg);
      delete pg;
      seq_num++;
      ig_ptse * ptse = new ig_ptse(InfoGroup::ig_nodal_info_group_id, 1, seq_num, 0, 120);
      ptse->AddIG(nig);
      PeerID * pid = new PeerID((ev->GetNode())->GetNID());
      cout << "From absorb to be inserted nodeid is " << *ev->GetNode() << endl;
      PTSPPkt * ptsp = new PTSPPkt(ev->GetNode()->GetNID(), ev->GetNode()->GetPeerGroup()->GetPGID());
      ptsp->AddPTSE(ptse);
      db->Insert(ptsp, true, ev->GetNode());
      // db->Insert(ev->GetNode(), ptse, pid, 1, 0, 1);
      delete pid;
    } else if (((ElectionVisitor *)v)->GetPurpose() == ElectionVisitor::RequestConnectivityInfo) {
      ElectionVisitor * ev = (ElectionVisitor *)v;
      sortseq<NodeID *, Nodalinfo *> _lis = *(ev->GetNodalInfo());
      seq_item si; 
      forall_items(si, _lis) {
	Nodalinfo * nif = _lis.inf(si);
	nif->SetElectionflag(1);
	cout << "The prority is " << nif->GetPriority() << endl;
      }
    }
  }
  v->Suicide();
}


StartTimer::StartTimer(Terminal *t, Visitor *v):TimerHandler(t,0),
  _t(t),_v(v){};
StartTimer::~StartTimer(){};

void StartTimer::Callback(void)
{
  _t->Inject(_v);
}

HelloTerm::HelloTerm(Database * _db) : db(_db)
{
  Visitor * v = new LinkUpVisitor(0);
  _go1 = new StartTimer(this, v);
  Register(_go1);
  seq_num = 2;
}

HelloTerm::~HelloTerm(){};

void main(int argc, char **argv)
{
  char path1[256];
  char path2[256];
  char filename[80];

  if (argc >= 3) {
    strcpy(path1, argv[1]);
    strcpy(path2, argv[2]);
  } else {
    if (!getcwd(path1,80)) {
      cout << "cant get CWD \n";
      exit(1);
    }
    strcat(path1,"/");
    strcpy(path2,path1);
    cout << "Enter filename for DB1: ";
    cin >> filename;
    strcat(path1,filename);
    cout << "Enter filename for DB2: ";
    cin >> filename;
    strcat(path2,filename);
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

  FILE * fp;

  if (!(fp = fopen(path1,"r"))) {
    cout << "cant open file " << path1 << " for reading." << endl;
    exit(1);
  }

  Database * db1 = 0;
  NodeID * n1 = 0, * pn1 = 0, * ppgl1 = 0;
  Conduit * dc1 = InitDB(fp, "DB1", db1, n1, pn1, ppgl1);

  if (!(fp = fopen(path2, "r"))) {
    cout << "cant open file " << path2 << " for reading." << endl;
    exit(1);
  }

  Database * db2 = 0; 
  NodeID  * n2 = 0, * pn2 = 0, * ppgl2 = 0;
  Conduit * dc2 = InitDB(fp, "DB2", db2, n2, pn2, ppgl2);

  DiagLevel("fsm", DIAG_DEBUG);
  // DiagLevel("fsm.election", DIAG_DEBUG);

  // Starters  to inject PortUpVisitor's on the 2 Hello's
  HelloTerm *t1 = new HelloTerm(db1);
  Adapter *a1 = new Adapter(t1);
  Conduit *tc1 = new Conduit("Term1",a1);

  HelloTerm *t2 = new HelloTerm(db2);
  Adapter *a2 = new Adapter(t2);
  Conduit *tc2 = new Conduit("Term2",a2);

  u_int p1 = 1;
  u_short cver = 1;
  u_short nver = 1;
  u_short over = 1;
  HelloState * fsm1 = new HelloState(*db1, n1, p1, cver,nver,over);
  Protocol * pro1 = new Protocol(fsm1);
  Conduit *hc1 = new Conduit("H1", pro1);

  u_int p2 = 2;
  HelloState *fsm2 = new HelloState(*db2, n2, p2, cver,nver,over);
  Protocol * pro2 = new Protocol(fsm2);
  Conduit *hc2 = new Conduit("H2", pro2);

  NodePeerState *np1 = new NodePeerState(n1,n2,db1);
  Protocol * np_pro1 = new Protocol(np1);
  Conduit      *npc1 = new Conduit("NP1", np_pro1);

  NodePeerState *np2 = new NodePeerState(n2,n1,db2);
  Protocol * np_pro2 = new Protocol(np2);
  Conduit      *npc2 = new Conduit("NP2", np_pro2);

  ElectionState *e1 = new ElectionState(db1, n1, 1);
  Protocol * ep1 = new Protocol(e1);
  Conduit * ec1 = new Conduit("E1", ep1);

  ElectionState *e2 = new ElectionState(db2, n2, 2);
  Protocol * ep2 = new Protocol(e2);
  Conduit * ec2 = new Conduit("E2", ep2);

  QueueState* q1=new QueueState(1.0, 100);
  Protocol * pq1 = new Protocol(q1);
  Conduit *qc1 = new Conduit("Queue1", pq1);

  QueueState* q2=new QueueState(1.0, 100);
  Protocol * pq2 = new Protocol(q2);
  Conduit *qc2 = new Conduit("Queue2", pq2);

  // join side A of terminals to side B of database
  Join( A_half(tc1), B_half(dc1) );
  Join( A_half(tc2), B_half(dc2) );

  // join side A of databases to side B of Elections
  Join( B_half(ec1), A_half(dc1) );
  Join( B_half(ec2), A_half(dc2) );

  if (!(fp = fopen(path1,"r"))) {
    cout << "cant open file " << path1 << " for reading." << endl;
    exit(1);
  }
  NodeID  * nn1 = 0, * npn1 = 0, * nppgl1 = 0;
  Conduit * unused1 = InitDB(fp, "DB1", db1, nn1, npn1, nppgl1, 1);

  if (!(fp = fopen(path2,"r"))) {
    cout << "cant open file " << path2 << " for reading." << endl;
    exit(1);
  }
  NodeID  * nn2 = 0, * npn2 = 0, * nppgl2 = 0;
  Conduit * unused2 = InitDB(fp, "DB2", db2, nn2, npn2, nppgl2, 1);
  
  // join side A of elections to side B of NP's
  Join( B_half(npc1), A_half(ec1) );
  Join( B_half(npc2), A_half(ec2) );

  // join side A side of the NPs to side A of Hellos
  Join( A_half(hc1), A_half(npc1) );
  Join( A_half(hc2), A_half(npc2) );

  // join side B half of Hellos to A helf of Qs
  Join( B_half(hc1), A_half(qc1) );
  Join( B_half(hc2), A_half(qc2) );

  // trapper between the B halfs of Qs
  DBtrapper* trap = new DBtrapper();
  Protocol * ptrap = new Protocol(trap);
  Conduit * ctrap = new  Conduit("dbvisitor_trap",ptrap);

  // glue in the trapper between Qs

  Join( B_half(qc1), A_half(ctrap) );
  Join( B_half(qc2), B_half(ctrap) );

  Kernel& controller=theKernel();
  controller.SetSpeed( (Kernel::Speed) speedvalue);

  controller.StopIn(simtime); // stop in 100 seconds!

  if (debug) controller.Debug();
  else controller.Run();

  cout << "Simulation ended (beep\a!), Press CTRL-C to exit out." << endl;

  delete dc1;
  delete dc2;
  delete tc1;
  delete tc2;
  delete hc1;
  delete hc2;
  delete npc1;
  delete npc2;
  delete ec1;
  delete ec2;
  delete qc1;
  delete qc2;
  delete ctrap;
}
