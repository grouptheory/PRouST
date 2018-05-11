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
static char const _cronos_cc_rcsid_[] =
"$Id: cronos.cc,v 1.92 1999/02/26 18:25:39 mountcas Exp $";
#endif

#include "cronos.h"
#include <DS/containers/list.h>
#include <DS/random/distributions.h>
#include <DS/util/String.h>
#include <FW/basics/diag.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <common/cprototypes.h>
#include <fsm/config/Configurator.h>
#include <fsm/config/LinkConfiguration.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/visitors/FileIOVisitors.h>
#include <fsm/visitors/LinkVisitor.h>
#include <fstream.h>
#include <iostream.h>
#include <math.h>

#define DEFAULT_PG           {  160, 0x47, 0x00, 0x05, 0x80, 0xff, 0xde, \
                               0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x02 }
#define DEFAULT_PGLEV 96

// -------------------------------- timeseqTimer ------------------------------
timeseqTimer::timeseqTimer(timeseq * ts, double t) 
  : TimerHandler(ts,t), _ts(ts) { }

timeseqTimer::~timeseqTimer() { }

void timeseqTimer::Callback(void) 
{
  void * v;
  void (* tocall) (void);
  
  list_item li;
  forall_items(li, _funcs) {
    v = _funcs.inf(li);
    tocall = (void(*)(void))v;
    if (tocall)
      (*tocall)();
  }

  KernelTime expiretime = GetExpirationTime();
  if (_ts) {
    double wait = _ts->NextBlip(expiretime);
    if (wait >= 0.0) {
      ExpiresIn(wait);
      Register(this);
    }
  }
}

void timeseqTimer::AttachProcedure(void * f)
{
  assert(f != 0);
  _funcs.append(f);
}

//-----------------------------------------------------------------------------
timeseq::timeseq(tstype t, char *filename) 
  : _t(t), _timer(0), _start(0), _end(0), _infile(0), _outfile(0)
{ 
  struct timeval tp;
  gettimeofday(&tp, 0);
  srand(tp.tv_usec);

  if (filename != 0) {
    _infile = new ifstream(filename);

    if (_infile->bad()) {	// filename doesn't exist
      delete _infile;
      _infile = 0;

      _outfile = new ofstream(filename);
      if (_outfile->bad()) {
	delete _outfile;
	diag("fate.timeseq", DIAG_FATAL,
	     "Can't open %s for reading or writing.\n", filename);

	exit(1);	// XXX Might be unreachable
      }
    }
  }
}

timeseq::~timeseq() { }

void timeseq::SetStart(double start) { _start=start; }
void timeseq::SetEnd(double end)     { _end=end; }

void timeseq::SetParam(double one) {/*nop*/}
void timeseq::SetParam(double one, double two) {/*nop*/}
void timeseq::SetParam(double one, double two, double three) {/*nop*/}
void timeseq::SetParam(double one, double two, double three, double four) 
{/*nop*/}

//
// What is the next timer interval?
//
// Read it from a file or write the last one to a file and ask
// CalculateBlip() for the next interval.
//
double timeseq::NextBlip(double last_expiretime)
{
  double answer = -1.0;
  double now = (theKernel().CurrentElapsedTime());

  if (_infile != 0) {	// Read the next blip from _infile if it exists
    if (! _infile->eof()) {
      double next = 0.0;
      *_infile >> next;
      answer = next - now;
    }
  } else {		/* Otherwise, write the time to _outfile if it
			 * exists and * ask the derived class to
			 * calculate the next blip.  */
    if (_outfile != 0) {
      _outfile->precision( 20 );
      *_outfile << now << endl;
    }
    answer = CalculateBlip(last_expiretime);
  }

  return answer;
}


//
// Setup for the first timer interval.
//
// Read the interval from a file if necessary.
//
void timeseq::Boot(void)
{
  if (_infile != 0) {
    *_infile >> _start;
  }

  _timer = new timeseqTimer(this,_start);
  double now = theKernel().CurrentTime();
  _start += now;
  _end += now;
  Register(_timer);
}

void timeseq::Interrupt(SimEvent * e) { }

void timeseq::AttachProcedure(void * f)
{
  assert(_timer && f);
  _timer->AttachProcedure(f);
}

double timeseq::Rand(double max)
{
  double r = (rand() / MAXINT);
  //  static random_source rand;
  //  double r;
  //  rand >> r;
  return (r * max);
}

// ------------------------- periodic_ts -------------------
periodic_ts::periodic_ts(char *filename) : timeseq(periodic, filename) { }
periodic_ts::~periodic_ts() { }
void periodic_ts::SetParam(double one) { _period=one; }
double periodic_ts::CalculateBlip(double last_expiretime)
{
  double next = _period + last_expiretime;
  if ((next >= _start) && (next <= _end)) 
    return _period;
  else 
    return -1.0;
}

// ------------------------- uniform_ts -------------------
uniform_ts::uniform_ts(char *filename) : timeseq(uniform, filename),
  _interval(1.0) { }
uniform_ts::~uniform_ts() { }
void uniform_ts::SetParam(double one) { _interval=one; }
double uniform_ts::CalculateBlip(double last_expiretime)
{
  double next = uniform_distribution::sample(1, _interval);

  if ((next + last_expiretime >= _start) && (next + last_expiretime <= _end)) 
    return next;
  else 
    return -1.0;
}

// ------------------------- poisson_ts -------------------
poisson_ts::poisson_ts(char *filename) : timeseq(poisson, filename), _intensity( 1 ) { }
poisson_ts::~poisson_ts() { }
void poisson_ts::SetParam(double one) { _intensity=one; }
double poisson_ts::CalculateBlip(double last_expiretime)
{
  double next = poisson_distribution::sample(_intensity);

  if ((next + last_expiretime >= _start) && (next + last_expiretime <= _end)) 
    return next;
  else 
    return -1.0;
}

// ------------------------- explicit_ts -------------------
explicit_ts::explicit_ts(char *filename) : timeseq(table, filename) { }
explicit_ts::~explicit_ts() { }
void explicit_ts::AddTick(double one) 
{ 
  // Don't bother appending anything if we're reading our blips from a
  // file.  This keeps _blips empty, which we want when
  // explicit_ts::Boot() is called.

  if (_infile == 0) {
    _blips.append(one);
    if (_blips.size() > 1)
      _blips.sort();
  }
}

double explicit_ts::CalculateBlip(double last_expiretime)
{
  double rval = -1.0;

  if (_blips.empty() == false)
    rval = _blips.pop() - last_expiretime;

  return rval;
}

void explicit_ts::Boot(void)
{
  if (_infile != 0)
    timeseq::Boot();
  else {
    if (_blips.empty()) 
      return;
    double f = _blips.pop();
    _timer = new timeseqTimer(this,f);
    Register(_timer);
  }
}

//-----------------------------------------------------------------------------
GenComp::GenComp(comptype t) : _t(t) { }
GenComp::~GenComp() { }

void GenComp::Associated(set<SwitchComp*>* g) {/*nop*/}
void GenComp::Associated(set<LinkComp*>* g)   {/*nop*/}
void GenComp::Expelled(set<SwitchComp*>* g)   {/*nop*/}
void GenComp::Expelled(set<LinkComp*>* g)     {/*nop*/}

//-----------------------------------------------------------------------------
SwitchComp::SwitchComp(ds_String *key, int id, ds_String * name, 
		       NodeID *myNode) :
  GenComp(switchtype), _ports(0), _myPreferredPeerGroupLeader(0)
#ifndef __FAKE__
  , _the_terminal(0), _the_conduit(0), _myNodeID(myNode), _terminal(0)
#endif
{
  _ports = theConfigurator().Ports(key);
  int i;

  if (_ports > 0) {
    _port_avail = new bool [_ports]; 
    for (i = 1; i < _ports; i++) 
      _port_avail[i] = true;
    _port_avail[0] = false;
  
    _links = new LinkComp * [_ports];
    for (i = 0; i < _ports; i++) 
      _links[i] = 0;
  
#ifndef __FAKE__
    birth_switch(key, id, name);
#endif
  } else {
    DIAG("fate", DIAG_FATAL, 
	 cerr << "fate (Fatal): No such key in Configurator: " 
	 << *key << endl; );
  }
}


SwitchComp::~SwitchComp() 
{
  for (int i = 1; i < _ports; i++) 
    if (_links[i]) delete _links[i];

  delete [] _port_avail;
  delete [] _links;

#ifndef __FAKE__
  death_switch();
#endif
}

int SwitchComp::TakeFreePort(LinkComp * lk) 
{ 
  if (!lk)
    return -1;

  for (int i = 1; i < _ports; i++) {
    if (_port_avail[i]) {
      _port_avail[i] = false;
      _links[i] = lk;
      return i;
    }
  }
  return -1;
}

void SwitchComp::ReturnPort(int p) 
{ 
  // Can never return port 0
  if (p != 0 && !_port_avail[p]) {
    _port_avail[p] = true;
    _links[p] = 0;
  }
}

void SwitchComp::Associated(set<SwitchComp*> * g)
{
  if (!_groups.member(g))
    _groups.insert(g);
}

void SwitchComp::Expelled(set<SwitchComp*> * g)
{
  if (_groups.member(g))
    _groups.del(g);
}

void SwitchComp::AcceptVisitor(Visitor * v, bool exp, double duration)
{
#ifndef __FAKE__
  assert( _terminal != 0 );
  _terminal->SetupCall( v, exp, duration );
#endif
}

#ifndef __FAKE__
bool SwitchComp::birth_switch(ds_String *key, int id, ds_String * name)
{
  bool answer = false;
  char switchNumber[255];
  sprintf(switchNumber, "_%d", id);
  ds_String *realName = new ds_String(*name);
  *realName += switchNumber;

  _terminal = new FateTerm( _myNodeID );
  _the_terminal = new Conduit( "FateTerminal", _terminal );
  _the_conduit = MakeSwitch( key, realName, _myNodeID, _the_terminal );
  answer = true;

  return answer;
}

bool SwitchComp::death_switch(void) 
{
  delete _the_conduit;
  delete _myNodeID;
  delete _myPreferredPeerGroupLeader;
  return true;
}

Conduit * SwitchComp::getConduit(void) const
{
  return _the_conduit;
}

NodeID * SwitchComp::getNodeID(void) const
{
  return _myNodeID;
}

const char * SwitchComp::getName(void) const
{
  return (_the_conduit->GetName());
}

#endif

//-----------------------------------------------------------------------------
LinkComp::LinkComp(SwitchComp * s1, SwitchComp * s2, 
		   ds_String *fbKey, ds_String *bbKey)
  : GenComp(linktype), _s1(s1), _s2(s2), _isvalid(false)
{ 
  int p1, p2;
  ds_String *actualBBKey = ((bbKey == 0) ? fbKey : bbKey);

  if (!s1 || !s2) {
    _p1 = _p2 = -1;
    _isvalid = false;
  } else {
    p1 = s1->TakeFreePort(this);
    p2 = s2->TakeFreePort(this);
    if ((p1 != -1) && (p2 != -1)) {
      _p1 = p1;
      _p2 = p2;
      _isvalid = true;
#ifndef __FAKE__
      birth_link(s1, p1, s2, p2, fbKey, actualBBKey);
#endif
    } else {
      s1->ReturnPort(p1);
      s2->ReturnPort(p2);
      _s1 = _s2 = 0;
      _p1 = _p2 = -1;
      _isvalid = false;
#ifndef __FAKE__
      _the_link = 0;
#endif
    }
  }
}

LinkComp::~LinkComp() 
{
  if (_isvalid) {
    _s1->ReturnPort(_p1);
    _s2->ReturnPort(_p2);
#ifndef __FAKE__
    death_link();
#endif
  }
}

bool LinkComp::IsValid(void) { return _isvalid; }

void LinkComp::Associated(set<LinkComp*> * g)
{
  if (!_groups.member(g))
    _groups.insert(g);
}

void LinkComp::Expelled(set<LinkComp*> * g)  
{
  if (_groups.member(g))
    _groups.del(g);
}

#ifndef __FAKE__
void LinkComp::Disable(void)
{
  _the_link->Disable();
}

void LinkComp::Enable(void)
{
  _the_link->Enable();
}

const char * LinkComp::getName(void) const
{
  assert( _the_link && _the_link->Container() );
  return _the_link->Container()->GetName();
}

bool LinkComp::birth_link(SwitchComp * c1, int port1, 
			  SwitchComp * c2, int port2, 
			  ds_String *fwd_bandwidthKey, 
			  ds_String *bwd_bandwidthKey)
{
  if (!c1 || !c2)
    return false;

  Conduit * s1 = c1->getConduit(),
          * s2 = c2->getConduit();

  _the_link = LinkSwitches(s1, port1, s2, port2, 
			   fwd_bandwidthKey, bwd_bandwidthKey);
  return true;
}

bool LinkComp::death_link(void)
{
  delete _the_link;
  return true;
}

#endif

//-----------------------------------------------------------------------------
CompMaker::CompMaker(void) : _next_sw_id(1), _num_switches(0), _num_links(0)
{ }

CompMaker::~CompMaker(void)
{
  // Delete all of the NodeID keys.
  dic_item ditem;
  forall_items(ditem, _peerGroups) {
    delete _peerGroups.key(ditem);
  }

  _peerGroups.clear();
}

SwitchComp * CompMaker::MakeSwitch(char *key, char *peerGroup)
{
  SwitchComp * answer = 0;
  ds_String *switch_key = (key != 0) ? new ds_String(key) : 0;
  ds_String *switch_name = 
    (switch_key != 0) ? new ds_String(*switch_key) : new ds_String("SWITCH");
  NodeID *tempNodeID = theConfigurator().BaseNodeID(switch_key);
  PeerID *pg = 0;

  if (tempNodeID != 0) {
    // Naughty! casting away constness!
    // This is safe to do because tempNodeID is a copy of 
    // what's in theConfigurator() and because we're going
    // to throw that NodeID away after we modify it.
    u_char *tempNodeIDchars = (u_char *)tempNodeID->GetNID();

    // [1...14] is the PeerGroup ID, but we clobber the last two bytes
    //  since the remainder is reserved for hosts.
    if (peerGroup != 0) {
      // peerGroup is specified.  Copy all of the bits and hope 
      // that they are enough to make the NodeID unique.
      //
      // The first two characters are the level.
      // We set the child level is to 160.
      // 
      pg = new PeerID((const char *)peerGroup);
      const u_char *peerGroupchars = pg->GetPGID();
      tempNodeIDchars[0] = pg->GetLevel();
      tempNodeIDchars[1] = 160;

      memcpy(tempNodeIDchars + 2, peerGroupchars + 1, 13);
    } else {
      // No peer group given, so use the one in tempNodeID.
      // Notice that we don't change either of the levels like we do above.
      pg = tempNodeID->GetPeerGroup();
    }

    // We need to limit the number of switches in the peer group to 255.
    // 
    // To do this, we 
    //		Make a NodeID out of the base node id and the peer group.
    //
    //		Look for it in _peerGroups.
    //
    //		If it's there, extract the number.
    //			If the number is bigger than 255, then abort.
    //
    //		If it's not, make a new entry in _peerGroups.
    //			Let the number be zero.
    //
    //		Make a unique NodeID.
    //		Increment the number.
    //		Put it back into _peerGroups.
    //
    dic_item ditem = _peerGroups.lookup(pg);
    int pg_switch_id = 0;

    if (ditem != 0) {
      pg_switch_id = _peerGroups.inf(ditem);

      if (pg_switch_id > 255) {
	DIAG("fate", DIAG_FATAL, 
	     cerr << "fate (Fatal): "
	     << "Can't have more than 255 switches per peer group ("
	     << *pg 
	     << ")"
	     << endl; );
      }

    } else {
      // _peerGroups owns the initial key for any peer group.
      ditem = _peerGroups.insert(new PeerID(*pg), 0);
    }

    //    if (id & 0xFF00) {
    //      tempNodeIDchars[13] = ((id & 0xFF00) >> 8);
    //    }
    tempNodeIDchars[14] =  (pg_switch_id & 0x00FF);
    NodeID *newSwitchNodeID = new NodeID(tempNodeIDchars);
    delete tempNodeID;
    
    // SwitchComp() owns newSwitchNodeID.
    answer = new SwitchComp(switch_key, _num_switches, switch_name, 
			    newSwitchNodeID);

    pg_switch_id++;
    _peerGroups.change_inf(ditem, pg_switch_id);

    _num_switches++;
    _u_switches.insert(answer);
  } else {
    DIAG("fate", DIAG_FATAL, 
	 cerr << "fate (Fatal): No such key in Configurator: " 
	 << *key << endl; );
  }

  delete pg;
  delete switch_key;
  delete switch_name;

  return answer;
}

LinkComp * CompMaker::MakeLink(SwitchComp * s1, SwitchComp * s2, 
			       char *fwd_bw, 
			       char *bwd_bw)
{
  if (!s1 || !s2) 
    return 0;
  ds_String *fwd_bw_key = 
    (fwd_bw != 0) ? new ds_String(fwd_bw) : new ds_String("OC3");

  ds_String *bwd_bw_key = 
    (bwd_bw != 0) ? new ds_String(bwd_bw) : new ds_String("OC3");

  
  LinkComp * lk = new LinkComp(s1, s2, fwd_bw_key, bwd_bw_key);
  if (lk->IsValid()) {
    _num_links++;
    _u_links.insert(lk);
    return lk;
  }
  delete lk;
  return 0;
}

Visitor * CompMaker::MakeVisitor(const char * type, SwitchComp * src, 
				 SwitchComp * dest, 
				 int mesg_type, char * filename, long opt_arg)
{
  Visitor * vis = 0;

  if (!strcmp(type, "FastUNIVisitor")) {
    assert(src && dest);
    if (opt_arg)
      vis = new FastUNIVisitor(src->getNodeID(), dest->getNodeID(), 0, 0, 0, 0,
			       (FastUNIVisitor::uni_message)mesg_type, 
			       filename, opt_arg);
    else
      vis = new FastUNIVisitor(src->getNodeID(), dest->getNodeID(), 0, 0, 0, 0,
			       (FastUNIVisitor::uni_message)mesg_type,
			       filename);
  }
  return vis;
}

void CompMaker::MakeConnection(const NodeID * src, const NodeID * dst, int bandwidth,
			       bool exp, double duration)
{
  SwitchComp * sc_src = 0, * sc_dst = 0;

  list_item li;
  forall_items( li, _u_switches ) {
    SwitchComp * tmp = _u_switches.inf( li );
    if ( tmp->getNodeID()->equals( src ) )
      sc_src = tmp;
    else if ( tmp->getNodeID()->equals( dst ) )
      sc_dst = tmp;

    if ( sc_src != 0 && sc_dst != 0 )
      break;
  }

  assert( sc_src != 0 && sc_dst != 0 );
  MakeConnection( sc_src, sc_dst, bandwidth, exp, duration );
}

void CompMaker::MakeConnection(SwitchComp * src, SwitchComp * dst, int bandwidth, 
			       bool exp, double duration)
{
  assert( src != 0 && dst != 0 && bandwidth > 0 && duration > 0.0 );

  FastUNIVisitor * fuv = new FastUNIVisitor( src->getNodeID(),
					     dst->getNodeID(),
					     0, 0, 0, 0,
					     FastUNIVisitor::FastUNISetup,
					     0, bandwidth );

  assert( fuv != 0 );

  // Send it down
  src->AcceptVisitor( fuv, exp, duration );
}

void CompMaker::KillSwitch(SwitchComp *& sw)
{
  if (sw) {
    _num_switches--;
    if (_u_switches.member(sw)) 
      _u_switches.del(sw);
  }
  delete sw;
  sw = 0;
}

void CompMaker::KillLink(LinkComp*& lk)
{
  if (lk) {
    _num_links--;
    if (_u_links.member(lk)) 
      _u_links.del(lk);
  }
  delete lk;
  lk = 0;
}

void CompMaker::AddToGroup(set<SwitchComp*>* grp, SwitchComp* sw)
{
  if (sw && !grp->member(sw)) {
    grp->insert(sw);
    sw->Associated(grp);
  }
}

void CompMaker::RemFromGroup(set<SwitchComp*>* grp, SwitchComp* sw)
{
  if (sw && grp->member(sw)) {
    grp->del(sw);
    sw->Expelled(grp);
  }
}

void CompMaker::ClearGroup(set<SwitchComp*>* grp)
{
  list_item li;
  forall_items(li, *grp) {
    SwitchComp * x = grp->inf(li);
    x->Expelled(grp);
  }
  grp->clear();
}


void CompMaker::AddToGroup(set<LinkComp*>* grp, LinkComp* lk)
{
  if (lk && (!grp->member(lk))) {
    grp->insert(lk);
    lk->Associated(grp);
  }
}

void CompMaker::RemFromGroup(set<LinkComp*>* grp, LinkComp* lk)
{
  if (lk && (grp->member(lk))) {
    grp->del(lk);
    lk->Expelled(grp);
  }
}

void CompMaker::ClearGroup(set<LinkComp*>* grp)
{
  list_item li;
  forall_items(li, *grp) {
    LinkComp * x = grp->inf(li);
    x->Expelled(grp);
  }
  grp->clear();
}

SwitchComp * CompMaker::ChooseSwitch( set<SwitchComp*>* g )
{
  int n = g->size();

  double next = uniform_distribution::sample(0, n);
  int index = (int)floor(next);

  return (*g)[index];
}

LinkComp * CompMaker::ChooseLink( set<LinkComp*>* g )
{
  int n = g->size();

  double next = uniform_distribution::sample(0, n);
  int index = (int)floor(next);

  return (*g)[index];
}

bool CompMaker::Toss(double x)
{
  if (x < 0.0) x = 0.0;
  if (x > 1.0) x = 1.0;

  double r = (rand() / MAXINT);
  //  static random_source rand;
  //  double r;
  //  rand >> r;
  if (r > x) return false;
  return true;
}

void CompMaker::Snap(const char * filename)
{
#ifdef OLD_SNAP
  FILE * fp;
  if (!(fp = fopen(filename, "w+")))
    return;

  fprintf(fp, "LEDA.GRAPH\nunknown\nunknown\n%d\n", _num_switches);
  for (int i = 0; i < _num_switches; i++)
    fprintf(fp, "\n");
  fprintf(fp, "%d\n", (_num_links * 2));
  fclose(fp);
#else
  // LATER
  ofstream ofs(filename);
  list_item li;
  forall_items(li, _u_switches) {
    SwitchComp * sw = _u_switches.inf(li);
     StreamSaveVisitor * v = new StreamSaveVisitor(ofs);
     // It flows up to ACAC who uses it's output operator to save it's
     // local view.
     sw->AcceptVisitor(v);
  }
  ofs.close();
  // by now we should have one big-ass file which contains the view
  // from each ACAC we can either leave it for Jack else to deal with,
  // or sort it nicely...
#endif
}

void CompMaker::UniverseOfSwitches( set<SwitchComp*>& g )
{
  g.clear();
  list_item li;
  forall_items(li, _u_switches) {
    SwitchComp * x = _u_switches.inf(li);
    g.insert(x);
  }
}

void CompMaker::UniverseOfLinks( set<LinkComp*>& g )
{
  g.clear();
  list_item li;
  forall_items(li, _u_links) {
    LinkComp * x = _u_links.inf(li);
    g.insert(x);
  }
}

// All numbers will be in hex, no separators, input is a null terminated string
u_char * MakeID(const u_char * input)
{
  static u_char rval[22];
  int    pos = 0;
  bool right_nibble = false; /* begins with left */
  
  while (*input) {
    int hexx = isdigit(*input) ? *input - '0' : tolower(*input) - 'a' + 10;
    if (right_nibble == false) {
      rval[pos] = (hexx << 4);
      right_nibble = true;
    } else {
      rval[pos++] |= hexx;
      right_nibble = false;
    }
    input++;
  }
  return rval;
}


void DefaultLinkConfig( void )
{
  ds_String *oc3 = new ds_String("OC3");
  ds_String *oc12 = new ds_String("OC12");
  ds_String *oc48 = new ds_String("OC48");

  if (theConfigurator().AggregationToken(oc3) == -1) {
    list< ig_resrc_avail_info * > * raigs3  = 
      new list< ig_resrc_avail_info * > ;
    raigs3->append(  CreateRAIG( OC3 ) );
    LinkConfiguration * linkOC3 = 
      new LinkConfiguration( new ds_String(*oc3),  0, raigs3  );
    ConfiguratorInsert( oc3,  linkOC3  );
  }

  if (theConfigurator().AggregationToken(oc12) == -1) {
    list< ig_resrc_avail_info * > * raigs12 = 
      new list< ig_resrc_avail_info * > ;
    raigs12->append( CreateRAIG( OC12 ) );
    LinkConfiguration * linkOC12 = 
      new LinkConfiguration( new ds_String(*oc12), 0, raigs12 );
    ConfiguratorInsert( oc12, linkOC12 );
  }

  if (theConfigurator().AggregationToken(oc48) == -1) {
    list< ig_resrc_avail_info * > * raigs48 = 
      new list< ig_resrc_avail_info * > ;
    raigs48->append( CreateRAIG( OC48 ) );
    LinkConfiguration * linkOC48 = 
      new LinkConfiguration( new ds_String(*oc48), 0, raigs48 );
    ConfiguratorInsert( oc48, linkOC48 );
  }

  delete oc3;
  delete oc12;
  delete oc48;
}
