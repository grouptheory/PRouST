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
#ifndef _E2E_TRANSIT_DELAY_H_
#define _E2E_TRANSIT_DELAY_H_

#ifndef LINT
static char const _UNI40_e2e_trans_delay_h_rcsid_[] =
"$Id: UNI40_e2e_trans_delay.h,v 1.5 1999/02/12 16:49:14 talmage Exp $";
#endif

#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/errmsg.h>
#include <codec/uni_ie/cause.h>

///
class UNI40_e2e_transit_delay : public InfoElem {
  friend ostream & operator << (ostream & os, UNI40_e2e_transit_delay & x);

public:
  ///
  enum identifiers {
    cumulative_id = 0x01,
    maximum_id    = 0x03,
    net_gen_ind   = 0x0a
  };

  ///
  UNI40_e2e_transit_delay(int cumulative = 0, int maximum = 0xFFFF, int net_gen_ind_set = 0);
  ///
  UNI40_e2e_transit_delay(const UNI40_e2e_transit_delay & etd);
  ///
  virtual ~UNI40_e2e_transit_delay() { }

  ///
  UNI40_e2e_transit_delay & operator = (const UNI40_e2e_transit_delay & rhs);
  ///
  int                    operator ==(const UNI40_e2e_transit_delay & rhs) const;
  ///
  int                    equals(const UNI40_e2e_transit_delay * rhs) const;
  ///
  int                    equals(const InfoElem * rhs) const;

  ///
  int                 encode(u_char * buffer);
  ///
  InfoElem::ie_status decode(u_char * buffer, int & id);

  ///
  UNI40_e2e_transit_delay * copy(void);
  ///
  InfoElem * copy(void) const;

  ///
  int                    getCumulativeTDV(void) const { return _cumulative; }
  ///
  int                    getMaximumTDV(void) const { return _maximum; }
  int Length();

  ///
  virtual char PrintSpecific(ostream & os);

private:
  ///
  int       _cumulative;
  ///
  int       _maximum;
  ///
  int       _net_gen_ind_set;
};
#endif


