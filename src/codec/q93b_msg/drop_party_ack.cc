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
#ifndef _DROP_PARTY_ACK_CC_
#define _DROP_PARTY_ACK_CC_

#ifndef LINT
static char const _drop_party_ack_cc_rcsid_[] =
"$Id: drop_party_ack.cc,v 1.8 1998/12/15 15:22:40 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/q93b_msg/drop_party_ack.h>

//------------------------------------
int q93b_drop_party_ack_message::min_len(void) const 
{ return min_size; }

//------------------------------------
int q93b_drop_party_ack_message::max_len(void) const 
{ return max_size; }

//--------------------------------------------------------------
q93b_drop_party_ack_message::q93b_drop_party_ack_message(InfoElem ** ie_array, 
							 u_long call_ref_value, 
							 u_int flag)
  : generic_q93b_msg(ie_array, drop_party_ack_msg, call_ref_value, flag)
{
  SetupLegal();
  SetupMandatory();
  process();
}

//--------------------------------------------------------------
q93b_drop_party_ack_message::q93b_drop_party_ack_message(InfoElem ** ie_array, 
							 u_long call_ref_value,
							 u_int flag,
							 Buffer * buf)
  : generic_q93b_msg(ie_array, drop_party_ack_msg, call_ref_value, flag,buf)
{
  SetupLegal();
  SetupMandatory();
  process();
}


//--------------------------------------------------------------
q93b_drop_party_ack_message::q93b_drop_party_ack_message(const u_char * buf, int len)
  : generic_q93b_msg(buf,len)
{
  SetupLegal();
  SetupMandatory();
  process();
}


//--------------------------------------------------------------
void q93b_drop_party_ack_message::SetupLegal(void)
{
  _ie_legal[InfoElem::ie_cause_ix]         = 1;
  _ie_legal[InfoElem::ie_end_pt_ref_ix]    = 1;
#ifdef UNI40
  _ie_legal[InfoElem::UNI40_generic_id_ix] = 1;
#endif
}


//--------------------------------------------------------------
void q93b_drop_party_ack_message::SetupMandatory(void)
{
  _ie_legal[InfoElem::ie_end_pt_ref_ix] = 1;
}


//--------------------------------------------------------------
generic_q93b_msg* q93b_drop_party_ack_message::copy(void)
{
  return (new q93b_drop_party_ack_message(*this));
}


//--------------------------------------------------------------
q93b_drop_party_ack_message::q93b_drop_party_ack_message(q93b_drop_party_ack_message & him) 
  : generic_q93b_msg(him, header_parser::drop_party_ack_msg, 
		     new u_char [him.msg_total_buflen()], 
		     him.msg_total_buflen()) 
{
  SetupLegal();
  SetupMandatory();
  re_encode();
}

#endif // _DROP_PARTY_ACK_CC_
