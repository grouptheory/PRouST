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
#ifndef __CALLED_PARTY_SOFT_PVCC_H__
#define __CALLED_PARTY_SOFT_PVCC_H__

#ifndef LINT
static char const _PNNI_called_party_pvpc_h_rcsid_[] =
"$Id: PNNI_called_party_pvpc.h,v 1.6 1998/02/19 16:36:40 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>

/** Called Party soft PVPC or PVCC indicates the VPI or VPI/VCI values of a PVC segment between
    the called connecting point and the user of a PVPC or PVCC respectively.  These values are
    conveyed to the called connecting point transparently.
 */
class PNNI_called_party_soft_pvpc : public InfoElem {
  friend ostream & operator << (ostream & os, PNNI_called_party_soft_pvpc & x);
public:

  enum SelectionTypes {
    AnyValue = 0,
    ReqValue = 2,
    AsgValue = 4,

    // constant identifiers
    VPI_ident = 0x81,
    VCI_ident = 0x82
  };

  PNNI_called_party_soft_pvpc(u_char sel = 0, int vpi = 0, int vci = 0);
  PNNI_called_party_soft_pvpc(const PNNI_called_party_soft_pvpc & rhs);
  virtual ~PNNI_called_party_soft_pvpc(void);

  PNNI_called_party_soft_pvpc * copy(void);

  InfoElem * copy(void) const;

  int                 encode(u_char *buffer);
  InfoElem::ie_status decode(u_char *buffer, int & id);

  int operator == (const PNNI_called_party_soft_pvpc & rhs) const;
  int equals(const InfoElem * him) const;
  int equals(const PNNI_called_party_soft_pvpc * himptr) const;
  int Length();
 protected:

  virtual char PrintSpecific(ostream & os);

 private:
 
  bool   _vci_present;
  u_char _selection_type;
  int    _vpi;
  int    _vci;
};

#endif // __CALLED_PARTY_SOFT_PVPC_H__
