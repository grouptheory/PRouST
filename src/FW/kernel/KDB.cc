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
static char const _KDB_cc_rcsid_[] =
"$Id: KDB.cc,v 1.1 1999/01/13 19:12:37 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/kernel/KDB.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/SimEvent.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/Visitor.h>
#include <FW/basics/VisitorType.h>
#include <FW/basics/Log.h>
#include <FW/basics/diag.h>

extern "C" {
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
};




// ---------------------------------------------------------------------------
// BPT -- The Simulation Kernel Breakpoint
// ---------------------------------------------------------------------------

int BPT::init(void)
{
  _target_Interrupt = 0;
  _target_Timer     = 0;
  _target_Input     = 0;
  _target_Output    = 0;
  _target_Conduit   = 0;
  _target_SimEntity = 0;
  _target_Visitor   = 0;
  _aux_Visitor      = 0;
  _aux_Interrupt    = 0;
  _aux_Conduit      = 0;

  return 1;
}

void BPT::setid(int x)
{ _id = x; }

void BPT::mark_for_deletion(void) { _dead = true; }
bool BPT::is_marked_for_deletion(void) { return _dead; }

BPT::BPT(TimedDeliveryHandler* x, BPT_Interrupt_triggertype t)
  : _inited(init()), _type(bptInterrupt), _interrupt_subtype(t), 
    _target_Interrupt(x), _dead(false), _timer_subtype(TimerUnsupported), 
    _input_subtype(InputUnsupported), _output_subtype(OutputUnsupported), 
    _conduit_subtype(ConduitUnsupported), _simentity_subtype(SimEntityUnsupported), 
    _visitor_subtype(VisitorUnsupported)
{ }

BPT::BPT(TimerHandler* x, BPT_Timer_triggertype t)
  : _inited(init()), _type(bptTimer), _timer_subtype(t), _target_Timer(x),
  _dead(false), _interrupt_subtype(InterruptUnsupported), 
  _input_subtype(InputUnsupported), _output_subtype(OutputUnsupported), 
  _conduit_subtype(ConduitUnsupported), _simentity_subtype(SimEntityUnsupported), 
  _visitor_subtype(VisitorUnsupported)
{ }

BPT::BPT(InputHandler* x, BPT_Input_triggertype t)
  : _inited(init()), _type(bptInput), _input_subtype(t), _target_Input(x),
  _dead(false), _interrupt_subtype(InterruptUnsupported), 
  _timer_subtype(TimerUnsupported), _output_subtype(OutputUnsupported), 
  _conduit_subtype(ConduitUnsupported), _simentity_subtype(SimEntityUnsupported), 
  _visitor_subtype(VisitorUnsupported)
{ }

BPT::BPT(OutputHandler* x, BPT_Output_triggertype t)
  : _inited(init()), _type(bptOutput), _output_subtype(t), _target_Output(x),
  _dead(false), _interrupt_subtype(InterruptUnsupported), 
  _timer_subtype(TimerUnsupported), _input_subtype(InputUnsupported), 
  _conduit_subtype(ConduitUnsupported), _simentity_subtype(SimEntityUnsupported), 
  _visitor_subtype(VisitorUnsupported)
{ }

BPT::BPT(Conduit* x, BPT_Conduit_triggertype t, Visitor* v)
  : _inited(init()), _type(bptConduit), _conduit_subtype(t), 
    _target_Conduit(x), _aux_Visitor(v), _dead(false),
    _interrupt_subtype(InterruptUnsupported), 
    _timer_subtype(TimerUnsupported), _input_subtype(InputUnsupported), 
  _output_subtype(OutputUnsupported), _simentity_subtype(SimEntityUnsupported), 
  _visitor_subtype(VisitorUnsupported)
{ }

BPT::BPT(SimEntity* x, BPT_SimEntity_triggertype t, TimedDeliveryHandler* h)
  : _inited(init()), _type(bptSimEntity), _simentity_subtype(t), 
    _target_SimEntity(x), _aux_Interrupt(h), _dead(false), 
    _interrupt_subtype(InterruptUnsupported), _timer_subtype(TimerUnsupported), 
    _input_subtype(InputUnsupported), _output_subtype(OutputUnsupported), 
    _conduit_subtype(ConduitUnsupported), _visitor_subtype(VisitorUnsupported)
{ }

BPT::BPT(Visitor* x, BPT_Visitor_triggertype t, Conduit* c)
  : _inited(init()), _type(bptVisitor), _visitor_subtype(t), 
    _target_Visitor(x), _aux_Conduit(c), _dead(false), 
    _interrupt_subtype(InterruptUnsupported), _timer_subtype(TimerUnsupported), 
    _input_subtype(InputUnsupported), _output_subtype(OutputUnsupported), 
  _conduit_subtype(ConduitUnsupported), _simentity_subtype(SimEntityUnsupported)
{ }

int BPT::equals(BPT & him)
{
  int ans = 0;

  if (_type == him._type) {
    switch (_type) {
      case bptInterrupt:
	if ((_target_Interrupt==him._target_Interrupt)&&
	    (_interrupt_subtype==him._interrupt_subtype))
	  ans = 1;
	break;
      case bptTimer:
	if ((_target_Timer==him._target_Timer)&&
	    (_timer_subtype==him._timer_subtype))
	  ans = 1;
	break;
      case bptInput:
	if ((_target_Input==him._target_Input)&&
	    (_input_subtype==him._input_subtype))
	  ans = 1;
	break;
      case bptOutput:
	if ((_target_Output==him._target_Output)&&
	    (_output_subtype==him._output_subtype))
	  ans = 1;
	break;
      case bptConduit:
	if ((_target_Conduit==him._target_Conduit)&&
	    (_conduit_subtype==him._conduit_subtype)&&
	    ((!_aux_Visitor) || (_aux_Visitor==him._aux_Visitor)) )
	  ans = 1;
	break;
      case bptSimEntity:
	if ((_target_SimEntity==him._target_SimEntity)&&
	    (_simentity_subtype==him._simentity_subtype)&&
	    ((!_aux_Interrupt) || (_aux_Interrupt==him._aux_Interrupt)) )
	  ans = 1;
	break;
      case bptVisitor:
	if ((_target_Visitor==him._target_Visitor)&&
	    (_visitor_subtype==him._visitor_subtype)&&
	    ((!_aux_Conduit) || (_aux_Conduit==him._aux_Conduit)) )
	  ans = 1;
	break;
      default:
	break;
    }
  }
  return ans;
}

// INVARIANT: argument 'him' is a breakpoint of type Destruction*
int BPT::affected_by(BPT & him)
{
  int ans = 0;
  switch (_type) {
    case bptInterrupt:
      if ((_target_Interrupt==him._target_Interrupt) ||
	  (_aux_Interrupt==him._target_Interrupt))
	ans = 1;
      break;
    case bptTimer:
      if (_target_Timer==him._target_Timer)
	ans = 1;
      break;
    case bptInput:
      if (_target_Input==him._target_Input)
	ans = 1;
      break;
    case bptOutput:
      if (_target_Output==him._target_Output)
	ans = 1;
      break;
    case bptConduit:
      if ((_target_Conduit==him._target_Conduit)||
	  (_aux_Conduit==him._target_Conduit))
	ans = 1;
      break;
    case bptSimEntity:
      if (_target_SimEntity==him._target_SimEntity)
	ans = 1;
      break;
    case bptVisitor:
      if ((_target_Visitor==him._target_Visitor)||
	  (_aux_Visitor==him._target_Visitor))
	ans = 1;
      break;
    default:
      break;
  }
  return ans;
}


void BPT::Print(ostream & os)
{
  switch (_type) {
    //-----------------------------------------------
    case bptInterrupt: os << "Interrupt "; 
      os << hex << _target_Interrupt << dec << " ";
      switch (_interrupt_subtype) {
        case InterruptDestruction: os << "InterruptDestruction";
	  break;
        case InterruptDelivery: os << "InterruptDelivery";
	  break;
      };
      break;
    //-----------------------------------------------
    case bptTimer: os << "Timer "; 
      os << hex << _target_Timer << dec << " ";
      switch (_timer_subtype) {
        case TimerDestruction: os << "TimerDestruction";
	  break;
        case TimerCallback: os << "TimerCallback";
	  break;
      };
      break;
    //-----------------------------------------------
    case bptInput: os << "Input "; 
      os << hex << _target_Input << dec << " ";
      switch (_input_subtype) {
        case InputDestruction: os << "InputDestruction";
	  break;
        case InputCallback: os << "InputCallback";
	  break;
      };
      break;
    //-----------------------------------------------
    case bptOutput: os << "Output "; 
      os << hex << _target_Output << dec << " ";
      switch (_output_subtype) {
        case OutputDestruction: os << "OutputDestruction";
	  break;
        case OutputCallback: os << "OutputCallback";
	  break;
      };
      break;
    //-----------------------------------------------
    case bptConduit: os << "Conduit "; 
      os << hex << _target_Conduit << dec << " ";
      switch (_conduit_subtype) {
        case ConduitDestruction: os << "ConduitDestruction";
	  break;
        case ConduitSiblingBirth: os << "ConduitSiblingBirth";
	  break;
        case ConduitIntrusion: os << "ConduitIntrusion";
	  if (_aux_Visitor)
	    os << ", by Visitor " << hex << _aux_Visitor << dec;
	  else
	    os << ", by ANY Visitor";
	  break;
      };
      break;
    //-----------------------------------------------
    case bptSimEntity: os << "SimEntity "; 
      os << hex << _target_SimEntity << dec << " ";
      switch (_simentity_subtype) {
        case SimEntityDestruction: os << "SimEntityDestruction";
	  break;
        case SimEntityInterruption: os << "SimEntityInterruption";
	  if (_aux_Interrupt)
	    os << ", by Interrupt " << hex << _aux_Interrupt << dec;
	  else
	    os << "by ANY Interrupt";
	  break;
      };
      break;
    //-----------------------------------------------
    case bptVisitor: os << "Visitor "; 
      os << hex << _target_Visitor << dec << " ";
      switch (_visitor_subtype) {
        case VisitorDestruction: os << "VisitorDestruction";
	  break;
        case VisitorSiblingBirth: os << "VisitorSiblingBirth";
	  break;
        case VisitorArrival: os << "VisitorArrival";
	  if (_aux_Conduit)
	    os << ", at Conduit " << hex << _aux_Conduit << dec;
	  else
	    os << ", at ANY Conduit";
	  break;
      };
      break;
  };
}

  
// --------------------------------------------------------------------------
// KDB -- The Simulation Kernel Debugger
// --------------------------------------------------------------------------
Debugger::Debugger(Kernel * k) : SimEntity(SimEntity::UNKNOWN, false), 
  _bptnum(1), _max_Interrupt(0), _max_Timer(0), _max_Input(0), 
  _max_Output(0), _max_Conduit(0), _max_SimEntity(0), _max_Visitor(0), 
  _kernel(k), _verbose(false), _single_step(false)
{
  _Interrupt_dict.clear();
  _Timer_dict.clear();
  _Input_dict.clear();
  _Output_dict.clear();
  _Conduit_dict.clear();
  _SimEntity_dict.clear();
  _Visitor_dict.clear();
}

Debugger::Debugger(Kernel * k, char* command_pipe) : SimEntity(SimEntity::UNKNOWN, false), 
  _bptnum(1), _max_Interrupt(0), _max_Timer(0), _max_Input(0), 
  _max_Output(0), _max_Conduit(0), _max_SimEntity(0), _max_Visitor(0), 
  _kernel(k), _verbose(false), _single_step(false) 
{
  _Interrupt_dict.clear();
  _Timer_dict.clear();
  _Input_dict.clear();
  _Output_dict.clear();
  _Conduit_dict.clear();
  _SimEntity_dict.clear();
  _Visitor_dict.clear();
}

Debugger::~Debugger()
{
  dic_item di;
  _kernel = 0;

  forall_items(di, _breakpoints) {
    BPT *breakpoint = _breakpoints.inf(di);
    delete breakpoint;
  }
  _breakpoints.clear();

  _Interrupt_dict.clear();
  _Timer_dict.clear();
  _Input_dict.clear();
  _Output_dict.clear();
  _Conduit_dict.clear();
  _SimEntity_dict.clear();
  _Visitor_dict.clear();
}

// --------------------------------------------------------------------------
int Debugger::Report(TimedDeliveryHandler* x)
{
  _Interrupt_dict.insert(_max_Interrupt++,x);
  return _max_Interrupt-1;
}

int Debugger::Report(TimerHandler* x)
{
  _Timer_dict.insert(_max_Timer++,x);
  return _max_Timer-1;
}

int Debugger::Report(InputHandler* x)
{
  _Input_dict.insert(_max_Input++,x);
  return _max_Input-1;
}

int Debugger::Report(OutputHandler* x)
{
  _Output_dict.insert(_max_Output++,x);
  return _max_Output-1;
}

int Debugger::Report(Conduit* x)
{
  _Conduit_dict.insert(_max_Conduit++,x);
  return _max_Conduit-1;
}

int Debugger::Report(SimEntity* x)
{
  _SimEntity_dict.insert(_max_SimEntity++,x);
  return _max_SimEntity-1;
}

int Debugger::Report(Visitor* x)
{
  _Visitor_dict.insert(_max_Visitor++,x);
  return _max_Visitor-1;
}

int Debugger::GetIndex(Conduit * x)
{
  int rval = -1;
  dic_item di;
  forall_items(di, _Conduit_dict) {
    Conduit * ptr = _Conduit_dict.inf(di);
    if (ptr == x) {
      rval = _Conduit_dict.key(di);
      break;
    }
  }
  return rval;
}

int Debugger::GetIndex(Visitor * x)
{
  int rval = -1;
  dic_item di;
  forall_items(di, _Visitor_dict) {
    Visitor * ptr = _Visitor_dict.inf(di);
    if (ptr == x) {
      rval = _Visitor_dict.key(di);
      break;
    }
  }
  return rval;
}

int Debugger::Resign(int id, TimedDeliveryHandler* x)
{
  dic_item di;
  if (di = _Interrupt_dict.lookup(id)) {
    if (_Interrupt_dict.inf(di)==x){
      _Interrupt_dict.del_item(di);
      return 0;
    }
  }
  DIAG("FW.kernel", DIAG_FATAL, cout
       << "*** KDB internal error in Resign( TimedDeliveryHandler )" << endl
       << "*** Detected deletion of non-existent SimEvent" << endl;);

  return -1;
}

int Debugger::Resign(int id, TimerHandler * x)
{
  dic_item di;
  if (di = _Timer_dict.lookup(id)) {
    if (_Timer_dict.inf(di)==x){
      _Timer_dict.del_item(di);
      return 0;
    }
  }
  DIAG("FW.kernel", DIAG_FATAL, cout
       << "*** KDB internal error in Resign( TimerHandler )" << endl
       << "*** Detected deletion of non-existent Timer" << endl;);

  return -1;
}

int Debugger::Resign(int id, InputHandler* x)
{
  dic_item di;
  if (di = _Input_dict.lookup(id)) {
    if (_Input_dict.inf(di)==x){
      _Input_dict.del_item(di);
      return 0;
    }
  }
  DIAG("FW.kernel", DIAG_FATAL, cout
       << "*** KDB internal error in Resign( InputHandler )" << endl
       << "*** Detected deletion of non-existent InputHandler" << endl;);

  return -1;
}

int Debugger::Resign(int id, OutputHandler * x)
{
  dic_item di;
  if (di = _Output_dict.lookup(id)) {
    if (_Output_dict.inf(di) == x) {
      _Output_dict.del_item(di);
      return 0;
    }
  }
  DIAG("FW.kernel", DIAG_FATAL, cout
       << "*** KDB internal error in Resign( OutputHandler )" << endl
       << "*** Detected deletion of non-existent OutputHandler" << endl;);
  
  return -1;
}

int Debugger::Resign(int id, Conduit * x)
{
  dic_item di;

  if (di = _Conduit_dict.lookup(id)) {
    if (_Conduit_dict.inf(di) == x) {
      _Conduit_dict.del_item(di);
      return 0;
    }
  }
  DIAG("FW.kernel", DIAG_FATAL, cout
       << "*** KDB internal error in Resign( Conduit )" << endl
       << "*** Detected deletion of non-existent Conduit" << endl;);

  return -1;
}

int Debugger::Resign(int id, SimEntity * x)
{
  dic_item di;
  if (di = _SimEntity_dict.lookup(id)) {
    if (_SimEntity_dict.inf(di) == x) {
      _SimEntity_dict.del_item(di);
      return 0;
    }
    DIAG("FW.kernel", DIAG_FATAL, cout
	 << "*** KDB internal error in Resign( SimEntity )" << endl
	 << "*** Detected deletion of non-existent SimEntity" << endl;);
  }
  return -1;
}

int Debugger::Resign(int id, Visitor * x)
{
  dic_item di;

  if (di = _Visitor_dict.lookup(id)) {
    if (_Visitor_dict.inf(di) == x) {
      _Visitor_dict.del_item(di);
      return 0;
    }
  }
  DIAG("FW.kernel", DIAG_FATAL, cout
       << "*** KDB internal error in Resign( Visitor )" << endl
       << "*** Detected deletion of non-existent Visitor." << endl;);

  return -1;
}

// --------------------------------------------------------------------------
void Debugger::List_Breakpoints(ostream& os)
{
  dic_item di;
  BPT* x;
  os << "----- Breakpoints -----\n";
  forall_items(di,_breakpoints){
    x=_breakpoints.inf(di);
    os << _breakpoints.key(di) << "\t" ;
    x->Print(os); 
    os << endl;
  }
}

void Debugger::List_Interrupt(ostream& os)
{
  dic_item di;
  SimEvent* x;
  TimedDeliveryHandler* th;
  os << "Current Time is " << _kernel->CurrentTime() << endl
     << "----- Interrupt Events -----\n";
  forall_items(di,_Interrupt_dict){
    th=_Interrupt_dict.inf(di);
    x=th->_se;
    os << _Interrupt_dict.key(di) << "\t" << hex << x << dec ;
    os << " (code " << (long)(*x) << ") --> ";
    os << hex << x->_to << dec << " at ";
    os << (double)(th->GetExpirationTime()) << endl;
  }
}

void Debugger::List_Timer(ostream& os)
{
  dic_item di;
  TimerHandler * x;
  char buf[1024];
  strcpy(buf,"!list_timers_begin\n");
  write(Log::_out_fd, buf, strlen(buf));

  forall_items(di,_Timer_dict) {
    x = _Timer_dict.inf(di);

    sprintf(buf,"%d %lf %s\n", _Timer_dict.key(di), 
	    (double)(x->GetExpirationTime()), "TimerHandler"); 
    write(Log::_out_fd, buf, strlen(buf));
  }

  sprintf(buf,"!list_timers_end\n");
  write(Log::_out_fd,buf,strlen(buf));

  os << "Current Time is " << _kernel->CurrentTime() << endl
     << "----- Timers -----\n";

  forall_items(di, _Timer_dict) {
    TimerHandler * x = _Timer_dict.inf(di);
    os << _Timer_dict.key(di) << "\t" << hex << x << dec << " " 
       << (x->IsRegistered() ? "R " : "U ");
    x->Print(os);
  }
}

void Debugger::List_Input(ostream& os)
{
  dic_item di;
  InputHandler* x;
  os << "----- Inputs -----\n";
  forall_items(di,_Input_dict) {
    x=_Input_dict.inf(di);
    os << _Input_dict.key(di) << "\t" << hex << x << dec << endl;
  }
}

void Debugger::List_Output(ostream& os)
{
  dic_item di;
  OutputHandler* x;
  os << "----- Outputs -----\n";
  forall_items(di,_Output_dict){
    x=_Output_dict.inf(di);
    os << _Output_dict.key(di) << "\t" << hex << x << dec << endl;
  }
}

void Debugger::List_Conduit(ostream& os)
{
  dic_item di;
  Conduit* x;
  char buf[1024];
  strcpy(buf,"!list_conduit_begin\n");
  write(Log::_out_fd,buf,strlen(buf));

  forall_items(di,_Conduit_dict){
    x=_Conduit_dict.inf(di);

    sprintf(buf,"%d %lx %s\n", _Conduit_dict.key(di), x, x->GetName()); 
    write(Log::_out_fd,buf,strlen(buf));
  }

  sprintf(buf,"!list_conduit_end\n");
  write(Log::_out_fd,buf,strlen(buf));

  os << "----- Conduits -----\n";
  forall_items(di,_Conduit_dict){
    x=_Conduit_dict.inf(di);
    os << _Conduit_dict.key(di) << "\t" << hex << x << dec << " " << x->GetName() << endl;
  }
}

void Debugger::List_SimEntity(ostream& os)
{
  dic_item di;
  SimEntity* x;
  os << "----- SimEntities -----\n";
  forall_items(di,_SimEntity_dict){
    x=_SimEntity_dict.inf(di);
    os << _SimEntity_dict.key(di) << "\t" << hex << x << dec << endl;
  }
}

void Debugger::List_Visitor(ostream& os)
{
  dic_item di;
  os << "----- Visitors -----\n";
  forall_items(di,_Visitor_dict){
    Visitor * x = _Visitor_dict.inf(di);
    char buf[Conduit::MAX_LOG_SIZE];

    VisitorType vt = x->GetType();
    char subtype[50];
    x->Fill_Subtype_Name(subtype);
    if (strcmp(subtype,"")==0)
      sprintf(buf, "%s.%X in %s ", vt.Name(),
	      x, 
	      ((x->CurrentConduit()) ? (x->CurrentConduit()->GetName()) : ""));
    else
      sprintf(buf, "%s.%X in %s ", subtype,
	      x, 
	      ((x->CurrentConduit()) ? (x->CurrentConduit()->GetName()) : ""));
      
    os << _Visitor_dict.key(di) << "\t" << hex << x << dec << " " << buf << endl;
  }
}

// --------------------------------------------------------------------------

int Debugger::SetBreakpoint(BPT* event){
  if (!event) return 0;
  BPT* b;
  int key=0;
  dic_item di;
  forall_items(di,_breakpoints){
    b=_breakpoints.inf(di);
    if (b->equals(*event)){
      key=_breakpoints.key(di);
      break;
    }
  }
  if (key) return key;
  _breakpoints.insert(_bptnum++,event);
  event->setid(_bptnum-1);
  return _bptnum-1;
}

int Debugger::UnsetBreakpoint(int id){
  dic_item di;
  if (di=_breakpoints.lookup(id)){
    _breakpoints.del_item(di);
    return 0;
  }
  else return -1;
}

// --------------------------------------------------------------------------
int Debugger::Notify(BPT * event)
{
  if (!event) 
    return 0;

  if (_verbose || _single_step) {
    cout << "KernelEvent: ";
    event->Print(cout);
    cout << endl;
    if (_single_step && _kernel->_debugging)
      _kernel->_debugging = kdb_Prompt();     // stdin
  }
  

  BPT * b;
  int key = 0;
  dic_item di;
  forall_items(di, _breakpoints) {
    b = _breakpoints.inf(di);
    if (b->equals(*event)) {
      key = _breakpoints.key(di);
      break;
    }
  }

  if (key) {
    cout << "Debugger: Breakpoint " << key << " reached.\n";
  }

  if ( ( event->_interrupt_subtype == BPT::InterruptDestruction ) ||
       ( event->_timer_subtype     == BPT::TimerDestruction     ) ||
       ( event->_input_subtype     == BPT::InputDestruction     ) ||
       ( event->_output_subtype    == BPT::OutputDestruction    ) ||
       ( event->_conduit_subtype   == BPT::ConduitDestruction   ) ||
       ( event->_simentity_subtype == BPT::SimEntityDestruction ) ||
       ( event->_visitor_subtype   == BPT::VisitorDestruction   ) ) {

    int cleanup = 0;

    forall_items(di, _breakpoints) {
      b=_breakpoints.inf(di);
      if (b->affected_by(*event)) {
	b->mark_for_deletion();
	cleanup++;
      }
    }

    if (cleanup) {

      if (_verbose) {
	cout << "Debugger: Automatically deregistering " << 
	  cleanup << " related breakpoints...\n";
      }

      forall_items(di,_breakpoints){
	dic_item destroy;
	bool more=false;
	forall_items(destroy,_breakpoints) {
	  b=_breakpoints.inf(di);
	  if (b->is_marked_for_deletion()) {
	    if (_verbose){
	      cout << "Debugger: ";
	      b->Print(cout);
	      cout << endl;
	    }
	    _breakpoints.del_item(destroy);
	    more=true;
	    break;
	  }
	}
	if (!more) break;
      }
      if (_verbose) {
	cout << "\nDebugger: Cleanup finished.\n";
      }
    }
  }
  
  return key;
}

// ------------- Helper Funcs for kdb_Prompt -----------
#define LOWER(c)  (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))

void skip_spaces(char **string)
{
  for (; **string && isspace(**string); (*string)++);
}

char * first_arg(char * argument, char * first_arg)
{
  // clear all whitespace
  skip_spaces(&argument);

  while (*argument && !isspace(*argument)) {
    *(first_arg++) = LOWER(*argument);
    argument++;
  }

  *first_arg = '\0';

  return argument;
}

char * first_two_args(char *argument, char *one_arg, char *two_arg)
{
  return first_arg(first_arg(argument, one_arg), two_arg);   
}

char * first_three_args(char *argument, char *one_arg, char *two_arg, char* three_arg)
{
  return first_arg(first_arg(first_arg(argument, one_arg), two_arg), three_arg);   
}

// returns true if arg1 is an abbreviation of arg2
bool is_abbrev(char * arg1, char * arg2)
{
  if (!*arg1)
    return false;

  for (; *arg1 && *arg2; arg1++, arg2++)
    if (LOWER(*arg1) != LOWER(*arg2))
      return false;

  if (!*arg1)
    return true;
  else
    return false;
}
// end of helper functions for kdb prompt - mountcas

void ProcessHelp(char * keywords)
{
  cout << "Sorry there is no help on '" << keywords << "'." << endl << flush;
}

// -----------------------------------------------------

bool Debugger::kdb_Prompt(char * text, bool one_shot)
{
  char input[255], word[255];
  bool done = false;

  int max_reps;
  if (one_shot) max_reps= +1;
  else max_reps= -1;
  int rep = 0;
  do {
    rep++;

    if (_verbose)
      cout << endl << "FW-Kernel <verbose KDB> ";
    else
      cout << endl << "FW-Kernel <terse KDB> ";

    if (text == 0) {
      cin.getline(input, 255);
    }
    else {
      strcpy( input, text );
    }
      
    char * remaining = first_arg(input, word);

    if (strlen(word) == 0) {
      if (_prev_arg)
	remaining = first_arg(_prev_arg, word);
      else {
	cout << "Bad command (missing first argument)" << endl;
	continue;
      }
    } else
      strcpy(_prev_arg, input);

    skip_spaces(&remaining);      

    if (is_abbrev(word, "next")) {
      _single_step = true;
      done = true;    
    } else if (is_abbrev(word, "quit")) {
      _kernel->Stop();
      done = true;
      return false;
    } else if (is_abbrev(word, "continue")) {
      _single_step = false;
      return false;
    } else if (is_abbrev(word, "list")) {
      char category[255];
      remaining = first_arg(remaining, category);
      if (strlen(category) == 0) {
	cout << "Bad command (missing second argument)" << endl;
	continue;
      }
      if (is_abbrev(category,"interrupt"))      List_Interrupt(cout);
      else if (is_abbrev(category,"timer"))     List_Timer(cout);
      else if (is_abbrev(category,"input"))     List_Input(cout);
      else if (is_abbrev(category,"output"))    List_Output(cout);
      else if (is_abbrev(category,"conduit"))   List_Conduit(cout);
      else if (is_abbrev(category,"simentity")) List_SimEntity(cout);
      else if (is_abbrev(category,"visitor"))   List_Visitor(cout);
      else if (is_abbrev(category,"break"))     List_Breakpoints(cout);
      else {
	cout << "Bad command (illegal second argument category)" << endl;
	continue;
      }

      done = false;
    } else if (is_abbrev(word, "break")) {      
      char category[255];
      char id[255], subop[255];
      remaining = first_arg(remaining, category);
      if (strlen(category)==0) {
	cout << "Bad command (missing second argument)" << endl;
	continue;
      }
      skip_spaces(&remaining);
      remaining = first_arg(remaining,id);
      if (strlen(id)==0) {
 	cout << "Bad command (missing third argument ID)" << endl;
	continue;
      }
      int ID;
      if (sscanf(id,"%d",&ID)!=1){
 	cout << "Bad command (illegal third argument ID)" << endl;
	continue;
      }

      skip_spaces(&remaining);
      remaining = first_arg(remaining,subop);

      BPT* b;
      if (is_abbrev(category,"interrupt")) {
	if (dic_item dd=_Interrupt_dict.lookup(ID)) {
	  BPT::BPT_Interrupt_triggertype subopcode;
	  TimedDeliveryHandler* x=_Interrupt_dict.inf(dd);
	  if (is_abbrev(subop,"destruction"))
	    subopcode=BPT::InterruptDestruction;
	  else if (is_abbrev(subop,"delivery"))
	    subopcode=BPT::InterruptDelivery;
	  b=new BPT(x,subopcode);
	  int bpn=SetBreakpoint(b);
	  cout << "Breakpoint (interrupt) " << bpn << " is set.\n";
	} else { 
	  cout << "No Interrupt with ID " << ID << " is registered.\n";
	}
      } else if (is_abbrev(category,"timer")) {
  	if (dic_item dd=_Timer_dict.lookup(ID)) {
	  BPT::BPT_Timer_triggertype subopcode;
	  TimerHandler* x=_Timer_dict.inf(dd);
	  if (is_abbrev(subop,"destruction"))
	    subopcode=BPT::TimerDestruction;
	  else if (is_abbrev(subop,"callback"))
	    subopcode=BPT::TimerCallback;
	  b=new BPT(x,subopcode);
	  int bpn=SetBreakpoint(b);
	  cout << "Breakpoint (timer) " << bpn << " is set.\n";
	} else { 
	  cout << "No Timer with ID " << ID << " is registered.\n";
	}
      } else if (is_abbrev(category,"input"))     {
  	if (dic_item dd=_Input_dict.lookup(ID)) {
	  BPT::BPT_Input_triggertype subopcode;
	  InputHandler* x=_Input_dict.inf(dd);
	  if (is_abbrev(subop,"destruction"))
	    subopcode=BPT::InputDestruction;
	  else if (is_abbrev(subop,"callback"))
	    subopcode=BPT::InputCallback;
	  b=new BPT(x,subopcode);
	  int bpn=SetBreakpoint(b);
	  cout << "Breakpoint (input) " << bpn << " is set.\n";
	} else { 
	  cout << "No InputHandler with ID " << ID << " is registered.\n";
	}
      } else if (is_abbrev(category,"output"))     {
    	if (dic_item dd=_Output_dict.lookup(ID)) {
	  BPT::BPT_Output_triggertype subopcode;
	  OutputHandler* x=_Output_dict.inf(dd);
	  if (is_abbrev(subop,"destruction"))
	    subopcode=BPT::OutputDestruction;
	  else if (is_abbrev(subop,"callback"))
	    subopcode=BPT::OutputCallback;
	  b=new BPT(x,subopcode);
	  int bpn=SetBreakpoint(b);
	  cout << "Breakpoint (output) " << bpn << " is set.\n";
	} else { 
	  cout << "No OutputHandler with ID " << ID << " is registered.\n";
	}
      } else if (is_abbrev(category,"conduit"))     {
    	if (dic_item dd=_Conduit_dict.lookup(ID)) {
	  BPT::BPT_Conduit_triggertype subopcode;
	  Conduit* x=_Conduit_dict.inf(dd);
	  if (is_abbrev(subop,"destruction")){
	    subopcode=BPT::ConduitDestruction;
	    b=new BPT(x,subopcode);
	    int bpn=SetBreakpoint(b);
	    cout << "Breakpoint (conduit) " << bpn << " is set.\n";
	  } else if (is_abbrev(subop,"intrusion")) {
	    subopcode=BPT::ConduitIntrusion;
	    char auxid[255];
	    remaining = first_arg(remaining, auxid);
	    if (strlen(auxid)==0) {
	      cout << "Bad command (missing fifth argument ID|any)" << endl;
	      continue;
	    }
	    if (is_abbrev(auxid,"any")){
	      b=new BPT(x,subopcode,0);
	      int bpn=SetBreakpoint(b);
	      cout << "Breakpoint (conduit) " << bpn << " is set.\n";
	    } else {
	      int auxnum;
	      if (sscanf(auxid,"%d",&auxnum) != 1) {
		cout << "Bad command (illegal fifth argument ID)" << endl;
		continue;
	      }
	      if (dic_item dd=_Visitor_dict.lookup(auxnum)) {
 		b = new BPT(x,subopcode,_Visitor_dict.inf(dd));
		int bpn=SetBreakpoint(b);
		cout << "Breakpoint (conduit) " << bpn << " is set.\n";
	      } else {
		cout << "No Visitor with ID " << auxnum << " is registered.\n";
	      }
	    }
	  }
	} else { 
	  cout << "No Conduit with ID " << ID << " is registered.\n";
	}
      } else if (is_abbrev(category,"simentity")) {
     	if (dic_item dd=_SimEntity_dict.lookup(ID)) {
	  BPT::BPT_SimEntity_triggertype subopcode;
	  SimEntity* x=_SimEntity_dict.inf(dd);
	  if (is_abbrev(subop,"destruction")){
	    subopcode=BPT::SimEntityDestruction;
	    b=new BPT(x,subopcode);
	    int bpn=SetBreakpoint(b);
	    cout << "Breakpoint (simentity) " << bpn << " is set.\n";
	  } else if (is_abbrev(subop,"interruption")){
	    subopcode=BPT::SimEntityInterruption;
	    char auxid[255];
	    remaining = first_arg(remaining, auxid);
	    if (strlen(auxid)==0) {
	      cout << "Bad command (missing fifth argument ID|any)" << endl;
	      continue;
	    }
	    if (is_abbrev(auxid,"any")){
	      b=new BPT(x,subopcode,0);
	      int bpn=SetBreakpoint(b);
	      cout << "Breakpoint (simentity) " << bpn << " is set.\n";
	    } else {
	      int auxnum;
	      if (sscanf(auxid,"%d",&auxnum)!=1){
		cout << "Bad command (illegal fifth argument ID)" << endl;
		continue;
	      }
	      if (dic_item dd=_Interrupt_dict.lookup(auxnum)){
		b=new BPT(x,subopcode,_Interrupt_dict.inf(dd));
		int bpn=SetBreakpoint(b);
		cout << "Breakpoint (simentity) " << bpn << " is set.\n";
	      } else {
		cout << "No Interrupt/SimEvent with ID " << auxnum << " is registered.\n";
	      }
	    }
	  }
	} else { 
	  cout << "No Simentity with ID " << ID << " is registered.\n";
	}
      } else if (is_abbrev(category,"visitor"))   {
     	if (dic_item dd=_Visitor_dict.lookup(ID)){
	  BPT::BPT_Visitor_triggertype subopcode;
	  Visitor* x=_Visitor_dict.inf(dd);
	  if (is_abbrev(subop,"destruction")){
	    subopcode=BPT::VisitorDestruction;
	    b=new BPT(x,subopcode);
	    int bpn=SetBreakpoint(b);
	    cout << "Breakpoint (visitor) " << bpn << " is set.\n";
	  } else if (is_abbrev(subop,"arrival")){
	    subopcode=BPT::VisitorArrival;
	    char auxid[255];
	    remaining = first_arg(remaining, auxid);
	    if (strlen(auxid)==0) {
	      cout << "Bad command (missing fifth argument ID|any)" << endl;
	      continue;
	    }
	    if (is_abbrev(auxid,"any")){
	      b=new BPT(x,subopcode, 0);
	      int bpn=SetBreakpoint(b);
	      cout << "Breakpoint (visitor) " << bpn << " is set.\n";
	    } else {
	      int auxnum;
	      if (sscanf(auxid,"%d",&auxnum)!=1){
		cout << "Bad command (illegal fifth argument ID)" << endl;
		continue;
	      }
	      if (dic_item dd=_Conduit_dict.lookup(auxnum)){
		b=new BPT(x,subopcode,_Conduit_dict.inf(dd));
		int bpn=SetBreakpoint(b);
		cout << "Breakpoint (visitor) " << bpn << " is set.\n";
	      } else {
		cout << "No Conduit with ID " << auxnum << " is registered.\n";
	      }
	    }
	  }
	} else { 
	  cout << "No Visitor with ID " << ID << " is registered.\n";
	}
      } else {
	cout << "Bad command (illegal second argument category)" << endl;
	continue;
      }
      done = false;
    } else if (is_abbrev(word, "delete")) {
      char id[255];
      skip_spaces(&remaining);
      remaining = first_arg(remaining,id);
      if (strlen(id)==0) {
 	cout << "Bad command (missing second argument ID)" << endl;
	continue;
      }
      int ID;
      if (sscanf(id,"%d",&ID)!=1){
 	cout << "Bad command (illegal second argument ID)" << endl;
	continue;
      }
      if (UnsetBreakpoint(ID)==-1){
	cout << "Sorry, no breakpoint with ID " << ID << " is registered.";
      } else {
	cout << "Breakpoint #"<< ID << " deregistered.";
      }
    } else if (is_abbrev(word, "help")) {
      help();
    } else if (is_abbrev(word, "time")) {
      cout << "KernelTime = " << _kernel->CurrentTime() << endl;
    } else if (is_abbrev(word, "verbose")) {
      cout << "FW-Kernel is now in verbose mode.";
      _verbose=true;
    } else if (is_abbrev(word, "terse")) {
      cout << "FW-Kernel is now in terse mode.";
      _verbose=false;
    } else if (is_abbrev(word, "print")) {
      char func[255];
      remaining = first_arg(remaining, func);
      if (!strcmp(func,"gdb()")) {
	ALERT_DEBUGGER;
      } else {
	cout << "Bad command (argument 2 is unknown pop target)" << endl;
	continue;
      }
    } else {
      cout << "Unknown command!" << endl;
      help();
    }
  } while ((!done) && ( rep < max_reps));

  return true;
}

bool kdb(void) { return theKernel().KDB()->kdb_Prompt(); }  // kick us into kdb

void help(void)
{
  cout << "\n";
  cout << "              KDB Quick Reference\n";
  cout << "\n";
  cout << "verbose -- chatty operation of the FW-Kernel\n";
  cout << "terse   -- quiet(er) operation of the FW-kernel\n";
  cout << "time	   -- print the current simulation time\n";
  cout << "help    -- display help file\n";
  cout << "\n";
  cout << "list	interrupt	-- lists all registered interrupt events\n";
  cout << "     timer		-- lists all registered timers\n";
  cout << "     input		-- lists all registered input handlers\n";
  cout << "     output		-- lists all registered output handlers\n";
  cout << "     conduit		-- lists all conduits\n";
  cout << "     simentity 	-- lists all simentities\n";
  cout << "     visitor 	-- lists all visitors\n";
  cout << "     break		-- lists all breakpoints\n";
  cout << "\n";
  cout << "break interrupt <interrupt#> destruction\n";
  cout << "                             delivery\n";
  cout << "\n";
  cout << "      timer     <timer#>     callback\n";
  cout << "                             destruction\n";
  cout << "\n";
  cout << "      input     <input#>     callback\n";
  cout << "                             destruction\n";
  cout << "\n";
  cout << "      output    <output#>    callback\n";
  cout << "                             destruction\n";
  cout << "\n";
  cout << "      conduit   <conduit#>   destruction\n";
  cout << "                             intrusion    <visitor#>\n";
  cout << "                                          any\n";
  cout << "\n";
  cout << "      simentity <simentity#> destruction\n";
  cout << "                             interruption <interrupt#>\n";
  cout << "                                          any\n";
  cout << "\n";
  cout << "      visitor   <visitor#>   destruction\n";
  cout << "                             arrival      <conduit#>\n";
  cout << "                                          any\n";
  cout << "\n";
  cout << "delete <breakpoint#>	-- deletes a breakpoint\n";
  cout << "\n";
  cout << "next 	-- proceed until next kernel event\n";
  cout << "continue 	-- proceed until breakpoint\n";
  cout << "\n";
  cout << "p gdb()      -- pop into gdb\n";
  cout << "p kdb()      -- pop into kdb\n";
  cout << "p Visualize(\" <filename> \") -- Visualize the conduit record so far\n";
}

void Debugger::Interrupt(SimEvent * se)
{
  // Doh!
}

