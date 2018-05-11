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
#ifndef __HORIZON_LINKS_H__
#define __HORIZON_LINKS_H__

#ifndef LINT
static char const _horizon_links_h_rcsid_[] =
"$Id: horizontal_links.h,v 1.7 1998/09/23 18:47:28 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <DS/containers/list.h>

/** The horizontal links IG is used to advertise links to other nodes within the
    same peer group.  This information group can appear multiple times and in 
    multiple different PTSEs, listing different horizontal links each time.  All
    metrics and attributes associated with each horizontal link must be included
    in the same horizontal link information group.
 */
class ig_resrc_avail_info;
class NodeID;

#define HLINK_HEADER 40
// type(2), length(2),flags(2),rnid(22),rpid(4), lpid(4), agg(4)

class ig_horizontal_links : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_horizontal_links & ig);
  friend istream & operator >> (istream & is, ig_horizontal_links & ig);

public:

  /// Constructor.
  ig_horizontal_links(sh_int flags = 0, NodeID *rnode = 0, int rport = 0, int lport = 0, int atok = 0);
  ig_horizontal_links(sh_int flags, u_char *rnode, int rport, int lport, int atok);

  ig_horizontal_links(const ig_horizontal_links & rhs);

  /// Destructor.
  virtual ~ig_horizontal_links();

  /**@name Methods for encoding/decoding horizontal links.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);

    /// Decode.
  virtual errmsg *decode(u_char * buffer);

  //@}

  void  AddRAIG(ig_resrc_avail_info * ptr);
  const list<ig_resrc_avail_info *> & GetRAIGS(void) const;
  const NodeID * GetRemoteID(void) const;
  const int      GetRemotePID(void) const;
  const int      GetLocalPID(void) const;
  const int      GetAggTok(void) const;

  int GetRemoteID(NodeID & rhs) const;

  virtual InfoGroup * copy(void);
  void    size(int & length);
  virtual int equals(const InfoGroup * other) const;

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  sh_int _flags;
  u_char _remote_node_id [22];
  int    _remote_port_id;
  int    _local_port_id;
  int    _aggregation_token;

  list<ig_resrc_avail_info *> _outgoing_raigs;
};

#endif //  __HORIZON_LINKS_H__
