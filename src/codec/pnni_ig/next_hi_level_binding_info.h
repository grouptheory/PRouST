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
#ifndef __NXT_HILVL_BIND_INFO_H__
#define __NXT_HILVL_BIND_INFO_H__

#ifndef LINT
static char const _nxt_hilvl_bind_info_h_rcsid_[] =
"$Id: next_hi_level_binding_info.h,v 1.4 1998/09/23 18:57:26 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>

class NodeID;
class PeerID;
class Addr;

#define BINDING_INFO_HEADER 84;
// type(2), Len(2), ParentLGNID(22), ParentLGNAESA(20),
// ParentPGID(14), NidofParentPGL(22), Reserved(2)

/**  The Next Higher Level Binding Information is included in the Nodal Information Group,
     sent by peer group leaders if there is a higher level.
 */
class ig_next_hi_level_binding_info : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_next_hi_level_binding_info & ig);
  friend istream & operator >> (istream & is, ig_next_hi_level_binding_info & ig);
public:

  /// Constructor.
  ig_next_hi_level_binding_info(u_char * = 0, u_char * = 0, u_char * = 0, u_char * = 0);
  ig_next_hi_level_binding_info(u_char * parent_nid, char * parent_pgl);
  ig_next_hi_level_binding_info(NodeID *, Addr *, PeerID *, NodeID *);
  ig_next_hi_level_binding_info(const ig_next_hi_level_binding_info & rhs);

  /// Destructor.
  virtual ~ig_next_hi_level_binding_info();

  /**@name Methods for encoding/decoding the IG.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}

  const NodeID * GetParentLogGroupID(void) const;
  void SetParentLogGroupID(NodeID *parent);

  const Addr   * GetParentLGNATMAddr(void) const;
  void SetParentLGNATMAddr(Addr *parent);

  const PeerID * GetParentPeerGroupID(void) const;
  void SetParentPeerGroupID(PeerID *parent);

  const NodeID * GetNodeIDPGLParentPG(void) const;
  void SetNodeIDPGLParentPG(NodeID *parent);

  virtual InfoGroup * copy(void);
  void size(int & length);

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  u_char _parent_log_group_id [22];
  u_char _parent_lgn_atm_esa [20];
  u_char _parent_peer_group_id [14];
  u_char _node_id_pgl_parent_pgroup [22];
  // 2 bytes reserved
};

#endif // __NXT_HILVL_BIND_INFO_H__
