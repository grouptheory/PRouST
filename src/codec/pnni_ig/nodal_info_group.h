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
#ifndef __NODAL_INFO_H__
#define __NODAL_INFO_H__

#ifndef LINT
static char const _nodal_info_h_rcsid_[] =
"$Id: nodal_info_group.h,v 1.8 1998/09/23 18:09:16 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>

class NodeID;
class Addr;
class PeerID;
class ig_next_hi_level_binding_info;

#define NODAL_INFO_HEADER 48
// type(2), len(2), ATMesa(20), leadershippriority(1), NodalFlags(1),
// PrefferedPGLID(22)

/** The Nodal Information Group is used to convey general information
    about the node such as its peer group ID and its ATM End System
    Address, and also for all PGL election information.  This
    information group appears once among all PTSEs advertised by this
    node.  */
class ig_nodal_info_group : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_nodal_info_group & ig);
  friend istream & operator >> (istream & is, ig_nodal_info_group & ig);

public:

  enum nflags {
    leader_bit     = 0x80, // 1000 0000
    restrans_bit   = 0x40, // 0100 0000
    nodal_rep_bit  = 0x20, // 0010 0000
    resbranch_bit  = 0x10, // 0001 0000
    ntrans_ele_bit = 0x08, // 0000 1000
  };

  /// Constructor.
  ig_nodal_info_group(Addr * atm_addy = 0, u_char prior = 0, u_char flags = 0,  
		      NodeID * pref_pgl = 0, 
                      ig_next_hi_level_binding_info * higher = 0);

  ig_nodal_info_group(u_char *atm_addy, u_char prior, u_char flags, 
		      u_char *pref_pgl, 
                      ig_next_hi_level_binding_info * higher = 0);

  ig_nodal_info_group(const ig_nodal_info_group & rhs); 
  /// Destructor.
  virtual ~ig_nodal_info_group();

  /**@name Methods for encoding/decoding the IG.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}

  /**@name Bit manipulation methods.
   */
  //@{
    /// Set the specified bit.
  void SetFlag(nflags flg) { _nodal_flags |= flg; }
    /// Check to see if the specified bit is set.
  bool IsSet(nflags flg)   { return (_nodal_flags & flg); }
    /// Un-Set the specified bit.
  void RemFlag(nflags flg) { _nodal_flags &= ~flg; }
  //@}

  const ig_next_hi_level_binding_info * GetNextHigherLevel(void) const;
  void  SetNextHigherLevel(ig_next_hi_level_binding_info * nhl);

  const Addr   * GetATMAddress(void) const; 
  const u_char   GetLeadershipPriority(void) const;
  const u_char   GetNodalFlags(void) const;
  const NodeID * GetPreferredPGL(void) const;

  int GetATM(Addr * rval) const;
  int GetPreferredPGL(NodeID & rval) const;

  virtual InfoGroup * copy(void);
  void    size(int & length);

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  u_char _atm_address [20];
  u_char _leadership_priority;
  u_char _nodal_flags;
  u_char _pref_peer_group_leader [22];
  
  ig_next_hi_level_binding_info * _next_higher_lvl;
};

#endif // __NODAL_INFO_H__
