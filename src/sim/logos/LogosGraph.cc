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
static char const _LogosGraph_cc_rcsid_[] =
"$Id: LogosGraph.cc,v 1.21 1999/01/21 22:00:06 marsh Exp $";
#endif

#include <common/cprototypes.h>
#include <sim/logos/Logos.h>
#include <sim/logos/LogosGraph.h>
#include <sim/logos/atoms.h>
#include <fstream.h> // for ShowLogosGraph

LogosGraph::LogosGraph (int lowest_level) : _level(lowest_level)
{
  _creator = 0;
}

LogosGraph::LogosGraph(NodeID *creator = 0)
{
  if (creator) _creator = new NodeID (*creator);
  else _creator = 0;
}
LogosGraph::~LogosGraph ( ) 
{
  dic_item di;

  if (_creator) delete _creator;


  while (  di = _nid_to_node.first()) {
    delete (NodeID *)_nid_to_node.key(di);
    _nid_to_node.del_item(di);
  }
  _nid_to_node.clear();

  // delete contents of _graph

  node n;
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
      delete le;
      _graph.del_edge(e);
      delete e;
    }

    while ( (e = n->first_edge()) ) {
      node to = _graph.target(e);
      to->del_edge(e);
      n->del_edge(e);
      logos_edge *le = _graph[e];
      delete le;
      _graph.del_edge(e);
      delete e;
    }
    
    logos_node * ln = _graph[n];
    _graph.del_node(n);
    
    Transit * tr_key = ln->TakeTransit(); // zeros _transit in ln
    NodeID *tr_nid=0;
    di = _Transit_to_node.lookup(tr_key);
    Transit *key1 =  _Transit_to_node.key(di);
    delete key1;
    _Transit_to_node.del_item(di);

    delete ln;
    delete n;
  }

  _Transit_to_node.clear();

  // clear out _port_to_NodeID
  while (! _port_to_NodeID.empty()) {
    di = _port_to_NodeID.first(); 
    list<NodeID *> * lnid = _port_to_NodeID.inf(di);
    if (lnid) {
      list_item li;
      while (lnid->size()) {
	NodeID *nid = lnid->pop();
	delete nid;
      }
    }
    _port_to_NodeID.del_item(di);
  }
}

node LogosGraph::insert_node(NodeID * nid, int logical_port)
{
  assert(nid);
  logos_node * ln = new logos_node(nid, logical_port);
  node n = _graph.new_node(ln);
  Transit *tr = new Transit(*ln->ShareTransit());
  _Transit_to_node.insert(tr, n);
  _nid_to_node.insert(nid->copy(), n);

  return n;
}

// copy logos node [from another graph] and insert into _graph
node LogosGraph::insert_logosnode_copy(logos_node *ln)
{
  logos_node * copy = ln->copy();
  assert(ln->ShareNID());
  node n = _graph.new_node(copy);
  _Transit_to_node.insert(copy->ShareTransit(), n);

  if (ln->LogicalPort() == 0)
    _nid_to_node.insert(copy->ShareNID()->copy(), n);

  return n;
}

node LogosGraph::GetNode(const NodeID * nid)
{
  dic_item di;
  node n = 0;
  NodeID *znid = Logos_NodeID(nid);

  if ( (di = _nid_to_node.lookup(znid)) )
    n= _nid_to_node.inf(di);

  delete znid;
  return n;
}

void LogosGraph::FinNodes()
{
  logos_node * ln;
  node n;
  
  forall_nodes(n, _graph) {
    ln = _graph[n];
    ln->Node(n);
  }
}

void LogosGraph::SetReversePaths(void)
{
  edge fe;

  FinNodes();
  
  forall_edges (fe, _graph) {
    logos_edge * le = _graph[fe];
    if (le->Reverse() == 0) {
      // le goes from _start to _end
      // find el that goes from _end to _start using same ports
      logos_node * Start_of_Reverse = le->GetEnd();
      edge re;
      node rn = Start_of_Reverse->Node();
      forall_out_edges(re, rn) {
	logos_edge * el = _graph[re];
	if ( el->Reverse(le) ) {
	  // _reverse is set in Reverse()
	  break;
	}
      }
    }
  }
}

// fill in _port_to_NodeID -- called from Logos::GetGraph(NodeID *);
// a NodeID can represent more than one Logtical Ports at a higher level
// a logical port can be represented by more than one NodeID
//
// return true if inserted, false if already present
//
bool LogosGraph::insert_logical_port(int logical_port, const NodeID *nid)
{
  bool answer = false;
  // all NodeIDs are copied before insertion
  assert(nid!=0);
  
  dic_item di;
  di = _port_to_NodeID.lookup(logical_port);
  if (di) {
    // find existing list and insert copy of nid if missing
    list<NodeID *> *lnid = _port_to_NodeID.inf(di);
    if (lnid->search((NodeID *)nid) == 0 ) {
      lnid->append(new NodeID(*nid));
      answer = true;
    }
  } else {
    // create a new list and enter into dictionary
    list<NodeID *> *lnid = new list<NodeID *>;
    lnid->append(new NodeID(*nid));
    _port_to_NodeID.insert(logical_port, lnid);
      answer = true;
  }
  return answer;
}

list<NodeID *> * LogosGraph::LookupPort_Shared(int logical_port)
{
  list<NodeID *> *lnid = 0;

  dic_item di = _port_to_NodeID.lookup(logical_port);
  if (di) 
    lnid = _port_to_NodeID.inf(di);
  return lnid;
}

// not called by code. Useful when debugging BaseLogos 
void LogosGraph::ShowLogosGraph(char *filename)
{
  ofstream of(filename);

  node n;
  forall_nodes(n, _graph) {
    logos_node * ln = _graph[n];
    of << "!birth "<< *ln->ShareNID()<<":"<<ln->LogicalPort()<<" !Switch"<< endl;
  }
  edge e;
  forall_edges(e, _graph) {
    logos_edge * le = _graph[e];
    of << "!connected "
       << *le->GetStart()->ShareNID() <<":"<<le->GetStart()->LogicalPort()<< " 3"
       << " !to "
       << *le->GetEnd()->ShareNID()   <<":"<< le->GetEnd()->LogicalPort() << " 3"
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
	 cout << "LogosGraph::ShowLogosGraph("<< filename <<"): error" << endl;
	 cout << "       GTbin not set in environment" << endl;
	 cout << " output is in " << filename<< endl;);
      
#endif
  }
}

void LogosGraph::SaveLogosGraph(char *file_name)
{
  _graph.write(file_name);
}

void LogosGraph::PrintNodes(ostream & os) const
{
  node n;
  forall_nodes(n, _graph) {
    logos_node * ln = _graph[n];

    os << *(ln->ShareNID()) << endl;
    //    os << *(ln->ShareTransit()->ShareNID()) << endl << endl;
  }
}

void LogosGraph::PrintEdges(ostream & os) const
{
  edge e;
  forall_edges(e, _graph) {
    logos_edge * le = _graph[e];
    logos_node * s = le->GetStart(),
               * e = le->GetEnd();

    os << *(s->ShareNID()) << " -- " << *(e->ShareNID()) << endl;
  }
}

void LogosGraph::PrintTransits(ostream & os) const
{
  dic_item di;
  Transit *tr;
  forall_items(di, _Transit_to_node) {
    tr = _Transit_to_node.key(di);

    os << "Tr: " << tr << ", nid=" << tr->ShareNID()
       << ", port= " <<tr->GetPort()<< endl;
  }
}

// just prints NodeID's in this dictionary
void LogosGraph::Print_nid_to_node(ostream & os) const
{
  os << "NodeIDs in LogosGraph dictionary _nid_to_node" << endl;
  dic_item di;
  forall_items(di,  _nid_to_node) {
    const NodeID * nid = _nid_to_node.key(di);
    os << "    "<< *nid << endl;
  }
}

void LogosGraph::Print_port_to_NodeID(ostream & os=cout) const
{
  if (_creator) {
    os << "LogosGraph _port_to_NodeID created for " << *_creator << endl;
    dic_item di;
    forall_items(di, _port_to_NodeID) {
      int logical_port = _port_to_NodeID.key(di);
      os << "  logical port " << logical_port << ": ";
      list<NodeID *> *lnid = _port_to_NodeID.inf(di);
      list_item li = lnid->first();
      NodeID *nid = 0;
      if (li) {
	nid = lnid->inf(li);
	os << *nid << endl;
	li = lnid->succ(li);
      }
      while (li) {
	nid = lnid->inf(li);
	li = lnid->succ(li);
	os << "                  " << *nid << endl;
      }
    }
  }
}
