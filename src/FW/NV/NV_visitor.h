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
#ifndef __NV_VISITORS_H__
#define __NV_VISITORS_H__

#ifndef LINT
static char const _NV_visitor_h_rcsid_[] =
"$Id: NV_visitor.h,v 1.1 1999/01/21 14:55:00 bilal Exp $";
#endif

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
#include <qcolor.h> 

class NV_conduit;
class NV_visitor;
class NV_visitor_registry;
class NV_visitor_colors_table;
class NV_fw_state;
class NV_application;

#define VIS_STRSIZE 255

//-----------------------------------------------------
class NV_visitor {
  friend class NV_visitor_registry;
public:

  NV_conduit * CurrentLocation(void) const;
  void         SetLocation(NV_conduit * c);
  const char * GetName(void) const;
  const char * GetType(void) const;

private:

  NV_visitor(char * visname, char * typ);
  ~NV_visitor();

  char         _visname[VIS_STRSIZE];
  char         _typename[VIS_STRSIZE];
  int          _colorcode;
  NV_conduit * _current_location;
};

//-----------------------------------------------------
class NV_visitor_registry {
  friend NV_visitor_registry * VisitorRegistry(void);
  friend class NV_fw_state;
  friend class NV_application;
public:

  NV_visitor * demand(char * visname, char * type);
  NV_visitor * query(char * visname, char * type = 0);
  bool type_is_active(const char * type) const;
  bool destroy(char * visname);
  bool destroy(NV_visitor * vis);
  bool lookup(NV_visitor * vis);

  const dictionary<char *, NV_visitor *> * GetVisitors(void) const;
  bool  visible(const char * visitor_type) const;
  void  ignore(const char * visitor_type);
  void  view(const char * visitor_type);

private:

  void clear(void);

  NV_visitor_registry(void);
  ~NV_visitor_registry();

  dictionary<char *, NV_visitor *> _population;
  list<char *>                     _ignored_vistypes;
  static NV_visitor_registry     * _singleton;
};

//-----------------------------------------------------
class NV_visitor_colors_table {
  friend class NV_fw_state;
  friend class NV_visitor;
  friend NV_visitor_colors_table * VisitorColorsTable(void);
public:

  bool   register_visitor_color(char * type, QColor color);
  QColor lookup_visitor_color(char * type);
  const dictionary<char *, QColor> * color_table(void) const;

private:

  NV_visitor_colors_table(void);
  ~NV_visitor_colors_table();

  dictionary<char *, QColor>       _colorcodes;
  static NV_visitor_colors_table * _singleton;
};

#endif
