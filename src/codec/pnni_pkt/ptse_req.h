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
#ifndef __PTSE_REQ_H__
#define __PTSE_REQ_H__

#ifndef LINT
static char const _ptse_req_h_rcsid_[] =
"$Id: ptse_req.h,v 1.10 1998/12/15 15:57:28 mountcas Exp $";
#endif

#include <DS/containers/list.h>
#include <codec/pnni_pkt/pnni_pkt.h>

class ig_req_ptse_header;

/** PTSE request packets are used during database synchronization to
    request from a neighboring peer those PTSEs that have been newly
    discovered or that have been found to be obsolete.
 */
class PTSEReqPkt : public PNNIPkt {
  friend ostream & operator << (ostream & os, const PTSEReqPkt & pkt);
public:

  /// Constructor.
  PTSEReqPkt();

  /// Destructor.
  virtual ~PTSEReqPkt();

  /**@name Methods for encoding/decoding PTSE request packets. */
  //@{

    /// Encode.
  u_char *encode(u_char * buffer, int & len);

    /// Decode.
  errmsg *decode(const u_char * buffer);
  //@}

  /**@name Methods for manipulating the list of PTSE requests. */
  //@{

    /// Adds a nodal PTSE request to the list.
  void AddNodalPTSEReq(ig_req_ptse_header *);

   /// Adds more PTSE requests to an existing pkt.
  void AddMoreReq(PTSEReqPkt *sRpkt);

    /// Removes a nodal PTSE request from the list with the same attributes as r.
  bool RemNodalPTSEReq(ig_req_ptse_header *r);
  //@}

  // Search for a header with a perticular Node Id
  ig_req_ptse_header * Search(const u_char * rnid);

  const list<ig_req_ptse_header *> & GetHeaders(void) const;

  virtual PNNIPkt * copy(void) const;

  void size(int & length);

protected:

  /// Method for printing list of requests.
  bool PrintSpecific(ostream & os) const;

  list<ig_req_ptse_header *> _list;
};

#endif // __PTSE_REQ_H__
