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
static char const _ElectionState_cc_rcsid_[] =
"$Id: ElectionState.cc,v 1.181 1999/02/10 18:32:59 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <fsm/election/ElectionState.h>
#include <fsm/election/InternalElection.h>
#include <fsm/election/ElectionVisitor.h>
#include <fsm/election/ElectionTimers.h>
#include <fsm/election/ElectionInterface.h>

#include <FW/basics/diag.h>
#include <FW/basics/Conduit.h>
#include <codec/pnni_ig/nodal_info_group.h>

#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/hello/HelloVisitor.h>
#include <fsm/visitors/LGNVisitors.h>
#include <fsm/visitors/VPVCVisitor.h>
#include <fsm/visitors/PortDownVisitor.h>
#include <fsm/nodepeer/NPVisitors.h>
#include <fsm/hello/HelloState.h>

// Page 120 of PNNI 1.0 Spec.
const double SEARCH_PEER_INTERVAL = (HelloState::InactivityFactor *
				     HelloState::TimerInterval);
const double PGL_INIT_INTERVAL    = ElectionState::PGLInitTime;
const double OVERRIDE_INTERVAL    = ElectionState::OverrideDelay;
const double REELECTION_INTERVAL  = ElectionState::ReElectionInterval;

// ---------------------------------------------------------------------------
void ElectionState::BuildStates(void)
{
  if (!_Starting) _Starting = new StateStarting();
  if (!_Awaiting) _Awaiting = new StateAwaiting();
  if (!_AwaitingFull) _AwaitingFull = new StateAwaitingFull();
  if (!_InitialDelay) _InitialDelay = new StateInitialDelay();
  if (!_Calculating) _Calculating = new StateCalculating();
  if (!_OperNotPGL) _OperNotPGL = new StateOperNotPGL();
  if (!_OperPGL) _OperPGL = new StateOperPGL();
  if (!_AwaitUnanimity) _AwaitUnanimity = new StateAwaitUnanimity();
  if (!_HungElection) _HungElection = new StateHungElection();
  if (!_AwaitReelection) _AwaitReelection = new StateAwaitReelection();
}

void ElectionState::FreeStates(void)
{
  delete _Starting;
  delete _Awaiting;
  delete _AwaitingFull;
  delete _InitialDelay;
  delete _Calculating;
  delete _OperNotPGL;
  delete _OperPGL;
  delete _AwaitUnanimity;
  delete _HungElection;
  delete _AwaitReelection;
}

InternalElectionState * GetState(ElectionState::StateID sid)
{
  InternalElectionState * rval = 0;

  switch (sid) {
    case ElectionState::Starting:
      rval = ElectionState::_Starting;
      break;
    case ElectionState::Awaiting:
      rval = ElectionState::_Awaiting;
      break;
    case ElectionState::AwaitingFull:
      rval = ElectionState::_AwaitingFull;
      break;
    case ElectionState::InitialDelay:
      rval = ElectionState::_InitialDelay;
      break;
    case ElectionState::Calculating:
      rval = ElectionState::_Calculating;
      break;
    case ElectionState::OperNotPGL:
      rval = ElectionState::_OperNotPGL;
      break;
    case ElectionState::OperPGL:
      rval = ElectionState::_OperPGL;
      break;
    case ElectionState::AwaitUnanimity:
      rval = ElectionState::_AwaitUnanimity;
      break;
    case ElectionState::HungElection:
      rval = ElectionState::_HungElection;
      break;
    case ElectionState::AwaitReelection:
      rval = ElectionState::_AwaitReelection;
      break;
    default:
      diag("fsm.election", DIAG_FATAL, 
	   "Invalid state passed to GetState(ElectionState::StateID).\n");
      break;
  }
  return rval;
}

// ---------------------------------------------------------------------------
int                     ElectionState::_ref_count       = 0;
InternalElectionState * ElectionState::_Starting        = 0;
InternalElectionState * ElectionState::_Awaiting        = 0;
InternalElectionState * ElectionState::_AwaitingFull    = 0;
InternalElectionState * ElectionState::_InitialDelay    = 0;
InternalElectionState * ElectionState::_Calculating     = 0;
InternalElectionState * ElectionState::_OperNotPGL      = 0;
InternalElectionState * ElectionState::_OperPGL         = 0;
InternalElectionState * ElectionState::_AwaitUnanimity  = 0;
InternalElectionState * ElectionState::_HungElection    = 0;
InternalElectionState * ElectionState::_AwaitReelection = 0;
const VisitorType     * ElectionState::_link_type       = 0;

ElectionState::ElectionState(const NodeID * nid, int priority) :
  _NodeType( PhysicalNode ), _LocNodeID(0),
  _cs(0), _PreferredPGL(0), _OldPGL(0), _OldPGLlp(0), 
  _ReelectionTimer(0), _LocalPriority(priority), 
  _PreferredPGLlp(0), _votecount(0), _SearchPeerTimer(0), 
  _PGLInitTimer(0), _OverrideUnanimityTimer(0), _parent_expander(0)
{
  BuildStates();

  //  if (!nid) nid = (NodeID *)db->GetNID();
  assert( nid != 0 );

  //  if (!_PreferredPGL) _PreferredPGL = (NodeID *)db->GetNID()->copy();  // BK 1/19/99

  ChangeState(Awaiting);
  // Make copies of our local NodeID
  assert( _LocNodeID      = new NodeID (*nid) );
  if (_LocNodeID->GetChildLevel() < 160)
    _NodeType = LogicalNode;

  // Initialize the timers
  _SearchPeerTimer = new SearchPeerTimer(this, SEARCH_PEER_INTERVAL);
  _PGLInitTimer    = new PGLInitTimer(this, PGL_INIT_INTERVAL);
  _OverrideUnanimityTimer = new OverrideUnanimityTimer(this, OVERRIDE_INTERVAL);
  _ReelectionTimer = new ReelectionTimer(this, REELECTION_INTERVAL);
  RegisterTimer(SearchPeerTIMER);

  _ref_count++;

  _name = new char[255];

  if (!_link_type)
    _link_type = QueryRegistry(LINK_VISITOR_NAME);

  AddPermission("*", new ElectionInterface(this));
}

ElectionState::~ElectionState( )
{
  if (_LocNodeID)              delete _LocNodeID;
  if (_PreferredPGL)           delete _PreferredPGL;
  if (_SearchPeerTimer)        delete _SearchPeerTimer;
  if (_PGLInitTimer)           delete _PGLInitTimer;
  if (_OverrideUnanimityTimer) delete _OverrideUnanimityTimer;
  if (_ReelectionTimer)        delete _ReelectionTimer;
  if (--_ref_count == 0)       FreeStates();

  delete [] _name;
  _name = 0;
}

State * ElectionState::Handle(Visitor * v)
{
  if (_cs) _cs->Handle(this, v);
  // Always return this to the parent Protocol
  return this;
}

void ElectionState::Interrupt(SimEvent * ev) { }

void ElectionState::RegisterTimer(ElectionState::ElectionTimers t)
{
  switch (t) {
    case SearchPeerTIMER:
      Register(_SearchPeerTimer); break;
    case OverrideUnanimityTIMER:
      Register(_OverrideUnanimityTimer); break;
    case ReelectionTIMER:
      Register(_ReelectionTimer); break;
    case PGLInitTIMER:
      Register(_PGLInitTimer); break;
  }
}

void ElectionState::CancelTimer(ElectionState::ElectionTimers t)
{
  switch (t) {
    case SearchPeerTIMER:
      Cancel(_SearchPeerTimer); break;
    case OverrideUnanimityTIMER:
      Cancel(_OverrideUnanimityTimer); break;
    case ReelectionTIMER:
      Cancel(_ReelectionTimer); break;
    case PGLInitTIMER:
      Cancel(_PGLInitTimer); break;
  }
}

void ElectionState::ChangeState(ElectionState::StateID s)
{
  _cs = GetState(s);
  DIAG("fsm.election", DIAG_DEBUG, 
       if (OwnerName()) cout << OwnerName() << " changed to " << _cs->StateName() << "." << endl;
      );
}

void ElectionState::CalculateVotes(void)
{
  _votecount = 0;

  seq_item si;
  forall_items(si, _members) {
    const NodeID * nid = _members.key(si);
    Nodalinfo    * nif = _members.inf(si);
    if (nif->GetElectionFlag()) {  // If electionfit is set participate in election
      DIAG("fsm.election", DIAG_DEBUG, cout << OwnerName() 
	   << " Election: node and its PGL are:"
           << endl << *nid << endl << *nif->GetPrefPGL() << endl << endl);
      if (nif->GetPrefPGL() && (*_LocNodeID == *nif->GetPrefPGL()))
        _votecount++;
    }
  }
}

void ElectionState::SetParent(const Conduit * c)
{ 
  assert( _parent_expander == 0 );
  _parent_expander = (Conduit *)c; 
}

ElectionState::NodeType ElectionState::GetNodeType(void) const
{ return _NodeType; }

const NodeID * ElectionState::GetLocalNID(void) const
{
  return _LocNodeID;
}

void ElectionState::DeleteParent(void)
{ 
  assert( _parent_expander != 0 );
  delete _parent_expander; 
}

InternalElectionState * ElectionState::GetCS(void) const
{
  return _cs;
}

void ElectionState::SetOldPGL(const NodeID * pgl)
{
  if (_OldPGL)
    delete _OldPGL;
  _OldPGL = (pgl != 0 ? new NodeID(*pgl) : 0);
}

void ElectionState::SetPreferredPGL(const NodeID * pgl)
{
  if (_PreferredPGL) {
    SetOldPGL(_PreferredPGL);
    // The above makes a copy
    delete _PreferredPGL;
  }
  _PreferredPGL = (pgl != 0 ? new NodeID(*pgl) : 0);
}

void ElectionState::SendVisitor(Visitor * v, Visitor::which_side s)
{
  switch (s) {
    case Visitor::A_SIDE:
      PassVisitorToA(v);
      break;
    case Visitor::B_SIDE:
      PassVisitorToB(v);
      break;
    case Visitor::OTHER:
    default:
      PassThru(v);
      break;
  }
}

const NodeID * ElectionState::GetPreferredPGL(void) const { return _PreferredPGL; }
const NodeID * ElectionState::GetPreferredPGL(const NodeID * nid) const 
{ 
  const NodeID * rval = 0;

  DatabaseInterface * dbi = (DatabaseInterface *)QueryInterface( "Database" );
  assert( dbi != 0 && dbi->good() );
  dbi->Reference();
  rval = dbi->PreferredPeerGroupLeader( nid->GetLevel() );
  dbi->Unreference();
  return rval;
}
const NodeID * ElectionState::GetOldPGL(void) const { return _OldPGL; }

const Nodalinfo * ElectionState::GetNodalinfo(const NodeID * nid) const
{
  seq_item si;
  if (si = _members.lookup(nid) )
    return _members.inf(si);
  return 0;
}

const int ElectionState::GetPreferredPrio(void) const
{ return _PreferredPGLlp; }

const int ElectionState::GetOldPrio(void) const
{ return _OldPGLlp; }

const int ElectionState::GetLocalPrio(void) const
{ return _LocalPriority; }

void      ElectionState::SetPreferredPrio(int p)
{ _PreferredPGLlp = p; }

void      ElectionState::SetOldPrio(int p)
{ _OldPGLlp = p; }

void      ElectionState::SetLocalPrio(int p)
{ _LocalPriority = p; }

int       ElectionState::GetVotes(void) const
{ return _votecount; }

int       ElectionState::NumVoters(void) const
{ 
  int rval = 0;
#if 1 // It's not quite that simple
  rval = _members.size(); 
#else
  seq_item si;
  forall_items( si, _members ) {
    Nodalinfo    * nif = _members.inf(si);

    if (nif->GetElectionFlag())
      rval++;
  }
#endif
  return rval;
}

void      ElectionState::AddVoter(const NodeID * nid, Nodalinfo * nif)
{
  _members.insert(nid, nif);
}

const sortseq<const NodeID *, Nodalinfo *> * ElectionState::GetVoters(void) const
{
  return &_members;
}

const sortseq<const NodeID *, list<int> *> * ElectionState::GetPorts(void) const
{
  return &_ports;
}

void ElectionState::UpdatePorts(Visitor * v)
{
  LinkVisitor * lv = (LinkVisitor *)v;
  VPVCVisitor * vv = (VPVCVisitor *)v;
    
  const NodeID * rnid = 0;
  int port = -1;

  if (v->GetType().Is_A(_link_type)) {
    rnid = lv->GetSourceNID();
    port = lv->GetLocalPort();
  } else {
    rnid = vv->GetSourceNID();
    port = vv->GetInPort();
  }

  if (rnid != 0 && port != -1) {
    seq_item si;
    if (si = _ports.lookup(rnid)) {
      list<int> * ports = _ports.inf(si);
      ports->append( port );
    } else {
      list<int> * ports = new list<int>;
      ports->append( port );
      _ports.insert(rnid->copy(), ports);
    }
  }
}

bool ElectionState::ClearPorts(const NodeID * rnid, LinkVisitor * v)
{
  seq_item si;
  if (si = _ports.lookup(rnid)) {
    list<int> * ports = _ports.inf(si);
    list_item li = ports->search(v->GetLocalPort());
    if (li) {
      ports->del_item(li); 
      if (ports->empty()) {
        delete ports;
	delete (NodeID *)_ports.key(si);
        _ports.del_item(si);
      }
    } else {
      diag("fsm.election", DIAG_ERROR, 
           "%s PortDown without an entry for the port\n", OwnerName());
    }
  } else {
    diag("fsm.election", DIAG_ERROR, 
	 "%s No entry for ports to the remote node \n",
         OwnerName());
  }

  if (_ports.empty()) {
    diag("fsm.election", DIAG_DEBUG, 
         "%s All connections to the remote peers lost \n", OwnerName());
    return true;
  }
  return false;
}

const char * ElectionState::GetName(void) {
  if (OwnerName())
    sprintf(_name,"%s",OwnerName());
  else
    sprintf(_name,"Unknown-ElectionState");
  return _name;
}

// ---------------------------------------------------------------------------
Nodalinfo::Nodalinfo(const NodeID * nid,  int priority, 
		     const NodeID * pref, int transit) 
  : _priority(priority), _ElectionFlag(false), 
    _PreferredPGL(0), _electiontransit(transit),
    _NodeID(0)
{  
  assert(nid != 0);
  _NodeID = new NodeID(*nid);
  _PreferredPGL = (pref ? new NodeID(*pref) : new NodeID((const unsigned char *)0)); 
}

Nodalinfo::~Nodalinfo() 
{ 
  delete _NodeID;
  delete _PreferredPGL; 
}

int Nodalinfo::GetPriority(void) const
{ 
  return _priority; 
}

void Nodalinfo::SetPriority(int prio)
{ 
  _priority = prio; 
}

void Nodalinfo::SetElectionFlag(bool flag)
{ 
  _ElectionFlag = flag; 
}

bool Nodalinfo::GetElectionFlag(void) const
{ 
  return _ElectionFlag; 
}

void Nodalinfo::SetElectionTransit(int flag)
{ 
  _electiontransit = flag; 
}

int Nodalinfo::GetElectionTransit(void) const
{ 
  return _electiontransit; 
}

void Nodalinfo::SetPrefPGL(const NodeID * Apgl)
{ 
  if (_PreferredPGL) delete _PreferredPGL;
  _PreferredPGL = (Apgl ? new NodeID(*Apgl) : (NodeID *)0); 
}

const NodeID * Nodalinfo::GetPrefPGL(void) const
{ 
  return _PreferredPGL; 
}

const NodeID * Nodalinfo::GetNodeID(void) const
{
  return _NodeID;
}


