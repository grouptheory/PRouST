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
static char const _ov_data_cc_rcsid_[] =
"$Id: ov_data.cc,v 1.8 1999/02/02 16:58:21 mountcas Exp $";
#endif

#include "ov_data.h"
#include "Event.h"
#include "OVNode.h"
#include "levelSubgraph.h"

#include <common/cprototypes.h>
#include <fstream.h>
#include <math.h>

#include <codec/pnni_ig/id.h>


//----------------------------------------
// Methods of class 'ov_data'
// ---------------------------------------

ov_data::ov_data(void) { }

//----------------------------------------
ov_data::~ov_data() 
{ 
  list_item li;
  dic_item di;

  forall_items(di, _events) {
    forall_items(li, *(_events.inf(di)))
      delete (_events.inf(di))->inf(li);

    // delete the list
    delete _events.inf(di);
  }
  _events.clear();

  forall_items(li, _nodes)
    delete _nodes.inf(li);
  _nodes.clear();
}

//----------------------------------------
void ov_data::clear(void)
{
  dic_item di;
  forall_items(di, _events)
    delete _events.inf(di);
  _events.clear();

  list_item li;
  forall_items(li, _nodes)
    delete _nodes.inf(li);
  _nodes.clear();
}

//----------------------------------------
bool ov_data::empty(void) const
{
  return (_events.empty() || _nodes.empty());
}

//----------------------------------------
void ov_data::save(const char * filename)
{
  // save the layout data for the nodes, ignore the events data
  ofstream out( filename );

  list_item li;
  forall_items(li, _nodes) {
    OVNode * ovn = _nodes.inf( li );

    out << ovn->getName() << " (" 
	<< ovn->getX() << ", " 
	<< ovn->getY() << ", "
	<< ovn->getZ() << ")" << endl;
  }
}

// ---------------------------------------
void ov_data::load(const char * filename)
{
  ifstream in( filename );

  while ( in.eof() != true ) {
    char buf[1024], name[128];
    double x, y, z;

    in.getline( buf, 1024 );

    if ( sscanf( buf, "%s (%lf, %lf, %lf)", name, &x, &y, &z ) < 4 ) {
      cerr << "Error: in ov_data::load " << endl;
    } else {
      OVNode * ovn = (OVNode *)find( name );
      if ( ovn != 0 ) {
	ovn->setX( x );
	ovn->setY( y );
	ovn->setZ( z );
      } else
	cerr << "ov_data::load unable to locate " << name << "!" << endl;
    }
  }
}


//----------------------------------------
bool ov_data::layout(void)
{
  allocateLevels();
  layoutLevelsIndependent();
  layoutLevelsDependent();
  deallocateLevels();

  return true;
}

//----------------------------------------
void ov_data::allocateLevels(void) 
{
  _levels.clear();

  for (int level = 96; level >= 8; level -= 8) {
    levelSubgraph* subg = new levelSubgraph(this,level);
    _levels.insert( level, subg );
  }
}

//----------------------------------------
void ov_data::deallocateLevels(void) {
  for (int level = 96; level >= 8; level -= 8) {
    dic_item di = _levels.lookup(level);
    assert(di);
    levelSubgraph* subg = _levels.inf(di);
    delete subg;
    _levels.del_item(di);
  }
  _levels.clear();
}

//----------------------------------------
bool ov_data::layoutLevelsIndependent(void)
{  
  for (int level = 96; level >= 8; level -= 8) {
    dic_item di = _levels.lookup(level);
    assert(di);
    levelSubgraph* subg = _levels.inf(di);
    subg->computeConnectedComponents(); 
    subg->layout();
  }
  return true;
}

//----------------------------------------
bool ov_data::layoutLevelsDependent(void)
{ 
  return true;
}

//----------------------------------------
Event * ov_data::notify( double time,
			 const char * type,
			 const char * loc,
			 const char * misc,
			 NodeID * n )
{
  dic_item di;
  list< Event * > * elst = 0;

  if (!(di = _events.lookup( time )))
    di = _events.insert( time, new list< Event * > );
  elst = _events.inf(di);

  Event * newE = new Event( time, type, loc, misc, n );
  if ( elst->lookup( newE ) == 0 )
    elst->insert( newE );
  else {
    delete newE; newE = 0;
  }
  return newE;
}

//----------------------------------------
OVNode * ov_data::notify(const char * n, bool pgl)
{
  OVNode * newN = new OVNode(n);
  assert( _nodes.lookup( newN ) == 0 );
  _nodes.insert( newN );
  if (pgl) newN->setPGL(pgl);
  return newN;
}      

//----------------------------------------
void ov_data::remove(const char * name)
{
  OVNode n(name);
  list_item li = _nodes.lookup( &n );
  assert( li != 0 );
  delete _nodes.inf(li);
  _nodes.del_item(li);
}

//----------------------------------------
const OVNode * ov_data::find(const char * name) const
{
  OVNode * r = 0;

  list_item li;
  forall_items(li, _nodes) {
    OVNode * n = _nodes.inf(li);
    if (! strncmp( n->getName(), name, strlen(n->getName()) )) {
      r = n;
      break;
    }
  }
  return r;
}

//----------------------------------------
const list< OVNode * > * ov_data::getNodes(void) const
{
  return &_nodes;
}

//----------------------------------------
const dictionary< double , list< Event * > * > * ov_data::getEvents(void) const
{
  return &_events;
}

//----------------------------------------
OVNode * ov_data::nearest_node(double x, double y, double z, double & minDist)
{
  assert(minDist > 0);

  OVNode * rval = 0;

  list_item li;
  forall_items(li, _nodes) {
    OVNode * tmp = _nodes.inf(li);
    
    double dx = x - tmp->getX();
    double dy = y - tmp->getY();
    double dz = z - tmp->getZ();

    double dist = sqrt( dx * dx + 
			dy * dy +
			dz * dz );

    if ((dist >= 0) && (dist < minDist)) {
      rval = tmp;
      minDist = dist;
    }
  }
  return rval;
}

//---------------------------------------------------------
// in desperate times, mortals are driven to desperate acts

void ovmemchk(void) {
  for (int i=1;i<4096;i++) {
    char* f = new char [i];

    for (int j=0;j<i;j++)
      f[j]=-1;

    delete [] f;
  }
}
