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
#ifndef _FREEFORM_CC_
#define _FREEFORM_CC_

#ifndef LINT
static char const _freeform_cc_rcsid_[] =
"$Id: freeform.cc,v 1.9 1998/12/15 15:17:08 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/q93b_msg/freeform.h>


//------------------------------------
int q93b_freeform_message::min_len(void) const
{ return min_size; }

//------------------------------------
int q93b_freeform_message::max_len(void) const
{ return max_size; }



//--------------------------------------------------------------
q93b_freeform_message::q93b_freeform_message(InfoElem ** ie_array, 
					     u_long call_ref_value, 
					     u_int flag)
  : generic_q93b_msg(ie_array, UNASSIGNED_MSG_TYPE, call_ref_value, flag)
{
  SetupLegal();
  SetupMandatory();
  process();
}

//--------------------------------------------------------------
q93b_freeform_message::q93b_freeform_message(InfoElem** ie_array, 
					     u_long call_ref_value,
					     u_int flag,
					     Buffer* buf)
: generic_q93b_msg(ie_array, UNASSIGNED_MSG_TYPE, call_ref_value, flag,buf)
{
  SetupLegal();
  SetupMandatory();
  process();
}


//--------------------------------------------------------------
q93b_freeform_message::q93b_freeform_message(const u_char* buf, int len)
  : generic_q93b_msg(buf, len)
{
  SetupLegal();
  SetupMandatory();
  process();
}


//--------------------------------------------------------------
void q93b_freeform_message::SetupLegal(void)
{
  _ie_legal[InfoElem::ie_cause_ix]=1;
  _ie_legal[InfoElem::ie_call_state_ix]=1;
  _ie_legal[InfoElem::ie_end_pt_ref_ix]=1;
  _ie_legal[InfoElem::ie_end_pt_state_ix]=1;
  _ie_legal[InfoElem::ie_aal_param_ix]=1;
  _ie_legal[InfoElem::ie_traffic_desc_ix]=1;
  _ie_legal[InfoElem::ie_conn_identifier_ix]=1;
  _ie_legal[InfoElem::ie_qos_param_ix]=1;
  _ie_legal[InfoElem::ie_bhli_ix]=1;
  _ie_legal[InfoElem::ie_broadband_bearer_cap_ix]=1;
  _ie_legal[InfoElem::ie_blli_ix]=1;
  _ie_legal[InfoElem::ie_broadband_locking_shift_ix]=1;
  _ie_legal[InfoElem::ie_broadband_nlocking_shift_ix]=1;
  _ie_legal[InfoElem::ie_broadband_send_comp_ind_ix]=1;
  _ie_legal[InfoElem::ie_broadband_repeat_ind_ix]=1;
  _ie_legal[InfoElem::ie_calling_party_num_ix]=1;
  _ie_legal[InfoElem::ie_calling_party_subaddr_ix]=1;
  _ie_legal[InfoElem::ie_called_party_num_ix]=1;
  _ie_legal[InfoElem::ie_called_party_subaddr_ix]=1;
  _ie_legal[InfoElem::ie_transit_net_sel_ix]=1;
  _ie_legal[InfoElem::ie_restart_ind_ix]=1;
#ifdef UNI40 // UNI40
  _ie_legal[InfoElem::UNI40_e2e_transit_delay_ix]=1;
  _ie_legal[InfoElem::UNI40_generic_id_ix]=1;
  _ie_legal[InfoElem::UNI40_leaf_call_id_ix]=1;
  _ie_legal[InfoElem::UNI40_leaf_params_ix]=1;
  _ie_legal[InfoElem::UNI40_leaf_sequence_num_ix]=1;
  _ie_legal[InfoElem::PNNI_called_party_soft_pvpc_ix]=1;
  _ie_legal[InfoElem::PNNI_crankback_ix]=1;
  _ie_legal[InfoElem::PNNI_designated_transit_list_ix]=1;
  _ie_legal[InfoElem::PNNI_calling_party_soft_pvpc_ix]=1;
  _ie_legal[InfoElem::UNI40_abr_setup_ix]=1;
  _ie_legal[InfoElem::UNI40_abr_params_ix]=1;
  _ie_legal[InfoElem::UNI40_conn_scope_ix]=1;
  _ie_legal[InfoElem::UNI40_xqos_param_ix]=1;
  _ie_legal[InfoElem::UNI40_notification_ind_ix]=1;
  _ie_legal[InfoElem::UNI40_alt_traff_desc_ix]=1;
  _ie_legal[InfoElem::UNI40_min_traff_desc_ix]=1;
#endif // UNI40
  //  _ie_legal[InfoElem::ie_unknown_ix]=1;	/* Bad index: -1 */
}


//--------------------------------------------------------------
void q93b_freeform_message::SetupMandatory(void)
{
}


//--------------------------------------------------------------
generic_q93b_msg* q93b_freeform_message::copy(void)
{
  return (new q93b_freeform_message(*this));
}


//--------------------------------------------------------------
q93b_freeform_message::q93b_freeform_message(q93b_freeform_message & him) 
  : generic_q93b_msg(him, header_parser::UNASSIGNED_MSG_TYPE, 
		     new u_char [him.msg_total_buflen()], 
		     him.msg_total_buflen()) 
{
  SetupLegal();
  SetupMandatory();
  re_encode();
}

#endif // _FREEFORM_CC_
