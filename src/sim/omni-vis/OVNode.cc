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
static char const _OVNode_cc_rcsid_[] =
"$Id: OVNode.cc,v 1.4 1999/02/02 20:47:46 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "OVNode.h"

//----------------------------------------
// Methods of class OVNode
//----------------------------------------

OVNode::OVNode( const char * name )
  : _name(0), _pgl(false), 
    _x(-1000.0), _y(-1000.0), _z(-1000.0), 
    _rank( -1 ), _leader(0),
    _jx(-1000.0), _jy(-1000.0), 
    _initialized(false)
{
  assert( name != 0 );
  _name = new char [ strlen(name) + 1];
  strcpy(_name, name);

  char buf[128], * s = 0;
  strcpy(buf, _name);
  if ( (s = strchr(buf, ':')) != 0 )
    *s = '\0';

  _level=atoi(buf);
}

//----------------------------------------
OVNode::~OVNode( )
{
  delete [] _name;
  _edges.clear();
}

//----------------------------------------
const char * OVNode::getName(void) const
{
  return _name;
}

//----------------------------------------
int OVNode::getLevel(void) const
{
  return _level;
}

//----------------------------------------
bool OVNode::isPGL(void) const
{
  return _pgl;
}

//----------------------------------------
void OVNode::setPGL(bool b)
{
  _pgl = b;
}

//----------------------------------------
void OVNode::setRank( int r )
{
  _rank = r;
}

//----------------------------------------
int OVNode::getRank( void ) const
{
  return _rank;
}

//----------------------------------------
const list< OVNode * > * OVNode::getEdges(void) const
{
  return &_edges;
}

//----------------------------------------
void OVNode::addEdge(const OVNode * neighbor)
{
  // This not an invariant because events like Hlink_Aggr 
  // can appear multiple times, each time resulting in an 
  // add edge of the very same edge.
  if ( _edges.lookup((OVNode *)neighbor) == 0 )
    _edges.insert((OVNode *)neighbor);
}

//----------------------------------------
void OVNode::remEdge(const OVNode * neighbor)
{
  list_item li = _edges.lookup((OVNode *)neighbor);
  assert( li != 0 );
  _edges.del_item(li);
}

//----------------------------------------
void OVNode::setLeader(const OVNode * neighbor)
{
  _leader = neighbor;
}

const OVNode * OVNode::getLeader(void) const
{
  return _leader;
}
