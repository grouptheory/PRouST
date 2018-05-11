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
static char const _SimEntity_cc_rcsid_[] =
"$Id: SimEntity.cc,v 1.1 1999/01/13 19:12:37 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/kernel/SimEntity.h>
#include <FW/kernel/SimEvent.h>
#include <FW/kernel/Handlers.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/KDB.h>

SimEntity::SimEntity(void)
  : _type(SimEntity::UNKNOWN), _kdb_simentity_id(-1), _register(true)
{ 
  Kernel & kernel = theKernel();
  Debugger * kdb = kernel.KDB();
  _kdb_simentity_id = kdb->Report(this);
}

// This defaults to true, and should be used for Creator, State, Terminal.
//   It should be passed in false ONLY for the Debugger.
SimEntity::SimEntity(SimEntity::se_types type, bool registerme) 
  : _type(type), _kdb_simentity_id(-1),  _register(registerme)
{ 
  if (_register) {
    Kernel & kernel = theKernel();
    Debugger * kdb = kernel.KDB();
    _kdb_simentity_id = kdb->Report(this);
  }
}

SimEntity::~SimEntity() 
{ 
  if (!_timers.empty()) {
    list_item li;
    forall_items(li, _timers) {
      TimedDeliveryHandler * tdh = _timers.inf(li);
      if (tdh) delete tdh;
    }
  }
  _timers.clear();

  if (_register) {
    if (theKernel().KDB()->Resign(_kdb_simentity_id, this) == 0) {
      BPT * b = new BPT(this,BPT::SimEntityDestruction);
      int is_breakpoint = (theKernel().KDB()->Notify(b));
      delete b;
      if (is_breakpoint) {
	ALERT_DEBUGGER;
      }
    }
  }
}

bool SimEntity::_Deliver(SimEvent* e)
{  return e->Register();  }

bool SimEntity::Register(TimerHandler* t)
{  return t->_Register();  }

bool SimEntity::Register(InputHandler* i)
{  return i->_Register();  }

bool SimEntity::Register(OutputHandler* o)
{  return o->_Register();  }

bool SimEntity::Cancel(SimEvent* e)
{  return e->Deregister();  }

bool SimEntity::Cancel(TimerHandler* t)
{  return t->Deregister();  }

bool SimEntity::Cancel(InputHandler* i)
{  return i->Deregister();  }

bool SimEntity::Cancel(OutputHandler* o)
{  return o->Deregister();  }

void SimEntity::UnhookMe(TimedDeliveryHandler * tdh)
{
  list_item li;
  if (li = _timers.search(tdh))
    _timers.del_item(li);
}

void SimEntity::Deliver(SimEvent * e, double time)
{
  if (time <= 0.0) {
    time = 0.0;
  }

  TimedDeliveryHandler * tdh = new TimedDeliveryHandler(this, e, time);
  // Insert it into the list.
  _timers.append(tdh);
  // Register it with the kernel.
  Register(tdh);
}

void SimEntity::ReturnToSender(SimEvent *e, double time)
{
  SimEntity * f = e->_from;
  e->_from = e->_to;
  e->_to = f;

  Deliver(e, time);
}

void SimEntity::ReturnToSenderSynchronous(SimEvent * e)
{
  SimEntity * f = e->_from;
  e->_from = e->_to;
  e->_to = f;

  _Deliver(e);
}

void SimEntity::DeliverSynchronous(SimEvent * e)
{  _Deliver(e);  }

SimEntity::se_types SimEntity::GetType(void) const
{  return _type;  }
