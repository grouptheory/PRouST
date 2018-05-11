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
#ifndef __NODAL_LIST_H__
#define __NODAL_LIST_H__

#ifndef LINT
static char const _nodal_list_h_rcsid_[] =
"$Id: nodal_hierarchy_list.h,v 1.11 1998/09/23 18:59:23 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <DS/containers/list.h>

class NodeID;
class PeerID;
class Addr;

#define NHL_HEADER 12 // type(2), len(2), seqno(4), res(2), levelcount(2)
#define NHL_CONTENT 56 // nhllgnid(22), nhlaesa(20), nhlpgid(14)

// Internal container class.
struct Level {
  friend int operator > (const Level & lhs, const Level & rhs);

  Level(u_char *logical_node_ID, u_char *ATM_end_sys_addr, u_char *peer_group_ID);
  Level(NodeID * nid, Addr * atm, PeerID * pid);
  Level(const Level & level);
  ~Level() { }

  u_char * encode(u_char *buffer, int & buflen);
  int equals(Level & rhs);
  bool PrintSpecific(ostream & os);
  friend int compare(Level * const &, Level * const &);
  Level * copy(void);

  PeerID * GetPID(void);
  NodeID * GetNID(void);
  Addr   * GetATM(void); 

  u_char _log_node_id [22];
  u_char _atm_end_sys_addr [20];
  u_char _peer_group_id [14];
};


/** The Nodal hierarchy list is included in order to allow lower-level neighbor
    nodes to determine their lowest-level common peer group, and also to exchange
    the node ID and ATM End System Addresses of the corresponding higher-level
    neighbor nodes within the common peer group.
 */
class ig_nodal_hierarchy_list : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_nodal_hierarchy_list & ig);
  friend istream & operator >> (istream & is, ig_nodal_hierarchy_list & ig);

public:

  /// Constructor.
  ig_nodal_hierarchy_list(long seq_num = -1);

  /// Copy Constructor.
  ig_nodal_hierarchy_list(const ig_nodal_hierarchy_list & nlist);

  /// Destructor.
  virtual ~ig_nodal_hierarchy_list();

  /**@name Methods for encoding/decoding the list.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char *buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char *buffer);
  //@}

  /**@name Methods for manipulating the list of hierarchical levels.
   */
  //@{
    /// Add a level to the list (and create it).
  void    AddLevel(u_char *log_id, u_char *atm_id, u_char *peer_id);
    /// Add the passed level to the list.
  void    AddLevel(Level * ptr);
    /// Add a level to the list (and create it).
  void    AddLevel(NodeID * nid, Addr * atm, PeerID * pid);
    /// Remove the specified level from the list.
  bool    DelLevel(u_char *log_id, u_char *atm_id, u_char *peer_id);

  bool    IsInLevels(u_char * log_id, u_char * atm_id, u_char * peer_id);
  bool    IsInLevels(NodeID * nid, Addr * atm, PeerID * pid);

    /// Retrieve the list of Levels.
  const list<Level *> & GetLevels(void) const;
  const list<Level *> * ShareLevels(void) const;
  //@}

  const long   GetSequenceNum(void) const;
  const sh_int GetLevelCount(void) const;

  Level * FindCommonPGID(ig_nodal_hierarchy_list *nhl);

  virtual InfoGroup * copy(void);
  void size(int & length);

protected:

  // Can't get rid of an annoying parse error so this is temporary
  inline list<Level *> * NewLev(void) { return new list<Level *>; }

  virtual bool        PrintSpecific(ostream& os);
  virtual bool        ReadSpecific(istream& is);

  long                _seq_num;
  sh_int              _level_count;  // _levels.length()
  list<Level *>     * _levels;
};

#endif // __NODAL_LIST_H__
