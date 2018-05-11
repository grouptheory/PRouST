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
#ifndef __CONN_SCOPE_SEL_H__
#define __CONN_SCOPE_SEL_H__

#ifndef LINT
static char const _UNI40_conn_scope_h_rcsid_[] =
"$Id: UNI40_conn_scope.h,v 1.4 1998/02/19 17:00:37 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/errmsg.h>
#include <codec/uni_ie/cause.h>

/** Connection Scope Selection IE allows the calling user to indicate
    to the network that the call/connection shall be processed and
    progressed within the selected routing range.  UNI 4.0 sec. 7.2.2.1
 */
class UNI40_conn_scope_sel : public InfoElem {
  friend ostream & operator << (ostream & os, UNI40_conn_scope_sel & x);

public:

  enum connection_scope_types {
    organizational = 0x01
    // All others are reserved
  };

  ///
  enum connection_scope_sels {
    reserved    = 0x00,
    local_net   = 0x01,
    local_netp1 = 0x02,
    local_netp2 = 0x03,
    sitem1      = 0x04,
    intra_site  = 0x05,
    sitep1      = 0x06,
    orgm1       = 0x07,
    intra_org   = 0x08,
    orgp1       = 0x09,
    communitym1 = 0x0A,
    intra_comm  = 0x0B,
    communityp1 = 0x0C,
    regional    = 0x0D,
    inter_regl  = 0x0E,
    global      = 0x0F
  };

  ///
  UNI40_conn_scope_sel(connection_scope_types type = organizational, connection_scope_sels sel = reserved) :
     InfoElem(UNI40_conn_scope_id), _conn_type(type), _conn_sel(sel) { }
  ///
  UNI40_conn_scope_sel(const UNI40_conn_scope_sel & css) : InfoElem(css._id),
     _conn_type(css._conn_type), _conn_sel(css._conn_sel) { }
  ///
  virtual ~UNI40_conn_scope_sel( ) { }

  ///
  UNI40_conn_scope_sel & operator = (const UNI40_conn_scope_sel & rhs);
  ///
  int                 operator ==(const UNI40_conn_scope_sel & rhs) const;
  ///
  int                 equals(const UNI40_conn_scope_sel * rhs) const;
  ///
  int                 equals(const InfoElem * ieptr) const;

  ///
  int                 encode(u_char * buffer);
  ///
  InfoElem::ie_status decode(u_char * buffer, int & id);

  ///
  UNI40_conn_scope_sel *copy(void) { return new UNI40_conn_scope_sel(*this); }
  ///
  InfoElem * copy(void) const { return (InfoElem *)(new UNI40_conn_scope_sel(this->_conn_type, 
                                this->_conn_sel)); }
 
  ///
  connection_scope_types getConnScopeType(void) const { return _conn_type; }
  ///
  connection_scope_sels  getConnScopeSel(void) const { return _conn_sel; }
  int Length();
 protected:
  ///
  char PrintSpecific(ostream & os);

 private:
  ///
  connection_scope_types     _conn_type;
  ///
  connection_scope_sels      _conn_sel;
};

#endif // __CONN_SCOPE_SEL_H__

