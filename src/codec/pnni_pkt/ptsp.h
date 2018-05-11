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
#ifndef __PTSP_H__
#define __PTSP_H__

#ifndef LINT
static char const _ptsp_h_rcsid_[] =
"$Id: ptsp.h,v 1.19 1998/12/15 15:57:54 mountcas Exp $";
#endif

#include <codec/pnni_pkt/pnni_pkt.h>
#include <DS/containers/list.h>

class ig_ptse;

#define FIXED_PTSP_HEADER 44
 // PnniHeader(8), Onid(22), Opgid(14)

/** PNNI Topology State Packets are used to distribute info throughout a
    peer group.
 */
class PTSPPkt : public PNNIPkt {
  friend ostream & operator << (ostream & os, const PTSPPkt & pkt);
  friend bool operator == ( const PTSPPkt & lhs, const PTSPPkt & rhs);
  friend int compare(PTSPPkt *const & lhs, PTSPPkt *const & rhs);
public:

  /// Constructor takes an Originating node ID and Originating node's peer group ID.
  PTSPPkt(const u_char * onid = 0, const u_char * opid = 0);

  /// Destructor.
  virtual ~PTSPPkt();

  /**@name Methods for encoding/decoding PTSP packets. */
  //@{

    /// Encode.
  u_char *encode(u_char * buffer, int & len);

    /// Decode.
  errmsg *decode(const u_char * buffer);
  //@}

  /**@name Methods for manipulating the list of PTSE's */
  //@{

    /// Add a PTSE to the list.
  bool AddPTSE(ig_ptse *p);

    /// Remove a PTSE from the list with the same attributes as p.
  bool RemPTSE(ig_ptse *p);
  //@}

  const u_char * GetOID(void) const;
  const u_char * GetPGID(void) const;
  const list<ig_ptse *> & GetElements(void) const;
  void                    AddElement(ig_ptse * ptse);
  void                    RemElement(ig_ptse * ptse);
  void ClearElements();
  void ClearExpiredPtses();
  int  GetSize(void);

  virtual PNNIPkt * copy(void) const;
  virtual PNNIPkt * copy(bool nondeep) const;
  void size(int & length);

protected:

  /// Method for printing PTSPPkt's data members to os.
  bool PrintSpecific(ostream & os) const;
  
  u_char _orig_node_id [22];
  u_char _orig_peer_group_id [14];
  list<ig_ptse *> _elements;
};

#endif // __PTSP_H__
