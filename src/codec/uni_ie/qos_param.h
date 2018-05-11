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
#ifndef _QOS_PARAM_H_
#define _QOS_PARAM_H_

#ifndef LINT
static char const _qos_param_h_rcsid_[] =
"$Id: qos_param.h,v 1.6 1998/02/19 01:27:40 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class ie_errmsg;
/*
 * see 5.4.5.18 page 233 UNI-3.1 book
 * The purpose of this information element is to request and
 * indicate the QOS of a connection.
 * Its length is 2 byte.
 */

class ie_qos_param : public InfoElem {
public:
  
  virtual int equals(const InfoElem* him) const;
  virtual int equals(const ie_qos_param* himptr) const;

  enum qos_indicator {
    qos0 = 0x00,
    qos1 = 0x01,
    qos2 = 0x02,
    qos3 = 0x03,
    qos4 = 0x04
  };

  ie_qos_param(qos_indicator fqos_class, qos_indicator bqos_class);
  ie_qos_param(const ie_qos_param & rhs);
  ie_qos_param(void);

  virtual ~ie_qos_param();

  InfoElem* copy(void) const;

  int operator == (const ie_qos_param & rs) const;
  
  virtual int                 encode(u_char * buffer);
  virtual InfoElem::ie_status decode(u_char *, int &);

  qos_indicator get_fqos_class(void);
  qos_indicator get_bqos_class(void);

  void set_fqos_class(qos_indicator fqos_class);

  void set_bqos_class(qos_indicator bqos_class);

  virtual char PrintSpecific(ostream & os);
  friend ostream & operator << (ostream & os, ie_qos_param & x);
  int Length();
private:

  qos_indicator _fqos_class;
  qos_indicator _bqos_class;
};

#endif
