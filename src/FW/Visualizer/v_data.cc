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
static char const _v_data_cc_rcsid_[] =
"$Id: v_data.cc,v 1.1 1999/01/20 22:58:13 mountcas Exp $";
#endif

#include "v_app.h"
#include "v_data.h"
#include "v_main.h"
#include "v_conduit.h"

#include <common/cprototypes.h>
#include <FW/jiggle/CGwithRestarts.h>
#include <FW/jiggle/SteepestDescent.h>
#include <FW/jiggle/SpringChargeModel.h>
#include <FW/jiggle/Node.h>
#include <FW/jiggle/Edge.h>

#define FORCE_ITERATIONS_PER_CALLBACK 5
#define OPTIMAL_EDGE_LENGTH           100.0
#define FORCE_THRESHOLD               0.5

v_data::v_data(void) { }

v_data::~v_data() { }

void v_data::clear(void)
{
  // delete all nodes and edges stored in dictionaries
}

bool v_data::empty(void) const
{
  // return dictionary.size() == 0
  return true;
}

void v_data::save(const char * filename) const
{
  // save the layout
}

void v_data::layout(void)
{
  // from JIGGLE
  Graph g;
  // build the graph
  list_item li;
  forall_items(li, _nodes) {
    v_conduit * nc = _nodes.inf(li);
    Node * v = g.insertNode(new Node(nc->GetName()));
    v->x( nc->GetX() );
    v->y( nc->GetY() );
    if (nc->Touched()) // Allow it to participate but don't move it
      v->fixed = true;
  }
  // insert all of the edges
  forall_items(li, _nodes) {
    v_conduit * nc = _nodes.inf(li);

    int from = _nodes.rank(nc);
    const list<Adjacency *> * neighbors = nc->GetNeighbors();
    list_item li2;
    forall_items(li2, *neighbors) {
      v_conduit * neigh = neighbors->inf(li2)->GetNeighbor();

      int to = -1;
      if (_nodes.lookup(neigh))
	to = _nodes.rank(neigh);

      if (from >= 0 && to >= 0)
	g.insertEdge(new Edge( g.nodes(from), g.nodes(to), false ));
    }
  }

  // the graph has been complete built now ...
  double optimalEdgeLen = OPTIMAL_EDGE_LENGTH;
  double initialStepSize = 0.1;
  CGwithRestarts opt(initialStepSize);
  // SteepestDescent opt(initialStepSize);  // Uncomment to try SteepestDescent
  SpringChargeModel model(optimalEdgeLen, Model::QUADRATIC, Model::INVERSE, true);

  double d = -1.0, last_d = 0.0;
  bool   done = false;
  int    i = FORCE_ITERATIONS_PER_CALLBACK;
  while (i-- > 0) {
    d = opt.improve(g, model);

    if (fabs(d - last_d) < FORCE_THRESHOLD) {
      done = true;
      break;
    }
    
    if (ConduitRegistry()->GetFlags() & v_conduit_registry::ViewWhileContinue) {
      restore_nodes(g);
      // view the result ...
      theApplication().centralWidget()->repaint(false);
    }
    last_d = d;
  }

  restore_nodes(g);

  // if we aren't done reset the timer
  if (!done)
    theApplication().centralWidget()->jiggleTimer();

  // view the result ...
  theApplication().centralWidget()->repaint(false);
}

void v_data::restore_nodes(Graph & g)
{
  double Jig_minX = g.nodes(0)->x();
  double Jig_minY = g.nodes(0)->y();
  double Jig_maxX = g.nodes(0)->x();
  double Jig_maxY = g.nodes(0)->y();

  // propagate the changes back into our data structure
  for (int i = 0; i < g.numberOfNodes; i++) {
    Node * v = g.nodes(i);
    
    if (v->x() < Jig_minX)
      Jig_minX = v->x();
    if (v->y() < Jig_minY)
      Jig_minY = v->y();

    if (v->x() > Jig_maxX)
      Jig_maxX = v->x();
    if (v->y() > Jig_maxY)
      Jig_maxY = v->y();
  }

  // now that we have the min and max values rescale them
  for (int i = 0; i < g.numberOfNodes; i++) {
    Node * v = g.nodes(i);

    int x = (int)v->x(), y = (int)v->y();
    // comment out the below block to try w/o scaling
    double xdiff = theApplication().centralWidget()->canvasWidth();
    double ydiff = theApplication().centralWidget()->canvasHeight();

    assert(xdiff > 0 && ydiff > 0);

    x = (int)((v->x() - Jig_minX) * xdiff/(Jig_maxX - Jig_minX));
    y = (int)((v->y() - Jig_minY) * ydiff/(Jig_maxY - Jig_minY));
    // comment out the above block to try w/o scaling

    if (x > theApplication().centralWidget()->canvasWidth() - BORDER_WIDTH)
      x -= BORDER_WIDTH;
    else if (x < BORDER_WIDTH) x += BORDER_WIDTH;
    if (y > theApplication().centralWidget()->canvasHeight() - BORDER_WIDTH)
      y -= BORDER_WIDTH;
    else if (y < BORDER_WIDTH) y += BORDER_WIDTH;
    _nodes[i]->SetX(x);
    _nodes[i]->SetY(y);
    _nodes[i]->_secret_x = v->x();
    _nodes[i]->_secret_y = v->y();
  }
}

void v_data::notify(v_conduit * nc)
{
  _nodes.insert(nc);
}

void v_data::remove(v_conduit * nc)
{
  list_item li = _nodes.lookup(nc);
  if (li != 0)
    _nodes.del_item(li);
}

void v_data::notify(v_visitor * nv)
{
  _visitors.insert(nv);
}

void v_data::remove(v_visitor * nv)
{
  list_item li = _visitors.lookup(nv);
  if (li != 0)
    _visitors.del_item(li);
}

const list<v_conduit *> * v_data::getNodes(void) const
{
  return &_nodes;
}

const list<v_visitor *> * v_data::getVisitors(void) const
{
  return &_visitors;
}

v_conduit * v_data::nearest_node(QPoint & p, double & minDist)
{
  assert(minDist > 0);

  v_conduit * rval = 0;

  list_item li;
  forall_items(li, _nodes) {
    v_conduit * tmp = _nodes.inf(li);

    if (!tmp->IsVisible()) 
      continue;
    QPoint q(tmp->GetX(), tmp->GetY());
    double dx = p.x() - q.x(), dy = p.y() - q.y();
    double dist = sqrt(dx * dx + dy * dy);
    if ((dist >= 0) && (dist < minDist)) {
      rval = tmp;
      minDist = dist;
    }
  }

  return rval;
}
