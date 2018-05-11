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
#ifndef __KDB_H__
#define __KDB_H__

#ifndef LINT
static char const _KDB_h_rcsid_[] =
"$Id: KDB.h,v 1.1 1999/01/13 19:12:37 mountcas Exp $";
#endif

#include <FW/kernel/SimEntity.h>
#include <DS/containers/dictionary.h>

class Kernel;
class TimedDeliveryHandler;
class TimerHandler;
class InputHandler;
class OutputHandler;
class Conduit;
class Visitor;

// ---------------------------------------------------------------------------
// BPT -- The Simulation Kernel Breakpoint
// ---------------------------------------------------------------------------
class BPT {
  friend class Debugger;
public:

  void setid(int x);

  enum bpt_type {
    bptInterrupt = 0x01,
    bptTimer     = 0x02,
    bptInput     = 0x04,
    bptOutput    = 0x08,
    bptConduit   = 0x10,
    bptSimEntity = 0x12,
    bptVisitor   = 0x14
  };
  
  enum BPT_Interrupt_triggertype {
    InterruptUnsupported   = 0x00,
    InterruptDestruction   = 0x01, // supported
    InterruptDelivery      = 0x02  // supported
  };
  enum BPT_Timer_triggertype {
    TimerUnsupported       = 0x00,
    TimerDestruction       = 0x01, // supported
    TimerCallback          = 0x02  // supported
  };
  enum BPT_Input_triggertype {
    InputUnsupported       = 0x00,
    InputDestruction       = 0x01, // supported
    InputCallback          = 0x02  // supported
  };
  enum BPT_Output_triggertype {
    OutputUnsupported      = 0x00,
    OutputDestruction      = 0x01, // supported
    OutputCallback         = 0x02  // supported
  };
  enum BPT_Conduit_triggertype {
    ConduitUnsupported     = 0x00,
    ConduitDestruction     = 0x01, // supported
    ConduitSiblingBirth    = 0x02, // not yet supported
    ConduitIntrusion       = 0x04  // supported
  };
  enum BPT_SimEntity_triggertype {
    SimEntityUnsupported   = 0x00,
    SimEntityDestruction   = 0x01, // supported
    SimEntityInterruption  = 0x02  // supported
  };
  enum BPT_Visitor_triggertype {
    VisitorUnsupported     = 0x00,
    VisitorDestruction     = 0x01, // supported
    VisitorSiblingBirth    = 0x02, // not yet supported
    VisitorArrival         = 0x04  // supported
  };
  
  BPT(TimedDeliveryHandler* x, BPT_Interrupt_triggertype t);  
  BPT(TimerHandler* x, BPT_Timer_triggertype t);
  BPT(InputHandler* x, BPT_Input_triggertype t);  
  BPT(OutputHandler* x, BPT_Output_triggertype t);  
  BPT(Conduit* x, BPT_Conduit_triggertype t, Visitor* v=0);  
  BPT(SimEntity* x, BPT_SimEntity_triggertype t, TimedDeliveryHandler* h=0);  
  BPT(Visitor* x, BPT_Visitor_triggertype t, Conduit* c=0);  
  int equals(BPT& him);
  int affected_by(BPT& him);

  void Print(ostream& os);

  void mark_for_deletion(void);
  bool is_marked_for_deletion(void);

private:

  int _inited;
  
  int init(void);

  bpt_type _type;
  
  TimedDeliveryHandler * _target_Interrupt;
  TimerHandler         * _target_Timer;
  InputHandler         * _target_Input;
  OutputHandler        * _target_Output;
  Conduit              * _target_Conduit;
  SimEntity            * _target_SimEntity;
  Visitor              * _target_Visitor;
  
  BPT_Interrupt_triggertype     _interrupt_subtype;
  BPT_Timer_triggertype         _timer_subtype;
  BPT_Input_triggertype         _input_subtype;
  BPT_Output_triggertype        _output_subtype;
  BPT_Conduit_triggertype       _conduit_subtype;
  BPT_SimEntity_triggertype     _simentity_subtype;
  BPT_Visitor_triggertype       _visitor_subtype;
  
  Visitor              * _aux_Visitor;
  TimedDeliveryHandler * _aux_Interrupt;
  Conduit              * _aux_Conduit;

  int  _id;
  bool _dead;
};


// ---------------------------------------------------------------------------
// KDB -- The Simulation Kernel Debugger
// ---------------------------------------------------------------------------
class Debugger : public SimEntity {
  friend class Kernel;
public:
  
  Debugger(Kernel * k);
  Debugger(Kernel * k, char* command_pipe);
  virtual ~Debugger();
 
  // ------------------------------------------------------------------------
 
  int Report(TimedDeliveryHandler* x);
  int Report(TimerHandler* x);
  int Report(InputHandler* x);
  int Report(OutputHandler* x);
  int Report(Conduit* x);
  int Report(SimEntity* x);
  int Report(Visitor* x);

  int GetIndex(Conduit * x);
  int GetIndex(Visitor * x);

  int Resign(int id, TimedDeliveryHandler* x);
  int Resign(int id, TimerHandler* x);
  int Resign(int id, InputHandler* x);
  int Resign(int id, OutputHandler* x);
  int Resign(int id, Conduit* x);
  int Resign(int id, SimEntity* x);
  int Resign(int id, Visitor* x);

  // -------------------------------------------------------------------------
  
  void List_Breakpoints(ostream& os);
  void List_Interrupt(ostream& os);
  void List_Timer(ostream& os);
  void List_Input(ostream& os);
  void List_Output(ostream& os);
  void List_Conduit(ostream& os);
  void List_SimEntity(ostream& os);
  void List_Visitor(ostream& os);

  // -------------------------------------------------------------------------

  int SetBreakpoint(BPT * event);
  int UnsetBreakpoint(int id);

  int Notify(BPT * event);

  // -------------------------------------------------------------------------

  bool kdb_Prompt(char* text = 0, bool one_shot = false );

  void Interrupt(SimEvent * se);

private:

  Kernel * _kernel;

  int _bptnum;
  
  dictionary<int, BPT *> _breakpoints;

  bool  _verbose;
  bool  _single_step;
  char  _prev_arg[255];

  // -------------------------------------------------------------------------
  
  dictionary <int, TimedDeliveryHandler*> _Interrupt_dict;     int _max_Interrupt;
  dictionary <int, TimerHandler*>         _Timer_dict;         int _max_Timer;
  dictionary <int, InputHandler*>         _Input_dict;         int _max_Input;
  dictionary <int, OutputHandler*>        _Output_dict;        int _max_Output;
  dictionary <int, Conduit*>              _Conduit_dict;       int _max_Conduit;
  dictionary <int, SimEntity*>            _SimEntity_dict;     int _max_SimEntity;
  dictionary <int, Visitor*>              _Visitor_dict;       int _max_Visitor;
};

extern "C"{
#include <unistd.h>
#include <signal.h>
};

#define ALERT_DEBUGGER { kill(getpid(),SIGTSTP); }

bool kdb(void);
void help(void);



#endif // __KDB_H__
