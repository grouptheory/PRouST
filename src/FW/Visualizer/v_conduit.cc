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
static char const _v_conduit_cc_rcsid_[] =
"$Id: v_conduit.cc,v 1.1 1999/01/20 22:58:13 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <math.h>

#include <fstream.h>
#include <qpixmap.h>
#include <qbitmap.h> 
#include <qimage.h> 
#include <qdstream.h> 
#include <qfile.h> 

#include "v_conduit.h"
#include "v_visitor.h"
#include "v_app.h"
#include "v_data.h"

// ----- Default Pixmaps -----
#include "pixmaps/adapter.xpm"
#include "pixmaps/protocol.xpm"
#include "pixmaps/factory.xpm"
#include "pixmaps/mux.xpm"
#include "pixmaps/cluster.xpm"
#include "pixmaps/switch.xpm"
#include "pixmaps/s_adapter.xpm"
#include "pixmaps/s_protocol.xpm"
#include "pixmaps/s_factory.xpm"
#include "pixmaps/s_mux.xpm"
#include "pixmaps/s_cluster.xpm"
#include "pixmaps/s_switch.xpm"
// ----- Default Pixmaps -----

// Distance for hanging nodes off of other nodes
#define HANG_OFF_DIST 25

//-----------------------------------------------------
bool connect_conduits(v_conduit * neighbor1, 
		      v_conduit * neighbor2,
		      v_conduit::WhichSide side1,
		      v_conduit::WhichSide side2) 
{
  bool rval = false;

  if (neighbor1 && neighbor2) {
    // connections take place at the lowest levels, no matter what
    rval = neighbor1->add_neighbor(neighbor2, side1);
    rval = neighbor2->add_neighbor(neighbor1, side2);
    
    // If either are invis, we must propagate the connect up to the highest visible node
    if (!neighbor1->IsVisible())
      neighbor1->HighestVisible()->assume_childs_links(neighbor1);
    if (!neighbor2->IsVisible())
      neighbor2->HighestVisible()->assume_childs_links(neighbor2);
  }
  return rval;
}
								  
bool disconnect_conduits(v_conduit * neighbor1,
			 v_conduit * neighbor2) 
{
  bool rval = false;
  if (neighbor1 && neighbor2) {
    rval = neighbor1->rem_neighbor(neighbor2);
    rval = neighbor2->rem_neighbor(neighbor1);
  }
  return rval;
}

bool move_visitor_between_conduits(v_conduit * neighbor1,
				   v_conduit * neighbor2,
				   v_visitor * vis) 
{
  bool rval = false;
  if (neighbor1 && neighbor2 && vis &&
      vis->CurrentLocation() == neighbor1) {
    rval = neighbor1->rem_visitor(vis);
    vis->SetLocation(neighbor2);
    rval = neighbor2->add_visitor(vis);
  }
  return rval;
}

bool kill_visitor_at_conduit(v_conduit * obj, v_visitor * vis) 
{
  bool rval = false;
  if (obj && vis && vis->CurrentLocation() == obj) {
    rval = obj->rem_visitor(vis);
    rval = VisitorRegistry()->destroy(vis);
  }
  return rval;
}

bool make_visitor_at_conduit(v_conduit * obj, v_visitor * vis) 
{
  bool rval = false;
  if (obj && vis) {
    vis->SetLocation(obj);
    rval = obj->add_visitor(vis);
  }
  return rval;
}

// arg is a conduit whose parent is to be collapse
bool collapse_containing_cluster(v_conduit * obj) 
{
  bool rval = false;
  v_conduit * parent = obj->GetParent();
  int xave = 0, yave = 0, children = 0;
  
  if (!parent)
    return rval;

  // Iterate over all of the parent's children ...
  list_item li;
  forall_items(li, parent->_child_conduits) {
    v_conduit * child = parent->_child_conduits.inf(li);
    // Make the child invisible
    child->SetVisible(false);
    child->SetChildrenVis(false);

    if (ConduitRegistry()->GetFlags() & v_conduit_registry::MoveAffectsChildren) {
      xave += child->GetX();
      yave += child->GetY();
      children++;
    }
  }
  if ((ConduitRegistry()->GetFlags() & v_conduit_registry::MoveAffectsChildren) &&
      children) {
    parent->SetX(xave/children);
    parent->SetY(yave/children);
  }
  // Make the parent visible
  parent->SetVisible(true);

  forall_items(li, parent->_child_conduits) {
    v_conduit * child = parent->_child_conduits.inf(li);
    parent->assume_childs_links(child);
  }
  return rval;
}

// parent takes all of childs links ...
void v_conduit::assume_childs_links(v_conduit * child)
{
  // this needs to be called recursively ...
  if (!(child->_child_conduits.empty())) {
    list_item li;
    forall_items(li, child->_child_conduits) {
      v_conduit * kid = _child_conduits.inf(li);
      child->assume_childs_links(kid);
    }
  }

  // Iterate over the child's neighbors ...
  list_item li;
  forall_items(li, child->_neighbors) {
    Adjacency * adj = child->_neighbors.inf(li);
    v_conduit * neighbor = adj->GetNeighbor();
    if (! HasChild(neighbor)) {
      // If the neighbor is not a child of the parent, then make a connection
      add_neighbor(neighbor);
      neighbor->add_neighbor(this);
      // NOTE:  You cannot call connect_conduits here as that will cause fatal recursion
    }
  }
}

bool v_conduit::HasChild(v_conduit * child) const
{
  bool rval = false;
  if (! _child_conduits.empty()) {
    list_item li;
    forall_items(li, _child_conduits) {
      v_conduit * tmp = _child_conduits.inf(li);
      if ((tmp == child) ||
	  tmp->HasChild(child)) {
	// True if this is our immediate child, or if a child of our child
	rval = true;
	break;
      }
    }
  }
  return rval;
}

// arg is a cluster to be expanded
bool expand_cluster(v_conduit * parent) 
{
  bool rval = false;
  int xave = 0, yave = 0, children = 0;

  // If this node has no children don't bother.
  if (parent->_child_conduits.empty())
    return rval;

  list_item li;
  forall_items(li, parent->_child_conduits) {
    v_conduit * child = parent->_child_conduits.inf(li);
    // Make the child visible
    child->SetVisible(true);

    // If the child is an expander which was not already collapsed it will be missing links ...
    if (!strcmp(child->GetType(), "Cluster")) {
      // assume your children's links ...
      list_item li2;
      forall_items(li2, child->_child_conduits) {
	v_conduit * kid = child->_child_conduits.inf(li2);
	child->assume_childs_links(kid);
      }
    }

    if (ConduitRegistry()->GetFlags() & v_conduit_registry::MoveAffectsChildren) {
      xave += child->GetX();
      yave += child->GetY();
      children++;
    }
  }

  if (ConduitRegistry()->GetFlags() & v_conduit_registry::MoveAffectsChildren) {
    int  delta_x = parent->GetX() - xave/children;
    int  delta_y = parent->GetY() - yave/children;
    
    forall_items(li, parent->_child_conduits) {
      v_conduit * child = parent->_child_conduits.inf(li);
      child->SetX(child->GetX() + delta_x);
      child->SetY(child->GetY() + delta_y);
    }
  }

  // remove all connections to the parent
  parent->kill_links();
  // Make the parent invisible
  parent->SetVisible(false);

  return rval;
}

int distance(v_conduit * src, v_conduit * dest)
{
  int dx = src->_xpos - dest->_xpos,
      dy = src->_ypos - dest->_ypos;
  return (int)(sqrt( dx*dx + dy*dy ));
}

void v_conduit::kill_links(void)
{
  list_item li;
  forall_items(li, _neighbors) {
    v_conduit * nb = _neighbors.inf(li)->GetNeighbor();
    disconnect_conduits(this, nb);
  }
}

void v_conduit::SetParent(v_conduit * p)
{  
  // This is a bit tricky now ...
  v_conduit * tmp = p;

  while (tmp && strcmp(tmp->GetType(), "Cluster"))
    tmp = tmp->GetParent();
  // At this point tmp is either 0 or a Cluster ...
  _parent_conduit = tmp;
}

v_conduit * v_conduit::GetParent(void) const
{  return _parent_conduit;  }

v_conduit * v_conduit::HighestVisible(void) const
{
  v_conduit * rval = (v_conduit *)this;

  // iterate up the parent hierarchy until we encounter a visible conduit
  while (!(rval->IsVisible())) {
    if (!rval->GetParent())
      break;
    rval = rval->GetParent();
  }

  assert( rval->IsVisible() );

  return rval;
}

bool v_conduit::add_conduit(v_conduit * component)
{
  bool rval = false;

  // If this is a factory, attempt to add to my parent
  if (!strcmp(_typname, "Factory")) {
    if (_parent_conduit)
      rval = _parent_conduit->add_conduit(component);
  } else if (!strcmp(_typname, "Cluster")) {
    if (!_child_conduits.lookup(component)) {
      _child_conduits.insert(component);
      component->SetParent(this);
      if (HasVisibleParent()) {
	component->SetVisible(false);
	component->SetChildrenVis(false);
      }
      rval = true;
    }
  }
  return rval;
}

int   v_conduit::GetRank(v_visitor * v) const
{
  int rval = 0;

  list_item li;
  forall_items(li, _inhabitants) {
    if (v == _inhabitants.inf(li))
      break;
    rval++;
  }
  return rval;
}

bool  v_conduit::HasVisibleParent(void)
{
  bool rval = false;

  v_conduit * nc = this;
  while ( nc ) {
    if (nc->IsVisible()) {
      rval = true;
      break;
    }
    nc = nc->_parent_conduit;
  }

  return rval;
}

bool  v_conduit::rem_conduit(v_conduit * component)
{
  bool rval = false;
  list_item li;
  if (li = _child_conduits.lookup(component)) {
    _child_conduits.del_item(li);
    component->SetParent(0);
    rval = true;
  }
  return rval;
}

bool  v_conduit::add_neighbor(v_conduit * neighbor, v_conduit::WhichSide side)
{
  bool rval = false;
  Adjacency adj(neighbor, side);
  if (!_neighbors.lookup(&adj)) {
    _neighbors.insert(new Adjacency(adj));
    rval = true;
  }
  return rval;
}

bool  v_conduit::rem_neighbor(v_conduit * neighbor)
{
  bool rval = false;
  list_item li;
  Adjacency adj(neighbor);
  if (li = _neighbors.lookup(&adj)) {
    delete _neighbors.inf(li);
    _neighbors.del_item(li);
    rval = true;
  }
  return rval;
}

bool  v_conduit::add_visitor(v_visitor * vis)
{
  bool rval = false;

  if (!_inhabitants.lookup(vis)) {
    _inhabitants.append(vis);
    rval = true;
  }
  return rval;
}

bool  v_conduit::rem_visitor(v_visitor * vis)
{ 
  bool rval = false;
  list_item li;
  if (li = _inhabitants.lookup(vis)) {
    _inhabitants.del_item(li);
    rval = true;
  }
  return rval;
}

const char * v_conduit::GetName(void) const
{  return _objname; }

const char * v_conduit::GetType(void) const
{  return _typname;  }

void v_conduit::SetVisible(bool v)
{  
  _visible = v;  
}

void v_conduit::SetChildrenVis(bool v)
{
  if (_child_conduits.empty())
    return;

  list_item li;
  forall_items(li, _child_conduits) {
    v_conduit * child = _child_conduits.inf(li);
    child->SetVisible(v);
    child->SetChildrenVis(v);
  }
}

bool v_conduit::IsVisible(void) const
{  return _visible;  }

bool v_conduit::Touched(void) const
{  return _touched;  }

int  v_conduit::GetX(void) const
{  return _xpos;  }

int  v_conduit::GetY(void) const
{  return _ypos;  }

void v_conduit::SetX(int x)
{  
#if 0
  if (x > GetApp().GetPixXMax())
    _xpos = GetApp().GetPixXMax();  
  else if (x < GetApp().GetPixXMin())
    _xpos = GetApp().GetPixXMin();  
  else 
#endif
    _xpos = x;
}

void v_conduit::SetY(int y)
{  
#if 0
  if (y > GetApp().GetPixYMax())
    _ypos = GetApp().GetPixYMax();  
  else if (y < GetApp().GetPixYMin())
    _ypos = GetApp().GetPixYMin();  
  else 
#endif
    _ypos = y;  
}

void v_conduit::SetTouched(void)
{ _touched = true; }

QPixmap * v_conduit::GetPixmap(void)
{  
  // Lookup based on this conduits specific name ...
  _image = ConduitPixmapsTable()->lookup_conduit_pixmap((char *)_objname);
  if (!_image) { // then lookup based on conduit's type ...
    _image = ConduitPixmapsTable()->lookup_conduit_pixmap((char *)_typname);
    if (!_image)
      _image = ConduitPixmapsTable()->load_default_pixmap((char *)_typname);
  }
  return _image;  
}

QPixmap * v_conduit::GetSelectedPixmap(void)
{
  return ConduitPixmapsTable()->load_default_pixmap((char *)_typname, true);
}

int v_conduit::VisPresence(v_visitor * v) const
{
  int rval = 0;
  list_item li;
  forall_items(li, _inhabitants) {
    rval++;
    if (_inhabitants.inf(li) == v)
      break;
  }
  return rval;
}

QPoint v_conduit::AvgNeighborPos(void) const
{
  QPoint rval(-1, -1);

  if (! _neighbors.empty()) {
    int x = 0, y = 0;
    bool changed = false;
    list_item li;
    forall_items(li, _neighbors) {
      v_conduit * n = _neighbors.inf(li)->GetNeighbor();
      if (n->GetX() != -1 && n->GetY() != -1) {
	x += n->GetX();
	y += n->GetY();
	changed = true;
      }
    }
    if (changed) {
      if (_neighbors.size() > 1) {
	x /= _neighbors.size();
	y /= _neighbors.size();
      } else {
	x += HANG_OFF_DIST;
	y += HANG_OFF_DIST;
      }
      rval.setX(x);
      rval.setY(y);
    }
  }
  return rval;
}

int v_conduit::orientation(void)
{
  int quadrant(double, double, double, double);

  double x_tot = (double)_xpos, 
         y_tot = (double)_ypos;
  int rval = 0;

  list_item li;
  forall_items(li, _neighbors) {
    Adjacency * adj = _neighbors.inf(li);

    // Factory and Creations effect us
    if (adj->GetSide() == v_conduit::A_SIDE)
      continue;

    v_conduit * neigh = adj->GetNeighbor();
    double x = (double)neigh->GetX(),
           y = (double)neigh->GetY();
    
    double opposite   = y - _ypos;
    double adjacent   = x - _xpos;
    double hypotenuse = sqrt(adjacent * adjacent +
			     opposite * opposite);
    opposite /= hypotenuse;
    adjacent /= hypotenuse;
    x_tot += adjacent;
    y_tot += opposite;
  }

  double adjacent = fabs(x_tot - (double)_xpos),
         opposite = fabs(y_tot - (double)_ypos);

  if (adjacent) {
    rval = (int)((atan(opposite/adjacent)) * 180.0 / 3.14159);

    switch (quadrant(x_tot, y_tot, _xpos, _ypos)) {
      case 1:
	rval = -(90 - rval);
	break;
      case 2:
	rval = 90 - rval;
	break;
      case 3:
	rval = 90 + rval;
	break;
      case 4:
	rval = -(90 + rval);
	break;
    }
  }
  return rval;
}

void v_conduit::MoveToAvgChildPos(void)
{
  int avgX = 0, avgY = 0;
  list_item li;
  forall_items(li, _child_conduits) {
    avgX += _child_conduits.inf(li)->GetX();
    avgY += _child_conduits.inf(li)->GetY();
  }
  avgX /= _child_conduits.length();
  avgY /= _child_conduits.length();

  SetX(avgX);
  SetY(avgY);
}

const list<Adjacency *> * v_conduit::GetNeighbors(void) const
{  return &_neighbors;  }

const list<v_conduit *> * v_conduit::GetChildren(void) const
{  return &_child_conduits;  }

const list<v_visitor *> * v_conduit::GetVisitors(void) const
{  return &_inhabitants;  }

v_conduit::v_conduit(char * objname, char * type) 
  : _xpos(-1), _ypos(-1), _image(0), _visible(true),
    _parent_conduit(0), _touched(false), _secret_x(-1),
    _secret_y(-1)
{
  assert(objname && type);
  strcpy(_objname, objname);
  strcpy(_typname, type);
}

v_conduit::~v_conduit() 
{ 
  list_item li;
  forall_items(li, _neighbors) {
    v_conduit * nb = _neighbors.inf(li)->GetNeighbor();
    nb->rem_neighbor(this);
  }
  forall_items(li, _child_conduits) {
    v_conduit * cc = _child_conduits.inf(li);
    if (_parent_conduit)
      cc->SetParent(_parent_conduit);
    else
      cc->SetParent(0);
  }
  if (_parent_conduit)
    _parent_conduit->rem_conduit(this);

  // Kill all contained visitors
  forall_items(li, _inhabitants) {
    v_visitor * nv = _inhabitants.inf(li);
    kill_visitor_at_conduit(this, nv);
  }
}

//-----------------------------------------------------
v_conduit_registry * v_conduit_registry::_singleton = 0;

v_conduit_registry * ConduitRegistry(void)
{
  if (!v_conduit_registry::_singleton)
    v_conduit_registry::_singleton = new v_conduit_registry();
  return v_conduit_registry::_singleton;
}

const dictionary<char *, v_conduit *> * v_conduit_registry::GetConduits(void) const
{ return &_universe; }

int v_conduit_registry::GetFlags(void) const
{ return _flags; }

void v_conduit_registry::SetFlag(int flag)
{
  _flags |= flag;
}

bool v_conduit_registry::SetFlag(const char * tmp)
{
  bool rval = true;

  if (!strcmp(tmp, "HighestVisible"))
    ConduitRegistry()->SetFlag(v_conduit_registry::HighestVisible);
  else if (!strcmp(tmp, "MoveAffectsChildren"))
    ConduitRegistry()->SetFlag(v_conduit_registry::MoveAffectsChildren);
  else if (!strcmp(tmp, "ViewWhileContinue"))
    ConduitRegistry()->SetFlag(v_conduit_registry::ViewWhileContinue);
  else if (!strcmp(tmp, "ViewAllVisitorColors"))
    ConduitRegistry()->SetFlag(v_conduit_registry::ViewAllVisitorColors);
  else if (!strcmp(tmp, "MoveFreezesConduit"))
    ConduitRegistry()->SetFlag(v_conduit_registry::MoveFreezesConduit);
  else if (!strcmp(tmp, "ImmediateZoom"))
    ConduitRegistry()->SetFlag(v_conduit_registry::ImmediateZoom);
  else
    rval = false;

  return rval;
}

void v_conduit_registry::RemFlag(int flag)
{
  _flags &= ~flag;
}

// returns the minimum distance between two nodes
int  v_conduit_registry::min_dist(void) const
{
  int rval = MAXINT;

  dic_item di;
  forall_items(di, _universe) {
    v_conduit * nc = _universe.inf(di);
    
    list_item li;
    forall_items(li, *(nc->GetNeighbors())) {
      Adjacency * adj = nc->GetNeighbors()->inf(li);
      v_conduit * neigh = adj->GetNeighbor();
      
      if (distance(nc, neigh) < rval)
	rval = distance(nc, neigh);
    }
  }
  return rval;
}

bool v_conduit_registry::has_pos(int x, int y)
{
  bool rval = false;

  dic_item di;
  forall_items(di, _universe) {
    v_conduit * nc = _universe.inf(di);

    if (nc->GetX() == x &&
	nc->GetY() == y) {
      rval = true;
      break;
    }
  }
  return rval;
}

v_conduit * v_conduit_registry::demand(char * objname, char * typ) 
{
  dic_item di;
  if (!(di = _universe.lookup(objname))) {
    v_conduit * nc = new v_conduit(objname, typ);
    char * st = new char [ strlen(objname) + 1 ];
    strcpy(st, objname);
    di = _universe.insert(st, nc);
    theApplication().getData()->notify(nc);
  }
  return _universe.inf(di);
}

v_conduit * v_conduit_registry::query(char * objname) const
{
  v_conduit * rval = 0;
  dic_item di;
  if (di = _universe.lookup(objname))
    rval = _universe.inf(di);
  return rval;
}

bool v_conduit_registry::destroy(char * objname) 
{
  bool rval = false;
  dic_item di;
  if (di = _universe.lookup(objname)) {
    v_conduit * nc = _universe.inf(di);
    theApplication().getData()->remove(nc);
    delete _universe.inf(di);
    delete [] _universe.key(di);
    _universe.del_item(di);
    rval = true;
  }
  return rval;
}

bool v_conduit_registry::namechange(char * old, char * newname)
{
  bool rval = false;
  dic_item di;
  if (di = _universe.lookup(old)) {
    v_conduit * nc = _universe.inf(di);
    char * st = _universe.key(di);
    delete [] st;
    _universe.del_item(di);
    st = new char [ strlen(newname) + 1 ];
    strcpy(st, newname);
    strcpy(nc->_objname, newname);
    _universe.insert(st, nc);

    if (!(nc->Touched())) {
#if 0
      LayoutContainer lc = GetApp().GetMainWin()->find_layout(nc);
      if (lc._x > 0)
	nc->SetX(lc._x);
      if (lc._y > 0)
	nc->SetY(lc._y);
#endif
    }
    rval = true;
  }
  return rval;
}

void v_conduit_registry::SaveLayout(const char * filename)
{
  ofstream outfile(filename);

  if (outfile.bad())
    cerr << "ERROR:  Unable to write layout to '" << filename << "'" << endl;
  else {
    dic_item di;
    outfile << "# Layout file generated by " << theApplication().input() << endl;
    outfile << "# Name Xcoord Ycoord Visible" << endl;
    forall_items(di, _universe) {
      char * name = _universe.key(di);
      v_conduit * c = _universe.inf(di);
      outfile << name << " " << c->GetX() << " " << c->GetY() 
	      << (c->IsVisible() ? " y" : " n" ) << endl;
    }
  }
}

v_conduit_registry::v_conduit_registry(void) : _flags(0) { }

v_conduit_registry::~v_conduit_registry() 
{ clear(); }

void v_conduit_registry::clear(void)
{
  dic_item di;
  forall_items(di, _universe) {
    delete _universe.inf(di);
    delete (char *)_universe.key(di);
  }
  _universe.clear();
}

//-----------------------------------------------------
v_conduit_pixmaps_table * v_conduit_pixmaps_table::_singleton = 0;

v_conduit_pixmaps_table * ConduitPixmapsTable(void) 
{
  if (!v_conduit_pixmaps_table::_singleton)
    v_conduit_pixmaps_table::_singleton = new v_conduit_pixmaps_table();
  return v_conduit_pixmaps_table::_singleton;
}

bool v_conduit_pixmaps_table::register_conduit_pixmap(char * typ, const char * pmap) 
{
  bool rval = false;
  dic_item di;
  if (di = _pixmaps.lookup(typ)) {
    // If it exists, remove it
    delete _pixmaps.inf(di);
    delete _pixmaps.key(di);
    _pixmaps.del_item(di);
  }
  // create the pixmap
  QPixmap * pix = new QPixmap( );
  char pathname[512];
  sprintf(pathname, "%s/%s", theApplication().path(), pmap);
  if (pix->load(pathname)) {
    // allocate a bit mask for the pixmap
    QBitmap mask = pix->createHeuristicMask();
    pix->setMask( mask );
    // allocate the key
    char * st = new char [ strlen(typ) + 1 ];
    strcpy(st, typ);
    _pixmaps.insert(st, pix);
    st = new char [ strlen(typ) + 1 ];
    strcpy(st, typ);
    pix = new QPixmap( );
    pix->load( pathname );
    pix->setMask( mask );
    _orig_pixmaps.insert(st, pix);

    if (pix->convertToImage().depth() != 8)
      cout << "Picture has more than 8 bpp!" << endl;

    rval = true;
  } else {
    cout << "Failed to load '" << pathname << "'" << endl;
    delete pix;
  }
  return rval;
}

void v_conduit_pixmaps_table::resize_pixmaps(double factor, int min_dist)
{
  _factor *= factor;

  dic_item di;
  forall_items(di, _pixmaps) {
    delete _pixmaps.key(di);
    delete _pixmaps.inf(di);
  }
  _pixmaps.clear();

  forall_items(di, _orig_pixmaps) {
    QPixmap * pix = _pixmaps.inf(di);
    if ( min_dist > 0 )
      _factor = min_dist / pix->width();
    QPixmap * new_pix = resize_pixmap(pix, _factor);
    if (new_pix) {
      char * st = new char [ strlen(_orig_pixmaps.key(di)) + 1 ];
      strcpy(st, _orig_pixmaps.key(di));
      // allocate a bit mask for the pixmap
      QBitmap mask = new_pix->createHeuristicMask();
      new_pix->setMask( mask );
      _pixmaps.insert(st, new_pix);
    }
  }
}

QPixmap * v_conduit_pixmaps_table::resize_pixmap(QPixmap * pix, double factor)
{
  assert( factor > 0.0 );
  QPixmap * rval = new QPixmap( );

  QImage img(pix->convertToImage());
  int x, y, width = img.width(), height = img.height();
  int nw = (int)(width * factor), nh = (int)(height * factor);

  uint   ** tmp = new uint * [ nh ];
  for (int i = 0; i < nh; i++)
    tmp[i] = new uint [ nw ];

  u_char ** values = img.jumpTable();

  for (x = 0; x < nw; x++)
    for (y = 0; y < nh; y++)
      tmp[y][x] = ComputeColor(img, x, y, nw, nh);

  QImage new_img(nw, nh, 32, img.numColors());
  uint ** p = (uint **)(new_img.jumpTable());

  for (x = 0; x < nw; x++)
    for (y = 0; y < nh; y++)
      p[y][x] = qRgb( (tmp[y][x] >> 16) & 0xFF, (tmp[y][x] >> 8) & 0xFF, tmp[y][x] & 0xFF );

  if (! rval->convertFromImage(new_img)) {
    delete rval; rval = 0;
  }

  delete [] tmp;
  return rval;
}

uint v_conduit_pixmaps_table::ComputeColor(QImage & img, int x, int y, int width, int height)
{
  int local_x = (x * img.width()) / width,
      local_y = (y * img.height()) / height;

  assert(img.depth() == 8);

  //  cout << "local x,y (" << local_x << "," << local_y << ") max x,y (" 
  //       << img.width() << "," << img.height() << ")" << endl;

  assert( (local_x < img.width()) && (local_y < img.height()) );

  int col_idx = (int)( (img.jumpTable())[local_y][local_x] );
  QRgb rgb = img.color( col_idx );
  uint rval = ((qRed(rgb) << 16) | (qGreen(rgb) << 8) | (qBlue(rgb)));

  return rval;
}

QPixmap * v_conduit_pixmaps_table::lookup_conduit_pixmap(char * typ) 
{
  QPixmap * rval = 0;
  dic_item di;
  if (di = _pixmaps.lookup(typ))
    rval = _pixmaps.inf(di);

  return rval;
}

QPixmap * v_conduit_pixmaps_table::load_default_pixmap(char * typ, bool selected)
{
  if (selected) {
    dic_item di;
    if (di = _sel_pixmaps.lookup( typ ))
      return _sel_pixmaps.inf(di);
  } else // If you're calling this, it had better not be in the dictionary
    assert(!(_pixmaps.lookup(typ)));

  QPixmap * pix = 0;

  if (!strcmp(typ, "Mux"))
    pix = ( selected ? new QPixmap( s_mux ) : new QPixmap( mux ) );
  else if (!strcmp(typ, "Protocol"))
    pix = ( selected ? new QPixmap( s_protocol ) : new QPixmap( protocol ) );
  else if (!strcmp(typ, "Adapter"))
    pix = ( selected ? new QPixmap( s_adapter ) : new QPixmap( adapter ) );
  else if (!strcmp(typ, "Cluster"))
    pix = ( selected ? new QPixmap( s_cluster ) : new QPixmap( cluster ) );
  else if (!strcmp(typ, "Factory"))
    pix = ( selected ? new QPixmap( s_factory ) : new QPixmap( factory ) );
  else if (!strcmp(typ, "Switch"))
    pix = ( selected ? new QPixmap( s_switch ) : new QPixmap( switch_icon ) );
  else
    cerr << "Unknown type '" << typ << "' for load_default_pixmap" << endl;

  if (pix || pix->isNull()) {
    QBitmap mask = pix->createHeuristicMask();
    pix->setMask( mask );
    if (! selected ) {
      char * st = new char [ strlen(typ) + 1 ];
      strcpy(st, typ);
      _pixmaps.insert(st, pix);
      st = new char [ strlen(typ) + 1 ];
      strcpy(st, typ);
      QPixmap * opix = new QPixmap(*pix);
      opix->setMask( mask );
      _orig_pixmaps.insert(st, opix);
    } else {
      char * st = new char [ strlen(typ) + 1 ];
      strcpy(st, typ);
      _sel_pixmaps.insert(st, pix);
    }
  } else if (!pix || pix->isNull()) {
    cerr << "FATAL ERROR: Unable to locate default pixmaps!" << endl;
    exit(1);
  }
  return pix;
}

v_conduit_pixmaps_table::v_conduit_pixmaps_table(void) :
  _factor(1.0) { }

v_conduit_pixmaps_table::~v_conduit_pixmaps_table() 
{ 
  dic_item di;
  forall_items(di, _pixmaps) {
    delete _pixmaps.key(di);
    delete _pixmaps.inf(di);
  }
  _pixmaps.clear();
}

// ------------------------------------------------------------------
Adjacency::Adjacency(v_conduit * neighbor, v_conduit::WhichSide side) 
  : _neighbor(neighbor), _side(side) { }

Adjacency::Adjacency(const Adjacency & rhs)
  : _neighbor(rhs._neighbor), _side(rhs._side) { }

Adjacency::~Adjacency( ) { }

const Adjacency & Adjacency::operator = (const Adjacency & rhs)
{
  _neighbor = rhs._neighbor;
  _side     = rhs._side;
  return (*this);
}

Adjacency::operator v_conduit * () const
{
  return _neighbor;
}

v_conduit * Adjacency::GetNeighbor(void) const
{ 
  return _neighbor;
}

v_conduit::WhichSide Adjacency::GetSide(void) const
{
  return _side;
}

int quadrant(double x, double y, double x_cen, double y_cen) 
{
  //  cout << "(" << x << "," << y << ") (" << x_cen << "," << y_cen <<")." << endl;
  if (x >= x_cen && y >= y_cen) return (1);
  if (x <= x_cen && y >= y_cen) return (2);
  if (x <= x_cen && y <= y_cen) return (3);
  else return (4);
}
