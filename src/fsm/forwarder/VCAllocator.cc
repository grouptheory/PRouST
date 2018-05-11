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
static char const _VCAllocator_cc_rcsid_[] =
"$Id: VCAllocator.cc,v 1.12 1998/10/15 19:11:18 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <fsm/forwarder/VCAllocator.h>
#include <FW/basics/diag.h>

#define Q93B_DEBUGGING "fsm.nni.VCAllocator_debugging"

// fill the svc free list with all possible vci's
VCAllocator::VCAllocator(void) :
  _lowest(MinVCI + 1), _ref_count(1)
{ }

VCAllocator::~VCAllocator() 
{ 
  _svc_free.clear();
  _svc_in_use.clear();
}

VCAllocator * VCAllocator::Reference(void) { _ref_count++; return this; }

void VCAllocator::UnReference(void) 
{ 
  if (! --_ref_count)
    delete this;
  else if (_ref_count < 0) {
    diag(Q93B_DEBUGGING, DIAG_ERROR, 
	 "Somone called delete on the VCAllocator with a negative reference count.\n");
  }
}


// Get the first free vci in the list - return vci=0 if the list is empty
unsigned int VCAllocator::GetNewVCI(void)
{
  unsigned int vci = 0;

  // If the free list is smaller than we'd like allocate more vc's
  if (_svc_free.size() < MinSize) {
    for (unsigned int i = _lowest; i < _lowest + 64 && i > 0; i++) 
      _svc_free.append(i);
    _lowest += 64;
  }

  if (!_svc_free.empty()) {
    vci = _svc_free.pop();
    list_item li;
    if (li = _svc_free.search(vci))
      _svc_free.del_item(li);
    _svc_in_use.append(vci); 
  }
  return vci;
}


// * verify that vci is not in use - then allocate it (move from free to in_use)
// * returns vci if ok to use the vci
// * returns 0 if vci is already in use (how do we do pmp?)
unsigned int VCAllocator::RequestVCI(unsigned int vci)
{
  list_item it;
  if (it = _svc_free.search(vci)) {
    _svc_free.del_item(it);
    _svc_in_use.append(vci);
    return vci;
  }
  return 0;
}

/* return vci to free list - remove from used list */
void VCAllocator::ReturnVCI(unsigned int vci)
{
  list_item it;
  if (it = _svc_in_use.search(vci)) {
    _svc_in_use.del_item(it);
    _svc_free.append(vci);
  }
  // otherwise you were mistaken
  //    ... but let's not rub your face it in.
  // return (you_are_such_a_fool);
}
