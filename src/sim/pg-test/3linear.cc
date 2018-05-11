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
static char const _3linear_cc_rcsid_[] =
"$Id: 3linear.cc,v 1.3 1998/09/21 16:14:58 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <DS/containers/list.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include <FW/basics/Log.h>
#include <FW/behaviors/Adapter.h>
#include <FW/behaviors/Protocol.h>
#include <FW/actors/Terminal.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/Handlers.h>
#include <FW/actors/Terminal.h>
#include <FW/actors/State.h>
#include <FW/interface/ShareableRegistry.h>
#include <codec/pnni_ig/id.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <sim/ctrl/ResourceManager.h>
#include <sim/switch/SwitchFunctions.h>
#include <sim/switch/SwitchTerminal.h>
#include <sim/port/LinkStateWrapper.h>

#define INJECT_TIME 25.0

class MySpecialTerminal : public Terminal {
public:

  MySpecialTerminal(NodeID * myAddr) : Terminal( ), 
    _destination(0), _myAddr(myAddr), _csTimer(0)
  { _fastuni_type = QueryRegistry(FAST_UNI_VISITOR_NAME); }

  virtual ~MySpecialTerminal( ) { delete _destination; }

  void Absorb(Visitor * v) 
  { 
    cout << "***** Received " << v->GetType();
    if (v->GetType().Is_A(_fastuni_type)) {
      FastUNIVisitor * fv = (FastUNIVisitor *)v;
      cout << ":" << fv->PrintMSGType() << endl;
    }
    cout << " in " << *(_myAddr) << endl;
    v->Suicide(); 
  }
  void Interrupt(SimEvent * se) { }

  void StartCallSetupTimer(NodeID *& dest) {
    _destination = dest;
    dest = 0;
    Register(_csTimer);
  }

  NodeID * GetAddr(void) const { return _myAddr; }
  void SetTimer(TimerHandler * th) { _csTimer = th; }

  void Inject(Visitor * v) { Terminal::Inject(v); }

private:
  
  NodeID       * _myAddr;
  NodeID       * _destination;
  TimerHandler * _csTimer;
  const VisitorType * _fastuni_type;
};

class CallSetupTimer : public TimerHandler {
public:

  CallSetupTimer(SimEntity * se, double time, NodeID * addr) :
    TimerHandler(se, time), _addr(addr), 
    _term((MySpecialTerminal *)se) { }

  virtual ~CallSetupTimer( ) { }

  void Callback(void) {
    assert(_addr && _term->GetAddr());
    FastUNIVisitor * fuv = new FastUNIVisitor(_term->GetAddr(), _addr);
    AppendEventToLog("CallSetup");
    cout << "-------- Initiating call setup --------" << endl;
    _term->Inject(fuv);

    Register(this);
  }

private:

  NodeID            * _addr;
  MySpecialTerminal * _term;
};

u_char base_addr[22] =
{ 96, 160, 0x47, 0x05, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,     
  0x11, 0x11, 0x10, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void main(int argc, char ** argv)
{
  struct timeval tp;
  gettimeofday(&tp, 0);
  srand(tp.tv_sec);

  dic_item di;
  double stop_time = 1000.0;

  if (argc < 2)
    cout << "usage: " << argv[0] << " [outfile]" << endl;

  if (argc > 1)
    VisPipe(argv[1]);
  else
    VisPipe("/dev/null");

  // Building switches
  u_char * addr = base_addr;
  addr[0] = 96; 
  addr[14] = 0x01;
  NodeID * nid  = new NodeID(addr);
  addr[14] = 0x01;
  NodeID * pref = new NodeID(addr);
  addr[0] = 88;
  NodeID * pgl  = new NodeID(addr);
  MySpecialTerminal * terminalZero;
  Conduit * term = new Conduit("SpecialTerm", (terminalZero = new MySpecialTerminal(nid)));
  Conduit * sw1 = MakeSwitch("Switch-1", 5, 0.01, 1000, nid, pgl, pref, term);

  addr[0] = 96; 
  addr[14] = 0x02;
  nid  = new NodeID(addr);
  addr[14] = 0x01;
  pref = new NodeID(addr);
  addr[0] = 88;
  pgl  = new NodeID(addr);
  term = new Conduit("SpecialTerm", new MySpecialTerminal(nid));
  Conduit * sw2 = MakeSwitch("Switch-2", 5, 0.01, 1000, nid, pgl, pref, term);

  addr[0] = 96; 
  addr[14] = 0x03;
  nid  = new NodeID(addr);
  addr[14] = 0x01;
  pref = new NodeID(addr);
  addr[0] = 88;
  pgl  = new NodeID(addr);
  term = new Conduit("SpecialTerm", new MySpecialTerminal(nid));
  Conduit * sw3 = MakeSwitch("Switch-3", 5, 0.01, 1000, nid, pgl, pref, term);

  cout << "---- Making connections ----" << endl;
  LinkStateWrapper * link_1_2, * link_2_3;

  assert(link_1_2 = LinkSwitches(sw1, 1, sw2, 1, OC3, ((rand() % 3) + 1)));
  assert(link_2_3 = LinkSwitches(sw2, 2, sw3, 1, OC3, ((rand() % 3) + 1)));

  DiagLevel("sim.leadership", DIAG_INFO);  // DIAG_ENV);
  DiagLevel("fsm.nodepeer",   DIAG_INFO);  // DIAG_ENV);
  DiagLevel("sim.acac",       DIAG_INFO);  // DIAG_ENV);
  DiagLevel("sim.logos",      DIAG_INFO);  // DIAG_ENV);
  DiagLevel("fsm.database",   DIAG_DEBUG); // DIAG_ENV);

  cout << "---- Registering Call Setup Callbacks ----" << endl;
  addr = base_addr;
  addr[0] = 96; 
  addr[14] = 0x03;
  NodeID * dest = new NodeID(addr);
  terminalZero->SetTimer( new CallSetupTimer(terminalZero, INJECT_TIME, dest) );
  terminalZero->StartCallSetupTimer(dest);

  cout << "---- Starting the SimKernel ----" << endl;
  theKernel().StopIn(stop_time);
  theKernel().Run();

  cout << "---- Cleaning up ----" << endl;
  UnlinkSwitches(link_1_2);
  delete link_1_2;
  UnlinkSwitches(link_2_3);
  delete link_2_3;

  delete sw1;
  delete sw2;
  delete sw3;
}


