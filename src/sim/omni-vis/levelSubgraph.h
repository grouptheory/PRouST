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
#ifndef __LEVELSUBGRAPH_H__
#define __LEVELSUBGRAPH_H__
#ifndef LINT
static char const _levelsubgraph_h_rcsid_[] =
"$Id: levelSubgraph.h,v 1.1 1999/01/04 16:23:30 mountcas Exp $";
#endif

#include <FW/jiggle/Graph.h>
#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
class OVNode;
class ov_data;
class connComp;

//----------------------------------------------
class levelSubgraph {
public:
  levelSubgraph(ov_data* data, int level);
  ~levelSubgraph();

  void computeConnectedComponents(void);
  void layout(void);

private:
  void extractLevelSubgraph(void);

  OVNode* uncoveredNode(void);
  int     dfsTree(OVNode* uncovered, connComp* dfstree);

  Node*   ovnode2jnode( OVNode* ovn );
  OVNode* jnode2ovnode( Node* n );

  ov_data                       * _data;
  int                             _level;

  // the Rank of each OVNode is its index in _levelg

  Graph                           _levelg;
  list< OVNode * >                _nodes;

  // this dictionary is from index of the node in _levelg,
  // to the corresponding OVNode*  
  dictionary< int, OVNode* >      _jigglenode2ovnode;  
  
  list<connComp*>                 _comps;
};

#endif // __LEVELSUBGRAPH_H__

