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

// C++
#ifndef __CALLING_PARTY_SOFT_PVCC_H__
#define __CALLING_PARTY_SOFT_PVCC_H__
#if !defined(LINT)
static char const _PNNI_calling_party_pvpc_h_rcsid_[] =
"$Id: PNNI_calling_party_pvpc.h,v 1.3 1998/02/19 16:37:15 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>

/** Calling Part soft PVPC or PVCC indicates the VPI or VPI/VCI values used for the PVC
    Segment by the calling connecting point.  These values are conveyed to the called
    connecting point transparently.
 */
class PNNI_calling_party_soft_pvpc : public InfoElem {
  friend ostream & operator << (ostream & os, PNNI_calling_party_soft_pvpc & x);
public:

  PNNI_calling_party_soft_pvpc(int vpi = 0, int vci = 0) :
    InfoElem(PNNI_calling_party_soft_pvpc_id), _vpi(vpi), _vci(vci)
    {
      _coding = InfoElem::net;
    }

  PNNI_calling_party_soft_pvpc(const PNNI_calling_party_soft_pvpc & rhs) :
    InfoElem(PNNI_calling_party_soft_pvpc_id), _vpi(rhs._vpi), _vci(rhs._vci) { }

  virtual ~PNNI_calling_party_soft_pvpc(void) { }

  PNNI_calling_party_soft_pvpc * copy(void) { return (new PNNI_calling_party_soft_pvpc(*this)); }

  InfoElem *copy(void) const { return (InfoElem *)new PNNI_calling_party_soft_pvpc(this->_vpi, this->_vci); } 

  int                 encode(u_char *buffer);
  InfoElem::ie_status decode(u_char *buffer, int & id);

  int operator == (const PNNI_calling_party_soft_pvpc & rhs) const;
  int equals(const InfoElem * him) const;
  int equals(const PNNI_calling_party_soft_pvpc * himptr) const;
  int Length();
 protected:

  virtual char PrintSpecific(ostream & os);

 private:

  int  _vpi;
  int  _vci;
};

#endif // __CALLING_PARTY_SOFT_PVPC_H__
