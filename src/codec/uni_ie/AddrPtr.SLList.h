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

#if !defined(LINT)
static char const _AddrPtr_SLList_h_rcsid_[] =
"$Id: AddrPtr.SLList.h,v 1.3 1997/06/24 18:39:31 bilal Exp $";
#endif

// This may look like C code, but it is really -*- C++ -*-
// WARNING: This file is obsolete.  Use ../SLList.h, if you can.
/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#ifndef _AddrPtrSLList_h
#ifdef __GNUG__
#pragma interface
#endif
#define _AddrPtrSLList_h 1

#include <Pix.h>
#include <codec/uni_ie/AddrPtr.h>
#include <codec/uni_ie/AddrPtr.defs.h>

#ifndef _AddrPtrSLListNode_h
#define _AddrPtrSLListNode_h 1

struct AddrPtrSLListNode
{
  AddrPtrSLListNode*         tl;
  AddrPtr                    hd;
                         AddrPtrSLListNode() { }
                         AddrPtrSLListNode(const AddrPtr  h, AddrPtrSLListNode* t = 0);
                         ~AddrPtrSLListNode() { }
};


inline AddrPtrSLListNode::AddrPtrSLListNode(const AddrPtr  h, AddrPtrSLListNode* t)
:hd(h), tl(t) {}

typedef AddrPtrSLListNode* AddrPtrSLListNodePtr;

#endif


class AddrPtrSLList
{
protected:
  AddrPtrSLListNode*        last;

public:
                        AddrPtrSLList();
                        AddrPtrSLList(const AddrPtrSLList& a);
                        ~AddrPtrSLList();

  AddrPtrSLList&            operator = (const AddrPtrSLList& a);

  int                   empty();
  int                   length();

  void                  clear();

  Pix                   prepend(AddrPtr  item);
  Pix                   append(AddrPtr  item);

  void                  join(AddrPtrSLList&);

  Pix                   prepend(AddrPtrSLListNode*);
  Pix                   append(AddrPtrSLListNode*);

  AddrPtr&                  operator () (Pix p);
  Pix                   first();
  void                  next(Pix& p);
  int                   owns(Pix p);
  Pix                   ins_after(Pix p, AddrPtr  item);
  void                  del_after(Pix p);

  AddrPtr&                  front();
  AddrPtr&                  rear();
  AddrPtr                   remove_front();
  int                   remove_front(AddrPtr& x);
  void                  del_front();

  void                  error(const char* msg);
  int                   OK();
};

inline AddrPtrSLList::~AddrPtrSLList()
{
  clear();
}

inline AddrPtrSLList::AddrPtrSLList()
{
  last = 0;
}

inline int AddrPtrSLList::empty()
{
  return last == 0;
}


inline Pix AddrPtrSLList::first()
{
  return (last == 0)? 0 : Pix(last->tl);
}

inline void AddrPtrSLList::next(Pix& p)
{
  p = (p == 0 || p == last)? 0 : Pix(((AddrPtrSLListNode*)(p))->tl);
}

inline AddrPtr& AddrPtrSLList::operator () (Pix p)
{
  if (p == 0) error("null Pix");
  return ((AddrPtrSLListNode*)(p))->hd;
}

inline AddrPtr& AddrPtrSLList::front()
{
  if (last == 0) error("front: empty list");
  return last->tl->hd;
}

inline AddrPtr& AddrPtrSLList::rear()
{
  if (last == 0) error("rear: empty list");
  return last->hd;
}

#endif
