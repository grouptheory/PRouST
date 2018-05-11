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
#ifndef __NODAL_PTSE_SUMMARY_H__
#define __NODAL_PTSE_SUMMARY_H__

#ifndef LINT
static char const _nodal_ptse_summary_h_rcsid_[] =
"$Id: nodal_ptse_summary.h,v 1.12 1998/09/23 19:04:04 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <DS/containers/list.h>

class DBKey;
class Database;
class NodeID;
class PeerID;

#define DS_SUMMARY_HEADER 44 //type(2), len(2), onid(22), opgid(14), reserve(2)
                             // SummaryCount(2)
#define DS_SUMMARY 16 // ptsetype(2), res(2), ptseid(4), PtseSeqNum(4),
                      // PtseCheck(2), PtseRemLife(2)

// Internal container class.
struct SumContainer {
  SumContainer(sh_int type, int id, int seq, sh_int check, sh_int life);
  ~SumContainer() { }

  int equals(const SumContainer & rhs) { return (_ptse_type == rhs._ptse_type &&
						 _ptse_id == rhs._ptse_id &&
						 _ptse_seq == rhs._ptse_seq &&
						 _ptse_checksum == rhs._ptse_checksum &&
						 _ptse_rem_life == rhs._ptse_rem_life); }
  friend int compare(SumContainer * const &, SumContainer * const &);
  
  sh_int _ptse_type;
  int    _ptse_id;
  int    _ptse_seq;
  sh_int _ptse_checksum;
  sh_int _ptse_rem_life;
};

/** A Nodal PTSE Summary IG is included in Database Summary Packet, for
    each set of PTSEs in the topology database.
 */
class ig_nodal_ptse_summary : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_nodal_ptse_summary & ig);
  friend istream & operator >> (istream & is, ig_nodal_ptse_summary & ig);
  friend int compare(ig_nodal_ptse_summary *const &, ig_nodal_ptse_summary *const &);
  friend class DBKey;
  friend class Database;
public:

  /// Constructor.
  ig_nodal_ptse_summary(u_char * onode = 0, u_char * opeer = 0);
  ig_nodal_ptse_summary(NodeID * onode, PeerID * opeer);
  ig_nodal_ptse_summary(const ig_nodal_ptse_summary & rhs);

  /// Destructor.
  virtual ~ig_nodal_ptse_summary();

  /**@name Methods for encoding/decoding the summary.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);

    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}
  
  /**@name Methods for adding summaries to the list.
   */
  //@{
    /// Add a summary.
  void AddSum(sh_int t, int i, int s, sh_int c, sh_int l);

    /// Remove a summary.
  bool RemSum(sh_int t, int i, int s, sh_int c, sh_int l);
  //@}

  int equals(ig_nodal_ptse_summary & rhs);

  const list<SumContainer *> & GetContainers(void) const;

  const NodeID * GetOrigin(void) const;
  const PeerID * GetPeer(void) const;
  const sh_int   GetPTSESumCnt(void) const;

  virtual InfoGroup * copy(void);
  void size(int & length);

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  u_char _originating_node [22];
  u_char _orig_peer_group [14];
  sh_int _ptse_summary_count;
  list<SumContainer *> _list;
};

#endif // __NODAL_PTSE_SUMMARY_H__
