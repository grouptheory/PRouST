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
#ifndef __FATETERM_H__
#define __FATETERM_H__

#ifndef LINT
static char const _FateTerm_h_rcsid_[] =
"$Id: FateTerm.h,v 1.13 1999/03/05 17:30:40 marsh Exp $";
#endif

#include <FW/kernel/Handlers.h>
#include <FW/actors/Terminal.h>
#include <FW/interface/Interface.h>

class Visitor;
class FateTerm;

class FateTimer : public TimerHandler {
public:

  FateTimer( FateTerm * owner, Visitor * key, double duration );
  virtual ~FateTimer( );

  void Callback(void);

private:

  FateTerm * _owner;
  Visitor  * _key;
};

class NodeID;
class FateTerm : public Terminal {
  friend class FateTimer;
public:

  FateTerm(const NodeID * nid);
  virtual ~FateTerm( );

  void Absorb(Visitor * v);
  void CREFNotification( const Visitor * v, long cref );
  void SetupCall( Visitor * v, bool exp, double duration );
  void Interrupt( SimEvent * se );

protected:

  void InjectionNotification( const Visitor * v ) const;
  void ReleaseCall( Visitor * key );

  NodeID                            * _nid;
  dictionary< const Visitor *, long > _vis2cref;

  static const VisitorType * _fate_type;
  static const VisitorType * _fast_uni_type;
  static int                 _counter;
};

class FateTermInterface : public fw_Interface {
  friend class FateTerm;
public:

  void CREFNotification( Visitor * v, long cref );

protected:

  FateTermInterface( FateTerm * ft );
  virtual ~FateTermInterface( );

  void ShareableDeath(void);

  FateTerm * _ft;
};

#endif
