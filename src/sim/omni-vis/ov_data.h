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
#ifndef __OV_DATA_H__
#define __OV_DATA_H__
#ifndef LINT
static char const _ov_data_h_rcsid_[] =
"$Id: ov_data.h,v 1.3 1999/02/01 14:26:02 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>

class levelSubgraph;
class NodeID;
class Event;
class OVNode;

//----------------------------------------------
class ov_data {
public:

  ov_data(void);
  ~ov_data();

  void clear(void);
  bool empty(void) const;

  void load(const char * filename);
  void save(const char * filename);
  bool layout(void);

  Event *  notify( double time,
		   const char * type,
		   const char * loc,
		   const char * misc,
		   NodeID * n );

  OVNode * notify( const char * name, bool pgl = false );
  void     remove( const char * name );

  const list< OVNode * >                         * getNodes(void)  const;
  const dictionary< double , list< Event * > * > * getEvents(void) const;

  const OVNode * find( const char * name ) const;

  OVNode *       nearest_node(double x, 
			      double y, 
			      double z, 
			      double & minDist);
private:

  bool layoutLevelsDependent(void);
  bool layoutLevelsIndependent(void);
  void allocateLevels(void);
  void deallocateLevels(void);

  // ---- temporary storage of state for ----
  //       use during layout procedures

  dictionary< int, levelSubgraph* > _levels;


  // ---- actual storage of state is here ----

  // all events, keyed by time
  dictionary< double, list< Event * > * > _events;

  // all nodes at all levels
  list< OVNode * >                        _nodes;
};

#endif // __OV_DATA_H__
