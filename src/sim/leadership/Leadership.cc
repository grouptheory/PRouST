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
static char const _Leadership_cc_rcsid_[] =
"$Id: Leadership.cc,v 1.184 1999/03/05 16:57:07 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "Leadership.h"
#include "LeadershipEvent.h"
#include "LeadershipInterfaces.h"

#include <FW/basics/diag.h>
#include <FW/basics/Log.h>
#include <FW/kernel/SimEvent.h>
#include <fsm/config/Configurator.h>
#include <fsm/database/Database.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/visitors/LinkVisitor.h>
#include <fsm/visitors/LGNVisitors.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/NPFloodVisitor.h>
#include <fsm/election/ElectionVisitor.h>
#include <fsm/netstats/NetStatsCollector.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/nodal_state_params.h>
#include <codec/pnni_ig/next_hi_level_binding_info.h>
#include <codec/pnni_ig/nodal_hierarchy_list.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_pkt/ptsp.h>
#include <codec/uni_ie/addr.h>
#include <sim/logos/LogosInterfaces.h>
#include <sim/aggregator/AggregatorInterfaces.h>

const int WINNER_DISTANCE    = 50;
const int NODAL_IG_ID        = 1;
const int NODAL_HIERARCHY_ID = 2;

VisitorType * Leadership::_election_vistype   = 0;
VisitorType * Leadership::_fastuni_vistype    = 0;
VisitorType * Leadership::_npflood_vistype    = 0;

Leadership::Leadership(const NodeID * myNode, const NodeID * myPreferredPGL)
  : _init_lev( 0 ), _nsp_seqnum(5), _originating(false)
{ 
  assert( myNode != 0 );
  _init_lev = myNode->GetLevel();

  // default is to prefer ourselves
  if (myPreferredPGL == 0)
    myPreferredPGL = myNode->copy();

  _elections.insert( _init_lev, new LevelInfo( myNode, myPreferredPGL ) );
  if (!_seqs.lookup(myNode))
    _seqs.insert(myNode->copy(), 1);

  if (!_election_vistype)
    _election_vistype = (VisitorType *)QueryRegistry( ELECTION_VISITOR_NAME );
  if (!_fastuni_vistype)
    _fastuni_vistype = (VisitorType *)QueryRegistry( FAST_UNI_VISITOR_NAME );
  if (!_npflood_vistype)
    _npflood_vistype = (VisitorType *)QueryRegistry( NPFLOOD_VISITOR_NAME );
 
  // Leadership must originate it's nodal IG and get
  // it into the DB before Election starts
  SimEvent * initEvent = new SimEvent(this, this, 0x5365616E);
  Deliver( initEvent, 0.0 );

  // This will allow us to properly re-originate our nodal IG
  _se = new SimEvent(this, this, 0x5365614E);
  Deliver( _se, Database::Default_PTSERefreshInterval );

  assert( _init_lev > 0 );

  AddPermission("*",                    new LeadershipInterface(this));
}

Leadership::~Leadership() 
{ 
  Cancel(_se);
  delete _se;

  dic_item di;
  forall_items(di, _elections)
    delete _elections.inf(di);
  _elections.clear();

  forall_items(di, _seqs)
    delete _seqs.key(di);
  _seqs.clear();
}

void Leadership::Interrupt(SimEvent * e)
{
                      // S e a n
  if (e->GetCode() == 0x5365616E) {
    NodeID * tmp = PhysicalNodeID();
    theNetStatsCollector().ReportNetEvent( "Switch_Up",
					   OwnerName(),
					   0,
					   tmp );
    delete tmp;
    OriginateNodalIG(_init_lev);
    delete e; // Is this okay?
  } else
                      // S e a N
  if (e->GetCode() == 0x5365614E) {
    // Reoriginate it all
    dic_item di;
    NodeID * lastNID = 0;
    
    if (_elections.size() == 1) {
      OriginateNodalIG(_init_lev);
      OriginateNodalHierarchy(_init_lev);
    } else {
      forall_items(di, _elections) {
	LevelInfo * li = _elections.inf(di);
	NodeID * myNID = li->_myNode;
	if (lastNID) // Originate the Nodal IG for the last level 
	  OriginateNodalIG(lastNID->GetLevel(), myNID);
	OriginateNodalHierarchy(myNID->GetLevel());
	lastNID = myNID;
      }
    }

    double refreshInterval = -1.0;
    DatabaseInterface * iface = (DatabaseInterface *)QueryInterface( "Database" );
    assert( iface != 0 && iface->good() );
    iface->Reference();
    refreshInterval = iface->PTSERefreshInterval();
    iface->Unreference();
    assert( refreshInterval > 0.0 );
    Deliver( e, refreshInterval );
  } else                // L E A D
    if (e->GetCode() == 0x4C454144) {
      LeadershipEvent * le = (LeadershipEvent *)e;
      OriginateNodalIG( le->getLevel(), le->getLeader() );

      // The nodal hierarchy list can't be constructed 
      // until the nodal IG is ready from the next level.
      DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
      assert( dbi != 0 && dbi->good() );
      dbi->Reference();

      int iterator = le->getLevel();
      while (iterator > 0 && dbi->LogicalNodeIsMe(iterator)) {
	OriginateNodalHierarchy(iterator);
	iterator = dbi->NextLevelUp( iterator );
      }

      dbi->Unreference();

  } else {
    NodeID * tmp = PhysicalNodeID();
    DIAG("sim.leadership", DIAG_WARNING, cout << 
	 "Leadership " << *tmp << 
	 " received SimEvent with unsupported code '" << e->GetCode() << "'." << endl);
    delete tmp;
  }
}

State * Leadership::Handle(Visitor * v)
{
  if (v->GetType().Is_A(_election_vistype)) {
    // ElectionVisitor
    ElectionVisitor * ev = (ElectionVisitor *)v;
    if (ev->GetPurpose() == ElectionVisitor::ToOriginateANodalinfo) {
      ProcessElectionVisitor(ev);
      v->Suicide();
      v = 0;
    }
  } else
  if (v && v->GetType().Is_A(_fastuni_vistype)) {
    FastUNIVisitor * fuv = (FastUNIVisitor *)v;
    NodeID * tmp = PhysicalNodeID();
    if ((fuv->GetMSGType() == FastUNIVisitor::FastUNISetup) &&
	!(fuv->GetSourceNID()->equals(tmp))) {

      DatabaseInterface * iface = (DatabaseInterface *)QueryInterface( "Database" );
      assert( iface != 0 && iface->good() );
      iface->Reference();

      u_char current_level  = fuv->GetSourceNID()->GetLevel();
      u_char physical_level = iface->PhysicalLevel( );
      int    level          = iface->NextLevelDown( current_level );
      iface->Unreference();

      if ( current_level == physical_level && level == -1 )
	level = 160;

      dic_item di;
      if (di = _elections.lookup(level)) {
	LevelInfo * li = _elections.inf(di);

	if (!li->_iAmLeader) {
	  v->Suicide();
	  v = 0;
	} else {
	  li->_remoteNodes.append(fuv->GetDestNID()->copy());
	  // _elections.change_inf(di, li);
	}
      }
    }
    // otherwise it can pass
    delete tmp;
  } else
  if (v && v->GetType().Is_A(_npflood_vistype)) {
    NPFloodVisitor * npv = (NPFloodVisitor *)v;
    const PTSPPkt * ptsp = npv->GetFloodPTSP();
    const list<ig_ptse *> ptses = ptsp->GetElements();
    list_item li;
    forall_items(li, ptses) {
      ig_ptse * p = ptses.inf( li );
      if ( p->GetType() != InfoGroup::ig_nodal_info_group_id )
	continue;

      list_item nli;
      forall_items(nli, *(p->ShareMembers())) {
	ig_nodal_info_group * nig = (ig_nodal_info_group *)p->ShareMembers()->inf( nli );
	if ( ! nig->IsSet( ig_nodal_info_group::leader_bit ) ||
	     ! nig->GetNextHigherLevel( ) )
	  continue;

	const ig_next_hi_level_binding_info * nhlbi = nig->GetNextHigherLevel( );
	assert( nhlbi != 0 );

	NodeID tmp( nig->GetATMAddress() );
	int level = tmp.GetLevel();

	dic_item di;
	if ( di = _elections.lookup( level ) ) {
	  LevelInfo * li = _elections.inf( di );
	  
	  if ( li->_iAmLeader == false && 
	       li->_pgl == 0 ) {
	    const NodeID * pgl = nhlbi->GetParentLogGroupID();
	    li->_pgl = pgl->copy();
	    OriginateNodalIG( level, pgl->copy() );
	  }
	}
      }
    }
  }

  if (v)
    PassThru(v);

  return this;
}

void Leadership::ProcessElectionVisitor(ElectionVisitor * ev)
{
  dic_item di;
  NodeID * ZeroNID        = 0;
  NodeID * node           = ev->GetNode();                      // which instance of us originated ev
  NodeID * leader         = (NodeID *)ev->GetPGL();             // PGL that ev mentions
  NodeID * leaderUp       = (leader ? OneLevelUp(leader) : 0);  // LGN that ev mentions
  int priority            = ev->GetPriority();

  ElectionVisitor::Action eState = ev->GetAction();   // response of election FSM

  u_char level = (node ? node->GetLevel() : 0);       // level at which the ev originated

  if (eState != ElectionVisitor::PGLE7 &&
      eState != ElectionVisitor::PGLE8) {
    delete leaderUp;
    leaderUp = 0;       // didn't win and didn't lose
  }

  // Find MY information for a given level
  if (di = _elections.lookup(level)) {       // get state at lower level in hierarchy

    // low_info will always be the LevelInfo at the PGLs level
    // up_info will always be  the LevelInfo at the LGNs level
    
    LevelInfo * low_info = _elections.inf(di);     

    DIAG("sim.leadership", DIAG_DEBUG, cout << OwnerName();
	 if (eState == ElectionVisitor::PGLE8) 
	   cout << ": Is elected the rightful heir to the throne." << endl;
	 else
	   cout << ": Sits idly by as the pretenders to the throne squabble over the peer group." << endl;
	 cout << "My Address " << *(low_info->_myNode) << endl
	      << "My sovereign's address ";
	 if (leader) cout << *(leader); else cout << "(Null)"; cout << endl);

    if ((eState == ElectionVisitor::PGLE8) ||
	(eState == ElectionVisitor::PGLE7)) {
      char buf[1024];
      sprintf(buf, "%s: %s at level %d.", 
	      (low_info->_myNode)->Print(),
	      (eState == ElectionVisitor::PGLE8) ? "Is elected PGL" :
	      (eState == ElectionVisitor::PGLE7 ? "Loses the election" :
	       "*Do not print this*"),
	      (int)(low_info->_myNode->GetLevel()));
      AppendCommentToLog(buf);
      DIAG("sim.leadership", DIAG_INFO, cout << buf << endl);
      DIAG("sim.leadership", DIAG_INFO, 
	   if (leaderUp && eState == ElectionVisitor::PGLE7) 
	     cout << "Leader is " << *leaderUp << endl;
	  );
    }

    // Either I won or I didn't
    if (eState == ElectionVisitor::PGLE8) {
      // I knew they'd see it my way ...
      DIAG("sim.leadership", DIAG_DEBUG, 
	   cout << OwnerName() << " Leadership: ";
	   cout << "I am " << *(low_info->_myNode) << endl;
	   cout << "My PGL is ";
	   if (leader) cout << *(leader); 
	   else cout << "(Null)"; cout << endl);

      NodeID * levelUp = OneLevelUp( low_info->_myNode );
 
      // call the NetStatsCollector with type I_am_PGL misc conts Lgn NodeID
      theNetStatsCollector().ReportNetEvent("I_am_PGL",
					    OwnerName(),
					    // my LGN NodeID
					    levelUp != 0 ? levelUp->Print() : low_info->_myNode->Print(),
					    low_info->_myNode); // my PGL NodeID

      LevelInfo * new_li = new LevelInfo(low_info->_myNode, low_info->_myNode, 
					 priority + WINNER_DISTANCE, levelUp);
      new_li->_iAmLeader = true;

      _elections.change_inf(di, new_li);
      delete low_info;

      DBLeadershipInterface * iface = (DBLeadershipInterface *)QueryInterface( "Database" );
      assert( iface != 0 && iface->good() );
      iface->Reference();

      NodeID * myPreferredPeerGroupLeader = 
	levelUp != 0 ? iface->PreferredPeerGroupLeader( levelUp->GetLevel() ) : 0;
      int next_pri = 
	levelUp != 0 ? iface->PeerGroupLeadershipPriority( levelUp->GetLevel() ) : 0;

      if ( levelUp != 0 ) {
	LevelInfo * next_li = 
	  new LevelInfo( levelUp, myPreferredPeerGroupLeader, next_pri, 0);
	// - mountcas - added assert below
	assert( ! _elections.lookup( levelUp->GetLevel() ) );
	_elections.insert( levelUp->GetLevel(), next_li );
      }
      // Originate a new Nodal IG for this level
      OriginateNodalIG(level, levelUp);

      if ( levelUp != 0 ) {
	// Now that I've been declared the rightful ruler of this fair
	// switch I shall instantiate a logical node
	LGNInstantiationVisitor * lgniv = new
	  LGNInstantiationVisitor(levelUp);
	lgniv->SetLoggingOn();
	PassVisitorToA(lgniv);

	// And for the newly formed level
	OriginateNodalIG( levelUp->GetLevel() );
      }
      // The nodal hierarchy list can't be constructed 
      // until the nodal IG is ready from the next level.
      int iterator = level;
      while (iterator > 0 && iface->LogicalNodeIsMe(iterator)) {
	OriginateNodalHierarchy(iterator);
	iterator = iface->NextLevelUp( iterator );
      }

      // Be sure to let the db in on our little secret
      iface->WonElection(level);

      // Make a ComplexRep that will deal with all the NSP issues
      int new_level = iface->NextLevelUp( level );

      iface->Unreference();

      if ( new_level != -1 ) {
	DIAG("sim.leadership.agg", DIAG_INFO, 
	     cout << "AGG -- Leadership: " << *(new_li->_myNode)
	     << " making ComplexRep at level " << new_level << endl);
	
	Make_ComplexRep( new_level );
      }
      // Everyone should've been making copies.
      delete levelUp;
    } else { // I didn't win
      dic_item ddi;

      if (eState ==  ElectionVisitor::PGLE7)
	// call the NetStatsCollector with type Lost_Election misc conts PGL NodeID
	theNetStatsCollector().ReportNetEvent("Lost_Election",
					      OwnerName(),
					      leader ? leader->Print() : 0,
					      low_info->_myNode);

      // Check if we WERE a winner at this level
      DBLeadershipInterface * iface = (DBLeadershipInterface *)QueryInterface( "Database" );
      assert( iface != 0 && iface->good() );
      iface->Reference();
      int new_level = iface->NextLevelUp( level );

      if (ddi = _elections.lookup( new_level )) {

	// - mountcas - added call to inf below and changed all occurances of low_info
	//              to up_info, to prevent ReportNetEvent from failing
	LevelInfo * up_info = _elections.inf( ddi );

	DIAG("sim.leadership", DIAG_INFO, 
	     cout << OwnerName() << " Leadership: ";
	     cout << "I am " << *(low_info->_myNode) << endl; // don't change this to up_info 
	     cout << "My PGL is ";
	     if (leader) cout << *(leader); 
	     else cout << "(Null)"; cout << endl);

	theNetStatsCollector().ReportNetEvent( "Voted_Out",
					       OwnerName(),
					       leader ? leader->Print() : 0,
					       low_info->_myNode);   // don't change this to up_info 

	// Ack! There's been a coup d'e'tat!
	iface->LostElection(level);

	bool topmost_instance = false;  
	// this will become true when the 
	// upward traversal in the rebellion processing 
	// is completed

	// First clean-up after the messy rebellion
	while (ddi) { 
	  assert( ! topmost_instance);
	  
	  // ddi starts at our LGN node
	  LevelInfo * this_info = _elections.inf(ddi);
	  u_char    this_level  = this_info->_myNode->GetLevel();

	  assert( this_level == _elections.key( ddi ) );

	  // This logic has been changed 1/12/99 BK
	  list_item li;
	  // Warn all of the RCC Hellos that we are going down.
	  forall_items(li, this_info->_remoteNodes) {
	    const NodeID * remNode = this_info->_remoteNodes.inf(li);
	    FastUNIVisitor * fuv = 
	      new FastUNIVisitor(this_info->_myNode, remNode, 
				 0, 0, 0, 0, FastUNIVisitor::PGLChanged, 
				 0, 0);
	    delete remNode;
	    PassVisitorToA(fuv);
	  }
	  this_info->_remoteNodes.clear();

	  // Generate an LGNDestructionVisitor for this level
	  LGNDestructionVisitor * lgniv = new
	    LGNDestructionVisitor(this_info->_myNode);
	  lgniv->SetLoggingOn();
	  PassVisitorToA(lgniv);

	  if (this_info->_iAmLeader == false) // we are not leader
	    topmost_instance = true;

	  // Force expiration of bad nodal_ig in Database
	  OustNodalIG(this_level);   
	  OustNodalHierarchy(this_level);

	  // The ComplexReps are relieved of their duties
	  DIAG("sim.leadership.agg", DIAG_INFO, 
	       cout << "AGG -- Leadership: " << *(this_info->_myNode)
	       << " killing ComplexRep at level " << (int)this_level << endl);
	  Kill_ComplexRep(this_level);  

	  // delete our voting record
	  _elections.del_item(ddi);
	  delete this_info;

	  int new_level = iface->NextLevelUp( this_level );
	  ddi = _elections.lookup( new_level );
	};

	assert ( topmost_instance );

      } else {
	// We are, and have always been, merely a loyal subject. Thus
	// we should sing the praises of our most honorable leader to
	// the Database
	iface->LostElection(level);
      }

      // At this point leaderUp is just a guess!
      //    We must put nothing into the next higher level binding info,
      //    and instead wait until the DB notifies us of the new nodal IG.
      //      leaderUp = 0;
      
      // Now express our fealty to the new pretender to the throne
      LevelInfo * new_li = new LevelInfo(low_info->_myNode,
					 (eState == ElectionVisitor::PGLE5 ? ZeroNID : leader),
					 low_info->_priority,
					 (eState == ElectionVisitor::PGLE7 ? leaderUp : 0));
      _elections.change_inf(di, new_li);
      delete low_info;

      const u_char * lhs = new_li->_myNode->GetNID() + 2;
      const u_char * rhs = leaderUp != 0 ? leaderUp->GetNID() + 2 : 0;
      // If we are in this block we must NOT have won
      assert( ( rhs == 0 ) || memcmp( lhs, rhs, 20 ) );
      OriginateNodalIG(level, leaderUp);

      int iterator = level;
      while (iterator && iface->LogicalNodeIsMe(iterator)) {
	OriginateNodalHierarchy(iterator);
	iterator = iface->NextLevelUp( iterator );
      }

      iface->Unreference();    
    }
  } else {
    DIAG("sim.leadership", DIAG_FATAL, cout << "FATAL: " 
	 << "Leadership " << *PhysicalNodeID() 
	 << " has no information for level " << (int)level 
	 << " and should NOT be involved in this election." << endl);
  }
  delete ZeroNID;
  delete leaderUp;
}

NodeID * Leadership::OneLevelDown(const NodeID * nid)
{
  DatabaseInterface * iface = (DatabaseInterface *)QueryInterface( "Database" );
  assert( iface != 0 && iface->good() );
  iface->Reference();

  int new_level      = nid->GetChildLevel();
  int child_level    = iface->NextLevelDown( new_level );
  if ( new_level == iface->PhysicalLevel( ) && child_level == -1 )
    child_level = 160;
  NodeID * levelDown = new NodeID( new_level, child_level, nid );
  iface->Unreference();

  return levelDown;
}

NodeID * Leadership::OneLevelUp(const NodeID * nid)
{
  DatabaseInterface * iface = (DatabaseInterface *)QueryInterface( "Database" );
  assert( iface != 0 && iface->good() );
  iface->Reference();

  int old_level    = nid->GetLevel();
  int new_level    = iface->NextLevelUp( old_level );

  NodeID * levelUp = 0;
  if ( new_level != -1 )
    levelUp = new NodeID( new_level, old_level, nid );
  iface->Unreference();

  return levelUp;
}

void Leadership::ASyncNodalIGOrig(u_char level, NodeID * leader)
{
  LeadershipEvent * le = new LeadershipEvent(this, level, leader);
  Deliver( le );
}

// 0 is always used as the id, this method also Inserts it into the DB
void Leadership::OriginateNodalIG(u_char level, NodeID * leader)
{
  NodeID * tmpnid = 0;
  dic_item di;
  if (!_originating && (di = _elections.lookup(level))) {
    _originating = true;

    LevelInfo * li = _elections.inf(di);
    NodeID * myNID = li->_myNode->copy();
    dic_item ddi   = _seqs.lookup(myNID);
    Addr * myAddr = myNID->GetAddr();

    if (!ddi)
      ddi = _seqs.insert(myNID, 2);
    else
      delete myNID;
      
    int bits = 0;

    if (li->_iAmLeader)
      bits = 0x80;

    ig_nodal_info_group * new_nig = 
      new ig_nodal_info_group(myAddr, li->_priority, 
			      bits, li->_prefPGL);

    if (leader != 0) {
      assert( leader->GetChildLevel() == _seqs.key( ddi )->GetLevel() &&
	      leader->GetLevel() < _seqs.key( ddi )->GetLevel() );

      PeerID * tmppeer    = leader->GetPeerGroup();
      Addr   * leaderAddr = leader->GetAddr();
      new_nig->SetNextHigherLevel(new ig_next_hi_level_binding_info(leader, 
								    leaderAddr, 
								    tmppeer, 0));
      delete leaderAddr;
      delete tmppeer;
    }
    DBLeadershipInterface * iface = (DBLeadershipInterface *)QueryInterface( "Database" );
    assert( iface != 0 && iface->good() );
    iface->Reference();
    ig_ptse * ptse = new ig_ptse(InfoGroup::ig_nodal_info_group_id, 
				 NODAL_IG_ID, _seqs.inf(ddi), 0,
				 iface->PTSELifetime());
    iface->Unreference();
    _seqs.change_inf(ddi, _seqs.inf(ddi) + 2);
    ptse->AddIG(new_nig);
    // Insert it into the Database
    Insert2DB(ptse, level);

    NodeID * tmp = PhysicalNodeID();

    DIAG("sim.leadership", DIAG_DEBUG,   cout << tmp->Print()
	 << ": (re)originated my nodal ig at level " 
	 << (int)level << endl << " for " << li->_myNode->Print() << endl );

    delete myAddr;  // Added 3/6/98 5:35am mountcas - as per Insra's instructions
    _originating = false;
  }
  // else, dunno what the caller was thinking ...
}

void Leadership::OriginateNodalHierarchy(u_char level)
{
  dic_item di;
  // See if we have the election information the specified level
  if (!_originating && (di = _elections.lookup(level))) {
    _originating = true;

    LevelInfo * li = _elections.inf(di);
    NodeID * myNID = li->_myNode->copy(), * nid = li->_myNode->copy();
    dic_item ddi   = _seqs.lookup(myNID);

    if (!ddi) // Start at sequence number 2
      ddi = _seqs.insert(myNID, 2);
    else
      delete myNID;

    // Build a new nodal hierarchy list
    ig_nodal_hierarchy_list * new_nhl = 
      new ig_nodal_hierarchy_list(_seqs.inf(ddi));

    bool done = false;
    // Iterate over all of the nodal information groups from this level upward
    // Adding my parents to my nodal hierarchy list
    while (!done) {
      // The first level in the NHL should be our immediate parent
      ig_nodal_info_group * nig = 0;

      DBLeadershipInterface * iface = (DBLeadershipInterface *)QueryInterface( "Database" );
      assert( iface != 0 && iface->good() );
      iface->Reference();

      if (nig = iface->GetNodalIG(nid)) {
	ig_next_hi_level_binding_info * nhlbi = 
	  (ig_next_hi_level_binding_info *)nig->GetNextHigherLevel();

	if (!nhlbi) 
	  done = true;
	else {
	  NodeID * parent = (NodeID *)nhlbi->GetParentLogGroupID();
	  Addr   * p_addr = (Addr *)parent->GetAddr();
	  PeerID * p_peer = (PeerID *)nhlbi->GetParentPeerGroupID();
	  // Add the next higher level parent to the NHL
	  new_nhl->AddLevel(parent, p_addr, p_peer);
	  // Clean up
	  delete nid; delete p_addr; delete p_peer; 
	  // setup for the next loop
	  nid = parent;
	}
      } else
	done = true;

      iface->Unreference();
    }
    delete nid;

    DBLeadershipInterface * iface = (DBLeadershipInterface *)QueryInterface( "Database" );
    assert( iface != 0 && iface->good() );
    iface->Reference();

    ig_ptse * ptse = new ig_ptse(InfoGroup::ig_nodal_hierarchy_list_id, 
				 NODAL_HIERARCHY_ID, _seqs.inf(ddi), 0, 
				 iface->PTSELifetime());
    iface->Unreference();
    _seqs.change_inf(ddi, _seqs.inf(ddi) + 2);
    ptse->AddIG(new_nhl);
    // Insert it into the Database
    Insert2DB(ptse, level);

    DIAG("sim.leadership", DIAG_DEBUG, cout << OwnerName() << ": " << endl
	 << "(re)originated my nodal hierarchy list at level " << (int)level 
	 << endl);
    _originating = false;
  }
  // else, dunno what the caller was thinking ...
}

void Leadership::Insert2DB(ig_ptse * ptse, u_char level)
{
  NodeID * phys   = PhysicalNodeID();
  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface("Database");
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  int child_level = dbi->NextLevelDown( level );
  dbi->Unreference();
  if ( child_level == -1 )
    child_level = 160;
  NodeID * myNID  = new NodeID( level, child_level, phys );
  PeerID * myPID  = myNID->GetPeerGroup();

  PTSPPkt * ptsp  = new PTSPPkt(myNID->GetNID(), 
				myPID->GetPGID());
  ptsp->AddPTSE(ptse);
  // Create the damn packet, floodVisitor, et al
  NPFloodVisitor * ptspv = new NPFloodVisitor(ptsp, true, myNID, myNID);
  PassVisitorToB(ptspv);
  delete phys;
  delete myNID;
  delete myPID;
}

// Prematurely expire the nodal IG at this level
void Leadership::OustNodalIG(u_char level)
{
  dic_item di;
  if (di = _elections.lookup(level)) {
    LevelInfo * li    = _elections.inf(di);
    NodeID    * myNID = li->_myNode->copy();

    dic_item ddi = _seqs.lookup(myNID);
    assert(ddi);

    DBLeadershipInterface * iface = (DBLeadershipInterface *)QueryInterface( "Database" );
    assert( iface != 0 && iface->good() );
    iface->Reference();

    ig_ptse * fake_ptse = new ig_ptse(InfoGroup::ig_nodal_info_group_id, 
				      NODAL_IG_ID, _seqs.inf(ddi), 0, 
				      iface->PTSELifetime());
    iface->Expire( iface->ReqPTSE(myNID, fake_ptse) );
    iface->Unreference();

    delete myNID;
  }
}

// Prematurely expire the nodal hierarchy list at this level
void Leadership::OustNodalHierarchy(u_char level)
{
  dic_item di;
  if (di = _elections.lookup(level)) {
    LevelInfo * li    = _elections.inf(di);
    NodeID    * myNID = li->_myNode->copy();

    dic_item ddi   = _seqs.lookup(myNID);
    assert(ddi);

    DBLeadershipInterface * iface = (DBLeadershipInterface *)QueryInterface( "Database" );
    assert( iface != 0 && iface->good() );
    iface->Reference();

    ig_ptse * fake_ptse = new ig_ptse(InfoGroup::ig_nodal_hierarchy_list_id, 
				      NODAL_HIERARCHY_ID, _seqs.inf(ddi), 0, 
				      iface->PTSELifetime());

    iface->Expire( iface->ReqPTSE(myNID, fake_ptse) );
    iface->Unreference();
    delete myNID;
  }
}

void Leadership::Print(ostream & os)
{
  dic_item di;

  os << "Election Information:" << endl;
  forall_items(di, _elections) {
    LevelInfo * li = _elections.inf(di);
    os << *(li);
  }
  os << "Total: " << _elections.size() << endl;
}

// ----------------------------------------------------------------------------
Leadership::LevelInfo::LevelInfo(const NodeID * myNode, 
				 const NodeID * myPreferredPeerGroupLeader, 
				 int priority, 
				 const NodeID * myPeerGroupLeader) :
  _myNode(0), _prefPGL(0), _pgl(0), _priority(priority), _iAmLeader(false)
{
  if (myNode)                     _myNode = new NodeID(*myNode);
  if (myPreferredPeerGroupLeader) _prefPGL = new NodeID(*myPreferredPeerGroupLeader);
  if (myPeerGroupLeader)          _pgl = new NodeID(*myPeerGroupLeader);
}

Leadership::LevelInfo::LevelInfo(const Leadership::LevelInfo & rhs) :
  _myNode(0), _prefPGL(0), _pgl(0), _priority(rhs._priority), 
  _iAmLeader(rhs._iAmLeader)
{
  if (rhs._myNode)  _myNode = new NodeID (*(rhs._myNode));
  if (rhs._prefPGL) _prefPGL = new NodeID (*(rhs._prefPGL));
  if (rhs._pgl)     _pgl = new NodeID (*(rhs._pgl));
}

// assignment operator in case you couldn't tell due to all the scoping crap
Leadership::LevelInfo & Leadership::LevelInfo::operator = (const Leadership::LevelInfo & rhs)
{
  _priority  = rhs._priority;
  _iAmLeader = rhs._iAmLeader;

  if (rhs._myNode) {
    if (_myNode) delete _myNode;
    _myNode = new NodeID (*(rhs._myNode));
  }
  if (rhs._prefPGL) {
    if (_prefPGL) delete _prefPGL;
    _prefPGL = new NodeID (*(rhs._prefPGL));
  }
  if (rhs._pgl) {
    if (_pgl) delete _pgl;
    _pgl = new NodeID (*(rhs._pgl));
  }

  return *this;
}

Leadership::LevelInfo::~LevelInfo( )
{
  delete _myNode;
  delete _prefPGL;
  delete _pgl;
}

int operator == (const Leadership::LevelInfo & lhs, const Leadership::LevelInfo & rhs)
{
  int rval = compare(lhs._myNode, rhs._myNode);
  if (!rval)
    rval = compare(lhs._prefPGL, rhs._prefPGL);
  if (!rval)
    rval = compare(lhs._pgl, rhs._pgl);
  return rval;
}

ostream & operator << (ostream & os, const Leadership::LevelInfo & li)
{
  if (li._myNode)
    os << "My NodeID: " << *(li._myNode) << endl;
  else
    os << "My NodeID: (Null)" << endl;
  if (li._pgl)
    os << "My PGL: " << *(li._pgl) << endl;
  else
    os << "My PGL: (Null)" << endl;
  os << "Priority " << li._priority << endl;
  return os;
}

NodeID * Leadership::PGLAtLevel(const u_char level) const
{
  NodeID * rval = 0;

  dic_item di = _elections.lookup(level);
  assert(di != 0);
  LevelInfo * info = _elections.inf(di);
  assert(info != 0);
  if (info->_pgl) // we may not have elected anyone yet
    rval = info->_pgl->copy();
  return rval;
}

NodeID * Leadership::PhysicalNodeID(void) const
{
  NodeID * rval = 0;
  
  dic_item di;
  if (di = _elections.lookup( _init_lev )) {
    LevelInfo * li = _elections.inf(di);
    rval = (li->_myNode)->copy();
  }
  return rval;
}

// This is called from the callback method of ComplexRep

void Leadership::GenerateComplexRep(u_char level,
				    list<ig_nodal_state_params*> * nsplist)
{     
  assert( nsplist != 0 );

  DBLeadershipInterface * iface = (DBLeadershipInterface *)QueryInterface( "Database" );
  assert( iface != 0 && iface->good() );
  iface->Reference();

  NodeID * phys = PhysicalNodeID();
  int child_level = iface->NextLevelDown( level );
  if ( child_level == -1 )
    child_level = 160;
  NodeID * lgn  = new NodeID( level, child_level, phys );
  delete phys;

  DIAG("sim.leadership.agg", DIAG_DEBUG,
       cout << "At time " << theKernel().CurrentElapsedTime() << endl
            << "\tAGG -- Leadership: " << *lgn << endl
            << "\tis being asked to update ComplexRep at level " << (int)level << endl;);
  delete lgn;

  ig_ptse * ptse = new ig_ptse(InfoGroup::ig_nodal_state_params_id,
			       0x800, _nsp_seqnum++, 0, iface->PTSELifetime());
  iface->Unreference();
  list_item li;
  forall_items(li,*nsplist) {
    ig_nodal_state_params* nsp = nsplist->inf(li);
    ptse->AddIG(nsp);
  }
  // Insert it into the Database
  Insert2DB(ptse, level);

  iface->Unreference();
}


// level is the level of the LGN for which the complexrep is made
void Leadership::Make_ComplexRep(u_char level) 
{
  NodeID * phys = PhysicalNodeID();
  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  int child_level = dbi->NextLevelDown( level );
  if ( child_level == -1 )
    child_level = 160;
  NodeID * lphn = new NodeID( level, child_level, phys );
  dbi->Unreference();
  NodeID * pgl  = PGLAtLevel(level);

  AggLeadershipInterface * agi = (AggLeadershipInterface *)QueryInterface( "Aggregator" );
  assert( agi != 0 && agi->good() );
  agi->Reference();
  Aggregator * aggr = agi->ObtainAggregator();
  agi->Unreference();

  LogosLeadershipInterface * lgi = (LogosLeadershipInterface *)QueryInterface( "Logos" );
  assert( lgi != 0 && lgi->good() );
  lgi->Reference();
  Logos * logos = lgi->ObtainLogos();
  lgi->Unreference();

  ComplexRep * cr = new ComplexRep(lphn, pgl, 
                                   aggr, this, logos);
  dic_item di;
  NodeID * lgn = cr->Get_LGN();
  assert(! (di = _complexReps.lookup( lgn )));
  _complexReps.insert( lgn, cr );

  cr->Callback();  // force first aggregation

  delete phys;
  delete lphn;
  delete pgl;
}


// level is the level of the LGN we lost elections at
// and for which the complexrep is now to be destroyed
void Leadership::Kill_ComplexRep(u_char level) 
{
  NodeID * phys = PhysicalNodeID();
  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface("Database");
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  int child_level = dbi->NextLevelDown( level );
  if ( child_level == -1 )
    child_level = 160;
  NodeID * lgn  = new NodeID( level, child_level, phys );
  dbi->Unreference();

  dic_item di;
  assert(di = _complexReps.lookup(lgn));
  ComplexRep * cr = _complexReps.inf(di);
  delete _complexReps.key(di);
  _complexReps.del_item(di);

  delete cr;   // kaboom!
  delete phys;
  delete lgn;
}

void Leadership::Notify( const ig_nodal_info_group * ig )
{
  assert( ig->GetNextHigherLevel() != 0 );
  const ig_next_hi_level_binding_info * nhlbi = ig->GetNextHigherLevel();

  NodeID tmp( ig->GetATMAddress() );
  int level = tmp.GetLevel();
  dic_item di;
  if ( di = _elections.lookup( level ) ) {
    LevelInfo * li = _elections.inf( di );
    assert( li->_iAmLeader == false );
    if ( li->_pgl == 0 )
      OriginateNodalIG( level, (NodeID *)nhlbi->GetParentLogGroupID() );
  }
}
