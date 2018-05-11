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
#ifndef __DIAMETER_H__
#define __DIAMETER_H__

#ifndef LINT
static char const _costMatrix_h_rcsid_[] =
"$Id: costMatrix.h,v 1.1 1999/02/16 00:29:28 bilal Exp $";
#endif

#include <DS/containers/graph.h>
class LogosGraph;

// A cell loss ratio of 20 will be 1 cell every 10,000 years at OC48
#define HUGE_CLR 20

class costMatrix {
public:

  // NB: The LogosGraph 'g' may *not* change once
  // it is passed into the ctor of costMatrix, 
  // for the duration of the costMatrix's existence.
  // If 'g' does change, the costMatrix is invalid 
  // as an object and must be destroyed.  Calling methods
  // other than the destructor on a costMatrix with a
  // dirty LogosGraph results in unspecified behavior.
  costMatrix(LogosGraph& g);  

  ~costMatrix();
  
  // service_class_index ranges from 0-4 inclusive
  enum cost_service_class  {
    CBR      = 0,
    RTVBR    = 1,
    NRTVBR   = 2,   // These values match those in class ComplexRep
    ABR      = 3,
    UBR      = 4,
    GCAC_CLP = 5  //  GCAC_CLP is NOT SUPPORTED yet.
  };

  // raigfield ranges from 0-6 inclusive, with interpretation
  enum cost_metric_or_attribute {
    mcr   = 0,
    acr   = 1,
    ctd   = 2,
    cdv   = 3,
    adm   = 4,
    clr0  = 5,
    clr01 = 6
  };

  double computeDiameter(cost_service_class service_class_index, 
		 cost_metric_or_attribute raigfield);

  // average over all service_classes
  double computeDiameter(cost_metric_or_attribute raigfield); 

private:
  enum accum_op    { plus, max, min };
  enum compare_op  { smallest, largest };
  
  initMatrix(int numnodes);
  double computeDiameter( accum_op acc, compare_op comp );

  void setCost(int row, int col, double val);

  int relax(int row, int col);
  double pathplus(double path1, double path2);
  double pathmax(double path1, double path2);
  double pathmin(double path1, double path2);

  LogosGraph& _g;
  int _numnodes;

  node_array<int> * _nodekey;

  double ** _matrix;
  double ** _relaxed;
  double ** _temp;

  list<int> _border_index;
  accum_op   _acc;
  compare_op _comp;

  double _cost[5][7];
};

#endif // __DIAMETER_H__

