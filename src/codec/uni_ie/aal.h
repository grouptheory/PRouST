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
#ifndef _AAL_H_
#define _AAL_H_

#if !defined(LINT)
static char const _aal_h_rcsid_[] =
"$Id: aal.h,v 1.9 1998/07/07 19:06:10 marsh Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class ie_errmsg;


/*
 * see 5.4.5.5 page 196 UNI-3.1 book
 * ATM Adaptation layer parameters
 * this is required for IP over ATM see RFC1755 section 5 page 71
 * so info for IP over ATM:
 *   aal type: aal5_type
 *
 */

class aal5;

class ie_aal : public InfoElem {
 public:
  enum AAL_TYPE {
    unspecified_or_unknow = 0x0,
    aal1_type             = 0x01,
    aal2_type             = 0x02,
    aal3_4_type           = 0x03,
    aal5_type             = 0x05,
    user_defined_aal_type = 0x10
  };
  ///
  ie_aal(AAL_TYPE aal_type);
  ///
  ie_aal(void);
  ///
  virtual ~ie_aal();
  ///
  virtual int operator == (const ie_aal & aal) const;
  ///
  AAL_TYPE             get_aal_type(void) const;
  ///
  virtual u_char       IsVariableLen(void);
  ///
  virtual int encode(u_char *buffer) = 0;
  virtual int Length() = 0;
  ///
  virtual InfoElem::ie_status decode(u_char *, int & ) = 0;

protected:

  const AAL_TYPE _aal_type;
};


/*
 * for IP over ATM:
 *  _sscs = sscs_null
 *  _fm_cpcs_sdu_size = 65535
 *  _bm_cpcs_sdu_size = 65535
 *
 */

class aal5 : public ie_aal {
 public:
  enum aal5_sscs {
    sscs_null        = 0x00,
    sscs_assured     = 0x01,
    sscs_non_assured = 0x02,
    sscs_frame_relay = 0x04
  };
  ///
  aal5(void);
  ///
  aal5(aal5_sscs sscs, int fm_sdu_size = 0, int bm_sdu_size = 0);
  /// Copy constructor
  aal5(const aal5 & old);
  ///
  virtual ~aal5();

  ///
  virtual int operator == (const ie_aal & aal) const;
  ///
  int equals(const InfoElem* him) const;
  ///
  int equals(const aal5 *himptr) const;
  ///
  virtual int encode(u_char *buffer);
  ///
  virtual InfoElem::ie_status decode(u_char *, int &);
  ///
  InfoElem* copy(void) const;
  ///
  aal5_sscs  get_sscs(void);
  ///
  void       set_sscs(aal5_sscs sscs);
  ///
  int  get_fm_cpcs_sdu_size(void);
  ///
  void set_fm_cpcs_sdu_size(int size);
  ///
  int  get_bm_cpcs_sdu_size(void);
  ///
  void set_bm_cpcs_sdu_size(int size);
  ///
  const aal5 * get_aal5_type(void) const;
  int Length();
protected:

  friend  ostream& operator << (ostream& os, aal5& x);
  virtual char PrintSpecific(ostream& os);

private:
  enum aal5_octets_id {
    fm_cpcs_sdu_size_id = 0x8c,
    bm_cpcs_sdu_size_id = 0x81,
    atm_mode_id         = 0x83, // UNI 3.0 compatiblity
    sscs_id             = 0x84
  };
  int _sscs_set;
  int _fm_cpcs_sdu_size_set;
  int _bm_cpcs_sdu_size_set;
  aal5_sscs _sscs;
  int       _fm_cpcs_sdu_size;
  int       _bm_cpcs_sdu_size;
};

#endif
