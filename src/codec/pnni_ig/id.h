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
#ifndef __ID_H__
#define __ID_H__

#ifndef LINT
static char const _id_h_rcsid_[] =
"$Id: id.h,v 1.58 1999/02/16 21:41:50 talmage Exp $";
#endif

#include <iostream.h>
#include <common/cprototypes.h>

// compare two byte streams at the bit level
int bitcmp (const unsigned char *lhs, const unsigned char *rhs, int bits);
// return the number of bits that are the same in two bit streams 
int bits_equal (const unsigned char *lhs, const unsigned char *rhs,
		int max_bits);
// zero from start_bit to last_bit - 1
void bits_zero(unsigned char *string, int start_bit, int last_bit);
// print a string as 1's and 0's for len bits
void bit_print(unsigned char *string, int len);

class Addr;

// Format   Level:: 0x47 0x05 ...
class PeerID {
  friend int compare(PeerID const &, PeerID const &);
  friend int compare(PeerID *const &, PeerID *const &);
  friend ostream & operator << (ostream & os, const PeerID & rhs);
public:

  PeerID(const u_char level, const u_char * prefix);
  PeerID(const u_char * pgid);
  PeerID(const char * string);
  PeerID(const PeerID & rhs);
  ~PeerID();

  int IsZero(void) const;
  int operator == (const PeerID & pgid) const;  
  int equals(const PeerID * rhs) const;
  int equals(const u_char * rhs) const;
  int PGIDcmp(const PeerID * p2) const;

  PeerID * copy(void) const;

  const u_char * GetPGID(void) const;
  const u_char * GetPID(void) const; 

  int GetPGID(u_char * rval) const;

  const u_char   GetLevel(void) const;
  void           SetLevel(int lev);

  const char *   Print(void) const;
  static int     GetRefcnt(void) { return _refcnt; }

protected:

  static int _refcnt;
  u_char     _id[14];
  char       _string[32];
};

typedef class PeerID PeerGroupID;

// Format   Level:Child Level: 0x47 0x05 ...
class NodeID {
  friend ostream & operator << (ostream & os, const NodeID & rhs);
  friend istream & operator >> (istream & is, NodeID & rhs);
  friend int compare(NodeID const &, NodeID const &);
  friend int compare(NodeID * const &, NodeID * const &);
  friend int compare(NodeID const * const &, NodeID const * const &);
  friend void abbreviatedPrint(char * str, const NodeID * node, bool dots);
public:

  NodeID(const u_char * nid = 0);
  NodeID(const u_char level, const u_char child_level, const NodeID * base_nodeid);
  NodeID(const u_char level, const u_char * pgid, const u_char * esi, const u_char sel = 0);
  NodeID(const char * string);
  // This pulls the level from the selector byte of the ATM Addr
  NodeID(const Addr * atmaddr, const PeerID * pgid = 0);

  NodeID(const NodeID & rhs);

  ~NodeID();

  int  IsZero(void) const;
  bool Is_Me_At_Some_Level( const NodeID * o ) const;

  int operator == (const NodeID & rhs) const;
  int operator <  (const NodeID & rhs) const;
  int operator >  (const NodeID & rhs) const;

  int equals(const NodeID * rhs) const;
  int equals(const Addr * rhs) const;
  int NIDcmp(const NodeID * n2) const;
  // Used to determine if a logical node id is equal to a physical node id
  bool prefixes(NodeID * rhs);

  NodeID * copy(void) const;

  PeerID * GetChildPeerGroup(void) const;
  PeerID * GetPeerGroup(void) const;

  int GetChildPeerGroup(PeerID & rval) const;
  int GetPeerGroup(PeerID & rval) const;

  const u_char GetLevel(void) const;
  void SetLevel(u_char level);

  const u_char GetChildLevel(void) const;
  void SetChildLevel(u_char level);

  const u_char GetSel(void) const;
        u_char * GetESI(void) const;
  const u_char * GetNID(void) const;
  const u_char * GetATM(void) const;
  // This places the level byte into the Addr's selector byte
        Addr   * GetAddr(void) const;
  const u_char * GetPID(void) const;

  int GetNID(u_char * rval) const;
  int GetATM(u_char * rval) const;
  int GetPID(u_char * rval) const;
  int GetESI(u_char * rval) const;
  int GetAddr(Addr * rval) const;

  static int GetRefcnt(void) { return _refcnt; }

  const char *   Print(void) const;

protected:

  static int _refcnt;
  u_char     _id[22];
  char       _string[50];
};

#endif // __ID_H__
