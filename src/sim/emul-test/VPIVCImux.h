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

#ifndef LINT
static char const _VPIVCImux_h_rcsid_[] =
"$Id: VPIVCImux.h,v 1.4 1998/12/23 14:55:06 battou Exp $";
#endif
#include <FW/actors/Accessor.h>
#include <FW/behaviors/Mux.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/Visitor.h>
#include <DS/containers/dictionary.h>

class VPVCaccessor: public Accessor {
public:
  VPVCaccessor(void);
  virtual Conduit * GetNextConduit(Visitor * v);
  void AddBinding(u_int vpvc, Conduit *c);
protected:
  virtual ~VPVCaccessor();
  virtual bool Broadcast(Visitor * v);
  virtual bool Add(Conduit * c, Visitor * v);
  virtual bool Del(Conduit * c);
  virtual bool Del(Visitor * v);
  dictionary <u_int , Conduit *> _conduits;
};




