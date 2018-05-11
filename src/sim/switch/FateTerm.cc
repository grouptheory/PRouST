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
static char const _FateTerm_cc_rcsid_[] =
"$Id: FateTerm.cc,v 1.23 1999/03/05 17:30:40 marsh Exp $";
#endif

#include "FateTerm.h"
#include "FateVisitor.h"

#include <DS/random/distributions.h>
#include <FW/basics/diag.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/netstats/NetStatsCollector.h>
#include <codec/pnni_ig/id.h>
#include <codec/q93b_msg/setup.h>
#include <codec/uni_ie/bhli.h>

FateTimer::FateTimer( FateTerm * owner, Visitor * key, double duration )
  : TimerHandler( owner, duration ), _key( key ), _owner( owner ) { }
FateTimer::~FateTimer( ) { }

void FateTimer::Callback(void)
{
  _owner->ReleaseCall( _key );
}

// -----------------------------------------
const VisitorType * FateTerm::_fast_uni_type = 0;
const VisitorType * FateTerm::_fate_type = 0;
int                 FateTerm::_counter = 0;

FateTerm::FateTerm( const NodeID * myNID ) : Terminal( ) 
{ 
  _nid = new NodeID( *myNID );

  if ( ! _fast_uni_type )
    _fast_uni_type = QueryRegistry( FAST_UNI_VISITOR_NAME );
  if ( ! _fate_type )
    _fate_type = QueryRegistry( FATE_VISITOR_NAME );

  AddPermission( "ACAC", new FateTermInterface( this ) );
}

FateTerm::~FateTerm( ) { delete _nid; }

void FateTerm::Absorb(Visitor * v)
{
  if ( v->GetType().Is_A( _fast_uni_type ) ) {
    FastUNIVisitor * fuv = (FastUNIVisitor *)v;
    switch ( fuv->GetMSGType() ) {
      case FastUNIVisitor::FastUNIConnect:
	break;
      default:
	break;
    }
  } else if ( v->GetType().Is_A( _fate_type ) ) {
    FateVisitor * fv = (FateVisitor *)v;
    CREFNotification( fv->getVP(), fv->getCREF() );
  }
  v->Suicide();
}

void FateTerm::CREFNotification( const Visitor * v, long cref )
{
  FastUNIVisitor * fv = (FastUNIVisitor *)v;
  diag( "sim.fate", DIAG_DEBUG, "FateTerm mapping cref %d --> %s.\n",
	cref, fv->GetDestNID()->Print() );
	
  dic_item di = _vis2cref.lookup( v );
  //  assert( di != 0 );
  if ( ! di ) // crankback screws things up a bit, we need some way to clean out old setups
    di = _vis2cref.insert( v, -1 );
  _vis2cref.change_inf( di , cref );
}

void FateTerm::SetupCall( Visitor * v, bool exp, double duration )
{
  _vis2cref.insert( v, -1 );

  if ( duration > 0 ) {
    FateTimer * ft = 0;

    if ( exp == true )
      ft = new FateTimer( this, v, duration );
    else
      ft = new FateTimer( this, v, poisson_distribution::sample( duration ) );
    assert( ft != 0 );
    
    Register( ft );
  }
  FastUNIVisitor * fv = (FastUNIVisitor *)v;
  // insert a BHLI IE containing _counter
  _counter++;
  u_char buf[4];
  buf[0] = (_counter >> 24) & 0xFF;
  buf[1] = (_counter >> 16) & 0xFF;
  buf[2] = (_counter >>  8) & 0xFF;
  buf[3] = (_counter      ) & 0xFF;
  user_bhli * bhli = new user_bhli( buf, 4 );
  q93b_setup_message * msg = fv->GetSetup( );
  if ( msg != 0 ) {
    InfoElem ** ies = msg->get_ie_array( );
    ies[ InfoElem::ie_bhli_ix ] = bhli;
    fv->SetSetup( msg );
  }
  
  char tmp[32];
  sprintf( tmp, "%d", _counter );
  theNetStatsCollector().ReportNetEvent( "User_Call_Submission",
					 OwnerName(),
					 tmp,
					 _nid );

  double time = theKernel().CurrentElapsedTime();
  diag( "sim.fate", DIAG_DEBUG, "Call setup to %s at time %lf.\n",
	fv->GetDestNID()->Print(), time );

  Inject( v );
}

void FateTerm::InjectionNotification(const Visitor * v) const
{
  // we cannot do this until all FastUNIVisitors go thru SetupCall
  //  if ( v->GetType().Is_A( _fast_uni_type ) )
  //    assert( _vis2cref.lookup( (Visitor *)v ) != 0 );
}

void FateTerm::ReleaseCall( Visitor * key )
{
  dic_item di = _vis2cref.lookup( key );
  assert( di != 0 );
  long cref = _vis2cref.inf( di );
  if ( cref != -1 ) {
    // I hope this is enough information for ACAC!
    FastUNIVisitor * fuv = new FastUNIVisitor( 0, 0, 0, 0, 0, 
					       cref, 
					       FastUNIVisitor::FastUNIRelease );
    fuv->SetOutPort( 0 );
    fuv->SetInPort( 0 );

    double time = theKernel().CurrentElapsedTime();
    diag( "sim.fate", DIAG_DEBUG, "Releasing call (cref %d) at time %lf.\n",
	  cref, time );

    Inject( fuv );
  } 
  _vis2cref.del_item( di );
}

void FateTerm::Interrupt( SimEvent * se )
{
  diag("sim.fate", DIAG_FATAL, "FateTerm::Interrupt received a SimEvent when it wasn't expecting one!\n" );
}

// -----------------------------------------
void FateTermInterface::CREFNotification( Visitor * v, long cref )
{
  _ft->CREFNotification( v, cref );
}

FateTermInterface::FateTermInterface( FateTerm * ft )
  : fw_Interface( ft ), _ft( ft ) { assert( _ft != 0 ); }

FateTermInterface::~FateTermInterface( ) { }

void FateTermInterface::ShareableDeath(void)
{
  _ft = 0;
}
