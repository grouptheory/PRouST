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
#ifndef __NOTIF_H__
#define __NOTIF_H__

#ifndef LINT
static char const _UNI40_notif_h_rcsid_[] =
"$Id: UNI40_notif.h,v 1.5 1998/09/01 20:44:23 battou Exp $";
#endif


#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>

/** The notification idicator information element indicates information pertaining to
    a call. 
 */
class UNI40_notification_indicator : public InfoElem {
  ///
  friend ostream & operator << (ostream & os, UNI40_notification_indicator & x);
 public:

  ///
  UNI40_notification_indicator( ) : InfoElem(UNI40_notification_ind_id) { }
  ///
  UNI40_notification_indicator(const UNI40_notification_indicator & rhs);

  ///
  virtual ~UNI40_notification_indicator( ) { }

  ///
  UNI40_notification_indicator & operator = (const UNI40_notification_indicator & rhs);
  ///
  int  operator == (const UNI40_notification_indicator & rhs) const;
  ///
  int  equals(const UNI40_notification_indicator * rhs) const;
  ///
  int  equals(const InfoElem * rhs) const;

  ///
  UNI40_notification_indicator *copy(void) { return new UNI40_notification_indicator(*this); }
  ///
  InfoElem *copy(void) const { return (InfoElem *)(new UNI40_notification_indicator(*this)); }

  ///
  int                 encode(u_char *buffer);
  ///
  InfoElem::ie_status decode(u_char *buffer, int & id);
  int Length();
 protected:
  ///
  char PrintSpecific(ostream & os);

 private:

  ///
  int    _length;
  
};

#endif // __NOTIF_H__



