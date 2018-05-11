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
#ifndef __CREFEVENT_H__
#define __CREFEVENT_H__

#ifndef LINT
static char const _CREFEvent_h_rcsid_[] =
"";
#endif

#include <FW/kernel/SimEvent.h>
typedef unsigned int u_int;
                        // S e a n
#define CREF_EVENT_CODE 0x5365616E

class CREFEvent : public SimEvent {
public:

  CREFEvent(SimEntity * from, SimEntity * to, int CREF = 0);

  virtual ~CREFEvent();

  int  GetCREF(void) const;
  void SetCREF(int cref);

  int  GetKey(void) const;

private:

  int   _cref;
  u_int _key;

  static u_int _nextkey;
};

#endif
