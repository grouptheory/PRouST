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
#ifndef _CONN_ID_H_
#define _CONN_ID_H_

#ifndef LINT
static char const _conn_id_h_rcsid_[] =
"$Id: conn_id.h,v 1.5 1998/02/19 01:35:44 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>


class ie_errmsg;
/*
 * see 5.4.5.16 page 230 UNI-3.1 book
 * Identifies  the local ATM connection resources on the interface.
 * Its length is 5 bytes.
 */


class ie_conn_id : public InfoElem {
  friend ostream & operator << (ostream & os, ie_conn_id & x);
 public:

  int equals(const InfoElem* him) const;
  int equals(const ie_conn_id* himptr) const;

  ie_conn_id(long vpi, long vci);
  ie_conn_id(long vpi, long vci, int vp_associated, int pref_exclusive);
  ie_conn_id(const ie_conn_id & rhs);
  virtual ~ie_conn_id();

  InfoElem* copy(void) const;
  int operator == (const ie_conn_id & cid) const;
  
  virtual int                 encode(u_char * buf);
  virtual InfoElem::ie_status decode(u_char *, int &);
  
  long get_vci(void);
  long get_vpi(void);
  void set_vci(long v);
  void set_vpi(long v);
  int  GetVPassociated();
  int  GetPrefExclusive();
  ie_conn_id(void);
  int Length();
  virtual char PrintSpecific(ostream & os);

private:
  long _vpi;
  long _vci;
  int _vp_associated;
  int _pref_exclusive;
};

#endif

