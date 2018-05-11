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
#ifndef __PTSE_ACK_H__
#define __PTSE_ACK_H__

#ifndef LINT
static char const _ptse_ack_h_rcsid_[] =
"$Id: ptse_ack.h,v 1.10 1998/12/15 15:56:50 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <codec/pnni_pkt/pnni_pkt.h>

class ig_nodal_ptse_ack;
class ig_ptse;

/** PNNI Topology State Element Acknowledgment Packets are used to
    acknowledge receipt of PTSEs from a neighbor node.
 */
class PTSEAckPkt : public PNNIPkt {
  friend ostream & operator << (ostream & os, const PTSEAckPkt & pkt);
public:

  /// Constructor.
  PTSEAckPkt();

  /// Destructor.
  virtual ~PTSEAckPkt();

  /**@name Methods for encoding/decoding PTSE acknowledgement packets. */
  //@{

    /// Encode.
  u_char *encode(u_char * buffer, int & len);

    /// Decode.
  errmsg *decode(const u_char * buffer);
  //@}

  /**@name Methods for manipulating the list of PTSE acknowledgements */
  //@{

    /// Adds a nodal PTSE ack to the list.
  void AddNodalPTSEAck(ig_nodal_ptse_ack *);

   /// Add a ptse ack for a ptse received, depending on 
   /// nodal_ptse_ack_header is already present or not.
  void AddNodalPTSEAck(const u_char *onid, ig_ptse *lPtse);


    /// Removes the nodal PTSE ack from the list with the same attributes as n.
  bool RemNodalPTSEAck(ig_nodal_ptse_ack *n);
  //@}

  /// Search for a Ack header based on NodeID
  ig_nodal_ptse_ack * Search(const u_char  * r_orig_node_id);

  const list<ig_nodal_ptse_ack *> & GetNodalPTSEAcks(void) const;

  virtual PNNIPkt * copy(void) const;

  void  size(int & length);

protected:

  /// Method for printing list of nodal PTSE acks.
  bool PrintSpecific(ostream & os) const;
  
  list<ig_nodal_ptse_ack *> _list;
};

#endif // __PTSE_ACK_H__
