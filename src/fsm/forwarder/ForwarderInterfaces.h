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
#ifndef __FORWARDERINTERFACES_H__
#define __FORWARDERINTERFACES_H__

#ifndef LINT
static char const _ForwarderInterfaces_h_rcsid_[] =
"$Id: ForwarderInterfaces.h,v 1.6 1999/03/05 17:30:33 marsh Exp $";
#endif

#include <FW/interface/Interface.h>

typedef unsigned int u_int;

class Forwarder;

class ForwarderDefaultInterface : public fw_Interface {
public:

  ForwarderDefaultInterface(Forwarder * fw);
  virtual ~ForwarderDefaultInterface();

  int  ObtainNewVCI(int port, int request = -1);
  void ReturnVCI(int port, int vci);

  u_int ObtainCREF(void);
  void  ReturnCREF(u_int cref);

private:

  void ShareableDeath(void);

  Forwarder * _forwarder;
};

#endif
