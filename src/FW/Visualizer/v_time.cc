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
static char const _v_time_cc_rcsid_[] =
"$Id: v_time.cc,v 1.1 1999/01/20 22:58:13 mountcas Exp $";
#endif

#include "v_time.h"
#include "v_main.h"
#include "v_app.h"

#include <qfont.h>

#include <common/cprototypes.h>

v_time::v_time(v_main * p, v_data * d) 
  : QLCDNumber(p), _parent(p), _data(d), 
    _time(0) 
{ 
  setFrameStyle( Box | Sunken );
  setSmallDecimalPoint( true );
  setNumDigits( 6 );
  setMode( QLCDNumber::DEC );
  setSegmentStyle( QLCDNumber::Filled );
}

v_time::~v_time() { }

void v_time::setTime(double t)
{
  _time = t;
  theApplication().updateSlider(t);
  display( _time );
}
