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
static char const _NV_conduit_cc_rcsid_[] =
"$Id: NV_conduit.cc,v 1.1 1999/01/21 14:55:00 bilal Exp $";
#endif

#include <common/cprototypes.h>
#include <math.h>

#include <fstream.h>
#include <qpixmap.h>
#include <qbitmap.h> 
#include <qimage.h> 
#include <qdstream.h> 
#include <qfile.h> 
#include "NV_conduit.h"
#include "NV_application.h"
#include "NV_visitor.h"
#include "graphfield.h"

// Distance for hanging nodes off of other nodes
#define HANG_OFF_DIST 25

//-----------------------------------------------------
bool connect_conduits(NV_conduit * neighbor1, 
		      NV_conduit * neighbor2,
		      NV_conduit::WhichSide side1,
		      NV_conduit::WhichSide side2) 
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
  if (rval)
    GetApp().GetMainWin()->SetDirty();
  return rval;
}
								  
bool disconnect_conduits(NV_conduit * neighbor1,
			 NV_conduit * neighbor2) 
{
  bool rval = false;
  if (neighbor1 && neighbor2) {
    rval = neighbor1->rem_neighbor(neighbor2);
    rval = neighbor2->rem_neighbor(neighbor1);
  }
  if (rval)
    GetApp().GetMainWin()->SetDirty();
  return rval;
}

bool move_visitor_between_conduits(NV_conduit * neighbor1,
				   NV_conduit * neighbor2,
				   NV_visitor * vis) 
{
  bool rval = false;
  if (neighbor1 && neighbor2 && vis &&
      vis->CurrentLocation() == neighbor1) {
    rval = neighbor1->rem_visitor(vis);
    vis->SetLocation(neighbor2);
    rval = neighbor2->add_visitor(vis);
    GetApp().GetMainWin()->set_active_visitor(vis);
  }
  if (rval && vis->CurrentLocation()->IsVisible())
    GetApp().GetMainWin()->SetDirty();
  return rval;
}

bool kill_visitor_at_conduit(NV_conduit * obj, NV_visitor * vis) 
{
  bool rval = false;
  if (obj && vis && vis->CurrentLocation() == obj) {
    GetApp().GetMainWin()->set_active_visitor(vis);
    rval = obj->rem_visitor(vis);
    rval = VisitorRegistry()->destroy(vis);
  }
  if (rval)
    GetApp().GetMainWin()->SetDirty();
  return rval;
}

bool make_visitor_at_conduit(NV_conduit * obj, NV_visitor * vis) 
{
  bool rval = false;
  if (obj && vis) {
    vis->SetLocation(obj);
    GetApp().GetMainWin()->set_active_visitor(vis);
    rval = obj->add_visitor(vis);
  }

  if (rval)
    GetApp().GetMainWin()->SetDirty();
  return rval;
}

// arg is a conduit whose parent is to be collapse
bool collapse_containing_cluster(NV_conduit * obj) 
{
  bool rval = false;
  NV_conduit * parent = obj->GetParent();
  int xave = 0, yave = 0, children = 0;
  
  if (!parent)
    return rval;

  // Iterate over all of the parent's children ...
  list_item li;
  forall_items(li, parent->_child_conduits) {
    NV_conduit * child = parent->_child_conduits.inf(li);
    // Make the child invisible
    child->SetVisible(false);
    child->SetChildrenVis(false);

    if (ConduitRegistry()->GetFlags() & NV_conduit_registry::MoveAffectsChildren) {
      xave += child->GetX();
      yave += child->GetY();
      children++;
    }
  }
  if ((ConduitRegistry()->GetFlags() & NV_conduit_registry::MoveAffectsChildren) &&
      children) {
    parent->SetX(xave/children);
    parent->SetY(yave/children);
  }
  // Make the parent visible
  parent->SetVisible(true);

  forall_items(li, parent->_child_conduits) {
    NV_conduit * child = parent->_child_conduits.inf(li);
    parent->assume_childs_links(child);
  }

  if (rval)
    GetApp().GetMainWin()->SetDirty();
  return rval;
}

// parent takes all of childs links ...
void NV_conduit::assume_childs_links(NV_conduit * child)
{
  // this needs to be called recursively ...
  if (!(child->_child_conduits.empty())) {
    list_item li;
    forall_items(li, child->_child_conduits) {
      NV_conduit * kid = _child_conduits.inf(li);
      child->assume_childs_links(kid);
    }
  }

  // Iterate over the child's neighbors ...
  list_item li;
  forall_items(li, child->_neighbors) {
    Adjacency adj = child->_neighbors.inf(li);
    NV_conduit * neighbor = adj.GetNeighbor();
    if (! HasChild(neighbor)) {
      // If the neighbor is not a child of the parent, then make a connection
      add_neighbor(neighbor);
      neighbor->add_neighbor(this);
      // NOTE:  You cannot call connect_conduits here as that will cause fatal recursion
    }
  }
}

bool NV_conduit::HasChild(NV_conduit * child) const
{
  bool rval = false;
  if (! _child_conduits.empty()) {
    list_item li;
    forall_items(li, _child_conduits) {
      NV_conduit * tmp = _child_conduits.inf(li);
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
bool expand_cluster(NV_conduit * parent) 
{
  bool rval = false;
  int xave = 0, yave = 0, children = 0;

  // If this node has no children don't bother.
  if (parent->_child_conduits.empty())
    return rval;

  list_item li;
  forall_items(li, parent->_child_conduits) {
    NV_conduit * child = parent->_child_conduits.inf(li);
    // Make the child visible
    child->SetVisible(true);

    // If the child is an expander which was not already collapsed it will be missing links ...
    if (!strcmp(child->GetType(), "Cluster")) {
      // assume your children's links ...
      list_item li2;
      forall_items(li2, child->_child_conduits) {
	NV_conduit * kid = child->_child_conduits.inf(li2);
	child->assume_childs_links(kid);
      }
    }

    if (ConduitRegistry()->GetFlags() & NV_conduit_registry::MoveAffectsChildren) {
      xave += child->GetX();
      yave += child->GetY();
      children++;
    }
  }

  if (ConduitRegistry()->GetFlags() & NV_conduit_registry::MoveAffectsChildren) {
    int  delta_x = parent->GetX() - xave/children;
    int  delta_y = parent->GetY() - yave/children;
    
    forall_items(li, parent->_child_conduits) {
      NV_conduit * child = parent->_child_conduits.inf(li);
      child->SetX(child->GetX() + delta_x);
      child->SetY(child->GetY() + delta_y);
    }
  }

  // remove all connections to the parent
  parent->kill_links();
  // Make the parent invisible
  parent->SetVisible(false);

  if (rval)
    GetApp().GetMainWin()->SetDirty();
  return rval;
}

int distance(NV_conduit * src, NV_conduit * dest)
{
  int dx = src->_xpos - dest->_xpos,
      dy = src->_ypos - dest->_ypos;
  return (int)(sqrt( dx*dx + dy*dy ));
}

void NV_conduit::kill_links(void)
{
  list_item li;
  forall_items(li, _neighbors) {
    NV_conduit * nb = _neighbors.inf(li);
    disconnect_conduits(this, nb);
  }
}

void NV_conduit::SetParent(NV_conduit * p)
{  
  // This is a bit tricky now ...
  NV_conduit * tmp = p;

  while (tmp && strcmp(tmp->GetType(), "Cluster"))
    tmp = tmp->GetParent();
  // At this point tmp is either 0 or a Cluster ...
  _parent_conduit = tmp;
}

NV_conduit * NV_conduit::GetParent(void) const
{  return _parent_conduit;  }

NV_conduit * NV_conduit::HighestVisible(void) const
{
  NV_conduit * rval = (NV_conduit *)this;

  // iterate up the parent hierarchy until we encounter a visible conduit
  while (!(rval->IsVisible())) {
    if (!rval->GetParent())
      break;
    rval = rval->GetParent();
  }

  assert( rval->IsVisible() );

  return rval;
}

bool NV_conduit::add_conduit(NV_conduit * component)
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

bool  NV_conduit::HasVisibleParent(void)
{
  bool rval = false;

  NV_conduit * nc = this;
  while ( nc ) {
    if (nc->IsVisible()) {
      rval = true;
      break;
    }
    nc = nc->_parent_conduit;
  }

  return rval;
}

bool  NV_conduit::rem_conduit(NV_conduit * component)
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

bool  NV_conduit::add_neighbor(NV_conduit * neighbor, NV_conduit::WhichSide side)
{
  bool rval = false;
  Adjacency adj(neighbor, side);
  if (!_neighbors.lookup(adj)) {
    _neighbors.insert(adj);
    rval = true;
  }
  return rval;
}

bool  NV_conduit::rem_neighbor(NV_conduit * neighbor)
{
  bool rval = false;
  list_item li;
  Adjacency adj(neighbor);
  if (li = _neighbors.lookup(adj)) {
    _neighbors.del_item(li);
    rval = true;
  }
  return rval;
}

bool  NV_conduit::add_visitor(NV_visitor * vis)
{
  bool rval = false;

  if (!_inhabitants.lookup(vis)) {
    _inhabitants.append(vis);
    rval = true;
  }
  return rval;
}

bool  NV_conduit::rem_visitor(NV_visitor * vis)
{ 
  bool rval = false;
  list_item li;
  if (li = _inhabitants.lookup(vis)) {
    _inhabitants.del_item(li);
    rval = true;
  }
  return rval;
}

const char * NV_conduit::GetName(void) const
{  return _objname; }

const char * NV_conduit::GetType(void) const
{  return _typname;  }

void NV_conduit::SetVisible(bool v)
{  
  _visible = v;  
}

void NV_conduit::SetChildrenVis(bool v)
{
  if (_child_conduits.empty())
    return;

  list_item li;
  forall_items(li, _child_conduits) {
    NV_conduit * child = _child_conduits.inf(li);
    child->SetVisible(v);
    child->SetChildrenVis(v);
  }
}

bool NV_conduit::IsVisible(void) const
{  return _visible;  }

bool NV_conduit::Touched(void) const
{  return _touched;  }

int  NV_conduit::GetX(void) const
{  return _xpos;  }

int  NV_conduit::GetY(void) const
{  return _ypos;  }

void NV_conduit::SetX(int x)
{  
  if (x > GetApp().GetPixXMax())
    _xpos = GetApp().GetPixXMax();  
  else if (x < GetApp().GetPixXMin())
    _xpos = GetApp().GetPixXMin();  
  else 
    _xpos = x;
}

void NV_conduit::SetY(int y)
{  
  if (y > GetApp().GetPixYMax())
    _ypos = GetApp().GetPixYMax();  
  else if (y < GetApp().GetPixYMin())
    _ypos = GetApp().GetPixYMin();  
  else 
    _ypos = y;  
}

void NV_conduit::SetTouched(void)
{ _touched = true; }

pixmap * NV_conduit::GetPixmap(void)
{  
  // Make it always consult the registry
  //  if (!_image) {
    // Lookup based on this conduits specific name ...
    _image = ConduitPixmapsTable()->lookup_conduit_pixmap((char *)_objname);
    if (!_image) { // then lookup based on conduit's type ...
      _image = ConduitPixmapsTable()->lookup_conduit_pixmap((char *)_typname);
      if (!_image)
	_image = ConduitPixmapsTable()->load_default_pixmap((char *)_typname);
    }
    //  }
  return _image;  
}

int NV_conduit::VisPresence(NV_visitor * v) const
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

QPoint NV_conduit::AvgNeighborPos(void) const
{
  QPoint rval(-1, -1);

  if (! _neighbors.empty()) {
    int x = 0, y = 0;
    bool changed = false;
    list_item li;
    forall_items(li, _neighbors) {
      NV_conduit * n = _neighbors.inf(li);
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

int NV_conduit::orientation(void)
{
  double x_tot = (double)_xpos, 
         y_tot = (double)_ypos;
  int rval = 0;

  list_item li;
  forall_items(li, _neighbors) {
    Adjacency adj = _neighbors.inf(li);

    // Factory and Creations effect us
    if (adj.GetSide() == NV_conduit::A_SIDE)
      continue;

    NV_conduit * neigh = adj.GetNeighbor();
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

void NV_conduit::MoveToAvgChildPos(void)
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

const list<Adjacency> * NV_conduit::GetNeighbors(void) const
{  return &_neighbors;  }

const list<NV_conduit *> * NV_conduit::GetChildren(void) const
{  return &_child_conduits;  }

const list<NV_visitor *> * NV_conduit::GetVisitors(void) const
{  return &_inhabitants;  }

NV_conduit::NV_conduit(char * objname, char * type) 
  : _xpos(-1), _ypos(-1), _image(0), _visible(true),
    _parent_conduit(0), _touched(false), _secret_x(-1),
    _secret_y(-1)
{
  assert(objname && type);
  strcpy(_objname, objname);
  strcpy(_typname, type);
}

NV_conduit::~NV_conduit() 
{ 
  list_item li;
  forall_items(li, _neighbors) {
    NV_conduit * nb = _neighbors.inf(li);
    nb->rem_neighbor(this);
  }
  forall_items(li, _child_conduits) {
    NV_conduit * cc = _child_conduits.inf(li);
    if (_parent_conduit)
      cc->SetParent(_parent_conduit);
    else
      cc->SetParent(0);
  }
  if (_parent_conduit)
    _parent_conduit->rem_conduit(this);

  // Kill all contained visitors
  forall_items(li, _inhabitants) {
    NV_visitor * nv = _inhabitants.inf(li);
    kill_visitor_at_conduit(this, nv);
  }
}

//-----------------------------------------------------
NV_conduit_registry * NV_conduit_registry::_singleton = 0;

NV_conduit_registry * ConduitRegistry(void)
{
  if (!NV_conduit_registry::_singleton)
    NV_conduit_registry::_singleton = new NV_conduit_registry();
  return NV_conduit_registry::_singleton;
}

const dictionary<char *, NV_conduit *> * NV_conduit_registry::GetConduits(void) const
{ return &_universe; }

int NV_conduit_registry::GetFlags(void) const
{ return _flags; }

void NV_conduit_registry::SetFlag(int flag)
{
  _flags |= flag;
}

bool NV_conduit_registry::SetFlag(const char * tmp)
{
  bool rval = true;

  if (!strcmp(tmp, "HighestVisible"))
    ConduitRegistry()->SetFlag(NV_conduit_registry::HighestVisible);
  else if (!strcmp(tmp, "MoveAffectsChildren"))
    ConduitRegistry()->SetFlag(NV_conduit_registry::MoveAffectsChildren);
  else if (!strcmp(tmp, "ViewWhileContinue"))
    ConduitRegistry()->SetFlag(NV_conduit_registry::ViewWhileContinue);
  else if (!strcmp(tmp, "ViewAllVisitorColors"))
    ConduitRegistry()->SetFlag(NV_conduit_registry::ViewAllVisitorColors);
  else if (!strcmp(tmp, "MoveFreezesConduit"))
    ConduitRegistry()->SetFlag(NV_conduit_registry::MoveFreezesConduit);
  else if (!strcmp(tmp, "ImmediateZoom"))
    ConduitRegistry()->SetFlag(NV_conduit_registry::ImmediateZoom);
  else
    rval = false;

  return rval;
}

void NV_conduit_registry::RemFlag(int flag)
{
  _flags &= ~flag;
}

// returns the minimum distance between two nodes
int  NV_conduit_registry::min_dist(void) const
{
  int rval = MAXINT;

  dic_item di;
  forall_items(di, _universe) {
    NV_conduit * nc = _universe.inf(di);
    
    list_item li;
    forall_items(li, *(nc->GetNeighbors())) {
      Adjacency adj = nc->GetNeighbors()->inf(li);
      NV_conduit * neigh = adj.GetNeighbor();
      
      if (distance(nc, neigh) < rval)
	rval = distance(nc, neigh);
    }
  }
  return rval;
}

bool NV_conduit_registry::has_pos(int x, int y)
{
  bool rval = false;

  dic_item di;
  forall_items(di, _universe) {
    NV_conduit * nc = _universe.inf(di);

    if (nc->GetX() == x &&
	nc->GetY() == y) {
      rval = true;
      break;
    }
  }
  return rval;
}

NV_conduit * NV_conduit_registry::demand(char * objname, char * typ) 
{
  dic_item di;
  if (!(di = _universe.lookup(objname))) {
    NV_conduit * nc = new NV_conduit(objname, typ);
    char * st = new char [ strlen(objname) + 1 ];
    strcpy(st, objname);
    di = _universe.insert(st, nc);
  }
  return _universe.inf(di);
}

NV_conduit * NV_conduit_registry::query(char * objname) const
{
  NV_conduit * rval = 0;
  dic_item di;
  if (di = _universe.lookup(objname))
    rval = _universe.inf(di);
  return rval;
}

bool NV_conduit_registry::destroy(char * objname) 
{
  bool rval = false;
  dic_item di;
  if (di = _universe.lookup(objname)) {
    delete _universe.inf(di);
    delete [] _universe.key(di);
    _universe.del_item(di);
    rval = true;
  }
  return rval;
}

bool NV_conduit_registry::namechange(char * old, char * newname)
{
  bool rval = false;
  dic_item di;
  if (di = _universe.lookup(old)) {
    NV_conduit * nc = _universe.inf(di);
    char * st = _universe.key(di);
    delete [] st;
    _universe.del_item(di);
    st = new char [ strlen(newname) + 1 ];
    strcpy(st, newname);
    strcpy(nc->_objname, newname);
    _universe.insert(st, nc);

    if (!(nc->Touched())) {
      LayoutContainer lc = GetApp().GetMainWin()->find_layout(nc);
      if (lc._x > 0)
	nc->SetX(lc._x);
      if (lc._y > 0)
	nc->SetY(lc._y);
    }
    rval = true;
  }
  return rval;
}

void NV_conduit_registry::SaveLayout(const char * filename)
{
  ofstream outfile(filename);

  if (outfile.bad())
    cerr << "ERROR:  Unable to write layout to '" << filename << "'" << endl;
  else {
    dic_item di;
    outfile << "# Layout file generated by " << GetApp().GetInputName() << endl;
    outfile << "# Name Xcoord Ycoord Visible" << endl;
    forall_items(di, _universe) {
      char * name = _universe.key(di);
      NV_conduit * c = _universe.inf(di);
      outfile << name << " " << c->GetX() << " " << c->GetY() 
	      << (c->IsVisible() ? " y" : " n" ) << endl;
    }
  }
}

NV_conduit_registry::NV_conduit_registry(void) : _flags(0) { }

NV_conduit_registry::~NV_conduit_registry() 
{ clear(); }

void NV_conduit_registry::clear(void)
{
  dic_item di;
  forall_items(di, _universe) {
    delete _universe.inf(di);
    delete (char *)_universe.key(di);
  }
  _universe.clear();
}

//-----------------------------------------------------
NV_conduit_pixmaps_table * NV_conduit_pixmaps_table::_singleton = 0;

NV_conduit_pixmaps_table * ConduitPixmapsTable(void) 
{
  if (!NV_conduit_pixmaps_table::_singleton)
    NV_conduit_pixmaps_table::_singleton = new NV_conduit_pixmaps_table();
  return NV_conduit_pixmaps_table::_singleton;
}

bool NV_conduit_pixmaps_table::register_conduit_pixmap(char * typ, const char * pmap) 
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
  pixmap * pix = new QPixmap( );
  char pathname[512];
  sprintf(pathname, "%s/%s", GetApp().GetPath(), pmap);
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

void NV_conduit_pixmaps_table::resize_pixmaps(double factor, int min_dist)
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

QPixmap * NV_conduit_pixmaps_table::resize_pixmap(QPixmap * pix, double factor)
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

uint NV_conduit_pixmaps_table::ComputeColor(QImage & img, int x, int y, int width, int height)
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

pixmap * NV_conduit_pixmaps_table::lookup_conduit_pixmap(char * typ) 
{
  pixmap * rval = 0;
  dic_item di;
  if (di = _pixmaps.lookup(typ))
    rval = _pixmaps.inf(di);

  return rval;
}

pixmap * NV_conduit_pixmaps_table::load_default_pixmap(char * typ)
{
  // If you're calling this, it had better not be in the dictionary
  assert(!(_pixmaps.lookup(typ)));

  extern const char * adapter[];
  extern const char * cluster[];
  extern const char * factory[];
  extern const char * mux[];
  extern const char * protocol[];
  extern const char * switchxpm[];

  QImage * img = 0;

  if (!strcmp(typ, "Mux"))
    img = new QImage( mux );
  else if (!strcmp(typ, "Protocol"))
    img = new QImage( protocol );
  else if (!strcmp(typ, "Adapter"))
    img = new QImage( adapter );
  else if (!strcmp(typ, "Cluster"))
    img = new QImage( cluster );
  else if (!strcmp(typ, "Factory"))
    img = new QImage( factory );
  else if (!strcmp(typ, "Switch"))
    img = new QImage( switchxpm );
  else
    cerr << "Unknown type '" << typ << "' for load_default_pixmap" << endl;

  QPixmap * pix = 0, * opix = 0;
  if (img && img->isNull() == false) {
    pix = new QPixmap( );
    if (!pix->convertFromImage(*img)) {
      delete pix; pix = 0;
    } else {
      opix = new QPixmap( );
      opix->convertFromImage(*img);
    }
  }

  if (pix || pix->isNull()) {
    QBitmap mask = pix->createHeuristicMask();
    pix->setMask( mask );
    // allocate the key
    char * st = new char [ strlen(typ) + 1 ];
    strcpy(st, typ);
    _pixmaps.insert(st, pix);
    st = new char [ strlen(typ) + 1 ];
    strcpy(st, typ);
    opix->setMask( mask );
    _orig_pixmaps.insert(st, opix);
  } else if (!pix || pix->isNull()) {
    cerr << "FATAL ERROR: Unable to locate default pixmaps!" << endl;
    exit(1);
  }
  return pix;
}

NV_conduit_pixmaps_table::NV_conduit_pixmaps_table(void) :
  _factor(1.0) { }

NV_conduit_pixmaps_table::~NV_conduit_pixmaps_table() 
{ 
  dic_item di;
  forall_items(di, _pixmaps) {
    delete _pixmaps.key(di);
    delete _pixmaps.inf(di);
  }
  _pixmaps.clear();
}

// ------------------------------------------------------------------
Adjacency::Adjacency(NV_conduit * neighbor, NV_conduit::WhichSide side) 
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

Adjacency::operator NV_conduit * () const
{
  return _neighbor;
}

NV_conduit * Adjacency::GetNeighbor(void) const
{ 
  return _neighbor;
}

NV_conduit::WhichSide Adjacency::GetSide(void) const
{
  return _side;
}
