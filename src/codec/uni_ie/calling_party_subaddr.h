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

#ifndef _CALLING_PARTY_SUBADDR_H_
#define _CALLING_PARTY_SUBADDR_H_
#if !defined(LINT)
static char const _calling_party_subaddr_h_rcsid_[] =
"$Id: calling_party_subaddr.h,v 1.4 1998/05/20 18:03:11 mountcas Exp $";
#endif

#include <codec/uni_ie/subaddr.h>
class Addr;

/* see 5.4.5.14 page 224 in UNI-3.1 book
 * Calling party subaddress: same comments as called party subaddr
 */

class ie_calling_party_subaddr : public subaddress {

 public:

  int equals(const InfoElem* him) const;
  int equals(const ie_calling_party_subaddr* himptr) const;

  ie_calling_party_subaddr(Addr *addr);
  ie_calling_party_subaddr(const ie_calling_party_subaddr & subaddr);

  virtual ~ie_calling_party_subaddr();

  virtual InfoElem* copy(void) const;

  int operator == (const ie_calling_party_subaddr& sub_addr) const;

  Addr* get_subaddr(void);

  virtual int                 encode(u_char * buffer);
  virtual InfoElem::ie_status decode(u_char * buffer, int &);

  ie_calling_party_subaddr(void);
  int Length();
  int max_repeats(void);
};

#endif
