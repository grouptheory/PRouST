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
#ifndef __NODAL_PTSE_ACK_H__
#define __NODAL_PTSE_ACK_H__

#ifndef LINT
static char const _nodal_ptse_ack_h_rcsid_[] =
"$Id: nodal_ptse_ack.h,v 1.16 1998/09/23 19:01:23 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <DS/containers/list.h>

/** The Nodal PTSE Acknowledgement is included in the PTSE Acknowledgement packet.
    It acknowledges the receipt of a PTSE from a neighbor node.
 */
class ig_ptse;
class NodeID;

#define ACK_HEADER 28 
// type(2),len(2),NodeID(22),AckCount(2)
#define ACK_CONTAINER 12 
// ptseid(4), ptseseqno(4), ptsechecksum(2), remlifetime(2)

struct AckContainer {
  AckContainer(int, int, sh_int, sh_int);
  ~AckContainer() { }
  
  int equals(const AckContainer & rhs) { return (_ptse_id == rhs._ptse_id &&
						 _ptse_seq == rhs._ptse_seq &&
						 _ptse_checksum == rhs._ptse_checksum &&
						 
						 _ptse_rem_life == rhs._ptse_rem_life); }
  
  friend int compare(AckContainer * const &, AckContainer * const &);
  
  int     _ptse_id;
  int     _ptse_seq;
  u_short _ptse_checksum;
  u_short _ptse_rem_life;
};

int operator == (ig_ptse & lhs, AckContainer & rhs);

class ig_nodal_ptse_ack : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_nodal_ptse_ack & ig);
  friend istream & operator >> (istream & is, ig_nodal_ptse_ack & ig);
  friend int compare(ig_nodal_ptse_ack *const &, ig_nodal_ptse_ack *const &);

public:

  /// Constructor.
  ig_nodal_ptse_ack(NodeID * nid = 0);
  ig_nodal_ptse_ack(const u_char * nid);
  ig_nodal_ptse_ack(const ig_nodal_ptse_ack & rhs);

  /// Destructor.
  virtual ~ig_nodal_ptse_ack();

  /**@name methods for encoding/decoding the IG.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}

  /**@name Manipulation methods for the list of acknowledgements.
   */
  //@{
    /// Add an acknowledgement to the list.
  void AddAck(int id, int seq, sh_int checksum, sh_int rem_life);
    /// Remove an acknowledgment from the list.
  bool RemAck(int id, int seq, sh_int checksum, sh_int rem_life);
  //@}

  int equals(ig_nodal_ptse_ack & rhs);

  bool equals(const u_char * r_orig_node_id);
 
  const NodeID * GetNodeID(void) const;
  const sh_int   GetAckCount(void) const;
  const u_char * GetOID() const;

  const list<AckContainer *> & GetAcks(void) const;

  virtual InfoGroup * copy(void);
  void size(int & length);

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  u_char _node_id [22];
  sh_int _ack_count;
  list<AckContainer *> _list;
};

#endif // __NODAL_PTSE_ACK_H__
