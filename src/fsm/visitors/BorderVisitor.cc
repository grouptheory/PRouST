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
static char const _BorderVisitor_cc_rcsid_[] =
"$Id: BorderVisitor.cc,v 1.17 1998/12/15 14:08:45 mountcas Exp battou $";
#endif
#include <common/cprototypes.h>

#include <fsm/visitors/BorderVisitor.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/pnni_ig/nodal_hierarchy_list.h>

BorderUpVisitor::BorderUpVisitor(int port,
				 NodeID * local_node , u_int lpid,
				 NodeID * remote_node, u_int rpid,
				 NodeID * UpNodeID,
				 PeerID * cpgid,
				 ig_uplink_info_attr     *ulia,
				 ig_nodal_hierarchy_list *nhl,
				 u_int aggr) :
  LinkVisitor(_my_type, port, aggr, local_node, remote_node) ,
  _upnodeid(0), _CommonPGID(0), _ulia(0), _nhl(0) 
{ 
  if (UpNodeID)
    _upnodeid = UpNodeID->copy();
  if (cpgid)
    _CommonPGID = cpgid->copy();
  if(ulia)
    _ulia = (ig_uplink_info_attr *)ulia->copy();
  if(nhl)
    _nhl = (ig_nodal_hierarchy_list *)nhl->copy();
  SetLocalPort(lpid);
  SetRemotePort(rpid);
}

BorderUpVisitor::BorderUpVisitor(const BorderUpVisitor & rhs) :
  LinkVisitor(rhs), _upnodeid(0), _CommonPGID(0), _ulia(0), _nhl(0)
{
  if (rhs._upnodeid)
    _upnodeid = new NodeID(*(rhs._upnodeid));
  if (rhs._CommonPGID)
    _CommonPGID = new PeerID(*(rhs._CommonPGID));
  if (rhs._ulia)
    _ulia = (ig_uplink_info_attr *)rhs._ulia->copy();
  if (rhs._nhl)
    _nhl = (ig_nodal_hierarchy_list *)rhs._nhl->copy();
}

BorderUpVisitor::BorderUpVisitor(vistype & child_type, int port,
				 NodeID * local_node , u_int lpid,
				 NodeID * remote_node, u_int rpid,
				 NodeID * UpNodeID,
				 PeerID *cpgid  ,
				 ig_uplink_info_attr     *ulia,
				 ig_nodal_hierarchy_list *nhl,
				 u_int aggr):
  LinkVisitor(child_type.derived_from(_my_type),
	      port, aggr, local_node, remote_node), 
  _upnodeid(0), _CommonPGID(0), _ulia(ulia), _nhl(nhl)
{
  if (UpNodeID)
    _upnodeid = UpNodeID->copy();
  if (cpgid)
    _CommonPGID = cpgid->copy();

  SetLocalPort(lpid);
  SetRemotePort(rpid);
}

BorderUpVisitor::~BorderUpVisitor() 
{ 
  delete _upnodeid;
  delete _CommonPGID;
}

const vistype & BorderUpVisitor::GetClassType(void) const 
{
  return _my_type;
}

const VisitorType BorderUpVisitor::GetType(void) const 
{  return VisitorType(GetClassType()); }

const ig_uplink_info_attr * BorderUpVisitor::GetULIA(void) const
{ return _ulia; }

void  BorderUpVisitor::SetULIA(ig_uplink_info_attr * ulia) { _ulia = ulia; }

const ig_nodal_hierarchy_list * BorderUpVisitor::GetNHL(void) { return _nhl;}

void  BorderUpVisitor::SetNHL(ig_nodal_hierarchy_list *nhl) { _nhl = nhl;}

const PeerID * BorderUpVisitor::GetCPGID(void) const { return _CommonPGID; }

void  BorderUpVisitor::SetCPGID(PeerID * cpgid) { _CommonPGID = cpgid; }

const NodeID * BorderUpVisitor::GetUpNodeID(void) { return _upnodeid; }

Visitor * BorderUpVisitor::dup(void) const { return new BorderUpVisitor(*this); }

// --------------- BorderDownVisitor ------------------------
BorderDownVisitor::BorderDownVisitor(int port,
				     NodeID * local_node , u_int lpid,
				     NodeID * remote_node, u_int rpid,
				     PeerID *cpgid  ,
				     ig_uplink_info_attr     *ulia,
				     ig_nodal_hierarchy_list *nhl,
				     u_int aggr):
  LinkVisitor(_my_type, port, aggr, local_node, remote_node), 
              _CommonPGID(0), _ulia(ulia), _nhl(nhl)
{
  if (cpgid)
    _CommonPGID = cpgid->copy();

  SetLocalPort(lpid);
  SetRemotePort(rpid);
}

BorderDownVisitor::BorderDownVisitor(const BorderDownVisitor & rhs) :
  LinkVisitor(rhs), _CommonPGID(0), _ulia(0), _nhl(0)
{
  if (rhs._CommonPGID)
    _CommonPGID = new PeerID(*(rhs._CommonPGID));
  if (rhs._ulia)
    _ulia = (ig_uplink_info_attr *)rhs._ulia->copy();
  if (rhs._nhl)
    _nhl = (ig_nodal_hierarchy_list *)rhs._nhl->copy();
}

BorderDownVisitor::BorderDownVisitor(vistype & child_type, int port,
				     NodeID * local_node , u_int lpid,
				     NodeID * remote_node, u_int rpid,
				     PeerID *cpgid  ,
				     ig_uplink_info_attr     *ulia,
				     ig_nodal_hierarchy_list *nhl,
				     u_int aggr):
  LinkVisitor(child_type.derived_from(_my_type),port, aggr,
	      local_node, remote_node), _CommonPGID(0),
	      _ulia(ulia),_nhl(nhl)
{
  if (cpgid)
    _CommonPGID = cpgid->copy();

  SetLocalPort(lpid);
  SetRemotePort(rpid);
}

BorderDownVisitor::~BorderDownVisitor() 
{ 
  delete _CommonPGID;
}

const vistype & BorderDownVisitor::GetClassType(void) const 
{
  return _my_type;
}

const VisitorType BorderDownVisitor::GetType(void) const 
{  return VisitorType(GetClassType()); }

const ig_uplink_info_attr * BorderDownVisitor::GetULIA(void) const
{ return _ulia; }

void  BorderDownVisitor::SetULIA(ig_uplink_info_attr * ulia) { _ulia = ulia; }

const ig_nodal_hierarchy_list * BorderDownVisitor::GetNHL(void) { return _nhl;}

void  BorderDownVisitor::SetNHL(ig_nodal_hierarchy_list *nhl) { _nhl = nhl;}

const PeerID * BorderDownVisitor::GetCPGID(void) const { return _CommonPGID; }

void  BorderDownVisitor::SetCPGID(PeerID  *cpgid) { _CommonPGID = cpgid; }

Visitor * BorderDownVisitor::dup(void) const { return new BorderDownVisitor(*this); }
