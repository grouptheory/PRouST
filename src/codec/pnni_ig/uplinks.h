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
#ifndef __UPLINKS_H__
#define __UPLINKS_H__

#ifndef LINT
static char const _uplinks_h_rcsid_[] =
"$Id: uplinks.h,v 1.12 1998/09/24 13:46:57 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <DS/containers/list.h>

class NodeID;
class PeerID;
class Addr;

#define FIXED_UPLINKS_CONTENT 72
  // type(2), len(2), flags(2), res(2), rhlnid(22), cpgid(14), lpid(4)
  // aggr(4), aesa_upnode(20)

/** The Uplinks Information Group is used to advertise links to nodes that are
    outside of this peer group.  This IG can appear multiple times and in multiple
    different PTSEs, listing different uplinks each time.  All metrics and attributes
    associated within each uplink must be included in the same uplink IG.
  */
class ig_uplinks : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_uplinks & ig);
  friend istream & operator >> (istream & is, ig_uplinks & ig);

public:

  /// Constructor.
  ig_uplinks(sh_int flags = 0, NodeID * rnid = 0, PeerID * cpid = 0, 
	     int lport = 0, int atok = 0, Addr * aesa = 0);
  ig_uplinks(sh_int flags, u_char * rnid, u_char * cpid, 
	     int lport, int atok, u_char * aesa);
  ig_uplinks(const ig_uplinks & rhs);

  /// Destructor.
  virtual ~ig_uplinks();

  /**@name Methods for encoding/decoding the IG.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}

  void AddIG(InfoGroup * ptr);
  const list<InfoGroup *> & GetIGs(void) const;
  const list<InfoGroup *> * ShareIGs(void) const;

  NodeID * GetRemoteID(void) const;
  PeerID * GetCommonPGID(void) const;
  const int GetLocalPID(void) const;
  const int GetAggTok(void) const;
  Addr   * GetATMAddr(void) const;

  virtual InfoGroup * copy(void);
  void size(int & length);
  virtual int equals(const InfoGroup * other) const;

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  sh_int _flags;
  // reserved sh_int
  u_char _remote_higher_level_node_id [22];
  u_char _common_peer_group_id [14];
  int    _local_port_id;
  int    _aggregation_token;
  u_char _atm_end_sys_address_upnode [20];

  // Contains Outgoing resource availability IGs / Uplink Info Attrib.
  list<InfoGroup *> _list;
};

#endif // __UPLINKS_H__
