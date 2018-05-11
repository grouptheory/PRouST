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
#ifndef __DESIGNATED_TRANSIT_LIST_H__
#define __DESIGNATED_TRANSIT_LIST_H__

#ifndef LINT
static char const _PNNI_des_xit_list_h_rcsid_[] =
"$Id: PNNI_designated_transit_list.h,v 1.12 1998/10/21 15:44:24 bilal Exp $";
#endif

#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>
#include <codec/pnni_ig/id.h>
#include <DS/containers/list.h>

/* DTLContainer ecapsulates a NodeID and port number.
 */
class DTLContainer {
  friend int compare ( DTLContainer const &lhs,  DTLContainer const  &rhs);
  friend ostream & operator << (ostream & os, const DTLContainer rhs);

public:

  DTLContainer(const NodeID * n, int port = 0);
  DTLContainer(const u_char * n, int port = 0);
  DTLContainer(DTLContainer * dtc);
  DTLContainer(const DTLContainer &dtc);
  ~DTLContainer( );

  // Casting operator to NodeID for ACAC
  operator NodeID * () const;

  const NodeID * GetNID(void) const;
        NodeID * TakeNID(void);
  const int      GetPort(void) const;

  DTLContainer * copy(void) const;

  bool equals(const DTLContainer * rhs) const;
  bool equals(const DTLContainer & rhs) const;
  int GetLevel();
  
private:

  NodeID * _nid;
  int      _port;
};

/** The designated transit list indicates the logical nodes and logical links that a connection
    is to traverse through a peer group at some level of hierarchy.
 */
class PNNI_designated_transit_list : public InfoElem {
  friend ostream & operator << (ostream & os, PNNI_designated_transit_list & x);
public:

  PNNI_designated_transit_list(int trans_ptr = 0);

  PNNI_designated_transit_list(const PNNI_designated_transit_list & rhs);

  virtual ~PNNI_designated_transit_list( );

  PNNI_designated_transit_list * copy(void);

  InfoElem * copy(void) const;

  int                 encode(u_char *buffer);
  InfoElem::ie_status decode(u_char *buffer, int & id);

  int operator == (const PNNI_designated_transit_list & rhs) const;
  int equals(const InfoElem * him) const;
  int equals(const PNNI_designated_transit_list * himptr) const;
 
  void AddToDTL(const u_char * nid, int port = 0);
  void AddToDTL(const NodeID * nid, int port = 0);
  void RemFromDTL(const u_char * nid, int port = 0);
  void RemFromDTL(const NodeID * nid, int port = 0);
  bool IsInDTL(const u_char * nid, int port = 0) const;
  bool IsInDTL(const NodeID * nid, int port = 0) const;

  const list<DTLContainer *> & GetDTL(void) const;

  int Length(void);

protected:

  virtual char PrintSpecific(ostream & os);

private:

  int        _transit_pointer;

  list<DTLContainer *>  _dtl;
  bool EqualDTLs(const list<DTLContainer *> & rhs) const;
};

#endif // __DESIGNATED_TRANSIT_LIST_H__
