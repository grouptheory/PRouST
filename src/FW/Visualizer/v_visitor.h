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
#ifndef __V_VISITOR_H__
#define __V_VISITOR_H__

#ifndef LINT
static char const _v_visitor_h_rcsid_[] =
"$Id: v_visitor.h,v 1.1 1999/01/20 22:58:05 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
#include <qcolor.h> 

class v_conduit;
class v_visitor;
class v_visitor_registry;
class v_visitor_colors_table;

#define VIS_STRSIZE 255

//-----------------------------------------------------
class v_visitor {
  friend class v_visitor_registry;
public:

  v_conduit  * CurrentLocation(void) const;
  void         SetLocation(v_conduit * c);
  const char * GetName(void) const;
  const char * GetType(void) const;

private:

  v_visitor(char * visname, char * typ);
  ~v_visitor();

  char         _visname[VIS_STRSIZE];
  char         _typename[VIS_STRSIZE];
  int          _colorcode;
  v_conduit * _current_location;
};

//-----------------------------------------------------
class v_visitor_registry {
  friend v_visitor_registry * VisitorRegistry(void);
  friend class NV_fw_state;
  friend class NV_application;
public:

  v_visitor * demand(char * visname, char * type);
  v_visitor * query(char * visname, char * type = 0);
  bool type_is_active(const char * type) const;
  bool destroy(char * visname);
  bool destroy(v_visitor * vis);
  bool lookup(v_visitor * vis);

  const dictionary<char *, v_visitor *> * GetVisitors(void) const;
  bool  visible(const char * visitor_type) const;
  void  ignore(const char * visitor_type);
  void  view(const char * visitor_type);

private:

  void clear(void);

  v_visitor_registry(void);
  ~v_visitor_registry();

  dictionary<char *, v_visitor *> _population;
  list<char *>                     _ignored_vistypes;
  static v_visitor_registry     * _singleton;
};

//-----------------------------------------------------
class v_visitor_colors_table {
  friend class NV_fw_state;
  friend class v_visitor;
  friend v_visitor_colors_table * VisitorColorsTable(void);
public:

  bool   register_visitor_color(char * type, QColor color);
  QColor lookup_visitor_color(const char * type);
  const dictionary<char *, QColor> * color_table(void) const;

private:

  v_visitor_colors_table(void);
  ~v_visitor_colors_table();

  dictionary<char *, QColor>       _colorcodes;
  static v_visitor_colors_table * _singleton;
};

#endif
