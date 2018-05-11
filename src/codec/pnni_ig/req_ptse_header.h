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
#ifndef __REQ_PTSE_HEADER_H__
#define __REQ_PTSE_HEADER_H__

#ifndef LINT
static char const _req_ptse_header_h_rcsid_[] =
"$Id: req_ptse_header.h,v 1.15 1998/09/24 13:18:42 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <DS/containers/list.h>

class NodeID;
class ig_ptse;

#define REQUEST_HEADER 28 // type(2), len(2), onid(22), reqcnt(2)
#define REQUEST_CONTENT 4 // ptseid(4)

/** The requested PTSE header is included in the PTSE Request Packet, for each
    set of PTSEs requested.
 */
class ReqContainer {
public:

  ReqContainer(int ptse_id) : _ptse_id(ptse_id) {}
  ReqContainer(int id, u_int seq, sh_int life) : _ptse_id(id), _seq_num(seq),
               _rem_life_time(life){}
  friend int compare(ReqContainer * const &, ReqContainer  * const &);
  bool equals(const ReqContainer * ptr);
  ~ReqContainer(){};

  int    _ptse_id;
  u_int  _seq_num;
  sh_int _rem_life_time;
};

bool operator > (const ReqContainer & lhs, const ig_ptse &);

class ig_req_ptse_header : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_req_ptse_header & ig);
  friend istream & operator >> (istream & is, ig_req_ptse_header & ig);
  friend int compare(ig_req_ptse_header *const &, ig_req_ptse_header *const &);

public:

  /// Constructor.
  ig_req_ptse_header(const u_char * origin = 0);
  ig_req_ptse_header(const NodeID * origin);
  ig_req_ptse_header(const ig_req_ptse_header & rhs);

  /// Destructor.
  virtual ~ig_req_ptse_header();

  /**@name Methods for encoding/decoding the IG.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}

  int equals(ig_req_ptse_header & rhs);

  bool equals(const u_char * r_orig_node_id);

  void AddReqContainer(ReqContainer *lRcont);

  void AddReqContainer(int id, u_int seq, sh_int life);

  void RemReqContainer(ReqContainer *);

  ReqContainer *  Search(int r_ptse_id);

  const NodeID * GetOrigin(void) const;
  const list<ReqContainer *> & GetReqSummary(void) const;

  virtual InfoGroup * copy(void);
  void size(int & length);

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & is);

  u_char _originating_node_id [22];
  sh_int _ptse_request_count;
  list<ReqContainer *> _list;
};

#endif // __REQ_PTSE_HEADER_H__
