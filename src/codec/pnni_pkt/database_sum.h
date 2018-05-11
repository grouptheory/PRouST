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
#ifndef __DATABASE_SUM_H__
#define __DATABASE_SUM_H__

#ifndef LINT
static char const _database_sum_h_rcsid_[] =
"$Id: database_sum.h,v 1.11 1998/12/15 15:55:58 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <codec/pnni_pkt/pnni_pkt.h>

class ig_nodal_ptse_summary;

#define FIXED_SUMMARY_HEADER 16 // pnni header(8), flags(2), reserved(2),
                                // DSseqnum(4)

/** Database Summary Packets are used during the initial database exchange
    process between two neighboring peers.
 */
class DatabaseSumPkt : public PNNIPkt {
  friend ostream & operator << (ostream & os, const DatabaseSumPkt & pkt);
public:

  // Bitfields
  enum dsflags {
    init_bit = 0x8000,
    more_bit = 0x4000,
    master_bit = 0x2000,
    // rest are reserved
  };

  /// Constructor takes sequence number.
  DatabaseSumPkt(int seq = 0);

  /// Destructor.
  virtual ~DatabaseSumPkt();

  /**@name Methods for encoding/decoding database summaries. */
  //@{

    /// Encode.
  u_char *encode(u_char * buffer, int & len);

    /// Decode.
  errmsg *decode(const u_char * buffer);
  //@}

  /**@name Methods for maintaining the list of summaries. */
  //@{

    /// Add a nodal PTSE summary.
  void AddNodalPTSESum(ig_nodal_ptse_summary *);

    /// Remove a nodal PTSE summary.
  bool RemNodalPTSESum(ig_nodal_ptse_summary *);
  //@}

  const list<ig_nodal_ptse_summary *> & GetNodalSummaries(void) const;

  /**@name Bit operations on flags */
  //@{

    /// Set.
  void SetBit(dsflags f);

    /// Is-set.
  bool IsSet(dsflags f) const;

    /// Unset.
  void RemBit(dsflags f);
  //@}

  const int GetSequenceNum(void) const;
  void      SetSequenceNum(int sn);

  virtual PNNIPkt * copy(void) const;
  void size(int & length);

protected:

  /// Print method for database summary data.
  bool PrintSpecific(ostream & os) const;
  
  sh_int   _flags;
  // sh_int reserved
  int      _ds_seq_num;

  list<ig_nodal_ptse_summary *> _list;
};

#endif // __DATABASE_SUM_H__
