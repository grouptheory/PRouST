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
static char const _connComp_cc_rcsid_[] =
"$Id: connComp.cc,v 1.2 1999/01/05 16:16:17 mountcas Exp $";
#endif
#include "connComp.h"
#include "OVNode.h"
#include "cHull.h"

#include <FW/jiggle/Graph.h>
#include <FW/jiggle/CGwithRestarts.h>
#include <FW/jiggle/SteepestDescent.h>
#include <FW/jiggle/SpringChargeModel.h>
#include <FW/jiggle/Node.h>
#include <FW/jiggle/Edge.h>

extern "C" {
  #include <stdio.h>
  #include <assert.h>
};

#define  FORCE_ITERATIONS_PER_CALLBACK 5
#define  OPTIMAL_EDGE_LENGTH           25.0
#define  FORCE_THRESHOLD               0.5
#define  DISTANCE_THRESHOLD            1.0e-26
#define  CONFIDENCE_BARRIER            25
#define  LAYOUT_RADIUS                 8.0

//----------------------------------------
// Methods of class 'connComp'
// ---------------------------------------

connComp::connComp(void) {
  _chull = 0;
}

// ---------------------------------------
connComp::~connComp() {
  list_item li;
  _nodes.clear();
  _jigglenode2ovnode.clear();
  delete _chull;
  _chull = 0;
}

// ---------------------------------------
void connComp::insertEdge( int from, int to ) {
  _g.insertEdge(new Edge( _g.nodes(from), 
			  _g.nodes(to), 
			  false ));
}

// ---------------------------------------
int  connComp::insertNode( OVNode* n ) {
  int next_id = _g.numberOfNodes;
  char name[50];
  sprintf(name,"%d",next_id);

  Node* v = _g.insertNode( new Node(name) );
  _nodes.append(n);
  _jigglenode2ovnode.insert(next_id, n);
  
  v->x( n->getJiggleX() );
  v->y( n->getJiggleY() );

  return next_id;
}

// ---------------------------------------
void connComp::printGraph( void ) {
  cout << "-----connected-component-----\n";
  for (int i = 0; i < _g.numberOfNodes; i++) {
    dic_item di = _jigglenode2ovnode.lookup(i);
    assert(di);
    OVNode* ovn = _jigglenode2ovnode.inf(di);
    cout << ovn->getName() << endl;
  }
}

// ---------------------------------------
int  connComp::index( OVNode* seek ) {
  for (int i = 0; i < _g.numberOfNodes; i++) {
    dic_item di = _jigglenode2ovnode.lookup(i);
    assert(di);
    OVNode* ovn = _jigglenode2ovnode.inf(di);
    if (ovn==seek) {
      return i;
    }
  }
  abort();
}

// ---------------------------------------
bool connComp::layout( void )
{
  bool done = false;
  
  // commence jiggle layout procedures

  if ( _g.numberOfNodes > 1 ) {

    double optimalEdgeLen = OPTIMAL_EDGE_LENGTH;
    double initialStepSize = 0.1;
    int ctr=0;
    while ( ctr < CONFIDENCE_BARRIER ) {
      
      CGwithRestarts* opt = new CGwithRestarts(initialStepSize);
      SpringChargeModel* model=new SpringChargeModel(optimalEdgeLen, 
						     Model::QUADRATIC, 
						     Model::INVERSE, 
						     true);
      bool converged = false;
      while (!converged) {
	double energy = opt->improve( _g, *model );
	double distance = restore_nodes_intermediate( );
	if (distance < DISTANCE_THRESHOLD)
	  converged = true;
      };
      ctr++;
      
      delete opt;    // Forcible flush of state, as something is 
      delete model;  // buggy in CGwithRestarts and SpringChargeModel.  
    };
  }
  restore_nodes_final( );

  _chull = new cHull(this);

  return true;
}

//----------------------------------------
double connComp::restore_nodes_intermediate(void)
{
  double diff=0;

  // now that we have the min and max values rescale them
  for (int i = 0; i < _g.numberOfNodes; i++) {
    Node * v = _g.nodes(i);

    dic_item di = _jigglenode2ovnode.lookup(i);
    assert(di);
    OVNode* ovn = _jigglenode2ovnode.inf(di);
    
    double dx = (ovn->getJiggleX() - v->x());
    double dy = (ovn->getJiggleY() - v->y());
    diff += (dx*dx) + (dy*dy);

    ovn->setJiggleX( v->x() );
    ovn->setJiggleY( v->y() );
  }

  return diff;
}

//----------------------------------------
bool connComp::restore_nodes_final(void)
{

  // compute center of mass
  double Jig_accumX = 0.0;
  double Jig_accumY = 0.0;
  for (int i = 0; i < _g.numberOfNodes; i++) {
    Node * v = _g.nodes(i);
    Jig_accumX += v->x();
    Jig_accumY += v->y();
  }
  double Jig_aveX = Jig_accumX / (double)_g.numberOfNodes;
  double Jig_aveY = Jig_accumY / (double)_g.numberOfNodes;

  // compute radius of layout
  double rad2=0.0;
  for (int i = 0; i < _g.numberOfNodes; i++) {
    Node * v = _g.nodes(i);
    double dx = ( v->x() - Jig_aveX );
    double dy = ( v->y() - Jig_aveY );
    double r2 = dx*dx + dy*dy;
    if (r2>rad2) rad2=r2;
  }

  if (_g.numberOfNodes==1)
    rad2=1.0;

  rad2=sqrt(rad2);

  // rescale actual coordinates to within the
  // circle of radius = LAYOUT_RADIUS centered at the origin
  for (int i = 0; i < _g.numberOfNodes; i++) {
    dic_item di = _jigglenode2ovnode.lookup(i);
    assert(di);
    OVNode* ovn = _jigglenode2ovnode.inf(di);

    Node * v = _g.nodes(i);
    ovn->setX( LAYOUT_RADIUS * (v->x() - Jig_aveX) / rad2 );
    ovn->setZ( LAYOUT_RADIUS * (v->y() - Jig_aveY) / rad2 );
    ovn->setJiggleX( v->x() );
    ovn->setJiggleY( v->y() );
  }

  return true;
}
