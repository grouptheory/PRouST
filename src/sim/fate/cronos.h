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
#ifndef __CRONOS_H__
#define __CRONOS_H__

#ifndef LINT
static char const _cronos_h_rcsid_[] =
"$Id: cronos.h,v 1.49 1999/02/26 15:28:56 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/kernel/Handlers.h>
#include <FW/kernel/KernelTime.h>
#include <FW/kernel/SimEntity.h>
#include <FW/basics/Visitor.h>

#include <DS/containers/set.h>
#include <DS/containers/list.h>

#ifndef __FAKE__
#include <FW/basics/Conduit.h>
#include <codec/pnni_ig/id.h>
#include <fsm/visitors/LinkVisitor.h>
#include <sim/port/LinkStateWrapper.h>
#include <sim/switch/FateTerm.h>
class NodeID;
#endif

class ifstream;
class ofstream;
class ds_String;
class timeseq;

int compare(void *const & x, void *const & y);
extern void DefaultLinkConfig( void );

class timeseqTimer : public TimerHandler { //----------------------------------
public:

  timeseqTimer(timeseq* ts, double t);
  virtual ~timeseqTimer();
  void Callback(void);

  void AttachProcedure(void* f);

private:

  timeseq    * _ts;
  list<void *> _funcs;
};

//------------------------------------------------
class timeseq : public SimEntity { // State { 
public:

  enum tstype { unknown, periodic, uniform, poisson, table };

  //
  // If filename is not zero, then the constructor tries to open the
  // file.  If the file exists, the constructor opens it for reading
  // and the timeseq gets all of its timer intervals ("blips") from
  // the file.  If the file does not exist, the constructor opens it
  // for writing and writes each blip to the file.
  //
  // If filename is zero, then the timeseq always calculates its blips
  // and never tries to read them from or write them to a file.
  //
  timeseq(tstype t, char *filename = 0);
  virtual ~timeseq();

  void SetStart(double start);
  void SetEnd(double end);
  virtual void SetParam(double one);
  virtual void SetParam(double one, double two);
  virtual void SetParam(double one, double two, double three);
  virtual void SetParam(double one, double two, double three, double four);

  //
  // A blip is the length of a timer.  When the timeseq's timer
  // expires, it calls NextBlip() to determine the next timer
  // interval.  If the timeseq is reading its blips from a file, then
  // NextBlip() reads the next one and returns it.  If the timeseq is
  // writing its blips to a file or if it's not using any file,
  // NextBlip() calls CalculateBlip() to determine the next timer
  // interval.
  //
  double NextBlip(double last_expiretime);

  // When the timeseq is directed to write its blips to a file
  // it calls CalculateBlip() to determine the next blip.
  //
  // CalculateBlip() is not called when the timeseq reads
  // its blips from a file.
  virtual double CalculateBlip(double last_expiretime) = 0;

  virtual void Boot(void);

  virtual void    Interrupt(SimEvent* e);

  void AttachProcedure(void* f);

  double Rand(double max);

protected:

  tstype _t;
  double _start, 
         _end;
  timeseqTimer * _timer;

  // Used when reading times from a file or writing times to a file
  ifstream *_infile;
  ofstream *_outfile;
};

class periodic_ts : public timeseq { //----------------------------------------
public:

  periodic_ts(char *filename = 0);
  virtual ~periodic_ts();
  virtual void SetParam(double one);
  virtual double CalculateBlip(double last_expiretime);

private:

  double _period;
};

class uniform_ts : public timeseq { //-----------------------------------------
public:

  uniform_ts(char *filename = 0);
  virtual ~uniform_ts();
  virtual void SetParam(double one);
  virtual double CalculateBlip(double last_expiretime);

private:

  double _interval;
};

class poisson_ts : public timeseq { //-----------------------------------------
public:

  poisson_ts(char *filename = 0);
  virtual ~poisson_ts();
  virtual void SetParam(double one);
  virtual double CalculateBlip(double last_expiretime);

private:

  double _intensity;
};

class explicit_ts : public timeseq { //----------------------------------------
public:

  explicit_ts(char *filename = 0);
  virtual ~explicit_ts();
  virtual void AddTick(double one);
  virtual double CalculateBlip(double last_expiretime);

  virtual void Boot(void);

private:

  list<double> _blips;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class SwitchComp;
class LinkComp;

class GenComp {
public:

  enum comptype { unknown=0, switchtype, linktype };
  GenComp(comptype t);
  virtual ~GenComp();

  virtual void Associated(set<SwitchComp*>* g);
  virtual void Associated(set<LinkComp*>* g);
  virtual void Expelled(set<SwitchComp*>* g);
  virtual void Expelled(set<LinkComp*>* g);  

protected:

  comptype _t;
};

class LinkComp;

class SwitchComp : public GenComp { //-----------------------------------------
public:

  SwitchComp(ds_String *key, int id, ds_String *name = 0, NodeID *myNode =0);
  virtual ~SwitchComp();

  int TakeFreePort(LinkComp* lk);
  void ReturnPort(int p);

  void AcceptVisitor( Visitor * v, bool exp = true, double duration = -1.0 );
  
  virtual void Associated(set<SwitchComp*>* g);
  virtual void Expelled(set<SwitchComp*>* g);

private:

  int         _ports;
  bool      * _port_avail;
  LinkComp ** _links;

  set<set<SwitchComp *> *> _groups;

#ifndef __FAKE__
  bool birth_switch(ds_String *key, int id, ds_String * name = 0);
  bool death_switch(void);

  NodeID   * _myPreferredPeerGroupLeader;
  NodeID   * _myNodeID;
  Conduit  * _the_conduit;
  Conduit  * _the_terminal;
  FateTerm * _terminal;

public:

  Conduit * getConduit(void) const;
  NodeID  * getNodeID(void)  const;
  const char * getName(void) const;
#endif
};

class LinkComp : public GenComp { //-------------------------------------------
public:

  // Default for fbKey is "OC3".
  LinkComp(SwitchComp * s1, SwitchComp * s2, 
	   ds_String *fbKey = 0, ds_String *bbKey = 0);
  virtual ~LinkComp();

  bool IsValid(void);

  virtual void Associated(set<LinkComp *> * g);
  virtual void Expelled(set<LinkComp *> * g);  

#ifndef __FAKE__
  void Disable(void);
  void Enable(void);

  const char * getName(void) const;
#endif

private:

  SwitchComp * _s1;
  SwitchComp * _s2;
  int          _p1, 
               _p2;
  bool         _isvalid;

  set<set<LinkComp *> *> _groups;

#ifndef __FAKE__
  bool birth_link(SwitchComp * c1, int port1, 
		  SwitchComp * c2, int port2, 
		  ds_String *fbKey, ds_String *bbKey);
  bool death_link(void);

  LinkStateWrapper * _the_link;
#endif
};

class CompMaker { //-----------------------------------------------------------
  friend u_char * MakeID(const u_char *);
public:

  CompMaker(void);
  virtual ~CompMaker(void);

  SwitchComp * MakeSwitch(char *key, char *peerGroup = 0);
  // Default for fbKey is "OC3"
  LinkComp   * MakeLink(SwitchComp * s1, SwitchComp * s2, 
			char *fbKey = 0, char *bbKey = 0);
  Visitor    * MakeVisitor(const char * type, SwitchComp * src, 
			   SwitchComp * dest, int, char *, long opt = 0);
  void         MakeConnection(SwitchComp * src, SwitchComp * dest, int bandwidth, 
			      bool exp, double duration);
  void         MakeConnection(const NodeID * src, const NodeID * dest, int bandwidth, 
			      bool exp, double duration);

  void KillLink(LinkComp*& lk);
  void KillSwitch(SwitchComp*& sw);

  void AddToGroup(set<SwitchComp*>* grp, SwitchComp* sw);
  void RemFromGroup(set<SwitchComp*>* grp, SwitchComp* sw);
  void ClearGroup(set<SwitchComp*>* grp);

  void AddToGroup(set<LinkComp*>* grp, LinkComp* lk);
  void RemFromGroup(set<LinkComp*>* grp, LinkComp* lk);
  void ClearGroup(set<LinkComp*>* grp);

  SwitchComp * ChooseSwitch( set<SwitchComp*>* g );
  LinkComp   * ChooseLink( set<LinkComp*>* g );

  bool Toss(double x);

  void Snap(const char * filename);

  void UniverseOfSwitches( set<SwitchComp*>& g );
  void UniverseOfLinks( set<LinkComp*>& g );
  
private:

  int _next_sw_id;
  int _num_switches;
  int _num_links;

  // _peerGroups counts the nodes in each peer group.
  // There is a limit of 255 nodes per peer group.
  dictionary<PeerID *, int> _peerGroups;

  set<SwitchComp *>       _u_switches;
  set<LinkComp *>         _u_links;
};

int compare(LinkComp *const & x, LinkComp *const & y);
int compare(set<SwitchComp *> *const & x, set<SwitchComp *> *const & y);
int compare(SwitchComp *const & x, SwitchComp *const & y);
int compare(set<LinkComp *> *const & x, set<LinkComp *> *const & y);

#endif // __CRONOS_H__
