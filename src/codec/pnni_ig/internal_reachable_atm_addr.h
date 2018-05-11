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
#ifndef __INT_REACH_ATM_ADDR_H__
#define __INT_REACH_ATM_ADDR_H__

#ifndef LINT
static char const _int_reach_atm_addr_h_rcsid_[] =
"$Id: internal_reachable_atm_addr.h,v 1.5 1998/09/23 18:50:13 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <codec/pnni_ig/external_reachable_atm_addr.h>

class ig_resrc_avail_info;

#define INT_REACH_HEADER 16 // type(2), len(2), Flag(2), Res(2), pid(4),
                           // AdScope(1), ail(1), aic(2)

/** The internal reachable ATM addresses information group is used to advertise
    the addresses of directly attached ATM endpoints.  This information can appear
    multiple times and in multiple different PTSEs, listing different sets of internal
    reachable ATM addresses each time.
 */
class ig_internal_reachable_atm_addr : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_internal_reachable_atm_addr & ig);
  friend istream & operator >> (istream & is, ig_internal_reachable_atm_addr & ig);

public:

  enum flags {
    vp_compat_bit = 0x8000,
  };

  /// Constructor.
  ig_internal_reachable_atm_addr(int pid = 0, u_char ad_scope = 0, u_char addrnfolen = 0);

  /// Constructor
  ig_internal_reachable_atm_addr(const ig_internal_reachable_atm_addr & rhs);

  /// Destructor.
  virtual ~ig_internal_reachable_atm_addr();

  /**@name Methods for encoding/decoding the IG.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}

  /**@name Accessor methods.
   */
  //@{
    /// Set the specified bit.
  void SetFlag(flags flag) { _flags |= flag; }
    /// Check to see if the specified bit is set.
  bool IsSet(flags flag)   { return (_flags & flag); }
    /// Un-Set the specified bit.
  void RemFlag(flags flag) { _flags &= ~flag; }
  //@}

  /**@name Manipluation methods for the list of prefixes.
   */
  //@{
    /// Add a prefix to the list.
  void AddPrefix(u_char plen, u_char *reachable);
    /// Remove a prefix from the list.
  bool RemPrefix(u_char plen, u_char *reachable);
  //@}

  const list<struct AddrPrefixCont *> & GetPrefixes(void) const;

  const int    GetPID(void) const;
  const u_char GetAddrScope(void) const;
  const u_char GetAddrInfoLen(void) const;
  const sh_int GetAddrInfoCnt(void) const;

  const list<ig_resrc_avail_info *> & GetOutGoingRAIGS(void) const;
  const list<ig_resrc_avail_info *> & GetInComingRAIGS(void) const;
  void  AddOutGoingRAIG(ig_resrc_avail_info *p);
  void  AddInComingRAIG(ig_resrc_avail_info *p);

  virtual InfoGroup * copy(void);
  void size(int & length);
  virtual int equals(const InfoGroup * other) const;

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  sh_int  _flags;
  int     _pid;
  u_char  _ad_scope;
  u_char  _addr_info_len;
  sh_int  _addr_info_cnt;
  // Repeat addr_info_cnt times
  list<struct AddrPrefixCont *> _prefixes;
  int _padding;
  list<ig_resrc_avail_info *> _outgoing_raigs;
  list<ig_resrc_avail_info *> _incoming_raigs;
};

#endif // __INT_REACH_ATM_ADDR_H__
