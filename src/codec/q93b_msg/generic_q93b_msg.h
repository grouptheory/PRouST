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
#ifndef _GENERIC_Q93B_MSG_H_
#define _GENERIC_Q93B_MSG_H_

#ifndef LINT
static char const _generic_q93b_msg_h_rcsid_[] =
"$Id: generic_q93b_msg.h,v 1.26 1998/12/15 15:17:29 mountcas Exp $";
#endif

#include <codec/uni_ie/cause.h>
#include <codec/q93b_msg/header_parser.h>
#include <codec/q93b_msg/body_parser.h>

class InfoElem;
class Buffer;

class generic_q93b_msg : public header_parser, public body_parser {
  friend generic_q93b_msg * Parse(const u_char * buf, int len);
public:

  // Constructors
  generic_q93b_msg(const u_char * buffer, u_int buflen);
  generic_q93b_msg(InfoElem ** ie_array, msg_type t, 
		   u_long call_ref_value, u_int flag);
  generic_q93b_msg(InfoElem ** ie_array, msg_type t, 
		   u_long call_ref_value, u_int flag,
		   Buffer * buf);
  // Destructor
  virtual ~generic_q93b_msg();

  // pure virtual methods
  virtual generic_q93b_msg * copy(void) = 0;

  // public methods
  InfoElem * ie(InfoElem::ie_ix ix) const;
  InfoElem * steal_ie(InfoElem::ie_ix ix);

  u_char * encoded_buffer(void);
  int encoded_bufferlen(void);

  virtual int   msg_header_buflen(void);
  virtual int & msg_body_buflen(void);
  virtual int & msg_total_buflen(void);

  void re_encode(void);
  char Valid(void);
  void PrintErrors(ostream& os);
  void PrintSymbolic(ostream& os);
  void PrintEncoded(ostream& os);
  void Print(ostream& os);

  void UniDump(const char * prefix = 0L);
  void HexDump(ostream& os, const char * prefix = 0L);

protected:

  header_parser_errmsg * re_encode_header(void);
  body_parser_errmsg * re_encode_body(void);

  int  _total_buflen;
  char _msg_valid;
  int min_len(void) const;
  int max_len(void) const;
  void process(void);
  generic_q93b_msg(generic_q93b_msg & him, header_parser::msg_type t, u_char* buf, int len);

private:

  enum {MIN_UNI_MESSAGE_LENGTH = 9};
  enum {MAX_UNI_MESSAGE_LENGTH = 1024};
};

void Print_Type(ostream& os, header_parser::msg_type t);

#endif // _GENERIC_Q93B_MSG_H_






