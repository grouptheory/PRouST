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
#ifndef __NV_CONDUIT_H__
#define __NV_CONDUIT_H__

#ifndef LINT
static char const _NV_conduit_h_rcsid_[] =
"$Id: NV_conduit.h,v 1.2 1999/02/24 21:48:20 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
#include <qpoint.h>

class NV_visitor;
class NV_conduit;
class NV_conduit_registry;
class NV_conduit_pixmaps_table;
class NV_fw_state;
class QPixmap;
class GraphField;

typedef QPixmap pixmap;

#define CON_STRSIZE 255

class Adjacency;

//-----------------------------------------------------
class NV_conduit {
  friend class NV_conduit_registry;
  friend class GraphField;
  friend bool  disconnect_conduits(NV_conduit * neighbor1, NV_conduit * neighbor2);
  friend bool  move_visitor_between_conduits(NV_conduit * start, NV_conduit * dest, NV_visitor * vis);
  friend bool  kill_visitor_at_conduit(NV_conduit * obj, NV_visitor * vis);
  friend bool  make_visitor_at_conduit(NV_conduit * obj, NV_visitor * vis);
  friend bool  collapse_containing_cluster(NV_conduit * obj);
  friend bool  expand_cluster(NV_conduit * obj);
  friend int   distance(NV_conduit * src, NV_conduit * dest);
public:

  enum WhichSide {
    A_SIDE = 1,
    B_SIDE = 2,
    OTHER  = 3
  };

  bool  add_conduit(NV_conduit * component);
  bool  rem_conduit(NV_conduit * component);
  bool  add_neighbor(NV_conduit * neighbor, NV_conduit::WhichSide side = NV_conduit::OTHER);
  bool  rem_neighbor(NV_conduit * neighbor);
  bool  add_visitor(NV_visitor * vis);
  bool  rem_visitor(NV_visitor * vis);

  const char * GetName(void) const;
  const char * GetType(void) const;
  void         SetParent(NV_conduit * p);
  NV_conduit * GetParent(void) const;
  void         SetVisible(bool v);
  bool         IsVisible(void) const;
  bool         Touched(void) const;
  NV_conduit * HighestVisible(void) const;
  int          GetX(void) const;
  int          GetY(void) const;
  void         SetX(int x);
  void         SetY(int y);
  pixmap     * GetPixmap(void);
  int          VisPresence(NV_visitor * v) const;
  QPoint       AvgNeighborPos(void) const;
  bool         HasChild(NV_conduit * c) const;
  void         SetChildrenVis(bool v);
  int          orientation(void);
  bool         HasVisibleParent(void);

  const list<Adjacency>    * GetNeighbors(void) const;
  const list<NV_conduit *> * GetChildren(void) const;
  const list<NV_visitor *> * GetVisitors(void) const;

  void SetTouched(void);
  void MoveToAvgChildPos(void);

private:

  NV_conduit(char * objname, char * typ);
  ~NV_conduit();

  void assume_childs_links(NV_conduit * child);
  void kill_links(void);

  int              _xpos;
  int              _ypos;
  double           _secret_x;
  double           _secret_y;

  char             _objname[CON_STRSIZE];
  char             _typname[CON_STRSIZE];
  pixmap         * _image;
  bool             _visible;
  bool             _touched;

  NV_conduit       * _parent_conduit;
  list<Adjacency>    _neighbors;
  list<NV_conduit *> _child_conduits;
  list<NV_visitor *> _inhabitants;

  friend bool  connect_conduits(NV_conduit * neighbor1, NV_conduit * neighbor2,
				NV_conduit::WhichSide s1 = NV_conduit::OTHER, 
				NV_conduit::WhichSide s2 = NV_conduit::OTHER);
};

//-----------------------------------------------------
class NV_conduit_registry {
  friend class NV_fw_state;
  friend class NV_application;
  friend NV_conduit_registry * ConduitRegistry(void);
public:

  enum flagvals {
    None                 = 0,
    MoveAffectsChildren  = 1,     // Moving the expander, moves the constituent conduits
    HighestVisible       = 2,     // Starts out with collapsed view
    ViewWhileContinue    = 4,     // Updates screen while continuing
    ViewAllVisitorColors = 8,     // Show all Visitor colors, not just active ones
    MoveFreezesConduit   = 16,    // Moving freezes the conduit in place
    ImmediateZoom        = 32,    // Zoom is immediate, no smoothing
  };

  // When looking up, you need not specify type.
  NV_conduit * demand(char * objname, char * typ);
  NV_conduit * query(char * objname) const;
  bool destroy(char * objname);
  bool namechange(char * oldname, char * newname);

  void SaveLayout(const char * filename);
  const dictionary<char *, NV_conduit *> * GetConduits(void) const;

  int  GetFlags(void) const;

  bool has_pos(int x, int y);

  int  min_dist(void) const;

private:

  void SetFlag(int flag);
  bool SetFlag(const char * tmp);
  void RemFlag(int flag);

  void clear(void);

  NV_conduit_registry(void);
  ~NV_conduit_registry();

  // Name to Conduit
  dictionary<char *, NV_conduit *> _universe;
  int                              _flags;
  static NV_conduit_registry     * _singleton;
};

//-----------------------------------------------------
class NV_conduit_pixmaps_table {
  friend class NV_conduit;
  friend class NV_fw_state;
  friend NV_conduit_pixmaps_table * ConduitPixmapsTable(void);
public:

  bool register_conduit_pixmap(char * typ, const char * xpm);
  void resize_pixmaps(double factor, int min_dist = -1);

private:

  pixmap * lookup_conduit_pixmap(char * typ);
  pixmap * load_default_pixmap(char * typ);
  pixmap * resize_pixmap(pixmap * pix, double fac);
  uint     ComputeColor(QImage & img, int x, int y, int width, int height);

  NV_conduit_pixmaps_table(void);
  ~NV_conduit_pixmaps_table();

  static const int pixmap_width = 16;
  // Type to pixmap
  dictionary<char *, pixmap *>      _pixmaps;
  dictionary<char *, pixmap *>      _orig_pixmaps;
  double                            _factor;
  static NV_conduit_pixmaps_table * _singleton;
};

class Adjacency {
  friend class NV_conduit;
  friend int compare(const Adjacency & lhs, const Adjacency & rhs);
public:

  Adjacency(NV_conduit * neighbor, NV_conduit::WhichSide side = NV_conduit::OTHER);
  Adjacency(const Adjacency & rhs);
  ~Adjacency( );

  const Adjacency & operator = (const Adjacency & rhs);
  operator NV_conduit * () const;

  NV_conduit * GetNeighbor(void) const;
  NV_conduit::WhichSide GetSide(void) const;

private:

  NV_conduit          * _neighbor;
  NV_conduit::WhichSide _side;
};

#endif
