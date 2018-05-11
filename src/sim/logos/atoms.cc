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
static char const _atoms_cc_rcsid_[] =
"$Id: atoms.cc,v 1.44 1999/01/07 23:04:06 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include <sim/logos/atoms.h>
#include <sim/logos/Logos.h>
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/addr.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/pnni_ig/next_hi_level_binding_info.h>

//---------- logos_node methods ------------------------------
int operator == (const logos_node & lhs, const logos_node & rhs)
{
  return ((*(lhs._node_id) == *(rhs._node_id)) &&
	  (lhs._logical_port == rhs._logical_port));
}

ostream & operator << (ostream & os, const logos_node & rhs)
{
    if (rhs._addr)    os << *(rhs._addr)    << " " ;
    if (rhs._node_id) {
      os << *(rhs.ShareTransitNID());
      os << ":" << rhs._logical_port;
    }
    if (rhs._border_node) os << " border node";
    return os;
}

ostream & operator << (ostream & os, const logos_node * rhs)
{
    if (rhs->_addr)         os << *(rhs->_addr)    << " " ;
    if (rhs->_node_id) {
      os << *(rhs->ShareTransitNID());
      os << ":" << rhs->_logical_port;
    }
    if (rhs->_border_node)  os << " border node";
    return os;
}

istream & operator >> (istream & is, logos_node & rhs)
{
    cerr << "operator >> not implemented for class logos_node " << endl;
    is.clear(ios::failbit);
    return is;
}


logos_node::logos_node(ig_nodal_info_group * ninf)
    : _node_id(0), _addr(0), _nodal_flag(0), _border_node(false), _node(0)
{
  _addr = new NSAP_DCC_ICD_addr((NSAP_DCC_ICD_addr *)ninf->GetATMAddress());
// _node_id = new NodeID(*(ninf->GetNextHigherLevel()->GetParentLogGroupID()));
  
  ig_next_hi_level_binding_info * up =
      (ig_next_hi_level_binding_info *) ninf->GetNextHigherLevel();
  
  if (up) {
    _node_id = new NodeID(*(up->GetParentLogGroupID()));
  } else {
    u_char nid[22];
    NodeID * ppgl = (NodeID *) ninf->GetPreferredPGL();
    if (ppgl) {
      const u_char * ppgl_id = (u_char *)ppgl->GetNID();
      nid[0] = ppgl_id[0];
      nid[1] = ppgl_id[1];
    } else { // I will just do something reasonable - not necessarily correct
      nid[0] =  96;
      nid[1] = 160;
    }
    _addr->encode(nid +2);
    _node_id = new NodeID(nid);
  }
}

logos_node::logos_node(const NodeID * nid, int logical_port) :
  _node_id(0), _addr(0), _transit(0), _nodal_flag(0),
  _border_node(false), _logical_port(logical_port), _node(0)
{
  assert(nid);
  if (nid) {
    _node_id =Logos_NodeID(nid);
    _transit = new Transit(nid, logical_port);
  }
}

logos_node::logos_node(Transit *tr) :
  _node_id(0), _addr(0), _transit(0), _nodal_flag(0),
  _border_node(false), _node(0)
{
  assert(tr);
  if (tr) {
    _node_id = Logos_NodeID(tr->ShareNID());
    _logical_port = tr->GetPort();
    _transit = new Transit(*tr);
  }
}

logos_node::~logos_node( )
{
  if (_node_id) delete _node_id; 
  if (_addr)    delete _addr;
  if (_transit) delete _transit;
}

// make a deep copy of a logos_node
logos_node *logos_node::copy()
{
  const NodeID *nid = this->_transit->ShareNID();
  logos_node * the_copy =
    new logos_node(nid, this->_logical_port);
  the_copy->NodalFlag(this->_nodal_flag);
  the_copy->border_node(this->_border_node);
  return the_copy;
}

void logos_node::NodalFlag(const int nodal_flag)
{
    _nodal_flag = nodal_flag;
}

int logos_node::NodalFlag(void)
{
    return _nodal_flag;
}

const NodeID * logos_node::ShareNID(void) const  { return _node_id; }
const NodeID * logos_node::ShareTransitNID(void) const  { return _transit->ShareNID(); }
Addr   * logos_node::GetAddr(void) const { return _addr; }
Transit *logos_node::ShareTransit (){  return _transit; }
Transit *logos_node::TakeTransit ()
{
  Transit *tr = _transit;
  _transit = 0;
  return tr;
}
bool logos_node::border_node(){  return _border_node; }

void logos_node::border_node(bool true_or_false)
{
  _border_node = true_or_false;
}

int logos_node::LogicalPort()
{
  return _logical_port;
}

void logos_node::LogicalPort(int port)
{
  _logical_port = port;
}

void logos_node::Node(node_struct * n) { _node = n; }
node_struct * logos_node::Node(void) { return _node; }

// ---------- logos_edge methods ---------------------------
logos_edge::logos_edge(logos_node * s, logos_node * e,
		       attr_metrics *met,
		       int port, int remote_port) :
  _start(s), _end(e), _port(port), _remote_port(remote_port),
  _reverse(0)
{
  if (met) {
    for (int i = 0; i < 5; i++)
      _metrics[i] = *met;
  } else {
    attr_metrics newmet(1412830, 1412780, 202, 182, 8, 8);
    for (int i = 0; i < 5; i++)
      _metrics[i] = newmet;
  }
  _aggregation_token   = 0;
  _route_state         = true;
  _service_class_index = 0;
  _bypass_exception    = false;

}

logos_edge::logos_edge(logos_edge *rhs) :
  _start(rhs->_start), _end(rhs->_end), _port(rhs->_port),
  _remote_port(rhs->_remote_port),
  _service_class_index(rhs->_service_class_index),
   _route_state(rhs-> _route_state),
   _aggregation_token(rhs->_aggregation_token),
   _bypass_exception(rhs->_bypass_exception)
{
  int i;
  for (i=0; i<5; i++)
    _metrics[i] = rhs->_metrics[i];
}
logos_edge::~logos_edge() { }

/*
 * convert bits in range 0x0800 to ox8000 [0xf800] to 0-4 [5 bits]
 * to determine metric index in logos_edge
 */
int service_class_index(int service_class)
{
  int index = -1;
  int sc = ( service_class & 0xf800 ) >> 11; // 5 bits now in right position
  //
  // shift sc right till it is gone, increasing index as we go
  // this will use the highest bit only
  // fwiw: to use lowest bit add - if(sc &1) return index; before >>
  //
  while (sc) {
    index ++;
    sc = sc >> 1;
  }
  return index;
}

// copy metric to all elements specified in raig_flags
void logos_edge::SetMetric(attr_metrics & metric, int raig_flags)
{
  if (raig_flags & 0x8000)
    _metrics[4] = metric;
  if (raig_flags & 0x4000)
    _metrics[3] = metric;
  if (raig_flags & 0x2000)
    _metrics[2] = metric;
  if (raig_flags & 0x1000)
    _metrics[1] = metric;
  if (raig_flags & 0x0800)
    _metrics[0] = metric;
}

attr_metrics * logos_edge::GetMetric (int service_class)
{
  return &_metrics[service_class];
}

attr_metrics * logos_edge::GetMetric ( )
{
  return &_metrics[_service_class_index];
}

void logos_edge::Setservice_class_index (int class_index)
{
  _service_class_index = class_index;
}

int  logos_edge::Getservice_class_index (void)
{
  return _service_class_index;
}

void logos_edge::Setroute_state(bool new_state)
{
  _route_state = new_state;
}

bool logos_edge::Getroute_state(void) const
{
  return (bool)_route_state;
}

int logos_edge::Getport(void) const
{
  return _port;
}
void logos_edge::Setport(int port)
{
  _port = port;
}

int  logos_edge::GetRemotePort ( )
{
  return _remote_port;
}

void logos_edge::SetRemotePort ( int remote_port )
{
  _remote_port = remote_port;
}

logos_node * logos_edge::GetStart(void) const        { return _start;  }
void         logos_edge::SetStart(logos_node *start) { _start = start; }
logos_node * logos_edge::GetEnd(void) const          { return _end;    }
void         logos_edge::SetEnd(logos_node *end)     { _end = end;     }

void         logos_edge::AggTok(const int a)   { _aggregation_token=a;      }
const int    logos_edge::GetAggTok(void) const { return _aggregation_token; }
void         logos_edge::set_exceptional(void) { _bypass_exception = true;  }
bool         logos_edge::is_exceptional(void)  { return _bypass_exception;  }

logos_edge * logos_edge::Reverse(void)              { return _reverse; }

bool         logos_edge::Reverse(logos_edge *le)
{
  bool result = false;
  if (le) {
    if (le->_reverse == this) {
      result = true;
    } else if (le->_port  == _remote_port &&
	       le->_start == _end         &&
	       le->_aggregation_token == _aggregation_token) {
      _reverse = le;
      le->_reverse = this;
    }
  }
  return result;
}
  
ostream &operator << (ostream & os, const logos_edge * rhs)
{
    os << endl
       << "s NodeID: " << *(rhs->_start) << " port=" << rhs->_port << endl
       << "e NodeID: " << *(rhs->_end)   << " port=" << rhs->_remote_port
       << endl;
    int sc  = rhs->_service_class_index;
    int scx = (0 <= sc && sc <5)?sc:0;
    os << "metric["<< sc << "] "
       << rhs->_metrics[scx]
       << ", aggtok= " << rhs->_aggregation_token
       << (rhs->_route_state?", route":", noroute");
    return os;
}

istream & operator >> (istream & is, logos_edge & rhs)
{
  //
    return is;
}

// ---------- attr_metrics methods ---------------------------
attr_metrics::~attr_metrics(void) { }

attr_metrics::attr_metrics (attr_metrics const &met)
{
    _mcr    = met._mcr;
    _acr    = met._acr;
    _ctd    = met._ctd;
    _cdv    = met._cdv;
    _adm    = met._adm;
    _clr_0  = met._clr_0;
    _clr_01 = met._clr_01;
}

attr_metrics::attr_metrics (int mcr, int acr, int ctd, int cdv,
			    sh_int clr0, sh_int clr01, int adm) :
    _mcr(mcr), _acr(acr), _ctd(ctd), _cdv(cdv),
    _clr_0(clr0),  _clr_01(clr01), _adm(adm)
{
}

attr_metrics::attr_metrics (const ig_resrc_avail_info *raig) :
    _mcr(raig->GetMCR()), _acr(raig->GetACR()),
    _ctd(raig->GetCTD()), _cdv(raig->GetCDV()),
    _clr_0(raig->GetCLR0()),  _clr_01(raig->GetCLR01()),
    _adm(raig->GetAdminWeight())
{
}

attr_metrics & attr_metrics::operator = (const attr_metrics & rhs)
{
    _mcr    = rhs._mcr;
    _acr    = rhs._acr;
    _ctd    = rhs._ctd;
    _cdv    = rhs._cdv;
    _adm    = rhs._adm;
    _clr_0  = rhs._clr_0;
    _clr_01 = rhs._clr_01;
    return *this;
}

ostream & operator << (ostream & os, const attr_metrics & rhs)
{
  os << rhs._mcr << " " << rhs._acr   << " " << rhs._adm   << " "
     << rhs._ctd << " " << rhs._cdv   << " " << rhs._clr_0 << " "
     << rhs._clr_01;
    return os;
}

ostream & operator << (ostream & os, const attr_metrics rhs)
{
  os << rhs._mcr << " " << rhs._acr   << " " << rhs._adm   << " "
     << rhs._ctd << " " << rhs._cdv   << " " << rhs._clr_0 << " "
     << rhs._clr_01;
  return os;
}

// ---------- friends ----------------------------------------
