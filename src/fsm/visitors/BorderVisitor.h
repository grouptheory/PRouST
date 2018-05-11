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
#ifndef __BORDERVISITOR_H__
#define __BORDERVISITOR_H__

#ifndef LINT
static char const _BorderVisitor_h_rcsid_[] =
"$Id: BorderVisitor.h,v 1.15 1998/08/11 18:29:58 mountcas Exp $";
#endif

#include <fsm/visitors/LinkVisitor.h>

class PeerID;
class ig_uplink_info_attr;
class ig_nodal_hierarchy_list;

#define BORDER_UP_VISITOR_NAME "BorderUpVisitor"
#define BORDER_DOWN_VISITOR_NAME "BorderDownVisitor"

class BorderUpVisitor : public LinkVisitor {
public:

  BorderUpVisitor(int port = 0,
		  NodeID * local_node  = 0, u_int lpid = 1,
		  NodeID * remote_node = 0, u_int rpid = 1,
		  NodeID * UpNodeID = 0,
		  PeerID *cpgid   = 0,
		  ig_uplink_info_attr     *_ulia = 0,
		  ig_nodal_hierarchy_list *_nhl = 0,
		  u_int aggr = 0);

  virtual const VisitorType GetType(void) const;

  const ig_uplink_info_attr * GetULIA(void) const;
  void  SetULIA(ig_uplink_info_attr * ulia);
  const ig_nodal_hierarchy_list *GetNHL(void);
  void  SetNHL(ig_nodal_hierarchy_list *nhl);
  const PeerID * GetCPGID(void) const;
  void  SetCPGID(PeerID  *cpgid);
  const NodeID * GetUpNodeID(void);

protected:

  virtual ~BorderUpVisitor();

  BorderUpVisitor(const BorderUpVisitor & rhs);

  BorderUpVisitor(vistype &child_type, int port = 0,
		  NodeID * local_node  = 0, u_int lpid = 1,
		  NodeID * remote_node = 0, u_int rpid = 1,
		  NodeID * UpNodeID = 0,
		  PeerID *cpgid   = 0,
		  ig_uplink_info_attr     *_ulia = 0,
		  ig_nodal_hierarchy_list *_nhl = 0,
		  u_int aggr = 0);

  virtual const vistype & GetClassType(void) const;
  virtual Visitor * dup(void) const;

  ig_uplink_info_attr     * _ulia;
  ig_nodal_hierarchy_list * _nhl;
  PeerID                  * _CommonPGID;
  NodeID                  * _upnodeid;

private:

  static vistype _my_type;
};



class BorderDownVisitor : public LinkVisitor {
public:

  BorderDownVisitor(int port = 0, 
		    NodeID * local_node  = 0, u_int lpid = 1,
		    NodeID * remote_node = 0, u_int rpid = 1,
		    PeerID *cpgid   = 0,
		    ig_uplink_info_attr     *_ulia = 0,
		    ig_nodal_hierarchy_list *_nhl = 0,
		    u_int aggr = 0);

  virtual const VisitorType GetType(void) const;

  // ULIA
  const ig_uplink_info_attr * GetULIA(void) const;
  void  SetULIA(ig_uplink_info_attr * ulia);
  // NHL
  const ig_nodal_hierarchy_list *GetNHL(void);
  void  SetNHL(ig_nodal_hierarchy_list *nhl);
  // CPGID
  const PeerID * GetCPGID(void) const;
  void  SetCPGID(PeerID  *cpgid);

protected:

  virtual ~BorderDownVisitor();

  BorderDownVisitor(const BorderDownVisitor & rhs);
  BorderDownVisitor(vistype &child_type, int port = 0,
		    NodeID * local_node  = 0, u_int lpid = 1,
		    NodeID * remote_node = 0, u_int rpid = 1,
		    PeerID *cpgid   = 0,
		    ig_uplink_info_attr     *_ulia = 0,
		    ig_nodal_hierarchy_list *_nhl = 0,
		    u_int aggr = 0);
  
  virtual const vistype & GetClassType(void) const;
  virtual Visitor * dup(void) const;
  
  ig_uplink_info_attr     * _ulia;
  ig_nodal_hierarchy_list * _nhl;
  PeerID                  * _CommonPGID;

private:

  static vistype _my_type;
};

#endif // __BORDERVISITOR_H__

