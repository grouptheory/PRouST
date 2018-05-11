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
static char const _testsvc_cc_rcsid_[] =
"$Id: testsvc.cc,v 1.8 1998/08/06 04:04:55 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/basics/Conduit.h>
#include <FW/behaviors/Protocol.h>
#include <FW/behaviors/Adapter.h>
#include <FW/kernel/SimEvent.h>
#include <FW/kernel/Handlers.h>
#include <FW/actors/Terminal.h>
#include <FW/basics/diag.h>

#include <codec/pnni_pkt/pkt_incl.h>
#include <codec/uni_ie/ie.h>
#include <codec/pnni_ig/id.h>

#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/database/Database.h>
#include <fsm/database/InitDB.h>
#include <fsm/hello/RCCHelloState.h>
#include <fsm/hello/DBtrapper.h>
#include <fsm/nodepeer/NodePeerState.h>
#include <fsm/hello/HelloVisitor.h>
#include <fsm/queue/QueueState.h>

// #include <fsm/hello/LGNHelloState.h>
#include <fsm/hello/LgnHelloState.h>

extern "C" {
  char * getcwd(char *, int);
};

#define AGG_TOKEN   10
#define BORDER_PORT 10
#define LEVEL       88

class StartTimer : public TimerHandler  {
public:

  StartTimer(Terminal * t, Visitor * v) : 
    TimerHandler(t,1.0), _t(t), _v(v) { }
  virtual ~StartTimer() { }
  void Callback(void) { _t->Inject(_v); }

private:

  Terminal * _t;
  Visitor  * _v;
};


class HelloTerm: public Terminal {
public:

  HelloTerm(NodeID * myNode, NodeID * remNode, bool CallingParty, NodeID * borderNode) :
    _CallingParty(CallingParty), _localNode(myNode), _remoteNode(remNode)
  {
    // Agg token 10,
    FastUNIVisitor *v = new FastUNIVisitor(myNode, remNode, 0, 0x05,  AGG_TOKEN, 
					   0, FastUNIVisitor::NewUplink);
    v->SetMSGType(FastUNIVisitor::NewUplink);
    v->SetBorderPort(BORDER_PORT);
    v->SetBorder(borderNode);
    _go1 = new StartTimer(this,v);
    Register(_go1);
  }
  virtual ~HelloTerm() { }
  void Interrupt(SimEvent* e)  {
    cout << "I was Interrupted by event " << e->GetID() << "!\n";
  }

  void Absorb(Visitor * v)
  {
    if (!_CallingParty) { 
      // Setup Suicide , Send Connect
      // others suicide.
      VisitorType * vt2 = (VisitorType *)QueryRegistry(FAST_UNI_VISITOR_NAME);
      if (vt2 && v->GetType().Is_A(vt2)) {
	delete vt2; vt2 =0;
	FastUNIVisitor *fu = (FastUNIVisitor *)v;
	if (fu->GetMSGType() == (FastUNIVisitor::FastUNISetup)) {
	  fu->Suicide();
	  // Need to Send Connect now
	  diag("fsm.hello", DIAG_DEBUG, "Setup received from Peer at CalledParty \n");
	  FastUNIVisitor *fv = new FastUNIVisitor(_localNode, _remoteNode, 
						  0, 0x05, AGG_TOKEN, 0,
						  FastUNIVisitor::FastUNIConnect);
	  fv->SetMSGType(FastUNIVisitor::FastUNIConnect);
	  
	  fv->SetOutVP(0); fv->SetOutVC(78);
	  fv->SetInVP(0); fv->SetInVC(78); 
	  
	  diag("fsm.hello", DIAG_DEBUG, "Connect Sent from Called Party\n");
	  Inject(fv);
	} else {
	  v->Suicide();
	} 
      } else {
	if (vt2)
	  delete vt2; vt2 = 0;
	  vt2 = (VisitorType *)QueryRegistry(HLINK_VISITOR_NAME);
	  
	  if (vt2 && v->GetType().Is_A(vt2)) {
	    delete vt2;vt2=0;
	    diag("fsm.hello.testsvc", DIAG_DEBUG, "Aggr: LGN Horizontal link up at Calling Party\n");
	  }
	  if (vt2) delete vt2;
	  v->Suicide();
      }
    } else {
      // Setup reverse.
      // Connect suicide
      // others suicide
      VisitorType * vt2 = (VisitorType *)QueryRegistry(FAST_UNI_VISITOR_NAME);
      if (vt2 && v->GetType().Is_A(vt2)) {
	delete vt2; vt2 =0;
	FastUNIVisitor *fu = (FastUNIVisitor *)v;
	if (fu->GetMSGType() == (FastUNIVisitor::FastUNISetup)) {
	  // Send it back
	  diag("fsm.hello.testsvc", DIAG_DEBUG, "Setup at the Calling Terminal, forwarding.\n");
	  Inject(fu);
	} else if (fu->GetMSGType() == (FastUNIVisitor::FastUNIConnect)) {
	  // Suicide it
	  diag("fsm.hello.testsvc", DIAG_DEBUG, "Connect Received at the Calling terminal.\n");
	  fu->Suicide(); 
	} else {
	  diag("fsm.hello.testsvc", DIAG_DEBUG, "Unknown FastUNI (%s) at Calling Terminal.\n", 
	       fu->PrintMSGType());
	  fu->Suicide();
	}
      } else {
	if (vt2)
	  delete vt2; vt2 = 0;
	  vt2 = (VisitorType *)QueryRegistry(HLINK_VISITOR_NAME);
	  
	  if (vt2 && v->GetType().Is_A(vt2)) {
	    delete vt2;vt2=0;
	    diag("fsm.hello.testsvc", DIAG_DEBUG, "Aggr: LGN Horizontal link up at Calling Party.\n");
	  }
	  if(vt2) delete vt2;
	  v->Suicide();
      }
    }
  }

private:
  
  StartTimer * _go1;
  NodeID     * _localNode;
  NodeID     * _remoteNode;
  bool         _CallingParty;
};

// -----------------------------------------------------------
void main(int argc, char **argv)
{
  char path1[256];
  char path2[256];
  char filename[80];
  FILE *fp;

  const unsigned char bn1[] = 
    "\x58\x60\x47\x00\x05\x80\xff\xde\x00\x00\x01\x00\x00\x04\x01\xff\x1c\x06\x92\x00\x01\x00";
  const unsigned char bn2[] = 
    "\x58\x60\x47\x00\x05\x80\xff\xde\x00\x00x01\x00\x00\x03\x01\xff\x1c\x06\x92\x00\x01\x00";

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
  int vpi = 0; 
  int vci = 78;

  NodeID * borderNode1 = new NodeID(bn1);
  NodeID * borderNode2 = new NodeID(bn2);

  DiagLevel(FW, DIAG_ERROR);
  DiagLevel(FSM, DIAG_DEBUG); 
  DiagLevel(CODEC, DIAG_ERROR);
  DiagLevel(SIM,   DIAG_ERROR);

  if (argc < 2) {
    cout << "usage: " << argv[0] << "CallingPartyDB CalledPartyDB Speed Time 0 OutputFile" << endl;

    if (!getcwd(path1,80))  {
      cout << "can't get CWD \n";
      exit(1);
    }
    strcat(path1, "/");
    cout << "Give the Calling Party DB File: " << endl;
    cin >> filename;
    strcat(path1, filename);
  } else {
    strcpy(path1, argv[1]);
  }

  if (argc < 3) {
    if (!getcwd(path2,80)) {
      cout << "can't get CWD \n";
      exit(1);
    }
    strcat(path2, "/");
    cout << "Give the Called Party DB File: " << endl;
    cin >> filename;
    strcat(path2, filename);
  } else  {
    strcpy(path2, argv[2]);
  }

  int speedvalue;
  if (argc < 4) {
    cout << "Enter log_2 speed (-2,-1,0,1,2) or enter 666 for sim-time: ";
    cin >> filename;
    speedvalue = atoi(filename);
  } else
    speedvalue = atoi(argv[3]);

  double simtime;
  if (argc < 5) {
    cout << "How long (seconds) to run the simulation: ";
    cin >> filename;
    simtime = atof(filename);
  } else
    simtime = atof(argv[4]);
  
  int debug;
  if (argc < 6) {
    cout << "(0)= Run, (1)= Debug, Enter 1 or 0: ";
    cin >> filename;
    debug = atoi(filename);
  } else
    debug = atoi(argv[5]);
  
  if (argc < 7) {
    cout << "Enter the name of the visualizer pipe: ";
    cin >> filename;
  } else
    strcpy(filename, argv[6]);
  
  VisPipe(filename);

  NodeID * ppgl1 = 0;

  if (!(fp = fopen(path1,"r"))) {
    cout << "can't open file '" << path1 << "' for reading." << endl;
    exit(1);
  }
  dc1 = InitDB(fp, "DB1", db1, n1, pn1, ppgl1, false, LEVEL);

  if (!(fp = fopen(path2, "r"))) {
    cout << "can't open file '" << path2 << "' for reading." << endl;
    exit(1);
  }

  NodeID* ppgl2=0;
  dc2 = InitDB(fp, "DB2", db2, n2, pn2,ppgl2, false, LEVEL);

  RCCHelloState * fsm1 = new RCCHelloState(*db1, n1, n2, vpi, vci, cver, nver, over);
  Protocol * pro1 = new Protocol(fsm1);
  Conduit *hc1 = new Conduit("SVH1", pro1);

  RCCHelloState *fsm2 = new RCCHelloState(*db2, n2,n1,vpi,vci,cver,nver,over);
  Protocol * pro2 = new Protocol(fsm2);
  Conduit *hc2 = new Conduit("SVH2", pro2);

  LgnHelloState *fsm3 = new LgnHelloState(10, n1,1,n2); // aggr 0 for now 
  Protocol * pro3 = new Protocol(fsm3);
  Conduit *hc3 = new Conduit("LVH1", pro3);
  
  LgnHelloState *fsm4 = new LgnHelloState(AGG_TOKEN, n2, 1, n1);
  Protocol * pro4 = new Protocol(fsm4);
  Conduit *hc4 = new Conduit("LVH4", pro4);

  NodePeerState *np1 = new NodePeerState(n1,n2,db1);
  Protocol * np_pro1 = new Protocol(np1);
  Conduit      *npc1 = new Conduit("NP1", np_pro1);

  NodePeerState *np2 = new NodePeerState(n2,n1,db2);
  Protocol * np_pro2 = new Protocol(np2);
  Conduit      *npc2 = new Conduit("NP2", np_pro2);

  QueueState* q1=new QueueState(0.01, 100);
  Protocol * pq1 = new Protocol(q1);
  Conduit *qc1 = new Conduit("Queue1", pq1);
 
  QueueState* q2=new QueueState(0.01, 100);
  Protocol * pq2 = new Protocol(q2);
  Conduit *qc2 = new Conduit("Queue2", pq2);
  
  // Starters  to inject PortUpVisitor's on the 2 Hello's
  HelloTerm *t1 = new HelloTerm(n1, n2, true, borderNode1);
  Adapter *a1 = new Adapter(t1);
  Conduit *tc1 = new Conduit("Term1",a1);

  HelloTerm *t2 = new HelloTerm(n2, n1, false, borderNode2);
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

  Kernel& controller=theKernel();
  controller.SetSpeed( (Kernel::Speed) speedvalue);

  controller.StopIn(simtime); // stop in 100 seconds!

  if (debug) controller.Debug();
  else controller.Run();

  // cout << "Simulation ended (\ab\ae\ae\ae\a\e\ae\ae\ae\ap\a!), Press CTRL-C to exit out." << endl;
  // system("sleep 9999");
}
