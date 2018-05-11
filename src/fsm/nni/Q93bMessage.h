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
#ifndef _Q93BMSG_H_
#define _Q93BMSG_H_

#ifndef LINT
static char const _Q93bMessage_h_rcsid_[] =
"$Id: Q93bMessage.h,v 1.2 1998/06/04 12:22:51 mountcas Exp $";
#endif

#include <ostream.h>
extern "C" {
#include <sys/types.h>
#include <netinet/in.h>
};

#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/cause.h>

class InfoElem;
class Q93bVisitor;
class Call;

// msg_header_len(9)=protocol_id(1)+call_reference(4)+msg_type(2)+msg_len(2)
const int msg_header_len = 9;

const u_long UNASSIGNED_CREF = ((u_long)(0x0FFF));

class generic_q93b_msg;

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

protected:

  header_error_code _header_error;
  
public:

  header_parser_errmsg(header_error_code x);

  char successful(void);

  void Print(ostream& os);
};


class header_parser {
friend class q93b_tp;
public:

  enum msg_type {
    call_proceeding_msg = 0x02,
    connect_msg         = 0x07,
    connect_ack_msg     = 0x0f,
    setup_msg           = 0x05,
    
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

    // UNI40
    leaf_setup_failure_msg = 0x90,
    leaf_setup_request_msg = 0x91,

    UNASSIGNED_MSG_TYPE = 0xFF
  };

protected:

  char is_msg_valid(msg_type t);

public:

  enum protocol_discriminator {
    protocol_id = 0x09
  };

  // second byte of message type (for this implementation: 0x80)
  enum action_indicator {
    clear_call = 0x01,
    discard_and_ignore = 0x02,
    discard_and_report_status = 0x03,
    reserved = 0x04,
  };

  enum { UNASSIGNED            = -1 };

  // constructor check note 1 on page 189 UNI-3.1 for flag and action values.

  header_parser(u_char *buffer,u_int buflen);
  header_parser(u_long call_ref_value, msg_type action, u_int flag);

  virtual ~header_parser();

  header_parser_errmsg* process(void);

  u_long get_crv(void);
  u_int get_crf(void);

protected:

  //------------------------------------
  virtual int min_len(void)=0;
  virtual int max_len(void)=0;

  virtual int  msg_header_buflen(void)=0;
  virtual int& msg_body_buflen(void)=0;
  virtual int& msg_total_buflen(void)=0;

  header_parser_errmsg* encode_msg_header(void);
  header_parser_errmsg* decode_msg_header(void);

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
  char   _duplicated_header;

  header_parser_errmsg* _errors;
  void list_errors(ostream& os);

  void Print(ostream& os);

  header_parser(header_parser& him, u_char* buf, int len);

public:

  msg_type type(void);
  friend msg_type MessageType(u_char* buf, int len);
};


class body_parser_errmsg {
  friend class body_parser;
  friend class generic_q93b_msg;
public:
  
  char successful(void);

  void Print(ostream& os);

protected:

  char _completed;

  body_parser_errmsg(void);
  ~body_parser_errmsg();

  int* _ie_mand_missing;
  int* _ie_opt_missing;
  int* _ie_mand_corrupted;
  int* _ie_opt_corrupted;
};


class body_parser {
  friend class Q93bVisitor;
public:

  body_parser(u_char *buffer, int buflen);
  body_parser(InfoElem** ie_array);

  virtual ~body_parser();

  body_parser_errmsg *process(void);

  InfoElem ** get_ie_array(void) { return _ie_array; }

protected:

  void Print(ostream& os);

  virtual int  min_len(void)=0;
  virtual int  max_len(void)=0;

  virtual int  msg_header_buflen(void)=0;
  virtual int& msg_body_buflen(void)=0;
  virtual int& msg_total_buflen(void)=0;

  body_parser_errmsg *process_decode(void);
  body_parser_errmsg *process_encode(void);

  u_char* _body_buffer;
  int     _body_buflen;
  int     _body_valid;

  int     _encode;
  char    _duplicated_body;

  InfoElem* _ie_array[num_ie];
  u_char*   _ie_encoded[num_ie];
  int       _ie_len[num_ie];

  int* _ie_legal;
  int* _ie_mandatory;
  int* _ie_hits;

  InfoElem*      _current_ie;
  InfoElem::ieid _current_id;
  int            _current_len;

  body_parser_errmsg* _errors;
  void list_errors(ostream& os);

  //----------------------------------
  virtual void SetupLegal(void)=0;        // define in subclasses
  virtual void SetupMandatory(void)=0;
  //-----------------------------------

  void zero(void);
  int parse_ie_header(u_char *buffer, int buflen);

  body_parser(body_parser& him, u_char* buf, int len);
};

class generic_q93b_msg : public header_parser, public body_parser {
  friend class q93b_tp;
  friend class q93b_call_adapter;
  friend class service_trie_upmux;
  // added for new port
  friend class Q93bVisitor;
  friend class Call;
  friend class Party;
  friend class Q93bState;
public:

  InfoElem* ie(int x);        // returns the ie
  InfoElem* steal_ie(int x);  // returns the ie, and sets the pointer to null
  u_char* encoded_buffer(void);
  int encoded_bufferlen(void);
  virtual int  msg_header_buflen(void);
  virtual int& msg_body_buflen(void);
  virtual int& msg_total_buflen(void);
  header_parser_errmsg* re_encode_header(void);
  body_parser_errmsg* re_encode_body(void);
  generic_q93b_msg(u_char *buffer, u_int buflen);
  generic_q93b_msg(InfoElem** ie_array, msg_type t, 
		   u_long call_ref_value, u_int flag);
  void re_encode(void);
  virtual ~generic_q93b_msg();
  char Valid(void);
  // added for new port
  void PrintErrors(ostream& os);
  void PrintSymbolic(ostream& os);
  void PrintEncoded(ostream& os);
  void Print(ostream& os);
  void HexDump(ostream& os, const char* prefix=0L);
  friend generic_q93b_msg* Parse(u_char* buf, int len);
  virtual generic_q93b_msg* copy(void) = 0;

protected:

  enum { MIN_UNI_MESSAGE_LENGTH = 9 };
  enum { MAX_UNI_MESSAGE_LENGTH = 1024 };

  int  _total_buflen;
  char _msg_valid;
  int min_len(void);
  int max_len(void);
  void process(void);
  generic_q93b_msg(generic_q93b_msg& him, header_parser::msg_type t, u_char* buf, int len);
};


void Print_Type(ostream& os, header_parser::msg_type t);

class q93b_freeform_message : public generic_q93b_msg {
public:
  q93b_freeform_message(generic_q93b_msg& him);
  q93b_freeform_message(InfoElem** ie_array, u_long call_ref_value, u_int flag);

  q93b_freeform_message(u_char* buf, int len);

  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);

protected:

  enum {
    min_size = 38,
    max_size = 209
  };

  virtual int min_len(void);
  virtual int max_len(void);
};


class q93b_add_party_message : public generic_q93b_msg {
public:

  q93b_add_party_message(generic_q93b_msg& him);
  q93b_add_party_message(InfoElem** ie_array, u_long call_ref_value,
			 u_int flag);
  q93b_add_party_message(u_char* buf, int len);

  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  generic_q93b_msg* copy(void);

protected:

  enum {
    min_size = 9+4+4+4+4+4+4+4+4+4+7,
    max_size = 9+20+13+17+4+25+26+25+5+8+7
  };

  virtual int min_len(void);
  virtual int max_len(void);
};

class q93b_add_party_ack_message : public generic_q93b_msg {
protected:

  enum {
    min_size = 9+7,
    max_size = 9+7
  };

  virtual int min_len(void);
  virtual int max_len(void);

public:

  q93b_add_party_ack_message(generic_q93b_msg& him);
  q93b_add_party_ack_message(InfoElem** ie_array, u_long call_ref_value,
			     u_int flag);

  q93b_add_party_ack_message(u_char* buf, int len);

  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  generic_q93b_msg* copy(void);
};


class q93b_add_party_rej_message : public generic_q93b_msg {
protected:

  enum {
    min_size = 9+6+7,
    max_size = 9+34+7
  };

  virtual int min_len(void);
  virtual int max_len(void);

public:

  q93b_add_party_rej_message(generic_q93b_msg& him);
  q93b_add_party_rej_message(InfoElem** ie_array, u_long call_ref_value,
			     u_int flag);

  q93b_add_party_rej_message(u_char* buf, int len);

  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
};

class q93b_call_proceeding_message : public generic_q93b_msg {

protected:
  enum {
    min_size = 9,
    max_size = 9+9+7
  };

  virtual int min_len(void);
  virtual int max_len(void);

public:
  q93b_call_proceeding_message(generic_q93b_msg& him);
  q93b_call_proceeding_message(InfoElem** ie_array, u_long call_ref_value,
			       u_int flag);
  q93b_call_proceeding_message(u_char* buf, int len);

  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
};


class q93b_connect_message : public generic_q93b_msg {
public:
  q93b_connect_message(generic_q93b_msg& him);
  q93b_connect_message(InfoElem** ie_array, u_long call_ref_value,
		       u_int flag);
  q93b_connect_message(u_char* buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
protected:
  enum {
    min_size = 9,
    max_size = 9+11+17+9+7
  };
  virtual int min_len(void);
  virtual int max_len(void);
};

class q93b_connect_ack_message : public generic_q93b_msg {
public:
  q93b_connect_ack_message(generic_q93b_msg& him);
  q93b_connect_ack_message(InfoElem** ie_array, u_long call_ref_value,
			   u_int flag);
  q93b_connect_ack_message(u_char* buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
protected:
  enum {
    min_size = 9,
    max_size = 9
  };
  virtual int min_len(void);
  virtual int max_len(void);
};

class q93b_drop_party_message : public generic_q93b_msg {
public:
  q93b_drop_party_message(generic_q93b_msg& him);
  q93b_drop_party_message(InfoElem** ie_array, u_long call_ref_value,
			  u_int flag);
  q93b_drop_party_message(u_char* buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
protected:
  enum {
    min_size = 9+6+7,
    max_size = 9+34+7
  };
  virtual int min_len(void);
  virtual int max_len(void);
};

class q93b_drop_party_ack_message : public generic_q93b_msg {

public:
  q93b_drop_party_ack_message(generic_q93b_msg& him);
  q93b_drop_party_ack_message(InfoElem** ie_array, u_long call_ref_value,
			      u_int flag);
  q93b_drop_party_ack_message(u_char* buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
protected:
  enum {
    min_size = 9+4+7,
    max_size = 9+34+7
  };
  virtual int min_len(void);
  virtual int max_len(void);
};

class UNI40_leaf_setup_failure_message : public generic_q93b_msg {
public:

  UNI40_leaf_setup_failure_message(generic_q93b_msg & him);
  UNI40_leaf_setup_failure_message(InfoElem** ie_array, u_long call_ref_value,
				   u_int flag);
  UNI40_leaf_setup_failure_message(u_char * buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  generic_q93b_msg * copy(void);
protected:
  enum {
    min_size = 23,
    max_size = 77
  };
  virtual int min_len(void);
  virtual int max_len(void);
};


class UNI40_leaf_setup_request_message : public generic_q93b_msg {
public:

  UNI40_leaf_setup_request_message(generic_q93b_msg & him);
  UNI40_leaf_setup_request_message(InfoElem** ie_array, u_long call_ref_value,
				   u_int flag);
  UNI40_leaf_setup_request_message(u_char * buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  generic_q93b_msg * copy(void);
protected:
  enum {
    min_size = 33,
    max_size = 91
  };
  virtual int min_len(void);
  virtual int max_len(void);
};

class q93b_release_message : public generic_q93b_msg {
public:
  q93b_release_message(generic_q93b_msg& him);
  q93b_release_message(InfoElem** ie_array, u_long call_ref_value, u_int flag);
  q93b_release_message(u_char* buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
protected:
  enum {
    min_size = 9+6,
    max_size = 9+34
  };
  virtual int min_len(void);
  virtual int max_len(void);
};

class q93b_release_comp_message : public generic_q93b_msg {
public:
  q93b_release_comp_message(generic_q93b_msg& him);
  q93b_release_comp_message(InfoElem** ie_array, u_long call_ref_value,
			    u_int flag);
  q93b_release_comp_message(u_char* buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
protected:
  enum {
    min_size = 9+4,
    max_size = 9+34
  };
  virtual int min_len(void);
  virtual int max_len(void);
};


class q93b_restart_message : public generic_q93b_msg {
public:
  q93b_restart_message(generic_q93b_msg& him);
  q93b_restart_message(InfoElem** ie_array, u_long call_ref_value, u_int flag);
  q93b_restart_message(u_char* buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
protected:
  enum {
    min_size = 9+5,
    max_size = 9+9+5
  };
  virtual int min_len(void);
  virtual int max_len(void);
};

class q93b_restart_ack_message : public generic_q93b_msg {
public:
  q93b_restart_ack_message(generic_q93b_msg& him);
  q93b_restart_ack_message(InfoElem** ie_array, u_long call_ref_value,
			   u_int flag);
  q93b_restart_ack_message(u_char* buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
protected:
  enum {
    min_size = 9+5,
    max_size = 9+5+9
  };
  virtual int min_len(void);
  virtual int max_len(void);
};

class q93b_setup_message : public generic_q93b_msg {
public:
  q93b_setup_message(generic_q93b_msg& him);
  q93b_setup_message(InfoElem** ie_array, u_long call_ref_value, u_int flag);
  q93b_setup_message(u_char* buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
protected:
  enum {
    min_size = 38,
    max_size = 209
  };
  virtual int min_len(void);
  virtual int max_len(void);
};

class q93b_status_message : public generic_q93b_msg {
public:
  q93b_status_message(generic_q93b_msg& him);
  q93b_status_message(InfoElem** ie_array, u_long call_ref_value, u_int flag);
  q93b_status_message(u_char* buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);
protected:
  enum {
    min_size = 9+5+6,
    max_size = 9+5+34+7+5
  };
  virtual int min_len(void);
  virtual int max_len(void);
};

class q93b_status_enq_message : public generic_q93b_msg {
public:

  q93b_status_enq_message(generic_q93b_msg& him);
  q93b_status_enq_message(InfoElem** ie_array, u_long call_ref_value,
			  u_int flag);
  q93b_status_enq_message(u_char* buf, int len);
  virtual void SetupLegal(void);
  virtual void SetupMandatory(void);
  virtual generic_q93b_msg* copy(void);

protected:

  enum {
    min_size = 9,
    max_size = 9+7
  };
  virtual int min_len(void);
  virtual int max_len(void);
};

#endif // _Q93BMSG_H_
