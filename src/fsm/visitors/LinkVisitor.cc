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

/** -*- C++ -*-
 * File: LinkVisitor.cc
 * @author talmage
 * @version $Id: LinkVisitor.cc,v 1.43 1998/10/20 13:05:57 mountcas Exp $
 *
 * Carries Link messages around in a switch.
 *
 * BUGS:
 */
#ifndef LINT
static char const rcsid[] =
"$Id: LinkVisitor.cc,v 1.43 1998/10/20 13:05:57 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <fsm/visitors/LinkVisitor.h>
#include <FW/basics/diag.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/resrc_avail_info.h>

LinkVisitor::LinkVisitor(int port, int agg, 
			 const NodeID * local, 
			 const NodeID * remote) 
  : NodeIDVisitor(_mytype, port, agg, remote, local), 
    _local_port(-1), _remote_port(-1), _raigs(0) {  }

LinkVisitor::LinkVisitor(const LinkVisitor & rhs) 
  : NodeIDVisitor(rhs), _local_port(rhs._local_port), 
    _remote_port(rhs._remote_port), _raigs(0)
{ 
  if (rhs._raigs) {
    list_item li;
    forall_items(li, *(rhs._raigs)) {
      ig_resrc_avail_info * raig = rhs._raigs->inf(li);
      AddRAIG(raig);      
    }
  }
}

LinkVisitor::LinkVisitor(vistype & child_type, int port, int agg, 
			 const NodeID * local, const NodeID * remote) 
  : NodeIDVisitor(child_type.derived_from(_mytype), port, agg, remote, local),
    _local_port(-1), _remote_port(-1), _raigs(0) {  }

LinkVisitor::~LinkVisitor() 
{
  if (_raigs) {
    while (!_raigs->empty()) {
      ig_resrc_avail_info * the_raig = _raigs->pop();
      delete the_raig;
    }
    _raigs->clear();
    delete _raigs;
    _raigs = 0;
  }
}

Visitor * LinkVisitor::dup(void) const { return new LinkVisitor(*this); }

void LinkVisitor::on_death(void) const
{
  DIAG(FSM, DIAG_INFO, 
       if (DumpLog(cout)) cout << endl;);
}

const vistype & LinkVisitor::GetClassType(void) const
{  return _mytype; }

const VisitorType LinkVisitor::GetType(void) const 
{  return VisitorType(GetClassType());  }

const int LinkVisitor::GetLocalPort(void) const  { return _local_port; }
const int LinkVisitor::GetRemotePort(void) const { return _remote_port; }

void      LinkVisitor::SetLocalPort(int port)  { _local_port = port; }
void      LinkVisitor::SetRemotePort(int port) { _remote_port = port; }

list<ig_resrc_avail_info *> * LinkVisitor::GetRAIGs(void)
{ 
  list<ig_resrc_avail_info *> * ptr = _raigs;
  _raigs = 0;
  return ptr;
}

// I make a copy so clean yours up, sloppy!
void LinkVisitor::SetRAIGs(const list<ig_resrc_avail_info *> * r)
{
  // Clear our raigs first
  if (_raigs) {
    list_item li;
    forall_items(li, *(_raigs)) {
      delete _raigs->inf(li);
    }
    _raigs->clear();
  }

  if (r) {
    if (!_raigs) 
      _raigs = new list<ig_resrc_avail_info *> ;
    // make a copy of the passed RAIGs
    list_item li;
    forall_items(li, * r) {
      ig_resrc_avail_info * raig = r->inf(li);
      _raigs->append((ig_resrc_avail_info *)raig->copy());
    }
  } else {
    delete _raigs;
    _raigs = 0;
  }
}

// the passed raig is mine now, so don't delete it on me!
void LinkVisitor::AddRAIG(ig_resrc_avail_info * raig)
{
  if (!_raigs)
    _raigs = new list<ig_resrc_avail_info *>;

  _raigs->append(raig);
}


// ----------------------------------------------------
const u_long NO_BW = 0;
const u_long OC3   = 366745;   // Cells/sec  (155.5 MB/s / 8) / 53
const u_long OC12  = 4 * OC3;  // Cells/sec
const u_long OC48  = 4 * OC12; // Cells/sec
const u_long OC192 = 4 * OC48; // Cells/sec

ig_resrc_avail_info * CreateRAIG(long bandwidth)
{
  return new ig_resrc_avail_info(ig_resrc_avail_info::outgoing, 
				 (u_short)(RAIG_FLAG_CBR | RAIG_FLAG_RTVBR |
					   RAIG_FLAG_NRTVBR | RAIG_FLAG_ABR |
					   RAIG_FLAG_UBR | RAIG_FLAG_GCAC_CLP), 
				 5040, bandwidth, bandwidth, 202, 182, 8, 8);
}
