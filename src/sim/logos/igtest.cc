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
//
// this is a pre-integration test
//
//   setup will be  Top terminal <--> Logos <--> Database <--> Bot terminal
//
// Top terminal will inject NPFloodVisitors with ptse's to describe
//              a network. Later it will generate a FastUNIVisitor's
//              to test routing.
//
// The Database will receive the NPFloodVisitors
// Logos will print the resulting graph
//
// This program is very fragile and is almost impossible to extend.
// It pokes values into u_char[22] arrays to form NodeID's.

#ifndef LINT
static char const _igtest_cc_rcsid_[] =
"$Id: igtest.cc,v 1.23 1999/01/07 23:04:06 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/nodal_state_params.h>
#include <codec/pnni_ig/nodal_hierarchy_list.h>
#include <codec/pnni_ig/next_hi_level_binding_info.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/DBKey.h>
#include <codec/pnni_pkt/ptsp.h>
#include <codec/uni_ie/PNNI_designated_transit_list.h>

#include <fsm/election/ElectionVisitor.h>
#include <fsm/visitors/NPFloodVisitor.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/FileIOVisitors.h>
#include <fsm/visitors/DBVisitors.h>
#include <fsm/visitors/PNNIVisitor.h>
#include <fsm/visitors/BorderVisitor.h>
#include <fsm/database/Database.h>
#include <fsm/visitors/PortDownVisitor.h>
#include <fsm/visitors/PortUpVisitor.h>

#include <FW/actors/Expander.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/diag.h>
#include <FW/behaviors/Protocol.h>
#include <FW/behaviors/Adapter.h>
#include <FW/kernel/Handlers.h>

#include <sim/switch/DebugTerminal.h>
#include <sim/logos/Logos.h>

#include <DS/containers/list.h>

#include <fstream.h>
#include <iostream.h>

// the basic NodeID for this network 
static u_char bnid[22] = {96, 160,
			  0x47, 0x00, 0x05, 0x80, 0xff, 0xde, 0x00, // prefix
			  0x00, 0x01, 0x00, 0x00, 0x00, 0x00,       // prefix
			  0xff, 0x1c, 0x06, 0x00, 0x00, 0x00,       // esi
			  0x00 };                                   // sel

NodeID *dest = 0;
NodeID *logn0 = 0;
NodeID *logn6 = 0;
NodeID *logn7 = 0;

// tells where a connection goes...
// linkto was originally called link.
// Changed to linkto one day when compiler refused to compile ???
class linkto {
public:
  linkto(int aggtok=0, int localport=-1, int remoteport=-1, NodeID *nid) :
      _aggtok(aggtok), _local_port( localport), _remote_port(remoteport)
  {
    if (nid)  _remote_nodeid = new NodeID(*nid);
    else nid = 0;;
  }
  ~linkto( ) { delete _remote_nodeid; }
  
  int _aggtok;
  int _local_port;
  int _remote_port;
  NodeID *_remote_nodeid;
};

// a raig is a set of communication line characteristics
class raig {
public:
  raig( short rflags=0x800,
	int admin_weight = 5040,
	int max_cell_rate = 1412830,
	int avail_cell_rate = 1412780,
	int cell_transfer_delay = 202,
	int cell_delay_var = 182,
	short clr0 = 8,
	short clr01 = 8) :
    _rflags(rflags),
    _admin_weight(admin_weight),
    _max_cell_rate (max_cell_rate ),
    _avail_cell_rate(avail_cell_rate),
    _cell_transfer_delay(cell_transfer_delay),
    _cell_delay_var(cell_delay_var),
    _clr0(clr0),
    _clr01(clr01)
  { }
  
  ~raig() {}
  
  short _rflags;
  int _admin_weight;
  int _max_cell_rate;
  int _avail_cell_rate;
  int _cell_transfer_delay;
  int _cell_delay_var;
  short _clr0;
  short _clr01;
  
};

raig normal_raig (0x800,  5040, 1412830, 1412830,  202, 182,  8, 8);
raig   good_raig (0x800,   500, 1412830, 1412830,   20,  18,  9, 9);
raig    bad_raig (0x800, 10000, 1412830, 1412830, 4000, 3600, 7, 7);

ig_resrc_avail_info * raigtoig(raig r)
{
  ig_resrc_avail_info::resrc_type x;

  return (new ig_resrc_avail_info(x, r._rflags, r._admin_weight,
			r._max_cell_rate, r._avail_cell_rate,
			r._cell_transfer_delay,
			r._cell_delay_var,
			r._clr0, r._clr01) );
};

// a fake_node has a NodeID and a list of linkto *
class fake_node {
public:
  fake_node(int switch_id,
	    int num_edges,
	    char *adj_nodes, char *aggtok, char *lport, char *rport,
	    u_char *bnid)
  {
    int i;

    _mynodeid = new NodeID(bnid);
    // build a list of all nodes adjacent to this one
    for (i=0; i<num_edges; i++) {

      if ( adj_nodes[i] >= 0) {
	bnid[14] = bnid[18] = bnid[19] = bnid[20] = (u_char)adj_nodes[i];
	if (switch_id <= 0) {
	  bnid[2+bnid[0]/8] = (u_char)adj_nodes[i];
	}
	NodeID *an = new NodeID(bnid);
	linkto *thelink = new linkto(aggtok[i], lport[i], rport[i], an);
	_adj_link.append(thelink);
	delete an;
      }
    }

    bnid[1] = bnid[0];
    bnid[0] = bnid[0] - 8;
    
    for (i=0; i<num_edges; i++) {

      if ( adj_nodes[i] < 0) {

	  // if the adjacent node is a negative value then the connection
	  // is to a logical node via an uplink. 
	  bnid[14] = bnid[18] = bnid[19] = bnid[20] =(u_char)(-adj_nodes[i]);
	  bnid[2+bnid[0]/8] = (u_char)(-adj_nodes[i]);
	  NodeID *ln = new NodeID(bnid);
	  linkto *thelink = new linkto(aggtok[i], lport[i], rport[i], ln);
	  _adj_link.append(thelink);
	  delete ln;
      }
    }
  }
  ~fake_node()
  {
    if (_mynodeid) delete  _mynodeid;
    while (_adj_link.size() != 0) {
      linkto *l = _adj_link.pop();
      delete l;
    }
  }
  void AddNSP(ig_nodal_state_params *nsp)
  {
    _nsps.append(nsp);
  }
  
  NodeID *_mynodeid;
  list <linkto *> _adj_link;
  list <ig_nodal_state_params *> _nsps;
};
 
#define NUM_NODES 9
ig_nodal_hierarchy_list *nhl = 0;

#define SIMPLE_NETWORK 0
#if SIMPLE_NETWORK

// the simple network I am building
//
//    +-------+                       +-------+          
//    |  PHYS |                       |  LGN  | Logical Instance of          
//    |   1   |                       |   0   | the physical node
//    |       |                       |       |  1
//    +-------+                       +-------+ 
//             
//                    +-------+       +-------+
//                    |  LGN  |------>|  LGN  |
//                    |   5   |       |   6   |
//                    |       |<------|       |
//                    +-------+       +-------+


list <fake_node *> *make_network(u_char *base_nid)
{
    int num_edges;
    char *adj_nodes;
    char *aggtok;	// aggregation token
    char *lport;	// local port
    char *rport;	// remote port

    int node_num;    
    u_char nid[22];
    
    int n, links;
    list<fake_node *> *network = new list<fake_node *>;
    fake_node *nfn;
    // if a value of adj_nodes is negative then it will be an up  link

    // this first "node" is the one that has the database we are building
    if (nhl) {delete nhl; nhl = 0;} // get rid of old one if called again
    nhl = new ig_nodal_hierarchy_list(9);
#define SUPER_SIMPLE 0
#if SUPER_SIMPLE != 0
    links = 0;
    node_num = 1;
    num_edges = links;
    adj_nodes = new char[links];    aggtok =    new char[links];
    lport  =    new char[links];    rport  =    new char[links];
    bcopy(base_nid, nid, 22);
    nid[14]=nid[18]=nid[19]=nid[20]=1;
    nhl->AddLevel(nid, 0, 0);
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);
    network->append(nfn);
    delete [] adj_nodes; delete [] aggtok; delete [] lport; delete [] rport;
#endif
    links = 1;
    node_num = -5;
    num_edges = links;
    adj_nodes = new char[links];    aggtok    = new char[links];
    lport  =    new char[links];    rport  =    new char[links];
    adj_nodes[0] = 6; aggtok[0] = 5; lport[0]=56, rport[0]=65;
    bcopy(base_nid, nid, 22);
    nid[0]=88; nid[1]=96; nid[2+10]=5;
    nid[14]=nid[18]=nid[19]=nid[20]=5;
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);
    {
      ig_nodal_state_params *nsp = new ig_nodal_state_params(0,0,0);
      nsp->AddRAIG(raigtoig(normal_raig));
      nfn->AddNSP(nsp);
    }
    network->append(nfn);
    delete [] adj_nodes; delete [] aggtok; delete [] lport; delete [] rport;
    dest=new NodeID(nfn->_mynodeid->GetNID());

#if SUPER_SIMPLE != 0
    links = 1;
    node_num = -6;
    num_edges = links;
    adj_nodes = new char[links];    aggtok    = new char[links];
    lport  =    new char[links];    rport  =    new char[links];
    adj_nodes[0] = 5; aggtok[0] = 5; lport[0]=65, rport[0]=56;
    bcopy(base_nid, nid, 22);
    nid[0]=88; nid[1]=96; nid[2+10]=6;
    nid[14]=nid[18]=nid[19]=nid[20]=6;
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);
    {
      ig_nodal_state_params *nsp = new ig_nodal_state_params(0,0,0);
      nsp->AddRAIG(raigtoig(bad_raig));
      nfn->AddNSP(nsp);
    }
    network->append(nfn);
    delete [] adj_nodes; delete [] aggtok; delete [] lport; delete [] rport;

    
    // this is the logical node for our 4 physical nodes [1]...[4]
    links = 0;
    node_num = 0;
    num_edges = links;
    adj_nodes = new char[links];    aggtok    = new char[links];
    lport  =    new char[links];    rport  =    new char[links];
    bcopy(base_nid, nid, 22);
    nid[0]=88; nid[1]=96; nid[2+10]=0;
    nid[14]=nid[18]=nid[19]=nid[20]=0;
    nhl->AddLevel(nid, 0, 0);
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);

    network->append(nfn);
    delete [] adj_nodes; delete [] aggtok; delete [] lport; delete [] rport;
#endif // !SUPER_SIMPLE
    return network;
}
#else
// the network I am building
//
//    +-------+       +-------+       +-------+       +-------+       
//    |  PHYS |------>|  PHYS |------>|  PHYS |------>|  PHYS |       
//    |   1   |       |   2   |       |   3   |       |   4   |       
//    |       |<------|       |<------|       |<------|       |       
//    +-------+       +-------+       +-------+       +-------+       
//                          |             |             |    
//                          +--uplink---+ | +---uplink--+    
//                                      | | |                
//                                      v v v                
//                    +-------+       +-------+       +-------+         +-----+
//                    |  LGN  |------>|  LGN  |------>|  LGN  |         |     |
//                    |   5   |       |   6   |       |   7   |-uplink->|  8  |
//                    |       |<------|       |<------|       |         |     |
//                    +-------+       +-------+       +-------+         +-----+
//                                     ^^^ ||| 
//                                     ||| ||| 
//                                     ||| vvv 
//                                    +-------+
//                                    |  LGN  | Logical Instance of
//                                    |   0   | the physical nodes
//                                    |       |  1, 2, 3, 4
//                                    +-------+
//
//
//              detail of LGN 6 (default radius + 1 bypass)
//                  +-------------------+
//                  |   NSP   LGN 6     | all links except ! from 64 to 67  
//                  | +---+ +---+ +---+ | use bad_raigs - this will allow
//                  | |62 | |63 | |64 | | routing to go out of its way to use
//                  | +---+ +---+ +---+ | this path.
//                  |      \  |  /  !   | ! link is good_raig
//                  | +---+ +---+ +---+ |
//                  | |65 |-| 0 |-|67 | |
//                  | +---+ +---+ +---+ |
//                  |     center 6      |
//                  +-------------------+
//
//

list <fake_node *> *make_network(u_char *base_nid)
{
    int num_edges;
    char *adj_nodes;
    char *aggtok;	// aggregation token
    char *lport;	// local port
    char *rport;	// remote port

    int node_num;    
    u_char nid[22];
    
    int links;
    list<fake_node *> *network = new list<fake_node *>;
    fake_node *nfn;
    // if a value of adj_nodes is negative then it will be an up  link

    // this first "node" is the one that has the database we are building
    if (nhl) {delete nhl; nhl = 0;} // get rid of old one if called again
    nhl = new ig_nodal_hierarchy_list(9);
    links = 1;
    node_num = 1;
    num_edges = links;
    adj_nodes = new char[links]; aggtok =    new char[links];
    lport  =    new char[links]; rport  =    new char[links];
    adj_nodes[0] = 2;  aggtok[0] = 0; lport[0]=2, rport[0]=1;
    bcopy(base_nid, nid, 22);
    nid[14]=nid[18]=nid[19]=nid[20]=1;
    nhl->AddLevel(nid, 0, 0);
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);
    network->append(nfn);
    delete [] adj_nodes; delete [] aggtok; delete [] lport; delete [] rport;
    
    links = 3;
    node_num = 2;
    num_edges = links;
    adj_nodes = new char[links];  aggtok    = new char[links];
    lport  =    new char[links];  rport  =    new char[links];
    adj_nodes[0] =  1; aggtok[0] = 0; lport[0]=1, rport[0]=2;
    adj_nodes[1] =  3; aggtok[1] = 0; lport[1]=3, rport[1]=2;
    adj_nodes[2] = -6; aggtok[2] = 2; lport[2]=6, rport[2]=-1;
    bcopy(base_nid, nid, 22);
    nid[14]=nid[18]=nid[19]=nid[20]=2;
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);
    network->append(nfn);
    delete [] adj_nodes; delete [] aggtok; delete [] lport; delete [] rport;

    links =3;
    node_num = 3;
    num_edges = links;
    adj_nodes = new char[links];  aggtok = new char[links];
    lport  =    new char[links];  rport  = new char[links];
    adj_nodes[0] =  2; aggtok[0] = 0; lport[0]=2, rport[0]=3;
    adj_nodes[1] =  4; aggtok[1] = 0; lport[1]=4, rport[1]=3;
    adj_nodes[2] = -6; aggtok[2] = 3; lport[2]=6, rport[2]=-1;
    bcopy(base_nid, nid, 22);
    nid[14]=nid[18]=nid[19]=nid[20]=3;
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);
    network->append(nfn);
    delete [] adj_nodes; delete [] aggtok; delete [] lport; delete [] rport;

    links = 2;
    node_num = 4;
    num_edges = links;
    adj_nodes = new char[links]; aggtok = new char[links];
    lport  =    new char[links]; rport  = new char[links];
    adj_nodes[0] =  3; aggtok[0] = 0; lport[0]=3, rport[0]=4;
    adj_nodes[1] = -6; aggtok[1] = 4; lport[1]=6, rport[1]=-1;
    bcopy(base_nid, nid, 22);
    nid[14]=nid[18]=nid[19]=nid[20]=4;
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);
    network->append(nfn);
    delete [] adj_nodes; delete [] aggtok; delete [] lport; delete [] rport;

    links = 1;
    node_num = -5;
    num_edges = links;
    adj_nodes = new char[links]; aggtok = new char[links];
    lport  =    new char[links]; rport  = new char[links];
    adj_nodes[0] = 6; aggtok[0] = 5; lport[0]=56, rport[0]=65;
    bcopy(base_nid, nid, 22);
    nid[0]=88; nid[1]=96; nid[2+10]=5;
    nid[14]=nid[18]=nid[19]=nid[20]=5;
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);
    network->append(nfn);
    delete [] adj_nodes; delete [] aggtok; delete [] lport; delete [] rport;

    links = 5;
    node_num = -6;
    num_edges = links;
    adj_nodes = new char[links]; aggtok = new char[links];
    lport  =    new char[links]; rport  = new char[links];
    adj_nodes[0] = 5; aggtok[0] = 5; lport[0]=65, rport[0]=56;
    adj_nodes[1] = 7; aggtok[1] = 7; lport[1]=67, rport[1]=76;
    adj_nodes[2] = 0; aggtok[2] = 2; lport[2]=62, rport[2]=2;
    adj_nodes[3] = 0; aggtok[3] = 3; lport[3]=63, rport[3]=3;
    adj_nodes[4] = 0; aggtok[4] = 4; lport[4]=64, rport[4]=4;
    bcopy(base_nid, nid, 22);
    nid[0]=88; nid[1]=96; nid[2+10]=6;
    nid[14]=nid[18]=nid[19]=nid[20]=6;
    logn6 = new NodeID(nid);
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);
    {
      ig_nodal_state_params *nsp = new ig_nodal_state_params(0,0,0);
      nsp->AddRAIG(raigtoig(bad_raig));
      nfn->AddNSP(nsp);
    }
    {
      ig_nodal_state_params *nsp = new ig_nodal_state_params(0,64,67);
      nsp->AddRAIG(raigtoig(good_raig));
      nfn->AddNSP(nsp);
    }
    {
      ig_nodal_state_params *nsp = new ig_nodal_state_params(0,67,64);
      nsp->AddRAIG(raigtoig(good_raig));
      nfn->AddNSP(nsp);
    }
    network->append(nfn);
    delete [] adj_nodes; delete [] aggtok; delete [] lport; delete [] rport;

    links = 2;
    node_num = -7;
    num_edges = links;
    adj_nodes = new char[links]; aggtok = new char[links];
    lport  =    new char[links]; rport  = new char[links];
    adj_nodes[0] = 6;  aggtok[0] = 7; lport[0]=76, rport[0]=67;
    adj_nodes[1] =-8;  aggtok[0] = 8; lport[1]=78, rport[1]=0;
    bcopy(base_nid, nid, 22);
    nid[0]=88; nid[1]=96; nid[2+10]=7;
    nid[14]=nid[18]=nid[19]=nid[20]=7;
    logn7 = new NodeID(nid);
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);
    {
      ig_nodal_state_params *nsp = new ig_nodal_state_params(0,0,0);
      nsp->AddRAIG(raigtoig(good_raig));
      nfn->AddNSP(nsp);
    }
    dest=new NodeID(nfn->_mynodeid->GetNID());
    network->append(nfn);
    delete [] adj_nodes; delete [] aggtok; delete [] lport; delete [] rport;

    // bad practice but at this point, nid has the id for the uplink to node 8
    links = 0;
    node_num = -8;
    num_edges = links;
    adj_nodes = new char[links];  aggtok    = new char[links];
    lport  =    new char[links];  rport  =    new char[links];
    //bcopy(base_nid, nid, 22);
    //nid[0]=80; nid[1]=88; nid[2+10]=8;
    //nid[14]=nid[18]=nid[19]=nid[20]=8;
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);
    network->append(nfn);
    delete [] adj_nodes;    delete [] aggtok;
    delete [] lport;        delete [] rport;

    /////////////////////////////////////////////////////////////////
    // this is the logical node for our 4 physical nodes [1]...[4] //
    /////////////////////////////////////////////////////////////////
    links = 3;
    node_num = 0;
    num_edges = links;
    adj_nodes = new char[links];    aggtok    = new char[links];
    lport  =    new char[links];    rport  =    new char[links];
    adj_nodes[0] = 6; aggtok[0] = 2; lport[0]=102, rport[0]=62;
    adj_nodes[1] = 6; aggtok[1] = 3; lport[1]=103, rport[1]=63;
    adj_nodes[2] = 6; aggtok[2] = 4; lport[2]=104, rport[2]=64;
    bcopy(base_nid, nid, 22);
    nid[0]=88; nid[1]=96; nid[2+10]=0;
    nid[14]=nid[18]=nid[19]=nid[20]=0;
    nhl->AddLevel(nid, 0, 0);
    logn0 = new NodeID(nid);
    nfn = new fake_node(node_num, num_edges, adj_nodes,
			aggtok, lport, rport, nid);

    {
      ig_nodal_state_params *nsp = new ig_nodal_state_params(0,0,0);
      nsp->AddRAIG(raigtoig(normal_raig));
      nfn->AddNSP(nsp);
    }
    network->append(nfn);
    delete [] adj_nodes;    delete [] aggtok;
    delete [] lport;        delete [] rport;

    return network;
}
#endif
//
// the network contains 10 fake_node's
//
list<NPFloodVisitor *> *make_NPFlood(list<fake_node *> *network,
				     int seq_num,
				     u_char *base_nid)
{
    list<NPFloodVisitor *> *NPFloodlist = new list<NPFloodVisitor *>;
    ig_ptse *hl_ptse     = 0;
    ig_nodal_info_group *nodal_ig = 0;
    int ttl = 10;		// ptse time to live -
				// as long as this is not 0
				// it is not important to this program

    // nodal_ig
    // nsap = 20 byte nsap
    // leadership_pri = 0
    // flags = 0
    // ppgl = preferred peer group leader 
    u_char leadership_pri = 0;
    u_char flags = 0;
    u_char *ppgl=0;
    u_char *nsap=0;
    ig_ptse *nodal_ptse  = 0;
    u_char *nid;
    base_nid[14] = base_nid[18] = base_nid[19] = base_nid[20] = 0;
    u_char parent[22];
    fake_node *fn;
    // no attempt is made to use ptsp's efficnetly. In fact they are
    // used very ineffiently (One ptse per ptsp)
    // loop over nodes

    cout << "--------- seq_num = " << seq_num << " ----------" << endl;
    int id = 2;
    nodal_ptse  =
      new ig_ptse(InfoGroup::ig_nodal_hierarchy_list_id,
		  id++, seq_num, 0, ttl);
    nodal_ptse->AddIG(nhl);
    // remember that the first fake node is US
    fn = network->inf(network->first());
    nid = (u_char *)fn->_mynodeid->GetNID();
    PTSPPkt *pstp = new PTSPPkt(nid, nid+1);
    pstp->AddElement (nodal_ptse);
    NPFloodVisitor *npflood = new NPFloodVisitor(pstp);
    NPFloodlist->append(npflood);
    list_item net_item;
    forall_items(net_item, *network) {
      fn = network->inf(net_item);
	nodal_ptse  =
	  new ig_ptse(InfoGroup::ig_nodal_info_group_id,
		      id++, seq_num, 0, ttl);
	nid = (u_char *)fn->_mynodeid->GetNID();
	nsap = nid + 2;

	bcopy(nid, parent, 22);
	parent[1] = nid[0];
	parent[0] = nid[0] - 8;

	ig_next_hi_level_binding_info *nhlb =
	  new ig_next_hi_level_binding_info(parent, (char *)0 );
	nodal_ig = new ig_nodal_info_group(nsap, leadership_pri,
					   flags, ppgl, nhlb);
	nodal_ptse->AddIG(nodal_ig);

	PTSPPkt *pstp = new PTSPPkt(nid, nid+1);
	pstp->AddElement (nodal_ptse);
	NPFloodVisitor *npflood = new NPFloodVisitor(pstp);
	NPFloodlist->append(npflood);

	// loop over horizontal links
	// all defined links use "normal_raig"

	int admin_weight = normal_raig._admin_weight;
	int max_cell_rate = normal_raig._max_cell_rate;
	int avail_cell_rate = normal_raig._avail_cell_rate;
	int cell_transfer_delay = normal_raig._cell_transfer_delay;
	int cell_delay_var = normal_raig._cell_delay_var;
	short clr0 = normal_raig._clr0;
	short clr01= normal_raig._clr01;

	// loop over links to adjacent nodes
    	NodeID *adj_node=0;
	linkto *thelink=0;
	list_item link_item;
	forall_items(link_item, fn->_adj_link) {
	  
	  thelink =  fn->_adj_link.inf(link_item);
	  adj_node = thelink->_remote_nodeid;
	  short int sflg = 0;
	  NodeID *rnode = adj_node;
	  u_char *rnid =  (u_char *)adj_node->GetNID();
	  int remote_port = thelink->_remote_port;
	  int local_port  = thelink->_local_port; 
	  u_int aggr_token= thelink->_aggtok;
	  ig_resrc_avail_info::resrc_type x;
	  int ig_type=128;

	  ig_resrc_avail_info *raig = 
	    new ig_resrc_avail_info(x, 0x800, admin_weight,
				    max_cell_rate, avail_cell_rate,
				    cell_transfer_delay,
				    cell_delay_var,
				    clr0, clr01);
	  if ( nid[0] == rnid[0] ) { // same level

	    hl_ptse  =
	      new ig_ptse(InfoGroup::ig_horizontal_links_id,
			  id++, seq_num, 0, ttl);

	    ig_horizontal_links *hl =
	      new ig_horizontal_links(sflg, rnode,
				      remote_port, local_port,
				      aggr_token);
	    hl_ptse->AddIG(hl);
	    
	    if(ig_type == 128) x = ig_resrc_avail_info::outgoing;
	    else	       x = ig_resrc_avail_info::incoming;
	    hl->AddRAIG(raig);
	    hl_ptse->AddIG(hl);


	  } else if (nid[0] > rnid[0] ) {

	     hl_ptse  =
	       new ig_ptse(InfoGroup::ig_uplinks_id, id++,seq_num, 0, ttl);
	    ig_uplinks *ul =
	      new ig_uplinks(0, rnid, 0,  local_port, aggr_token, 0);
	    
	    ul->AddIG(raig);
	    hl_ptse->AddIG(ul);
	  }

	  PTSPPkt *pstp = new PTSPPkt(nid, nid+1);
	  pstp->AddElement (hl_ptse);
	  NPFloodVisitor *npflood = new NPFloodVisitor(pstp);
	  NPFloodlist->append(npflood);
	    
	}// end loop over links

	// loop over nodal state parameters
	if (fn->_nsps.size()) {
	  PTSPPkt *pstp = new PTSPPkt(nid, nid+1);
	  ig_ptse *nsp_ptse = new ig_ptse(InfoGroup::ig_nodal_state_params_id,
		      id++, seq_num, 0, ttl);
	  while (fn->_nsps.size()) {
	    ig_nodal_state_params *nsp = fn->_nsps.pop();
	    nsp_ptse->AddIG(nsp);
	  }
	  pstp->AddElement (nsp_ptse);
	  NPFloodVisitor *npflood = new NPFloodVisitor(pstp);
	  NPFloodlist->append(npflood);
	}
    } // loop over fake nodes in network
    return NPFloodlist;
}


#define TEST_IG_EVENT 3020	// Run TestSwitch() once
#define STOP_IG_EVENT 3099	// Stop the simulation

// Logos_Helper is a friend class to Logos

class Logos_Helper {

private:
    int _num_adj;
    NodeID *_this_switch;
    list<NodeID *> _adj_switches;
    DebugTerminal *_theTopDebugTerminal;
    Conduit *_theTop;
    Conduit *_theGcac;
    Conduit *_theDatabase;
    Conduit *_theBot;
    Logos *_gcac;
    double _interval;
    int _valid_nodes;
public:

    Logos_Helper (NodeID * switch_id);

    ~Logos_Helper() { 
  
      delete _theTop;

      delete _theGcac;
      delete _theDatabase;  //
      delete _theBot;
      delete _this_switch;
    }

    void  build_network()
    {
      static int seq_num=0;
      
      list <fake_node *> *network = make_network(bnid);
      
      list<NPFloodVisitor *> *npfv_list =
	make_NPFlood(network, seq_num++, bnid);

      cout << "generated " << npfv_list->size() << " NpFloodVisitors\n";
      NPFloodVisitor *npfv;
      while ( npfv_list->size() ) {
	npfv= npfv_list->pop(); 
	_theTopDebugTerminal->Inject((Visitor *)npfv);
      }
      delete npfv_list;
      while (network->size()) {
	fake_node *fn = network->pop();
	delete fn;
      }
      delete network;
    }
	
    void Boot(void)
    {
      const long BANDWIDTH = 121855;
      FastUNIVisitor * fuv=0; 

      cout << "TEST_IG_EVENT!  _valid_nodes = " << _valid_nodes << endl;
      build_network();
#if 1
      // create a SaveVisitor for Logos to dump _graph to stdout
      cout << "============================================" <<endl;
      _gcac->SaveGraph("/dev/tty");
      cout << "============================================" <<endl;
      cout << endl;
	  
      // create a ShowVisitor for Logos to run $GTbin on
//       char *user=getenv("USER");
//       char rec[80];
//       sprintf(rec,"/tmp/igtest.%s",user);
//       _gcac->ShowGraph(rec);
exit(0);
#endif
#if !SIMPLE_NETWORK
      cout << "----- begin nodal state route test ------" << endl;
      // create a FastUNIVisitor to test Logos routing
      // dest was picked in make_network (!SIMPLE_NETWORK)
      fuv =new FastUNIVisitor(_this_switch, dest, 0, 5, 1, 1, 
			      FastUNIVisitor::FastUNISetup, 0, BANDWIDTH);

      _theTopDebugTerminal->Inject((Visitor *)fuv);
      cout << "-----  end  nodal state route test ------" << endl;
      cout << "----- begin nodal state route test ------" << endl;
      // create a FastUNIVisitor to test Logos routing
      // dest was picked in make_network (!SIMPLE_NETWORK)
      fuv =new FastUNIVisitor(_this_switch, dest, 0, 5, 1, 1, 
			      FastUNIVisitor::FastUNISetup, 0, BANDWIDTH);

      _theTopDebugTerminal->Inject((Visitor *)fuv);
      cout << "-----  end  nodal state route test ------" << endl;
#endif
#if !SIMPLE_NETWORK
      cout << "----- begin   logical   route test ------" << endl;      
      // try injecting at border node level
      fuv = new FastUNIVisitor(_this_switch, dest, 0, 5, 1, 1, 
			       FastUNIVisitor::FastUNISetup, 0, BANDWIDTH);
      fuv->Append(logn0, 103);	// add logical node 0 port 3 to route
      fuv->Append(logn6, 67);
      fuv->Append(logn7, 0);
      
      _theTopDebugTerminal->Inject((Visitor *)fuv);
      cout << "-----  end    logical   route test ------" << endl;
#endif
#if 0
      cout << "----- begin   memory    leak  test ------" << endl;
      // look for NodeID leaks
      NodeID *n = new NodeID();
      _gcac->Clear ();
      cout << "before Update () ref count =     " << n->GetRefcnt() <<endl;
      _gcac->Update ();
      cout << "after  Update () ref count =" << n->GetRefcnt() <<endl;
      _gcac->Clear();
      cout << "after   Clear()  ref count =     " << n->GetRefcnt() <<endl;
      _gcac->Update ();
      cout << "after  Update () ref count =" << n->GetRefcnt() <<endl;
      _gcac->Clear();
      cout << "after   Clear()  ref count =     " << n->GetRefcnt() <<endl;
      _gcac->Update ();
      cout << "after  Update () ref count =" << n->GetRefcnt() <<endl;
      _gcac->Clear();
      cout << "after   Clear()  ref count =     " << n->GetRefcnt() <<endl;
      delete n;
      cout << "-----  end    memory    leak  test ------" << endl;
#endif
    }

};


Logos_Helper::Logos_Helper (NodeID *switch_id)
   : _valid_nodes(NUM_NODES-1)
{
    bool answer = false;
    _this_switch = new NodeID(*switch_id);

    // build the stack:
    //
    //  [Bot (A)] <--> [(B) Database (A)] <--> [(B) Logos (A)] <--->[(A) Top]
    //
    // the order of Database and gcac is not particualrly important
    // since Gcac ignores all the PTSEs and Database ignores the FastUNISetup
    
    DebugTerminal *theBotDebugTerminal = new DebugTerminal(_this_switch);
    Adapter *theBotAdapter = new Adapter(theBotDebugTerminal);
    _theBot = new Conduit("Bot", theBotAdapter);
    
    Database *db = new Database(_this_switch);
    assert(db);
    Protocol *protodb = new Protocol(db);
    _theDatabase = new Conduit("Database", protodb);
    
    _gcac = new Logos(db);
    assert(_gcac);
    Protocol *protogcac = new Protocol(_gcac);
    _theGcac = new Conduit("Gcac", protogcac);
    
    _theTopDebugTerminal = new DebugTerminal(_this_switch);
    Adapter *theTopAdapter = new Adapter(_theTopDebugTerminal);
    _theTop = new Conduit("Top", theTopAdapter);

    // join the pieces
    if (Join(A_half(_theTop), A_half(_theGcac), 0, 0)) {
      if (Join(B_half(_theGcac), A_half(_theDatabase), 0, 0)) {
	if (Join(B_half(_theDatabase), A_half(_theBot), 0, 0)) {
	  answer = true;
	} else {
	  cout << "Can't join Database to bottom" << endl;
	}
      } else {
	cout << "Can't join Logos to Database" << endl;
      }
    } else {
	cout << "Can't join Top to Logos" << endl;
    }

    assert(answer);
}

int main (int argc, char** argv)
{
    NodeID *switch_id=0;
    bnid[14] = bnid[18] = bnid[19] = bnid[20] = 1;
    switch_id = new NodeID(bnid);
    DiagLevel(SIM, DIAG_FATAL);
    DiagLevel("sim.logos", DIAG_DEBUG);
    VisPipe("/dev/null"); // discard the log of NPFloodVisitors
    Logos_Helper *tast = new Logos_Helper( switch_id);

    tast->Boot();
    
    delete tast;

    // cout << endl << "Recording sw's History in conduit.rec" << endl;
    // ofstream of("conduit.rec");
    //    DumpRecord(of);
    DeleteFW();

    exit(0);
}
#include <DS/containers/list.cc>
template class list<fake_node *> ;
template class list<linkto *> ;

int compare(fake_node *const &lhs, fake_node *const &rhs)
{
  int ret;
  if ((long)lhs <  (long)rhs) ret = -1;
  else if ((long)lhs == (long)rhs) ret = 0;
  else  ret = +1;
  return ret;
}

int compare(linkto *const &lhs, linkto *const &rhs)
{
  int ret;
  if ((long)lhs <  (long)rhs) ret = -1;
  else if ((long)lhs == (long)rhs) ret = 0;
  else  ret = +1;
  return ret;
}

int compare(NPFloodVisitor *const &lhs, NPFloodVisitor *const &rhs)
{
  int ret;
  if ((long)lhs <  (long)rhs) ret = -1;
  else if ((long)lhs == (long)rhs) ret = 0;
  else  ret = +1;
  return ret;
}
