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

#if !defined(LINT)
static char const _call_state_cc_rcsid_[] =
"$Id: call_state.cc,v 1.9 1998/09/28 09:12:00 bilal Exp $";
#endif
#include <common/cprototypes.h>
#include <codec/uni_ie/call_state.h>
#include <codec/uni_ie/errmsg.h>

/*
 * see 5.4.5.10 Call State, page 218 UNI-3.1 book
 * This information element has 1 byte length.
 */
ie_call_state::ie_call_state(call_state_values state_value) : InfoElem(ie_call_state_id),
  _state_value(state_value) { }

ie_call_state::ie_call_state(const ie_call_state & rhs) : InfoElem(ie_call_state_id),
    _state_value(rhs._state_value) { }

ie_call_state::ie_call_state(void) : InfoElem(ie_call_state_id),
      _state_value(u0_null) { }

ie_call_state::~ie_call_state() { }


InfoElem* ie_call_state::copy(void) const {return new ie_call_state(_state_value);}

int ie_call_state::operator == (const ie_call_state & rs) const
{
  return equals(&rs);
}

int ie_call_state::equals(const ie_call_state *himptr) const
{
  ie_call_state rs = *himptr;
  return (rs._state_value == _state_value);
}

int ie_call_state::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}
  
int ie_call_state::Length()
{
  return(ie_header_len+1);
}

int ie_call_state::encode(u_char * buffer)
{
  u_char * temp;
  int buflen = 0;
  
  if ((temp = LookupCache()) != 0L)
    {
      bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
      buffer += buflen;
      return(buflen);
    }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);
  put_coding(buffer,_coding);
  put_8(temp,buflen,_state_value);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

InfoElem::ie_status ie_call_state::decode(u_char * buffer, int & id)
{
  int call_state;
  id = buffer[0];
  short len = get_len(buffer);
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 1)
    return (InfoElem::invalid_contents);
  buffer += ie_header_len;
  get_8(buffer,call_state);
  switch(call_state)
    {
    case u0_null:
    case u1_call_initiated:
      // case u2_overlap_sending:
    case u3_out_going_call_proceeding:
      // case u4_call_delivered:
    case u6_call_present:
      // case u7_call_received:
    case u8_connect_request:
    case u9_incoming_call_proceeding:
    case u10_active:
    case u11_release_request:
    case u12_release_indication:
      // case u25_overlap_receiving:
    case rest1_restart_request:
    case rest2_restart:
      _state_value = (call_state_values) call_state;
      return(InfoElem::ok);
      break;

    default:
      return (InfoElem::invalid_contents);
      break;
    }
  return (InfoElem::ok);
}

enum ie_call_state::call_state_values ie_call_state::get_state_value(void) {return _state_value;}

void ie_call_state::set_state_value(call_state_values state_value)
{
  _state_value = state_value;
  MakeStale();
}

char ie_call_state::PrintSpecific(ostream & os)
{
  os << endl << "   state::";
  switch (_state_value) {
  case u0_null: os << "u0_null " << endl; break;
  case u1_call_initiated: os << "u1_call_initiated " << endl; break;
  case u2_overlap_sending: os << "u2_overlap_sending " << endl; break;
  case u3_out_going_call_proceeding: 
    os << "u3_out_going_call_proceeding " << endl; break;
  case u4_call_delivered: os << "u4_call_delivered " << endl; break;
  case u6_call_present: os << "u6_call_present " << endl; break;
  case u7_call_received: os << "u7_call_received " << endl; break;
  case u8_connect_request: os << "u8_connect_request " << endl; break;
  case u9_incoming_call_proceeding: 
    os << "u9_incoming_call_proceeding " << endl; break;
  case u10_active: os << "u10_active " << endl; break;
  case u11_release_request: os << "u11_release_request " << endl; break;
  case u12_release_indication: os << "u12_release_indication " << endl; break;
  case u25_overlap_receiving: os << " u25_overlap_receiving " << endl; break;
  case rest1_restart_request: os << "rest1_restart_request " << endl; break;
  case rest2_restart: os << "rest2_restart " << endl; break;
  default: os << "unknown " << endl; break;
  }
  return 0;
}

ostream & operator << (ostream & os, ie_call_state & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}



