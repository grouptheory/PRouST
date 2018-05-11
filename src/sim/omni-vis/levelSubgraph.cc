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
static char const _levelSubgraph_cc_rcsid_[] =
"$Id: levelSubgraph.cc,v 1.6 1999/02/02 20:21:48 mountcas Exp $";
#endif

#include <FW/jiggle/Graph.h>
#include <FW/jiggle/Node.h>
#include <FW/jiggle/Edge.h>

#include <common/cprototypes.h>
#include "levelSubgraph.h"
#include "ov_data.h"
#include "OVNode.h"
#include "connComp.h"

//----------------------------------------
// Methods of class 'levelSubgraph'
// ---------------------------------------

levelSubgraph::levelSubgraph(ov_data* data, int level) : 
  _data(data), _level(level) {
    extractLevelSubgraph();
}

// ---------------------------------------
levelSubgraph::~levelSubgraph() {

  list_item li;
  forall_items( li, _comps ) {
    connComp* c = _comps.inf(li);
    delete c;
  }
  _comps.clear();

  _nodes.clear();
  _jigglenode2ovnode.clear();

  _data  =  0;
  _level = -1;
}

// ---------------------------------------
void levelSubgraph::extractLevelSubgraph(void) {
  int  i = 0;

  // ----- build the graph -----

  _jigglenode2ovnode.clear();

  const list< OVNode * > * all_nodes = _data->getNodes();
  list_item li;

  // ----- cull the nodes that are at the correct level

  forall_items(li, *all_nodes) {
    OVNode * nc = all_nodes->inf(li);
    int its_level = nc->getLevel();

    // we do not care about nodes on different levels
    if ( its_level != _level ) 
      continue;
    // otherwise, we are interested...

    _nodes.append(nc);

    Node * v = _levelg.insertNode(new Node(nc->getName()));
    nc->setRank( i );
    v->x( nc->getJiggleX() );
    v->y( nc->getJiggleY() );

    _jigglenode2ovnode.insert( i , nc );
    i++;
  }

  if (i < 2) {
    // degenerate case
    // we're done, as no edges will exist
  }
  else {
    // otherwise, we must build the induced subgraph

    // ----- insert the edges
    forall_items(li, _nodes) {
      OVNode * nc = _nodes.inf(li);
      int src_level = nc->getLevel();

      // we do not care about nodes on different levels
      if ( src_level != _level ) 
	continue;

      int from = nc->getRank(); 
      
      const list< OVNode * > * neighbors = nc->getEdges();
      list_item li2;
      forall_items(li2, *neighbors) {
	OVNode * neigh = neighbors->inf(li2);
	int dest_level = neigh->getLevel();

	// we do not care about nodes on different levels
	if ( dest_level != _level ) 
	  continue;
	
	int to = -1;
	if (_nodes.lookup(neigh)) {
	  to = neigh->getRank();
	}
      
	if (from >= 0 && to >= 0) {
	  _levelg.insertEdge(new Edge( _levelg.nodes(from), 
				       _levelg.nodes(to), 
				       false ));
	}
      }
    }
  }
  // DEBUGGING
  if ( _levelg.numberOfNodes > 0 ) {
    cout << "***** Level " << _level << " graph has " 
	 << _levelg.numberOfNodes << " nodes.\n";
  }
}

//----------------------------------------
void levelSubgraph::computeConnectedComponents(void) 
{
  // zero traversal bit
  for (int i = 0; i < _levelg.numberOfNodes; i++)
    _levelg.nodes(i)->flag = false;

  OVNode * uncovered = 0;

  while ( uncovered = uncoveredNode() ) {
    connComp* dfstree = new connComp;
    dfsTree (uncovered , dfstree);
    _comps.append(dfstree);
  }

  // DEBUGGING
  if ( _comps.size() > 0 ) {
    cout << "number of connected components at level " << _level 
	 << " is " << _comps.size() << endl;

    list_item li;
    forall_items( li, _comps ) {
      connComp * c = _comps.inf(li);
      c->printGraph();
    }
  }
}

//----------------------------------------
OVNode * levelSubgraph::uncoveredNode(void) 
{
  for (int i = 0; i < _levelg.numberOfNodes; i++) {
    Node * n = _levelg.nodes(i);
    if (n->flag == false) {
      dic_item di = _jigglenode2ovnode.lookup(i);
      assert(di);
      OVNode* ovn = _jigglenode2ovnode.inf(di);
      return ovn;
    }
  }
  return 0;
}

//----------------------------------------
int levelSubgraph::dfsTree(OVNode* uncovered, connComp* dfstree) 
{
  int    unc_index = uncovered->getRank();

  Node*  jnode     = _levelg.nodes(unc_index);
  jnode->flag      = true; 
  int from = dfstree->insertNode( uncovered );

  int degree       = jnode->neighbors.size();

  for (int i = 0; i < degree; i++) {
    Node * adj = (Node*)( jnode->neighbors[i] );
    OVNode * adj_ovn = jnode2ovnode(adj);
    int to;
    if ( ! adj->flag) {
      to = dfsTree( adj_ovn, dfstree );
    } else {
      to = dfstree->index( adj_ovn );
    }
    dfstree->insertEdge( from, to );
  }

  return from;
}

//----------------------------------------
Node * levelSubgraph::ovnode2jnode( OVNode* ovn ) 
{
  for (int i = 0; i < _levelg.numberOfNodes; i++) {
    dic_item di = _jigglenode2ovnode.lookup(i);
    assert(di);
    Node* n = _levelg.nodes(i);
    return n;
  }
  return 0;
}


//----------------------------------------
OVNode * levelSubgraph::jnode2ovnode( Node* n ) 
{
  for (int i = 0; i < _levelg.numberOfNodes; i++) {
    if (_levelg.nodes(i) == n) {
      dic_item di = _jigglenode2ovnode.lookup(i);
      assert(di);
      OVNode* ovn = _jigglenode2ovnode.inf(di);
      return ovn;
    }
  }
  return 0;
}

// ---------------------------------------
void levelSubgraph::layout( void )
{  
  list_item li;
  forall_items( li, _comps ) {
    connComp* c = _comps.inf(li);
    c->layout();
  }
}
