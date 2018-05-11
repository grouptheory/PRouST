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
static char const _Omni_cc_rcsid_[] =
"$Id: Omni.cc,v 1.26 1999/02/19 21:22:46 marsh Exp $";
#endif

#include <common/cprototypes.h>

#include "Omni.h"
#include <DS/util/String.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/DBKey.h>
#include <codec/pnni_pkt/ptsp.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/next_hi_level_binding_info.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <FW/basics/diag.h>
#include <FW/basics/Conduit.h>

#include <iostream.h>
#include <fstream.h>
#include <fsm/database/Database.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/netstats/NetStatsCollector.h>
#include <strings.h>

#define __BRUTE_FORCE_TESTING__ 1

Omni* Omni::_singleton = 0;

//--------------------------------------------------------
Omni & theOmni(void) 
{
  if (!Omni::_singleton) {
    Omni::_singleton = new Omni;
  }
  return *(Omni::_singleton);
}

//--------------------------------------------------------
Omni::pg_convergence_status_change
  Omni::LocalConvergenceTest(const NodeID * sw, const NodeID *& pgl)
{
  // do you know what you're doing?
  // passing in a pointer that is
  // already in use.  It will leak!
  assert(!pgl);

  dic_item di = _subject2leader.lookup(sw);
  if (!di) {
    pgl = 0;
    return Omni::unchanged;
  }

  NodeID* leader = _subject2leader.inf(di);
  const u_char level = leader->GetChildLevel();
  if (level==0) {  // undecided sentinel
    return Omni::unchanged;
  }
  
  Omni::pg_convergence_status old_state = Omni::not_converged;
  dic_item statit = _PGL2status.lookup(leader);
  if (statit) {
    old_state = (Omni::pg_convergence_status) (_PGL2status.inf(statit));
  }
  
  bool convergence_status = InSynch(leader);
  if (statit) {
    _PGL2status.change_inf(statit, convergence_status);
  }

  
  // this is who we are referring to in the return value
  pgl = leader->copy();

  Omni::pg_convergence_status_change occurred;

  if ((convergence_status) && (old_state==Omni::not_converged)) {
    occurred = Omni::changed_to_converged;
  }
  else if (( ! convergence_status) && (old_state==Omni::converged)) {
    occurred = Omni::changed_to_not_converged;
  }
  else 
    occurred = Omni::unchanged;

  return occurred;
}

//--------------------------------------------------------
bool Omni::FullConvergenceTest( int & num_nodes ) const
{
  // clear the certificate
  strcpy(_certificate, "");

  bool global_convergence = true;

  int num_logicals = 0;

  dic_item iter;

  // who's on top?
  u_char toplevel = 160;
  forall_items(iter, _leader2subjects) {
    const NodeID * leader = _leader2subjects.key(iter);
    const u_char level = leader->GetLevel();
    if ((level != 0) && (level < toplevel))
      toplevel = level;
  }

  int num_at_top  = 0;
  NodeID * topdog = 0;
  forall_items(iter, _leader2subjects) {
    const NodeID * leader = _leader2subjects.key(iter);
    const u_char level = leader->GetLevel();
    if (level == toplevel) {
      topdog = (NodeID*) leader;
      num_at_top++;
      if (num_at_top > 1) { // still need to reach the hierarchy apex
	num_nodes = 0;
	if ( _certify )
	  strcat(_certificate, "The network does not yet have a unique hierarchy apex:\n");
	return false;
      }
    }
  }

  if (num_at_top == 0) { // nothing going on
    if ( _certify )
      strcat(_certificate, "No leaders are present.\n");
    return false;
  }

  int ctnodes=0;

  forall_items(iter, _leader2subjects) {
    const NodeID * leader = _leader2subjects.key(iter);
    list<NodeID *> * subjects = _leader2subjects.inf(iter);

    ctnodes += subjects->size();

    const u_char level = leader->GetChildLevel();
    if (level == 0) {  
      if (subjects->size() > 1) {
	if ( _certify ) {
	  sprintf(_entry, "There are %d (more than one) undecided voters, see list below\n", subjects->size());
	  strcat(_certificate, _entry);
	}
	list_item li;
	forall_items(li, *subjects) {
	  NodeID* undecided = subjects->inf(li);
	  sprintf(_entry,"%s\n",undecided->Print());
	  strcat(_certificate, _entry);
	}
	return false;
      }
      if ((subjects->size() == 1) &&
	  ( ! (subjects->inf(subjects->first()))->equals( topdog )) ) { 
	if ( _certify ) {
	  sprintf(_entry, 
		  "There is an undecided voter: %s\n"
		  "who is not the apex node ( %s ).\n",
		  subjects->inf(subjects->first())->Print(),
		  topdog->Print());
	  strcat(_certificate, _entry);
	}
	return false;
      }
      
      continue;
    }
    num_logicals++;

    bool converged = InSynch( (NodeID*) leader );

    if (converged) {
      DIAG("omni",DIAG_INFO,
	   cout << "omni: All " << subjects->size() 
	        << " subjects of leader " << *leader 
	        << " are converged" << endl;);
    } else {
      if (_certify) {
	// we should assert( *_certificate );
	sprintf(_entry, 
		"the evidence above shows non-convergence of the subjects of PGL\n%s\n",
		leader->Print());
	strcat(_certificate, _entry);
      }
      DIAG("omni",DIAG_INFO,
	   cout << "omni: NOT all " << subjects->size() 
	   << " subjects of leader " << *leader 
	   << " are converged" << endl;);
      global_convergence = false;
    }
  }

  if (num_logicals == 0) {
    global_convergence = false;
    DIAG("omni",DIAG_INFO,
	 cout << "omni: Too early to test for global convergence." << endl;);
    num_nodes = 0;
    if (_certify) 
      strcat(_certificate, "There are no logical nodes present.\n");
    return false;
  } else {
    if (global_convergence) {
      DIAG("omni",DIAG_INFO,
	   cout << "omni: Global convergence attained." << endl;);

      num_nodes = ctnodes;

      // Check for tautological convergence
      if (ctnodes == 0) {
	if (_certify)
	  strcat(_certificate, "There are no subjects, the tautological convergence is ignored.\n");
	return false;
      }
    } else {
      DIAG("omni",DIAG_INFO,
	   cout << "omni: NO global convergence yet." << endl;);
      num_nodes=0;
    }
  }

  if (_certify) {
    if ( ! global_convergence) {
      sprintf(_entry,"Hence, the network is not converged.\n\n");
      strcat(_certificate, _entry);
    } else {
      sprintf(_entry,"The network is converged!\n\n");
      strcat(_certificate, _entry);
    }
  }
    
  return global_convergence;
}
  
//--------------------------------------------------------
bool Omni::InSynch(NodeID * node1, NodeID * node2) const
{
  dic_item di1, di2;

  di1 = _census.lookup(node1);
  di2 = _census.lookup(node2);

  if (!di1) {
    if (_certify) {
      sprintf(_entry,
	      "!!! Synchrony violation: node\n%s\nis nonexistent\n",node1->Print());      
      strcat(_certificate, _entry);
    }
  }

  if (!di2) {
    if (_certify) {
      sprintf(_entry,
	      "!!! Synchrony violation: node\n%s\nis nonexistent\n",node2->Print());     
      strcat(_certificate, _entry);
    }
  }

  if ((!di1) || (!di2))
    return false;

  dictionary<DBKey*, int>* little_black_book1 = _census.inf(di1);
  dictionary<DBKey*, int>* little_black_book2 = _census.inf(di2);

  if (little_black_book1->size() != little_black_book2->size()) {
    if (_certify) {
      sprintf(_entry,
	      "!!! Synchrony violation: node\n%s\n"
	      "has %d items in its database, but node\n%s\n"
	      "has %d items in its database.\n",
	      node1->Print(),
	      little_black_book1->size(),
	      node2->Print(),
	      little_black_book2->size());
      strcat(_certificate, _entry);
    }
    return false;
  }

  dic_item iter;
  forall_items(iter, *little_black_book1) {
    DBKey* key = little_black_book1->key(iter);
    int seqnum1 = little_black_book1->inf(iter);

    dic_item srch = little_black_book2->lookup(key);
    if ( ! srch) {

      if (_certify) {
	sprintf(_entry,"!!! Synchrony violation: node\n%s HAS the dbkey:\n",
		node1->Print());
	strcat(_certificate, _entry);
      
	key->Print(_entry);
	strcat(_certificate, _entry);
	
	sprintf(_entry, "but node %s\nDOES NOT HAVE the above dbkey.\n",
		node2->Print());
	strcat(_certificate, _entry);
      }
      return false;
    }
    int seqnum2 = little_black_book2->inf(srch);

    if (seqnum1 != seqnum2) {
      if (_certify) {
	sprintf( _entry, "!!! Synchrony violation: node\n%s\nhas the dbkey:\n",
		node1->Print());
	strcat(_certificate, _entry);
      
	key->Print(_entry);
	strcat(_certificate, _entry);

	sprintf(_entry," with sequence number %d\n"
		"but node %s\nhas the above dbkey with sequence number %d",
		seqnum1, node2->Print(), seqnum2);
	strcat(_certificate, _entry);
      }
      return false;
    }
  }

  // reverso-faro shuffle
  forall_items(iter, *little_black_book2) {
    DBKey* key = little_black_book2->key(iter);
    int seqnum1 = little_black_book2->inf(iter);

    dic_item srch = little_black_book1->lookup(key);
    if ( ! srch) {

      if (_certify) {
	sprintf(_entry,"!!! Synchrony violation: node\n%s HAS the dbkey:\n",
		node2->Print());
	strcat(_certificate, _entry);
	
	key->Print(_entry);
	strcat(_certificate, _entry);

	sprintf(_entry,"but node %s\nDOES NOT HAVE the above dbkey.\n",
		node1->Print());
	strcat(_certificate, _entry);
      }
      return false;
    }

    int seqnum2 = little_black_book1->inf(srch);

    if (seqnum1 != seqnum2) {

      if (_certify) {
	sprintf(_entry,"!!! Synchrony violation: node\n%s\nhas the dbkey:\n",
		node2->Print());
	strcat(_certificate, _entry);
      
	key->Print(_entry);
	strcat(_certificate, _entry);

	sprintf(_entry, " with sequence number %d\n"
		"but node %s\nhas the above dbkey with sequence number %d",
		seqnum1, node1->Print(), seqnum2);
	strcat(_certificate, _entry);
      }
      return false;
    }
  }

  return true;
}

//--------------------------------------------------------
bool Omni::InSynch(NodeID* leader) const
{
  dic_item di = _leader2subjects.lookup(leader);
  assert(di);

  list<NodeID*>* subjects = _leader2subjects.inf(di);
  
  list_item li;
  bool converged = true;
  NodeID* subject_name, *prev_name = 0;

  bool virgin = true;
  forall_items(li, *subjects) {
    subject_name = subjects->inf(li);

    if ( (prev_name) && ( ! InSynch(subject_name, prev_name))) {

      if (_certify) {
	sprintf(_entry,"The evidence above shows that the following nodes did not agree:\n%s\n%s\n",
		subject_name->Print(),
		prev_name->Print());
	strcat(_certificate, _entry);
      }

      converged = false;
      if ( ! _certify ) {
	break;
      }
    }
    prev_name = subject_name;
  }

  return converged;
}
  
//--------------------------------------------------------
bool Omni::Report_PTSE(NodeID* citizen, DBKey* key) {
  dic_item di;
  dictionary<DBKey*, int>* little_black_book;
  if (di = _census.lookup(citizen)) {
    little_black_book = _census.inf(di);
    dic_item it = little_black_book->lookup(key);
    if (it) {
      DBKey* old_key = little_black_book->key(it);
      little_black_book->del_item(it);
      delete old_key;
      little_black_book->insert(key, key->GetSN());
    }
    else
      little_black_book->insert(key, key->GetSN());
  }
  else { // welcome new citizen
    little_black_book = new dictionary<DBKey*, int>;
    _census.insert(citizen->copy(), little_black_book);
    little_black_book->insert(key, key->GetSN());
  }

  sniffer1.AttemptDetection( citizen, key );
  sniffer2.AttemptDetection( citizen, key );
  sniffer3.AttemptDetection( citizen, key );
  sniffer4.AttemptDetection( citizen, key );
  
  return true;
}

//--------------------------------------------------------
void Omni::Leadership_Notice(const NodeID * Originator, NodeID * newleader) {

  DIAG("omni",DIAG_INFO,
       cout << "omni: Switch " << *Originator << " pledges to " 
            << *newleader << endl;);

  // --- update subject->leader dictionary
  dic_item di = _subject2leader.lookup(Originator);
  NodeID * old_leader = 0;
  if (di != 0) {
    old_leader = _subject2leader.inf(di);
    _subject2leader.change_inf(di,newleader);
  } else
    _subject2leader.insert(Originator->copy(), newleader);

  list<NodeID*>* subjects = 0;
  NodeID* old_entry = 0;
  // --- remove entry from old leaders list of subjects
  if (old_leader) {
    di = _leader2subjects.lookup(old_leader);
    assert(di);
    subjects = _leader2subjects.inf(di);
    list_item li = subjects->search( (NodeID*)Originator);
    assert(li);
    old_entry = subjects->inf(li);
    subjects->del_item(li);
    
    if (subjects->empty()) {
      // for who can say that they are la eader
      // if they have no followers?
      _leader2subjects.del_item(di);
      
      dic_item statit = _PGL2status.lookup(old_leader);
      assert(statit);
      const NodeID* oldid = _PGL2status.key(statit);
      delete oldid;
      _PGL2status.del_item(statit);
    }
  }

  // --- add entry to new leaders list of subjects
  if (di = _leader2subjects.lookup(newleader)) 
    subjects = _leader2subjects.inf(di);
  else {
    subjects = new list<NodeID *>;

    _leader2subjects.insert(newleader->copy(),subjects);
    int val = (int) Omni::not_converged;
    _PGL2status.insert(newleader->copy(), val);
  }
  if ( ! subjects->search( (NodeID*)Originator)) {
    subjects->append(Originator->copy());
  }

  // --- clean up
  delete old_leader;
  delete old_entry;
}

//--------------------------------------------------------
Omni::Omni(void) : _globalSynchrony( false ), _certify( false ) 
{
  if (_singleton) {
    cout << "omni: Fie! There can be but --ONE-- Omni!" << endl;
    abort();
  }
  _singleton = this;

  _certificate = new char[ CERTIFICATE_SIZE ];
  _entry = new char[ ENTRY_SIZE ];

  strcpy(_certificate,"");

  _omni_node_id = new NodeID( OMNI_NODE_ID );
  ds_String key("Omni");
  _omni_database = new Database( &key, _omni_node_id );
  _omni_database_conduit = new Conduit( "omni_db", _omni_database);
}

//--------------------------------------------------------
Omni::~Omni() {
  if (_singleton == 0) {
    cout << "omni: Fie! There can be but --ONE-- Omni!" << endl;
    abort();
  }
  _singleton = 0;

  delete [] _certificate;
  delete [] _entry;
  _certificate = 0;
  _entry = 0;

  delete _omni_database_conduit;
  _omni_database_conduit = 0;
  _omni_database=0;
}

//--------------------------------------------------------
void Omni::DumpElectionTree(const char * fname) 
{
  ostream * out = 0;

  if (!fname || !*fname)
    out = &cout;
  else
    out = new ofstream(fname);

  bool global_convergence = true;

  dic_item iter;
  int num_logicals = 0;

  cout << "omni: ---------- Election Tree ---------" << endl;
  forall_items(iter, _leader2subjects) {

    const NodeID * leader = _leader2subjects.key(iter);

    // do not delete leader_str, it's the gut of the nodeid
    const char * leader_str = leader->Print();
    list<NodeID *> * subjects = _leader2subjects.inf(iter);

    cout << "omni: Leader = " << *leader << endl;

    NodeID * pgl = 0;
    pg_convergence_status_change conv = LocalConvergenceTest( subjects->head(), pgl );
    if (pgl) // undecided sentinel
      assert( pgl->equals( leader ) );

    *out << leader_str << endl << subjects->size() << endl
	 << ( conv == changed_to_converged ? "converged" :
	      conv == changed_to_not_converged ? "not converged" : "unchanged" )
	 << endl;

    list_item li;
    NodeID * subject_name;
    
    cout << "omni: Has Subjects:" << endl;

    forall_items(li, *subjects) {
      subject_name = subjects->inf(li);

      // do not delete subject_name_str, it's the gut of the nodeid
      // const char * subject_name_str = subject_name->Print();

      cout << "omni: --> " << *subject_name << endl;

      // *out << subject_name_str << endl;
    }
    cout << "omni: ---------------------------------" << endl;
    *out << endl;
  }

  if (fname && *fname)
    delete out; // the dtor will close the file
}


//--------------------------------------------------------
void Omni::UpdateOmniDB(NPFloodVisitor * npv) 
{
  ProcessNPFlood( npv );
  _omni_database_conduit->Accept( npv );
}

void Omni::ProcessNPFlood( NPFloodVisitor * npv )
{
  const PTSPPkt * fptsp = npv->GetFloodPTSP();
  Process_PTSP(fptsp);
}

//----------------------------------------
void Omni::Process_PTSP(const PTSPPkt * ptsp)
{
  assert(ptsp != 0);

  // loop through ptsp->_elements (all of which are ig_ptse *),
  // storing them.
  const list<ig_ptse *> & ptse_list = ptsp->GetElements();

  NodeID nid(ptsp->GetOID());
  PeerID pid(ptsp->GetPGID());
  
  list_item li;
  forall_items(li, ptse_list) {
    ig_ptse * tmp = (ptse_list.inf(li));

    if ( tmp->GetType() == InfoGroup::ig_nodal_info_group_id )
      Process_NodalIG(&nid, tmp);
  }
}

void Omni::Process_NodalIG(const NodeID * Originator, ig_ptse * ptse)
{
  const list<InfoGroup *> * igs = ptse->ShareMembers();

  // if someone is expiring this PTSE...
  if (( ! igs ) ||
      ( igs->size() == 0 )) {
    NodeID * undecided = 
      new NodeID("00:000:0000000000000000000000000000000000000000");
    Leadership_Notice( Originator, undecided );
    return;
  }

  // otherwise, there are IGs...
  int ct = 0;
  list_item li;
  forall_items(li, *igs) {
    ig_nodal_info_group* nig = (ig_nodal_info_group*) (igs->inf(li));
    const ig_next_hi_level_binding_info * nhlb = nig->GetNextHigherLevel();
    if (nhlb) {
      NodeID * lgn = (NodeID*) (nhlb->GetParentLogGroupID());
      assert(lgn);
      Leadership_Notice( Originator, lgn );
      ct++;
    } else {
      NodeID * undecided = 
	new NodeID("00:000:0000000000000000000000000000000000000000");
      Leadership_Notice( Originator, undecided );
      ct++;
    }
  }
  assert( ct <= 1 );
}

//--------------------------------------------------------
void Omni::setCertificationMode(bool val) 
{
  _certify = val;
}

//--------------------------------------------------------
const char* Omni::getCertificate(void) const 
{
  return _certificate;
}

//--------------------------------------------------------
bool Omni::isConverged(void) const
{
  return _globalSynchrony;
}

//--------------------------------------------------------
void Omni::RecomputeGlobalSynchrony( NodeID* citizen ) {
  NodeID * leader = 0;
  Omni::pg_convergence_status_change result =
    theOmni().LocalConvergenceTest(citizen, leader);

  if (result == Omni::changed_to_converged) {
    assert(leader);
    DIAG("omni",DIAG_INFO,
	 cout << "omni: **** PG of " << *leader << "  Has Converged ****\n");
    theNetStatsCollector().ReportNetEvent( "Local_Synchrony", "omni", 0, leader );

    int num_nodes;
    if (_globalSynchrony = theOmni().FullConvergenceTest( num_nodes )) {
      char netsize_str[128];
      assert( num_nodes != 0 );
      sprintf( netsize_str, "n%d-ht%lf-nt%lf-ut%lf", num_nodes,
	       theNetStatsCollector().TrafficSize( NetStatsCollector::Hello ),
	       theNetStatsCollector().TrafficSize( NetStatsCollector::NodePeer ),
	       theNetStatsCollector().TrafficSize( NetStatsCollector::UNI ) );
      theNetStatsCollector().ReportNetEvent( "Global_Synchrony", "omni", netsize_str, 0 );
    }
  }
  else if (result == Omni::changed_to_not_converged) {
    assert(leader);
    DIAG("omni",DIAG_INFO,
	 cout << "omni: **** PG of " << *leader << "  Has Diverged ****\n");
    theNetStatsCollector().ReportNetEvent( "Local_Discord", "omni", 0, leader );
    
    if (_globalSynchrony) {
      theNetStatsCollector().ReportNetEvent( "Global_Discord", "omni", 0, 0 );
      _globalSynchrony = false;
    }
  }
}


//--------------------------------------------------------
void Omni::DiscordLog(void) const {
  dic_item iter;

  forall_items(iter, _leader2subjects) {
    const NodeID * leader = _leader2subjects.key(iter);
    list<NodeID *> * subjects = _leader2subjects.inf(iter);

    const u_char level = leader->GetChildLevel();
    if (level == 0) {  
      continue;
    }

    DiscordLog( (NodeID*) leader );
  }
}


//--------------------------------------------------------
void Omni::DiscordLog(NodeID * leader) const {

  dic_item di = _leader2subjects.lookup(leader);
  assert(di);
  list<NodeID*>* subjects = _leader2subjects.inf(di);

  if (subjects->size() == 0)
    return;

  cout << "********* DISCORD LOG OF PGL *********\n";
  abbreviatedPrint(_entry, leader, false);
  cout << _entry << endl;
  cout << "**************************************\n";

  list_item li1, li2;
  bool converged = true;
  NodeID* subject1_name = 0;
  NodeID* subject2_name = 0;

  bool virgin = true;
  forall_items(li1, *subjects) {
    subject1_name = subjects->inf(li1);
    
    forall_items(li2, *subjects) {
      if (li1==li2)
	continue;

      subject2_name = subjects->inf(li2);
      DiscordLog(subject1_name, subject2_name);
    }
  }
}

//--------------------------------------------------------
void Omni::DiscordLog(NodeID * s1, NodeID* s2) const {
  dic_item di1, di2;

  di1 = _census.lookup(s1);
  di2 = _census.lookup(s2);
  if ((!di1) || (!di2)) return;

  dictionary<DBKey*, int>* little_black_book1 = _census.inf(di1);
  dictionary<DBKey*, int>* little_black_book2 = _census.inf(di2);

  bool is_discordant=false;

  dic_item iter;
  forall_items(iter, *little_black_book1) {
    DBKey* key = little_black_book1->key(iter);
    int seqnum1 = little_black_book1->inf(iter);

    dic_item srch = little_black_book2->lookup(key);
    if ( ! srch) {
      is_discordant=true;
      break;
    }
    int seqnum2 = little_black_book2->inf(srch);

    if (seqnum1 < seqnum2) {
      is_discordant=true;
      break;
    }
  }

  // reverso-faro shuffle

  forall_items(iter, *little_black_book2) {
    DBKey* key = little_black_book2->key(iter);
    int seqnum2 = little_black_book2->inf(iter);

    dic_item srch = little_black_book1->lookup(key);
    if ( ! srch) {
      is_discordant=true;
      break;
    }

    int seqnum1 = little_black_book1->inf(srch);

    if (seqnum1 > seqnum2) {
      is_discordant=true;
      break;
    }
  }

  if ( ! is_discordant)
    return;

  cout << "---------discord log between---------\n";
  abbreviatedPrint(_entry, s1, false);
  cout << "S1 : " << _entry << endl;
  abbreviatedPrint(_entry, s2, false);
  cout << "S2 : " << _entry << endl;
  cout << "-------------------------------------\n";


  forall_items(iter, *little_black_book1) {
    DBKey* key = little_black_book1->key(iter);
    int seqnum1 = little_black_book1->inf(iter);

    dic_item srch = little_black_book2->lookup(key);
    if ( ! srch) {
      cout << "+++ S1 --- S2 : ";
      key->Print(_entry);
      cout << _entry << endl;
      continue;
    }
    int seqnum2 = little_black_book2->inf(srch);

    if (seqnum1 < seqnum2) {
      cout << "S1 ("<<seqnum1<<") != S2 ("<<seqnum2<<") :";
      key->Print(_entry);
      cout << _entry << endl;
    }
  }

  // reverso-faro shuffle

  forall_items(iter, *little_black_book2) {
    DBKey* key = little_black_book2->key(iter);
    int seqnum2 = little_black_book2->inf(iter);

    dic_item srch = little_black_book1->lookup(key);
    if ( ! srch) {
      cout << "+++ S2 --- S1 : ";
      key->Print(_entry);
      cout << _entry << endl;
      continue;
    }

    int seqnum1 = little_black_book1->inf(srch);

    if (seqnum1 > seqnum2) {
      cout << "S1 ("<<seqnum1<<") != S2 ("<<seqnum2<<") :";
      key->Print(_entry);
      cout << _entry << endl;
    }
  }
}

bool Omni::AmIOmniscient( const Database * d ) const
{
  return (d == _omni_database);
}

DBLogosInterface * Omni::LogosInterface( void ) const
{
  return new DBLogosInterface( _omni_database );
}
