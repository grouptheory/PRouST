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
#ifndef __DBKEY_H__
#define __DBKEY_H__

#ifndef LINT
static char const _DBKey_h_rcsid_[] =
"$Id: DBKey.h,v 1.22 1999/01/08 15:05:02 mountcas Exp $";
#endif

#include <codec/pnni_ig/ig.h>
#include <iostream.h>

class NodeID;
class PeerID;
class Kernel;

class PTSPPkt;
class DatabaseSumPkt;
class PTSEReqPkt;

class DBKey {
  friend int operator == (const DBKey & lhs, const DBKey & rhs);
  friend int operator >  (const DBKey & lhs, const DBKey & rhs);
  friend int operator <  (const DBKey & lhs, const DBKey & rhs);
  friend ostream & operator << (ostream & os, const DBKey & him);
  friend int compare(DBKey const & lhs, DBKey const & rhs);
public:

#ifndef ExpiredAge
  enum dbk_internal { ExpiredAge = 1 };
#endif

  DBKey(InfoGroup::ig_id type = InfoGroup::ig_unknown_id, 
	const u_char * orig = 0, int id = 0, int seq = 0, 
	const u_char * peer = 0, short time = 0);
  DBKey(InfoGroup::ig_id type, const NodeID * orig, int id, int seq, const PeerID * pid, short time);
  DBKey(const DBKey & other);
  ~DBKey( );

  DBKey operator = (const DBKey him);

  const u_char  * GetOrigin(void) const;
  const u_char  * GetPeer(void) const;
  const int       GetSN(void) const;
  const int       GetID(void) const;
  const short int GetTTL(void) const;
  const InfoGroup::ig_id GetType(void) const;

  int GetOrigin(u_char * rval) const;
  int GetPeer(u_char * rval) const;
  int GetOrigin(NodeID & rval) const;
  int GetPeer(PeerID & rval) const;
  int GetSeqNum(int & rval) const;
  int GetID(int & rval) const;
  int GetTimeToLive(int & rval) const;
  int GetType(InfoGroup::ig_id & rval) const;

  bool has_same_originator(const DBKey & other) const;
  bool has_same_originator(const u_char * origin) const;
  bool has_same_originator(const NodeID * origin) const;

  void Print(char * out) const;

private:

  u_char           _originator_id[22];
  u_char           _peergroup_id[14];
  u_int            _id;
  u_int            _seq_num;
  short int        _rem_life_time;
  InfoGroup::ig_id _type;
};

#endif // __DBKEY_H__

