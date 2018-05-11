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
#ifndef _IE_C_
#define _IE_C_

#ifndef LINT
static char const _ie_cc_rcsid_[] =
"$Id: ie.cc,v 1.7 1998/09/28 09:12:00 bilal Exp $";
#endif
#include <common/cprototypes.h>

#define __UNI40__
#include <codec/uni_ie/ie_base.h>

InfoElem::InfoElem(ieid id) : Cache(), _uni_version(uni40), _id(id),_coding(itu), 
  _action_ind(clear_call),_first_of_many(0), _num_in_list(1), _next(0L),
  _last(0L) { }

InfoElem::~InfoElem() 
{
  // Safe to delete null pointers
  delete _next;
}


InfoElem::InfoElem(const InfoElem &him) : Cache()
{
  _id = him._id;
  _uni_version = him._uni_version;
  _coding = him._coding;
  _coding_mask = him._coding_mask;
  _flag = him._flag;
  _flag_mask = him._flag_mask;
  _action_ind = him._action_ind;
  _action_ind_mask = him._action_ind_mask;
  _next = 0;
  _last = 0;
  _first_of_many = 0;
  _num_in_list = 1;
}


enum InfoElem::ieid InfoElem::get_id(void) { return _id  ;}

bool InfoElem::InstanceOf(InfoElem::ieid id)
{
  if(id == _id)
    return true;
  return false;
}

void InfoElem::SetUniVersion(InfoElem::SignallingVersion v) { _uni_version = v;}

InfoElem::SignallingVersion  InfoElem::GetUniVersion() { return _uni_version;}


// this is redefined in blli and cause for 3 and 2 respectively
int InfoElem:: max_repeats(void) {return 1;}

char is_ie_valid(enum InfoElem::ieid x)
{
    return( (ieid_to_index(x)!=(-1)) );
}

void Print_IE_name(InfoElem::ie_ix ix, ostream& os){
  
  switch(ix)
    {
    case InfoElem::ie_cause_ix:
      os << "ie_cause";
      break;
    case InfoElem::UNI40_e2e_transit_delay_ix:
      os << "UNI40_e2e_transit_delay";
      break;
    case InfoElem::ie_call_state_ix:
      os << "ie_call_state";
      break;
    case InfoElem::UNI40_notification_ind_ix:
      os << "UNI40_notification_indicator";
      break;
    case InfoElem::ie_end_pt_ref_ix:
      os << "ie_end_pt_ref";
      break;
    case InfoElem::ie_end_pt_state_ix:
      os << "ie_end_pt_state";
      break;
    case InfoElem::ie_aal_param_ix:
      os << "ie_aal_param";
      break;
    case InfoElem::ie_traffic_desc_ix:
      os << "ie_traffic_desc";
      break;
    case InfoElem::UNI40_alt_traff_desc_ix:
      os << "UNI40_alt_traffic_desc";
      break;
    case InfoElem::UNI40_min_traff_desc_ix:
      os << "UNI40_min_traffic_desc";
      break;
    case InfoElem::ie_conn_identifier_ix:
      os << "ie_conn_identifier";
      break;
    case InfoElem::ie_qos_param_ix:
      os << "ie_qos_param";
      break;
    case InfoElem::ie_bhli_ix:
      os << "ie_bhli";
      break;
    case InfoElem::ie_broadband_bearer_cap_ix:
      os << "ie_bbc";
      break;
    case InfoElem::ie_blli_ix:
      os << "ie_blli";
      break;
    case InfoElem::ie_broadband_locking_shift_ix:
      os << "ie_broadband_locking_shift";
      break;
    case InfoElem::ie_broadband_nlocking_shift_ix:
      os << "ie_broadband_nlocking_shift";
      break;
    case InfoElem::ie_broadband_send_comp_ind_ix:
      os << "ie_broadband_send_comp_ind";
      break;
    case InfoElem::ie_broadband_repeat_ind_ix:
      os << "ie_broadband_repeat_ind";
      break;
    case InfoElem::ie_calling_party_num_ix:
      os << "ie_calling_party_num";
      break;
    case InfoElem::ie_calling_party_subaddr_ix:
      os << "ie_calling_party_subaddr";
      break;
    case InfoElem::ie_called_party_num_ix:
      os << "ie_called_party_num";
      break;
    case InfoElem::ie_called_party_subaddr_ix:
      os << "ie_called_party_subaddr";
      break;
    case InfoElem::ie_transit_net_sel_ix:
      os << "ie_transit_net_sel";
      break;
    case InfoElem::ie_restart_ind_ix:
      os << "ie_restart_ind";
      break;
    case InfoElem::UNI40_generic_id_ix:
      os << "ie_generic_id";
      break;
    case InfoElem::UNI40_leaf_call_id_ix:
      os << "ie_lij_call_id";
      break;
    case InfoElem::UNI40_leaf_params_ix:
      os << "ie_lij_params";
      break;
    case InfoElem::UNI40_leaf_sequence_num_ix:
      os << "UNI40_leaf_sequence_num";
      break;
    case InfoElem::UNI40_abr_params_ix:
      os << "UNI40_abr_params";
      break;
    case InfoElem::UNI40_abr_setup_ix:
      os << "UNI40_abr_setup";
      break;
    case InfoElem:: UNI40_conn_scope_ix:
      os << "UNI40_conn_scope";
      break;
    case InfoElem::UNI40_xqos_param_ix:
      os << "UNI40_xqos_param";
      break;
    case InfoElem::PNNI_designated_transit_list_ix:
      os << "PNNI_designated_transit_list";
      break;
    case InfoElem::PNNI_crankback_ix:
      os << "PNNI_crankback";
      break;
    default:
      os << "ie_unknown";
      break;
    }
}


enum InfoElem::ieid index_to_ieid(enum InfoElem::ie_ix ix)
{

  switch(ix)
    {
    case InfoElem::ie_cause_ix:
      return InfoElem::ie_cause_id;
      break;
    case InfoElem::UNI40_e2e_transit_delay_ix:
      return InfoElem::UNI40_e2e_transit_delay_id;
      break;
    case InfoElem::ie_call_state_ix:
      return InfoElem::ie_call_state_id;
      break;
    case InfoElem::UNI40_notification_ind_ix:
      return InfoElem::UNI40_notification_ind_id;
      break;
    case InfoElem::ie_end_pt_ref_ix:
      return InfoElem::ie_end_pt_ref_id;
      break;
    case InfoElem::ie_end_pt_state_ix:
      return InfoElem::ie_end_pt_state_id;
      break;
    case InfoElem::ie_aal_param_ix:
      return InfoElem::ie_aal_param_id;
      break;
    case InfoElem::ie_traffic_desc_ix:
      return InfoElem::ie_traffic_desc_id;
      break;
    case InfoElem::UNI40_alt_traff_desc_ix:
      return InfoElem::UNI40_alt_traff_desc_id;
      break;
    case InfoElem::UNI40_min_traff_desc_ix:
      return InfoElem::UNI40_min_traff_desc_id;
      break;
    case InfoElem::ie_conn_identifier_ix:
      return InfoElem::ie_conn_identifier_id;
      break;
    case InfoElem::ie_qos_param_ix:
      return InfoElem::ie_qos_param_id;
      break;
    case InfoElem::ie_bhli_ix:
      return InfoElem::ie_bhli_id;
      break;
    case InfoElem::ie_broadband_bearer_cap_ix:
      return InfoElem::ie_broadband_bearer_cap_id;
      break;
    case InfoElem::ie_blli_ix:
      return InfoElem::ie_blli_id;
      break;
    case InfoElem::ie_broadband_locking_shift_ix:
      return InfoElem::ie_broadband_locking_shift_id;
      break;
    case InfoElem::ie_broadband_nlocking_shift_ix:
      return InfoElem::ie_broadband_nlocking_shift_id;
      break;
    case InfoElem::ie_broadband_send_comp_ind_ix:
      return InfoElem::ie_broadband_send_comp_ind_id;
      break;
    case InfoElem::ie_broadband_repeat_ind_ix:
      return InfoElem::ie_broadband_repeat_ind_id;
      break;
    case InfoElem::ie_calling_party_num_ix:
      return InfoElem::ie_calling_party_num_id;
      break;
    case InfoElem::ie_calling_party_subaddr_ix:
      return InfoElem::ie_calling_party_subaddr_id;
      break;
    case InfoElem::ie_called_party_num_ix:
      return InfoElem::ie_called_party_num_id;
      break;
    case InfoElem::ie_called_party_subaddr_ix:
      return InfoElem::ie_called_party_subaddr_id;
      break;
    case InfoElem::ie_transit_net_sel_ix:
      return InfoElem::ie_transit_net_sel_id;
      break;
    case InfoElem::ie_restart_ind_ix:
      return InfoElem::ie_restart_ind_id;
      break;
    case InfoElem::UNI40_generic_id_ix:
      return InfoElem::UNI40_generic_id_id;
      break;
    case InfoElem::UNI40_leaf_call_id_ix:
      return InfoElem::UNI40_leaf_call_id;
      break;
    case InfoElem::UNI40_leaf_params_ix:
      return InfoElem::UNI40_leaf_params_id;
      break;
    case InfoElem::UNI40_leaf_sequence_num_ix:
      return InfoElem::UNI40_leaf_sequence_num_id;
      break;
    case InfoElem::UNI40_abr_params_ix:
      return InfoElem::UNI40_abr_params_id;
      break;
    case InfoElem::UNI40_abr_setup_ix:
      return InfoElem::UNI40_abr_setup_id;
      break;
    default:
      return InfoElem::ie_unknown_id;
      break;
    }
}

enum InfoElem::ie_ix ieid_to_index(enum InfoElem::ieid id)
{
  switch(id)
    {
    case InfoElem::ie_cause_id:
      return InfoElem::ie_cause_ix;
      break;
    case InfoElem::UNI40_e2e_transit_delay_id:
      return InfoElem::UNI40_e2e_transit_delay_ix;
      break;
    case InfoElem::ie_call_state_id:
      return InfoElem::ie_call_state_ix;
      break;
    case InfoElem::UNI40_notification_ind_id:
      return InfoElem::UNI40_notification_ind_ix;
      break;
    case InfoElem::ie_end_pt_ref_id:
      return InfoElem::ie_end_pt_ref_ix;
      break;
    case InfoElem::ie_end_pt_state_id:
      return InfoElem::ie_end_pt_state_ix;
      break;
    case InfoElem::ie_aal_param_id:
      return InfoElem::ie_aal_param_ix;
      break;
    case InfoElem::ie_traffic_desc_id:
      return InfoElem::ie_traffic_desc_ix;
      break;
    case InfoElem::ie_conn_identifier_id:
      return InfoElem::ie_conn_identifier_ix;
      break;
    case InfoElem::ie_qos_param_id:
      return InfoElem::ie_qos_param_ix;
      break;
    case InfoElem::ie_bhli_id:
      return InfoElem::ie_bhli_ix;
      break;
    case InfoElem::ie_broadband_bearer_cap_id:
      return InfoElem::ie_broadband_bearer_cap_ix;
      break;
    case InfoElem::ie_blli_id:
      return InfoElem::ie_blli_ix;
      break;
    case InfoElem::ie_broadband_locking_shift_id:
      return InfoElem::ie_broadband_locking_shift_ix;
      break;
    case InfoElem::ie_broadband_nlocking_shift_id:
      return InfoElem::ie_broadband_nlocking_shift_ix;
      break;
    case InfoElem::ie_broadband_send_comp_ind_id:
      return InfoElem::ie_broadband_send_comp_ind_ix;
      break;
    case InfoElem::ie_broadband_repeat_ind_id:
      return InfoElem::ie_broadband_repeat_ind_ix;
      break;
    case InfoElem::ie_calling_party_num_id:
      return InfoElem::ie_calling_party_num_ix;
      break;
    case InfoElem::ie_calling_party_subaddr_id:
      return InfoElem::ie_calling_party_subaddr_ix;
      break;
    case InfoElem::ie_called_party_num_id:
      return InfoElem::ie_called_party_num_ix;
      break;
    case InfoElem::ie_called_party_subaddr_id:
      return InfoElem::ie_called_party_subaddr_ix;
      break;
    case InfoElem::ie_transit_net_sel_id:
      return InfoElem::ie_transit_net_sel_ix;
      break;
    case InfoElem::ie_restart_ind_id:
      return InfoElem::ie_restart_ind_ix;
      break;
    case InfoElem::UNI40_generic_id_id:
      return InfoElem::UNI40_generic_id_ix;
      break;
    case InfoElem::UNI40_leaf_call_id:
      return InfoElem::UNI40_leaf_call_id_ix;
      break;
    case InfoElem::UNI40_leaf_params_id:
      return InfoElem::UNI40_leaf_params_ix;
      break;
    case InfoElem::UNI40_leaf_sequence_num_id:
      return InfoElem::UNI40_leaf_sequence_num_ix;
      break;
    case InfoElem::UNI40_abr_params_id:
      return InfoElem::UNI40_abr_params_ix;
      break;
    case InfoElem::UNI40_abr_setup_id:
      return InfoElem::UNI40_abr_setup_ix;
      break;
    default:
      return InfoElem::ie_unknown_ix;
      break;
    }
}

void InfoElem::encode_ie_header(u_char * buffer, InfoElem::ieid id,int len, 
				InfoElem::coding_std coding)
{
  buffer[0] = (u_char) id;
  buffer[1] = (coding | 0x80);
  buffer[2] = (u_char) ((len >> 8) & 0xff);
  buffer[3] = (u_char) (0xff & len);
}

// Appends another IE (of same type) to list not to exceed
//  max_repeats
// NOTE: now returns a ptr to the head of the list
InfoElem * InfoElem::Append(InfoElem* him, char tag)
{
  InfoElem * tmp = this;
  
  if (tag) {
    _first_of_many = 1;
    _num_in_list++;

    // Check to see if we are exceeding max_repeats
    if (_num_in_list > max_repeats()) {
      _num_in_list--;
      tmp = _next;
      tmp->_last = _last;

      // free removed IE
      // WARNING:  Do _NOT_ try to refer to 'this' if Append does not
      //   return a ptr to 'this'!
      delete this;
    }
  }

  // If next is null, we can assume last is this
  if (tmp->_next == 0L) {
    tmp->_next = him;
    tmp->_last = him;    
  } else {
    // next is used, so last should point to the last
    tmp->_last->Append(him, 0);
    tmp->_last = him;
  }
  MakeStale();
  return(tmp);
}


InfoElem * InfoElem::get_next_ie(void)
{
  return (_next);
}

int InfoElem::IsInList(InfoElem *t)
{
  InfoElem *ptr = this;

  while (ptr) {
    if (ptr == t)
      return 1;
    else
      ptr = ptr->_next;
  }
  return 0;
}


char InfoElem::NumInList(void) const
{
  return (_num_in_list);
}
    
u_char InfoElem::IsFirstOfMany(void) const
{
  return (_first_of_many);
  //return (_next != 0L);
}

u_char InfoElem::IsVariableLen(void) const
{
  return ((u_char)0);
}


char InfoElem::PrintGeneral(ostream & os)
{
  os << "! ";

  switch(_id) {
    case ie_cause_id:
      os << "ie_cause: ";
      break;
    case UNI40_e2e_transit_delay_id:
      os << "UNI40_e2e_transit_delay: ";
      break;
    case ie_call_state_id:
      os << "ie_call_state: ";
      break;
    case UNI40_notification_ind_id:
      os << "UNI40_notification_ind_id: ";
      break;
    case ie_end_pt_ref_id:
      os << "ie_end_pt_ref: ";
      break;
    case ie_end_pt_state_id:
      os << "ie_end_pt_state: ";
      break;
    case ie_aal_param_id:
      os << "ie_aal_param: ";
      break;
    case ie_traffic_desc_id:
      os << "ie_traffic_desc: ";
      break;
    case UNI40_alt_traff_desc_id:
      os << "UNI40_alt_traffic_desc: ";
      break;
    case UNI40_min_traff_desc_id:
      os << "UNI40_min_traffic_desc: ";
      break;
    case ie_conn_identifier_id:
      os << "ie_conn_identifier: ";
      break;
    case ie_qos_param_id:
      os << "ie_qos_param: ";
      break;
    case ie_bhli_id:
      os << "ie_bhli: ";
      break;
    case ie_broadband_bearer_cap_id:
      os << "ie_bbc: ";
      break;
    case ie_blli_id:
      os << "ie_blli: ";
      break;
    case ie_broadband_locking_shift_id:
      os << "ie_broadband_locking_shift: ";
      break;
    case ie_broadband_nlocking_shift_id:
      os << "ie_broadband_nlocking_shift: ";
      break;
    case ie_broadband_send_comp_ind_id:
      os << "ie_broadband_send_comp_ind: ";
      break;
    case ie_broadband_repeat_ind_id:
      os << "ie_broadband_repeat_ind: ";
      break;
    case ie_calling_party_num_id:
      os << "ie_calling_party_num: ";
      break;
    case ie_calling_party_subaddr_id:
      os << "ie_calling_party_subaddr: ";
      break;
    case ie_called_party_num_id:
      os << "ie_called_party_num: ";
      break;
    case ie_called_party_subaddr_id:
      os << "ie_called_party_subaddr: ";
      break;
    case ie_transit_net_sel_id:
      os << "ie_transit_net_sel: ";
      break;
    case ie_restart_ind_id:
      os << "ie_restart_ind: ";
      break;
    case UNI40_generic_id_id:
      os << "ie_generic_id: ";
      break;
    case UNI40_leaf_call_id:
      os << "UNI40_leaf_call_id: ";
      break;
    case UNI40_leaf_params_id:
      os << "UNI40_leaf_params: ";
      break;
    case UNI40_leaf_sequence_num_id:
      os << "UNI40_leaf_sequence_num: ";
      break;
    case UNI40_abr_params_id:
      os << "UNI40_abr_params: ";
      break;
    case UNI40_abr_setup_id:
      os << "UNI40_abr_setup: ";
      break;
    case UNI40_conn_scope_id:
      os << "UNI40_conn_scope: ";
      break;
    case UNI40_xqos_param_id:
      os << "UNI40_xqos_param: ";
      break;
    case PNNI_calling_party_soft_pvpc_id:
      os << "PNNI_calling_party_soft_pvpc: ";
      break;
    case PNNI_called_party_soft_pvpc_id:
      os << "PNNI_called_party_soft_pvpc: ";
      break;
    case PNNI_designated_transit_list_id:
      os << "PNNI_designated_transit_list: ";
      break;
    case PNNI_crankback_id:
      os << "PNNI_crankback: ";
      break;
    default:
      os << "ie_unknown: ";
      break;
  }
  return 0;
}


char InfoElem::PrintSpecific(ostream& os)
{
  return 0;
}

ostream& operator << (ostream& os, InfoElem& x) 
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return(os);
}

// Not finished quite yet
InfoElem* Read_IE(istream& is){

  const int TEMPSTR=255;
  char tmp[TEMPSTR];

  int i=0;
  char ch;

  while((is.get(ch))&&(ch=='!')) i++;

  while((is.get(ch))&&(ch!='!')) {
    tmp[i]=ch;
    i++;
  }
  tmp[i]=0;

  while((is.get(ch))&&(ch=='!')) i++;

  InfoElem* ans;

  if (strcmp("ie_cause: ",tmp)==0)                         ans=0L;
  else if (strcmp("UNI40_e2e_transit_delay: ",tmp)==0)     ans=0L;
  else if (strcmp("ie_call_state: ",tmp)==0)               ans=0L;
  else if (strcmp("UNI40_notification_ind_id: ",tmp)==0)   ans=0L;
  else if (strcmp("ie_end_pt_ref: ",tmp)==0)               ans=0L;
  else if (strcmp("ie_end_pt_state: ",tmp)==0)             ans=0L;
  else if (strcmp("ie_aal_param: ",tmp)==0)                ans=0L;
  else if (strcmp("ie_traffic_desc: ",tmp)==0)             ans=0L;
  else if (strcmp("ie_conn_identifier: ",tmp)==0)          ans=0L;
  else if (strcmp("ie_qos_param: ",tmp)==0)                ans=0L;
  else if (strcmp("ie_bhli: ",tmp)==0)                     ans=0L;
  else if (strcmp("ie_bbc: ",tmp)==0)                      ans=0L;
  else if (strcmp("ie_blli: ",tmp)==0)                     ans=0L;
  else if (strcmp("ie_broadband_locking_shift: ",tmp)==0)  ans=0L;
  else if (strcmp("ie_broadband_nlocking_shift: ",tmp)==0) ans=0L;
  else if (strcmp("ie_broadband_send_comp_ind: ",tmp)==0)  ans=0L;
  else if (strcmp("ie_broadband_repeat_ind: ",tmp)==0)     ans=0L;
  else if (strcmp("ie_calling_party_num: ",tmp)==0)        ans=0L;
  else if (strcmp("ie_calling_party_subaddr: ",tmp)==0)    ans=0L;
  else if (strcmp("ie_called_party_num: ",tmp)==0)         ans=0L;
  else if (strcmp("ie_called_party_subaddr: ",tmp)==0)     ans=0L;
  else if (strcmp("ie_transit_net_sel: ",tmp)==0)          ans=0L;
  else if (strcmp("ie_restart_ind: ",tmp)==0)              ans=0L;
  else if (!strcmp("ie_generic_id: ",tmp))                 ans=0L;
  else if (!strcmp("UNI40_leaf_call_id: ",tmp))               ans=0L;
  else if (!strcmp("UNI40_leaf_params: ",tmp))                ans=0L;
  else if (!strcmp("UNI40_leaf_sequence_num: ",tmp))          ans=0L;
  else if (!strcmp("UNI40_abr_params: ",tmp))                 ans=0L;
  else if (!strcmp("UNI40_abr_setup: ",tmp))                  ans=0L;
   
  if (ans) ans->ReadSpecific(is);
  
  return(ans);
}

char InfoElem::ReadSpecific(istream& is){
  return 0;
}

#endif
