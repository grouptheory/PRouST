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
#ifndef _RESTART_IND_H_
#define _RESTART_IND_H_

#ifndef LINT
static char const _restart_ind_h_rcsid_[] =
"$Id: restart_ind.h,v 1.6 1998/02/19 01:19:43 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class ie_errmsg;
/*
 * see 5.4.5.20 page 236 UNI-3.1 book
 * The purpose of this information element is to describe what to restart.
 * Its length is 1 byte.
 */
class ie_restart_ind : public InfoElem {
 public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const ie_restart_ind* himptr) const;

  enum restart_indicator {
    specific_vc = 0x00,
    all_vc      = 0x02
  };

  ie_restart_ind(restart_indicator cind);
  ie_restart_ind(const ie_restart_ind & rhs);
  ie_restart_ind(void);

  virtual ~ie_restart_ind();

  InfoElem* copy(void) const;

  int operator == (const ie_restart_ind & rs) const;
  
  virtual int                 encode(u_char * buffer);
  virtual InfoElem::ie_status decode(u_char *, int &);

  restart_indicator get_class(void);
  void set_class(restart_indicator cind);

  virtual char PrintSpecific(ostream & os);
  friend ostream & operator << (ostream & os, ie_restart_ind & x);
  int Length();
private:

  restart_indicator _restart_ind;
};

#endif
