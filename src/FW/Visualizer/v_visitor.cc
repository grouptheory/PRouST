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
static char const _v_visitor_cc_rcsid_[] =
"$Id: v_visitor.cc,v 1.1 1999/01/20 22:58:13 mountcas Exp $";
#endif

#include <common/cprototypes.h>

#include "v_visitor.h"
#include "v_conduit.h"
#include "v_app.h"
#include "v_data.h"

//-----------------------------------------------------
v_visitor::v_visitor(char * visname, char * typ) 
  : _colorcode(0), _current_location(0)
{
  assert(visname && typ);
  strcpy(_visname, visname);
  strcpy(_typename, typ);
}

v_visitor::~v_visitor() { }

v_conduit * v_visitor::CurrentLocation(void) const
{  return _current_location;  }

void         v_visitor::SetLocation(v_conduit * c)
{  _current_location = c;  }

const char * v_visitor::GetName(void) const
{  return _visname;  }

const char * v_visitor::GetType(void) const
{  return _typename;  }

//-----------------------------------------------------
v_visitor_registry * v_visitor_registry::_singleton = 0;

v_visitor_registry * VisitorRegistry(void) 
{
  if (!v_visitor_registry::_singleton)
    v_visitor_registry::_singleton = new v_visitor_registry();
  return v_visitor_registry::_singleton;
}

const dictionary<char *, v_visitor *> * v_visitor_registry::GetVisitors(void) const
{  return &_population;  }

v_visitor * v_visitor_registry::demand(char * visname, char * type) 
{
  dic_item di;
  if (!(di = _population.lookup(visname))) {
    v_visitor * nv = new v_visitor(visname, type);
    char * st = new char [ strlen(visname) + 1 ];
    strcpy(st, visname);
    di = _population.insert(st, nv);
    theApplication().getData()->notify(nv);
  }
  return _population.inf(di);
}

v_visitor * v_visitor_registry::query(char * visname, char * type) 
{
  v_visitor * rval = 0;
  dic_item di;
  if (di = _population.lookup(visname)) {
    rval = _population.inf(di);
    // If the types don't match, change it's type
    if (type && strcmp(type, rval->GetType()))
      strcpy(rval->_typename, type);
  }
  return rval;
}

bool v_visitor_registry::type_is_active(const char * type) const
{
  bool rval = false;
  dic_item di;
  forall_items(di, _population) {
    v_visitor * nv = _population.inf(di);
    if (!strcmp(type, nv->GetType())) {
      rval = true;
      break;
    }
  }
  return rval;
}

bool v_visitor_registry::destroy(char * visname) 
{
  bool rval = false;
  dic_item di;
  if (di = _population.lookup(visname)) {
    v_visitor * vis = _population.inf(di);
    theApplication().getData()->remove(vis);
    if (vis->CurrentLocation())
      vis->CurrentLocation()->rem_visitor(vis);
    char * name = _population.key(di);
    delete [] name;
    _population.del_item(di);
    rval = true;
  }
  return rval;
}

bool v_visitor_registry::destroy(v_visitor * vis)
{
  return destroy((char *)vis->GetName());
}

bool v_visitor_registry::lookup(v_visitor * vis)
{
  bool rval = false;
  dic_item di;
  forall_items(di, _population) {
    v_visitor * tmp = _population.inf(di);
    if (tmp == vis) {
      rval = true;
      break;
    }
  }
  return rval;
}

bool v_visitor_registry::visible(const char * visitor_type) const
{
  if (_ignored_vistypes.search((char *)visitor_type))
    return false;  // If its in the list, it's NOT visible (i.e. ignored)
  return true;
}

void v_visitor_registry::ignore(const char * visitor_type)
{
  if (!_ignored_vistypes.search((char *)visitor_type)) {
    char * st = new char [ strlen(visitor_type) + 1 ];
    strcpy(st, visitor_type);
    _ignored_vistypes.insert(st);
  }
}

void v_visitor_registry::view(const char * visitor_type)
{
  list_item li;
  if (li = _ignored_vistypes.search((char *)visitor_type)) {
    delete _ignored_vistypes.inf(li);
    _ignored_vistypes.del_item(li);
  }
}

v_visitor_registry::v_visitor_registry(void) { }

v_visitor_registry::~v_visitor_registry() 
{ clear(); }

void v_visitor_registry::clear(void)
{
  dic_item di;
  forall_items(di, _population) {
    // delete _population.inf(di);
    v_visitor * vis = _population.inf(di);
    if (vis->CurrentLocation())
      vis->CurrentLocation()->rem_visitor(vis);
    delete vis;
    delete _population.key(di);
  }
  _population.clear();
}

//-----------------------------------------------------
v_visitor_colors_table * v_visitor_colors_table::_singleton = 0;

v_visitor_colors_table * VisitorColorsTable(void) 
{
  if (!v_visitor_colors_table::_singleton)
    v_visitor_colors_table::_singleton = new v_visitor_colors_table();
  return v_visitor_colors_table::_singleton;
}

bool v_visitor_colors_table::register_visitor_color(char * typ, QColor color) 
{
  dic_item di;
  if (!(di = _colorcodes.lookup(typ))) {
    char * st = new char [ strlen(typ) + 1 ];
    strcpy(st, typ);
    _colorcodes.insert(st, color);
  } else
    _colorcodes.change_inf(di, color);

  return true;
}

QColor v_visitor_colors_table::lookup_visitor_color(const char * type) 
{
  // default is a random color
  QColor rval(rand() % 255,
	      rand() % 255,
	      rand() % 255);
  dic_item di;
  if (di = _colorcodes.lookup((char *)type))
    rval = _colorcodes.inf(di);
  else // This keeps the colors from shifting hypnotically
    register_visitor_color((char *)type, rval);

  return rval;
}

const dictionary<char *, QColor> * 
v_visitor_colors_table::color_table(void) const
{
  return &_colorcodes;
}

v_visitor_colors_table::v_visitor_colors_table(void) { }

v_visitor_colors_table::~v_visitor_colors_table() 
{ 
  dic_item di;
  forall_items(di, _colorcodes) {
    delete _colorcodes.key(di);
  }
  _colorcodes.clear();
}


