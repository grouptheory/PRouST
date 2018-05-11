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
#ifndef __OMNI_POC_H__
#define __OMNI_POC_H__

#ifndef LINT
static char const _OmniPOC_h_rcsid_[] =
"$Id: OmniPOC.h,v 1.4 1999/01/14 13:35:41 mountcas Exp $";
#endif

#include <FW/actors/State.h>

class Omni;
class NodeID;
class NPFloodVisitor;
class PTSPPkt;
class VisitorType;
class ig_ptse;
class ostream;

class OmniPOC : public State {
public:

  OmniPOC(const NodeID * me, bool logging = false);

  State * Handle(Visitor * v);
  void Interrupt(SimEvent * se);

  void SetLogging(bool l = false);

protected:

  virtual ~OmniPOC(); 

  void Process_NPFV(NPFloodVisitor* npv);
  void Process_PTSP(const PTSPPkt* ptsp, 
		    bool only_nullbodies,
		    bool made_by_me_at_some_level);
  void Process_PTSE(NodeID* Originator, 
		    ig_ptse * ptse,
		    bool made_by_me_at_some_level);

  void Process_My_Nodal_IG(NodeID* Originator, ig_ptse * ptse);

private:

  Omni    & _the_omni;
  NodeID  * _me;
  bool      _logging;

  static VisitorType * _npflood_type;
};

#endif
