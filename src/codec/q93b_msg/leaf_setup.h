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
#ifndef _LEAF_SETUP_H_
#define _LEAF_SETUP_H_

#ifndef LINT
static char const _leaf_setup_h_rcsid_[] =
"$Id: leaf_setup.h,v 1.11 1998/12/15 15:26:23 mountcas Exp $";
#endif

#include <codec/q93b_msg/generic_q93b_msg.h>
class Buffer;

class UNI40_leaf_setup_failure_message : public generic_q93b_msg {
public:

  UNI40_leaf_setup_failure_message(UNI40_leaf_setup_failure_message & him);
  UNI40_leaf_setup_failure_message(InfoElem ** ie_array, u_long call_ref_value, u_int flag);
  UNI40_leaf_setup_failure_message(InfoElem ** ie_array, u_long call_ref_value,
				   u_int flag, Buffer* buf);
  UNI40_leaf_setup_failure_message(const u_char * buf, int len);

  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  generic_q93b_msg * copy(void);

protected:

  enum {    // H C L
    min_size = 9+6+4,
            // H C  CN CS L T
    max_size = 9+34+25+25+8+9
  };

  virtual int min_len(void) const;
  virtual int max_len(void) const;
};


class UNI40_leaf_setup_request_message : public generic_q93b_msg {
public:

  UNI40_leaf_setup_request_message(UNI40_leaf_setup_request_message & him);
  UNI40_leaf_setup_request_message(InfoElem ** ie_array, u_long call_ref_value, u_int flag);
  UNI40_leaf_setup_request_message(InfoElem ** ie_array, u_long call_ref_value,
				   u_int flag, Buffer * buf);
  UNI40_leaf_setup_request_message(const u_char * buf, int len);

  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  generic_q93b_msg * copy(void);

protected:

  enum {    // H C C J S
    min_size = 9+4+4+4+4,
            // H T CN CS CN CS J S
    max_size = 9+9+26+25+25+25+9+8
  };

  virtual int min_len(void) const;
  virtual int max_len(void) const;
};

#endif // _LEAF_SETUP_H_
