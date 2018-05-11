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
#ifndef __CHULL_H__
#define __CHULL_H__
#ifndef LINT
static char const _cHull_h_rcsid_[] =
"$Id: cHull.h,v 1.2 1999/01/05 16:16:26 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
class connComp;

//---------------------------------------
class cHull {
public:
  cHull(connComp* c);
  ~cHull();

private:

  void computeHull(void);

  void pruneDuplicates(void);

  bool rightTurn(double x1, double y1,
		 double x2, double y2,
		 double x3, double y3);

  bool leftTurn(double x1, double y1,
		double x2, double y2,
		double x3, double y3);

  void appendSphere(double jx, double jy);

  list<double> _x;
  list<double> _y;

  list<double> _xupper;
  list<double> _yupper;

  list<double> _xlower;
  list<double> _ylower;
};

#endif // __CHULL_H__

