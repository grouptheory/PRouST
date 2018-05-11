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
static char const _Kernel_cc_rcsid_[] =
"$Id: Kernel.cc,v 1.4 1999/01/19 15:04:17 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/kernel/Kernel.h>
#include <FW/kernel/SimEvent.h>
#include <FW/kernel/SimEntity.h>
#include <FW/kernel/KDB.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include <FW/basics/Log.h>

extern "C" {
#include <math.h>
#include <signal.h>
#include <fcntl.h>
};

bool TestTheKernel = true;
extern int MEMORY_DEBUGGING;
extern double DELAY_THRESHOLD;

// MEMORY_DEBUGGING
#define KERNEL_INVARIANTS 1
#define TIMELINE_ORDER    2
#define DELAY_WARNINGS    4
#define VERBOSE_DEBUG     8


#include "Kernel_NV_Synch.h"


#define __on__KERNEL_INVARIANTS   (MEMORY_DEBUGGING & KERNEL_INVARIANTS)
#define __on__TIMELINE_ORDER      (MEMORY_DEBUGGING & TIMELINE_ORDER)
#define __on__DELAY_WARNINGS      (MEMORY_DEBUGGING & DELAY_WARNINGS)
#define __on__VERBOSE_DEBUG       (MEMORY_DEBUGGING & VERBOSE_DEBUG)

void test_kernel(bool);

#define TESTKERNEL { if (__on__KERNEL_INVARIANTS) test_kernel(false); }
#define TESTTIMERS { if (__on__TIMELINE_ORDER) test_timeline(); }

Kernel & theKernel(void)
{
  return *(Kernel::_kernel_singleton);
}

Kernel * Kernel::_kernel_singleton = 0;

// ---------------- OS Dep. Calls ---------------
double GetTimeOfDay(struct timezone * tzp = 0)
{ TESTKERNEL; 
  struct timeval tv;
  ::gettimeofday(&tv, tzp);

  return (double)(tv.tv_sec + (((double)tv.tv_usec) / 1000000.0));
  TESTKERNEL;
}

int Select(int size, fd_set * rfds, fd_set * wfds = 0,
	   fd_set * efds = 0, struct timeval * tvp = 0)
{ TESTKERNEL; 
  return ::select(size, rfds, wfds, efds, tvp);
  TESTKERNEL;
}

#ifndef FD_SET
#define FD_SET FD_SET // define this with the equiv function in your OS
#endif

#ifndef FD_CLR
#define FD_CLR FD_CLR // define this with the equiv function in your OS
#endif

#ifndef FD_ISSET
#define FD_ISSET FD_ISSET // define this with the equiv function in your OS
#endif

#ifndef FD_ZERO
#define FD_ZERO FD_ZERO // define this with the equiv function in your OS
#endif

#ifndef assert
#define assert(x)  ( !x ? abort() : ; )
#endif

// --------------------- Kernel ------------------------
Kernel::Kernel(Speed sp) : _next_token(1), _running(false), _debugging(false),
  _timeline(0), _boot_time(0), _timescale(0), _pure_simulation(true), 
  _sim_time(0), _xxx(true), _last_time(-1.0)
{ 
  _timeline = new p_queue<KernelTime, TimerHandler *>;
  
  _boot_time = KernelTime( GetTimeOfDay() );
  _sim_time = _boot_time;

  FD_ZERO(&_in_fdset);
  FD_ZERO(&_out_fdset);
  FD_ZERO(&_save_in_fdset);
  FD_ZERO(&_save_out_fdset);

  SetSpeed(sp);

  _debugger = new Debugger(this);
  Kernel_Governor::_singleton = new Kernel_Governor();

  _in_lockstep=false;
  _blocked=false;

#ifdef __USE_SIGQUIT_AS_STOP__
  signal(SIGQUIT, Kernel::Stop);
#endif
TESTKERNEL; }

// -----------------------------------------------------
Kernel::~Kernel(void)
{
  TestTheKernel = false;
  dic_item ditem = 0;

  // Clean the queues and arrays ...
  forall_items(ditem, _timers) {
    TimerHandler *h = _timers.inf(ditem);
    delete h;
  }
  _timers.clear();
  
  forall_items(ditem, _inputs) {
    InputHandler *h = _inputs.inf(ditem);
    delete h;
  }
  _inputs.clear();
  
  forall_items(ditem, _outputs) {
    OutputHandler *h = _outputs.inf(ditem);
    delete h;
  }
  _outputs.clear();
  
  forall_items(ditem, _events) {
    SimEvent *e = _events.inf(ditem);
    delete e;
  }
  _events.clear();

  FreeConduits();

  _timeline->clear();
  _in_fds.clear();
  _out_fds.clear();
  _pqset.clear();

  delete _timeline;

  delete _debugger;
}

// -----------------------------------------------------
// Our time starts at the real-world _boot_time ,
// and proceeds at a rate of _timescale units per real-world second

KernelTime Kernel::CurrentTime(void)
{ TESTKERNEL;
  if (!_pure_simulation) { TESTKERNEL;
    KernelTime now( GetTimeOfDay() );
    return (((now - _boot_time) * _timescale) + _boot_time);
  TESTKERNEL; } else
    return _sim_time;
TESTKERNEL; }

KernelTime Kernel::CurrentElapsedTime(void)
{ TESTKERNEL;
  if (!_pure_simulation) {
    KernelTime now( GetTimeOfDay() );
    // Where is timescale initialized to 1 if we are running in real=time?!?!?!?!??!?!!?
    return ((now - _boot_time) * _timescale);
  } else
    return (_sim_time - _boot_time);
TESTKERNEL; }

KernelTime Kernel::CurrentSimTime(void)
{ TESTKERNEL;

  if (_pure_simulation)
    return (_sim_time - _boot_time);
  else
    return CurrentElapsedTime();
TESTKERNEL; }

// -----------------------------------------------------
// Some time in the future

KernelTime Kernel::Future(double seconds)
{ TESTKERNEL;
  return (CurrentTime() + KernelTime(seconds));
TESTKERNEL; }

// -----------------------------------------------------
bool Kernel::Register(TimerHandler * t)
{ 
  TESTTIMERS;
  assert(t);
  
  KernelToken k = NextToken();
  _timers.insert(k, t);
  t->SetToken(k);

  KernelTime e = t->GetExpirationTime();
  pq_item pi = _timeline->insert(e,t);
  _pqset.insert(k, pi);

  TESTTIMERS;
  return true;
}

// -----------------------------------------------------
bool Kernel::Register(InputHandler * i)
{ TESTKERNEL;
  assert(i);

  int fd;
  if (_in_fds.lookup(fd = i->GetFD())) 
    return false;
  else { TESTKERNEL;
    KernelToken k = NextToken();
    i->SetToken(k);
    _inputs.insert(k, i);
    _in_fds.insert(fd, i);
    FD_SET(fd, &_in_fdset);
    return true;
  TESTKERNEL; }
TESTKERNEL; }

// -----------------------------------------------------
bool Kernel::Register(OutputHandler * o)
{ TESTKERNEL;
  assert(o);

  int fd;
  if (_out_fds.lookup(fd = o->GetFD())) 
    return false;
  else { TESTKERNEL;
    KernelToken k = NextToken();
    o->SetToken(k);
    _outputs.insert(k, o);
    _out_fds.insert(fd, o);
    FD_SET(fd, &_out_fdset);
    return true;
  TESTKERNEL; }
TESTKERNEL; }

// -----------------------------------------------------
bool Kernel::Register(SimEvent* e)
{ TESTKERNEL;
  assert(e);

  KernelToken k = NextToken();
  e->SetToken(k);
  _events.insert(k, e);
  SimEntity * target = e->_to;

  Deregister(k);

  target->Interrupt(e);
  return true;
TESTKERNEL; }

// -----------------------------------------------------
bool Kernel::Deregister(KernelToken t)
{
  TESTTIMERS;
  dic_item di;
  if (di = _timers.lookup(t)) {
    _timers.del_item(di);
    if (di = _pqset.lookup(t)) {
      pq_item pi = _pqset.inf(di);
      _timeline->del_item( pi );
      _pqset.del_item(di);
    }
    TESTTIMERS;
    return true;
  } else if (di = _inputs.lookup(t)) {
    int fd = _inputs.inf(di)->GetFD();
    FD_CLR(fd, &_in_fdset);
    _inputs.del_item(di);
    _in_fds.del(fd);
    return true;
  } else if (di = _outputs.lookup(t)) {
    int fd = _outputs.inf(di)->GetFD();
    FD_CLR(fd, &_out_fdset);
    _outputs.del_item(di);
    _out_fds.del(fd);
    return true;
  } else if (di = _events.lookup(t)) {
    SimEvent * e = _events.inf(di);
    _events.del_item(di);
    return true;
  } else 
    return false;
}

bool Kernel::IsRegistered(KernelToken t)
{ TESTKERNEL;
  if (_timers.lookup(t))
    return true;
  if (_inputs.lookup(t))
    return true;
  if (_outputs.lookup(t))
    return true;
  if (_events.lookup(t))
    return true;
  return false;
TESTKERNEL; }

// -----------------------------------------------------
void Kernel::_Run(void)
{ TESTKERNEL;
  if (_running) 
    return;

  if (_xxx) { TESTKERNEL;
    AppendXXXToRecord();
  TESTKERNEL; }

  _running = true;
  while (_running)
    ProcessNextEvent();
TESTKERNEL; }

// -----------------------------------------------------
void Kernel::DisableXXX(void)
{ TESTKERNEL;
  _xxx = false;
TESTKERNEL; }

// -----------------------------------------------------
void Kernel::Run(void)
{ TESTKERNEL;
  _debugging = false;
  _debugger->_single_step=false;

  if (_running) 
    return;
  else
    _Run();
TESTKERNEL; }

// -----------------------------------------------------
void Kernel::Debug(void)
{ TESTKERNEL;
  _debugging = true;
  _debugger->_single_step=true;

  if (_running) 
    return;
  else
    _Run();
TESTKERNEL; }

// -----------------------------------------------------
void Kernel::Stop(void)
{ TESTKERNEL;
  _running = false; 
  
  cout << "Kernel is being stopped\n";

  // For double clicking Continue in GT
  AppendTimeToRecord();
TESTKERNEL; }

// -----------------------------------------------------
void Kernel::ProcessNextEvent(void)
{
  char buf[1024];
  pq_item pq;

  bool timeline_empty;

  if (_timeline->empty()) 
  { 
    timeline_empty = true;
    if (__on__VERBOSE_DEBUG) {
      cout << "The timeline is empty\n";
    }
    KernelTime long_interval = 1000.0;
    WaitForInput(long_interval);
  } 
  else if (pq = _timeline->find_min()) // find the next timer to go off
  {
    timeline_empty = false;
    TimerHandler * th = _timeline->inf(pq);

    KernelTime th_exp, now;
    if ((th_exp = th->GetExpirationTime()) > (now = CurrentTime())) {
      KernelTime diff;
      diff = (th_exp - now);
      if (__on__VERBOSE_DEBUG) {
	cout << "diff outside = " << diff << endl;
      }

      while ( (!timeline_empty) && ((diff = (th_exp - now)) > (KernelTime)0) ) {
	if (__on__VERBOSE_DEBUG) {
	  cout << "diff inside = " << diff << endl;
	}

	// Wait until this TimerHandler should be executed.
	WaitForInput(diff);
	if (_timeline->empty()) timeline_empty=true;
	else {
	  now = CurrentTime();
	  pq = _timeline->find_min();
	  th = _timeline->inf(pq);
	  th_exp = th->GetExpirationTime();

	  if (_pure_simulation)
	    _sim_time = th_exp;
	}
      };
    }
    
    if ( ! timeline_empty ) {

      if (__on__VERBOSE_DEBUG) {
	cout << "ProcessNextEvent loop";  
	if (_pure_simulation) cout << "in pure simulation\n";
	else cout << "not pure simulation\n";
      }

      // Now we should be ready for the call back ...
      int is_breakpoint = 0;
      TESTTIMERS;
      if (th->IsInterruptMessenger()) {
	BPT * b = new BPT((TimedDeliveryHandler*)th,BPT::InterruptDelivery);
	is_breakpoint = _debugger->Notify(b);
	delete b;
      } else {
	BPT * b = new BPT(th,BPT::TimerCallback);
	is_breakpoint = _debugger->Notify(b);
	delete b;
      }
      // push the simulation clock
      if (_pure_simulation)
	_sim_time = th_exp;
      AppendTimeToRecord();
    
      if (is_breakpoint) {
	ALERT_DEBUGGER;
      }
      
      // save the oldtoken, in case the
      // timer handlers callback does a 'delete this'--
      // so we can still clean up
      KernelToken oldtoken = th->_token;
      Deregister(oldtoken);

      TESTTIMERS;    
      if (__on__DELAY_WARNINGS) {
	KernelTime bang = CurrentTime();
	KernelTime boom = th->GetExpirationTime();
	double delay = bang-boom;
	if (delay> DELAY_THRESHOLD) {
	  cout << "Kernel notes a dangerous postponement of callback by " << delay << " seconds." << endl;
	}
	if (delay<0) {
	  cout << "Kernel notes callback is ahead of schedule by " << delay << " seconds." << endl;
	}
      }
      th->Callback();
      TESTTIMERS;
    }
  }
  FreeConduits();
  // return;
}

// -----------------------------------------------------
void Kernel::SetSpeed(Kernel::Speed newspeed)
{ TESTKERNEL;
  switch (newspeed) { 
  case QUARTER_SPEED:
  case HALF_SPEED: 
  case REAL_TIME:
  case DOUBLE_SPEED:
  case QUADRUPLE_SPEED:
    SetTimeScale(pow(2.0, (double)newspeed));
    _pure_simulation = false;
    break;

  case SIM_SPEED:
  default:
    SetTimeScale(1.0);
    _pure_simulation = true;
    break;
  }
TESTKERNEL; }

// -----------------------------------------------------
Kernel::Speed Kernel::GetSpeed(void)
{ TESTKERNEL;
  int x = (int) (log( GetTimeScale() ) / log( 2.0 ));
  Speed xspeed;
  switch (x){
  case QUARTER_SPEED: 
  case HALF_SPEED: 
  case DOUBLE_SPEED:
  case QUADRUPLE_SPEED:
  case SIM_SPEED:
    xspeed = (Speed)x;
    break;
  case REAL_TIME:
    if ( _pure_simulation )
      xspeed = SIM_SPEED;
    else
      xspeed = REAL_TIME;
    break;
  default:
    xspeed = UNKNOWN_SPEED;
  }
  return xspeed;
TESTKERNEL; }

// -----------------------------------------------------
void Kernel::SetTimeScale(double newtimescale)
{ TESTKERNEL; 
  if (!_pure_simulation){ TESTKERNEL;
    _boot_time =  KernelTime( GetTimeOfDay() );
  TESTKERNEL; } else 
    _boot_time = _sim_time;

  // Assign new timescale
  _timescale = newtimescale; 
TESTKERNEL; }


// -----------------------------------------------------
double Kernel::GetTimeScale(void)
{ TESTKERNEL; return _timescale; TESTKERNEL; }

// -----------------------------------------------------
KernelToken Kernel::NextToken(void)
{ TESTKERNEL;
  KernelToken answer = _next_token;
  _next_token = KernelToken(_next_token._id+1);
  return answer;
TESTKERNEL; }

Debugger * Kernel::KDB(void) const { return _debugger; }

// -----------------------------------------------------
void Kernel::WaitForInput(KernelTime t)
{ TESTKERNEL;
  KernelTime true_t = t;

  if (!_pure_simulation)
    true_t = t * (1.0/_timescale);

  _save_in_fdset  = _in_fdset;
  _save_out_fdset = _out_fdset;
  
  if (_debugging)
    cout << "Waiting for " << t << " simulation seconds -- (" << true_t
	 << " real-world sec)" << endl;

  struct timeval * tvp = true_t.timeval();
  if (_pure_simulation)
    { TESTKERNEL; tvp->tv_sec = tvp->tv_usec = 0; TESTKERNEL; }

  if (__on__VERBOSE_DEBUG) cout << "Entering Select\n";
  int numio =
    Select(FD_SETSIZE, &_save_in_fdset, &_save_out_fdset, (fd_set *)0, tvp);
  // Changed 3/23/98 9:50am - mountcas
  free(tvp);
  if (__on__VERBOSE_DEBUG) cout << "Leaving select, numio= " << numio << "\n";

  if (numio > 0) { TESTKERNEL;
    int fd;

    for (fd = 0; fd < FD_SETSIZE; fd++) { TESTKERNEL;
      if (FD_ISSET(fd, &_save_in_fdset)) {// Changed 3/23/98 9:50am - mountcas
	TESTKERNEL;
	InputHandler * h = _in_fds.inf(_in_fds.lookup(fd));

	BPT* b=new BPT(h,BPT::InputCallback);
	int is_breakpoint=_debugger->Notify(b);
	delete b;

	if (is_breakpoint) { TESTKERNEL;
	  ALERT_DEBUGGER;
	TESTKERNEL; }
	h->Callback();
      TESTKERNEL; }
    TESTKERNEL; }

    for (fd = 0; fd < FD_SETSIZE; fd++) { TESTKERNEL;
    if (FD_ISSET(fd, &_save_out_fdset)) { // Changed 3/23/98 9:50am - mountcas
      TESTKERNEL;
	OutputHandler* h = _out_fds.inf(_out_fds.lookup(fd));

	BPT* b=new BPT(h,BPT::OutputCallback);
	int is_breakpoint=_debugger->Notify(b);
	delete b;

	if (is_breakpoint) { TESTKERNEL;
	  ALERT_DEBUGGER;
	TESTKERNEL; }
	h->Callback();
      TESTKERNEL; }
    TESTKERNEL; }
  TESTKERNEL; }

  // Push the simulation clock
  if (_pure_simulation)
    _sim_time = _sim_time + true_t;
TESTKERNEL; }

void Kernel::StopIn(double t)
{ TESTKERNEL;
  StopTimer * stop = new StopTimer(t);
  stop->_Register();

  if (__on__VERBOSE_DEBUG) {
    cout << "After StopIn Registers...\n";
    dump_timers();
  }

TESTKERNEL; }

void Kernel::CleanQueue(void)
{ TESTKERNEL;
  while (RemoveUnreg())
    ;
TESTKERNEL; }

bool Kernel::RemoveUnreg(void) 
{ TESTKERNEL;
  dic_item di;
  forall_items(di, _pqset) { TESTKERNEL;
    TimerHandler * th = _timeline->inf( _pqset.inf(di) );

    if (!(th->IsRegistered())) { TESTKERNEL;
      // _timeline->del_item( _pqset.inf(di) );
      Deregister(th->_token);
      return true;;
    TESTKERNEL; }
  TESTKERNEL; }
  return false;
TESTKERNEL; }

void Kernel::dump_timers(void)
{
  if (_timers.empty() && _pqset.empty() && _timeline->empty())
    return;

  cout << "----------------------------------------------------------" << endl;
  // Dump _timers
  dic_item di; int i = 0;
  cout << "Timers (dictionary<KernelToken, TimerHandler *>):" << endl;
  forall_items(di, _timers) {
    cout << i << ") KernelToken: " << _timers.key(di)
	 << " TimerHandler: " << _timers.inf(di) << endl;
  }
  cout << endl << "PQSet (dictionary<KernelToken, pq_item>):" << endl;
  // Dump _pqset
  forall_items(di, _pqset) {
    cout << "KernelToken: " << _pqset.key(di)
	 << " pq_item: " << _pqset.inf(di) << endl;
  }
  cout << endl << "Timeline (p_queue<KernelTime, TimerHandler *>):" << endl;
  // Dump _timeline
  pq_item pi; i = 0;
  forall_items(pi, *_timeline) {
    cout << i << ") pq_item: " << pi << " Time: " << _timeline->prio(pi)
	 << " TimerHandler: " << _timeline->inf(pi) << endl;
  }
  cout << "----------------------------------------------------------" << endl;
}

// Appends the time, relative to the boot time, to the record
void Kernel::AppendTimeToRecord(void)
{
  char text[255];
  double time_now = -1.0;

  if (_pure_simulation)
    time_now = _sim_time - _boot_time;
  else
    time_now = CurrentTime() - _boot_time;

  if (time_now != _last_time) {
    sprintf(text, "!time %lf", time_now);
    theLog().AppendToRecord(text);
    _last_time = time_now;

    DIAG("FW.kernel", DIAG_INFO, cout << "Current Simulation Time is "
	 << CurrentElapsedTime() << endl);
  }
}

void Kernel::AppendXXXToRecord(void)
{
  char text[255];
  strcpy(text, "xxxxxxxxxxxxxxxx");
  theLog().AppendToRecord(text);
}

void Kernel::FreeConduits(void)
{
  while (!(_free_list.empty())) {
    list_item li = _free_list.first();
    delete _free_list.inf(li);
  }
  _free_list.clear();
}

void Kernel::AddConduit(Conduit * c)
{
  _free_list.append(c);
}

void Kernel::RemConduit(Conduit * c)
{
  list_item li;
  if (li = _free_list.search(c))
    _free_list.del_item(li);
}

void  Kernel::Block(void) {
  _blocked = true;
}

void  Kernel::Unblock(void) {
  _blocked = false;
}

void Kernel::Set_Lockstep_Mode_Off(void) {
  _in_lockstep = false;
}

void Kernel::Set_Lockstep_Mode_On(char* pipe) {
  _in_lockstep = true;
  Kernel_Governor::_singleton->Setup(pipe);
}

void  Kernel::Lockstep_Notification(void) {
  if (_in_lockstep) {
    diag("FW.kernel.lockstep", DIAG_DEBUG, "Kernel entering synch point...\n");
    Block_for_NV();
    diag("FW.kernel.lockstep", DIAG_DEBUG, "Kernel leaves synch point...\n");
  }
}




// -----------------------------------------------
void Kernel_Governor::Setup(char* pipe) {
   _save_index = 0;

    diag("FW.kernel.lockstep", DIAG_DEBUG, "Kernel_Governor begins setup...\n");

    _fdin = open(pipe,O_RDONLY|O_NDELAY|O_CREAT, 777);
    if (_fdin>0) 
      diag("FW.kernel.lockstep", DIAG_DEBUG, "input pipe %s opened successfully.\n",pipe);
    else 
      diag("FW.kernel.lockstep", DIAG_DEBUG, "input pipe %s open failed.\n",pipe);
    
    FD_ZERO(&_in_fdset);
    FD_ZERO(&_ignore_fdset);
    
    FD_SET(_fdin, &_in_fdset);

    diag("FW.kernel.lockstep", DIAG_DEBUG, "Kernel_Governor ends setup...\n");
}

// -----------------------------------------------
Kernel_Governor::Kernel_Governor(void) {
}

// -----------------------------------------------
Kernel_Governor::~Kernel_Governor() {
}

// -----------------------------------------------
void Kernel_Governor::Relinquish_Token(void) {
  theKernel().Block();
  theLog().AppendToRecord(TOKEN_KERNEL_TO_NV);
}

// -----------------------------------------------
bool Kernel_Governor::Attempt_To_Acquire_Token(void) {
  char c; 
  bool eoline = false;
  int index = _save_index; 

  while (read(_fdin, &c, 1) == 1) {  // still stuff to read
    _raw[index] = c;
    index++;
    if ( c == '\n' ) {
      eoline = true;
    }
  };
  _save_index = index;   // in case the tokens not around
  _raw[index] = 0;

  bool token_acquired = false;
  char* token_loc;
  if ( token_loc = strstr(_raw,TOKEN_NV_TO_KERNEL) ) {     // token found
    int tail = strlen(token_loc);

    *token_loc=0;
    strcpy(_next_command_block,_raw);

    int len = tail - (strlen(TOKEN_NV_TO_KERNEL) + 1);
    char* start = token_loc + (strlen(TOKEN_NV_TO_KERNEL) + 1);
    bcopy(start, _raw, len);
    _raw[len] = 0;
    _save_index = len;
    token_acquired = true;
  }
  return token_acquired;
}

// -----------------------------------------------
void Kernel_Governor::Wait_For_Input(void) {
  ::select(_fdin+1,&_in_fdset,&_ignore_fdset,&_ignore_fdset,0);
}

// -----------------------------------------------
void Kernel_Governor::Wait_Until_Token_Returns(void) {
  bool token_acquired;
  do {
    Wait_For_Input();
    token_acquired = Attempt_To_Acquire_Token();
  }
  while (!token_acquired);
  theKernel().Unblock();
}

// -----------------------------------------------
void Kernel_Governor::Process_Next_Command_Block(void) {
  char* begin = _next_command_block;
  char* nextline = 0;
  while( nextline = strchr( begin,'\n') ) {
    *nextline = 0;
    cout << "Kernel_Governor ==> " << begin << endl;
    theKernel().KDB()->kdb_Prompt( begin, true );
    begin = nextline+1;
  };
  strcpy( _next_command_block,"" );
}

Kernel_Governor* Kernel_Governor::_singleton = 0;

// -----------------------------------------------
void Block_for_NV(void) {
  Kernel_Governor* gov = Kernel_Governor::_singleton;
  assert (gov);

  diag("FW.kernel.lockstep", DIAG_DEBUG, "Kernel sends token to NV...\n");
  gov->Relinquish_Token();    

  diag("FW.kernel.lockstep", DIAG_DEBUG, "Kernel waits for token to return...\n");
  gov->Wait_Until_Token_Returns();    
  
  diag("FW.kernel.lockstep", DIAG_DEBUG, "Kernel processes NV commands...\n");
  gov->Process_Next_Command_Block();
}

void test_timeline(void)
{
  if (theKernel()._timers.empty() && 
      theKernel()._pqset.empty() && 
      theKernel()._timeline->empty())
    return;

  if ( MEMORY_DEBUGGING & VERBOSE_DEBUG ) {
    cout << "Entering 'test_timeline'" << endl;
    theKernel().dump_timers();
  }

  dictionary<KernelToken, TimerHandler *> & timers = theKernel()._timers;
  dictionary<KernelToken, pq_item> & pqset = theKernel()._pqset;
  p_queue<KernelTime, TimerHandler *> & tl = *(theKernel()._timeline);

  // 1. Check _timers
  dic_item di;
  forall_items(di, timers) {
    TimerHandler * th = timers.inf(di);
    pq_item pi = 0, th_pi = 0;
    forall_items(pi, tl) {
      TimerHandler *tl_th = tl.inf(pi);
      if (tl_th == th) {
	th_pi = pi;
	break;
      }
    }
    KernelToken tok = th->_token;
    dic_item di2 = pqset.lookup(tok);
    pq_item pqs_pi = pqset.inf(di2);
    // The two pq_items should be equal
    assert( th_pi == pqs_pi );
    // The KernelToken from the Timer should equal our key
    assert( tok == timers.key(di) );

    // Check for duplicate keys and values
    KernelToken    key = timers.key(di);
    TimerHandler * val = timers.inf(di);
    dic_item di3;
    int kk = 0, kv = 0;
    forall_items(di3, timers) {
      if (timers.key(di3) == key)
	kk++;
      if (timers.inf(di3) == val)
	kv++;
    }
    // We should find only one item that matches in each case.
    assert(kk == 1);
    assert(kv == 1);
  }

  // 2. Check _pqset
  forall_items(di, pqset) {
    KernelToken key = pqset.key(di);
    pq_item     val = pqset.inf(di);
    TimerHandler * tth = timers.inf( timers.lookup(key) );
    TimerHandler * pth = tl.inf( val );
    // The TH * in _timers should equal the TH * in _timeline
    assert( tth == pth );
    // Check for duplicate keys and values
    dic_item di2;
    int kk = 0, kv = 0;
    forall_items(di2, pqset) {
      if (pqset.key(di2) == key)
	kk++;
      if (pqset.inf(di2) == val)
	kv++;
    }
    assert(kk == 1);
    assert(kv == 1);
  }

  // 3. Check _timeline
  pq_item pq;
  forall_items(pq, tl) {
    TimerHandler * pth = tl.inf( pq );
    KernelToken tok = pth->_token;
    di = pqset.lookup(tok);
    // The tokens must match
    assert( tok == pqset.key(di) );
    di = timers.lookup(tok);
    // The pointers must match
    assert( pth == timers.inf(di) );

    // Check for duplicates;
    pq_item pi; int kv = 0;
    forall_items(pi, tl) {
      if (tl.inf(pi) == pth)
	kv++;
    }
    assert(kv == 1);
  }

  // Check that the timeline is in order
  // dictionary<KernelTime, TimerHandler *> tmp_dict;
  KernelTime lastkt = 0.0;
  bool testTimerOrder = false;
  p_queue<KernelTime, TimerHandler *> tmp_tl;
  tmp_tl = tl;

  if ( tmp_tl.size() != tl.size() ) {
    cout << "tmp_tl = " << tmp_tl.size() << endl;
    cout << "tl = " << tl.size() << endl;
  }

  assert( tmp_tl.size() == tl.size() );
  while (!(tmp_tl.empty())) {
    // Find the minimum
    pq = tmp_tl.find_min();

    // Test the ordering
    if (testTimerOrder)
      assert( lastkt <= tmp_tl.prio(pq) );
    lastkt = tmp_tl.prio(pq);
    testTimerOrder = true;
    // Store it in the dictionary
    // tmp_dict.insert(tl.prio(pq), tl.inf(pq));
    // Remove it from the timeline
    tmp_tl.del_item(pq);
  }
  // Restore the timeline ...
  //  forall_items(di, tmp_dict) {
  //    tl.insert(tmp_dict.key(di), tmp_dict.inf(di));
  //  }

  if ( MEMORY_DEBUGGING & VERBOSE_DEBUG )
    cout << endl << "Exiting 'test_timeline'" << endl;
}

void test_kernel(bool print)
{ 
  if ( MEMORY_DEBUGGING & VERBOSE_DEBUG )
    print = true;
  
  if (print) {
    pq_item int_iter;
    cout << "Timers:" << endl;
    for (int_iter = theKernel()._timeline->first_item(); 
	 int_iter != 0; 
	 int_iter = theKernel()._timeline->next_item(int_iter)) {
      TimerHandler * timer = theKernel()._timeline->inf(int_iter);
      timer->Print(cout);
    }
  }
  
  // *******************************************************
  pq_item pq;
  if (pq = theKernel()._timeline->find_min()) { 
    TimerHandler * th = theKernel()._timeline->inf(pq);
    pq_item iter;
    int ct=0;
    for (iter = theKernel()._timeline->first_item(); 
	 iter != 0; 
	 iter = theKernel()._timeline->next_item(iter)) {
      ct++;
      TimerHandler * test = theKernel()._timeline->inf(iter);

      pq_item int_iter;
      int counter = 0;
      for (int_iter = theKernel()._timeline->first_item(); 
	   int_iter != 0; 
	   int_iter = theKernel()._timeline->next_item(int_iter)) {
	TimerHandler * timer = theKernel()._timeline->inf(int_iter);

	if (timer->_token == test->_token) {
	  if (++counter > 1) {
	    cout << "Timeline invariant violation 3 in Kernel "
		 << "(Duplicate Entries in PQ)"
		 << endl;
	    {
	      pq_item int_iter;
	      cout << "Timers:" << endl;
	      for (int_iter = theKernel()._timeline->first_item(); 
		   int_iter != 0; 
		   int_iter = theKernel()._timeline->next_item(int_iter)) {
		TimerHandler * timer = theKernel()._timeline->inf(int_iter);
		timer->Print(cout);
	      }
	    }
	    abort();
	  }
	}
      }
      
      if (test->GetExpirationTime() < (th->GetExpirationTime())) { 
	cout <<"Timeline invariant violation 1 in Kernel (Timers out of order)"
	     << endl;

	cout << "Timers:" << endl;
	for (int_iter = theKernel()._timeline->first_item(); 
	     int_iter != 0; 
	     int_iter = theKernel()._timeline->next_item(int_iter)) {
	  TimerHandler * timer = theKernel()._timeline->inf(int_iter);
	  timer->Print(cout);
	}
	abort();
      }
    }
    if (ct!=theKernel()._timeline->size()) { 
      cout << "Timeline invariant violation 2 in Kernel (Orphaned timers)"
	   << endl;
      abort();
    }
  }
  // *******************************************************
}
