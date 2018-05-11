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
#ifndef __LEVELREGISTRY_H__
#define __LEVELREGISTRY_H__
#ifndef LINT
static char const _levelRegistry_h_rcsid_[] =
"$Id: levelRegistry.h,v 1.2 1999/01/05 16:16:27 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
class levelRegistry;

class levelRegistry {
  friend class ov_main;
public:
  friend levelRegistry* theLevelRegistry(void);

  bool isSelected(int level) const;

private:
  void setSelected(int level, bool val);
  void setSelectedAll(void);
  void setSelectedNone(void);

  levelRegistry(void);
  ~levelRegistry(void);

  dictionary<int,int> _state;
  bool                _all_selected;
  bool                _none_selected;

  static levelRegistry* _singleton;
};

#endif // __LEVELREGISTRY_H__
