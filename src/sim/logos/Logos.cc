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
static char const _Logos_cc_rcsid_[] =
"$Id: Logos.cc,v 1.146 1999/03/05 17:30:38 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include <sim/logos/Logos.h>
#include <sim/logos/BaseLogos.h>
#include <sim/logos/LogosGraph.h>
#include <sim/logos/DynamicLogos.h>
#include <sim/logos/LogosInterfaces.h>
#include <DS/util/String.h>
#include <FW/basics/diag.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/pnni_ig/next_hi_level_binding_info.h>
#include <codec/pnni_ig/nodal_hierarchy_list.h>
#include <codec/pnni_ig/nodal_state_params.h>
#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/PNNI_crankback.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/visitors/VPVCVisitor.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/FileIOVisitors.h>
#include <fsm/visitors/NPFloodVisitor.h>
#include <fsm/election/ElectionVisitor.h>
#include <fsm/election/ElectionState.h>
#include <fsm/netstats/NetStatsCollector.h>
#include <fsm/omni/Omni.h>
#include <fstream.h>

const VisitorType * Logos::_fast_uni_type = 0;
const VisitorType * Logos::_np_flood_type = 0;
const VisitorType * Logos::_read_type = 0;
const VisitorType * Logos::_save_type = 0;
const VisitorType * Logos::_show_type = 0;
const VisitorType * Logos::_election_type = 0;

Logos::Logos(const NodeID * nid, ds_String * plugin_prefix, bool omni) 
  : _LocNodeID( 0 ), _rebuilding( false ), _omni( omni )
{
  assert( nid != 0 );

  _LocNodeID = new NodeID(*nid);
  _Logos();

  router( AllocateLogos( plugin_prefix != 0 ? plugin_prefix->chars() : 0, this) );

  AddPermission("*",          new LogosInterface(this));
  AddPermission("Leadership", new LogosLeadershipInterface(this));
}

void Logos::_Logos(void)
{
  _nickname = 0;
  if (_fast_uni_type == 0)
    _fast_uni_type = QueryRegistry(FAST_UNI_VISITOR_NAME);

  if (_np_flood_type == 0)
    _np_flood_type = QueryRegistry(NPFLOOD_VISITOR_NAME);

  if (_read_type == 0)
    _read_type = QueryRegistry(READ_VISITOR_NAME);

  if (_save_type == 0)
    _save_type = QueryRegistry(SAVE_VISITOR_NAME);

  if (_show_type == 0)
    _show_type = QueryRegistry(SHOW_VISITOR_NAME);

  if (_election_type == 0)
    _election_type = QueryRegistry(ELECTION_VISITOR_NAME);
}

Logos::~Logos (void) 
{
  delete _LocNodeID;  _LocNodeID = 0;
  delete [] _nickname; _nickname = 0;
  Clear();
  if ( _router ) delete _router;
#if 1
  delete (VisitorType *)_fast_uni_type; _fast_uni_type = 0;
  delete (VisitorType *)_np_flood_type; _np_flood_type = 0;
  delete (VisitorType *)_read_type; _read_type = 0;
  delete (VisitorType *)_save_type; _save_type = 0;
  delete (VisitorType *)_show_type; _show_type = 0;
  delete (VisitorType *)_election_type; _election_type = 0;
#endif
}

void Logos::router(BaseLogos * routing_plugin)
{
  _router = routing_plugin;
}

BaseLogos *Logos::router()
{
  return _router;
}

// insert a logos_node into _graph
node Logos::Insert_Node(logos_node * ln)
{
  assert(ln);

  const NodeID * nid = ln->ShareNID();
  node n = NewNode(ln);

  // only insert the root node of a logical group into _nid_to_node
  if (ln->LogicalPort()==0) {
    _nid_to_node.insert(Logos_NodeID(nid), n);
  }
  Transit *tr = new Transit (nid, ln->LogicalPort());

  // bug away lines added while developing. can be deleted when working
  // careful not to delete the .insert()
//   dic_item di_in, di_out; // bug away sjm
//   di_in  = // bug away sjm

  _Transit_to_node.insert(tr, n);
  
//   di_out = _Transit_to_node.lookup(tr); // bug away sjm
//   Transit *tr_out =  _Transit_to_node.key(di_out); // bug away sjm

  //  delete tr;

  return n;
}

// insert a logos_edge into _graph
edge Logos::Insert_Edge (logos_edge * le)
{
  assert(le);

  edge e = 0;
  dic_item di;
  
  if ( (di = _le_to_edge.lookup(le)) )
    if ( (e = _le_to_edge.inf(di)) )
      diag(SIM_LOGOS, DIAG_ERROR, "Logos: Found duplicate edge in graph.\n");

  e = NewEdge(le->GetStart(), le->GetEnd(), le);
  return e;
}


node Logos::NewNode(logos_node * ln)
{
  assert(ln);

  node n = _graph.new_node(ln);
  _ln_to_node.insert(ln, n);
  return n;
}

// shortcut
node Logos::NewNode(NodeID * nid)
{
  assert(nid);

  node n = Get_node(nid);
  if ( n != 0 )
    return n;
  _nid_to_node.insert(Logos_NodeID(nid),
		      (n = NewNode(new logos_node(nid))));
  return n;
}

//
// interface to protocol stack
//
State * Logos::Handle (Visitor* v)
{
  VisitorType vt = v->GetType();

  switch (VisitorFrom(v)) {
    case Visitor::A_SIDE:
      if (vt.Is_A(_fast_uni_type)) {
	FastUNIVisitor *fuv = (FastUNIVisitor *)v;

	switch (fuv->GetMSGType()) {
	  case FastUNIVisitor::FastUNISetup:
	    Rebuild();
	    if (_router->ComputeRoute(fuv) == true) {

	      if (fuv->Peek()) PassVisitorToA(v);
	      else             PassVisitorToB(v);

	    } else {
	      ie_cause * cause = fuv->GetCause();
	      if (cause) cause = new ie_cause(*cause);
	      else cause = new ie_cause(ie_cause::unspecified_or_unknow, ie_cause::transit_network);
	      PNNI_crankback * crankback = fuv->GetCrankback();
	      if (crankback) crankback = crankback->copy();
	      DIAG("sim.logos", DIAG_INFO, cout << "Logos " << *_LocNodeID <<
		   ":\n\trejecting call from " << *(fuv->GetSourceNID()) << ".\n");

	      // call the NetStatsCollector with type Call_Rejection
	      theNetStatsCollector().ReportNetEvent("Call_Rejection",
						    OwnerName(),
						    0, _LocNodeID);

	      u_long crv = fuv->GetCREF();
	      fuv->SetMSGType(FastUNIVisitor::FastUNISetupFailure);
	      fuv->SetRelease(cause, crankback, 0);
	      fuv->SetCREF(crv);
	      PassVisitorToA(v);
	    }
	    return this;       // <---- Get out of this function
	    break;
	  default: // Pass it on through
	    break;
	  }
      } else if (vt.Is_A(_read_type))
	ReadGraph((char *)((ReadVisitor *)v)->AsString());
      else if (vt.Is_A(_save_type))
	SaveGraph((char *)((SaveVisitor *)v)->AsString());
      else if (vt.Is_A(_show_type))
	ShowGraph((char *)((ShowVisitor *)v)->AsString());
      else if (vt.Is_A(_election_type)) {
	  ElectionVisitor *ev = (ElectionVisitor *)v;
	  if (ev->GetPurpose() == ElectionVisitor::RequestConnectivityInfo) {
	      sortseq<const NodeID *, Nodalinfo *> *NodeList =
		ev->GetNodalInfo();
	      Election_Connectivity(NodeList);
	  }
      }
      PassVisitorToB(v);
      break;

    case Visitor::B_SIDE:
      if (vt.Is_A(_fast_uni_type)) {
	FastUNIVisitor *fuv = (FastUNIVisitor *)v;
	
	switch (fuv->GetMSGType()) {
	  case FastUNIVisitor::FastUNIUplinkResponse:
	  case FastUNIVisitor::FastUNISetup:
	    Rebuild();
	    if (_router->ComputeRoute(fuv) == true) {

	      if (fuv->Peek()) PassVisitorToA(v);
	      else             PassVisitorToB(v);
      
	    } else {
	      ie_cause * cause = fuv->GetCause();
	      if (cause) cause = new ie_cause(*cause);
	      else cause = new ie_cause(ie_cause::unspecified_or_unknow, ie_cause::transit_network);
	      PNNI_crankback * crankback = fuv->GetCrankback();
	      if (crankback) crankback = crankback->copy();	      
	      DIAG("sim.logos", DIAG_INFO, cout << "Logos " << *_LocNodeID <<
		   ":\n\tFailing Setup from " << *(fuv->GetSourceNID()) << ".\n");
	      u_long crv = fuv->GetCREF();
	      fuv->SetMSGType(FastUNIVisitor::FastUNISetupFailure);
	      fuv->SetRelease(cause, crankback, 0);
	      fuv->SetCREF(crv);
	      PassVisitorToB(v);  // back to database
	    }
	    return this;       // <---- Get out of this function
	    break;
	  default: // Pass it on through
	    break;
	  }
      } else if (vt.Is_A(_np_flood_type)) {
	;
      } else if (vt.Is_A(_read_type)) {
	ReadGraph((char *)((ReadVisitor *)v)->AsString());
      } else if (vt.Is_A(_save_type)) {
	SaveGraph((char *)((SaveVisitor *)v)->AsString());
      } else if (vt.Is_A(_show_type)) {
	ShowGraph((char *)((ShowVisitor *)v)->AsString());
      }
      PassVisitorToA(v);
      break;

    case Visitor::OTHER:
      diag(SIM_LOGOS, DIAG_FATAL, 
	"State ERROR: A visitor entered logos from an unexpected source.\n");
      break;
  }
  return this;
}


void Logos::Interrupt(SimEvent* e)
{ 
  DIAG(SIM_LOGOS, DIAG_DEBUG, 
       cout << "Ack! Logos has been interrupted." << e << endl;)
}


edge Logos::NewEdge(logos_node *start, logos_node *end, logos_edge *le)
{
  assert(start && end && le);

  node v, w;
  dic_item di;
  if ( (di = _ln_to_node.lookup(start)) ) { 
    v = _ln_to_node.inf(di);
    if (end && (di = _ln_to_node.lookup(end))) {
      w = _ln_to_node.inf(di);
      return NewEdge(v, w, le);
    }
  }
  return 0;
}

edge Logos::NewEdge(node start, node end, logos_edge *le)
{
  assert(start && end && le);

  edge e = _graph.new_edge(start, end, le);
  _le_to_edge.insert(le, e);
  return e;
}


// extract info from Nodal IGs in database
void Logos::CullNodalInfo(void)
{
  DBLogosInterface * dbi = (DBLogosInterface *)QueryInterface("Database");
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  // NOTICE:  We own the list, but NOT the contents, so DON'T delete the igs!
  list< ig_ptse * > * nodal_igs = dbi->NodalInfoGroupList( );

  if ( nodal_igs != 0 && nodal_igs->empty() == false ) {
    list_item li;

    forall_items( li, *nodal_igs ) {
      ig_ptse * p = nodal_igs->inf( li );

      assert( p->GetType() == InfoGroup::ig_nodal_info_group_id );

      const list<InfoGroup *> * nigs = p->ShareMembers();

      assert( nigs != 0 && nigs->empty() == false );

      list_item nli;
      forall_items( nli, *nigs ) {
	ig_nodal_info_group * inig = (ig_nodal_info_group *)nigs->inf( nli );

	u_char buf[22];
	Addr * entity_atm_addr = (Addr *)(inig->GetATMAddress());
	entity_atm_addr->encode(buf + 2);         // use 20 byte NSAP from NIG
	delete entity_atm_addr;
	
	const DBKey * origin_dbk = dbi->ObtainOriginator( p );
	
	const u_char * origin_buffer = origin_dbk->GetOrigin();
	buf[0] = origin_buffer[0];
	buf[1] = origin_buffer[1];       // take the level from the originator
	origin_dbk = 0; origin_buffer = 0;
	
	NodeID * entity = new NodeID(buf);
	logos_node * ln = new logos_node(entity);
	
	int bit_mask =
	  ig_nodal_info_group::restrans_bit   |
	  ig_nodal_info_group::nodal_rep_bit  |
	  ig_nodal_info_group::ntrans_ele_bit;
	
	ln->NodalFlag(inig->IsSet((ig_nodal_info_group::nflags)bit_mask));
	delete entity;
	
	// If this node is not in the graph insert it
	if (!_ln_to_node.lookup(ln))
	  Insert_Node(ln);
	else
	  delete ln;
      }
    }
  }
  dbi->Unreference();
}

// search the database for all links
// both horizontal and uplinks are returned
list<ig_ptse *> * Logos::CullHUPLinks(void)
{
  list<ig_ptse *> * rval = 0;

  DBLogosInterface * dbi = (DBLogosInterface *)QueryInterface("Database");
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();

  list< ig_ptse * > * hup_igs = dbi->HorizontalUplinkList();

  if ( hup_igs != 0 && hup_igs->empty() == false ) {
    list_item li;

    forall_items( li, *hup_igs ) {

      ig_ptse * p = hup_igs->inf( li );

      assert( (p->GetType() == InfoGroup::ig_uplinks_id ||
	       p->GetType() == InfoGroup::ig_horizontal_links_id) );

      if (!rval)
	rval = new list<ig_ptse *>;
      rval->append( p );
    }
  }
  dbi->Unreference();
  return rval;
}

//
// find all nodal state parameters
// insert border nodes default radii, bypasses and exceptions
// move uplinks to border nodes when appropriate
//
void Logos::CullNodalStateParameters(Logos::prune_type trim)
{
  DBLogosInterface * dbi = (DBLogosInterface *)QueryInterface("Database");
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();

  dic_item di;
  dictionary<const NodeID *, list<ig_resrc_avail_info   *> *> Default_Radius;
  dictionary<const NodeID *, list<ig_nodal_state_params *> *> Bypass_Exception;

  list< ig_ptse * > * nsp_igs = dbi->NodalStateParamsList();

  if ( nsp_igs != 0 && nsp_igs->empty() == false ) {
    list_item li;

    forall_items( li, *nsp_igs ) {

      ig_ptse * p = nsp_igs->inf( li );

      assert( p->GetType() == InfoGroup::ig_nodal_state_params_id );

      const DBKey  * dbk = dbi->ObtainOriginator( p );
      const NodeID * originator = new NodeID(dbk->GetOrigin());
      // it is awkward for the aggregator and unnecessary for routing
      // to expand my parents 
      if (MyFamily(originator)) {
	delete originator;
	continue;
      }
      const list<InfoGroup *> * ig_list = p->ShareMembers();
      if (ig_list == 0) { // ignore empty ptse.
	delete originator;
	continue;
      }

      list_item nli;
      ig_nodal_state_params * nsp = 0;

      forall_items( nli, *ig_list ) {
	
	InfoGroup * igp = ig_list->inf( nli );
	nsp = (ig_nodal_state_params *)igp;
	const int in_port  = nsp->GetInputPID();
	const int out_port = nsp->GetOutputPID();
	
	//
	// there are three types of nodal state parameters
	//
	// 1) in_port == 0 && out_port == 0 - default Radius
	// 2) in_port == 0 || out_port == 0 - Exception
	// 3) in_port != 0 && out_port != 0 - Bypass
	// 
	if (in_port == 0 && out_port == 0 ) { // a Default Radius
	  
	  // do we already have a raig list for this node
	  if (!(di = Default_Radius.lookup(originator))) {
	    const NodeID * ocopy = originator->copy();
	    di = Default_Radius.insert(ocopy, new list<ig_resrc_avail_info *>);
	  }
	  list<ig_resrc_avail_info *> *r = Default_Radius.inf(di);
	  // copy from RAIG - note these are pointers into the database
	  // these pointers may not be deleted by us
	  
	  ig_resrc_avail_info *raig = 0;
	  const list<ig_resrc_avail_info *> & RAIG = nsp->GetRAIGS();
	  list_item rli;
	  forall_items(rli, RAIG) {
	    raig = RAIG.inf(rli);
	    r->append(raig);
	  }

	} else {		// exception or bypass
	  
	  list<ig_nodal_state_params *> * n = 0;

	  // do we already have a nsp list for this node
	  if ( (di = Bypass_Exception.lookup(originator)) ) {
	    n = Bypass_Exception.inf(di);
	  } else { // no create one
	    n = new list<ig_nodal_state_params *>;
	    const NodeID *ocopy = originator->copy();
	    Bypass_Exception.insert(ocopy, n);
	  }
	  n->append(nsp);
	}
	
      }
      delete originator;
    }
  }
  
  //
  // process the Default_Radius dictionary
  //
  Add_Radial_Links (&Default_Radius);

  //
  // add bypasses to the graph and update radial exceptions
  //
  Add_ByPasses (&Bypass_Exception);

  // when possible move uplink to remote border node
  Replace_UpLinks();

  // cleanup
  while (!Default_Radius.empty()) {
    dic_item di = Default_Radius.first();
    delete Default_Radius.key(di);
    Default_Radius.del_item(di);
  }
  while (!Bypass_Exception.empty()) {
    dic_item di = Bypass_Exception.first();
    delete Bypass_Exception.inf(di);
    delete Bypass_Exception.key(di);
    Bypass_Exception.del_item(di);
  }
  
  dbi->Unreference();
}

// is this NodeID a possible leader for me
bool Logos::MyFamily(const NodeID *GroupLeader)
{
  int result = false;
  if (GroupLeader) {
    const u_char *my_id =  _LocNodeID->GetNID() + 2;
    const u_char *gl_id = GroupLeader->GetNID() + 2;
    int Leader_Level = GroupLeader->GetLevel();
    int match = bits_equal(my_id, gl_id, Leader_Level);
    if (match == Leader_Level) result = true;
  }
  return result;
}

void Logos::Add_Radial_Links(dictionary<const NodeID *,list<ig_resrc_avail_info *>*>
			     *Default_Radius)
{
  dic_item di;
  forall_items(di, *Default_Radius) {

    edge e;
    node n;
    const NodeID *orig_nodeid = Default_Radius->key(di);
    const list<ig_resrc_avail_info *> *RAIG = Default_Radius->inf(di);
    
    n = Get_node(orig_nodeid);

    // 
    // splice border nodes into the graph
    //
    
    list<edge>  in_edges = _graph.in_edges(n);
    list<edge> out_edges = _graph.out_edges(n);
    
    while (in_edges.size() > 0) {
      e = in_edges.pop();
      split_incoming_edge(e, RAIG);
    }
    while (out_edges.size() > 0) {
      e = out_edges.pop();
      split_outgoing_edge(e, RAIG);
    } 
    
  }
}

void Logos::Add_ByPasses(dictionary<const NodeID *, list<ig_nodal_state_params *> *>
			 *Bypass_Exception)

{
  dic_item di;
  forall_items (di, *Bypass_Exception) {

    list<ig_nodal_state_params *> *be_list = Bypass_Exception->inf(di);
    const NodeID *Originator = Bypass_Exception->key(di);
    ig_nodal_state_params *nsp=0;

    while ( be_list->size() > 0) {
      // nsp is a pointer into the Database
      // it must NOT be deleted!
      nsp = be_list->pop();

      const int in_port  = nsp->GetInputPID();
      const int out_port = nsp->GetOutputPID();

      // locate border nodes in the _graph - create if necessary

      node  in_node = validate_transit_node(Originator,  in_port, 0, false,0);
      node out_node = validate_transit_node(Originator, out_port, 0, false,0);

      // find any logos_edge that links these border nodes - the_edge
      edge the_edge = 0;
      edge e;
      forall_out_edges(e, out_node) {
	logos_edge *candidate = _graph[e];
	if (candidate->GetRemotePort() == in_port) {
	  the_edge = e;
	  break;
	}
      }
      
      // create the_edge if none exists
      if (the_edge == 0) {
	// can't find it -- create it
	logos_node *out = _graph[out_node];
	logos_node * in = _graph[ in_node];
	attr_metrics m; // virtually unroutable metric 
	logos_edge *newle = new logos_edge(out, in, &m, out_port, in_port);
	the_edge = Insert_Edge(newle);
      }

      logos_edge *the_logos_edge = _graph[the_edge];
      the_logos_edge->set_exceptional();
      
      // insert Raigs from the nodal state parameter list 
      //
      const list<ig_resrc_avail_info *> RAIG = nsp->GetRAIGS();
      ig_resrc_avail_info *raig;
      list_item rli;
      forall_items(rli, RAIG)  {
	raig = RAIG.inf(rli);
	attr_metrics m2(raig);
	u_int flags = (u_int)raig->GetFlags();
	the_logos_edge->SetMetric(m2, flags);
      }

    } // end while ( be_list->size() > 0)
    
  } // end forall_items (di, Bypass_Exception)

}

// compare contents of two nodeid ptr contents.
// returns ZERO if they are equivalent for routing
// if both are null "they are the same"
// if one is null compare as pointes
// limit comparison to prefix.
// use only in routing code
int same_as (const NodeID *lhs, const NodeID *rhs)
{

  if ( lhs == 0 && rhs == 0) return 0;
  if ( lhs == 0 || rhs == 0) 
    return ((long)lhs > (long)rhs) ? 1 : -1;
  
  const u_char *lhs_id = lhs->GetNID();
  const u_char *rhs_id = rhs->GetNID();

  int len = 16 + (lhs_id[1]<104 ? lhs_id[1] : 104);
  return bitcmp(lhs_id, rhs_id, len);
  
}

//
// if an uplink goes to the center of a higher group, replace it if possible
// with a link to the appropriate border node in that group.
// The object _uplinks is emptied by this process. At the same time
// the object _borderlinks is created as an exact copy of _uplinks.
// a copy is created because _uptimes contains the actual logos_edges in use
// in _graph, and these are subject to change (by this very code);
//
void Logos::Replace_UpLinks()
{
  while (_uplinks.size()) {
    logos_edge *le = _uplinks.pop();
    _borderlinks.append(new logos_edge(le)); // copy
    
    logos_node *upper = le->GetEnd();
    int aggtok = le->GetAggTok();
    
    if (aggtok) { // ignore it if aggregation token is zero
      const u_char *myupchar = MatchingLevel(upper->ShareNID());
      
      if (myupchar) { // ignore it if I don't have a logical representation 
	NodeID *upnid = Logos_NodeID(myupchar);
	dic_item d = _nid_to_node.lookup(upnid);
	
	if (d) { 
	  // router node at same level as upper end of link
	  node up_node = _nid_to_node.inf(d);
	  // look for a link with same aggregation token
	  edge the_edge = 0;
	  edge e = 0;
	  logos_edge *radius = 0;

	  forall_out_edges(e, up_node) {
	    radius = _graph[e];
	    if (radius->GetAggTok() == aggtok) {
	      the_edge = e;
	      break;
	    }
	  }
	    
	  if (the_edge) { 
	    //
	    // the_edge might lead to
	    // a) border node in our peer group       - follow
	    // b) border node in target peer group    - move uplink
	    // c) center node in target peer group    - ignore
	    // d) center node in unrelated peer group - ignore
	    //
	    const NodeID *target = radius->GetEnd()->ShareNID();
	    if (same_as(target, upnid)==0) { // a)
		
	      logos_node *my_border = radius->GetEnd();
	      up_node = _ln_to_node.inf(_ln_to_node.lookup(my_border));
	      radius = 0;
	      target = 0;
	      forall_out_edges(e, up_node) {
		radius = _graph[e];
		if (radius->GetAggTok() == aggtok  &&
		    same_as(radius->GetEnd()->ShareNID(), upper->ShareNID())==0 &&
		    radius->GetEnd()->LogicalPort() != 0 ) {
		  the_edge = e;
		  target = radius->GetEnd()->ShareNID();
		  break;
		}
	      }
	    }

	    if (same_as(target, upper->ShareNID())==0 ) {
	      int logicalport; 
	      if ( (logicalport = radius->GetEnd()->LogicalPort()) != 0 ) {
		// b) remote border node
		
		node uplink_border =
		  validate_transit_node(target, logicalport,
					0, false, 0);
		if (uplink_border) {
		  d = _le_to_edge.lookup(le);
		  if (d) {
		    edge uplink_edge = _le_to_edge.inf(d);
		    _graph.move_edge(uplink_edge,
				     _graph.source(uplink_edge),
				     uplink_border);
		    le->SetEnd(_graph[uplink_border]);
		    le->SetRemotePort(logicalport);
		  }
		}
	      }
	    } 
	  }
	}
	delete upnid;

      }
    } // aggtok == 0
  } // end while _uplinks.size()
}


//     we must make [o] <---------- e --------- [n]
//            into  [o] <-- e1 --> [B] <-- e -- [n]
//                                  
void Logos::split_incoming_edge (edge e,
			    const list<ig_resrc_avail_info *> *RAIG)
{
  logos_edge *le = _graph[e];
  int central_port = le->GetRemotePort();

  if ( central_port != 0 ) {
    
    const NodeID *dest = le->GetEnd()->ShareTransitNID();
    node border_node =
      validate_transit_node(dest, central_port, le, true, RAIG);
    
    // move the loge_edge; terminate at border node
    _graph.move_edge(e, _graph.source(e), border_node);
    logos_node *ln = _graph[border_node];
    le->SetEnd(ln);
  }
}

//     we must make [o] ---------- e --------> [n]
//            into  [o] <-- e1--> [B] -- e --> [n]
void Logos::split_outgoing_edge (edge e,
				 const list<ig_resrc_avail_info *> *RAIG)
{
  logos_edge *le = _graph[e];
  int central_port = le->Getport();
  
  if (central_port != 0) {

    const NodeID *source = le->GetStart()->ShareTransitNID();
    node border_node =
      validate_transit_node(source, central_port, le, false, RAIG);
    
    // copy the loge_edge; originate at border node;
    _graph.move_edge(e, border_node, _graph.target(e));
    logos_node *ln = _graph[border_node];
    le->SetStart(ln);
  } 
}

//
// find node (nid,port) -- insert if not found
// if a logos_edge is given split it at the node
//
node Logos::validate_transit_node ( const NodeID *nid, int port,
				    logos_edge *le, bool in,
				    const list<ig_resrc_avail_info *> *RAIG )
{
  node border_node;
  Transit *tr = new Transit (nid, port);
  dic_item dic = _Transit_to_node.lookup(tr);
  delete tr;

  if ( dic == 0 ) {
    
    logos_node * ln = new logos_node(nid, port);

    Insert_Node (ln);
    border_node = _ln_to_node.inf(_ln_to_node.lookup(ln));

    if (le) {
      if (in) // le is an incoming edge
	add_central_radius(le, le->GetEnd(),   ln, RAIG);
      else    // le is an outgoing edge
	add_central_radius(le, le->GetStart(), ln, RAIG);
    }
    
  } else {
    border_node =  _Transit_to_node.inf(dic);

  }
  
  return border_node;
}

void Logos::add_central_radius(logos_edge *le,
			       logos_node *central,
			       logos_node *border,
			       const list<ig_resrc_avail_info *> *RAIG)
{
      // start at central node and go to border node 
      logos_edge *le2 = new logos_edge(le);
      le2->SetStart(central);
      le2->SetEnd(border);
      le2->Setport(0);
      le2->SetRemotePort(border->LogicalPort());
 
      // zero the attr_metrics -- make them unroutable!
      attr_metrics m;
      le2->Setservice_class_index(0);
      attr_metrics *m2 = le2->GetMetric(); // pointer to  array of metrics
      for (int i=0; i<5; i++) m2[i] = m;
      
      // insert the default radius
      if (RAIG) {
	ig_resrc_avail_info *raig;
	list_item ra;
	forall_items(ra, *RAIG)  {
	  raig = RAIG->inf(ra);
	  attr_metrics m2(raig);
	  u_int flags = (u_int)raig->GetFlags();
	  le2->SetMetric(m2, flags);
	}
      }
      Insert_Edge (le2);
      
      // from the border to the central node 
      logos_edge * le3 = new logos_edge(le2);
      le3->SetStart(le2->GetEnd());
      le3->SetEnd(le2->GetStart());
      le3->Setport(le2->GetRemotePort());
      le3->SetRemotePort(le2->Getport());
      Insert_Edge(le3);
}

//
// delete all the nodes, edges, logos_nodes and logos_edges from _graph
// and the use Update to rebuild from the database.
//
void Logos::Rebuild(prune_type trim)
{
  _rebuilding = true;
  Clear( );
  Update(trim);
  _rebuilding = false;
}

// clean up _graph and the dictionaries
void Logos::Clear(void)
{
  // First delete all the nodes
  node n;
  dic_item di;
  while( !_nid_to_node.empty()) {
    di = _nid_to_node.first();
    const NodeID *key =_nid_to_node.key(di);
    _nid_to_node.del_item(di);
    delete key;
  }
  _nid_to_node.clear();

  list <node> all_nodes = _graph.all_nodes();
  while(all_nodes.size()) {
    n = all_nodes.pop();
    // delete all edges in and out of this node
    edge e;
    while ( (e = n->first_in_edge()) ) {
      node from = _graph.source(e);
      from->del_edge(e);
      n->del_edge(e);
      logos_edge *le = _graph[e];
      _le_to_edge.del(le);
      delete le;
      _graph.del_edge(e);
      delete e;
    }

    while ( (e = n->first_edge()) ) {
      node to = _graph.target(e);
      to->del_edge(e);
      n->del_edge(e);
      logos_edge *le = _graph[e];
      _le_to_edge.del(le);
      delete le;
      _graph.del_edge(e);
      delete e;
    }
    
    logos_node * ln = _graph[n];
    _graph.del_node(n);
    
    di = _ln_to_node.lookup(ln);
    node nn = _ln_to_node.inf(di);
    delete nn;
    _ln_to_node.del_item(di);
    
    Transit * tr_key = ln->TakeTransit(); // zeros _transit in ln

    di = _Transit_to_node.lookup(tr_key);
    Transit *key1 =  _Transit_to_node.key(di);
    delete key1;
    _Transit_to_node.del_item(di);

    delete tr_key;
    delete ln;
  }

  _Transit_to_node.clear();
    
  while (_levels.size()) {
    NodeID *nid = _levels.pop();
    delete nid;
  }

  // then delete all the rmaining edges - should be a no-op
  list <edge> all_edges = _graph.all_edges();

  while (all_edges.size()) {
    edge e = all_edges.pop();
    logos_edge * le = _graph[e];
     di = _le_to_edge.lookup(le);
     edge ee = _le_to_edge.inf(di);
     delete ee;
     _le_to_edge.del(le);
     delete le;
    _graph.del_edge(e);
    delete e;
  }

  _graph.clear();

  // get rid of _borderlinks
  while  (_borderlinks.size()) {
    logos_edge *le = _borderlinks.pop();
    delete le;
  }
}

//
// populate the graph
// if trim=routing a full _graph is created with complex nodes expanded
// if trim=aggregation something different happens
//
void Logos::Update(prune_type trim)
{
  // construct list of NodeID's of my hierarchy
  GetLevels();

  // include all the nodes
  CullNodalInfo();

  DBLogosInterface * dbi = (DBLogosInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();

  // update the edges
  list<ig_ptse *> * huplink_list = CullHUPLinks();
  if (huplink_list) {
    list_item li;
    forall_items(li, *huplink_list) {
      logos_edge  * fle  = 0;
      ig_ptse     * ptse = huplink_list->inf(li);
      const DBKey * dbk  = dbi->ObtainOriginator( ptse );
      NodeID      * nid  = new NodeID( dbk->GetOrigin() );

      list_item li;
      const list<InfoGroup *> * ig_list = ptse->ShareMembers();
      if (ig_list == 0) { // ignore empty ptse.
	delete nid;
	continue;
      }
      // Search through the list of HUPLinks
      forall_items(li, *ig_list) {
	InfoGroup * igp = ig_list->inf(li);
	
	// horizontal links
	if (igp->GetId() == InfoGroup::ig_horizontal_links_id) {
	  fle = process(this, nid, (ig_horizontal_links *)igp);
	  // If this edge is not in the graph insert it
	  if (fle && !_le_to_edge.lookup(fle)){
	    Insert_Edge (fle);
	  } else {
	    delete fle;	// DT 970829 LEAK_SCOPE
	  }
	}
	// uplinks
	else if (igp->GetId() == InfoGroup::ig_uplinks_id) {
	  fle = processfwd(this, nid, (ig_uplinks *)igp);
	  if (!_le_to_edge.lookup(fle)) {
	    Insert_Edge (fle);
	    _uplinks.append(fle);
	  } else {
	    delete fle;
	  }
	  //
	  // We must not have both forward and backward links
	  // in the same graph - page ? of pnni 1.0
	  //
	  // ble = processbwd(this, nid, (ig_uplinks *)igp);
	  //if (!_le_to_edge.lookup(ble))
	  //  Insert_Edge (ble);
	  //else
	  //  delete ble;
	}
      } // end of forall_items(li, ig_list)
      delete nid;		// DT 970829 LEAK_SCOPE
    } // end of forall_items(li, *huplink_list) 
  }
  delete huplink_list;	// DT 970829 LEAK_SCOPE

  dbi->Unreference();

  // finally  deal with complex nodes
  CullNodalStateParameters(trim);
}


void Logos::ShowGraph(char *filename)
{
  ofstream of(filename);
  if ( !_rebuilding ) // avoid recursive rebuild
    Rebuild();
  
  node n;
  forall_nodes(n, _graph) {
    logos_node * ln = _graph[n];
    of<<"!birth "<< *ln->ShareNID()<<":"<<ln->LogicalPort()<<" !Switch"<< endl;
  }
  edge e;
  forall_edges(e, _graph) {
    logos_edge * le = _graph[e];
    of<<"!connected "
      <<*le->GetStart()->ShareNID()<<":"<<le->GetStart()->LogicalPort()<< " 3"
      <<" !to "
      <<*le->GetEnd()->ShareNID()   <<":"<< le->GetEnd()->LogicalPort()<< " 3"
      << endl;
    
  }
  of << "xxxxxxxxxx" << endl;
  of.close();
  
  char *visprog = (char *)getenv("GTbin");
  if (visprog) {
    int cmdlen = strlen(visprog) + strlen(filename) +2;
    char * cmd = new char[cmdlen];
    strcpy(cmd, visprog);
    strcat(cmd," ");
    strcat(cmd,filename);
    system( cmd );
    delete [] cmd;
  } else {
#ifdef DEBUG_LOGOS
    DIAG(SIM_LOGOS, DIAG_DEBUG, 
	 cout << "Logos::ShowGraph(" << filename <<"): error" << endl;
	 cout << "       GTbin not set in environment" << endl;)
#endif
  }
}

void Logos::SaveGraph(char * filename)
{
  if ( !_rebuilding )  // avoid recursive rebuild
    Rebuild();
  //  _graph.write(filename);
  ofstream ofs(filename);
  node n;
  forall_nodes(n, _graph) {
    logos_node * ln = _graph[n];
    ofs << ln << endl;
  }
  edge e;
  forall_edges(e,_graph) {
    logos_edge * le = _graph[e];
    ofs << le << endl;
  }
}

int Logos::ReadGraph(char * filename)
{
  return _graph.read(filename);
}


// Volatile debugging diagnostic - Do NOT use in simulation.
// change as needed to help debugging - (gdb) p Logos::Help()
void Logos::help(void)
{
  node ni;
  char outstring[256];
  forall_nodes(ni,_graph){
    logos_node * n = _graph[ni];
    
    sprintf(outstring, "node: addr = %p, contents:", n);
    cout << outstring << n << endl;
  }
}

// djikstra
// Single Source Shortest Path
void Logos::sssp(node s,                   // starting node
		 edge_array<int>&  cost,   // cost to traverse an edge 
		 node_array<int>&  dist,   // distance of node from source 
		 node_array<edge>& pred,   // edge leading to node
		 p_queue<int,node> * PQ)
{
  node_array<pq_item> I(_graph);
  node v;

  // set distance (cost) to all nodes to MAXIMUM
  forall_nodes(v,_graph) {
    pred[v] = 0x0;
    dist[v] = MAXINT;
  }

  // distance to starting point is 0, insert start into priority queue 
  dist[s] = 0;
  I[s] = PQ->insert(0,s);

  // while there are items in the priority queue
  while (! PQ->empty()) {
    pq_item it = PQ->find_min(); // minimum cost node in priority queue
    node u = PQ->inf(it);
    int du = dist[u];
    edge e;
    
    forall_adj_edges(e,u) {	// loop on edges to the node
      logos_edge *le = _graph[e];
      if (le->Getroute_state()) {  
	v = _graph.target(e);
	int c = du + cost[e];	// cost to next node is the min of current cost
	if (c < dist[v]) {	// and cost to current node + cost of edge
	  if (dist[v] == MAXINT)
	    I[v] = PQ->insert(c,v); // insert on first encounter
	  else
	    PQ->decrease_p(I[v],c); // reset position in priority queue
	  dist[v] = c;		// remember cost 
	  pred[v] = e;		// remember edge with least cost
	}
      }
    }
    PQ->del_item(it);		// remove current node
  }
}

// returns logos_node associated with a NodeID
logos_node * Logos::Getlogos_node(const NodeID * nid)
{
  assert(nid);

  node n;
 
  if ( (n = Get_node(nid)) == 0)
    return 0;
  
  return _graph[n];
}

logos_node * Logos::Getlogos_node(Transit * tr)
{
  assert(tr);

  node n;
 
  if ( (n = Get_node(tr)) == 0)
    return 0;
  
  return _graph[n];
}

node Logos::Get_node(const NodeID *nid)
{
  dic_item di;
  node n = 0;
  NodeID *znid = Logos_NodeID(nid);
  if ( (di = _nid_to_node.lookup(znid)) )
    n = _nid_to_node.inf(di);
  delete znid;
  return n;
}

node Logos::Get_node(Transit *tr)
{
  dic_item di;
  node n = 0;

  if ( (di = _Transit_to_node.lookup(tr)) )
    n = _Transit_to_node.inf(di);

  return n;
}

// extract edge info about horizontal links from Database
logos_edge * process(Logos *gcac, const NodeID * local,
		     ig_horizontal_links * horiz)
{
  NodeID     * remote = (NodeID *)horiz->GetRemoteID();
  logos_node * l      = gcac->Getlogos_node(local);  // new logos_node(local);
  logos_node * r      = gcac->Getlogos_node(remote); // new logos_node(remote);

  delete remote;	/* DT 970829 logos_node() makes a copy, so
			 * this delete is (1) safe and (2) required to
			 * avoid a LEAK_SCOPE 
			 */
  // If either one is 0 we cannot make a proper edge
  if (!l || !r)
    return 0;

  logos_edge * edge   = new logos_edge(l, r);
  // l and r are never getting freed ... because they are in _graph

  list<ig_resrc_avail_info *> raigs = horiz->GetRAIGS();

  edge->Setport(horiz->GetLocalPID());
  edge->SetRemotePort(horiz->GetRemotePID());
  edge->AggTok(horiz->GetAggTok());

  list_item li;
  forall_items(li, raigs) {
    ig_resrc_avail_info * rai = raigs.inf(li);
    attr_metrics metric(rai);
    u_int index = rai->GetFlags();
    edge->SetMetric(metric, (int)index);
  }
  edge->AggTok(horiz->GetAggTok());
  // assert(edge && edge->GetStart() && edge->GetEnd());
  return edge;
}

logos_edge * processfwd (Logos *gcac, const NodeID * local, ig_uplinks * upl)
{
  NodeID * tmp = upl->GetRemoteID();
  NodeID * remote = new NodeID(*tmp);
  delete tmp;
  logos_node * l = gcac->Getlogos_node(local),
             * r = gcac->Getlogos_node(remote);
  //
  // uplinks present a special challenge. When we first try to setup the
  // svc between two peergoups, the other peergroup is represented only
  // as remote node in the uplink ptse. It is not in the graph (yet).
  // later it will be inserted as a nodal ig. If 'r' is zero, we insert
  // 'remote' into the graph.
  //
  if (r == 0) {
    NodeID *entity = new NodeID(*remote);
    logos_node * ln = new logos_node(entity);
    int bit_mask =
      ig_nodal_info_group::restrans_bit   |
      ig_nodal_info_group::nodal_rep_bit  |
      ig_nodal_info_group::ntrans_ele_bit;
	    
    ln->NodalFlag(bit_mask);
    delete entity;
    node n = gcac->Insert_Node(ln);
    if (n==0) cout << " insert(ln) "<< __FILE__ << __LINE__ <<"failed" <<endl;
    r = gcac->Getlogos_node(remote);
  }
  
  logos_edge * edge = new logos_edge(l, r);

  edge->Setport(upl->GetLocalPID());
  edge->AggTok(upl->GetAggTok());

  list<InfoGroup *> ig_list = upl->GetIGs();

  list_item li;
  forall_items(li, ig_list) {
    InfoGroup * igp = ig_list.inf(li);

    // Is it an Outgoing RAIG?
    if (igp->GetId() == 128) {
      ig_resrc_avail_info * rai = (ig_resrc_avail_info *)igp;
      attr_metrics metric(rai);
      u_int mdex = rai->GetFlags();
      edge->SetMetric(metric, (int) mdex);
    }
  }
  delete remote;
  return edge;
}

// this is code to extract downlinks
// it is not called -- call in Update is commented out
logos_edge * processbwd (Logos *gcac, const NodeID * remote, ig_uplinks * upl)
{
  NodeID * local = new NodeID(*(upl->GetRemoteID()));
  logos_node * l = gcac->Getlogos_node(local),
             * r = gcac->Getlogos_node(remote);
  logos_edge * edge = new logos_edge(l, r);
  delete local;
  
  edge->Setport(upl->GetLocalPID());
  edge->AggTok(upl->GetAggTok());

  list<InfoGroup *> ig_list = upl->GetIGs();

  list_item li;
  forall_items(li, ig_list) {
    InfoGroup * igp = ig_list.inf(li);

    // Is it an ULIA?
    if (igp->GetId() == 34) {
      ig_uplink_info_attr * ulia = (ig_uplink_info_attr *)igp;

      list_item ulia_li;
      forall_items(ulia_li, ulia->GetIGs()) {
	//InfoGroup * sub_igp = ulia->GetIGs().inf(li);

	// Is it an Outgoing RAIG?
	if (igp->GetId() == 128) {
	  ig_resrc_avail_info * rai = (ig_resrc_avail_info *)igp;
	  attr_metrics metric(rai);
	  u_int index = rai->GetFlags();
	  edge->SetMetric(metric, (int)index);
	}
      }
      break;
    }
  }
  return edge;
}


void Logos::Election_Connectivity(sortseq<const NodeID *, Nodalinfo *>
				  *NodeList)
{
  edge e;
  node n;
  node nfrom; // this node
  logos_edge * le;
  logos_node * ln;
  
  // get a fresh graph this must be done before declaring cost, dist and pred
  Rebuild();
  
  edge_array<int>  cost(_graph);
  node_array<int>  dist(_graph);
  node_array<edge> pred(_graph);
  p_queue<int,node>* PQ = new p_queue<int,node>;
  
  if ( (nfrom = Get_node(_LocNodeID)) == 0) {
    cerr << "Logos::Election_Connectivity can't find self in _graph"
	 << endl;
    return;
  }
  // loop through all edges -- mark as routable
  forall_edges(e, _graph) {
    le = _graph[e];
    le->Setroute_state((bool)true);
    cost[e] = 1;  // hop cost
  }
  // loop through all nodes
  //    if a node is marked as no election transit and is not the source,
  //    then eliminate all edges leading away from this node
  // we route "to"/"from" a election non-transit node but not "through"
  forall_nodes(n, _graph) {
    if ( n != nfrom) { 
      ln = _graph[n];
      if (ln->NodalFlag() & ig_nodal_info_group::ntrans_ele_bit) {
	forall_out_edges(e,n) {
	  le = _graph[e];
	  le->Setroute_state(false);
	}
      }
    }
  }  
  
  //
  // use dijkstra alogorithm to find path based on cost = hops
  //
  sssp(nfrom, cost, dist, pred, PQ);
  delete PQ;
  
  seq_item si;
  for (si = NodeList->min(); si; si=NodeList->next(si)) {
    Nodalinfo * ninfo = NodeList->inf(si);
    const NodeID * nid = NodeList->key(si);
    
    if  ( (n = Get_node(nid)) != 0) {
      if (dist[n] < MAXINT) ninfo->SetElectionFlag(true);
      else                  ninfo->SetElectionFlag(false);
    } else {
      cerr << "Logos::Election_Connectivity: "
	   << *nid << " unreachable" << endl;
      ninfo->SetElectionFlag(false);
    }
  }
}

list<NodeID *> * Logos::GetLevels(void)
{
  ig_nodal_hierarchy_list * hlevels = 0;
  // there is no simple way to know that the levels have changed
  // so delete the old levels
  while (_levels.size()) {
    NodeID *old =_levels.pop();
    delete old;
  }

  // retrieve levels from database if possible
  DBLogosInterface * dbi = (DBLogosInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  hlevels = dbi->GetNHL( _LocNodeID );
  dbi->Unreference();

  // add myself at the physical level
  NodeID *me = new NodeID(*_LocNodeID);
  _levels.push(me);
  int physlevel = me->GetLevel();

  if (hlevels) {
    const list<Level *> * levels = hlevels->ShareLevels();

    list_item it;
    forall_items(it, *levels) {
      Level * Lev = levels->inf(it);
      // avoid adding a second occurrence at physical level
      NodeID *tmp = Lev->GetNID();
      if (tmp->GetLevel() != physlevel)
	_levels.append(tmp); // this makes a copy!
      //      delete tmp;
    }
    delete hlevels;
  }
  //  delete me;  
  return &_levels;
}

// copy elements of _graph that are at level lowest level and above,
// up to but not including target level.
//
// level has a hierachical meaning. thus 96 is lower than 80
// level here refers to the parent level byte in the NodeID - _id[0]

LogosGraph * Logos::GetGraph (int target_level, Logos::prune_type trim)
{
  LogosGraph *lg = new LogosGraph(target_level);
  logos_node *ln=0;
  node n;
  dic_item di;
  
  // copy all the nodes that satisfy the level requirements
  forall_nodes(n, _graph) {
    ln = _graph.inf(n);
    const NodeID *nid = ln->ShareNID();
    int level = nid->GetLevel();
    
    if ( (trim == routing && level > target_level) ||
	 (trim == aggregation && level == target_level)) {
      lg->insert_logosnode_copy(ln);
    }
  }
  
  //
  // for each node copied, copy all relevant edges
  //
  list<node> base = lg->_graph.all_nodes();
  list<logos_edge *> logos_edge_list;

  while (base.empty() == false) {
    n = base.pop();
    // for each node in the new graph
    ln = lg->_graph.inf(n);
    
    Transit *tr = ln->ShareTransit();  // live pointer don't delete
    // find the node in the LogosGraph
    di = _Transit_to_node.lookup(tr);
    node Ln = _Transit_to_node.inf(di);

    // loop on edges of the node 
    edge e;
    list<edge> edge_list = _graph.adj_edges(Ln);
    while ( edge_list.size() ) {
      e = edge_list.pop();
      logos_edge *le = _graph[e];
      if ( logos_edge_list.search(le) == 0 ) { 
	logos_node *start = le->GetStart();
	logos_node *target= le->GetEnd();

	// each edge has two ends at least one is already in the LogosGraph
	// we are building. If we are routing, we will add any missing
	// nodes to the Logos_graph. For aggregation we do not add
	// missing nodes 
	Transit *start_tr =  start->ShareTransit();
	Transit *end_tr   = target->ShareTransit();
	node start_node=0, end_node=0;
	logos_node *start_logos_node=0, *end_logos_node=0;

	// check the start node
	int start_level = start_tr->GetLevel();
	start_node = demand_node ( lg, start_tr, (trim==routing) );
	if (start_node != 0) start_logos_node = lg->_graph[start_node];

	// check the end node
	int end_level = end_tr->GetLevel();
	end_node = demand_node ( lg, end_tr, (trim==routing) );
	if (end_node) end_logos_node = lg->_graph[end_node];

	// see if this edge makes either node a border node
	// note that routing doesn't care about or use _border_node
	if (trim == aggregation) {
	  if ( (start_level == target_level) && (end_level != target_level))
	    start_logos_node->border_node(true);
	}

	// create the new edge if both ends are in our LogosGraph
	if (start_logos_node != 0 && end_logos_node != 0) {
      
	  // copy the port and attr_metric information
	  int port        = le->Getport();
	  int remote_port = le->GetRemotePort();
	  int aggtok      = le->GetAggTok(); 
	  attr_metrics *a = new attr_metrics(*le->GetMetric());
	
	  logos_edge *newlogosedge =
	    new logos_edge(start_logos_node,
			   end_logos_node,a, port, remote_port);
	  newlogosedge->AggTok(aggtok);
	  lg->_graph.new_edge(start_node, end_node, newlogosedge);
	  logos_edge_list.push(le);
	  delete a;
	}
      }
    }
  }
  // to deal with reverse bandwidths it is convienient
  // for each edge to know where its reverse is.
  // It is easiest to delay setting the reverse paths
  // until the entire graph is complete -- Now
  lg->SetReversePaths();
  
  // clean up
  while (logos_edge_list.size()) logos_edge_list.pop();
  return lg;
}

// for aggregator 
LogosGraph * Logos::GetGraph( NodeID *LogicalNode )
{
  LogosGraph *lg = new LogosGraph(LogicalNode);
  list <node> child_border_nodes;
  logos_node *ln=0;
  node n;
  dic_item di;
  node LogicalSelf = 0;
  const u_char parent_level = LogicalNode->GetLevel();
  const u_char child_level  = LogicalNode->GetChildLevel();

  Clear();
  Update(Logos::aggregation);
  // copy all the nodes at the parent and child level
  forall_nodes(n, _graph) {
    ln = _graph.inf(n);
    const NodeID *nid = ln->ShareNID();
    const u_char level = nid->GetLevel();
    
    if ( level == parent_level || level == child_level) {
     node nls = lg->insert_logosnode_copy(ln);
     if (same_as(LogicalNode, nid) == 0) LogicalSelf = nls;
    }
  }
  
  //
  // for each node copied, copy all edges that terminate on an extracted node
  //
  list<node> base = lg->_graph.all_nodes();
  list<logos_edge *> logos_edge_list;
  list_item itnode;

  forall_items(itnode, base) {
    n = base.inf(itnode);

    // for each node in the new graph
    ln = lg->_graph.inf(n);

    // find the node in the Logos _graph
    // we just copied it, it MUST be here
    
    Transit *tr = ln->ShareTransit();  // live pointer don't delete
    di = _Transit_to_node.lookup(tr);
    node Logos_n = _Transit_to_node.inf(di);

    // loop on edges of the node 
    edge e;
    list_item itedge;
    list<edge> edge_list = _graph.out_edges(Logos_n);
    forall_items(itedge, edge_list) {
      e = edge_list.inf(itedge);
      logos_edge *Logos_edge = _graph[e];

      // copy any edges that start and in in our new LogosGraph
      // if not previously inserted in new LogosGraph 
      if ( logos_edge_list.search(Logos_edge) == 0 ) { 
	logos_node *start = Logos_edge->GetStart(); // Shared  Do NOT delete
	logos_node *target= Logos_edge->GetEnd();   // Shared  Do NOT delete

	Transit *start_tr =  start->ShareTransit();
	Transit *end_tr   = target->ShareTransit();
	node start_node=0, end_node=0;
	logos_node *start_logos_node=0, *end_logos_node=0;

	// check the start node in LogosGraph lg
	int start_level = start_tr->GetLevel();
	start_node = demand_node ( lg, start_tr, false );
	if (start_node != 0) start_logos_node = lg->_graph[start_node];
	else continue;
	
	// check the end node in LogosGraph
	int end_level = end_tr->GetLevel();
	end_node = demand_node ( lg, end_tr, false );
	if (end_node) end_logos_node = lg->_graph[end_node];
	else continue;
	
	// see if this edge makes starting node a border node - uplinks only
	if ( (start_level == child_level) && (end_level != child_level)) {
	    start_logos_node->border_node(true);
	    child_border_nodes.insert(start_node);
	}

	// create the new edge if both ends are in our LogosGraph
	if (start_logos_node != 0 && end_logos_node != 0) {
      
	  // copy the port and attr_metric information
	  int port        = Logos_edge->Getport();
	  int remote_port = Logos_edge->GetRemotePort();
	  int aggtok      = Logos_edge->GetAggTok(); 
	  attr_metrics *a = new attr_metrics(*Logos_edge->GetMetric());
	
	  logos_edge *newlogosedge =
	    new logos_edge(start_logos_node,
			   end_logos_node, a, port, remote_port);
	  newlogosedge->AggTok(aggtok);
	  lg->_graph.new_edge(start_node, end_node, newlogosedge);
	  logos_edge_list.push(Logos_edge);
	  delete a;
	}
      }
    }
  }
  //   at this point we have a fairly simple graph, for example:
  //
  //     88:80:LogicalNode ==== 88:*:LogicalNode_Peers
  //                                   //
  //                                  // uplinks to peers
  //                                 //
  //     80:*:children_of_LogicalNode (connected by links)

  //
  // map the children into logical ports on LogicalNode
  //
  edge e;

  if ( LogicalSelf != 0 ) { // or should we assert( LogicalSelf != 0 );
    // check edges out of the Leader 
    forall_out_edges(e, LogicalSelf) {
      logos_edge * le = lg->_graph[e];
      int logical_port = le->Getport();
      int AggTok       = le->GetAggTok();
      const NodeID * Remote  = le->GetEnd()->ShareNID();
      // look for children with a matching edge
      list_item cit;
      forall_items(cit, child_border_nodes) {
	node cn = child_border_nodes.inf(cit);
	edge ce;
	forall_out_edges(ce, cn) {
	  logos_edge * candidate = lg->_graph[ce];
	  if (candidate->GetAggTok() == AggTok &&
	      same_as(candidate->GetEnd()->ShareNID(), Remote) == 0) {
	    // found one - there can be more
	    logos_node * child_border = lg->_graph[cn];
	    child_border->LogicalPort(logical_port);
	    bool was_inserted =
	      lg->insert_logical_port(logical_port, child_border->ShareNID());
	    DIAG("sim.logos-aggregator", DIAG_DEBUG, if (was_inserted) {
		 cout << "Logical Node:" << *LogicalNode <<" declares "<< endl;
		 cout << "        Node:" << *child_border->ShareNID()
		      << " is port " << logical_port << endl; } );
	  }
	}
      }
    }
    // check edges into the Leader
    forall_in_edges(e, LogicalSelf) {
      logos_edge * le  = lg->_graph[e];
      int logical_port = le->GetRemotePort();
      int AggTok       = le->GetAggTok();
      const NodeID * Remote  = le->GetStart()->ShareNID();
      // look for children with a matching edge
      list_item cit;
      forall_items(cit, child_border_nodes) {
	node cn = child_border_nodes.inf(cit);
	edge ce;
	forall_out_edges(ce, cn) {
	  logos_edge * candidate = lg->_graph[ce];
	  if (candidate->GetAggTok() == AggTok &&
	      same_as(candidate->GetEnd()->ShareNID(), Remote) == 0) {
	    // found one - there can be more
	    logos_node * child_border = lg->_graph[cn];
	    child_border->LogicalPort(logical_port);
	    bool was_inserted =
	      lg->insert_logical_port(logical_port, child_border->ShareNID());
	    DIAG("sim.logos-aggregator", DIAG_DEBUG, if (was_inserted) {
		 cout << "Logical Node:" << *LogicalNode <<" declares "<< endl;
		 cout << "        Node:" << *child_border->ShareNID()
		      << " is port " << logical_port << endl; } );
	  }
	}
      }
    }
  }
  // clean up
  // remove all the nodes and their links at the upper level
  //
  list <node> all_nodes = lg->_graph.all_nodes();
  list <node> parent_and_peers;
  list_item itall;
  forall_items(itall, all_nodes) {
    n = all_nodes.inf(itall);
    logos_node * ln = lg->_graph[n];
    if (ln->ShareNID()->GetLevel() == parent_level) {
      parent_and_peers.insert(n);
    }
  }
  
  while (parent_and_peers.size()) {
    n = parent_and_peers.pop();
    while ( (e = n->first_in_edge()) ) {
      node from = lg->_graph.source(e);
      from->del_edge(e);
      n->del_edge(e);
      logos_edge *le = lg->_graph[e];
      delete le;
      lg->_graph.del_edge(e);
      delete e;
    }

    while ( (e = n->first_edge()) ) {
      node to = lg->_graph.target(e);
      to->del_edge(e);
      n->del_edge(e);
      logos_edge *le = lg->_graph[e];
      delete le;
      lg->_graph.del_edge(e);
      delete e;
    }
    
    lg->_graph.del_node(n);
  }

  while (logos_edge_list.size()) logos_edge_list.pop();
  return lg;
}

// is there an edge to some_neighbor
// used in the very special case that may never occur in
// our simulator -- requies routing at a border node with a logical port id
// matching our own. but our code only includes ports if a bypass or
// exception is used in the route. Not likely in a double border (in/out).
bool Logos::I_am_adjacent (const NodeID *some_neighbor)
{
  node me;
  dic_item di;
  bool result = false;

  if ( (di = _nid_to_node.lookup(_LocNodeID)) != 0 ) {
    me = _nid_to_node.inf(di);
    edge e;
    forall_out_edges(e, me) {
      node remote = _graph.target(e);
      logos_node *ln = _graph[remote];
      const NodeID *remote_nodeid = ln->ShareNID();
      if ( same_as(remote_nodeid, some_neighbor) == 0 ) {
	result = true;
	break;
      }
    }
  }
  return result;
}

//
// find transit (NodeID,port) in the LogosGraph and return the node
// if routing then insert any missing transits and return the inserted node
// this is a private function used by GetGraph()
//
node Logos::demand_node ( LogosGraph *lg, Transit *tr, bool insert_missing )
{
  dic_item di;
  node the_node = 0;
  if ( (di = lg->_Transit_to_node.lookup(tr))==0) {

    if (insert_missing) {
      //
      // only insert simple nodes (port = 0)
      // and only if missing
      //
      Transit *simple = new Transit(tr->ShareNID(), 0);
      if ( (di = lg->_Transit_to_node.lookup(simple))==0) {
	di = _Transit_to_node.lookup(simple);
	node Lnode = _Transit_to_node.inf(di);
	logos_node *Lln = _graph[Lnode];

	the_node = lg->insert_logosnode_copy(Lln);

      } else {
	the_node = lg->_Transit_to_node.inf(di);
      }
      delete simple;
    }
    
  } else {
    the_node = lg->_Transit_to_node.inf(di);
  }
  return the_node;
}

// return hierarchical NodeID data that best matches nid
const u_char *Logos::MatchingLevel(const NodeID *nid)
{
  return MatchingLevel(nid->GetNID());
}

// return hierarchical NodeID data that best matches id (20 bytes)
const u_char *Logos::MatchingLevel(const u_char *id)
{
  const u_char * rval=0;
  int bits_same = bits_equal(_LocNodeID->GetNID()+2, id+2, 104);

  if (_levels.size() == 0) GetLevels();
  list_item li;
  forall_items(li, _levels) {
    NodeID * tmp = _levels.inf(li);
    int mylevel = tmp->GetLevel();
    if (bits_same >= mylevel) {
      rval = tmp->GetNID();
      break;
    }
  }
  return rval;
}

// BaseLogos has to ask
const NodeID * Logos::My_NodeID(void) 
{
  if (_LocNodeID)
    return new NodeID(*_LocNodeID);
  return (NodeID *)0;
}

//
// If a DTL arrives at a border entry node with an output port
// specified. Then the border entry node must route to the correct
// lower level border exit node. This routine finds the correct node.
//
// to do this the upper level horizontal link with the correct
// local port and the correct remote port are located. The aggragation
// token of this link is then used to find all uplinks with a matching
// aggregatioon token and with go to the correct upper leve node
//

list<NodeID *> *Logos::LogicalPortToBorder (const NodeID *ln, int lport,
					    const NodeID *upnode)
{
  list_item it;
  edge e;
  int aggtok;
  list <NodeID *> *rval = 0;
  
  // find the horizontal link representing this uplink

  forall_edges(e, _graph) {
    logos_edge * le = _graph[e];
    logos_node * start = le->GetStart();
    logos_node * end   = le->GetEnd();

    if ( (le->Getport() == lport)              &&
	 (same_as(end->ShareNID(), upnode) == 0) &&
	 (same_as(start->ShareNID(), ln) == 0)     )
      {
	aggtok = le->GetAggTok();
	break;
      }
  }

  // make a list of all uplinks to upnode with matching agg token
  
  forall_items(it, _borderlinks) {
    logos_edge * le = _borderlinks.inf(it);
    int letok = le->GetAggTok();
    if (letok == aggtok) {
      
      logos_node * upper = le->GetEnd();
      if ( same_as(upper->ShareNID(), upnode) == 0 ) {
	
	logos_node * lower = le->GetStart();
	if (rval == 0) rval = new list <NodeID *>;
	NodeID *nid = new NodeID(*lower->ShareNID());
	rval->append(nid);
      }
    }
  }
  return rval;
}

const char *Logos::LogosOwnerName() {return OwnerName();}
// condensed version of OwnerName() - up to first . from last .
// a.b.c.d become a.d - assumes that OwnerName is fixed.
const char * Logos::MyNickName() {
  if (_nickname == 0) {
    const char *owner = OwnerName();
    if (owner) {
      _nickname = new char[1 + strlen(owner)];
      char *first_dot = (char *)strchr (owner,'.');
      char *last_dot  = (char *)strrchr(owner,'.');
      int len;
      
      if (first_dot) {
	if (last_dot == first_dot) { /* only one dot */
	  strcpy(_nickname,owner);
	} else {
	  len =  (int)(first_dot - owner);
	  strncpy(_nickname, owner, len);
	  strcpy (_nickname+len, last_dot);
	}
    
      } else {
	strcpy(_nickname,owner);
      }
    }
  }
  return _nickname;
}

// return a copy of the origianl NodeID with trailing
// non significant bits zeroed.
// bit_zero is in codec/pnni_ig/id.h

NodeID *Logos_NodeID(const NodeID *nid)
{
  unsigned char new_id[22];
  bcopy(nid->GetNID(), new_id, 22);
  int start = (new_id[1]<104) ? new_id[1] : 104;
  bits_zero (new_id+2, start, 160);
  return new NodeID(&new_id[0]);
}

NodeID *Logos_NodeID(const unsigned char *id)
{
  unsigned char new_id[22];
  bcopy(id, new_id, 22);
  int start = (new_id[1]<104) ? new_id[1] : 104;
  bits_zero(new_id+2, start, 160);
  return new NodeID(&new_id[0]);
}

fw_Interface * Logos::QueryInterface( const char * s ) const
{
  if ( _omni && !strcasecmp( s, "Database" ) ) {
    return theOmni().LogosInterface( );
  } else // use 'global' QueryInterface
    return Shareable::QueryInterface( s );
}
