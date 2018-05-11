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

#ifndef __DTL_H__
#define __DTL_H__
#ifndef LINT
static char const _DTL_h_rcsid_[] =
"$Id: DTL.h,v 1.2 1998/12/18 14:24:21 mountcas Exp marsh $";
#endif

#include <DS/containers/list.h>
#include <codec/uni_ie/PNNI_designated_transit_list.h>
class DTLContainer; // contains (NodeID,Port) pair

// Designateted Transit Peer List
// all entries are at the same level.
class DTLPeerList {
public:
  DTLPeerList();
  DTLPeerList(list <DTLContainer *> dtl);
  ~DTLPeerList();
  int index();
  void set_index(int i);
  int size();
  DTLContainer *operator [] (int i);
private:
  int _index;			// rank into list
  list <DTLContainer *> _dtl;	// list of (NodeID,Port) pairs
};

// 
class DTL {
public:
  DTL (list <DTLPeerList *> *newDTL);
  ~DTL(void);

  void Insert (list <DTLPeerList *> *newDTL);
  // returns pointer to current DTLcontainer 
  const DTLContainer *Peek();

  // advances the pointer within this DTL and returns a copy of
  // the next DTLcontainer
  // if the pointer is at the end of the list, a null is returned
  // The caller must then pop this DTL off the list of DTl's and delete it
  DTLContainer *Pop();

  // return pointer
  const DTLContainer *Next();
private:
  void AdvancePointer();
  list <DTLPeerList *> _peers;
};

int  DTLPeerList::index() { return _index; }
void DTLPeerList::set_index(int i) { _index = i; }
DTLContainer * DTLPeerList::operator [] (int i) { return this->_dtl[i]; }
int DTLPeerList::size() { return _dtl.empty()? 0 : _dtl.size(); } 

DTLContainer *DTL::Pop ()
{
  if (_peers.empty()) return 0;

  DTLPeerList *top = _peers.inf(_peers.first());

  DTLContainer *dp = new DTLContainer((*top)[top->index()]);

  AdvancePointer();
  return dp;
}


void DTL::AdvancePointer()
{
  if (_peers.empty()) return ; // nothing to do;

  DTLPeerList *top = _peers.inf(_peers.first());
  int index = top->index();
  index++;
  if (index < top->size())
    top->set_index(index);
  else {
    while (index == top->size()) {
      delete _peers.pop(); // perhaps save somewhere like old version did.
      if (_peers.empty()) return ; // nothing to do;
      
      DTLPeerList *top = _peers.inf(_peers.first());
      int index = top->index();
      index++;
      if (index < top->size())
	top->set_index(index);
    }
  }
}

#endif
