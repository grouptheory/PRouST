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
#ifndef __EXPERIMENTALINVESTIGATOR_H__
#define __EXPERIMENTALINVESTIGATOR_H__

#ifndef LINT
static char const _ExperimentalInvestigator_h_rcsid_[] =
"$Id: ExperimentalInvestigator.h,v 1.8 1999/02/18 17:23:08 mountcas Exp $";
#endif

#include <fsm/netstats/Investigator.h>

class ExperimentalInvestigator : public Investigator {
public:

  ExperimentalInvestigator( const char * filename, double initialTime, 
			    void (*func_ptr)(void) = 0,
			    double finalTime = -1.0, int maxSetups = 1 );

protected:

  void makeObservation(char * const notebook_entry, bool & quit_now);
  virtual ~ExperimentalInvestigator();

private:

  double _lastTime;
  double _finalTime;
  int    _maxSetups;
  int    _seenSetups;
  bool   _seenGS;

  void (*_func_ptr)(void);
};

#endif
