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
#ifndef _GENERIC_Q93B_MSG_CC_
#define _GENERIC_Q93B_MSG_CC_

#ifndef LINT
static char const _generic_q93b_msg_cc_rcsid_[] =
"$Id: generic_q93b_msg.cc,v 1.24 1998/12/15 15:20:46 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <generic_q93b_msg.h>

#include <codec/q93b_msg/alerting.h>
#include <codec/q93b_msg/add_party.h>
#include <codec/q93b_msg/drop_party.h>         
#include <codec/q93b_msg/release_comp.h>
#include <codec/q93b_msg/add_party_ack.h>         
#include <codec/q93b_msg/drop_party_ack.h>        
#include <codec/q93b_msg/restart.h>
#include <codec/q93b_msg/add_party_rej.h>         
#include <codec/q93b_msg/party_alerting.h>
#include <codec/q93b_msg/freeform.h>          
#include <codec/q93b_msg/restart_ack.h>
#include <codec/q93b_msg/notify.h>
#include <codec/q93b_msg/setup.h>
#include <codec/q93b_msg/call_proceeding.h>
#include <codec/q93b_msg/status.h>
#include <codec/q93b_msg/connect.h>          
#include <codec/q93b_msg/status_enq.h>
#include <codec/q93b_msg/connect_ack.h>          
#include <codec/q93b_msg/release.h>
#include <codec/q93b_msg/leaf_setup.h>
#include <codec/q93b_msg/Buffer.h>


InfoElem * generic_q93b_msg::ie(InfoElem::ie_ix x) const
{
  if (x != InfoElem::ie_unknown_ix)
    return _ie_array[x];
  return 0L;
}

InfoElem * generic_q93b_msg::steal_ie(InfoElem::ie_ix x)
{
  if (x != InfoElem::ie_unknown_ix) {
    InfoElem * ans = _ie_array[x];
    _ie_array[x] = 0L;
    return ans;
  }
  return 0L;
}

u_char* generic_q93b_msg::encoded_buffer(void)
{
  if (Valid()) return(_header_buffer);
  else return(0L);
}

int generic_q93b_msg::encoded_bufferlen(void)
{
  if (Valid()) return(msg_total_buflen());
  else return(0);
}

int  generic_q93b_msg::msg_header_buflen(void) { return msg_header_len; }

int& generic_q93b_msg::msg_body_buflen(void)   { return _body_buflen; }

int& generic_q93b_msg::msg_total_buflen(void)  { return _total_buflen; }

char generic_q93b_msg::Valid(void)
{
  return(_msg_valid);
}

void generic_q93b_msg::UniDump(const char* prefix)
{
  u_char* buf = encoded_buffer();
  if(!buf)
    return;
  int len = msg_total_buflen();
  char *tab = 0;
  if(prefix)
    {
      int plen = strlen(prefix);
      printf("%s",prefix);
      tab = new char[plen+1];
      for(int k = 0; k < plen; k++)
	tab[k] = ' ';
      tab[plen] = '\0';
    }

  char trueprefix[50];
  if (prefix) strcpy(trueprefix,prefix);
  else strcpy(trueprefix,"");

  // print header
  printf("%02x%02x %02x%02x%02x %02x%02x %02x%02x\n",
	 buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8]);
  int body_len = (buf[7] << 8) | buf[8];
  // protect against bad length in message
  if(body_len > len)
    body_len = len -9;
  int k = 9;
  while(body_len > 0)
    {
      if(tab)
	printf("%s",tab);
      printf("%02x %02x %02x%02x : ",buf[k],buf[k+1],buf[k+2],buf[k+3]);
      int ie_len = (buf[k+2] << 8) | buf[k+3];

      // protect against bad ie length 
      if(ie_len > body_len)
	ie_len = body_len -4;

      k += 4;
      for(int j = 0; j < ie_len; j++)
	printf("%02x ",buf[k+j]);
      printf("\n");
      k+= ie_len;
      body_len -= (ie_len + 4);
      if( body_len < 4)
	break;
    }
  if(body_len)
    printf("corrupted  message -- length inconsistent with contents\n");
}
 
void generic_q93b_msg::HexDump(ostream& os, const char* prefix)
{
  u_char* buf=encoded_buffer();
  int len=msg_total_buflen();
  
  char trueprefix[50];
  if (prefix) strcpy(trueprefix,prefix);
  else strcpy(trueprefix,"");
  
  int val;
    
  os << trueprefix << "length=" << dec << len << endl;
  if (buf != 0) {
    for (int i=0;i<len;i++){

      if (i==0) os << trueprefix;
      if ((i!=0) && (i            == 0)) os << endl << trueprefix;
      
      val = 0;
      val = (int)(buf[i]);
      if (val<16) os << "0";
      os << hex << val;
      
      if ((i ) == 1) os << " ";
    }
  } else {
    os << trueprefix << "Invalid message" << endl;
  }
  
  os << dec << "";
}

int generic_q93b_msg::min_len(void) const
{ return MIN_UNI_MESSAGE_LENGTH; }

int generic_q93b_msg::max_len(void) const
{ return MAX_UNI_MESSAGE_LENGTH; }

void generic_q93b_msg::PrintErrors(ostream & os)
{
  os << "---- Errors ----" << endl;
  header_parser::list_errors(os);
  body_parser::list_errors(os);
}

void generic_q93b_msg::PrintSymbolic(ostream& os)
{
  os << "---- Symbolic Description ----" << endl;
  header_parser::Print(os);
  body_parser::Print(os);
}

void generic_q93b_msg::PrintEncoded(ostream& os)
{
  os << "---- Hex Encoding ----" << endl;
  HexDump(os,0L);
}

void generic_q93b_msg::Print(ostream& os)
{
  os << ">>>> Q93B MESSAGE <<<<" << endl;  
  PrintSymbolic(os);
  PrintEncoded(os);
  PrintErrors(os);  
}

void Print_Type(ostream& os, header_parser::msg_type t)
{
  switch (t) 
    {
    case generic_q93b_msg::alerting_msg:
      os << "<alerting_msg>";
      break;
    case generic_q93b_msg::connect_msg:
      os << "<connect_msg>";
      break;
    case generic_q93b_msg::setup_msg:
      os << "<setup_msg>";
      break;
    case generic_q93b_msg::release_msg:
      os << "<release_msg>";
      break;
    case generic_q93b_msg::release_comp_msg:
      os << "<release_comp_msg>";
      break;
    case generic_q93b_msg::add_party_ack_msg:
      os << "<add_party_ack_msg>";
      break;
    case generic_q93b_msg::add_party_rej_msg:
      os << "<add_party_rej_msg>";
      break;
    case generic_q93b_msg::drop_party_ack_msg:
      os << "<drop_party_ack_msg>";
      break;
    case generic_q93b_msg::call_proceeding_msg:
      os << "<call_proceeding_msg>";
      break;
    case generic_q93b_msg::connect_ack_msg:
      os << "<connect_ack_msg>";
      break;
    case generic_q93b_msg::status_msg:
      os << "<status_msg>";
      break;
    case generic_q93b_msg::status_enq_msg:
      os << "<status_enq_msg>";
      break;
    case generic_q93b_msg::add_party_msg:
      os << "<add_party_msg>";
      break;
    case generic_q93b_msg::drop_party_msg:
      os << "<drop_party_msg>";
      break;
    case generic_q93b_msg::restart_msg:
      os << "<restart_msg>";
      break;
    case generic_q93b_msg::restart_ack_msg:
      os << "<restart_ack_msg>";
      break;
    case generic_q93b_msg::party_alerting_msg:
      os << "<party_alerting_msg>";
      break;
    case generic_q93b_msg::notify_msg:
      os << "<notify_msg>";
      break;
    case generic_q93b_msg::leaf_setup_failure_msg:
      os << "<leaf_setup_failure_msg>";
      break;
    case generic_q93b_msg::leaf_setup_request_msg:
      os << "<leaf_setup_request_msg>";
      break;
    default:
      os << "<unknown>";
      break;
    }
}

//-------------------------------------------------------------
generic_q93b_msg::generic_q93b_msg(const u_char * buffer, u_int buflen)
  : header_parser(buffer, msg_header_len),
    body_parser(buffer + msg_header_len,buflen - msg_header_len),
    _total_buflen(buflen), _msg_valid(0)
{
}


//-------------------------------------------------------------
generic_q93b_msg::generic_q93b_msg(InfoElem** ie_array, msg_type t, u_long call_ref_value, u_int flag)
  : header_parser(call_ref_value,t,flag),
    body_parser(ie_array),
  _msg_valid(0),_total_buflen(0)
{
  int sz = max_len();
  _header_buffer = new u_char [ sz ];
  _header_buflen = msg_header_len;

  for (int j=0;j<sz;j++) _header_buffer[j]=0;

  _body_buffer = _header_buffer + msg_header_len;
  _body_buflen = max_len() - msg_header_len;
}

//-------------------------------------------------------------
generic_q93b_msg::generic_q93b_msg(InfoElem** ie_array, 
				   msg_type t, 
				   u_long call_ref_value, 
				   u_int flag,
				   Buffer* buf)
  : header_parser(call_ref_value,t,flag,buf),
    body_parser(ie_array,buf),
  _msg_valid(0),_total_buflen(0)
{
  int sz = max_len();
  _header_buffer = buf->data();
  _header_buflen = msg_header_len;

  for (int j=0;j<sz;j++) _header_buffer[j]=0;

  _body_buffer = _header_buffer + msg_header_len;
  _body_buflen = max_len() - msg_header_len;
}

//-------------------------------------------------------------
header_parser_errmsg* generic_q93b_msg::re_encode_header(void)
{
  int sz = max_len();
  for (int j=0;j<msg_header_len;j++) _header_buffer[j]=0;
  _header_buflen = msg_header_len;

  header_parser_errmsg* old,*header_errors=0L;
  if (old=header_parser::_errors) {
    //    header_parser::_errors=0L;
    delete old;
    old=0L;
  }

  header_errors = header_parser::encode_msg_header();
  header_parser::_errors = header_errors;
  return header_errors;
}

//-------------------------------------------------------------
body_parser_errmsg* generic_q93b_msg::re_encode_body(void)
{
  body_parser_errmsg* old,*body_errors=0L;
  if (old=body_parser::_errors) {
    delete old;
    old=0L;
  }
  body_parser::_errors = new body_parser_errmsg();

  body_errors = body_parser::process_encode();

  body_parser::_errors = body_errors;
  return body_errors;
}

//-------------------------------------------------------------
void generic_q93b_msg::re_encode(void)
{
  _msg_valid=0;

  if (!_external_buffer) {
    delete [] _header_buffer;
    int sz = max_len();
    _header_buffer = new u_char [ sz ];
    _header_buflen = msg_header_len;
    for (int j=0;j<sz;j++) _header_buffer[j]=0;
    _body_buffer = _header_buffer + msg_header_len;
    _body_buflen = max_len() - msg_header_len;
  }

  body_parser_errmsg*  body_errors    = re_encode_body();
  header_parser_errmsg* header_errors = re_encode_header();

  if ((header_errors->successful()) &&
      (body_errors->successful())) {
    _msg_valid=1;
    _total_buflen=msg_header_buflen()+msg_body_buflen();
  }
}

//-------------------------------------------------------------
generic_q93b_msg::~generic_q93b_msg()
{
}

//-------------------------------------------------------------
void generic_q93b_msg::process(void)
{
  
  body_parser_errmsg* body_errors=0L;
  header_parser_errmsg* header_errors=0L;

  body_errors = body_parser::process();
  header_errors = header_parser::process();

  if ((header_errors->successful()) &&
      (body_errors->successful())) {
    _msg_valid=1;
    _total_buflen=msg_header_buflen()+msg_body_buflen();
  }
}


//-------------------------------------------------------------
generic_q93b_msg * Parse(const u_char * buf, int len)
{
  if (len == 0) 
    return 0L;

  generic_q93b_msg * answer = 0L;
  header_parser::msg_type t;
  t = MessageType(buf, len);

  switch (t) {
    case header_parser::UNASSIGNED_MSG_TYPE:
      answer = new q93b_freeform_message(buf, len);
      break;

    case header_parser::alerting_msg:
      answer = new q93b_alerting_message(buf, len);
      break;

    case header_parser::call_proceeding_msg:
      answer = new q93b_call_proceeding_message(buf, len);
      break;

    case header_parser::setup_msg:
      answer = new q93b_setup_message(buf, len);
      break;

    case header_parser::connect_msg:
      answer = new q93b_connect_message(buf, len);
      break;

    case header_parser::notify_msg:
      answer = new q93b_notify_message(buf, len);
      break;

    case header_parser::connect_ack_msg:
      answer = new q93b_connect_ack_message(buf, len);
      break;

    case header_parser::release_msg:
      answer = new q93b_release_message(buf, len);
      break;

    case header_parser::release_comp_msg:
      answer = new q93b_release_comp_message(buf, len);
      break;

    case header_parser::restart_msg:
      answer = new q93b_restart_message(buf, len);
      break;

    case header_parser::restart_ack_msg:
      answer = new q93b_restart_ack_message(buf, len);
      break;

    case header_parser::status_msg:
      answer = new q93b_status_message(buf, len);
      break;

    case header_parser::status_enq_msg:
      answer = new q93b_status_enq_message(buf, len);
      break;

    case header_parser::add_party_msg:
      answer = new q93b_add_party_message(buf, len);
      break;

    case header_parser::add_party_ack_msg:
      answer = new q93b_add_party_ack_message(buf, len);
      break;

    case header_parser::add_party_rej_msg:
      answer = new q93b_add_party_rej_message(buf, len);
      break;

    case header_parser::drop_party_msg:
      answer = new q93b_drop_party_message(buf, len);
      break;

    case header_parser::drop_party_ack_msg:
      answer = new q93b_drop_party_ack_message(buf, len);
      break;

    case header_parser::party_alerting_msg:
      answer = new q93b_party_alerting_message(buf, len);
      break;

    case header_parser::leaf_setup_failure_msg:
      answer = new UNI40_leaf_setup_failure_message(buf, len);
      break;

    case header_parser::leaf_setup_request_msg:
      answer = new UNI40_leaf_setup_request_message(buf, len);
      break;

    default:
      answer = 0L;
      break;
  }

  if (answer==0) {
    cout << "Bilal: Unable to parse this buffer\n";
    for (int j=0;j<len;j++)
      printf(" %x",buf[j]);
    cout << endl;
  }
  if (answer && (!answer->Valid())) {
    cout << "Bilal: parsed buffer, but found errors\n";
    answer->PrintSymbolic(cout);
    cout << endl;
  }

  return answer;
}

generic_q93b_msg::generic_q93b_msg(generic_q93b_msg& him, header_parser::msg_type t, u_char* buf, int len) :
  header_parser(him,buf,msg_header_len),
  body_parser(him,buf+msg_header_len,len-msg_header_len),
  _total_buflen(him._total_buflen),
  _msg_valid(him._msg_valid)
{
  _action = t;
}

#endif // _GENERIC_Q93B_MSG_CC_
