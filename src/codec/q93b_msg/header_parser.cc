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
#ifndef _HEADER_PARSER_CC_
#define _HEADER_PARSER_CC_

#ifndef LINT
static char const _header_parser_cc_rcsid_[] =
"$Id: header_parser.cc,v 1.29 1998/12/15 15:26:56 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/q93b_msg/header_parser.h>
#include <iostream.h>

extern "C" {
#include <assert.h>
#include <strings.h>
};

#define CREF_FLAG 0x800000

//----------------------------------------------------------
u_long header_parser::get_crv(void)
{
  return _call_ref_value;
}
u_int header_parser::get_crf(void)
{
  return _call_ref_flag;
}

void header_parser::set_crv(u_long crv)
{
  u_char *b = _header_buffer+2;
  b[0] = (crv>>16)&255;
  b[1] = (crv>> 8)&255;
  b[2] = (crv    )&255;
  _call_ref_value = crv;

  if (_call_ref_value & CREF_FLAG) 
    _call_ref_flag = 1;
  else 
    _call_ref_flag = 0;
}

void header_parser::set_crf(u_int crf)
{
  // Can only be zero or one
  assert((crf==0)||(crf==1));

  u_long val = get_crv();
  if (crf) 
    val |= 0x800000;
  else 
    val &= 0x7fffff;

  set_crv(val);
}

//----------------------------------------------------------
void header_parser::Print(ostream& os)
{
  os << "*** Header Info: ***" << endl;
  os << "cref= " << hex << _call_ref_value << dec << endl;
  os << "type= ";
  switch (_action) {
    case alerting_msg:
      os << "alerting msg" << endl;
      break;
    case call_proceeding_msg:
      os << "call proceeding msg" << endl;
      break;
    case setup_msg:
      os << "setup msg" << endl;
      break;
    case connect_msg:
      os << "connect msg" << endl;
      break;
    case notify_msg:
      os << "notify msg" << endl;
      break;
    case connect_ack_msg:
      os << "connect ack msg" << endl;
      break;
    case release_msg:
      os << "release msg" << endl;
      break;
    case release_comp_msg:
      os << "release comp msg" << endl;
      break;
    case restart_msg:
      os << "restart msg" << endl;
      break;
    case restart_ack_msg:
      os << "restart ack msg" << endl;
      break;
    case status_msg:
      os << "status msg" << endl;
      break;
    case status_enq_msg:
      os << "status enq msg" << endl;
      break;
    case add_party_msg:
      os << "add party msg" << endl;
      break;
    case add_party_ack_msg:
      os << "add party ack msg" << endl;
      break;
    case add_party_rej_msg:
      os << "add party rej msg" << endl;
      break;
    case drop_party_msg:
      os << "drop party msg" << endl;
      break;
    case drop_party_ack_msg:
      os << "drop party ack msg" << endl;
      break;
    case party_alerting_msg:
      os << "party alerting msg" << endl;
      break;
    case leaf_setup_failure_msg:
      os << "leaf setup failure msg" << endl;
      break;
    case leaf_setup_request_msg:
      os << "leaf setup request msg" << endl;
      break;
    default:
      os << "unknown" << endl;
      break;
  }
  os << "cref flag = " << _call_ref_flag << endl;
}


//----------------------------------------------------------
void header_parser::list_errors(ostream& os)
{
  if (_errors) _errors->Print(os);
  else os << "Message-Header errors data structure is still non-existent (ok)\n";
}

//----------------------------------------------------------
header_parser::header_parser(const u_char * buffer, u_int buflen)
  : _call_ref_value(UNASSIGNED), _action(UNASSIGNED_MSG_TYPE), 
    _up(1), _header_valid(0), _external_buffer(1)
{
  _header_buffer = (u_char *)buffer;
  _header_buflen = buflen;

  _call_ref_flag = UNASSIGNED;
  _duplicated_header = 0;
  _errors=0L;
};

//----------------------------------------------------------
header_parser::header_parser(u_long call_ref_value, msg_type action, u_int flag)
  : _call_ref_value(call_ref_value), _call_ref_flag(flag), _action(action), 
    _up(0), _header_valid(0), _external_buffer(0)
{
  _header_buffer = 0L;
  _header_buflen = 0L;
  _duplicated_header = 0;
  _errors=0L;
}

//----------------------------------------------------------
header_parser::header_parser(u_long call_ref_value, 
			     msg_type action, u_int flag,
			     Buffer * buf)
  : _call_ref_value(call_ref_value), 
    _call_ref_flag(flag), _action(action), _up(0), _header_valid(0), 
    _external_buffer(1)
{
  _header_buffer = 0L;
  _header_buflen = 0L;
  _duplicated_header = 0;
  _errors = 0L;
}


char header_parser::is_msg_valid(msg_type t)
{
  switch(t) {
  case alerting_msg:
  case call_proceeding_msg:
  case connect_msg:
  case notify_msg:
  case connect_ack_msg:
  case setup_msg:
  case release_msg:
  case release_comp_msg:
  case restart_msg:
  case restart_ack_msg:
  case status_msg:
  case status_enq_msg:
  case add_party_msg:
  case add_party_ack_msg:
  case add_party_rej_msg:
  case drop_party_msg:
  case drop_party_ack_msg:
  case party_alerting_msg:
  case leaf_setup_failure_msg:
  case leaf_setup_request_msg:
    
  case UNASSIGNED_MSG_TYPE: // freeform messages

      return 1;
    default:
      return 0;
  }
}


header_parser::~header_parser() 
{
  //*** I made the buffer so I'll free it
  if ((_duplicated_header) || (!header_parser::_up)) {
    if (!_external_buffer)
      delete [] _header_buffer;
    _header_buffer = 0L;
  }
  if (_errors) {
    delete _errors;
    _errors = 0L;
  }
}


//----------------------------------------------------------
header_parser_errmsg::header_parser_errmsg(header_error_code x)
  : _header_error(x) { }

char header_parser_errmsg::successful(void)
{
  return (_header_error == no_header_parser_errmsg);
}

void header_parser_errmsg::Print(ostream & os)
{
  os << "*** Message-Header Errors ***\n";
  
  switch (_header_error) {
    case unassigned_call_ref_value:
      os << "unassigned call ref value";
      break;
    case unassigned_action:
      os << "unassigned action";
      break;
    case unassigned_flag:
      os << "unassigned flag";
      break;
    case wrong_discriminator:
      os << "wrong discriminator";
      break;
    case bad_cref_len:
      os << "bad cref len";
      break;
    case invalid_msg_type:
      os << "invalid msg type";
      break;
    case invalid_msg_mode:
      os << "invalid msg mode";
      break;
    case inconsistent_msg_len:
      os << "inconsistent msg len";
      break;
    case below_min_len:
      os << "below min len";
      break;
    case above_max_len:
      os << "above max len";
      break;
    case no_header_parser_errmsg:
      os << "no header parser errmsg";
      break;
    default:
      os << "unknown header error!";
  }
  os << "\n";
}

header_parser_errmsg * header_parser::process(void)
{
  if (_up) return ( _errors = decode_msg_header() );
  else return ( _errors = encode_msg_header() );
}

//----------------------------------------------------------
header_parser_errmsg * header_parser::encode_msg_header(void)
{
  u_char * temp = _header_buffer;

  // stick in the protocol discriminator
  *temp++ = protocol_id;

  // stick in the call reference value length in bytes (3 bytes)
  *temp++ = 0x03;

  // stick in the call reference value itself
  // get first byte of _call_ref_value

  int call_ref_value = _call_ref_value;
  if (_call_ref_flag == 1)
    *temp++ = (u_char) ((call_ref_value >> 16) & 0x00FF);
  else
    *temp++ = (u_char) ((call_ref_value >> 16) & 0x007F);
    
  // get second byte of _call_ref_value
  *temp++ = (u_char) ((call_ref_value >> 8) & 0x00FF);

  // get third byte of _call_ref_value
  *temp++ = (u_char) (call_ref_value & 0x00FF);

  // stick in msg type (2 bytes), 2nd byte is 0x80 see note 1, p189 of UNI-3.1
  *temp++ = (u_char) _action;
  *temp++ = 0x80;

  // stick in the length of the UNI message ( 2 bytes)
  assert (msg_body_buflen() >= 0);

  u_short len = msg_body_buflen();

  // get first byte of len
  *temp++ = (u_char)((len >> 8) & 0xff);

  // get second byte of len
  *temp++ = (u_char)(len & 0xff);

  _header_valid = 1;

  return (new header_parser_errmsg(header_parser_errmsg::no_header_parser_errmsg));
}


//----------------------------------------------------------
header_parser_errmsg *header_parser::decode_msg_header(void)
{
  int cref_len;
  u_char *buffer = _header_buffer;
  // extract the protocol descriminator 1 byte
  _proto_id = (protocol_discriminator)(*buffer);
  buffer++;
  if(_proto_id != header_parser::protocol_id)
    return( new header_parser_errmsg(header_parser_errmsg::wrong_discriminator));
  // extract byte 2 which must be 0x03
  cref_len = *buffer++;
  if(cref_len != 0x03)
    return( new header_parser_errmsg(header_parser_errmsg::bad_cref_len));
  // extract call reference flag and value use XOR to get rid of the flag bit.
  _call_ref_flag  =  (*buffer & 0x80);
  int call_ref_value;
  call_ref_value =  (*buffer++) << 16;
  call_ref_value |= (*buffer++) << 8;
  call_ref_value |= *buffer++;
  _call_ref_value = call_ref_value;
  // extract message type and mode
  _action = (msg_type)(*buffer);
  buffer++;
  if(!is_msg_valid(_action))
    return( new header_parser_errmsg(header_parser_errmsg::invalid_msg_type));
  _msg_mode = (action_indicator)(*buffer);
  buffer++;
  if(_msg_mode != 0x80)
    return( new header_parser_errmsg(header_parser_errmsg::invalid_msg_mode));
  // extract msg length 2 bytes and check consistency
  u_short len;
  len = *buffer++ << 8;
  len |= *buffer;
  msg_body_buflen() = len;
  if( msg_body_buflen() != ( msg_total_buflen() - msg_header_len)) {
    cout << "inconsistent_msg_len:   msg_body_buflen() = " << msg_body_buflen() << endl;
    cout << "inconsistent_msg_len:   msg_total_buflen() = " << msg_total_buflen() << endl;
    cout << "inconsistent_msg_len:   msg_header_len = " << msg_header_len << endl;
    return( new header_parser_errmsg(header_parser_errmsg::inconsistent_msg_len));
  }
  if( msg_total_buflen() < min_len())
    return( new header_parser_errmsg(header_parser_errmsg::below_min_len));
  if( msg_total_buflen() > max_len())
    return( new header_parser_errmsg(header_parser_errmsg::above_max_len));
  // header is OK 
  _header_valid=1;
  return(new header_parser_errmsg(header_parser_errmsg::no_header_parser_errmsg));
}


enum header_parser::msg_type MessageType(const u_char * buf, int len)
{
  if ((buf) && (len >= 5)) 
    return ((enum header_parser::msg_type)(buf[5]));
  return (header_parser::UNASSIGNED_MSG_TYPE);
}


enum header_parser::msg_type header_parser::type(void)
{
  return _action;
}


header_parser::header_parser(header_parser& him, u_char* buf, int len)
: _call_ref_value(him._call_ref_value), _call_ref_flag(him._call_ref_flag), 
  _action(him._action), _up(0), _header_valid(0), _errors(0), _proto_id(protocol_id),
  _external_buffer(0)
{
  _header_buffer = buf;
  _header_buflen = len;
  _duplicated_header = 1;
}


void header_parser::set_external_buffer_on(void) {
  _external_buffer = 1;
}

#endif // _HEADER_PARSER_CC_



