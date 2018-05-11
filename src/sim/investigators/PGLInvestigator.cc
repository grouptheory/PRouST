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
static char const _PGLInvestigator_cc_rcsid_[] =
"$Id: PGLInvestigator.cc,v 1.1 1999/02/04 19:20:40 mountcas Exp $";
#endif

#include "PGLInvestigator.h"
#include <FW/basics/diag.h>
#include <FW/kernel/Kernel.h>
#include <codec/pnni_ig/id.h>
#include <fsm/omni/Omni.h>
#include <fsm/netstats/StatRecord.h>
#include <fstream.h>

PGLInvestigator::PGLInvestigator(const char * filename, int time, int triggerlevel) 
  : Investigator(filename, time), _lastTime(0.0), _triggerlevel(triggerlevel) { }

PGLInvestigator::~PGLInvestigator() { }

void PGLInvestigator::makeObservation(char * const notebook_entry, bool & quit_now)
{
  quit_now = false;
  double t = (double)(theKernel().CurrentElapsedTime()) - _lastTime + 1.0;

  list< const StatRecord * > * events =
    obtainRecentEvents( t, 1, "I_am_PGL", "*" );

  if ( events != 0 && events->empty() == false ) {
    const StatRecord * sr = events->head();

    NodeID tmp( sr->getMisc() );

    if ( tmp.GetLevel() <= _triggerlevel ) {
      char filename[64];
      sprintf( filename, "discord.%d.time=%lf.level=%d", getpid(), t, tmp.GetLevel() );
      ofstream ofs( filename );

      theOmni().setCertificationMode(true);
      int numnodes;
      theOmni().FullConvergenceTest( numnodes );
      ofs << theOmni().getCertificate();
      theOmni().setCertificationMode(false);

      sprintf( notebook_entry, "%s %lf : Leader elected at %s.\n"
	       "\tGlobal_Synchrony hasn't been reached at %lf, more info in '%s'.\n", 
	       sr->getType(), sr->getTime(), tmp.Print(), theKernel().CurrentElapsedTime(),
	       filename);
      diag( "investigator", DIAG_INFO, "%s", notebook_entry );
      quit_now = true;
    }
  }

  _lastTime = theKernel().CurrentElapsedTime();
}
