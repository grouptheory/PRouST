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
static char const _cHull_cc_rcsid_[] =
"$Id: cHull.cc,v 1.2 1999/01/05 16:16:16 mountcas Exp $";
#endif
#include "cHull.h"
#include "connComp.h"
#include "canvas3D.h"
#include "OVNode.h"

extern "C" {
  #include <math.h>
  #include <assert.h>
};

#define HULL_GRANULARITY    8

//------------------------------------------------

cHull::cHull(connComp* c) {
  const list< OVNode * > * nodes = c->getNodes();
  list_item li;
  forall_items(li, *nodes) {
    OVNode* ovn = nodes->inf(li);
    assert(ovn);
    double jx = ovn->getJiggleX();
    double jy = ovn->getJiggleY();
    appendSphere(jx,jy);
  }

  computeHull();
}

//------------------------------------------------
cHull::~cHull() {
}

//------------------------------------------------
void cHull::appendSphere(double jx, double jy) {
  double x,y;
  for (double theta = 0; theta<=2*PI; theta+=(2.0*PI/HULL_GRANULARITY)) {
    x = NODE_RADIUS * cos(theta);
    y = NODE_RADIUS * sin(theta);
    _x.append(x);
    _y.append(y);
  }
}

//------------------------------------------------
void cHull::computeHull(void) {
  pruneDuplicates();
}

//------------------------------------------------
void cHull::pruneDuplicates(void) {
  
  list<list_item> killx, killy;

  list_item lix, liy;
  lix = _x.first();
  liy = _y.first();
  while (lix && liy) {
    double x1 = _x.inf(lix);
    double y1 = _y.inf(liy);

    list_item chx, chy;
    chx = _x.first();
    chy = _y.first();

    bool begin=false;
    bool killed=false;
    while (chx && chy && (!killed)) {
      if (begin) {
	double x2 = _x.inf(chx);
	double y2 = _y.inf(chy);
      
	if ((x2==x1) && (y2==y1)) {
	  killx.append(lix);
	  killy.append(liy);
	  killed=true;
	}
      }

      if ((chx==lix) || (chy==liy)) {
	begin=true;
      }

      chx = _x.next(chx);
      chy = _y.next(chy);
    };

    lix = _x.next(lix);
    liy = _y.next(liy);
  };

  list_item kx;
  forall_items( kx, killx ) {
    list_item victimx = killx.inf( kx );
    _x.del_item( victimx );
  }

  list_item ky;
  forall_items( ky, killy ) {
    list_item victimy = killy.inf( ky );
    _y.del_item( victimy );
  }
}

//------------------------------------------------
bool cHull::rightTurn(double x1, double y1,
		      double x2, double y2,
		      double x3, double y3) {
  bool rht = false;
  if ( x1 != x2 ) {
    double m = (y2-y1) / (x2-x1);
    double b = y1 - (x1 * m);
    
    double yproj = (m * x3) + b;
    int side;
    if (x1 < x2) { side = -1; }
    else         { side = +1; }

    if (( ((y3 - yproj) >= 0) && (side == +1) ) ||
	( ((y3 - yproj) <= 0) && (side == -1) ))    {
      rht = true; 
    }
  }
  else {
    assert(y1!=y2);

    if (y1 < y2) {
      if (x3 > x1) rht = true;
    }
    else {
      if (x3 < x1) rht = true;
    }
  }

  return rht;
}

//------------------------------------------------
bool cHull::leftTurn(double x1, double y1,
		     double x2, double y2,
		     double x3, double y3) {
  bool rht = false;
  if ( x1 != x2 ) {
    double m = (y2-y1) / (x2-x1);
    double b = y1 - (x1 * m);
    
    double yproj = (m * x3) + b;
    int side;
    if (x1 < x2) { side = +1; }
    else         { side = -1; }

    if (( ((y3 - yproj) >= 0) && (side == +1) ) ||
	( ((y3 - yproj) <= 0) && (side == -1) ))    {
      rht = true; 
    }
  }
  else {
    assert(y1!=y2);

    if (y1 < y2) {
      if (x3 < x1) rht = true;
    }
    else {
      if (x3 > x1) rht = true;
    }
  }

  return rht;
}
