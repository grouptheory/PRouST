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
#ifndef __V_CONDUIT_H__
#define __V_CONDUIT_H__

#ifndef LINT
static char const _v_conduit_h_rcsid_[] =
"$Id: v_conduit.h,v 1.3 1999/03/01 19:27:07 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
#include <qpoint.h>

class v_visitor;
class v_conduit;
class v_parser;
class v_conduit_registry;
class v_conduit_pixmaps_table;
class QPixmap;

#define CON_STRSIZE 255

class Adjacency;

//-----------------------------------------------------
class v_conduit {
  friend class v_conduit_registry;
  friend class v_data;
  friend bool  disconnect_conduits(v_conduit * neighbor1, v_conduit * neighbor2);
  friend bool  move_visitor_between_conduits(v_conduit * start, v_conduit * dest, v_visitor * vis);
  friend bool  kill_visitor_at_conduit(v_conduit * obj, v_visitor * vis);
  friend bool  make_visitor_at_conduit(v_conduit * obj, v_visitor * vis);
  friend bool  collapse_containing_cluster(v_conduit * obj);
  friend bool  expand_cluster(v_conduit * obj);
  friend int   distance(v_conduit * src, v_conduit * dest);
public:

  enum WhichSide {
    A_SIDE = 1,
    B_SIDE = 2,
    OTHER  = 3
  };

  bool  add_conduit(v_conduit * component);
  bool  rem_conduit(v_conduit * component);
  bool  add_neighbor(v_conduit * neighbor, v_conduit::WhichSide side = v_conduit::OTHER);
  bool  rem_neighbor(v_conduit * neighbor);
  bool  add_visitor(v_visitor * vis);
  bool  rem_visitor(v_visitor * vis);

  const char * GetName(void) const;
  const char * GetType(void) const;
  void         SetParent(v_conduit * p);
  v_conduit  * GetParent(void) const;
  void         SetVisible(bool v);
  bool         IsVisible(void) const;
  bool         Touched(void) const;
  v_conduit  * HighestVisible(void) const;
  int          GetX(void) const;
  int          GetY(void) const;
  void         SetX(int x);
  void         SetY(int y);
  QPixmap    * GetPixmap(void);
  QPixmap    * GetSelectedPixmap(void);
  int          VisPresence(v_visitor * v) const;
  QPoint       AvgNeighborPos(void) const;
  bool         HasChild(v_conduit * c) const;
  void         SetChildrenVis(bool v);
  int          orientation(void);
  bool         HasVisibleParent(void);
  int          GetRank(v_visitor * v) const;

  const list<Adjacency *> * GetNeighbors(void) const;
  const list<v_conduit *> * GetChildren(void) const;
  const list<v_visitor *> * GetVisitors(void) const;

  void SetTouched(void);
  void MoveToAvgChildPos(void);

private:

  v_conduit(char * objname, char * typ);
  ~v_conduit();

  void assume_childs_links(v_conduit * child);
  void kill_links(void);

  int              _xpos;
  int              _ypos;
  double           _secret_x;
  double           _secret_y;

  char             _objname[CON_STRSIZE];
  char             _typname[CON_STRSIZE];
  QPixmap        * _image;
  bool             _visible;
  bool             _touched;

  v_conduit       * _parent_conduit;
  list<Adjacency *> _neighbors;
  list<v_conduit *> _child_conduits;
  list<v_visitor *> _inhabitants;

  friend bool  connect_conduits(v_conduit * neighbor1, v_conduit * neighbor2,
				v_conduit::WhichSide s1 = v_conduit::OTHER, 
				v_conduit::WhichSide s2 = v_conduit::OTHER);
};

//-----------------------------------------------------
class v_conduit_registry {
  friend class v_parser;
  friend class v_app;
  friend v_conduit_registry * ConduitRegistry(void);
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
  v_conduit * demand(char * objname, char * typ);
  v_conduit * query(char * objname) const;
  bool destroy(char * objname);
  bool namechange(char * oldname, char * newname);

  void SaveLayout(const char * filename);
  const dictionary<char *, v_conduit *> * GetConduits(void) const;

  int  GetFlags(void) const;

  bool has_pos(int x, int y);

  int  min_dist(void) const;

private:

  void SetFlag(int flag);
  bool SetFlag(const char * tmp);
  void RemFlag(int flag);

  void clear(void);

  v_conduit_registry(void);
  ~v_conduit_registry();

  // Name to Conduit
  dictionary<char *, v_conduit *> _universe;
  int                              _flags;
  static v_conduit_registry     * _singleton;
};

//-----------------------------------------------------
class v_conduit_pixmaps_table {
  friend class v_conduit;
  friend v_conduit_pixmaps_table * ConduitPixmapsTable(void);
public:

  bool register_conduit_pixmap(char * typ, const char * xpm);
  void resize_pixmaps(double factor, int min_dist = -1);

private:

  QPixmap * lookup_conduit_pixmap(char * typ);
  QPixmap * load_default_pixmap(char * typ, bool selected = false);
  QPixmap * resize_pixmap(QPixmap * pix, double fac);
  uint      ComputeColor(QImage & img, int x, int y, int width, int height);

  v_conduit_pixmaps_table(void);
  ~v_conduit_pixmaps_table();

  static const int pixmap_width = 16;
  // Type to pixmap
  dictionary<char *, QPixmap *>      _pixmaps;
  dictionary<char *, QPixmap *>      _orig_pixmaps;
  dictionary<char *, QPixmap *>      _sel_pixmaps;
  double                             _factor;
  static v_conduit_pixmaps_table   * _singleton;
};

class Adjacency {
  friend class v_conduit;
  friend int compare(Adjacency * const & lhs, Adjacency * const & rhs);
public:

  Adjacency(v_conduit * neighbor, v_conduit::WhichSide side = v_conduit::OTHER);
  Adjacency(const Adjacency & rhs);
  ~Adjacency( );

  const Adjacency & operator = (const Adjacency & rhs);
  operator v_conduit * () const;

  v_conduit * GetNeighbor(void) const;
  v_conduit::WhichSide GetSide(void) const;

private:

  v_conduit          * _neighbor;
  v_conduit::WhichSide _side;
};

#endif
