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
static char const _Cluster_cc_rcsid_[] =
"$Id: Cluster.cc,v 1.1 1999/01/13 19:12:50 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <FW/behaviors/Cluster.h>

#include <FW/basics/Conduit.h>
#include <FW/actors/Expander.h>
#include <FW/behaviors/Mux.h>
#include <FW/kernel/KDB.h>
#include <FW/interface/ShareableRegistry.h>

#include <iostream.h>

Cluster::Cluster(Expander * e, Accessor * accA, Accessor * accB) : 
  Behavior(e, Behavior::CLUSTER),
  _accA(accA), _accB(accB), _mA(0), _mB(0), _muxA(0), _muxB(0) 
{ 
  char buf[Conduit::MAX_LOG_SIZE];

  if (accA) {
    _mA = new Mux(accA);
    _muxA = new Conduit("MuxA", _mA);
    sprintf(buf, "!add %s to %lx", _muxA->GetName(), e);
    theLog().AppendToRecord(buf);
    e->_children.append(_muxA);
  }
  if (accB) {
    _mB = new Mux(accB);
    _muxB = new Conduit("MuxB", _mB);
    sprintf(buf, "!add %s to %lx", _muxB->GetName(), e);
    theLog().AppendToRecord(buf);
    e->_children.append(_muxB);
  }
}

Cluster::~Cluster() { delete _muxA; delete _muxB; }

bool Cluster::IsComposite(void) const { return (bool)true; }

void Cluster::SetOwner(Conduit * c)
{
  Behavior::SetOwner(c);

  if (_mA && _muxA) {
    c->set_sideA(B_half(_muxA));
    Join( A_half(_muxA), A_half((Expander *)_actor) );
  } else 
    c->set_sideA(((Expander *)_actor)->GetAHalf());

  if (_mB && _muxB) {
    c->set_sideB(B_half(_muxB));
    Join( A_half(_muxB), B_half((Expander *)_actor) );
  } else 
    c->set_sideB(((Expander *)_actor)->GetBHalf());
}

void Cluster::SetOwnerName(const char * const name)
{
  char buf[1024];

  if (_actor)  {
    _actor->SetParent( GetOwner() );
    _actor->SetOwnerName(name);
  }
}

void Cluster::Accept(Visitor * v, int is_breakpoint)
{
  cout << "FATAL ERROR.  Cluster::Accept() called.\n" << flush;
  abort();
}

bool Cluster::ConnectA(Conduit *a, Visitor *v)
{
  if (_mA) return _mA->ConnectB(a,v);
  else {
    Conduit * border = A_half((Expander *)_actor);
    switch (border->_h){
    case Conduit::A_HALF: 
      return border->_behavior->ConnectA(a,v);
      break;
    case Conduit::B_HALF: 
      return border->_behavior->ConnectB(a,v);
      break;
    default:
      return false;
    }
  }
}

bool Cluster::DisconnectA(Conduit *a, Visitor *v)
{
  if (_mA) return _mA->DisconnectB(a,v);
  else {
    Conduit * border = A_half((Expander *)_actor);
    switch (border->_h){
    case Conduit::A_HALF: 
      return border->_behavior->DisconnectA(a,v);
      break;
    case Conduit::B_HALF: 
      return border->_behavior->DisconnectB(a,v);
      break;
    default:
      return false;
    }
  }
}

bool Cluster::ConnectB(Conduit *b, Visitor *v)
{
  if (_mB) return _mB->ConnectB(b,v);
  else {
    Conduit * border = B_half((Expander *)_actor);
    switch (border->_h){
    case Conduit::A_HALF: 
      return border->_behavior->ConnectA(b,v);
      break;
    case Conduit::B_HALF: 
      return border->_behavior->ConnectB(b,v);
      break;
    default:
      return false;
    }
  }
}

bool Cluster::DisconnectB(Conduit *b, Visitor *v)
{
  if (_mB) return _mB->DisconnectB(b,v);
  else {
    Conduit * border = B_half((Expander *)_actor);
    switch (border->_h){
    case Conduit::A_HALF: 
      return border->_behavior->DisconnectA(b,v);
      break;
    case Conduit::B_HALF: 
      return border->_behavior->DisconnectB(b,v);
      break;
    default:
      return false;
    }
  }
}

void Cluster::Suicide(void)
{
  if (_mA) {
    delete OwnerSideA();
    _muxA = 0;
  }
  if (_mB) {
    delete OwnerSideB();
    _muxB = 0;
  }
  delete this;
}
