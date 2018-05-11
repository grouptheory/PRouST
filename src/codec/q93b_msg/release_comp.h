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
#ifndef _RELEASE_COMP_H_
#define _RELEASE_COMP_H_

#ifndef LINT
static char const _release_comp_h_rcsid_[] =
"$Id: release_comp.h,v 1.10 1998/12/15 15:24:09 mountcas Exp $";
#endif

#include <codec/q93b_msg/generic_q93b_msg.h>

class InfoElem;
class Buffer;

class q93b_release_comp_message : public generic_q93b_msg {
public:

  q93b_release_comp_message(q93b_release_comp_message & him);
  q93b_release_comp_message(InfoElem ** ie_array, u_long call_ref_value, u_int flag);
  q93b_release_comp_message(InfoElem ** ie_array, u_long call_ref_value,
			    u_int flag, Buffer* buf);

  q93b_release_comp_message(const u_char * buf, int len);

  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg * copy(void);

protected:

  enum {    // H
    min_size = 9,
            // H Ca Ca GI GI GI
    max_size = 9+34+34+33+33+33
  };

  virtual int min_len(void) const;
  virtual int max_len(void) const;
};

#endif // _RELEASE_COMP_H_
