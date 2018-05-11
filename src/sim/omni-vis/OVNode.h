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
#ifndef __OVNODE_H__
#define __OVNODE_H__
#ifndef LINT
static char const _ovnode_h_rcsid_[] =
"$Id: OVNode.h,v 1.2 1999/01/04 17:07:32 mountcas Exp $";
#endif

class NodeID;
#include <GL/gl.h>

#include <DS/containers/list.h>

//----------------------------------------------
class OVNode {
public:

  OVNode( const char * name );
  ~OVNode( );

  const char   * getName(void) const;
  bool           isPGL(void) const;

  void           setPGL(bool b);
  void           setRank(int r);
  int            getRank(void) const;

  const list< OVNode * > * getEdges(void) const;
  void  addEdge(const OVNode * neighbor);
  void  remEdge(const OVNode * neighbor);

  const OVNode * getLeader(void) const;
  void           setLeader(const OVNode * neighbor);

  GLfloat getX(void) const { return _x; }
  GLfloat getY(void) const { return _y; }
  GLfloat getZ(void) const { return _z; }

  void setX(GLfloat x) { _x = x; }
  void setY(GLfloat y) { _y = y; }
  void setZ(GLfloat z) { _z = z; }

  double getJiggleX(void) const { return _jx; }
  double getJiggleY(void) const { return _jy; }

  void setJiggleX(double x) { _jx = x; }
  void setJiggleY(double y) { _jy = y; }

  bool isInitialized(void) const { return _initialized; }
  void setInitialized(bool val) { _initialized = val; }

  int getLevel(void) const;

private:

  char           * _name;
  int              _level;
  bool             _pgl;

  const OVNode   * _leader;
  list< OVNode * > _edges;
  int              _rank;

  GLfloat  _x, _y, _z;
  double   _jx, _jy;

  bool     _initialized;
};

#endif // __OVNODE_H__
