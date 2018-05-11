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
static char const _NID2VCMapper_cc_rcsid_[] =
"$Id: NID2VCMapper.cc,v 1.17 1999/02/12 16:14:14 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "NID2VCMapper.h"
#include <FW/basics/diag.h>
#include <codec/pnni_ig/id.h>
#include <fsm/visitors/FastUNIVisitor.h>

const VisitorType * NID2VCMapper::_fastuni_type = 0;

NID2VCMapper::NID2VCMapper( void )
{ 
  if (!_fastuni_type)
    _fastuni_type = QueryRegistry(FAST_UNI_VISITOR_NAME);
  assert( _fastuni_type );
}

NID2VCMapper::~NID2VCMapper() 
{ 
  dic_item di;
  forall_items(di, _rnid_to_vc) {
    NodeID      * n = (NodeID *)_rnid_to_vc.key(di);
    VCcontainer * v = _rnid_to_vc.inf(di);
    delete n;
    delete v;
  }
  _rnid_to_vc.clear();
}

State * NID2VCMapper::Handle(Visitor * v)
{
  if (v->GetType().Is_A(_fastuni_type))
    HandleSVC((FastUNIVisitor *)v);

  PassThru(v);
  return this;
}

void    NID2VCMapper::Interrupt(SimEvent * e) { }

void    NID2VCMapper::HandleSVC(FastUNIVisitor * v)
{
  VCcontainer * vcc = 0;

  switch (VisitorFrom(v)) {
    case Visitor::A_SIDE:  // Outside
      if (v->GetMSGType() == FastUNIVisitor::FastUNISetup) {
	const NodeID * src = v->GetSourceNID();

	// If we are coming from the outside, we may already have a mapping for this node
	if ( src != 0 ) {
	  if ( Find(src) )
	    Remove( src );
	  Insert(src, v->GetOutVP(), v->GetOutVC());
	} else if (!src || src->IsZero())
	  diag("sim.ctrl.NID2VCMapper", DIAG_WARNING, 
	       "FastUNISetup (SVCC Setup) does not have a valid Source Node ID.\n");
      }
      break;
    case Visitor::B_SIDE: // Inside
      if ((v->GetMSGType() == FastUNIVisitor::FastUNISetup) &&
	  !Find(v->GetDestNID()))
	Insert(v->GetDestNID(), v->GetOutVP(), v->GetOutVC());
      else if (vcc = Find(v->GetDestNID())) {
	v->SetInVP(vcc->_vp);
	v->SetInVC(vcc->_vc);
      }
      break;
  }
}

void NID2VCMapper::Insert(const NodeID * nid, int vp, int vc)
{
  // added 02/12/99 mountcas
  assert( vp >= 0 && vc >= 0 );

  if (!_rnid_to_vc.lookup(nid))
    _rnid_to_vc.insert(nid->copy(), new VCcontainer(vp, vc));
}

NID2VCMapper::VCcontainer * NID2VCMapper::Find(const NodeID * nid)
{
  dic_item di;
  if (di = _rnid_to_vc.lookup(nid))
    return _rnid_to_vc.inf(di);
  return 0;
}

void NID2VCMapper::Remove(const NodeID * nid)
{
  dic_item di = _rnid_to_vc.lookup(nid);
  assert( di != 0 );
  delete _rnid_to_vc.key( di );
  delete _rnid_to_vc.inf( di );
  _rnid_to_vc.del_item( di );
}
