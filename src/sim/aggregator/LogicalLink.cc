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
static char const _LogicalLink_cc_rcsid_[] =
"$Id: LogicalLink.cc,v 1.6 1999/03/05 17:01:51 mountcas Exp $";
#endif

#include <common/cprototypes.h>

#include "LogicalLink.h"
#include "LogicalPeer.h"
#include "AggregationPolicy.h"
#include "Aggregator.h"
#include "Anchor.h"

#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/pnni_ig/resrc_avail_info.h>

#include <fsm/hello/HelloVisitor.h>
#include <fsm/netstats/NetStatsCollector.h>

#include <FW/basics/diag.h>

LogicalLink::LogicalLink(int id, 
			 AggregationPolicy * agg,
			 LogicalPeer * parent) : 
  TimerHandler(agg->PTSERefreshInterval()), _agg(agg), 
  _id(id), _seqnum(1), _dirty(false), _saved_hlink(0), _saved_uplink(0), 
  _rNID(0), _rport(0), _lport(0), _aggtok(0), _parent(parent)
{ }

LogicalLink::~LogicalLink() 
{ 
  dic_item di;
  // clear the anchor dictionary
  forall_items(di, _uplinks) {  
    delete _uplinks.key(di);    
    delete _uplinks.inf(di);
  }
  _uplinks.clear();
  _induced_ids.clear();
  _induced_seqnum.clear();
}


int LogicalLink::Update(int id_of_dying_ptse, NodeID * origin) 
{
  int affected = 0;
  if ( ! _agg->_master->AmILeader((int)(origin->GetLevel())))
    return affected; // I am NOT leader in the PG of the originator of this uplink...

  ig_uplinks * lnk = 0;
  Anchor * anc = 0;

  dic_item di = Scan(id_of_dying_ptse, origin);
  assert(di);
  
  lnk = _uplinks.inf(di);;
  anc = _uplinks.key(di);
  _uplinks.del_item(di);   // remove the expired uplink
  affected = 1;

  // this logical link is still justified?
  if ( ! Disconnected() ) { 

    // Do I need to recalculate the induced uplinks at the next higher level?
    // Do I need to recalculate the higher-level Hlink?

    NodeID * my_address = _agg->_master->_me->copy();
    const NodeID * upnode = lnk->GetRemoteID();
    int properLevel = _agg->_master->GetNextLevelUp(origin->GetLevel());

    //   If the upnode level is not equal to my next level
    if (upnode->GetLevel() != properLevel) { 
      //  we do need to originate an (INDUCED) uplinks ig at the next level.
      NodeID * next_level = my_address->copy();
      next_level->SetLevel( properLevel );
      next_level->SetChildLevel(_agg->_master->GetNextLevelDown(properLevel));

      ig_uplinks * ins_ulig = (ig_uplinks *)( lnk->copy() );
      list<ig_resrc_avail_info *> * lp;
      // ask the aggregation policy to combine the uplink raigs
      if (lp = _agg->AggregateUpLinks(&_uplinks, 0)) {

	// save the ULIA for now as we do not support ULIA formation
	// in the aggregation policy object (yet)
	ig_uplink_info_attr * ulia = 0;

	list<InfoGroup *> * igs = (list<InfoGroup *>*) ( ins_ulig->ShareIGs() );
	list_item iter;
	forall_items(iter, *igs) {
	  InfoGroup * ig = igs->inf(iter);
	  if (ig->GetId()==InfoGroup::ig_uplink_info_attr_id) {
	    ulia = (ig_uplink_info_attr*)ig;
	  }
	}
	assert(ulia);

	// empty out the guts of the ULIG
	while( igs->size() > 0) {
	  igs->del_item( igs->first() );
	}
	igs->clear();
	igs->insert( ulia ); // put back the ULIA

	forall_items(iter, *lp) {
	  ins_ulig->AddIG( lp->inf(iter)); // shove in the aggregated RAIGs
	}

	dic_item di_seqnum = _induced_seqnum.lookup( properLevel );
	int seqnum = 1;
	if (di_seqnum) {
	  seqnum = 1 + _induced_seqnum.inf( di_seqnum );
	  _induced_seqnum.change_inf( di_seqnum, seqnum );
	}
	else _induced_seqnum.insert( properLevel, seqnum );

	dic_item di_id = _induced_ids.lookup( properLevel );
	int id = 0;
	if (di_id) id = _induced_ids.inf( di_id );
	else {
	  id = _agg->_master->NextID();
	  _induced_ids.insert( properLevel, id );
	}  

	NodeID * ind_uplink_origin = _agg->_myNode->copy();
	ind_uplink_origin->SetLevel( _rNID->GetLevel() );
	ind_uplink_origin->SetChildLevel( _agg->
					  _master->
					  GetNextLevelDown(_rNID->GetLevel()));
	char uplink_name[256];
	sprintf(uplink_name, "%s-->%s", 
		origin->Print(), 
		_rNID->Print());

	// call the NetStatsCollector with type Induced_Uplink
	theNetStatsCollector().ReportNetEvent("Induced_Uplink",
					      _agg->GetName(),
					      uplink_name,
					      ind_uplink_origin );
	delete ind_uplink_origin;

	// bat the induced uplink into the database
	_agg->InjectPTSE(ins_ulig, id, seqnum, next_level->copy());
      }
    }
    else {
      // upnode level IS equal to my next level,
      // the induced uplinks have reached the top at last...
      if ( HLinkUp( _aggtok ) ) {
	// Aggregate the higher HLink, if the LGNHellos are in 2-way
	Aggregate( 0 );
      }
    }
    delete my_address;
  }
  else {
    // this logical link is no longer justifiable!
    // TODO
  }

  delete lnk;
  delete anc;
  return affected;
}

void LogicalLink::Update(int id_of_new_ptse, NodeID* origin, ig_uplinks * lnk)
{

  if ( ! _agg->_master->AmILeader((int)(origin->GetLevel())))
    return; // I am NOT leader in the PG of the originator of this uplink...

  dic_item di;
  if (di = Scan(id_of_new_ptse, origin)) {
    
    /* sanity checking to see if invariants are satisfied.
       network instability requires we ignore upnode identity here...
       */
    ig_uplinks * u = _uplinks.inf(di);
    assert ( (lnk->GetLocalPID() == u->GetLocalPID()) &&
	     (lnk->GetAggTok()   == u->GetAggTok()) );
    
    assert( lnk != _uplinks.inf(di) );  // Oh God, not the same object twice!
    delete _uplinks.inf(di);
    delete _uplinks.key(di);
    _uplinks.del_item(di);   // remove any old one we might be superceding
  }
  
  // a new anchor in the dictionary
  Anchor * anch = new Anchor(id_of_new_ptse, origin, lnk->GetLocalPID());
  _uplinks.insert( anch, (ig_uplinks *)lnk->copy() );
  
  if (_rNID) delete _rNID;
  _rNID   = lnk->GetRemoteID();
  _aggtok = lnk->GetAggTok();
  _dirty  = true;

  NodeID * my_address = _agg->_master->_me->copy();

  // Do I need to make an induced uplink at the next higher level?
  // Do I need to make a higher-level Hlink?

  const NodeID * upnode = lnk->GetRemoteID();
  int properLevel = _agg->_master->GetNextLevelUp(origin->GetLevel());

  //   If the upnode level is not equal to my next level
  if (upnode->GetLevel() != properLevel) { 
    //  we do need to originate an (INDUCED) uplinks ig at the next level.
    NodeID * next_level = my_address->copy();
    next_level->SetLevel( properLevel );
    next_level->SetChildLevel(_agg->_master->GetNextLevelDown(properLevel));

    ig_uplinks * ins_ulig = (ig_uplinks *)( lnk->copy() );

    list<ig_resrc_avail_info *> * lp;
    // ask the aggregation policy to combine the uplink raigs
    if (lp = _agg->AggregateUpLinks(&_uplinks, 0)) {

      // save the ULIA for now as we do not support ULIA formation
      // in the aggregation policy object (yet)
      ig_uplink_info_attr * ulia = 0;
      list<InfoGroup *> * igs = (list<InfoGroup *>*) ( ins_ulig->ShareIGs() );
      list_item iter;
      forall_items(iter, *igs) {
	InfoGroup * ig = igs->inf(iter);
	if (ig->GetId()==InfoGroup::ig_uplink_info_attr_id) {
	  ulia = (ig_uplink_info_attr*)ig;
	}
      }

      // empty out the guts of the ULIG
      while( igs->size() > 0) {
	igs->del_item( igs->first() );
      }
      igs->clear();
      if (ulia)
	igs->insert( ulia ); // put back the ULIA

      forall_items(iter, *lp) {
	ins_ulig->AddIG( lp->inf(iter)); // shove in the aggregated RAIGs
      }

      dic_item di_seqnum = _induced_seqnum.lookup( properLevel );
      int seqnum = 1;
      if (di_seqnum) {
	seqnum = 1 + _induced_seqnum.inf( di_seqnum );
	_induced_seqnum.change_inf( di_seqnum, seqnum );
      }
      else _induced_seqnum.insert( properLevel, seqnum );
    
      dic_item di_id = _induced_ids.lookup( properLevel );
      int id = 0;
      if (di_id) id = _induced_ids.inf( di_id );
      else {
	id = _agg->_master->NextID();
	_induced_ids.insert( properLevel, id );
      }  

      NodeID * ind_uplink_origin = _agg->_myNode->copy();
      assert( _rNID != 0 );
      ind_uplink_origin->SetLevel( _rNID->GetLevel() );
      ind_uplink_origin->SetChildLevel( _agg->
					_master->GetNextLevelDown( _rNID->GetLevel() ) );
      char uplink_name[256];
      sprintf( uplink_name, "%s-->%s", 
	       origin->Print(), 
	       _rNID->Print() );

      // call the NetStatsCollector with type Induced_Uplink
      theNetStatsCollector().ReportNetEvent("Induced_Uplink",
					    _agg->GetName(),
					    uplink_name,
					    ind_uplink_origin );
      delete ind_uplink_origin;

      // bat the induced uplink into the database
      _agg->InjectPTSE(ins_ulig, id, seqnum, next_level->copy());
    }
  }
  else {
    // upnode level IS equal to my next level,
    // the induced uplinks have reached the top at last...
    if ( HLinkUp( _aggtok ) ) {
      // Aggregate the higher HLink, if the LGNHellos are in 2-way
      Aggregate( 0 );
    }
  }

  delete my_address;
}

void LogicalLink::Aggregate(HorLinkVisitor * hv)
{
  if (!_dirty) {
    // doh!
    diag("sim.aggregator", DIAG_DEBUG, 
	 "LogicalLink %d: Nothing to aggregate (# of uplinks is %d).\n",
	 _aggtok, _uplinks.size());
    return;
  }

  // we now make a higher-level HLink
  list_item li;
  list<ig_resrc_avail_info *> * lp;
  // originate a new hlink to reflect modified bundle
  // re-aggregates, then refreshes PTSE in the DB
  if (lp = _agg->AggregateUpLinks(&_uplinks, _saved_hlink)) {
    if (hv) {
      _lport = hv->GetLocalPort();  // now we know the logical ports
      _rport = hv->GetRemotePort();
    }
    ig_horizontal_links * hlig = 
      new ig_horizontal_links(0, _rNID, _rport, _lport, _aggtok);
    forall_items(li, *lp) {
      hlig->AddRAIG(lp->inf(li));
    }
    if (_saved_hlink)       // used for re-origination 
      delete _saved_hlink;
    _saved_hlink = (ig_horizontal_links *)hlig->copy();

    NodeID * hlink_origin= _agg->_myNode->copy();
    hlink_origin->SetLevel( _rNID->GetLevel() );
    hlink_origin->SetChildLevel( _agg->
				 _master->
				 GetNextLevelDown(_rNID->GetLevel()));

    // call the NetStatsCollector with type Hlink_Aggr
    theNetStatsCollector().ReportNetEvent("Hlink_Aggr",
					  _agg->GetName(),
					  _rNID->Print(), hlink_origin );
    delete hlink_origin;

    _agg->InjectPTSE(hlig, _id, _seqnum++);
  }
  _dirty = false;
}

void LogicalLink::Callback(void)
{
  if ( ! HLinkUp( _aggtok )) {
    ExpiresIn(_agg->PTSERefreshInterval());
    Register(this);
    return;
  }

  // Reoriginate the hlink IG
  if (!_saved_hlink) {
    list_item li;
    list<ig_resrc_avail_info *> * lp;

    if (lp = _agg->AggregateUpLinks(&_uplinks, _saved_hlink)) {
      ig_horizontal_links * hlig = 
	new ig_horizontal_links(0, _rNID, _rport, _lport, _aggtok);
      forall_items(li, *lp) {
	hlig->AddRAIG(lp->inf(li));
      }
      _saved_hlink = (ig_horizontal_links *)hlig->copy();
    }
  }
  _agg->InjectPTSE(_saved_hlink, _id, _seqnum++);
  // reregister to continue reoriginating the logical hlinks
  ExpiresIn(_agg->PTSERefreshInterval());
  Register(this);
}

bool LogicalLink::Disconnected(void) 
{
  return (_uplinks.empty());
}

dic_item LogicalLink::Scan(int id_of_new_ptse, NodeID * origin) 
{
  dic_item di;
  forall_items(di, _uplinks) {
    Anchor* anch   = _uplinks.key(di);
    if (anch->GetID() == id_of_new_ptse) {
      NodeID * uorigin = _uplinks.key(di)->ShareOrigin();
      assert (origin->equals( uorigin ));
      return di;
    }
  }
  return 0;
}

bool LogicalLink::HLinkUp( int token )
{
  return _parent->HLinkUp( token );
}

void LogicalLink::SetPorts(int lp, int rp)
{
  _lport = lp; _rport = rp;
}

int LogicalLink::localPort(void) const
{
  return _lport;
}

int LogicalLink::remotePort(void) const
{
  return _rport;
}


