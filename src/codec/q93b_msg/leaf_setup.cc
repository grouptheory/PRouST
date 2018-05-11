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
#ifndef _LEAF_SETUP_CC_
#define _LEAF_SETUP_CC_

#ifndef LINT
static char const _leaf_setup_cc_rcsid_[] =
"$Id: leaf_setup.cc,v 1.9 1998/12/15 15:26:34 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/q93b_msg/leaf_setup.h>


//------------------------------------
int UNI40_leaf_setup_failure_message::min_len(void) const
{ return min_size; }

//------------------------------------
int UNI40_leaf_setup_failure_message::max_len(void) const
{ return max_size; }


//--------------------------------------------------------------
UNI40_leaf_setup_failure_message::
UNI40_leaf_setup_failure_message(InfoElem ** ie_array, 
				 u_long call_ref_value, 
				 u_int flag)
  : generic_q93b_msg(ie_array, leaf_setup_failure_msg, call_ref_value, flag)
{
  SetupLegal();
  SetupMandatory();
  process();
}


//--------------------------------------------------------------
UNI40_leaf_setup_failure_message::
UNI40_leaf_setup_failure_message(InfoElem ** ie_array, 
				 u_long call_ref_value, 
				 u_int flag,
				 Buffer* buf)
  : generic_q93b_msg(ie_array, leaf_setup_failure_msg, call_ref_value, flag, buf)
{
  SetupLegal();
  SetupMandatory();
  process();
}


//--------------------------------------------------------------
UNI40_leaf_setup_failure_message::UNI40_leaf_setup_failure_message(const u_char* buf, int len)
  : generic_q93b_msg(buf,len)
{
  SetupLegal();
  SetupMandatory();
  process();
}

// This whole message is UNI 4.0 so don't look for UNI40
//--------------------------------------------------------------
void UNI40_leaf_setup_failure_message::SetupLegal(void)
{ 
  _ie_legal[InfoElem::ie_cause_ix]                = 1;
  _ie_legal[InfoElem::ie_called_party_num_ix]     = 1;
  _ie_legal[InfoElem::ie_called_party_subaddr_ix] = 1;
  _ie_legal[InfoElem::UNI40_leaf_sequence_num_ix] = 1;
  _ie_legal[InfoElem::ie_transit_net_sel_ix]      = 1;
}


//--------------------------------------------------------------
void UNI40_leaf_setup_failure_message::SetupMandatory(void)
{
  _ie_mandatory[InfoElem::ie_cause_ix]                = 1;
  _ie_mandatory[InfoElem::UNI40_leaf_sequence_num_ix] = 1;
}

//--------------------------------------------------------------
generic_q93b_msg* UNI40_leaf_setup_failure_message::copy(void)
{
  //  int maxlen = max_len();
  //  u_char* buf = new u_char [maxlen];
  return (new UNI40_leaf_setup_failure_message(*this));
}


//--------------------------------------------------------------
UNI40_leaf_setup_failure_message::
UNI40_leaf_setup_failure_message(UNI40_leaf_setup_failure_message & him) 
  : generic_q93b_msg(him, header_parser::leaf_setup_failure_msg, 
		     new u_char [him.msg_total_buflen()], 
		     him.msg_total_buflen()) 
{
  SetupLegal();
  SetupMandatory();
  re_encode();
}


//------------------------------------
int UNI40_leaf_setup_request_message::min_len(void) const 
{ return min_size; }

//------------------------------------
int UNI40_leaf_setup_request_message::max_len(void) const 
{ return max_size; }


//--------------------------------------------------------------
UNI40_leaf_setup_request_message::
UNI40_leaf_setup_request_message(InfoElem ** ie_array, 
				 u_long call_ref_value, u_int flag) 
  : generic_q93b_msg(ie_array, leaf_setup_request_msg, call_ref_value, flag)
{
  SetupLegal();
  SetupMandatory();
  process();
}

//--------------------------------------------------------------
UNI40_leaf_setup_request_message::
UNI40_leaf_setup_request_message(InfoElem ** ie_array, 
				 u_long call_ref_value, 
				 u_int flag,
				 Buffer * buf) 
  : generic_q93b_msg(ie_array, leaf_setup_request_msg, call_ref_value, flag, buf)
{
  SetupLegal();
  SetupMandatory();
  process();
}


//--------------------------------------------------------------
UNI40_leaf_setup_request_message::UNI40_leaf_setup_request_message(const u_char * buf, int len) 
  : generic_q93b_msg(buf,len)
{
  SetupLegal();
  SetupMandatory();
  process();
}


//--------------------------------------------------------------
void UNI40_leaf_setup_request_message::SetupLegal(void)
{ 
  _ie_legal[InfoElem::ie_transit_net_sel_ix]       = 1;
  _ie_legal[InfoElem::ie_calling_party_num_ix]     = 1;
  _ie_legal[InfoElem::ie_calling_party_subaddr_ix] = 1;
  _ie_legal[InfoElem::ie_called_party_num_ix]      = 1;
  _ie_legal[InfoElem::ie_called_party_subaddr_ix]  = 1;
  _ie_legal[InfoElem::UNI40_leaf_call_id_ix]       = 1;
  _ie_legal[InfoElem::UNI40_leaf_sequence_num_ix]  = 1;
}


//--------------------------------------------------------------
void UNI40_leaf_setup_request_message::SetupMandatory(void)
{
  _ie_mandatory[InfoElem::ie_calling_party_num_ix]    = 1;
  _ie_mandatory[InfoElem::ie_called_party_num_ix]     = 1;
  _ie_mandatory[InfoElem::UNI40_leaf_call_id_ix]      = 1;
  _ie_mandatory[InfoElem::UNI40_leaf_sequence_num_ix] = 1;
}

//--------------------------------------------------------------
generic_q93b_msg* UNI40_leaf_setup_request_message::copy(void)
{
  //int maxlen = max_len();
  //u_char* buf = new u_char [maxlen];
  return (new UNI40_leaf_setup_request_message(*this));
}


//--------------------------------------------------------------
UNI40_leaf_setup_request_message::
UNI40_leaf_setup_request_message(UNI40_leaf_setup_request_message & him) 
  : generic_q93b_msg(him, header_parser::leaf_setup_request_msg, 
		     new u_char [him.msg_total_buflen()], 
		     him.msg_total_buflen()) 
{
  SetupLegal();
  SetupMandatory();
  re_encode();
}

#endif // _LEAF_SETUP_CC_
