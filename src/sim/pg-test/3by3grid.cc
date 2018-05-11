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
static char const _3by3grid_cc_rcsid_[] =
"$Id: 3by3grid.cc,v 1.26 1999/02/19 21:22:54 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include <DS/containers/list.h>
#include <DS/util/String.h>
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
#include <fsm/config/Configurator.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <sim/ctrl/ResourceManager.h>
#include <sim/switch/SwitchFunctions.h>
#include <sim/switch/SwitchTerminal.h>

#define INJECT_TIME 25.0

class MySpecialTerminal : public Terminal {
public:

  MySpecialTerminal(NodeID * myAddr) : Terminal( ), 
    _destination(0), _myAddr(myAddr), _csTimer(0),
    _fastuni_type(0)
  { if (!_fastuni_type) _fastuni_type = QueryRegistry(FAST_UNI_VISITOR_NAME); }

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

  theConfigurator( "Configuration.nfo" );

  dic_item di;
  dictionary<int, Conduit *> switches;
  MySpecialTerminal * terminalZero = 0;
  double stop_time = 1000.0;
  int num_switches = 5;

  if (argc < 2)
    cout << "usage: " << argv[0] << " [outfile]" << endl;

  if (argc > 1)
    VisPipe(argv[1]);
  else
    VisPipe("/dev/null");

  cout << "---- Building the 3 by 3 grid of switches ----" << endl;
  
  int i;
  for (i = 0; i < 9; i++) {
    Conduit * term = 0; 
    u_char * addr = base_addr;
    addr[0] = 96; 
    addr[14] = (i & 0xFF);

    NodeID * nid  = new NodeID(addr);
    addr[14] = 0x0;
    NodeID * pref = new NodeID(addr);
    addr[0] = 88;
    NodeID * pgl = new NodeID(addr);
    
    char name[64];
    sprintf(name, "Switch-%d", i);

    if (!i || i == 8) {
      MySpecialTerminal * backup = terminalZero;
      terminalZero = new MySpecialTerminal(nid);
      term = new Conduit("SpecialTerminal", terminalZero);
      if (i == 8)
	terminalZero = backup;
    }

    ds_String * n = new ds_String( name );
    ds_String * r = new ds_String( "PGOne" );
    Conduit * current = MakeSwitch(r, n, nid, term);
    switches.insert(i, current);
  }

  cout << "---- Making connections ----" << endl;
  LinkStateWrapper * link = 0;
  for (i = 0; i < 8; i++) {
    Conduit * lhs = 0, * rhs = 0;

    if (di = switches.lookup(i))
      lhs = switches.inf(di);

    if (i != 2 && i != 5) {  // (i + 1) % 3
      // Join i and i + 1
      if (di = switches.lookup(i + 1))
	rhs = switches.inf(di);

      assert(link = LinkSwitches(lhs, 1, rhs, 3, OC3, ((rand() % 3) + 1)));
    }
    if (i < 6) {
      // Join i and i + 3
      if (di = switches.lookup(i + 3))
	rhs = switches.inf(di);
      assert(link = LinkSwitches(lhs, 2, rhs, 4, OC3, ((rand() % 3) + 1)));
    }
  }

  DiagLevel("sim.leadership", DIAG_INFO);  // DIAG_ENV);
  DiagLevel("fsm.nodepeer",   DIAG_INFO);  // DIAG_ENV);
  DiagLevel("sim.acac",       DIAG_INFO);  // DIAG_ENV);
  DiagLevel("sim.logos",      DIAG_ENV);

  cout << "---- Registering Setup Callbacks ----" << endl;
  u_char * addr = base_addr;
  addr[0] = 96; 
  addr[14] = 0x08;
  NodeID * dest = new NodeID(addr);
  terminalZero->SetTimer( new CallSetupTimer(terminalZero, INJECT_TIME, dest) );
  terminalZero->StartCallSetupTimer(dest);

  cout << "---- Starting the SimKernel ----" << endl;
  theKernel().StopIn(stop_time);
  theKernel().Run();

  cout << "---- Cleaning up ----" << endl;
  forall_items(di, switches)
    delete switches.inf(di);
  switches.clear();
}
