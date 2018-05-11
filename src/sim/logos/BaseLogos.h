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
#ifndef __BASELOGOS_H__
#define __BASELOGOS_H__

#ifndef LINT
static char const _BaseLogos_h_rcsid_[] =
"$Id: BaseLogos.h,v 1.19 1999/01/07 20:03:44 marsh Exp $";
#endif

#define NEW_CODE

#include <DS/containers/list.h>
#include <DS/containers/graph.h>
#include <sim/logos/transit.h>

class Logos;
class LogosGraph;
class FastUNIVisitor;
class NodeID;
class attr_metrics;
class PNNI_crankback;
class ie_cause;

class transit {
public:

  transit(const NodeID * nid, int port);
  transit(Transit * Tr);
  ~transit();

  NodeID * TakeNID(void);
  NodeID * ShareNID(void);
  int GetPort(void);
  int GetLevel(void);

private:

  NodeID * _transit_nid;
  int      _port;
};

class BaseLogos {
public:

  BaseLogos(Logos * logos);

 virtual ~BaseLogos();
  // top level plugin. If not overloaded by a derived class
  //    it calls BuildGraph and RouteCompute each of which can be
  //    overloaded.
  // If ComputeRoute is overloaded then BuildGraph and RouteCompute
  //    are not called unless the overloaded function calls them.
  virtual bool ComputeRoute(FastUNIVisitor * fuv);
  // build a graph relevant to the current routing request.
  
  // the actually route computation
  virtual list<transit *> *RouteCompute(const NodeID * from,
					const NodeID * to,
					const NodeID *physical,
					attr_metrics *reqmet,
					int service_class,
					int &routing_cost,
					list<PNNI_crankback *> *cb,
					ie_cause * & cause,
					PNNI_crankback *  & crankback);

  virtual bool process_crankback (list<PNNI_crankback *> * cb, LogosGraph *LG);

#ifdef NEW_CODE
  virtual list<NodeID *> *border_destination(FastUNIVisitor * fuv,
					     const NodeID * & source,
					     int top_port,
					     const NodeID  * & destination,
					     const NodeID *physical_nodeid,
					     bool & immediate_return,
					     bool & immediate_return_val,
					     ie_cause  * & cause,
					     PNNI_crankback  * &crankback);
#endif					   
protected:

  const char *MyNickName();
  bool top_is_me(const NodeID *source,
		 FastUNIVisitor *fuv,
		 ie_cause * & cause,
		 PNNI_crankback * & crankback);

  node verify_from (LogosGraph * LG,
		    const NodeID *physical,
		    const NodeID *from,
		    ie_cause * & cause,
		    PNNI_crankback * & crankback);

  node verify_to (LogosGraph * LG,
		  const NodeID *destination,
		  const NodeID *from,
		  ie_cause * & cause,
		  PNNI_crankback * & crankback);

  void prune_edges(LogosGraph * LG,
		   attr_metrics * reqmet,
		   int class_index);

  void prune_nodes (LogosGraph *LG, node nfrom);

  list<transit *> *BaseLogos::GetRoute(LogosGraph *LG,
				       node nfrom,
				       node nto,
				       const NodeID *from,
				       int max_cost,
				       int &routing_cost);

private:
  
  Logos * _logos;
  char  * _nickname;
};

BaseLogos * AllocateLogos(const char *name, Logos *logos);
extern "C" BaseLogos * BaseLogosConstruct(Logos * logos);

#endif
