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
#ifndef _HEADER_PARSER_H_
#define _HEADER_PARSER_H_

#ifndef LINT
static char const _header_parser_h_rcsid_[] =
"$Id: header_parser.h,v 1.17 1998/12/15 15:18:19 mountcas Exp $";
#endif

#include <iostream.h>

// msg_header_len(9)=protocol_id(1)+call_reference(4)+msg_type(2)+msg_len(2)
const int msg_header_len = 9;

const u_long UNASSIGNED_CREF = ((u_long)(0x0FFF));

class header_parser_errmsg;
class q93b_transporter;
class Buffer;

//------------------------------------------------
class header_parser_errmsg {
  friend class generic_q93b_msg;
public:

  enum header_error_code {
    unassigned_call_ref_value,
    unassigned_action, 
    unassigned_flag,
    wrong_discriminator,
    bad_cref_len,
    invalid_msg_type,
    invalid_msg_mode,
    inconsistent_msg_len,
    below_min_len,
    above_max_len,
    no_header_parser_errmsg 
  };

  header_parser_errmsg(header_error_code x);
  char successful(void);
  void Print(ostream& os);

protected:

  header_error_code _header_error;

};


class header_parser {
public:

  enum msg_type {
    alerting_msg        = 0x01,
    call_proceeding_msg = 0x02,
    setup_msg           = 0x05,
    connect_msg         = 0x07,
    notify_msg          = 0x0e,
    connect_ack_msg     = 0x0f,

    release_msg         = 0x4d,
    release_comp_msg    = 0x5a,
    
    restart_msg         = 0x46,
    restart_ack_msg     = 0x4e,
    
    status_msg          = 0x7d,
    status_enq_msg      = 0x75,

    add_party_msg       = 0x80,
    add_party_ack_msg   = 0x81,
    add_party_rej_msg   = 0x82,
    drop_party_msg      = 0x83,
    drop_party_ack_msg  = 0x84,
    party_alerting_msg  = 0x85,

    // UNI40
    leaf_setup_failure_msg = 0x90,
    leaf_setup_request_msg = 0x91,

    UNASSIGNED_MSG_TYPE = 0xFF
  };

  enum protocol_discriminator {
    protocol_id = 0x09
  };

  // second byte of message type 
  enum action_indicator {
    clear_call = 0x01,
    discard_and_ignore = 0x02,
    discard_and_report_status = 0x03,
    reserved = 0x04,
  };

  enum { UNASSIGNED            = -1 };

  header_parser(const u_char * buffer, u_int buflen);
  header_parser(u_long call_ref_value, msg_type action, u_int flag);
  header_parser(u_long call_ref_value, msg_type action, u_int flag, 
		Buffer * buf);

  virtual ~header_parser();

  msg_type type(void);
  header_parser_errmsg * process(void);
  u_long get_crv(void);
  u_int  get_crf(void);
  void   set_crv(u_long crv);
  void   set_crf(u_int  crf);

  void set_external_buffer_on(void);

protected:
  // Must be declared here since msg_type is declared right above.
  friend msg_type MessageType(const u_char * buf, int len);

  char is_msg_valid(msg_type t);

  virtual int   min_len(void) const     = 0;
  virtual int   max_len(void) const     = 0;
  virtual int   msg_header_buflen(void) = 0;
  virtual int & msg_body_buflen(void)   = 0;
  virtual int & msg_total_buflen(void)  = 0;

  header_parser_errmsg * encode_msg_header(void);
  header_parser_errmsg * decode_msg_header(void);

  // encoded
  u_char *_header_buffer;
  u_int   _header_buflen;
  int     _header_valid;
  protocol_discriminator  _proto_id;
  u_long                  _call_ref_value;
  u_int                   _call_ref_flag;
  msg_type                _action;
  action_indicator        _msg_mode;
  int    _up;
  int    _external_buffer;
  char   _duplicated_header;
  header_parser_errmsg* _errors;
  void list_errors(ostream& os);
  void Print(ostream& os);
  header_parser(header_parser& him, u_char* buf, int len);
};

#endif // _HEADER_PARSER_H_

