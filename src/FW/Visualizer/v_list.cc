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
static char const _v_list_cc_rcsid_[] =
"$Id: v_list.cc,v 1.1 1999/01/20 22:58:13 mountcas Exp $";
#endif

#include <qpainter.h>
#include <common/cprototypes.h>

#include "v_list.h"
#include "v_main.h"
#include "v_visitor.h"
#include "v_conduit.h"

v_list::v_list(v_main * p)
  : QListBox(p), _parent(p)
{ 
  setFrameStyle( WinPanel | Sunken );
  setScrollBar( true );
  setBottomScrollBar( false );
}

v_list::~v_list() { }

void v_list::notify(v_visitor * v)
{
  _visitors.append(v);

  char text[128];
  sprintf(text, "%s:%s is in %s.", v->GetType(), 
	  v->GetName(), v->CurrentLocation()->GetName());
  insertItem( text );

#if 0
  for (int i = 0; i < count(); i++) {
    if (! strcmp(txt, text(i)) ) {
      paintCell( &p, i, 0 );
      break;
    }
  }
#endif
}

void v_list::remove(v_visitor * v)
{
  list_item li = _visitors.lookup(v);
  if (li != 0)
    _visitors.del_item(li);

  char txt[128];
  sprintf(txt, "%s:%s is in %s.", v->GetType(), 
	  v->GetName(), v->CurrentLocation()->GetName());
  for (int i = 0; i < count(); i++) {
    if (! strcmp(txt, text(i)) ) {
      removeItem( i );
      break;
    }
  }
}

const list<v_visitor *> * v_list::visitors(void) const
{
  return &_visitors;
}
