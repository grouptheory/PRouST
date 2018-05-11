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
static char const rcsid[] =
"$Id: ACAC.cc,v 1.231 1999/03/05 16:45:40 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "ACAC_DIAG.h"
#include "ACAC.h"
#include "ACACPolicy.h"
#include "ACACPolicyRegistry.h"
#include "call_identifier.h"
#include "pluggable_port_info.h"
#include "ACACInterfaces.h"

#include <DS/util/String.h>

#include <FW/basics/diag.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/SimEvent.h>

#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/next_hi_level_binding_info.h>
#include <codec/pnni_ig/nodal_hierarchy_list.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/pnni_pkt/ptsp.h>
#include <codec/q93b_msg/setup.h>
#include <codec/uni_ie/PNNI_crankback.h>
#include <codec/uni_ie/UNI40_td.h>
#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/called_party_num.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/PNNI_designated_transit_list.h>
#include <codec/uni_ie/bhli.h>

#include <fsm/config/Configurator.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/forwarder/ForwarderInterfaces.h>
#include <fsm/visitors/BorderVisitor.h>
#include <fsm/visitors/DBVisitors.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/FileIOVisitors.h>
#include <fsm/visitors/LGNVisitors.h>
#include <fsm/visitors/NPFloodVisitor.h>
#include <fsm/visitors/PortDownVisitor.h>
#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/visitors/VPVCRebindingVisitor.h>
#include <fsm/netstats/NetStatsCollector.h>

#include <sim/router/CREFEvent.h>
#include <sim/switch/FateVisitor.h>

#include <fstream.h>
#include <iostream.h>
#include <string.h>

#define  CREF_FLAG  0x800000

//
// 22 bytes of zeros.  Use this when you need a NodeID that's all zero.
//
const u_char ZERO_NODE_ID[22] = {0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00};

// ------------------ CAC Routing FSM ----------------
//
// Side A of the ACAC is connected to the B side of the control port.
//
// Side B of the ACAC is connected to the A side of the GCAC.
//
const VisitorType * ACAC::_border_down_type = 0;
const VisitorType * ACAC::_border_up_type = 0;
const VisitorType * ACAC::_db_intro_type = 0;
const VisitorType * ACAC::_fast_uni_type = 0;
const VisitorType * ACAC::_npstate_type  = 0;
const VisitorType * ACAC::_port_up_type  = 0;
const VisitorType * ACAC::_port_down_type  = 0;
const VisitorType * ACAC::_save_type  = 0;
const VisitorType * ACAC::_show_type  = 0;
const VisitorType * ACAC::_stream_save_type  = 0;

//
// The identifier for each hlink and uplink ig is the number of
// the port plus three.  See also fsm/database/DBInit.cc.
//
#define INT_TO_PORT_ID(x) ((x) + 3)

//
// A port number uniquely names a port in a switch.
// ACAC has an array of ports, _ports.  Port number zero
// is reserved for the control port.  ACAC does not keep track
// of the control port although it does have a space for it in
// the _ports array.
//

//
// The is the ACAC, the Actual Call Admission Control.  It tells
// the database about changes in the links and nodes that this switch
// is connected to.  It decides whether the switch can admit a
// specific call or not, based on the availability of resources.
//
// ACAC has _Calls, a mapping from a call reference number to a
// pointer to an ordered triple <input port, output port, traffic
// descriptor *>.  Whenever the ACAC admits a new call, it updates
// _Calls.  Whenever ACAC sees a Release, it retrieves the
// corresponding triple from Calls and frees the resources reserved
// for it on the input port and the output port.
//
// ACAC has _Nodes, a mapping from NodeID to List of output ports.
// When ACAC is asked to admit a new call, it retrieves the list of
// output ports for the destination NodeID from Nodes and asks each
// port in turn if it can support the call until either it finds one
// that can or all ports have said "no".
//
// Copies myNode.  Uses plugin_prefix only in the constructor.  The
// caller must delete plugin_prefix.
//
ACAC::ACAC(NodeID *myNode, int numPorts, double refreshInterval, 
	   ds_String *pluginPrefix) :
  _numPorts(numPorts), _myNode(0), _myAddr(0),
  _ports(0), _wakeup(0), 
  _iAmTheLeader(0)
{
  assert(myNode != 0);
  assert(_numPorts > 1);
  assert(pluginPrefix != 0);

  if (_border_down_type == 0)
    _border_down_type = QueryRegistry(BORDER_DOWN_VISITOR_NAME);

  if (_border_up_type == 0)
    _border_up_type = QueryRegistry(BORDER_UP_VISITOR_NAME);

  if (_db_intro_type == 0)
    _db_intro_type = QueryRegistry(DB_INTRO_VISITOR_NAME);

  if (_fast_uni_type == 0)
    _fast_uni_type = QueryRegistry(FAST_UNI_VISITOR_NAME);

  if (_npstate_type == 0)
    _npstate_type = QueryRegistry(NPSTATE_VISITOR_NAME);

  if (_port_up_type == 0)
    _port_up_type = QueryRegistry(PORT_UP_VISITOR_NAME);

  if (_port_down_type == 0)
    _port_down_type = QueryRegistry(PORT_DOWN_VISITOR_NAME);

  if (_save_type == 0)
    _save_type = QueryRegistry(SAVE_VISITOR_NAME);

  if (_show_type == 0)
    _show_type = QueryRegistry(SHOW_VISITOR_NAME);

  if (_stream_save_type == 0)
    _stream_save_type = QueryRegistry(STREAM_SAVE_VISITOR_NAME);

  _myNode = new NodeID(*myNode);
  _myPeerGroup = _myNode->GetPeerGroup();
  _myAddr = _myNode->GetAddr();	// returns a copy.

  _sharedACACPolicy = AllocateACACPolicy(pluginPrefix->chars());

  //
  // Create and populate the array of pluggable_port_info.
  // _ports[0] is the control port.
  //
  // _ports[1] through ports[_numPorts - 1] are the data ports.
  //

  _ports = new pluggable_port_info * [_numPorts];

  for (int i = 0; i < _numPorts; i++) {
    //
    // Initially, each port is down.  Its type is unspecified.
    // Its Node Peer FSM is in NonFullState.  Its ID is some
    // perversion of its port number.  Its remote port is zero.
    // Its remote NodeID and aggregation token are each zero.
    // 
    _ports[i] = new pluggable_port_info(_sharedACACPolicy,
					false, pluggable_port_info::unspecified, 
					NPStateVisitor::NonFullState, 
					INT_TO_PORT_ID(i), i, 0, 0, 0, 0);
  }

  pluggable_port_info *port_zero = _ports[0];
  port_zero->LocalNode(_myNode);
  port_zero->NPState(NPStateVisitor::FullState);
  port_zero->Up(true);

  //
  // To preserve the notion that _Nodes knows where all ports lead,
  // seed _Nodes with _myNode.  If you want to connect to _myNode, 
  // you have to leave on port zero.
  //
  int_list *port_zero_nodes = new int_list();
  port_zero_nodes->push(0);
  _Nodes.insert(_myNode->copy(), port_zero_nodes);

    //
    // If there is a database, then we have to wake up as often as it
    // wants us to.
    //
  if (refreshInterval > 0.0) {
    //
    // _wakeup instructs ACAC to reoriginate PTSEs
    //
    //    OriginateNodalIG();
    _wakeup = new SimEvent(this, this, ACAC_WAKEUP_EVENT);
    Deliver(_wakeup, refreshInterval);
  }

  AddPermission( "*", new ACACDefaultInterface(this) );
}


ACAC::~ACAC(void)
{
  dic_item ditem = 0;
  int i = 0;
  seq_item sitem = 0;

  delete _myNode;
  delete _myAddr;
  delete _myPeerGroup;
  delete _wakeup;

  //
  // Clear _ports
  //
  for (i = 0; i < _numPorts; i++) 
    delete _ports[i];
  delete [] _ports;

  delete _sharedACACPolicy;

  //
  // Clear _Nodes
  //
  forall_items(ditem, _Nodes) {
    delete _Nodes.key( ditem );
    delete _Nodes.inf( ditem );
  }
  _Nodes.clear();

  _Ignore.clear();

  list_item li;
  forall_items(li, _SVCCs) {
    delete _SVCCs.inf(li);
  }
  _SVCCs.clear();
}


//
// When a port comes up, return all of its resources if they haven't
// already been returned, then fetch the resources from NodalState.
// Tell the Database that there is a new HLINK.  Add the remote NodeID
// and port to Nodes.
//
// BUG: Doesn't release old calls
// BUG: doesn't really reclaim resources
// BUG: Permits the insertion of two RAIGs whose service category
// flags are identical but for the GCAC_CLP bit.  Any setup that
// requires one of those service categories will use the first RAIG
// that ACAC finds but there is no telling which one that will be.
//
void ACAC::PortUp(u_int local_port, u_int remote_port, 
			   NodeID *remote_node, int aggregation_token,
			   list<ig_resrc_avail_info *> *theRAIGs)
{
  dic_item d_item = 0;
  int_list *the_ports = 0;
  list_item l_item = 0;
  NPFloodVisitor *ptspv = 0;
  pluggable_port_info * port = _ports[local_port];
  bool this_port_must_be_full = false;

  // Report that we have reached full state only if we are not already full
  if (port->NPState() != NPStateVisitor::FullState) {
    DIAG(SIM_ACAC_PORT_UP, DIAG_INFO, 
	 cout << "ACAC " << *_myNode << " PORT " << local_port 
	 << " UP --->" << endl
	 << *remote_node << " port " << remote_port << " at time " 
	 << theKernel().CurrentElapsedTime() << endl);
  }
  //
  // Insert the local port into _Nodes.
  //
  // Usual case: There is already an entry for remote_node.
  //
  if ((d_item = _Nodes.lookup(remote_node)) != 0) {
    the_ports = _Nodes.inf(d_item);
    l_item = the_ports->search(local_port);
    pluggable_port_info *first_port = _ports[the_ports->head()];

    //
    // If the first port in the list is in FullState then
    // this one must be in FullState, too.
    //
    if (first_port != 0) {
      if (first_port->NPState() == NPStateVisitor::FullState) 
	this_port_must_be_full = true;
    }

    if (l_item == 0) the_ports->push(local_port);
  } else {
    //
    // Make an entry for remote_node
    //
    the_ports = new int_list();
    the_ports->push(local_port);
    _Nodes.insert(remote_node->copy(), the_ports);
  }
  port->AggregationToken(aggregation_token);
  port->PortType(pluggable_port_info::horizontal_link);

  port->LocalNode(_myNode);
  port->RemoteNode(remote_node);
  port->RemotePort(remote_port);
  port->Up(true);

  //
  // Tell the port about its the RAIGs.
  // We own the contents of theRAIGs but we don't own theRAIGs itself.
  //
  if (theRAIGs != 0) {
    ig_resrc_avail_info *aRAIG = 0;

    while (theRAIGs->size() > 0) {
      diag(SIM_ACAC_PORT_UP, DIAG_DEBUG, "%s: ", OwnerName());
      aRAIG = theRAIGs->pop();
      port->RAIG(aRAIG);
      delete aRAIG;
    }
  }
  
  //
  // If the other ports that connect to remote_node are in FullState,
  // then this one must be in FullState, too.  Make it so.
  //
  if (this_port_must_be_full) PortFull(local_port);
}


//
// When a port goes down, return all of its resources to NodalState,
// remove its calls from Calls, and remove its RemoteNodeID from
// Nodes.  Tell the Database that the HLINK is down.
//
// BUG: Doesn't release old calls
// BUG: Doesn't really reclaim resources
//
void ACAC::PortDown(u_int local_port)
{
  dic_item d_item = 0;
  int_list *the_ports = 0;
  list_item l_item = 0;
  NodeID *old_node = 0;

  assert(local_port >= 0);	/* If it's < 0, then we can't index
				 * _ports[] correctly.  All manner
				 * of weirdness will ensue.
				 */

  pluggable_port_info *port = _ports[local_port];

  DIAG(SIM_ACAC_PORT_DOWN, DIAG_INFO, 
       cout << "ACAC " << *_myNode << " PORT " << local_port 
       << " DOWN " << endl 
       << " at time " << theKernel().CurrentElapsedTime() << endl);

  Release(local_port);
  port->Up(false);


  //
  // Remove the old entry from _Nodes.
  //
  if ((old_node = port->RemoteNode()) != 0) {
    if ((d_item = _Nodes.lookup(old_node)) != 0) {
      the_ports = _Nodes.inf(d_item);
      l_item = the_ports->search(local_port);
      if (l_item != 0) the_ports->del_item(l_item);
    }
  }

  if (port->NPState() != NPStateVisitor::NonFullState)
    PortNonFull(local_port);
}


//
// Do the same as PortUp wrt NodalState?
//
// The link to another peer group is up.  We'll see a BorderUpVisitor
// every time the link goes up and every time the ULIA changes.
// Whenever we see the BorderUpVisitor, we pull out everything but the
// RAIGs and set them in one of our pluggable_port_info objects.  If
// that pluggable_port_info object doesn't have any RAIGs then we set
// them in it too.  Otherwise, we ignore the RAIGs in the
// BorderUpVisitor.
//
void ACAC::BorderUp(BorderUpVisitor *buv)
{
  dic_item d_item = 0;
  int_list *the_ports = 0;
  list_item l_item = 0;
  NPFloodVisitor *ptspv = 0;
  u_int local_port = buv->GetLocalPort();

  assert(local_port >= 0);	/* If it's < 0, then we can't index
				 * _ports[] correctly.  All manner
				 * of weirdness will ensue.
				 */

  pluggable_port_info * port = _ports[local_port];
  bool this_port_must_be_full = false;
  u_int remote_port = buv->GetRemotePort();
  const NodeID *remote_node = buv->GetUpNodeID();
  const PeerID *peer_group = buv->GetCPGID();
  int aggregation_token = buv->GetAgg();
  // This steals the RAIGs from the Visitor, so be sure to delete it
  list<ig_resrc_avail_info *> *theRAIGs = buv->GetRAIGs();
  const ig_uplink_info_attr *theULIA = buv->GetULIA();

  // Report that we have reached full state only if we are not already full
  if (port->NPState() != NPStateVisitor::FullState) {
    DIAG(SIM_ACAC_BORDER_UP, DIAG_INFO, 
	 cout << "ACAC " << *_myNode << " BORDER " << local_port 
	 << " UP --->" << endl 
	 << *remote_node << " port " << remote_port << " at time " 
	 << theKernel().CurrentElapsedTime() << endl);
  }

  //
  // Insert the local port into _Nodes.
  //
  // Usual case: There is already an entry for remote_node.
  //
  if ((d_item = _Nodes.lookup(remote_node)) != 0) {
    the_ports = _Nodes.inf(d_item);
    l_item = the_ports->search(local_port);
    pluggable_port_info * first_port = 0;
    if (the_ports->empty() == false)
      first_port = _ports[the_ports->head()];
    
    //
    // If the first port in the list is in FullState then
    // this one must be in FullState, too.
    //
    if (first_port != 0) {
      if (first_port->NPState() == NPStateVisitor::FullState) 
	this_port_must_be_full = true;
    }
    
    if (l_item == 0) the_ports->push(local_port);
  } else {
    //
    // Make an entry for remote_node
    //
    the_ports = new int_list();
    the_ports->push(local_port); 
    _Nodes.insert(remote_node->copy(), the_ports);
  }
  
  // Set the RAIGs for this port if we don't have them already.
  if (port->ShareAllRAIGs().size() == 0) {
    if (theRAIGs != 0) {
      while (theRAIGs->size() > 0) {
	ig_resrc_avail_info *aRAIG = theRAIGs->pop();
	port->RAIG(aRAIG);
	delete aRAIG;
      }
      delete theRAIGs;
    }
  }
  
  //
  // Remember the ULIA for this port.
  //
  port->ULIA(theULIA);
  
  port->LocalNode(_myNode);
  port->AggregationToken(aggregation_token);
  port->PortType(pluggable_port_info::uplink);
  port->CommonPeerGroup(peer_group);
  port->RemoteNode(remote_node);
  port->RemotePort(remote_port);
  port->Up(true);
  
  //
  // Pretend we've already received an NPStateVisitor of type
  // PortFull.  We won't actually receive it until after the SVC is
  // totally set up.
  //
  PortFull(local_port);
}


void ACAC::BorderDown(u_int local_port)
{
  dic_item d_item = 0;
  int_list *the_ports = 0;
  list_item l_item = 0;
  NodeID *old_node = 0;

  assert(local_port >= 0);	/* If it's < 0, then we can't index
				 * _ports[] correctly.  All manner
				 * of weirdness will ensue.
				 */

  pluggable_port_info *port = _ports[local_port];

  DIAG(SIM_ACAC_BORDER_DOWN, DIAG_INFO, 
       cout << "ACAC " << *_myNode << " BORDER " << local_port 
       << " DOWN" << endl
       << " at time " << theKernel().CurrentElapsedTime() << endl);

  // TEMPORARY CODE
  //  abort();

  Release(local_port);
  port->Up(false);


  //
  // Remove the old entry from _Nodes.
  //
  if ((old_node = port->RemoteNode()) != 0) {
    if ((d_item = _Nodes.lookup(old_node)) != 0) {
      the_ports = _Nodes.inf(d_item);
      l_item = the_ports->search(local_port);
      
      if (l_item != 0) {
	the_ports->del_item(l_item);
      }
    }
  }
  
  if (port->NPState() != NPStateVisitor::NonFullState) // ### Bilal ###
    PortNonFull(local_port);
}


//
// Mark the transition of the NodePeer FSM from NonFullState to FullState.
// ACAC can originate and reoriginate {H,Up}Link PTSE's when the NodePeer
// FSM is in FullState.
//
void ACAC::PortFull(u_int local_port)
{
  NPFloodVisitor *ptspv = 0;

  assert(local_port >= 0);	/* If it's < 0, then we can't index
				 * _ports[] correctly.  All manner
				 * of weirdness will ensue.
				 */
  int lifetime = -1;		// Lifetime of a ptse in seconds.
  pluggable_port_info * port = _ports[local_port];
  ig_ptse * ptse = 0;
  ig_horizontal_links *hlink = 0;
  ig_uplinks * uplink = 0;
  ig_uplink_info_attr *theULIA = 0;

  port->NPState(NPStateVisitor::FullState);

  DIAG(SIM_ACAC_NODE_PEER, DIAG_DEBUG, 
       cout << OwnerName() << ": At time " 
       << theKernel().CurrentElapsedTime() << endl;
       cout << OwnerName()
       << ": Port " << local_port << " is in Full State." << endl; );

  port->NextSequenceNumber();

  DBACACInterface *db = (DBACACInterface *)QueryInterface("Database");
  assert (db != 0);


  db->Reference();
  assert(db->good());

  lifetime = db->PTSELifetime();

  db->Unreference();

  if (lifetime > 0) {
    dic_item ditem;
    dictionary<u_int, ig_resrc_avail_info *> theRAIGs;
    ig_resrc_avail_info *aRAIG = 0;
    PTSPPkt * ptsp = 0;

    switch (port->PortType()) {
    case pluggable_port_info::horizontal_link:
      hlink = new ig_horizontal_links(0, port->RemoteNode(), 
				      port->RemotePort(), local_port, 
				      port->AggregationToken());

      DIAG(SIM_ACAC_IG_HLINK, DIAG_DEBUG, 
	   cout << OwnerName() << ": originating HLINK from " << endl
	   << *_myNode << " port " << local_port << " to " << endl
	   << *(port->RemoteNode()) << " port " << port->RemotePort()
	   << " with SN " << port->SequenceNumber()
	   << endl;)

	theRAIGs = port->ShareAllRAIGs();

	//
	// Make a copy of the port's list of RAIGs.
	// Put that copy into the hlink.
	//
	forall_items(ditem, theRAIGs) {
	  aRAIG = theRAIGs.inf(ditem);

	  hlink->AddRAIG((ig_resrc_avail_info *)aRAIG->copy());
	}

	ptse = new ig_ptse(InfoGroup::ig_horizontal_links_id,
			   port->ID(), 
			   port->SequenceNumber(), 0, 
			   lifetime);
	ptse->AddIG(hlink);
	ptsp = new PTSPPkt(_myNode->GetNID(), _myPeerGroup->GetPGID());
	ptsp->AddPTSE(ptse);

	break;

    case pluggable_port_info::uplink:
      //
      // Whose ATM address?  local or remote?
      //
      uplink = new ig_uplinks(0, port->RemoteNode(), port->CommonPeerGroup(),
			      local_port, port->AggregationToken(),
			      port->RemoteAddress());

      DIAG(SIM_ACAC_IG_UPLINK, DIAG_DEBUG, 
	   cout << OwnerName() << ": originating UPLINK from "  << endl
	   << *_myNode << " port " << local_port << " to " << endl
	   << *(port->RemoteNode()) << " port " << port->RemotePort()
	   << " for peer group " << endl
	   << *(port->CommonPeerGroup())
	   << " with SN " << port->SequenceNumber()
	   << endl;)

	theRAIGs = port->ShareAllRAIGs();

	//
	// Make a copy of the port's list of RAIGs.
	// Put that copy into the uplink.
	//
	forall_items(ditem, theRAIGs) {
	  aRAIG = theRAIGs.inf(ditem);

	  uplink->AddIG(aRAIG->copy());
	}

	theULIA = port->ULIA();

	if (theULIA != 0) 
	  uplink->AddIG(new ig_uplink_info_attr(*theULIA));

	ptse = new ig_ptse(InfoGroup::ig_uplinks_id, 
			   port->ID(), 
			   port->SequenceNumber(), 0, 
			   lifetime);
	ptse->AddIG(uplink);

	ptsp = new PTSPPkt(_myNode->GetNID(), 
			   port->CommonPeerGroup()->GetPGID());
	ptsp->AddPTSE(ptse);
	break;

    case pluggable_port_info:: unspecified:
      break;

    default:
      break;
    }

    if (ptsp != 0) {
      //
      // Send the PTSP to the database.
      // This tells the database that the HLINK is up.
      //
      ptspv = new NPFloodVisitor(ptsp, _myNode, _myNode);
      PassVisitorToB(ptspv);
    }
  }
}

//
// Mark the transition of the NodePeer FSM from FullState to
// NonFullState.  ACAC must expire a {H,Up}Link PTSE's when the
// NodePeer FSM is in NonFullState.
//
void ACAC::PortNonFull(u_int local_port)
{
  assert(local_port >= 0);	/* If it's < 0, then we can't index
				 * _ports[] correctly.  All manner
				 * of weirdness will ensue.
				 */

  pluggable_port_info *port = _ports[local_port];
  ig_ptse * ptse = 0;
  ig_ptse * old_ptse = 0;

  DIAG(SIM_ACAC_NODE_PEER, DIAG_DEBUG, 
       cout << OwnerName() << ": At time " 
       << theKernel().CurrentElapsedTime() << endl;
       cout << OwnerName() << ": Port " << local_port 
       << " is in NonFullState." << endl; );

  port->NPState(NPStateVisitor::NonFullState);

  //  port->NextSequenceNumber();
  DBACACInterface *db = (DBACACInterface *)QueryInterface("Database");
  assert (db != 0);
  db->Reference();

  assert(db->good());

  switch (port->PortType()) {
  case pluggable_port_info::horizontal_link:
    //
    // Remove the old PTSE from the database.  This tells the Database
    // that the HLINK is down.
    //
    ptse = new ig_ptse(InfoGroup::ig_horizontal_links_id, 
		       port->ID(), port->SequenceNumber(), 0, 
		       DBKey::ExpiredAge);

    //
    // We must not UnReference() old_ptse lest we give
    // db a dangling pointer.
    //
    assert(db->good());
    old_ptse = db->ReqPTSE(_myNode, ptse);
      
    DIAG(SIM_ACAC_IG_HLINK, DIAG_DEBUG, 
	 cout << OwnerName() 
	 << " PortNonFull() EXPIRING ptse HLINK ";
	 if (old_ptse != 0) cout << *old_ptse << endl;
	 else cout << "0" << endl; )
      
      if (old_ptse != 0) {
	assert(db->good());
	bool expire_worked = db->Expire(old_ptse);
	assert(expire_worked);
      }

      ptse->UnReference();
      break;

  case pluggable_port_info::uplink:
    ptse = new ig_ptse(InfoGroup::ig_uplinks_id, 
		       port->ID(), port->SequenceNumber(), 0, 
		       DBKey::ExpiredAge);

    //
    // We must not UnReference() old_ptse lest we give
    // db a dangling pointer.
    //
    assert(db->good());
    old_ptse = db->ReqPTSE(_myNode, ptse);

    DIAG(SIM_ACAC_IG_UPLINK, DIAG_DEBUG, 
	 cout << OwnerName() 
	 << " PortNonFull() EXPIRING ptse UPLINK "; 
	 if (old_ptse != 0) cout << *old_ptse << endl;
	 else cout << "0" << endl; )

      if (old_ptse != 0) {
	assert(db->good());
	bool expire_worked = db->Expire(old_ptse);
	assert(expire_worked);
      }

      ptse->UnReference();
      break;

  case pluggable_port_info:: unspecified:
    break;

  default:
    break;
  }

  db->Unreference();
}

/*
 * When an node peer goes into FullState, every port
 * that connects to the remote node is "Full".
 */
void ACAC::NodeFull(const NodeID * remote_node)
{
  dic_item d_item = 0;
  int_list *the_ports = 0;
  list_item l_item = 0;

  if (remote_node != 0) {
    DIAG(SIM_ACAC_NODE_PEER, DIAG_INFO, 
	 cout << OwnerName() << ": At time " 
	 << theKernel().CurrentElapsedTime() << endl;
	 cout << OwnerName() << ": FullState to remote node "
	 << endl << *remote_node << endl;)

      if ((d_item = _Nodes.lookup(remote_node)) != 0) {
	the_ports = _Nodes.inf(d_item);
	forall_items(l_item, *the_ports) {
	  PortFull(the_ports->inf(l_item));
	}
      }
  } else {
    DIAG(SIM_ACAC_NODE_PEER, DIAG_INFO, 
	 cout << OwnerName() << ": At time " 
	 << theKernel().CurrentElapsedTime() << endl;
	 cout << OwnerName() << ": FullState to remote node 0"
	 << endl;)
  }
}


/*
 * When an node peer goes into FullState, every port
 * that connects to the remote node is "Full".
 */
void ACAC::NodeNonFull(const NodeID *remote_node)
{
  dic_item d_item = 0;
  int_list *the_ports = 0;
  list_item l_item = 0;

  if (remote_node != 0) {
    DIAG(SIM_ACAC_NODE_PEER, DIAG_INFO, 
	 cout << OwnerName() << ": At time " 
	 << theKernel().CurrentElapsedTime() << endl;
	 cout << OwnerName() << ": NonFullState to remote node "
	 << *remote_node << endl;)

      if ((d_item = _Nodes.lookup(remote_node)) != 0) {
	the_ports = _Nodes.inf(d_item);
	forall_items(l_item, *the_ports) {
	  PortNonFull(the_ports->inf(l_item));
	}
      }
  } else {
    DIAG(SIM_ACAC_NODE_PEER, DIAG_INFO, 
	 cout << OwnerName() << ": At time " 
	 << theKernel().CurrentElapsedTime() << endl;
	 cout << OwnerName() << ": NonFullState to remote node 0"
	 << endl;)
      }
}


//
// GCAC, our B-side, told us to setup a call.  It gave us a route in
// the DTL.  Perform admission control.
//
void ACAC::SetupFromGCAC(FastUNIVisitor *fuv)
{
  //
  // This is a hack so that ACAC can remember if a call is for setting
  // up an SVCC that it already set up a call for.
  // processThisSetup is true when fuv doesn't represent an SVCC
  // setup *or* when fuv represents an SVCC setup to a remote node
  // that doesn't alreay have an SVCC going to it.  In the case of an
  // SVCC setup to a remote node that already has an SVCC going to it,
  // ACAC must PassThru(fuv) without allocating any bandwidth.
  //
  bool processThisSetup = true;

  const NodeID *remoteNodeID = fuv->GetDestNID();

  //
  // I originated the FastUNIVisitor if
  //
  // 1. I have a database and it tells me that fuv's source NodeID is
  // logically equal to my own and I am the Peer Group Leader.
  //
  // or
  //
  // 2. I don't have a database and fuv's source NodeID is non-zero
  // and it is exactly equal to my own NodeID.
  //
  bool iAmOriginator = false;
  DBACACInterface *db = (DBACACInterface *)QueryInterface("Database");
  assert(db != 0);

  db->Reference();
    
  assert(db->good());
  iAmOriginator = db->LogicalNodeIsMe(fuv->GetSourceNID());

  db->Unreference();

  if ( iAmOriginator ) {
    const generic_q93b_msg * msg = fuv->GetSharedMessage();
    ie_bhli * bhli = (ie_bhli *)msg->ie(InfoElem::ie_bhli_ix);

    char buf[32];
    if ( bhli != 0 ) {
      u_char * tmp = bhli->get_bhli_buffer();
      int id  = (tmp[0] << 24) & 0xFF000000;
          id |= (tmp[1] << 16) & 0xFF0000;
	  id |= (tmp[2] <<  8) & 0xFF00;
	  id |= (tmp[3]      ) & 0xFF;
      sprintf( buf, "%d", id );
    } else
      sprintf( buf, "0" );

    if ( fuv->IsForSVCC() )
      theNetStatsCollector().ReportNetEvent( "Network_Call_Submission",
					     OwnerName(),
					     buf,
					     // fuv->GetDestNID() ? fuv->GetDestNID()->Print() :
					     // "!UNKNOWN_CALLED_PARTY!",
					     _myNode);
  }

  //
  // If we've processed this FastUNIVisitor before, when it was a
  // FastUNIRelease then remove it from _Ignore.
  //
  dic_item di = _Ignore.lookup(fuv);
  if (di != 0) _Ignore.del_item(di);

  if (fuv->GetSourceNID()) {
    DIAG(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	 cout << "From: " << *(fuv->GetSourceNID()) << endl;); }

  if (fuv->GetDestNID()) {
    DIAG(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	 cout << "To:   " << *(fuv->GetDestNID()) << endl;); }
    
  if (fuv->IsForSVCC()) {
    int aggregationToken = fuv->GetAgg();
    DIAG(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG,
	 cout << "For SVCC with aggregation token " << aggregationToken
	 << endl; );

    //
    // If there is already an SVCC to remoteNodeID then don't bother
    // setting up another one.
    //
    if (remoteNodeID != 0) {
      if ((fuv->IsForSVCC()) && (_SVCCs.search(remoteNodeID) != 0)) {
	processThisSetup = false;
      }
    } else {
      processThisSetup = false;
    }
  }

  const DTLContainer * dtc = fuv->Peek();
  NodeID * thatsMe = dtc ? (NodeID *)dtc->GetNID() : 0;


  DIAG(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
       cout << "I am: " << *_myNode << endl;
       cout << "Top of DTL is ";
       if (thatsMe != 0) cout << *thatsMe << endl;
       else cout << "0" << endl; );
  if (thatsMe != 0) {
    if (*_myNode == *thatsMe) {
      diag(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, "Hey!  That's me!\n");
      thatsMe = (NodeID *)((fuv->Pop())->GetNID());
      delete thatsMe;
    }
  }
  thatsMe = 0;


  if (processThisSetup) {
    SimEvent *reoriginateEvent = 0;

      //
      // Choose the call reference if the call originated here.  That's
      // probably not the right answer, but it works.  We have to choose
      // a unique call reference so that the pluggable_port_info mappings are
      // consistent.
      //
      //      if (iAmOriginator) fuv->SetCREF(_ports[0]->NextCallReference());

    if (iAmOriginator) fuv->SetCREF(GetPortZeroCREF());

    ACACPolicy::CallStates callState = CallAdmissionControl(fuv);
    switch (callState) {
    case ACACPolicy::AcceptedSignificantChange:
      //
      // Schedule reorigination.  
      //
      // In the situation where a border node comes up, ACAC
      // originates the uplink IG, and the database sets up the
      // SVCC, ACAC will reoriginate the uplink IG.  This is bad if
      // the stack isn't broken because someone will be holding onto
      // a pointer to the original uplink IG but it will be spam;
      // dereferencing it will cause a SEGV or a SIGBUS.
      //
      // By scheduling an interruption to reorigination, ACAC
      // terminates the thread of execution that began with the
      // border up.  When the interruption occurs (immediately after
      // the stack unwinds) ACAC can safely reoriginate the uplink
      // IG.
      //

      reoriginateEvent = new SimEvent(this, this, ACAC_REORIGINATION_EVENT);
      Deliver(reoriginateEvent);

      // Allow this to fall through
    case ACACPolicy::Accepted:  // This wasn't a significant change
      {
	const generic_q93b_msg * msg = fuv->GetSharedMessage();
	ie_bhli * bhli = (ie_bhli *)msg->ie(InfoElem::ie_bhli_ix);
	
	char buf[32];
	if ( bhli != 0 ) {
	  u_char * tmp = bhli->get_bhli_buffer();
	  int id  = (tmp[0] << 24) & 0xFF000000;
          id |= (tmp[1] << 16) & 0xFF0000;
	  id |= (tmp[2] <<  8) & 0xFF00;
	  id |= (tmp[3]      ) & 0xFF;
	  sprintf( buf, "%d", id );
	} else
	  sprintf( buf, "0" );
	theNetStatsCollector().ReportNetEvent("Call_Admission",
					      OwnerName(),
					      buf, 
					      // (fuv->GetSourceNID() != 0 ? 
					      //  fuv->GetSourceNID()->Print()
					      // : "!UNKNOWN_CALLING_PARTY!"),
					      _myNode);
	//
	// Outbound port is set by CallAdmisionControl().
	// Outbound VPI and VCI are not yet set.
	//
	    
	//
	// Inform Routing Control about the call reference by setting
	// the return event's code to the call reference.
	//
	CREFEvent *ce = (CREFEvent *)fuv->GetReturnEvent();
	if (ce != 0) {
	  diag(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	       "%s: setting CREFEvent CREF to 0x%x\n",
	       OwnerName(), fuv->GetCREF());
	  ce->SetCREF(fuv->GetCREF());
	  Deliver(ce);
	}
	    
	//
	// If we're the calling party, we get to choose the vpi and vci
	// for our path through the control port.  DataForwarder will
	// choose the real vpi and vci.
	//
	// We always choose vpi=0.
	//
	// BUG: We have no way to know when to return the VCI to
	// _vc_oracle.
	//
	if (iAmOriginator) {

	  //
	  // When we're the calling party and the call is not for
	  // an SVCC, we might have to notify a simulation of the
	  // call reference of the new call.
	  //
	  if (!fuv->IsForSVCC()) {
	    FateVisitor * fv = new FateVisitor( fuv, fuv->GetCREF() );
	    PassVisitorToB( fv );
	  }

	  u_int oldVPI = fuv->GetInVP();
	  u_int oldVCI = fuv->GetInVC();
	  u_int newVPI = 0;
	  u_int newVCI = GetNewVCI();
	  if (newVCI < 0)
	    newVCI = 0;
	      
	  if (newVCI != 0) {
	    fuv->SetInVP(newVPI);
	    fuv->SetInVC(newVCI);
	    fuv->SetOutVP(newVPI); // Added 3/24/98 mountcas
	    fuv->SetOutVC(newVCI); // Added 3/24/98 mountcas
		
	    // Remember the cref, vpi, and vci so that we can
	    // reclaim them when this call is released.
		
	    assert(fuv->GetOutPort() >= 0);	/* If it's < 0, then
						 * we can't index _ports[] 
						 * correctly.  All manner 
						 * of weirdness will ensue.  
						 */
		
		
	    _ports[fuv->GetOutPort()]->SetVPVC(fuv->GetCREF(), 
					       newVPI, newVCI);
		
	    //
	    // We haven't made an SVCC to remoteNodeID already.  We'll do
	    // it this time.  Remember the call reference so we can remove
	    // the SVCC from _SVCCs when it's released.
	    //
	    if (fuv->IsForSVCC()) {
	      _SVCCs.append(remoteNodeID->copy());
		  
	      //
	      // If the old VPI and VCI of the FastUNIVisitor are not
	      // -1, then we know that we can rebind the VPVC
	      // mapping of the Logical SVCC instead of creating a new
	      // one.
	      //
	      // Tell the Logical Node muxes that we're changing the
	      // VPVC binding of one of its Logical SVCCs.
	      //
	      if ((oldVPI != -1) && (oldVCI != -1)) {
		VPVCRebindingVisitor *rebinder = 
		  new VPVCRebindingVisitor(fuv->GetOutPort(), 
					   fuv->GetAgg(),
					   MAKE_VPVC(oldVPI, oldVCI),
					   MAKE_VPVC(oldVPI, oldVCI),
					   MAKE_VPVC(newVPI, newVCI),
					   _myNode);
		rebinder->SetDestNID(remoteNodeID);
		    
		DIAG(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG,
		     cout << OwnerName() << ": At time " 
		     << theKernel().CurrentElapsedTime() << endl;
		     cout << OwnerName() 
		     << ": Rebinding SVCC for to "
		     << *remoteNodeID << " port= " 
		     << fuv->GetOutPort()
		     << " oldVPI= 0x" << hex << oldVPI
		     << " oldVCI= 0x" << hex << oldVCI
		     << " newVPI= 0x" << hex << newVPI
		     << " newVCI= 0x" << hex << newVCI
		     << dec
		     << endl; );
		    
		PassVisitorToA(rebinder);
	      }
	    }
	  }
	      
	  diag(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	       "%s: ORIGINATOR setting outVP to 0x%x, outVC to 0x%x\n",
	       OwnerName(), newVPI, newVCI);
	      
	  PassVisitorToA(fuv);	// to DataForwarder
	} else if (ItsForMe(fuv)) {
	  u_int in_vpi = fuv->GetInVP();
	  u_int in_vci = fuv->GetInVC();
	  int in_port = fuv->GetInPort();
	  const u_long cref = fuv->GetCREF();
	  const NodeID *source_node = fuv->GetSourceNID();
	  const NodeID *destination_node = fuv->GetDestNID();


	  DIAG(SIM_ACAC_CALL, DIAG_INFO, 
	       cout << OwnerName() << ": At time " 
	       << theKernel().CurrentElapsedTime() << endl 
	       << " " << *_myNode << endl << " accepted call from: ";

	       if (source_node != 0) cout << *source_node;
	       else cout << "ANONYMOUS CALLER";

	       cout << endl
	       << " on port " << in_port 
	       << " CREF= 0x" << hex << cref << dec 
	       << endl
	       << " vpi= " << in_vpi << " (0x" 
	       << hex << in_vpi << dec << ")"
	       << " vci= " << in_vci << " (0x" 
	       << hex << in_vci << dec << ")"
	       << endl;
	       if (ItsFromMe(fuv))
	       cout << endl << "I AM CALLING PARTY and CALLED PARTY" 
	       << endl << endl;); 

	  // call the NetStatsCollector with type Call_Admission
	  const generic_q93b_msg * msg = fuv->GetSharedMessage();
	  ie_bhli * bhli = (ie_bhli *)msg->ie(InfoElem::ie_bhli_ix);
	  
	  char buf[32];
	  if ( bhli != 0 ) {
	    u_char * tmp = bhli->get_bhli_buffer();
	    int id  = (tmp[0] << 24) & 0xFF000000;
	    id |= (tmp[1] << 16) & 0xFF0000;
	    id |= (tmp[2] <<  8) & 0xFF00;
	    id |= (tmp[3]      ) & 0xFF;
	    sprintf( buf, "%d", id );
	  } else
	    sprintf( buf, "0" );
	  theNetStatsCollector().ReportNetEvent("Call_Arrival",
						OwnerName(),
						buf,
						// (source_node != 0 ? 
						// source_node->Print() 
						// : "!UNKNOWN_CALLING_PARTY!"),
						_myNode);

	  //
	  // Send CONNECT to the calling party.  in-port = 0.
	  // FastUNIVisitor::Connect
	  //
	  FastUNIVisitor *connect = 
	    new FastUNIVisitor(destination_node, source_node,
			       0, MAKE_VPVC(in_vpi, in_vci),
			       fuv->GetAgg(), cref, 
			       FastUNIVisitor::FastUNIConnect);
	  connect->SetInVP(in_vpi);
	  connect->SetInVC(in_vci);
	  connect->SetInPort(0);
	  PassVisitorToA(connect);

	  //
	  // Send another CONNECT to the Aggregator.
	  //
	  FastUNIVisitor *connectForAggregator = 
	    new FastUNIVisitor(source_node, destination_node,
			       0, MAKE_VPVC(in_vpi, in_vci),
			       fuv->GetAgg(), cref, 
			       FastUNIVisitor::FastUNIConnect);

	  PassVisitorToB(connectForAggregator);
	  fuv->Suicide();
	} else {

	  diag(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	       "%s: NEITHER ORIGINATOR NOR DESTINATOIN leaving outVP \
as 0x%x, outVC as 0x%x\n",
	       OwnerName(), fuv->GetOutVP(), fuv->GetOutVC());
	  PassVisitorToA(fuv);	// to DataForwarder
	}
	    

      }
    break;
    case ACACPolicy::Rejected:
      {
	// A new VC could not be allocated, so
	// Bounce the Visitor back where it came from, 
	// with a Type of FastUNIRelease
	    
	DIAG(SIM_ACAC_CALL, DIAG_INFO, 
	     cout << OwnerName() 
	     << ":" << *_myNode
	     << " rejected call " );

	// call the NetStatsCollector with type Call_Rejection
	const generic_q93b_msg * msg = fuv->GetSharedMessage();
	ie_bhli * bhli = (ie_bhli *)msg->ie(InfoElem::ie_bhli_ix);
	
	char buf[32];
	if ( bhli != 0 ) {
	  u_char * tmp = bhli->get_bhli_buffer();
	  int id  = (tmp[0] << 24) & 0xFF000000;
	  id |= (tmp[1] << 16) & 0xFF0000;
	  id |= (tmp[2] <<  8) & 0xFF00;
	  id |= (tmp[3]      ) & 0xFF;
	  sprintf( buf, "%d", id );
	} else
	  sprintf( buf, "0" );
	theNetStatsCollector().ReportNetEvent("Call_Rejection",
					      OwnerName(),
					      buf,
					      // (fuv->GetSourceNID() != 0 ? 
					      //  fuv->GetSourceNID()->Print()
					      //  : "!UNKNOWN_CALLING_PARTY!"),
					      _myNode);
	    
	if (fuv->GetSourceNID()) {
	  DIAG(SIM_ACAC_CALL, DIAG_INFO, 
	       cout << "from: " << *(fuv->GetSourceNID()) << endl); };
	    
	// CallAdmissionControl() sets the type to
	// FastUNIVisitor::FastUNISetupFailure.
	//
	// Set the out-bound vpi, vci, and port to their
	// in-bound values so that the DataForwarder knows
	// where to send the Release.
	//
	fuv->SetOutVC( fuv->GetInVC() );	// Probably 0x05
	fuv->SetOutVP( fuv->GetInVP() );	// Probably 0x00
	fuv->SetOutPort(fuv->GetInPort());	// Could be any physical port
	fuv->SetInPort(0);		/* The DataForwarder needs to think
					 * that the release came from
					 * the Control port.  Otherwise,
					 * it will send the release back
					 * here.
					 */
	    
	// This sets fuv's return event to zero.
	CREFEvent *ce = (CREFEvent *)fuv->GetReturnEvent();
	    
	//
	// If there is a return event and a crankback,
	// pass the entire visitor back to RouteControl
	// for rerouting.
	//
	if ((ce != 0) && (fuv->GetCrankback() != 0)) {
	  diag(SIM_ACAC_CALL_RELEASE, DIAG_DEBUG, 
	       "%s Returning event and FastUNIVisitor (0x%x) "
	       "to RouteControl\n",
	       OwnerName(), fuv);
	      
	  fuv->SetReturnEvent(ce);	// Restore the return event
	  PassVisitorToB(fuv);	// to RouteControl
	} else {
	  if (ce != 0) {
	    //
	    // Inform Routing Control that the call setup failed by
	    // setting the return event's code to zero, and there is no
	    // crankback.
	    //
	    diag(SIM_ACAC_CALL_RELEASE, DIAG_DEBUG,
		 "%s returning event with code=0 to RouteControl\n",
		 OwnerName());
		
	    ce->SetCREF(0);
	    Deliver(ce);
	  }
	      
	  diag(SIM_ACAC_CALL_RELEASE, DIAG_DEBUG, 
	       "%s passing FastUNIVisitor to DataForwarder\n", 
	       OwnerName());
	      
	  //
	  // CallAdmissionControl already set the message type
	  // to FastUNIVisitor::FastUNISetupFailure and the
	  // generic_q93b_msg to a q93b_release_message.
	  //
	  // fuv->SetMSGType(FastUNIVisitor::FastUNISetupFailure);
	  PassVisitorToA(fuv); 	// to DataForwarder
	}
      }
    break;
    default:
      break;
    }
  } else PassThru(fuv);	/* ACAC already did something for this
			 * setup, so it need only pass it along.
			 * Somebody else will filter it out.
			 */
}


//
// Call Admission Control
//
// Look up the NodeID in Nodes to find the list of ports that are
// connected to it.
//
// Search the list for ports that can support the call's traffic descriptor.
//
// If there is such a port, 
//
//	reserve the resources at the input and output ports in the 
//	appropriate service class
//
//	put the call reference, traffic descriptior, input port, and output
//	port into Calls
//
//	stamp the outbound vp (0) and vc (5) onto the FastUNIVisitor.
//	
//	stamp the output port onto the FastUNIVisitor
//
//	return true
//
// Otherwise, set the cause and crankback in the FastUniVisitor, set the
// type of the FastUNIVisitor to FastUNISetupFailure, and return false.
//
ACACPolicy::CallStates ACAC::CallAdmissionControl(FastUNIVisitor *fuv)
{
  ACACPolicy::CallStates answer = ACACPolicy::Rejected;
  ACACPolicy::Direction failureDirection = ACACPolicy::Unknown;
  u_int outPort = 0;
  u_int inPort = fuv->GetInPort();
  pluggable_port_info *oport = 0;
  pluggable_port_info *iport = 0;
  u_int in_call_reference = 0;
  u_int out_call_reference = 0;
  u_int in_vpi = 0;
  u_int in_vci = 0;
  ie_cause *cause = 0;
  PNNI_crankback *crankback = 0;
  bool iAmOriginator = false;
  bool iAmDestination = false;

  DBACACInterface *db = (DBACACInterface *)QueryInterface("Database");
  assert (db != 0);

  db->Reference();
    
  assert(db->good());
  iAmOriginator = db->LogicalNodeIsMe(fuv->GetSourceNID());

  assert(db->good());
  iAmDestination = db->LogicalNodeIsMe(fuv->GetDestNID());

  db->Unreference();



  // The destination node is either the top of the DTL, _myNode, or
  // zero.  It's the top of the DTL when the DTL isn't empty.  It's
  // _myNode when the DTL is empty and iAmDestination is true.  It's
  // zero otherwise.
  NodeID * destNode = (NodeID *)(fuv->Peek() ? fuv->Peek()->GetNID() : 
				 (iAmDestination ? _myNode : 0));

  dic_item ditem = (destNode != 0) ? _Nodes.lookup(destNode) : 0;
  const u_char crankback_level = (destNode != 0) ? destNode->GetLevel() : 0;

  in_call_reference = fuv->GetCREF();

  assert(inPort >= 0);	/* The input port is -1 by default.
			 * If it's -1, then we can't index
			 * _ports[] correctly.  All manner
			 * of weirdness will ensue.
			 */

  //
  // No destNode in the cache?
  // Ask the database.  Put the information into the cache.
  //
  if (ditem == 0) {
    DIAG(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	 cout << OwnerName() << ": CallAdmissionControl(): My address is " 
	 << *_myNode << endl;

	 cout << "Visitor says Source= ";
	 if (fuv->GetSourceNID()) cout << *(fuv->GetSourceNID()) << endl;
	 else cout << "0" << endl;

	 cout << "Visitor says Destination= ";
	 if (fuv->GetDestNID())  cout << *(fuv->GetDestNID()) << endl;
	 else cout << "0" << endl;

	 cout << "DTL says Destination= ";
	 if (destNode != 0) {
	   cout << *destNode << endl;
	   cout << "Can't find Destination in the _Nodes.  HELP!" << endl;
	   cout << "Interrim version not looking in the database by design." 
		<< endl;
	 } else cout << "0" << endl;
	 );
  } 

  if (ditem != 0) {
    u_int service_category = 0;
    int_list *thePorts = _Nodes.inf(ditem);
    list_item litem = thePorts->first();
    u_int saved_vpi = fuv->GetSavedVP();
    u_int saved_vci = fuv->GetSavedVC();

    in_vpi = fuv->GetInVP();
    in_vci = fuv->GetInVC();

    //
    // No matter what the FastUNIVisitor claims, if we are the
    // calling party, then the input port must be zero.
    //
    if (*_myNode == *(fuv->GetSourceNID())) {
      fuv->SetInPort(0);
      inPort = 0;
    }

    DIAG(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	 cout << OwnerName() << ": At time " 
	 << theKernel().CurrentElapsedTime() << endl;
	 cout << OwnerName() 
	 <<": CallAdmissionControl(): attempting for inPort "
	 << inPort
	 << " inCREF 0x" << hex << in_call_reference << dec
	 << " inVPI " << in_vpi << " (0x" << hex << in_vpi << dec << ") "
	 << " inVCI " << in_vci << " (0x" << hex << in_vci << dec << ") "
	 << " savedVPI " << saved_vpi 
	 << " (0x" << hex << saved_vpi << dec << ") "
	 << " savedVCI " << saved_vci 
	 << " (0x" << hex << saved_vci << dec << ") "
	 << endl; );

    pluggable_port_info *iport = _ports[inPort];
    q93b_setup_message *setup = fuv->GetSetup();

    const NodeID *called_party = fuv->GetDestNID();

    //
    // Search thePorts for an output port that can support the call.
    //
    while (litem != 0) {
      outPort = thePorts->inf(litem);
      diag(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG,
	   "%s: inPort is %d outPort is %d.\n", 
	   OwnerName(), inPort, outPort);
      oport = _ports[outPort];


      if ((oport->Up()) && (oport->NPState() == NPStateVisitor::FullState)) {
	if ((answer  = 
	     oport->AllocateBandwidth(setup, inPort, outPort, 
				      in_call_reference, out_call_reference,
				      in_vpi, in_vci, 
				      iport,
				      crankback_level, 
				      called_party,
				      cause, crankback,
				      failureDirection)) != 0) {
	  litem = 0;	// Stops the loop.

	  //
	  // Shouldn't have to do this.
	  // In/Out VP and VC should already be set.
	  //
	  //	  fuv->SetInVP(0);
	  //	  fuv->SetInVC(5);
	  //	  fuv->SetOutVP(0);
	  //	  fuv->SetOutVC(5);

	  fuv->SetOutVP(-1);
	  fuv->SetOutVC(-1);

	  fuv->SetOutPort(outPort);
	  fuv->SetInPort(0);

	  if ((iAmOriginator) || (iAmDestination))
	    fuv->SetCREF(in_call_reference);
	  else 
	    fuv->SetCREF(out_call_reference);

	  diag(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	       "%s: sending the setup out port %d.\n", 
	       OwnerName(), outPort);
	  diag(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	       "%s: setting the call reference to 0x%x.\n",
	       OwnerName(), out_call_reference);

	} else {
	  //
	  // Forget about the reasons for release if we haven't 
	  // asked all of the ports that connect to the next
	  // hop.
	  //
	  // Report the cause and crankback after the last port
	  // refuses to accept the call.
	  //
	  if (litem != thePorts->last()) {
	    delete cause;
	    delete crankback;
	    cause = 0;
	    crankback = 0;
	  }

	  litem = thePorts->succ(litem);
	  
	}
      } else {
	//
	// DT: This shouldn't happen.  
	// SM: Oh, but it does ...
	// DT: Wise guy.
	//
	diag(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	     "%s can't set up on port %d when its %sup and its NodePeer FSM "
	     "is %sFullState.\n", OwnerName(), outPort,
	     (oport->Up() ? "" : "not "), 
	     (oport->NPState()!=NPStateVisitor::FullState ? "not " : ""));

	cause = new ie_cause(ie_cause::no_route_to_destination,
			     ie_cause::transit_network);

	//
	// Make the crankback with zero as the port number.
	// Zero means that all ports are down or not in FullState.
	// Since we'll use this crankback only if all of them
	// are that way, it's ok to make the crankback that way
	// when we encounter any down or non-full ports.
	//
	crankback = new PNNI_crankback(crankback_level,
				       PNNI_crankback::BlockedLink,
				       PNNI_crankback::NextNodeUnreachable,
				       _myNode, 0, destNode);

	litem = thePorts->succ(litem);
      }
    }

  } else {
    //
    // Can't find the destination in _Nodes.
    // Must reject the call.
    //
    // XXX Are these the right cause and crankback?
    //
    cause = new ie_cause(ie_cause::no_route_to_destination,
			 ie_cause::transit_network);

    crankback = new PNNI_crankback(crankback_level,
				   PNNI_crankback::BlockedNode,
				   PNNI_crankback::NextNodeUnreachable,
				   _myNode, 0, 0);
  }

  if ((cause != 0) || (crankback != 0)) {
    fuv->SetMSGType(FastUNIVisitor::FastUNISetupFailure);
    fuv->SetRelease(cause, crankback, 0);
    fuv->SetCREF(in_call_reference);
  }

  return answer;
}


//
// Reclaim the resources allocated for the call's service class.  Do
// nothing if there is no call.  Pass the Visitor on to the right port.
//
// Free the resources we allocated for some call.  We're going to
// forward the Release to Forwarder.  To help Forwarder remember that
// it already processed half of the release, we copy the in port to
// the out port and set the in port to zero.
//
// Ignores the FastUNIVisitor if it's in the _Ignore list.  Membership
// in _Ignore means that we've already processed the FastUNIVisitor.
// It makes no sense to process it a second time.
// 
void ACAC::Release(FastUNIVisitor *fuv)
{
  pluggable_port_info *iport = 0;
  pluggable_port_info *oport = 0;
  u_int inPort = fuv->GetInPort();
  u_int outPort = 0;
  int inCREF = fuv->GetCREF();
  int outCREF = 0;
  u_int vpi = 0;
  u_int vci = 0;
  FastUNIVisitor *toCalledParty = 0;
  FastUNIVisitor *toCallingParty = 0;

  dic_item di = _Ignore.lookup(fuv);

  assert(inPort >= 0);	/* The input port is -1 by default.
			 * If it's -1, then we can't index
			 * _ports[] correctly.  All manner
			 * of weirdness will ensue.
			 */
  //
  // Ignore a Release that's in the _Ignore list.
  // We've processed that Release before.
  //
  if (di != 0) {
    bool iAmOriginator = _Ignore.inf(di);
    _Ignore.del_item(di);

      // I originated the call and did something
      // about it in the else part of this if.
      //
      // There is nothing else to do with the Release but destroy it.
      //
    if (iAmOriginator) {
    DIAG(SIM_ACAC_CALL_RELEASE, DIAG_DEBUG, 
	 cout << OwnerName() << ": At time " 
	 << theKernel().CurrentElapsedTime() << endl;
	 cout << OwnerName() << ": " << *_myNode << 
	 " I already released port " << fuv->GetOutPort()
	 << " call reference 0x" 
	 << hex << inCREF << dec
	 << endl
	 << OwnerName()
	 << ": It is mine to kill."
	 << endl;)

      // DT Feb 25 '98 - Need to return vci?  I don't think so.

      fuv->Suicide();
    } else PassThru(fuv);
  } else {

    DIAG(SIM_ACAC_CALL, DIAG_INFO, 
	 cout << OwnerName() << ": At time " 
	 << theKernel().CurrentElapsedTime() << endl;
	 cout << OwnerName() << ": " << *_myNode << 
	 " releasing port " << inPort
	 << " call reference 0x" 
	 << hex << inCREF << dec
	 << endl;)

      iport = _ports[inPort];

      //
      // The Translator sets the CREF_FLAG bit whenever we are the
      // called party.  It unsets the bit when we are the calling
      // party.
      //
      bool releasedByCallingParty = (inCREF & CREF_FLAG);
      bool releasedByCalledParty = (!(inCREF & CREF_FLAG));

      //
      // Releases from the terminal on the B side of Control don't
      // have the CREF_FLAG set to indicate the calling party
      // 
      if (VisitorFrom(fuv) != Visitor::A_SIDE) {
	releasedByCallingParty = ! releasedByCallingParty;
	releasedByCalledParty = ! releasedByCalledParty;
      }

      if (releasedByCallingParty) 
	diag(SIM_ACAC_CALL, DIAG_INFO, "Released by calling party\n");

      if (releasedByCalledParty)
	diag(SIM_ACAC_CALL, DIAG_INFO, "Released by called party\n");

      if (iport != 0) {

	//
	// Remove the call from _SVCCs if it's an SVCC.
	// You can tell by the tuple <remoteNodeID, call reference>
	//
	if (fuv->IsForSVCC()) {
	  const NodeID * called_party = iport->CalledParty(inCREF);
	  list_item li;
	  if ((li = _SVCCs.search(called_party)) != 0) {
	    delete _SVCCs.inf(li);
	    _SVCCs.del_item(li);
	  }
	}

	iport->GetDest(inCREF, outPort, outCREF);

	iport->GetVPVC(inCREF, vpi, vci);
	  
	iport->FreeBandwidth(inCREF);

	// Forward the release to the calling party.
	//	  if (outPort > 0 && outPort != inPort) {
	// Release() used to insist that the outPort and the inPort
	// be different.  That wasn't a good idea.  Suppose that a
	// host called itself?  If it did, Release() would never be
	// able to release the call.
	//
	if (outPort > 0) {
	  //
	  // I did not originate this call.
	  // I must pass fuv to the next Conduit.
	  //
	  oport = _ports[outPort];
	  oport->FreeBandwidth(outCREF);

	  //
	  // This helps the Forwarder to remember that he already
	  // processed the other half of this Release.
	  //
	  fuv->SetOutPort(inPort);
	  fuv->SetInPort(0);

	  //
	  // Remember this Release in case we see it agin.
	  // We don't want to process it twice.
	  //
	  _Ignore.insert(fuv, false);

	  PassThru(fuv);

	} else if (outPort == 0) {
	  //
	  // I originated the call.  I can kill the Release.
	  //

	  ReturnPortZeroCREF(fuv->GetCREF());

	  //
	  // If we're the calling party, then we might want to
	  // initiate crankback.
	  //
	  if (!(inCREF & CREF_FLAG)) {
	    if (fuv->GetCrankback() == 0) {
	      //
	      // Since I originated the call and fuv has no crankback,
	      // there is no need to send it to RouteControl and Logos.
	      // I can Suicide() fuv with impunity.
	      //
	      diag(SIM_ACAC_CALL_RELEASE, DIAG_DEBUG, 
		   "%s: I originated the call and the RELEASE has no "
		   "crankback.  I'm killing it now.\n",
		   OwnerName());

	      ReturnVCI(vci);
	      fuv->Suicide();

	    } else if (VisitorFrom(fuv) != Visitor::A_SIDE) {
	      //
	      // XXX The only way that this block will be executed is if
	      // XXX the Terminal at the B side of Control (i.e. on the
	      // XXX B-side of the Database) decides to release a call.  
	      //
	      // XXX In reality, there must be *no crankback* and we must
	      // XXX send the Release to each side of the call.
	      //
	      // Not from my A-side.  fuv must have come from
	      // RouteControl.  Since it's a FastUNIRelease, I know that
	      // RouteControl and Logos couldn't perform crankback.
	      // Since I originated the call, I can Suicide() fuv.
	      //
	      diag(SIM_ACAC_CALL_RELEASE, DIAG_DEBUG, 
		   "%s: I originated the call and the RELEASE didn't come "
		   "from A_SIDE .  I'm killing it now.\n",
		   OwnerName());
	      
	      ReturnVCI(vci);
	      fuv->Suicide();

	    } else {
	      //
	      // I originated the call.
	      // The call has a crankback.
	      // The FastUNIVisitor came from my A-side.
	      //
	      fuv->SetOutPort(inPort);
	      fuv->SetInPort(0);
	      
	      //
	      // Remember this Release in case we see it agin.
	      // We don't want to process it twice.
	      //
	      _Ignore.insert(fuv, true);
	      PassThru(fuv);
	    }
	  } else {
	    //
	    // I am the called party.
	    //
	    diag(SIM_ACAC_CALL_RELEASE, DIAG_DEBUG, 
		 "%s: I am the called party.\n",
		 OwnerName());
	    
	    ReturnVCI(vci);
	    fuv->Suicide();
	  }
	} else {
	  if (inCREF & CREF_FLAG)
	    diag(SIM_ACAC_CALL_RELEASE, DIAG_DEBUG, 
		 "%s: Can't find port to calling party\n",
		 OwnerName());
	  else
	    diag(SIM_ACAC_CALL_RELEASE, DIAG_DEBUG, 
		 "%s: Can't find port to called party\n",
		 OwnerName());

	  diag(SIM_ACAC_CALL_RELEASE, DIAG_DEBUG, 
	       "%s: Killing the FastUNIVisitor\n",
	       OwnerName());

	  // DT Feb 25 '98 - Need to return vci?  I don't think so.

	  fuv->Suicide();
	}
      } else {
	diag(SIM_ACAC_CALL_RELEASE, DIAG_ERROR, 
	     "%s: No port info object for port %d\n", 
	     OwnerName(), inPort);
	diag(SIM_ACAC_CALL_RELEASE, DIAG_DEBUG, 
	     "%s: Killing the FastUNIVisitor\n",
	     OwnerName());

	// DT Feb 25 '98 - Need to return vci?  I don't think so.

	fuv->Suicide();
      }
  }
}


/* Release a specific call on a specific port.  
 *
 * Do this by calling pluggable_port_info::Release() twice.  The first
 * time is for the call <inport, cref>.  The second time is for the
 * other half of the call (conceptually, <outport, outcref>). 
 *
 * Send just one RELEASE message for this call.  The Forwarder will
 * make a RELASE for the other half.
 */
void ACAC::Release(int inport, int cref)
{
  assert(inport >= 0);	/* If it's < 0, then we can't index
			 * _ports[] correctly.  All manner
			 * of weirdness will ensue.
			 */

  if ((inport < _numPorts) && (inport >= 0)) {
    pluggable_port_info * port = _ports[inport];
    pluggable_port_info * other_port = 0;

    FastUNIVisitor *release = 0;
    FastUNIVisitor *other_release = 0;

    struct call_identifier *call_id = 0;
    struct call_identifier *other_call_id = 0;

    DIAG(SIM_ACAC_CALL_RELEASE, DIAG_INFO, 
	 cout << OwnerName() << ": At time " 
	 << theKernel().CurrentElapsedTime() << endl;
	 cout << OwnerName() << ": Releasing call " << cref
	 << " on port " << inport << endl;);

    port->Release(cref, &release, &call_id);
    if (release != 0) {
      if (call_id != 0) {
	other_port = _ports[call_id->_port];
	other_port->Release(call_id->_cref, &other_release,
			    &other_call_id);

	delete other_call_id; /* This is <inport, cref>.  We've taken
			       * care of it already.
			       */
	delete call_id;
	other_release->Suicide();
      }

      PassVisitorToA(release);
      //      if (other_release != 0) PassVisitorToA(other_release);
    }
  }
}


/*
 * Release all calls on a port.
 *
 * Send a RELEASE only for the half of the call that is on this port.
 * Forwarder will make a RELEASE for the other half of the call.
 */
void ACAC::Release(int inport)
{
  assert(inport >= 0);	/* If it's < 0, then we can't index
			 * _ports[] correctly.  All manner
			 * of weirdness will ensue.
			 */

  if ((inport < _numPorts) && (inport >= 0)) {
    FastUNIVisitor * release = 0;
    pluggable_port_info *port = _ports[inport];
    list <FastUNIVisitor *> *releases = 0;
    list <struct call_identifier *> *call_ids = 0;
    
    DIAG(SIM_ACAC_CALL_RELEASE, DIAG_INFO, 
	 cout << OwnerName() << ": At time " 
	 << theKernel().CurrentElapsedTime() << endl;
	 cout << OwnerName() << ": Releasing all calls on port " 
	 << inport << endl;);

    //
    // Release all of the calls on the port.  That tears down half of
    // each call.  Tear down the other half by asking the ports named
    // in call_ids.
    //
    port->Release(&releases, &call_ids);

    if (releases != 0) {
      if (call_ids != 0) {

	//
	// Remove a call_identifer from call_ids.  Release the call
	// that it represents.  Put the FastUNIVisitor at the end of
	// the list of releases.
	//
	// Do this until the call_ids list is emtpy.
	//
	while (call_ids->size() != 0) {
	  struct call_identifier *other_call_id = 0;
	  struct call_identifier *call_id = call_ids->pop();

	  pluggable_port_info *other_port = _ports[call_id->_port];

	  DIAG(SIM_ACAC_CALL_RELEASE, DIAG_INFO, 
	       cout << OwnerName() << ": At time " 
	       << theKernel().CurrentElapsedTime() << endl;
	       cout << OwnerName() << ": Releasing call " << call_id->_cref 
	       << " on port " << call_id->_port << endl;);

	  other_port->Release(call_id->_cref, &release, &other_call_id);

	  release->Suicide();
	  delete other_call_id;
	  delete call_id;
	}

	delete call_ids;
      }

      //
      // We've collected all of the releases for all of the calls that
      // go through this port.  Now deliver each one.
      //
      while (releases->size() > 0) {
	release = releases->pop();
	PassVisitorToA(release);
      }

      delete releases;
    }
  }
}


//
// Do the same as Release(FastUNIVisitor *fuv).
//
void ACAC::SetupFailure(FastUNIVisitor *fuv)
{
  DIAG(SIM_ACAC_CALL, DIAG_INFO, 
       cout << OwnerName() << ": " << *_myNode << 
       " setup failure!  Releasing port " << fuv->GetInPort()
       << " call reference 0x" 
       << hex << fuv->GetCREF() << dec 
       << endl;)

    Release(fuv);
}


//
// Originate PTSEs for each physical HLINK and UPLINK.  Send one
// PTSPPkt for each port that is up and in FullState and has changed
// significantly.  The port's Dirty() method indicates the
// significance of change.
//
// TODO- put all PTSEs for the same peer group together in one
// PTSPPkt.
//
void ACAC::Reoriginate(void)
{
  NPFloodVisitor *ptspv = 0;
  ig_resrc_avail_info *theRAIG = 0;

  ig_ptse * hlink_ptse = 0;
  ig_ptse * uplink_ptse = 0;

  pluggable_port_info *port = 0;
  ig_horizontal_links *hlink = 0;
  ig_uplinks *uplink = 0;
  ig_uplink_info_attr *theULIA = 0;
  u_int num_hlink_PTSEs = 0;
  u_int num_uplink_PTSEs = 0;

  DBACACInterface *db = (DBACACInterface *)QueryInterface("Database");
  assert (db != 0);

  db->Reference();

  assert(db->good());
  int lifetime = db->PTSELifetime();

  db->Unreference();

  if (lifetime > 0) {
    dic_item ditem;
    dictionary<u_int, ig_resrc_avail_info *> theRAIGs;
    ig_resrc_avail_info *aRAIG = 0;

    for (int i = 1; i < _numPorts; i++) {
      port = _ports[i];

      //
      // If the port is up and in FullState and has a significant change,
      // then reoriginate a PTSP.
      //
      if ((port->Up()) && (port->NPState() == NPStateVisitor::FullState) &&
	  (port->Dirty())) {
	PTSPPkt *reoriginate_ptsp = 0;

	switch (port->PortType()) {

	case pluggable_port_info::unspecified:
	  break;

	case pluggable_port_info::horizontal_link:	// Reoriginate HLINK
	  if (!reoriginate_ptsp)
	    reoriginate_ptsp = new PTSPPkt(_myNode->GetNID(), 
					   _myPeerGroup->GetPGID());

	  port->NextSequenceNumber();
	  hlink_ptse = 
	    new ig_ptse(InfoGroup::ig_horizontal_links_id, 
			port->ID(), port->SequenceNumber(), 0, 
			lifetime);
	  
	  hlink = new ig_horizontal_links(0, port->RemoteNode(), 
					  port->RemotePort(),
					  i, 
					  port->AggregationToken());

	  DIAG(SIM_ACAC_IG_HLINK, DIAG_DEBUG, 
	       cout << OwnerName() << ": reoriginating HLINK from "  << endl
	       << *_myNode << " port " << i << " to " << endl
	       << *(port->RemoteNode()) << " port " << port->RemotePort() 
	       << " with SN " << port->SequenceNumber()
	       << endl;)

	    theRAIGs = port->ShareAllRAIGs();

	    //
	    // Make a copy of the port's list of RAIGs.
	    // Put that copy into the hlink.
	    //
	    forall_items(ditem, theRAIGs) {
	      aRAIG = theRAIGs.inf(ditem);
	      
	      hlink->AddRAIG((ig_resrc_avail_info *)aRAIG->copy());
	    }

	    hlink_ptse->AddIG(hlink);
	    reoriginate_ptsp->AddPTSE(hlink_ptse);
	    num_hlink_PTSEs ++;
	    port->Dirty(false);	// Once we re-originate, the port is clean.
	    break;

	case pluggable_port_info::uplink:		// Reoriginate UPLINK
	  if (!reoriginate_ptsp)
	    reoriginate_ptsp = new PTSPPkt(_myNode->GetNID(), 
					   port->CommonPeerGroup()->GetPGID());

	  port->NextSequenceNumber();
	  uplink_ptse = 
	    new ig_ptse(InfoGroup::ig_uplinks_id, 
			port->ID(), port->SequenceNumber(), 0, 
			lifetime);
	  
	  DIAG(SIM_ACAC_IG_UPLINK, DIAG_DEBUG, 
	       cout << OwnerName() << ": Reoriginating UPLINK from "  << endl
	       << *_myNode << " port " << i << " to " << endl
	       << *(port->RemoteNode()) << " port " << port->RemotePort()
	       << " for peer group " << endl
	       << *(port->CommonPeerGroup())
	       << " with SN " << port->SequenceNumber()
	       << endl;)

	    uplink = new ig_uplinks(0, port->RemoteNode(), 
				    port->CommonPeerGroup(), i, 
				    port->AggregationToken(),
				    port->RemoteAddress());

	    theRAIGs = port->ShareAllRAIGs();

	    //
	    // Make a copy of the port's list of RAIGs.
	    // Put that copy into the hlink.
	    //
	    forall_items(ditem, theRAIGs) {
	      aRAIG = theRAIGs.inf(ditem);
	      
	      uplink->AddIG(aRAIG->copy());
	    }

	    theULIA = port->ULIA();

	    if (theULIA != 0) 
	      uplink->AddIG(new ig_uplink_info_attr(*theULIA));

	    uplink_ptse->AddIG(uplink);
	    reoriginate_ptsp->AddPTSE(uplink_ptse);
	    num_uplink_PTSEs ++;
	    port->Dirty(false);	// Once we re-originate, the port is clean.
	    break;

	default:
	  port->Dirty(false);
	  break;
	}

	//
	// Send the reoriginated PTSE's to the database
	//
	if ((num_hlink_PTSEs != 0) || (num_uplink_PTSEs != 0)) {
	  ptspv = new NPFloodVisitor(reoriginate_ptsp, _myNode, _myNode);
	  PassVisitorToB(ptspv);
	} else 
	  delete reoriginate_ptsp;
      }
    }
  }
}




/*
 * Immediately inform the database of each change.  */
State * ACAC::Handle(Visitor * v)
{
  VisitorType vt = v->GetType();
  list<ig_resrc_avail_info *> *theRAIGs = 0;

  DIAG("sim.acac", DIAG_DEBUG, 
       cout << OwnerName() << " received " << vt << " (" << v << ") ";
       if (vt.Is_A(_fast_uni_type))
         cout << ((FastUNIVisitor *)v)->PrintMSGType();
       cout << endl);

  if (vt.Is_A(_port_up_type)) {
    // HLINK IG
    PortUpVisitor *puv = (PortUpVisitor *)v;
    const NodeID *dest = puv->GetDestNID();	// Don't delete it

    if ((dest != 0) && (*dest == *_myNode)) {	// Physical
      theRAIGs = puv->GetRAIGs();
    
      PortUp(puv->GetLocalPort(), puv->GetRemotePort(), 
	     (NodeID *)puv->GetSourceNID(), puv->GetAgg(), theRAIGs);
    
      delete theRAIGs;
    
      v->Suicide();
    } else PassThru(v);	// Logical -- let Aggregator take care of it
  } else if (vt.Is_A(_port_down_type)) {
    // HLINK IG
    PortDownVisitor *pdv = (PortDownVisitor *)v;
    const NodeID *source = pdv->GetSourceNID();	// Don't delete it

    if ((source != 0) && (*source == *_myNode)) {	// Physical
      u_int lport = pdv->GetLocalPort();
    
      PortDown(lport);
    
      v->Suicide();
    } else PassThru(v);	// Logical -- let Aggregator take care of it
  } else if (vt.Is_A(_border_down_type)) {
    // UPLINK IG
    BorderDownVisitor *bv = (BorderDownVisitor *)v;
    const NodeID *destination = bv->GetDestNID();	// Don't delete it

    if ((destination != 0) && (*destination == *_myNode)) {	// Physical
      int lport = bv->GetLocalPort();
    
      BorderDown(lport);
    
      v->Suicide();
    } else PassThru(v);	// Logical -- let Aggregator take care of it
  } else if (vt.Is_A(_border_up_type)) {
    // UPLINK IG
    BorderUpVisitor *bv = (BorderUpVisitor *)v;
    const NodeID *destination = bv->GetDestNID();	// Don't delete it

    if ((destination != 0) && (*destination == *_myNode)) {	// Physical
      BorderUp(bv);
      v->Suicide();
    } else {
      DIAG(SIM_ACAC_BORDER_UP, DIAG_WARNING, 
	   cout << SIM_ACAC_BORDER_UP 
	   << " (Warning): "
	   << OwnerName()
	   << ": Border Up for destination node ";
	   if (destination != 0) cout << *destination;
	   else cout << "0";
	   cout << endl;
	   cout << SIM_ACAC_BORDER_UP 
	   << " (Warning): "
	   << OwnerName()
	   << ": My node is " << *_myNode << endl;
	   cout << SIM_ACAC_BORDER_UP 
	   << " (Warning): "
	   << OwnerName()
	   << ": Passing it along." << endl;
	   );

      PassThru(v);	// Logical -- let Aggregator take care of it
    }
  } else if (vt.Is_A(_npstate_type)) {
    //
    // If the NPStateVisitor is for me at my physical address, then
    // mark some nodes as Full or NonFull.  If it's not then don't act
    // on it.
    //
    // In either case, pass it to the next Conduit.
    //
    NPStateVisitor *npv = (NPStateVisitor *)v;
    const NodeID *source = npv->GetSourceNID();

    if ((source != 0) && (*source == *_myNode)) {
      switch (npv->GetVT()) {
      case NPStateVisitor::FullState:
	NodeFull(npv->GetDestNID());
	break;
      
      default:
	NodeNonFull(npv->GetDestNID());
	break;
      }
    }
    PassThru(v);

  } else if (vt.Is_A(_show_type)) {
    Show(((ShowVisitor *)v)->AsString());
    PassThru(v);
  } else if (vt.Is_A(_save_type)) {
    Show(((ShowVisitor *)v)->AsString());
    PassThru(v);
  } else if (vt.Is_A(_stream_save_type)) {
    //    ostream os = ((StreamSaveVisitor *)v)->GetStream();
    //    StreamSave(os);
    StreamSave( ((StreamSaveVisitor *)v)->GetStream() );
    PassThru(v);
  } else if (vt.Is_A(_fast_uni_type)) {
    FastUNIVisitor *fuv = (FastUNIVisitor *)v;
    dic_item di = 0;

    switch (VisitorFrom(v)) {
      case Visitor::A_SIDE:		// From B side of the control port
	switch (fuv->GetMSGType()) {
	  case FastUNIVisitor::FastUNISetup:
	    PassThru(v);    // Passes the Visitor in the right direction
	    break;
	  case FastUNIVisitor::FastUNIRelease:
	    Release(fuv);
	    break;
	  case FastUNIVisitor::FastUNISetupFailure:
	    diag(SIM_ACAC, DIAG_DEBUG, 
		 "%s deleting unexpected FastUNISetupFailure from "
		 "DataForwarder\n", OwnerName());
	    fuv->Suicide();
	    break;
	  default:
	    PassThru(v);
	    break;
	}
	break;
      case Visitor::B_SIDE:		// From the GCAC
	switch (fuv->GetMSGType()) {
	  case FastUNIVisitor::FastUNISetup:
	    SetupFromGCAC(fuv); // Passes the Visitor in the right direction
	    break;
	  case FastUNIVisitor::FastUNIRelease:
	    Release(fuv);
	    break;
	  case FastUNIVisitor::FastUNISetupFailure:
	    //
	    // If we've processed this FastUNIVisitor before, when it
	    // was a FastUNIRelease, then we won't see it again and we
	    // must remove it from _Ignore.
	    //
	    di = _Ignore.lookup(fuv);
	    if (di != 0) _Ignore.del_item(di);

	    fuv->SetOutPort(fuv->GetInPort());	// Could be any physical port
	    fuv->SetInPort(0);		/* The DataForwarder needs to think
					 * that the release came from
					 * the Control port.  Otherwise,
					 * it will send the release back
					 * here.
					 */
	    if (fuv->GetSourceNID()) {
	      DBACACInterface *db = 
		(DBACACInterface *)QueryInterface("Database");

	      assert(db != 0);
	      db->Reference();

	      assert(db->good());
	      if (db->LogicalNodeIsMe( fuv->GetSourceNID())) {
		// DT Feb 25 '98 - Need to return vci?  I don't think so.
		fuv->Suicide();
	      } else 
		PassThru(v);
	      
	      db->Unreference();
	    } else 
	      PassThru(v);
	    break;
	default:
	  PassThru(v);
	  break;
	}
	break;
      case Visitor::OTHER:
	PassVisitorToB(v);
	break;
    }
  } else
    PassThru(v);
  
  return this;
}

void ACAC::Interrupt(SimEvent *event)
{
  DBACACInterface *db = 0;

  u_int vpi = 0;
  u_int vci = 0;
  int cref = 0;
  u_int port = 0;
  int code = (int)(*event);
  /*
     * Inform the database of the status of each port that is up.
     * Give each PTSE the same time to live.
     */
  switch (code) {
  case ACAC_WAKEUP_EVENT:
    //
    // DWT 980902 Maybe make a new method, ReoriginateAll() that marks
    // ports dirty and calls Reoriginate().
    //
    // Mark each port dirty to force Reoriginate() to 
    // reoriginate a PTSP for each port.
    for (int port_num = 0; port_num < _numPorts; port_num++)
      _ports[port_num]->Dirty(true);
    
    Reoriginate();

    db = (DBACACInterface *)QueryInterface("Database");

    assert (db != 0);
    db->Reference();

    assert(db->good());
    ReturnToSender(event, db->PTSERefreshInterval());

    db->Unreference();
    break;

  case ACAC_REORIGINATION_EVENT:
    Reoriginate();
    delete event;
    break;

  default:
    DIAG(SIM_ACAC, DIAG_ERROR, 
	 cout << OwnerName()
	 << ": unknown SimEvent code= " << code 
	 << " (" << hex << code << dec << ")" << endl;)
      delete event;
  }
}

void ACAC::Show(const char *filename)
{
  if ((filename == 0) || (strlen(filename) == 0))
    cout << *this;
  else {
    const char *ownername = OwnerName();
    char *acac_filename = new char[strlen(filename) + strlen(ownername) + 2];

    sprintf(acac_filename, "%s.%s", filename, ownername);

    ofstream of(acac_filename);

    of << *this;

    delete [] acac_filename;

    of.close();
  }
}

ostream & operator << (ostream & os, const ACAC & cac)
{
  dic_item di;
  seq_item si;

  os << "ACAC ";
  if (cac._myNode) os << *(cac._myNode) << endl;
  else os << "null" << endl;

  os << "_numPorts " << cac._numPorts << endl;

  os << "_myNode ";
  if (cac._myNode) os << *(cac._myNode) << endl;
  else os << "null" << endl;

  os << "_myPeerGroup ";
  if (cac._myPeerGroup) os << *(cac._myPeerGroup) << endl;
  else os << "null" << endl;

  os << "_iAmTheLeader " << cac._iAmTheLeader << endl;


  os << "Nodes:" << endl;
  forall_items(di, cac._Nodes) {
    list_item li;
    ACAC::int_list *ports = cac._Nodes.inf(di);
    forall_items(li, *ports) {
      os << "  Port " << ports->inf(li)
	 << " goes to Node " << *(cac._Nodes.key(di)) << endl;
    }
  }
  os << "--------------------------------------------------" << endl;
  os << endl;
  
  os << "Ports:" << endl;
  for (int i=0; i < cac._numPorts; i++) {
    os << "Port " << i << ": ";
    if (cac._ports[i] != 0) os << *(cac._ports[i]) << endl;
    else os << "***" << endl;
  }
  
  os << "--------------------------------------------------" << endl;
  os << endl;

  return os;
}


// Method to be used with StreamSaveVisitor (currently used by Jack's
// route optimality test)
void ACAC::StreamSave(ostream & os)
{
  if (_myNode == 0) {
    diag(SIM_ACAC, DIAG_ERROR, "ACAC %ld doesn't know its NodeID!\n", this);
    return;
  }
  // Our NodeID
  os << "(" << *_myNode << endl;

  // Link information
  for (int i = 1; i < _numPorts; i++) {
    dic_item ditem;
    dictionary<u_int, ig_resrc_avail_info *> theRAIGs;
    ig_resrc_avail_info *aRAIG = 0;

    if (!_ports[i] || !(_ports[i]->RemoteNode()))
      continue;
    // First output local and remote port
    os << "  (" << i << " " << _ports[i]->RemotePort() << endl;
    // Next comes the remote NodeID
    os << "   " << *(_ports[i]->RemoteNode()) << dec << endl;
    // now print each of the RAIGs
    os << "   metrics(" << endl;

    theRAIGs = _ports[i]->ShareAllRAIGs();

    forall_items(ditem, theRAIGs) {
      aRAIG = theRAIGs.inf(ditem);
      os << "   [0x" ;
      os.width(4);	// Width and fill are temporary.
      os.fill('0');	// They end after the next os << foo;
      os << hex << aRAIG->GetFlags();
      os << dec << "] " 
	 << aRAIG->GetMCR() 
	 << " " << aRAIG->GetACR() << " " << aRAIG->GetCTD() << " " 
	 << aRAIG->GetCDV() 
	 << " " << aRAIG->GetCLR0() << " " << aRAIG->GetCLR01() << endl;
    }

    os << "    )" << endl;
    os << "  )" << endl;
  }
  // end of block
  os << ")" << endl;
}

int ACAC::GetNewVCI(void)
{
  int rval = -1;
  ForwarderDefaultInterface * fwi = 
    (ForwarderDefaultInterface *)QueryInterface("DataForwarder");
  assert (fwi != 0);

  fwi->Reference();
  rval = fwi->ObtainNewVCI(0);
  fwi->Unreference();

  return rval;
}

void ACAC::ReturnVCI(int vci)
{
  ForwarderDefaultInterface * fwi = 
    (ForwarderDefaultInterface *)QueryInterface("DataForwarder");
  assert(fwi != 0);

  fwi->Reference();
  fwi->ReturnVCI(0, vci);
  fwi->Unreference();
}

u_int ACAC::GetPortZeroCREF(void)
{
  int rval = -1;
  ForwarderDefaultInterface * fwi = 
    (ForwarderDefaultInterface *) QueryInterface("DataForwarder");
  assert(fwi != 0);

  fwi->Reference();
  rval = fwi->ObtainCREF();
  fwi->Unreference();

  return rval;
}

void ACAC::ReturnPortZeroCREF(u_int cref)
{
  ForwarderDefaultInterface * fwi = 
    (ForwarderDefaultInterface *)QueryInterface("DataForwarder");
  assert(fwi != 0);
  fwi->Reference();
  fwi->ReturnCREF(cref);
  fwi->Unreference();
}

bool ACAC::ItsForMe(FastUNIVisitor *fuv)
{
  bool answer = false;

  // The dataforwarder will have stamped the Dest 
  // field of the visitor on the very last hop
  Addr * destAddr = 0;

  //
  // Remember! GetAddr() makes a copy of the address.
  // Delete the copy later.
  //
  if (fuv->GetDestNID()) destAddr = fuv->GetDestNID()->GetAddr();

  if (destAddr != 0) {
    answer = 
      (((NSAP_DCC_ICD_addr*)_myAddr)->equals((NSAP_DCC_ICD_addr*)destAddr));

    delete destAddr;
    destAddr = 0;
  } else {
    // GetSetup() shares a pointer to the q93b_setup_message
    q93b_setup_message *setup = fuv->GetSetup();
    ie_called_party_num *called_party = 
      (ie_called_party_num *)setup->ie(InfoElem::ie_called_party_num_ix);
    answer = ((called_party != 0) &&
	      (((NSAP_DCC_ICD_addr*)(called_party->get_addr()))->
	       equals((NSAP_DCC_ICD_addr*)_myAddr)));
    
  }

  if (!answer) {
    // let's ask the database, shall we?
    DBACACInterface *db = (DBACACInterface *)QueryInterface("Database");

    assert(db != 0);
    db->Reference();
    
    assert(db->good());
    answer = db->LogicalNodeIsMe(fuv->GetDestNID());
    db->Unreference();
  }

  return answer;
}


bool ACAC::ItsFromMe(FastUNIVisitor *fuv)
{
  bool answer = false;

  // The dataforwarder will have stamped the Dest 
  // field of the visitor on the very last hop
  Addr * sourceAddr = 0;

  //
  // Remember! GetAddr() makes a copy of the address.
  // Delete the copy later.
  //
  if (fuv->GetSourceNID()) sourceAddr = fuv->GetSourceNID()->GetAddr();

  if (sourceAddr != 0) {
    answer = 
      (((NSAP_DCC_ICD_addr*)_myAddr)->equals((NSAP_DCC_ICD_addr*)sourceAddr));

    delete sourceAddr;
    sourceAddr = 0;
  } else {
    // GetSetup() shares a pointer to the q93b_setup_message
    q93b_setup_message *setup = fuv->GetSetup();
    ie_called_party_num *called_party = 
      (ie_called_party_num *)setup->ie(InfoElem::ie_called_party_num_ix);
    answer = ((called_party != 0) &&
	      (((NSAP_DCC_ICD_addr*)(called_party->get_addr()))->
	       equals((NSAP_DCC_ICD_addr*)_myAddr)));
    
  }

  if (!answer) {
    // let's ask the database, shall we?
    DBACACInterface *db = (DBACACInterface *)QueryInterface("Database");

    assert(db != 0);
    db->Reference();
    assert(db->good());
    answer = db->LogicalNodeIsMe(fuv->GetSourceNID());
    db->Unreference();
  }

  return answer;
}
