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
#ifndef __EXT_REACH_ATM_ADDR_H__
#define __EXT_REACH_ATM_ADDR_H__

#ifndef LINT
static char const _ext_reach_atm_addr_h_rcsid_[] =
"$Id: external_reachable_atm_addr.h,v 1.8 1998/09/24 13:35:32 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <DS/containers/list.h>

class ig_resrc_avail_info;
class ig_transit_net_id;
class ig_internal_reachable_atm_addr;

#define EXT_REACH_ADDR_HEADER 16
// type (2), length (2), Flags(2), Reserved (2), PortID (4)
// AdScope (1), Ail (1), Aic (2)

// Internal Container class
struct AddrPrefixCont {

  AddrPrefixCont(u_char plen, u_char * reach, u_char ail);
  AddrPrefixCont(const AddrPrefixCont & rhs);
  ~AddrPrefixCont() { delete [] _reachable_addr_prefix; }

  bool PrintSpecific(ostream & os);
  int equals(AddrPrefixCont & rhs);
  u_char *encode(u_char * buffer, int & buflen);
  friend int compare(AddrPrefixCont * const &, AddrPrefixCont * const &);
  void size(int & length);

  u_char  _prefix_len;
  u_char *_reachable_addr_prefix;
  u_char  _pad_len;
  u_char  _ail;
};

/** The exterior reachable ATM addresses information group is used to advertise
    the addresses of ATM endpoints that can be reached by passing through
    exterior networks, that donot participate in PNNI routing.  This information
    group can appear multiple times and in multiple different PTSEs, listing
    different sets of exterior reachable ATM addresses each time.
 */
class ig_external_reachable_atm_addr : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_external_reachable_atm_addr & ig);
  friend istream & operator >> (istream & is, ig_external_reachable_atm_addr & ig);
  friend class ig_internal_reachable_atm_addr;

public:

  enum flags {
    vp_compat_bit = 0x8000,
  };

  /// Constructor.
  ig_external_reachable_atm_addr(int pid = 0, u_char scope = 0, u_char addrlen = 0);
  ig_external_reachable_atm_addr(const ig_external_reachable_atm_addr & rhs);

  /// Destructor.
  virtual ~ig_external_reachable_atm_addr();

  /**@name Encode/Decode methods.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);

    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}

  /**@name Bitflag operations.
   */
  //@{
    /// Set the specified bit.
  void SetFlag(flags flag) { _flags |= flag; }

    /// Check if specified bit is set.
  bool IsSet(flags flag)   { return (_flags & flag); }

    /// UnSet the specified bit.
  void RemFlag(flags flag) { _flags &= ~flag; }
  //@}

  /**@name Methods for manipulating the list of prefixes.
   */
  //@{
    /// Add a prefix.
  void AddPrefix(u_char plen, u_char *reachable);

    /// Remove a prefix.
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

  const list<ig_transit_net_id *> & GetTransitNetID(void) const;
  void  AddTransitNetID(ig_transit_net_id *p);

  virtual InfoGroup * copy(void);
  void    size(int & length);
  virtual int equals(const InfoGroup * other) const;

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  sh_int  _flags;
  int     _pid;
  u_char  _ad_scope;
  u_char  _addr_info_len;
  sh_int  _addr_info_cnt;
  int _padding;

  // Repeat addr_info_cnt times
  list<struct AddrPrefixCont *> _prefixes;
  list<ig_resrc_avail_info *> _outgoing_raigs;
  list<ig_resrc_avail_info *> _incoming_raigs;
  list<ig_transit_net_id *>   _transit_net_ids;
};

#endif // __EXT_REACH_ATM_ADDR_H__

