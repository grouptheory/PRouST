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
static char const _costMatrix_cc_rcsid_[] =
"$Id: costMatrix.cc,v 1.2 1999/02/19 22:55:40 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include "costMatrix.h"
#include <sim/logos/LogosGraph.h>

extern "C" {
  #include <math.h>
};

//-------------------------------------------------------------
costMatrix::costMatrix(LogosGraph& g) : _g(g) {

  // allocate memory
  initMatrix( g._graph.number_of_nodes() );

  // get row/col index numbers into adjacency matrix
  list<node> all_nodes = g._graph.all_nodes();
  _nodekey = new node_array<int> (g._graph);
  node n;
  forall_nodes(n, g._graph) {
    int rank = all_nodes.rank(n);
    (*_nodekey) [n] = rank;
    // insert rank of border nodes into _border_index
    logos_node * ln = g._graph[n];
    if ( ln->border_node() ) _border_index.append(rank);
  }

  // initialize the memo-ization table
  for (int service_class=0; service_class < 5; service_class++) {
    for (int raigfield = 0; raigfield < 7; raigfield++) {
      _cost[service_class][raigfield] = -1.0;
    }
  }
}

//-------------------------------------------------------------
double costMatrix::computeDiameter(cost_metric_or_attribute raigfield) {
  double val = 0;
  for (int service_class=0; service_class < 5; service_class++) {
      switch( raigfield ) {
      case mcr: /* mcr_field */ 
      case acr: /* acr_field */ 
      case ctd: /* ctd_field */ 
      case cdv: /* cdv_field */ 
      case adm: /* adm_field */ 
	val += computeDiameter( (cost_service_class) service_class, 
			    (cost_metric_or_attribute) raigfield );
	break;

      case clr0: /* clr0_field */
      case clr01: /* clr01_field */
	{
	  // limit clr fileds to HUGE_CLR - from costMatrix.h
	  double the_val =
	    computeDiameter( (cost_service_class) service_class, 
			     (cost_metric_or_attribute) raigfield );
	  if ( HUGE_CLR < (int)the_val )
	    the_val = (double)HUGE_CLR;
	  val += the_val;
	}

	break;
      default: abort();
	break;
      }
  }
  val /= 5.0;
  return val;
}

//-------------------------------------------------------------
double costMatrix::computeDiameter(cost_service_class service_class_index, 
				   cost_metric_or_attribute raigfield) {

  if (_cost[service_class_index][raigfield] >= 0.0) // lookup any
    return _cost[service_class_index][raigfield];   // memo-ized answers

  // otherwise, we have work to do...

  // initialize base costs
  for (int row = 0; row < _numnodes; row++) {
    for (int col = 0; col < _numnodes; col++) {
      switch( raigfield ) {
      case 0: /* mcr_field */ 
      case 1: /* acr_field */ 
      case 5: /* clr0_field */
      case 6: /* clr01_field */
	_matrix [row] [col] = (double)0.0;
	break;
      case 2: /* ctd_field */ 
      case 3: /* cdv_field */ 
      case 4: /* adm_field */ 
	_matrix [row] [col] = (double)HUGE_VAL;
	break;
      default: abort();
	break;
      }
    }
  }

  // insert the graph edge costs into the matrix
  edge e;
  forall_edges(e, _g._graph) {
    int startnode = (*_nodekey)[ _g._graph.source(e) ];
    int endnode = (*_nodekey)[ _g._graph.target(e) ];
    
    logos_edge lge = _g._graph[e];
    attr_metrics * metrics = lge.GetMetric(service_class_index);
    
    switch( raigfield ) {
    case 0: /* mcr_field */ 
      _matrix [startnode] [endnode] = metrics->_mcr;
      break;
    case 1: /* acr_field */ 
      _matrix [startnode] [endnode] = metrics->_acr;
      break;
    case 2: /* ctd_field */ 
      _matrix [startnode] [endnode] = metrics->_ctd;
      break;
    case 3: /* cdv_field */ 
      _matrix [startnode] [endnode] = metrics->_cdv;
      break;
    case 4: /* adm_field */ 
      _matrix [startnode] [endnode] = metrics->_adm;
      break;
    case 5: /* clr0_field */
      _matrix [startnode] [endnode] = metrics->_clr_0;
      break; 
    case 6: /* clr01_field */
      _matrix [startnode] [endnode] = metrics->_clr_01;
      break;
    default: abort();
      break;
    }
  }
  
  // begin relaxation procedures
  double val;
  switch( raigfield ) {
  case 0: /* mcr_field */ 
  case 1: /* acr_field */ 
    val = computeDiameter(costMatrix::min,       // metric under min
			  costMatrix::largest);  // find the fattest paths
    // and of these, return the thinnest one

     break;
  case 5: /* clr0_field */
  case 6: /* clr01_field */    
    val = computeDiameter(costMatrix::min,       // metric under min
			  costMatrix::largest);  // find the fattest paths
    // and of these, return the thinnest one
    // but apply some sanity
    if (val < 0 || val >HUGE_CLR)
      val = HUGE_CLR;

     break;
  case 2: /* ctd_field */ 
  case 3: /* cdv_field */ 
  case 4: /* adm_field */ 
    val = computeDiameter(costMatrix::plus,      // metrics add
			  costMatrix::smallest); // compute all shortest paths
    // and of these return the longest one

    break;
  default:
    break;
  }
  _cost[service_class_index][raigfield] = val;
  return val;
}

//-------------------------------------------------------------
costMatrix::initMatrix(int numnodes) {
  _numnodes = numnodes;
  _matrix = new double* [numnodes];
  for (int i=0;i<numnodes;i++)
    _matrix[i]=new double [numnodes];

  _relaxed = new double* [numnodes];
  for (int i=0;i<numnodes;i++)
    _relaxed[i]=new double [numnodes];

  _temp = new double* [numnodes];
  for (int i=0;i<numnodes;i++)
    _temp[i]=new double [numnodes];
}

//-------------------------------------------------------------
costMatrix::~costMatrix() {
  for (int i=0;i < _numnodes;i++)
    delete [] _matrix[i];
  delete [] _matrix;
  _matrix = 0;

  for (int i=0;i < _numnodes;i++)
    delete [] _relaxed[i];
  delete [] _relaxed;
  _relaxed = 0;


  for (int i=0;i < _numnodes;i++)
    delete [] _temp[i];
  delete [] _temp;
  _temp = 0;

  delete _nodekey;
}

//-------------------------------------------------------------
double costMatrix::computeDiameter( accum_op acc, compare_op comp ) {
  _comp = comp;
  _acc  = acc;
  int changed;

  // copy the costs into the relaxation matrix
  for (int row = 0; row < _numnodes; row++) {
    for (int col = 0; col < _numnodes; col++) {
      _relaxed [row] [col] = _matrix [row] [col];
    }
  }

  // begin relaxation, but be wary for
  // degenerate cases of negative weight cycles
  // and non connected graphs by keeping a watch
  // on the number of relaxation phases
  int ctr = 0;
  do {
    changed = 0;
    for (int row = 0; row < _numnodes; row++) {
      for (int col = 0; col < _numnodes; col++) {
	changed += relax ( row , col );
      }
    }

    if (changed) {
      ctr++;

      // copy the relaxed costs from the temp matrix,
      // for further relaxation...
      // if nothing changed these are already identical
      for (int row = 0; row < _numnodes; row++) {
	for (int col = 0; col < _numnodes; col++) {
	  _relaxed [row] [col] = _temp [row] [col];
	}
      }
    }
  }
  while ((changed > 0) && (ctr <= _numnodes));
	  
  double bestval;
  if (ctr > _numnodes) {   // we are in a degenerate case, i.e.
    switch (comp) {
      // we want the biggest entry in the matrix
    case costMatrix::smallest:     // but costs can be made 
      bestval = (double)HUGE_VAL;  // arbitrarily big!
      break;
      // we want the smallest entry in the matrix
    case costMatrix::largest:      // but costs can be made 
      bestval = (double)0.0;       // arbitrarily small!
      break;
    default: abort();
      break;
    }
  }
  else {                  // we are in a standard scenario
    switch (comp) {
      // we want the biggest entry in the matrix
    case costMatrix::smallest: 
      bestval = (double)0.0;      // we will find maximal entry
      break;
      // we want the smallest entry in the matrix
    case costMatrix::largest:  
      bestval = (double)HUGE_VAL; // we will find minimal entry
      break;
    default:  abort();
      break;
    }

    // commence search
//    for (int row = 0; row < _numnodes; row++) {
//      for (int col = 0; col < _numnodes; col++) {

    // only look at cost between border nodes
    list_item row_item, col_item;
    int row, col;
    forall_items (row_item, _border_index) {
      row = _border_index.inf(row_item);

      forall_items (col_item, _border_index) {
	col = _border_index.inf(col_item);
    
	if (row==col)
	  continue;

	switch (comp) {
      // we want the biggest entry in the matrix
	case costMatrix::smallest: 
	  if ( _relaxed [row] [col] > bestval )
	    bestval = _relaxed [row] [col];
	  break;
      // we want the smallest entry in the matrix
	case costMatrix::largest:  
	  if ( _relaxed [row] [col] < bestval )
	    bestval = _relaxed [row] [col];
	  break;
	default: abort();
	  break;
	}
      }
    }
  }

  // return the value
  return bestval;
}

//-------------------------------------------------------------
int costMatrix::relax(int row, int col) {

  // ignore the diagonals
  if (row == col) {
    _temp [row] [col] = _relaxed [row] [col];
    return 0;
  }
  // we can compute anything we want, 
  // all we've gotta do is *relax*

  int changes = 0;
  double existing = _relaxed [row] [col];
  for (int mid = 0; mid < _numnodes; mid++) {
    
    if ((row==mid) ||
	(col==mid))
      continue;

    double path1 = _relaxed [row] [mid];
    double path2 = _relaxed [mid] [col];
    double candidate;
    switch (_acc) {
    case costMatrix::plus:
      candidate = pathplus(path1,path2);
      break;
    case costMatrix::max:
      candidate = pathmax(path1,path2);
      break;
    case costMatrix::min:
      candidate = pathmin(path1,path2);
      break;
    default: abort();
      break;
    }

    _temp [row] [col] = existing;

    switch (_comp) {
    case costMatrix::smallest: 
      if ( candidate < existing ) {
	_temp [row] [col] = candidate;
	existing = candidate;
	changes = 1;
      }
      break;
    case costMatrix::largest:  
      if ( candidate > existing ) {
	_temp [row] [col] = candidate;
	existing = candidate;
	changes = 1;
      }
      break;
    default: abort(); 
      break;
    }
  }

  return changes;
}
  
//-------------------------------------------------------------
double costMatrix::pathplus(double path1, double path2) {
  return path1+path2;
}
 //-------------------------------------------------------------
double costMatrix::pathmax(double path1, double path2) {
  return ((path1>path2)? path1 : path2);
}

//-------------------------------------------------------------
double costMatrix::pathmin(double path1, double path2) {
  return ((path1<path2)? path1 : path2);
}

//-------------------------------------------------------------
void costMatrix::setCost(int row, int col, double val) {
  _matrix [row] [col] = val;
}
