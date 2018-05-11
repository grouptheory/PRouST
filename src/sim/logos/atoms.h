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

// -*-C++-*-
#ifndef __ATOMS_H__
#define __ATOMS_H__
#ifndef LINT
static char const _atoms_h_rcsid_[] =
"$Id: atoms.h,v 1.22 1999/01/07 23:04:06 marsh Exp $";
#endif

#include <iostream.h>
//#include <LEDA/graph.h>

class ig_nodal_info_group;
class ig_horizontal_links;
class ig_resrc_avail_info;
class ig_uplinks;
class ig_ptse;
class Addr;
class NodeID;
class Logos;
class Transit;
class node_struct;
int service_class_index (int service_class);

class logos_node {
  friend int operator == (const logos_node & lhs, const logos_node & rhs);
  friend ostream & operator << (ostream & os, const logos_node & rhs);
  friend ostream & operator << (ostream & os, const logos_node * rhs);
  friend istream & operator >> (istream & os, logos_node & rhs);
  friend int compare (logos_node  * const &lhs, logos_node * const &rhs);
public:

  logos_node(ig_nodal_info_group * ninf);
  logos_node(const NodeID * nid = 0, int logical_port=0);
  logos_node(Transit *tr);
  ~logos_node();
  logos_node * copy();
  
  const NodeID * ShareNID(void) const;
  const NodeID * ShareTransitNID(void) const;
  Addr   * GetAddr(void) const;
  Transit *ShareTransit();
  Transit *TakeTransit();
  int  NodalFlag();
  void NodalFlag(const int nodal_flag);
  int  LogicalPort();
  void LogicalPort(int portid);
  bool border_node();
  void border_node(bool true_or_false);

  void Node(node_struct * n);
  node_struct * Node(void);
  

private:

  NodeID  *_node_id;
  Transit *_transit;
  Addr    *_addr;
  int    _nodal_flag;
  int    _logical_port;
  bool   _border_node;   // used by the aggregator - set by Logos->GetGraph
  node_struct * _node;
};

class attr_metrics {
public:

  attr_metrics(int mcr = 0, int acr = 0, int ctd = MAXINT,
	       int cdv = MAXINT, short int clr0 = 0, short int clr01 = 0,
	       int adm = 5400);
  attr_metrics(const attr_metrics & rhs);
  attr_metrics(const ig_resrc_avail_info *raig);
  ~attr_metrics( );

  attr_metrics & operator = (const attr_metrics & rhs);
  friend ostream & operator << (ostream & os, const attr_metrics & rhs);
  int          _mcr;
  int          _acr;
  int          _ctd;
  int          _cdv;
  int          _adm;
  short int    _clr_0;
  short int    _clr_01;
};

class logos_edge {
  friend logos_edge * process(Logos *gcac, const NodeID * nid,
			      ig_horizontal_links * horiz);
  friend logos_edge * processfwd(Logos *gcac, const NodeID * nid,
				 ig_uplinks * upl);
  friend logos_edge * processbwd(Logos *gcac, const NodeID * nid,
				 ig_uplinks * upl);
  friend ostream & operator << (ostream & os, const logos_edge & rhs);
  friend ostream & operator << (ostream & os, const logos_edge * rhs);
  friend istream & operator >> (istream & os, logos_edge & rhs);
  friend int compare (logos_edge * const &e1, logos_edge * const &e2);
public:
  
  // logos_edge(logos_node * start = 0, logos_node * end = 0);
  logos_edge(logos_node * start = 0, logos_node * end = 0,
	     attr_metrics * met = 0, int port = -1, int remote_port = 0);

  logos_edge(logos_edge *le);
  ~logos_edge();

  void SetMetric(attr_metrics & metric, int raig_flag); // raig flag 0x8f00
  attr_metrics *GetMetric(int service_class);
  attr_metrics *GetMetric(void);
  void Setservice_class_index(int service_class_index);
  int  Getservice_class_index(void);
  void Setroute_state(bool new_state);
  bool Getroute_state(void) const;
  int  Getport(void) const;
  void Setport(int port);
  int  GetRemotePort();
  void SetRemotePort(int remote_port);
  logos_node * GetStart(void) const;
  void         SetStart(logos_node *start);
  logos_node * GetEnd(void) const;
  void         SetEnd(logos_node *end);
  void         AggTok(const int aggtoken);
  const int    GetAggTok(void) const;
  void set_exceptional(void);
  bool is_exceptional(void);

  // get/set _reverse edge
  logos_edge * Reverse();
  bool Reverse(logos_edge * candidate);
  
private:


  logos_node * _start;
  logos_node * _end;
  logos_edge * _reverse;            // matching backward link
  
  attr_metrics _metrics[5];
  int  _port;                    // local port on _start, -1 = unknown
  int  _remote_port;            // port on _end, -1 = unknown
  int  _service_class_index;   // _metrics[] to use in routing decisions
  bool _route_state;          // exclude from routing if false
                             // set true for all edges at start of findroutes
  int  _aggregation_token;  // used as port for border links
  bool _bypass_exception;  // used to determine if port of remote peer is used
};

#endif // __ATOMS_H__
