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
#ifndef _CALLED_PARTY_NUM_H_
#define _CALLED_PARTY_NUM_H_

#ifndef LINT
static char const _called_party_num_h_rcsid_[] =
"$Id: called_party_num.h,v 1.5 1998/02/19 01:54:53 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class Addr;
class ie_errmsg;
class ie_called_party_num;

/* see 5.4.5.11 page 219 in UNI-3.1 book
 * Called party number
 */

class ie_called_party_num : public InfoElem{
  friend ostream & operator << (ostream & os, ie_called_party_num & x);

public:

  int equals(const InfoElem* him) const;
  int equals(const ie_called_party_num* himptr) const;
  ///
  ie_called_party_num(Addr * dst_addr);
  ///
  ie_called_party_num(const ie_called_party_num & dst_addr);
  ///
  virtual ~ie_called_party_num(void);
  ///
  Addr* get_addr(void);
  ///
  virtual InfoElem* copy(void) const;
  ///
  virtual int encode(u_char * buffer);
  ///
  virtual InfoElem::ie_status decode(u_char * buffer, int &);
  ///
  int operator == (const ie_called_party_num & dst) const;
  ///
  ie_called_party_num(void);
  int Length();
private:

  virtual char PrintSpecific(ostream & os);

  Addr * _dst_addr;
};


#endif

