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

#ifndef __SSCFVISITORS_H__
#define __SSCFVISITORS_H__
#ifndef LINT
static char const _SSCFVisitor_h_rcsid_[] =
"$Id: SSCFVisitor.h,v 1.2 1998/04/23 20:05:07 mountcas Exp $";
#endif

#include <FW/basics/Visitor.h>
#include <FW/kernel/Kernel.h>
#include <FW/actors/State.h>

#define SSCF_LINKSTATUS_VISITOR_NAME "SSCF_linkstatus_Visitor"


class SSCFLinkStatusVisitor : public Visitor {
public:
  enum status {
    unknown = 0,
    up,
    down
  };
    
  SSCFLinkStatusVisitor(status st);  // need a deriving ctor TODO
  virtual ~SSCFLinkStatusVisitor();

  status get_status(void);
  int get_ttl(void);

private:
  status _st;
  int    _ttl;

  static vistype _my_type;
};
#endif
