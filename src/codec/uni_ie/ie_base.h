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
#ifndef __IE_BASE_H__
#define __IE_BASE_H__

#ifndef LINT
static char const _ie_base_h_rcsid_[] =
"$Id: ie_base.h,v 1.13 1998/12/24 16:09:31 mountcas Exp $";
#endif

#ifndef UNI40
#define UNI40
#endif

#include <iostream.h>
#include <codec/uni_ie/cache.h>

typedef u_char  U8BitValue;
typedef char    S8BitValue;
typedef u_short U16BitValue;
typedef short   S16BitValue;
typedef int     S32bitValue;
typedef unsigned int U32BitValue;

class E164_addr;
class NSAP_addr;
class NSAP_DCC_ICD_addr;
class NSAP_E164_addr;
class iso_bhli;
class user_bhli;
class vendor_bhli;
class ie_blli;
class ie_bbc; 
// class A_Bbc;
// class C_Bbc;
// class X_Bbc;
class aal5;
class ie_broadband_repeat_ind;
class ie_broadband_send_comp_ind;
class ie_call_state;
class ie_called_party_num;
class ie_called_party_subaddr;
class ie_calling_party_num;
class ie_calling_party_subaddr;
class ie_cause;
class ie_conn_id;
class ie_end_pt_ref;
class ie_end_pt_state;
class ie_qos_param;
class ie_restart_ind;
class ie_traffic_desc;
class ie_transit_net_sel;
class UNI40_notification_indicator;
class UNI40_alt_traffic_desc;
class UNI40_min_traffic_desc;
class UNI40_lij_call_id;
class UNI40_lij_params;
class UNI40_leaf_sequence_num;
class UNI40_abr_params;
class UNI40_abr_setup;
class UNI40_generic_id;
class UNI40_e2e_transit_delay;
class PNNI_called_party_soft_pvpc;
class PNNI_crankback;
class PNNI_designated_transit_list;
class PNNI_calling_party_soft_pvpc;

// MEGA-BRUTAL-DEBUG
// #define new (!(cout << "Line " << dec << __LINE__ << ". File: " << __FILE__ << endl)) ? 0L : new

const int MAX_DIAGNOSTIC_LEN = 32;
const int ie_header_len  = 4;
#ifdef UNI40
const int num_ie         = 37;
#else
const int num_ie         = 21;
#endif
const int E164_MAX_SIZE  = 15;

class ie_errmsg;
class body_parser;

class InfoElem : public Cache {
  friend class body_parser;

public:
  enum SignallingVersion {
    uni30 = 0x30,
    uni31 = 0x31,
    uni40 = 0x40
  };

  ///
  enum ieid {
    ie_cause_id                     = 0x08,
    UNI40_e2e_transit_delay_id      = 0x0A, // UNI 40
    ie_call_state_id                = 0x14,
    UNI40_notification_ind_id       = 0x27, // UNI 40
    ie_end_pt_ref_id                = 0x54,
    ie_end_pt_state_id              = 0x55,
    ie_aal_param_id                 = 0x58,
    ie_traffic_desc_id              = 0x59,
    ie_conn_identifier_id           = 0x5a,
    ie_qos_param_id                 = 0x5c,
    ie_bhli_id                      = 0x5d,
    ie_broadband_bearer_cap_id      = 0x5e,
    ie_blli_id                      = 0x5f,
    ie_broadband_locking_shift_id   = 0x60,
    ie_broadband_nlocking_shift_id  = 0x61,
    ie_broadband_send_comp_ind_id   = 0x62,
    ie_broadband_repeat_ind_id      = 0x63,
    ie_calling_party_num_id         = 0x6c,
    ie_calling_party_subaddr_id     = 0x6d,
    ie_called_party_num_id          = 0x70,
    ie_called_party_subaddr_id      = 0x71,
    ie_transit_net_sel_id           = 0x78,
    ie_restart_ind_id               = 0x79,
    UNI40_generic_id_id             = 0x7F, // UNI 40
    UNI40_min_traff_desc_id         = 0x80, // UNI 40
    UNI40_alt_traff_desc_id         = 0x81, // UNI 40
    UNI40_abr_setup_id              = 0x84, // UNI 40
    PNNI_called_party_soft_pvpc_id  = 0xE0, // UNI 40
    PNNI_crankback_id               = 0xE1, // UNI 40
    PNNI_designated_transit_list_id = 0xE2, // UNI 40
    PNNI_calling_party_soft_pvpc_id = 0xE3, // UNI 40
    UNI40_abr_params_id             = 0xE4, // UNI 40
    UNI40_leaf_call_id              = 0xE8, // UNI 40
    UNI40_leaf_params_id            = 0xE9, // UNI 40
    UNI40_leaf_sequence_num_id      = 0xEA, // UNI 40
    UNI40_conn_scope_id             = 0xEB, // UNI 40
    UNI40_xqos_param_id             = 0xEC, // UNI 40
    ie_unknown_id                   = 0xFF
  };

  ///
  enum ie_ix {
    ie_cause_ix                     = 0,
    ie_call_state_ix                = 1,
    ie_end_pt_ref_ix                = 2,
    ie_end_pt_state_ix              = 3,
    ie_aal_param_ix                 = 4,
    ie_traffic_desc_ix              = 5,
    ie_conn_identifier_ix           = 6,
    ie_qos_param_ix                 = 7,
    ie_bhli_ix                      = 8,
    ie_broadband_bearer_cap_ix      = 9,
    ie_blli_ix                      = 10,
    ie_broadband_locking_shift_ix   = 11,
    ie_broadband_nlocking_shift_ix  = 12,
    ie_broadband_send_comp_ind_ix   = 13,
    ie_broadband_repeat_ind_ix      = 14,
    ie_calling_party_num_ix         = 15,
    ie_calling_party_subaddr_ix     = 16,
    ie_called_party_num_ix          = 17,
    ie_called_party_subaddr_ix      = 18,
    ie_transit_net_sel_ix           = 19,
    ie_restart_ind_ix               = 20,
    UNI40_e2e_transit_delay_ix      = 21, // UNI 40
    UNI40_generic_id_ix             = 22, // UNI 40
    UNI40_leaf_call_id_ix           = 23, // UNI 40
    UNI40_leaf_params_ix            = 24, // UNI 40
    UNI40_leaf_sequence_num_ix      = 25, // UNI 40
    PNNI_called_party_soft_pvpc_ix  = 26, // UNI 40
    PNNI_crankback_ix               = 27, // UNI 40
    PNNI_designated_transit_list_ix = 28, // UNI 40
    PNNI_calling_party_soft_pvpc_ix = 29, // UNI 40
    UNI40_abr_setup_ix              = 30, // UNI 40
    UNI40_abr_params_ix             = 31, // UNI 40
    UNI40_conn_scope_ix             = 32, // UNI 40
    UNI40_xqos_param_ix             = 33, // UNI 40
    UNI40_notification_ind_ix       = 34, // UNI 40
    UNI40_alt_traff_desc_ix         = 35, // UNI 40
    UNI40_min_traff_desc_ix         = 36, // UNI 40
    ie_unknown_ix                   = -1
  };


  ///
  enum coding_std {
    itu = 0x00,
    net = 0x60
  };

  /** ie_status indicates the success or failure of the decode() method
   * of an information element.
   */
  enum ie_status {
    /// decode() thinks that it did not fail.
    ok         = 0,

    /// The information element's id is out of range.
    bad_id,

    /// There is nothing in the information element.
    empty,

    /// The information element is underspecified.
    incomplete,

    /// The contents of the information element are in error.
    invalid_contents,

    /// The information element is not supported.
    unsupported
  };

  ///
  enum ie_action_ind {
    clear_call             = 0,
    discard_ie_proc        = 1,
    discard_ie_proc_status = 2,
    reserved_1             = 3,
    reserved_2             = 4,
    discard_msg_ignore     = 5,
    discard_msg_status     = 6,
    reserved_3             = 7
  };

  ///
  InfoElem(ieid id);

  ///
  InfoElem(const InfoElem &him);

  ///
  virtual ~InfoElem();

  ///
  virtual int       encode(u_char *buffer) = 0;
  virtual ie_status decode(u_char *buffer, int & id) = 0;

  void              SetUniVersion(SignallingVersion v);
  SignallingVersion GetUniVersion(void);

  ieid    get_id(void); 

  bool InstanceOf(ieid id);

  virtual u_char IsVariableLen(void) const;

  virtual u_char IsFirstOfMany(void) const;

  virtual char   NumInList(void) const;

  // this is redefined in blli and cause for 3 and 2 respectively
  virtual int max_repeats(void);

  // Returns new head of list
  InfoElem *    Append(InfoElem* him, char tag=1);
  int           IsInList(InfoElem *t);
  InfoElem *    get_next_ie(void);

  friend  char is_ie_valid(ieid x); 

  friend  ieid index_to_ieid(ie_ix ix);

  friend  ie_ix ieid_to_index(ieid id);

  virtual InfoElem* copy(void) const = 0;

  virtual int equals(const InfoElem* x) const = 0;
  virtual int equals(const E164_addr* x) const;
  virtual int equals(const NSAP_addr* x) const;
  virtual int equals(const NSAP_DCC_ICD_addr* x) const;
  virtual int equals(const NSAP_E164_addr* x) const;
  virtual int equals(const iso_bhli* x) const;
  virtual int equals(const user_bhli* x) const;
  virtual int equals(const vendor_bhli* x) const;
  virtual int equals(const ie_blli* x) const;
  virtual int equals(const ie_bbc * x) const;
  //  virtual int equals(const A_Bbc* x) const;
  //  virtual int equals(const C_Bbc* x) const;
  //  virtual int equals(const X_Bbc* x) const;
  virtual int equals(const aal5 * x) const;
  virtual int equals(const ie_broadband_repeat_ind* x) const;
  virtual int equals(const ie_broadband_send_comp_ind* x) const;
  virtual int equals(const ie_call_state* x) const;
  virtual int equals(const ie_called_party_num* x) const;
  virtual int equals(const ie_called_party_subaddr* x) const;
  virtual int equals(const ie_calling_party_num* x) const;
  virtual int equals(const ie_calling_party_subaddr* x) const;
  virtual int equals(const ie_cause* x) const;
  virtual int equals(const ie_conn_id* x) const;
  virtual int equals(const ie_end_pt_ref* x) const;
  virtual int equals(const ie_end_pt_state* x) const;
  virtual int equals(const ie_qos_param* x) const;
  virtual int equals(const ie_restart_ind* x) const;
  virtual int equals(const ie_traffic_desc* x) const;
  virtual int equals(const ie_transit_net_sel* x) const;
  ///
  virtual int equals(const UNI40_lij_call_id* x) const;
  virtual int equals(const UNI40_lij_params * x) const;
  virtual int equals(const UNI40_leaf_sequence_num* x) const;
  virtual int equals(const UNI40_abr_params * x) const;
  virtual int equals(const UNI40_abr_setup * x) const;
  virtual int equals(const UNI40_generic_id * x) const;
  virtual int equals(const UNI40_e2e_transit_delay * x) const;
  virtual int equals(const UNI40_alt_traffic_desc * x) const;
  virtual int equals(const UNI40_min_traffic_desc * x) const;
  virtual int equals(const PNNI_called_party_soft_pvpc * x) const;
  virtual int equals(const PNNI_crankback * x) const;
  virtual int equals(const PNNI_designated_transit_list * x) const;
  virtual int equals(const PNNI_calling_party_soft_pvpc * x) const;

#if 1
#define put_8(buf, len, v)   { *buf++ = v & 0xFF;   len++; }

#define put_16(buf, len, v) { \
     *buf++ = (v>> 8) & 0xFF;   *buf++ = (v & 0xFF);  len += 2;}

#define put_24(buf, len, v) { \
 				 *buf++ = (v >> 16); \
				 *buf++ = (v >> 8) & 0xFF;\
				 *buf++ = v & 0xFF; len += 3;}

#define put_32(buf, len, v) { \
     *buf++ = (v >> 24) & 0xFF;  *buf++ = (v >> 16) & 0xFF; \
     *buf++ = (v >>  8) & 0xFF;  *buf++ = (v & 0xFF); \
     len += 4;}

#define  get_7(buf,v) { v = ((*buf) & 0x7f); buf++; }

#define  get_8(buf,v) { v = *buf++; }

  // get_8 with type cast
#define  get_8tc(buf,v,t) { v = (t)*buf++; }

#define  get_16(buf,v) { v = (buf[0] << 8) | (buf[1]); buf += 2; }

#define  get_24(buf,v) { \
   v = (buf[0] << 16) | (buf[1] << 8) | buf[2]; buf += 3; }

#define  get_32(buf,v) { \
     v = (buf[0] << 24) | (buf[1] << 16) | (buf[2] <<  8) | (buf[3] & 0xFF);\
	  buf += 4; }
  
#else
  // this is wrong and will be removed after testing on little endian machine
#define put_8(buffer, len, v)   { *buffer++ = v & 0xFF;   len++; }

#define put_16(buffer, len, v) { short x = htons(v); \
     *buffer++ = (x>> 8) & 0xFF;   *buffer++ = (x & 0xFF);  len += 2;}

#define put_24(buffer, len, v) { int x = htonl(v & 0xFFFFFF); \
 				     *buffer++ = (x >> 16); \
				     *buffer++ = (x >> 8) & 0xFF;\
				     *buffer++ = x & 0xFF; len += 3;}


#define put_32(buffer, len, v) { int x = htonl(v); \
     *buffer++ = (x >> 24) & 0xFF;  *buffer++ = (x >> 16) & 0xFF; \
     *buffer++ = (x >>  8) & 0xFF;  *buffer++ = (x & 0xFF); \
     len += 4;}
  
#define  get_7(buffer,v) { v = ((*buffer) & 0x7f); buffer++; }

#define  get_8(buffer,v) { v = *buffer++; }

  // get_8 with type cast
#define  get_8tc(buffer,v,t) { v = (t)*buffer++; }

#define  get_16(buffer,v) { v = ntohs(((buffer[0] << 8) | (buffer[1]))); \
                          buffer += 2; }

#define  get_24(buffer,v) { v = ntohl((buffer[0] << 16) | (buffer[1] << 8) | buffer[2]); buffer += 3; }

#define  get_32(buffer,v) { v = ((buffer[0] << 24) & 0xFF000000); \
     v |= ((buffer[1] << 16) & 0x00FF0000); \
     v |= ((buffer[2] <<  8) & 0x0000FF00); \
     v |= (buffer[3] & 0xFF); v = ntohl(v); buffer += 4; }
#endif
  ///
  virtual char PrintSpecific(ostream& os);

protected:

  SignallingVersion   _uni_version;
  
  ieid          _id;
  coding_std    _coding;
  u_char        _coding_mask;
  int           _flag;
  u_char        _flag_mask;
  ie_action_ind _action_ind;
  u_char        _action_ind_mask;

  InfoElem *    _next, *_last;
  char          _first_of_many;
  char          _num_in_list;

  ///
  char PrintGeneral(ostream& os);

  ///
  friend ostream& operator << (ostream& os, InfoElem& x);

  ///  HexDump( );
  void special_print(u_char * buf, int len, ostream & os);

  friend InfoElem* Read_IE(istream& is);

  virtual char ReadSpecific(istream& is);

  void encode_ie_header(u_char * buffer, InfoElem::ieid id,int len, 
			InfoElem::coding_std coding=InfoElem::itu);

  friend void Print_IE_name(ie_ix ix, ostream& os);

  inline void put_id(u_char * buffer, U8BitValue v) { buffer[0] = v & 0xFF;}
  inline void put_coding(u_char * buffer, U8BitValue v) {buffer[1]=(v | 0x80);}

  inline void put_len(u_char * buffer, U16BitValue v) {
    int L = 0;
    u_char * buf_plus_2 = buffer + 2;
    put_16( buf_plus_2, L, v );
  }

  inline short get_len(u_char * buffer)  { return (buffer[2]<<8 | buffer[3]); }
  // inline short get_len(u_char * buffer)  { return ntohs((buffer[2] << 8) | (buffer[3])); }
};

// Compiler needs to define UNI40 to make 4.0 lib.
// For some reason this frags the message lib ...

#endif // __IE_BASE_H__



