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
#ifndef __KERNEL_H__
#define __KERNEL_H__

#ifndef LINT
static char const _Kernel_h_rcsid_[] =
"$Id: Kernel.h,v 1.1 1999/01/13 19:12:37 mountcas Exp $";
#endif

#include <FW/kernel/KernelToken.h>
#include <FW/kernel/KernelTime.h>
#include <FW/kernel/Handlers.h>
#include <DS/containers/dictionary.h>
#include <DS/containers/p_queue.h>
#include <DS/containers/list.h>

class TimerHandler;
class InputHandler;
class OutputHandler;
class SimEvent;
class Debugger;
class KernelToken;
class KernelTime;
class XXXTimer;
    
void DeleteFW(void);

void test_kernel(bool);
void test_timeline(void);

class Kernel {
  friend void test_kernel(bool);
  friend void test_timeline(void);
  friend bool init_ctors_FW(void);
  friend class Handler;
  friend class TimerHandler;
  friend class InputHandler;
  friend class OutputHandler;
  friend class XXXTimer;
  friend class SimEvent;
  friend class Debugger;
  friend class Conduit;
  friend void  DeleteFW(void);
  friend class asynchronous_manager;
public:

  enum Speed { QUARTER_SPEED   = -2, 
	       HALF_SPEED      = -1, 
	       REAL_TIME       = 0, 
	       DOUBLE_SPEED    = 1, 
	       QUADRUPLE_SPEED = 2, 
	       SIM_SPEED       = 666,
	       UNKNOWN_SPEED   = 999 };

  friend Kernel & theKernel(void);

  KernelTime CurrentTime(void);
  KernelTime CurrentElapsedTime(void);
  KernelTime CurrentSimTime(void);
  KernelTime Future(double seconds);

  void  Debug(void);
  void  Run(void);
  void  Stop(void);
  
  void  SetSpeed(Speed newspeed);
  Speed GetSpeed(void);

  Kernel(Speed sp = SIM_SPEED);
  ~Kernel();

  Debugger * KDB(void) const;

  void StopIn(double time = 0.0);
  void DisableXXX(void);

private:

  void Set_Lockstep_Mode_On(char* pipe);
  void Set_Lockstep_Mode_Off();

  // Do not define this method under penalty of death!
  //  The sneaky compiler attempts to define this method for us,
  //  and causes quite a few problems by stealing pointers.
  Kernel(const Kernel & rhs);

  void _Run(void);

  // methods to register a new handler
  bool Register(TimerHandler* t);
  bool Register(InputHandler* i);
  bool Register(OutputHandler* o);
  bool Register(SimEvent* e);

  // method which returns true if the SimEvent/Handler is currently registered.
  bool IsRegistered(KernelToken t);

  // method to deregister a new handler
  bool Deregister(KernelToken t);

  // method to set and get the current timescale
  // NOTE:  kernel time flows at a rate of _timescale units per real-world second
  //        or as fast as possible if _pure_simulation==true
  void   SetTimeScale(double timescale);
  double GetTimeScale(void);

  // the next token, to be used in registration
  KernelToken NextToken(void);

  // process the next event
  void ProcessNextEvent(void);

  // wait for input til the next timer expiry
  void WaitForInput(KernelTime t);

  // Appends the current time to the conduit record
  void AppendTimeToRecord(void);
  void AppendXXXToRecord(void);

  void CleanQueue(void);
  bool RemoveUnreg(void);

  void FreeConduits(void);
  void AddConduit(Conduit * c);
  void RemConduit(Conduit * c);

  // Prints out all the members of _timers, _pqset, and _timeline
  void dump_timers(void);  

  // data members
  p_queue<KernelTime, TimerHandler *>  * _timeline;  // priority queue of timers
  dictionary<KernelToken, pq_item>          _pqset;     // tokens -> pq_item

  dictionary<KernelToken, TimerHandler *>  _timers;    // registered timers
  dictionary<KernelToken, InputHandler *>  _inputs;    // registered input handlers
  dictionary<KernelToken, OutputHandler *> _outputs;   // registered output handlers
  dictionary<KernelToken, SimEvent *>      _events;    // registered events

  dictionary<int, InputHandler *>          _in_fds;    // fd -> InputHandler
  dictionary<int, OutputHandler *>         _out_fds;   // fd -> OutputHandler

  fd_set _in_fdset;   // current fdset of all inputhandler fds
  fd_set _out_fdset;  // current fdset of all outputhandler fds
  fd_set _save_in_fdset;
  fd_set _save_out_fdset;

  KernelTime  _boot_time;       // time of boot, or last timescale change
  KernelToken _next_token;      // next token for handler registration

  double      _timescale;       // current timescale
  double      _last_time;

  bool        _pure_simulation; // pure simulation mode? (as fast as possible)
  KernelTime  _sim_time;        // time (applies to pure simulation mode only)

  bool _running;    // kernel is running
  bool _debugging;
  bool _xxx;

  Debugger      * _debugger;

  list<Conduit *> _free_list;

  static Kernel * _kernel_singleton;

  //- - - - - - - - - - - - - - - - 
  void Lockstep_Notification(void);
  bool _in_lockstep;

  void  Block(void);
  void  Unblock(void);
  char  _blocked;

  friend class Kernel_Governor;
  friend void Visualize(char * pipename);
};


// -----------------------------------------------

class Kernel_Governor {
public:
  friend class Kernel;
private:

  friend void Block_for_NV(void);

  Kernel_Governor(void);
  ~Kernel_Governor();

  void Setup(char* pipe);

  void Wait_For_Input(void);
  bool Attempt_To_Acquire_Token(void);

  void Wait_Until_Token_Returns(void);
  void Relinquish_Token(void);

  void Process_Next_Command_Block(void);

  char _raw[2048];
  char _next_command_block[1024];
  int  _save_index;

  int    _fdin;
  fd_set _in_fdset;
  fd_set _ignore_fdset;

  static Kernel_Governor* _singleton;
};

#endif // __KERNEL_H__
