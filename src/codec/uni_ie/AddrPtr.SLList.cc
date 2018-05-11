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
static char const _AddrPtr_SLList_cc_rcsid_[] =
"$Id: AddrPtr.SLList.cc,v 1.4 1998/08/06 23:13:28 marsh Exp $";
#endif
#include <common/cprototypes.h>

// This may look like C code, but it is really -*- C++ -*-
// WARNING: This file is obsolete.  Use ../SLList.cc, if you can.
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

#ifdef __GNUG__
#pragma implementation
#endif
#include <limits.h>
#include <stream.h>
#include <builtin.h>
#include <codec/uni_ie/AddrPtr.SLList.h>

void AddrPtrSLList::error(const char* msg)
{
  (*lib_error_handler)("SLList", msg);
}

int AddrPtrSLList::length()
{
  int l = 0;
  AddrPtrSLListNode* t = last;
  if (t != 0) do { ++l; t = t->tl; } while (t != last);
  return l;
}

AddrPtrSLList::AddrPtrSLList(const AddrPtrSLList& a)
{
  if (a.last == 0)
    last = 0;
  else
  {
    AddrPtrSLListNode* p = a.last->tl;
    AddrPtrSLListNode* h = new AddrPtrSLListNode(p->hd);
    last = h;
    for (;;)
    {
      if (p == a.last)
      {
        last->tl = h;
        return;
      }
      p = p->tl;
      AddrPtrSLListNode* n = new AddrPtrSLListNode(p->hd);
      last->tl = n;
      last = n;
    }
  }
}

AddrPtrSLList& AddrPtrSLList::operator = (const AddrPtrSLList& a)
{
  if (last != a.last)
  {
    clear();
    if (a.last != 0)
    {
      AddrPtrSLListNode* p = a.last->tl;
      AddrPtrSLListNode* h = new AddrPtrSLListNode(p->hd);
      last = h;
      for (;;)
      {
        if (p == a.last)
        {
          last->tl = h;
          break;
        }
        p = p->tl;
        AddrPtrSLListNode* n = new AddrPtrSLListNode(p->hd);
        last->tl = n;
        last = n;
      }
    }
  }
  return *this;
}

void AddrPtrSLList::clear()
{
  if (last == 0)
    return;

  AddrPtrSLListNode* p = last->tl;
  last->tl = 0;
  last = 0;

  while (p != 0)
  {
    AddrPtrSLListNode* nxt = p->tl;
    delete(p);
    p = nxt;
  }
}


Pix AddrPtrSLList::prepend(AddrPtr  item)
{
  AddrPtrSLListNode* t = new AddrPtrSLListNode(item);
  if (last == 0)
    t->tl = last = t;
  else
  {
    t->tl = last->tl;
    last->tl = t;
  }
  return Pix(t);
}


Pix AddrPtrSLList::prepend(AddrPtrSLListNode* t)
{
  if (t == 0) return 0;
  if (last == 0)
    t->tl = last = t;
  else
  {
    t->tl = last->tl;
    last->tl = t;
  }
  return Pix(t);
}


Pix AddrPtrSLList::append(AddrPtr  item)
{
  AddrPtrSLListNode* t = new AddrPtrSLListNode(item);
  if (last == 0)
    t->tl = last = t;
  else
  {
    t->tl = last->tl;
    last->tl = t;
    last = t;
  }
  return Pix(t);
}

Pix AddrPtrSLList::append(AddrPtrSLListNode* t)
{
  if (t == 0) return 0;
  if (last == 0)
    t->tl = last = t;
  else
  {
    t->tl = last->tl;
    last->tl = t;
    last = t;
  }
  return Pix(t);
}

void AddrPtrSLList::join(AddrPtrSLList& b)
{
  AddrPtrSLListNode* t = b.last;
  b.last = 0;
  if (last == 0)
    last = t;
  else if (t != 0)
  {
    AddrPtrSLListNode* f = last->tl;
    last->tl = t->tl;
    t->tl = f;
    last = t;
  }
}

Pix AddrPtrSLList::ins_after(Pix p, AddrPtr  item)
{
  AddrPtrSLListNode* u = (AddrPtrSLListNode*)p;
  AddrPtrSLListNode* t = new AddrPtrSLListNode(item);
  if (last == 0)
    t->tl = last = t;
  else if (u == 0) // ins_after 0 means prepend
  {
    t->tl = last->tl;
    last->tl = t;
  }
  else
  {
    t->tl = u->tl;
    u->tl = t;
    if (u == last) 
      last = t;
  }
  return Pix(t);
}


void AddrPtrSLList::del_after(Pix p)
{
  AddrPtrSLListNode* u = (AddrPtrSLListNode*)p;
  if (last == 0 || u == last) error("cannot del_after last");
  if (u == 0) u = last; // del_after 0 means delete first
  AddrPtrSLListNode* t = u->tl;
  if (u == t)
    last = 0;
  else
  {
    u->tl = t->tl;
    if (last == t)
      last = u;
  }
  delete t;
}

int AddrPtrSLList::owns(Pix p)
{
  AddrPtrSLListNode* t = last;
  if (t != 0 && p != 0)
  {
    do
    {
      if (Pix(t) == p) return 1;
      t = t->tl;
    } while (t != last);
  }
  return 0;
}

AddrPtr AddrPtrSLList::remove_front()
{
  if (last == 0) error("remove_front of empty list");
  AddrPtrSLListNode* t = last->tl;
  AddrPtr res = t->hd;
  if (t == last)
    last = 0;
  else
    last->tl = t->tl;
  delete t;
  return res;
}

int AddrPtrSLList::remove_front(AddrPtr& x)
{
  if (last == 0)
    return 0;
  else
  {
    AddrPtrSLListNode* t = last->tl;
    x = t->hd;
    if (t == last)
      last = 0;
    else
      last->tl = t->tl;
    delete t;
    return 1;
  }
}


void AddrPtrSLList::del_front()
{
  if (last == 0) error("del_front of empty list");
  AddrPtrSLListNode* t = last->tl;
  if (t == last)
    last = 0;
  else
    last->tl = t->tl;
  delete t;
}

int AddrPtrSLList::OK()
{
  int v = 1;
  if (last != 0)
  {
    AddrPtrSLListNode* t = last;
    long count = LONG_MAX;      // Lots of chances to find last!
    do
    {
      count--;
      t = t->tl;
    } while (count > 0 && t != last);
    v &= count > 0;
  }
  if (!v) error("invariant failure");
  return v;
}
