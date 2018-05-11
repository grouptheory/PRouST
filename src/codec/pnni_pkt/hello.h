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

//-*- C++ -*-
#ifndef __HELLO_H__
#define __HELLO_H__

#ifndef LINT
static char const _hello_h_rcsid_[] =
"$Id: hello.h,v 1.18 1998/12/15 15:55:37 mountcas Exp $";
#endif

#include <codec/pnni_pkt/pnni_pkt.h>
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/addr.h>

#include <iostream.h>

class ig_aggregation_token;
class ig_nodal_hierarchy_list;
class ig_uplink_info_attr;
class ig_lgn_horizontal_link_ext;
class ig_req_ptse_header;

#define FIXED_HELLO_HEADER 100
 // PnniHeader(8), Flags(2), NodeID(22), Aesa(20), Pgid(14),
 // Rnid(22), Pid(4), Rpid(4), HelloInterval(2), reserve(2)

/** Hello Packets are transmitted by each node over (a) all physical links
    to immediate neighbor nodes, (b) all virtual path connections for
    which this node is an endpoint, and (c) all SVCCs established for the
    purpose of exchanging PNNI routing information for which this node is
    an endpoint.  The purpose of exchanging Hellos between neighbor nodes
    is in order to establish the state of the link(s) between them.
    af-pnni-0055.000 sec 5.14.8
 */
class HelloPkt : public PNNIPkt {
  friend ostream & operator << (ostream & os, const HelloPkt & pkt);
  friend void PrintSysAddress(const u_char * buffer, int len, ostream& os);
  friend void PrintNodeID(const u_char * buffer, int len, ostream& os);
  friend void PrintPeerGroupID(const u_char * buffer, int len, ostream& os);
public:

  /// Constructor.
  HelloPkt(u_char * nodeid = 0, u_char * atm_esa = 0, u_char * peerid = 0, 
	   u_char * rem_nodeid = 0, int portid = 0, int rportid = 0, sh_int hello_interval = 0);

  HelloPkt(const NodeID * const , const NodeID * const = 0,
           int portid = 0, int rportid = 0, sh_int hello_interval = 0);

  HelloPkt(const HelloPkt & hp);

  /// Desctructor.
  virtual ~HelloPkt();


  /**@name Methods for encoding/decoding Hello Packets. */
  //@{

  /// Encode takes a pointer to the buffer to encode into, and a reference to an int to store the length.
  u_char * encode(u_char *, int &);

  /// Decode takes the buffer where the encoded packet resides.
  errmsg * decode(const u_char *);
  //@}

  /**@name Accessor Methods */
  //@{

  /// Set node ID to ID pointed to by n.
  void    SetNodeID(u_char *n);

  /// Retrieve the node ID.
  const NodeID *GetNodeID(void) const;

  /// Set the ATM end system address to the one pointed to by a.
  void    SetATMEndSysAddr(u_char *a);

  /// Retrieve the ATM end system address.
  const Addr *GetATMEndSysAddr(void) const;
    
  /// Set the peer group ID to the one pointed to by p.
  void    SetPeerGroupID(u_char *p);

  /// Retrieve the peer group ID.
  const PeerGroupID *GetPeerGroupID(void) const;

  /// Set the remote node id to the one pointed to by r.
  void    SetRemoteNodeID(u_char *r);

  /// Retrieve the remote node ID.
  const NodeID *GetRemoteNodeID(void) const;

    /// Set the port ID to p.
  void    SetPortID(int p);

    /// Get the port ID.
  int     GetPortID(void) const;

    /// Set the remote port ID to p.
  void    SetRemotePortID(int p);

    /// Get the remote port ID.
  int     GetRemotePortID(void) const;

    /// Set the hello interval to i (in seconds).
  void    SetHelloInterval(sh_int i);

    /// Get the hello interval (in seconds).
  int     GetHelloInterval(void) const;

    /// Set the aggregation token to the token pointed to by a.
  void    SetAggregationToken(ig_aggregation_token * a);

    /// Get the aggregation token.
  const ig_aggregation_token *GetAggregationToken(void) const;

    /// Set the nodal hierarchy list to the one pointed to by n.
  void    SetNodalHierarchyList(ig_nodal_hierarchy_list * n);

    /// Get the nodal hierarchy list.
  const ig_nodal_hierarchy_list *GetNodalHierarchyList(void) const;

    /// Set the uplink information attribute.
  void    SetUplinkInfoAttr(ig_uplink_info_attr * u);

    /// Get the uplink information attribute.
  const ig_uplink_info_attr *GetUplinkInfoAttr(void) const;

    /// Set the LGN Horizontal Link Extension (if this Hello is being sent between LGNs).
  void    SetLGNHorizontalLinkExt(const ig_lgn_horizontal_link_ext * l);

    /// Get the LGN Horizontal Link Extention.
  const ig_lgn_horizontal_link_ext *GetLGNHorizontalLinkExt(void) const;
  //@}

  virtual PNNIPkt * copy(void) const;
  void size(int & length);

protected:

  /// Print Method.
  bool PrintSpecific(ostream &os) const;

  sh_int  _flags; /// Reserved
  NodeID * _node_id;
  Addr   * _atm_end_sys_addr;
  PeerID * _peer_group_id;
  NodeID * _remote_node_id;
  u_int   _port_id;
  u_int   _rport_id;
  sh_int  _hello_interval;
  // sh_int reserved

  /**@name Included when Hellos are sent over links destined for outside of this node's peer group. */
  //@{

    /// Aggregation token @see Information Group Documentation.
  ig_aggregation_token    * _agg_tok;

    /// Nodal hierarchy list @see Information Group Documentation.
  ig_nodal_hierarchy_list * _nod_lst;

    /// Uplink information attribute @see Information Group Documentation.
  ig_uplink_info_attr     * _upl_nfo;

  //@}

  /**@name Included when Hellos are sent between LGNs as part of the Horizontal Link Hello Protocol. */
  //@{

    /// LGN horizontal link extension.
  ig_lgn_horizontal_link_ext * _lgn_ext;

  //@}
};

#endif // __HELLO_H__
