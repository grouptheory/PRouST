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
#ifndef LINT
static char const _body_parser_cc_rcsid_[] =
"$Id: body_parser.cc,v 1.16 1998/12/15 15:25:58 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/ie.h>
#include <codec/q93b_msg/body_parser.h>

//---------------------------------------------------------------
char body_parser_errmsg::successful(void) { return(_completed); }

void body_parser_errmsg::Print(ostream& os)
{
  os << "*** Message-Body Errors ***\n";

  int count = 0, i;

  for (i = 0; i < num_ie; i++) {
    if (_ie_mand_missing[i]) {
      os << "IE: "; Print_IE_name((InfoElem::ie_ix)i,os); os << " is *missing* but is Mandatory \n";
      count++;
    }
    if (_ie_mand_corrupted[i]) {
      os << "IE: "; Print_IE_name((InfoElem::ie_ix)i,os); os << " is *corrupted* but is Mandatory \n";
      count++;
    }
  }
  if (count==0) os << "None.\n";
  count = 0;

  os << "*** Message-Body Warnings ***\n";
  for (i = 0; i < num_ie; i++) {
    if (_ie_opt_corrupted[i]) {
      os << "Optional IE: " ; Print_IE_name((InfoElem::ie_ix)i,os); os << " is *corrupted* \n";
      count++;
    }
    if (_ie_opt_missing[i]) {
      os << "Optional IE: " ; Print_IE_name((InfoElem::ie_ix)i,os); os << " is *missing* \n";
      count++;
    }
  }
  if (count==0) os << "None.\n";
}

body_parser_errmsg::body_parser_errmsg(void)
{
  _ie_mand_missing = new int [num_ie];
  _ie_opt_missing = new int [num_ie];
  _ie_mand_corrupted = new int [num_ie];
  _ie_opt_corrupted = new int [num_ie];

  for (int i = 0; i < num_ie; i++) {
    _ie_mand_missing[i]   = 0;
    _ie_opt_missing[i]    = 0;
    _ie_mand_corrupted[i] = 0;
    _ie_opt_corrupted[i]  = 0;
  }

  _completed=0;
}

//---------------------------------------------------------------
body_parser_errmsg::~body_parser_errmsg(void)
{
  delete [] _ie_mand_missing; _ie_mand_missing=0L;
  delete [] _ie_opt_missing; _ie_opt_missing=0L;
  delete [] _ie_mand_corrupted; _ie_mand_corrupted=0L;
  delete [] _ie_opt_corrupted; _ie_opt_corrupted=0L;
}


//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
void body_parser::list_errors(ostream& os) 
{
  if (_errors) _errors->Print(os);
  else os << "Message-Body errors data structure is still non-existent (ok)\n";
}

//---------------------------------------------------------------
void body_parser::zero(void)
{
  _current_ie  = 0;
  _current_len = 0;

  _ie_legal     = new int [num_ie];
  _ie_mandatory = new int [num_ie];
  _ie_hits      = new int [num_ie];

  for (int i = 0; i < num_ie; i++) {
    _ie_hits[i]           = 0;
    _ie_legal[i]          = -1;
    _ie_mandatory[i]      = 0;

    _ie_array[i] = (InfoElem *)0;
    _ie_encoded[i] = (u_char *)0;
  }
  _errors = new body_parser_errmsg();
}


//---------------------------------------------------------------
void body_parser::Print(ostream& os)
{
  os << "*** Body Info: ***" << endl;
  int count=0;
  
  for (int i = 0; i < num_ie; i++) {
    if ((_ie_array[i])&&(_ie_legal[i])) {
      os << "IE (" << dec << i << ") : " ;
      Print_IE_name((InfoElem::ie_ix)i,os); 
      os << endl;
      
      _ie_array[i]->PrintSpecific(os);
      os << dec;
      count++;
    }
  }
  if (count==0) os << "No IEs.\n";
}


//---------------------------------------------------------------
body_parser::body_parser(InfoElem** ie_array):_encode(1),_body_valid(0)
{
  zero(); 

  for (int i = 0; i < num_ie; i++) {
    _ie_array[i] = ie_array[i];
    _ie_len[i] = 0;   // sjm ?? 
  }
  _duplicated_body = 0;

  // The Derived class must make the _body_buffer!
}

//---------------------------------------------------------------
body_parser::body_parser(InfoElem** ie_array, Buffer* buf)
  :_encode(1),_body_valid(0)
{
  zero(); 

  for (int i = 0; i < num_ie; i++) {
    _ie_array[i] = ie_array[i];
    _ie_len[i] = 0;   // sjm ?? 
  }
  _duplicated_body = 0;

  // The Derived class must make the _body_buffer!
}

//---------------------------------------------------------------
body_parser::body_parser(const u_char *buffer, int buflen)
  : _encode(0), _body_valid(0)
{
  zero();

  _body_buffer = (u_char *)buffer;
  _body_buflen = buflen;

  _duplicated_body = 0;
};

//---------------------------------------------------------------
body_parser::~body_parser(void) 
{
  delete [] _ie_legal; _ie_legal=0L;
  delete [] _ie_mandatory; _ie_mandatory=0L;
  delete [] _ie_hits; _ie_hits=0L;
  
  int i;

  if (_ie_array) {
    for (i=0;i<num_ie;i++) if (_ie_array[i]) {
      delete _ie_array[i];
      _ie_array[i]=0L;
    }
  }
  if (_ie_encoded){
    for (i=0;i<num_ie;i++) if (_ie_encoded[i]) {
      delete [] _ie_encoded[i];
      _ie_encoded[i]=0L;
    }
  }
  delete _errors;
  _errors=0L;
}

//---------------------------------------------------------------
int body_parser::parse_ie_header(u_char *buffer, int buflen)
{
  u_char *temp = buffer;
  int coding;
  get_8tc(buffer,_current_id, enum InfoElem::ieid);
  if(!is_ie_valid(_current_id))
    return(_current_id);
  get_8(buffer,coding);
  get_16(buffer,_current_len);
  _current_len += ie_header_len;
  if ((coding & 0x80) != 0x80)
    return(_current_id);
  coding = (temp[1] & 0x60);

  switch (_current_id) {

  case InfoElem::UNI40_e2e_transit_delay_id:
  case InfoElem::UNI40_generic_id_id:
  case InfoElem::UNI40_leaf_call_id:
  case InfoElem::UNI40_leaf_params_id:
  case InfoElem::UNI40_leaf_sequence_num_id:
  case InfoElem::UNI40_abr_setup_id:
  case InfoElem::UNI40_abr_params_id:
  case InfoElem::UNI40_conn_scope_id:
  case InfoElem::UNI40_xqos_param_id:
  case InfoElem::UNI40_notification_ind_id:
  case InfoElem::UNI40_alt_traff_desc_id:
  case InfoElem::UNI40_min_traff_desc_id:
    // if (coding != InfoElem::net)
    //   return(_current_id);
    break;

  case InfoElem::ie_qos_param_id: 
  case InfoElem::ie_cause_id :
  case InfoElem::ie_traffic_desc_id:
    
    if ((coding != InfoElem::itu) && (coding != InfoElem::net))
      return(_current_id);
    break;

  default:
    if (coding != InfoElem::itu)
      return(_current_id);
  }


  if (_current_len > buflen)
    return(_current_id);
  // save the encoded IE information and its length
  long inx = ieid_to_index(_current_id);
  _ie_encoded[inx] = new u_char[_current_len];
  _ie_len[inx] = _current_len;
  bcopy((char *)buffer, (char *)_ie_encoded[inx],_current_len);
  ie_aal::AAL_TYPE which_aal;
  ie_bbc::Bbc_class which_class;
  ie_bhli::bhli_type bhlitype;

  switch (_current_id) {
    // ie_cause_id                     = 0x08,
    case InfoElem::ie_cause_id:
      _ie_array[inx] = new ie_cause;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_e2e_transit_delay_id      = 0x0A,
    case InfoElem::UNI40_e2e_transit_delay_id:
      _ie_array[inx] = new UNI40_e2e_transit_delay;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_call_state_id                = 0x14,
    case InfoElem::ie_call_state_id:
      _ie_array[inx] = new ie_call_state;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_notification_ind_id       = 0x27,
    case InfoElem::UNI40_notification_ind_id:
      _ie_array[inx] = new UNI40_notification_indicator;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_end_pt_ref_id                = 0x54,
    case InfoElem::ie_end_pt_ref_id:
      _ie_array[inx] = new ie_end_pt_ref;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_end_pt_state_id              = 0x55,
    case InfoElem::ie_end_pt_state_id:
      _ie_array[inx] = new ie_end_pt_state;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_aal_param_id                 = 0x58,
    case InfoElem::ie_aal_param_id:
      // we need at least 5 bytes in buffer for the AAL IE
      if (buflen < 5)
	return(_current_id);
      // which aal ?
      which_aal = (ie_aal::AAL_TYPE)(*buffer);
      switch (which_aal) {
	case ie_aal::aal1_type:
	case ie_aal::aal2_type:
	case ie_aal::aal3_4_type:
	  return(_current_id);
	  break;
	case ie_aal::aal5_type:
	  _ie_array[inx] = new aal5;
	  _current_ie = _ie_array[inx]; return(0);
	  break;
	default:
	  return (_current_id);
	  break;
      }
      break;
    // ie_traffic_desc_id              = 0x59,
    case InfoElem::ie_traffic_desc_id:
      _ie_array[inx] = new ie_traffic_desc;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_conn_identifier_id           = 0x5a,
    case InfoElem::ie_conn_identifier_id:
      _ie_array[inx] = new ie_conn_id;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_qos_param_id                 = 0x5c,
    case InfoElem::ie_qos_param_id:
      _ie_array[inx] = new ie_qos_param;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_bhli_id                      = 0x5d,
    case InfoElem::ie_bhli_id:
      bhlitype=(ie_bhli::bhli_type)(*buffer);
      switch (bhlitype) {
        case ie_bhli::iso:
	  _ie_array[inx]= new iso_bhli();
	  break;
        case ie_bhli::user_specific:
	  _ie_array[inx]= new user_bhli();
	  break;
        case ie_bhli::vendor_specific_application_id:
	  _ie_array[inx]= new vendor_bhli();
	  break;
        default:
	  break;
      }
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_broadband_bearer_cap_id      = 0x5e,
    case InfoElem::ie_broadband_bearer_cap_id:
      _ie_array[inx] = new ie_bbc;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_blli_id                      = 0x5f,
    case InfoElem::ie_blli_id:
      _ie_array[inx] = new ie_blli;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_broadband_locking_shift_id   = 0x60,
    case InfoElem::ie_broadband_locking_shift_id:
      return (_current_id);
      break;
    // ie_broadband_nlocking_shift_id  = 0x61,
    case InfoElem::ie_broadband_nlocking_shift_id:
      return(_current_id);
      break;
    // ie_broadband_send_comp_ind_id   = 0x62,
    case InfoElem::ie_broadband_send_comp_ind_id:
      _ie_array[inx] = new ie_broadband_send_comp_ind;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_broadband_repeat_ind_id      = 0x63,
    case InfoElem::ie_broadband_repeat_ind_id:
      _ie_array[inx] = new ie_broadband_repeat_ind;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_calling_party_num_id         = 0x6c,
    case InfoElem::ie_calling_party_num_id:
      _ie_array[inx] = new ie_calling_party_num;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_calling_party_subaddr_id     = 0x6d,
    case InfoElem::ie_calling_party_subaddr_id:
      _ie_array[inx] = new ie_calling_party_subaddr;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_called_party_num_id          = 0x70,
    case InfoElem::ie_called_party_num_id:
      _ie_array[inx] = new ie_called_party_num;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_called_party_subaddr_id      = 0x71,
    case InfoElem::ie_called_party_subaddr_id:
      _ie_array[inx] = new ie_called_party_subaddr;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_transit_net_sel_id           = 0x78,
    case InfoElem::ie_transit_net_sel_id:
      _ie_array[inx] = new ie_transit_net_sel;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_restart_ind_id               = 0x79,
    case InfoElem::ie_restart_ind_id:
      _ie_array[inx] = new ie_restart_ind;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_generic_id_id             = 0x7F,
    case InfoElem::UNI40_generic_id_id:
      _ie_array[inx] = new UNI40_generic_id;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_min_traff_desc_id         = 0x80,
    case InfoElem::UNI40_min_traff_desc_id:
      _ie_array[inx] = new UNI40_min_traffic_desc;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_alt_traff_desc_id         = 0x81,
    case InfoElem::UNI40_alt_traff_desc_id:
      _ie_array[inx] = new UNI40_alt_traffic_desc;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_abr_setup_id              = 0x84,
    case InfoElem::UNI40_abr_setup_id:
      _ie_array[inx] = new UNI40_abr_setup;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // PNNI_called_party_soft_pvpc_id  = 0xE0,
    case InfoElem::PNNI_called_party_soft_pvpc_id:
      _ie_array[inx] = new PNNI_called_party_soft_pvpc;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // PNNI_crankback_id               = 0xE1,
    case InfoElem::PNNI_crankback_id:
      _ie_array[inx] = new PNNI_crankback;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // PNNI_designated_transit_list_id = 0xE2,
    case InfoElem::PNNI_designated_transit_list_id:
      _ie_array[inx] = new PNNI_designated_transit_list;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // PNNI_calling_party_soft_pvpc_id = 0xE3,
    case InfoElem::PNNI_calling_party_soft_pvpc_id:
      _ie_array[inx] = new PNNI_calling_party_soft_pvpc;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_abr_params_id             = 0xE4,
    case InfoElem::UNI40_abr_params_id:
      _ie_array[inx] = new UNI40_abr_param;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_leaf_call_id              = 0xE8,
    case InfoElem::UNI40_leaf_call_id:
      _ie_array[inx] = new UNI40_lij_call_id;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_leaf_params_id            = 0xE9,
    case InfoElem::UNI40_leaf_params_id:
      _ie_array[inx] = new UNI40_lij_params;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_leaf_sequence_num_id      = 0xEA,
    case InfoElem::UNI40_leaf_sequence_num_id:
      _ie_array[inx] = new UNI40_leaf_sequence_num;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_conn_scope_id             = 0xEB,
    case InfoElem::UNI40_conn_scope_id:
      _ie_array[inx] = new UNI40_conn_scope_sel;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // UNI40_xqos_param_id             = 0xEC,
    case InfoElem::UNI40_xqos_param_id:
      _ie_array[inx] = new UNI40_xqos_param;
      _current_ie = _ie_array[inx]; return(0);
      break;
    // ie_unknown_id                   = 0xFF
    default:
      return (_current_id);
      break;
  }
  _current_ie = _ie_array[inx]; 
  return(0);
}


// parses buffer for mandatory and optional  IE's
body_parser_errmsg * body_parser::process_decode(void)
{
  int ret;
  int ix;
  int num_errors;

  u_char *buf = _body_buffer;
  int buflen  = _body_buflen;

  num_errors=0;

  while (buflen > 0) {
    if (!(ret = parse_ie_header(buf, buflen))) {
      ix = ieid_to_index(_current_id);
      if (_ie_hits[ix] == _current_ie->max_repeats()) {
	delete _current_ie;
	_current_ie=0L;
	buflen -= _current_len;
	buf += _current_len;
	num_errors++;
	continue;
      }
      // IE unexpected see 5.5.6.8.1 and  5.5.6.8.3
      if (_ie_legal[ix] == -1) { 
	delete _current_ie;
	_current_ie=0L;
	buflen -= _current_len;
	buf += _current_len;
	num_errors++;
	continue;
      }
      InfoElem::ie_status err; int sid;
      if (err = _current_ie->decode(buf, sid)) { // error in IE
	if (err == InfoElem::empty) {
	  if (_ie_mandatory[ix]) {
	    // add this id to missing mandatory IE's
	    _errors->_ie_mand_missing[ix] = _current_id;	      
	    num_errors++;
	  } else {
	    // add this id to the missing optional IE's
	    _errors->_ie_opt_missing[ix] = _current_id;	      
	    num_errors++;
	  }
	} else {
	  // increment total error count
	  switch (err) {
	  case InfoElem::incomplete:
	  case InfoElem::invalid_contents:
	    if (_ie_mandatory[ix]) {
	      // add this id to invalid_contents mandatory IE's
	      _errors->_ie_mand_corrupted[ix] = _current_id;	      
	      num_errors++;
	    } else {
	      // add this id to the invalid_contents optional IE's
	      _errors->_ie_opt_corrupted[ix] = _current_id;	      
	      num_errors++;
	    }
	    break;
	  case InfoElem::bad_id:
	  case InfoElem::unsupported:
	    break;
	  default:
	    break;
	  }
	}
	delete _current_ie;
	_current_ie=0L;
	_ie_array[ix]=0L;
	buflen -= _current_len;
	buf += _current_len;
	continue;
      }
      // so far so good, no errors in IE, additional check for unsupported IE
      _ie_hits[ix]++;
      _ie_array[ix] = _current_ie;
      buflen -= _current_len;
      buf += _current_len;
    } else {
      // IE header is bad, set the _completed flag to 0
      // For now, this is too restrictive 
      _errors->_completed=0;
      return(_errors);	
    }
  } // eow
  _errors->_completed = 1;
  int dbg = 0; 
  // done with whole msg do we have all required IE's
  for (int i = 0; i < num_ie; i++) {
    InfoElem::ieid id = index_to_ieid((InfoElem::ie_ix)i);
    if (_ie_mandatory[i]) dbg++;
  }
  _errors->_completed=1;
  if (num_errors == 0) {
    _body_valid = 1;
  }
  return (_errors);
}


//---------------------------------------------------------------
body_parser_errmsg * body_parser::process_encode(void)
{
  char good = 1;

  _body_buflen = 0;
  u_char * buffer = _body_buffer;

  for (int i = 0; i < num_ie; i++) {
    if (_ie_mandatory[i]) {
      if (_ie_array[i]) {
	buffer += (_ie_len[i] = _ie_array[i]->encode(buffer));
	_body_buflen+= _ie_len[i];
      } else {
	_errors->_ie_mand_missing[i]=1;
	good=0;
      }
    } else if (_ie_legal[i] == 1) {
      if (_ie_array[i]) {
	buffer += (_ie_len[i] = _ie_array[i]->encode(buffer));
	_body_buflen+= _ie_len[i];
      } else {
	_errors->_ie_opt_missing[i]=1;
      }
    }
  }

  if (good) {
    _errors->_completed = 1;
    _body_valid = 1;
  } else {
    _errors->_completed = 0;
    _body_valid = 0;
  }

  return (_errors);
}


//---------------------------------------------------------------
body_parser_errmsg * body_parser::process(void)
{
  if (_encode) return (process_encode());
  else return (process_decode());
}

//---------------------------------------------------------------
body_parser::body_parser(body_parser& him, u_char * buf, int len)
{
  _current_ie  = 0;
  _current_len = 0;

  _ie_legal     = new int [num_ie];
  _ie_mandatory = new int [num_ie];
  _ie_hits      = new int [num_ie];

  for (int i = 0; i < num_ie; i++) {
    _ie_hits[i]           = 0;
    _ie_legal[i]          = -1;
    _ie_mandatory[i]      = 0;

    if (him._ie_array[i]) {
      _ie_array[i] = him._ie_array[i]->copy();
      _ie_len[i] = him._ie_len[i];       
      _ie_encoded[i] = (u_char *)0;
    } else {
      _ie_array[i] = (InfoElem *)0;
      _ie_encoded[i] = (u_char *)0;   
      _ie_len[i] = 0;       
    }
  }

  _errors = new body_parser_errmsg();

  _duplicated_body = 1;
  _body_valid = 0;
  _encode = 1;

  _body_buffer = buf;
  _body_buflen = len;
}
