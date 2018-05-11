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
#ifndef _BROADBAND_BEARER_CAP_H_
#define _BROADBAND_BEARER_CAP_H_

#ifndef LINT
static char const _broadband_bearer_cap_h_rcsid_[] =
"$Id: _defunct__broadband_bearer_cap.h,v 1.1 1998/09/28 09:12:00 bilal Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

/*
 * Broadband Bearer Capability: TODO
 * see 5.4.5.7 page 218 in UNI-3.1 book
 * 
 */

class ie_errmsg;
class A_Bbc;
class C_Bbc;
class X_Bbc;
class VP_Bbc;

class ie_broadband_bearer_cap : public InfoElem {
public:

  enum Bbc_class {
    unspecified_or_unknow = 0x0,
    BCOB_A = 0x01,
    BCOB_C = 0x03,
    BCOB_X = 0x10, 
    BCOB_VP = 0x18
    };
  
  enum clipping {
    not_clipped = 0x00,
    clipped     = 0x20 
  };
  
  enum conn_type {
    p2p  = 0x00,
    p2mp = 0x01 
  };

  // Values recog. on TX and RC
  enum trans_caps {
    CBR           = 5,    // Also: 4, 6
    CBR_CLR       = 7,
    RT_VBR        = 9,    // Also: 1
    RT_VBR_CLR    = 19,
    NonRT_VBR     = 10,   // Also: 0, 2, 8
    NonRT_VBR_CLR = 11,
    ABR           = 12
  };

  ie_broadband_bearer_cap(Bbc_class bbc, clipping clip, conn_type ctype);
  ie_broadband_bearer_cap(const ie_broadband_bearer_cap &him);
  virtual ~ie_broadband_bearer_cap();

  virtual InfoElem* copy(void) const = 0;
  ///
  virtual int                 encode(u_char * buffer) = 0;
  ///
  virtual InfoElem::ie_status decode(u_char *, int &) = 0;
  virtual const  ie_broadband_bearer_cap * get_bbc_class(void) const =  0;

  // common to all qos classes
  Bbc_class get_qos_class(void) const;
  clipping  get_clipping(void) const;
  conn_type get_conn_type(void) const;
  void      set_qos_class(Bbc_class qos_class);
  void      set_clipping(clipping clip);
  void      set_conn_type(conn_type ctype);

  char get_ATC(void) const;
  void set_ATC(char atc);
  int Length(void);

protected:

  ie_broadband_bearer_cap(Bbc_class qos_class);
  virtual char PrintSpecific(ostream & os);
  Bbc_class _qos_class;
  clipping  _clip;
  conn_type _conn_type;
  char      _atc;
};


//-----------------------Class A BBC------------------------------------------

class A_Bbc : public ie_broadband_bearer_cap {
  friend ostream & operator << (ostream & os, A_Bbc & x);
public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const A_Bbc* himptr) const;

  A_Bbc(clipping clip, conn_type ctype);
  A_Bbc(void);
  A_Bbc(const A_Bbc &him);

  virtual ~A_Bbc();

  InfoElem* copy(void) const;
  int operator == (const A_Bbc &) const;
  const ie_broadband_bearer_cap * get_bbc_class(void) const;

  int                 encode(u_char * buffer);
  InfoElem::ie_status decode(u_char *, int &);

private:

  virtual char PrintSpecific(ostream & os);
};


//-------------------------------Class C BBC----------------------------------

class C_Bbc : public ie_broadband_bearer_cap {
  friend ostream & operator << (ostream & os, C_Bbc & x);
public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const C_Bbc* himptr) const;

  C_Bbc(clipping clip, conn_type ctype);
  C_Bbc(void);
  C_Bbc(const C_Bbc &him);

  virtual ~C_Bbc();

  InfoElem* copy(void) const;
  int operator == (const C_Bbc &) const;
  const ie_broadband_bearer_cap * get_bbc_class(void) const;
  
  int                 encode(u_char * buffer);
  InfoElem::ie_status decode(u_char *, int &);

private:

  virtual char PrintSpecific(ostream & os);
};


//-------------------------------Class X BBC----------------------------------

class X_Bbc : public ie_broadband_bearer_cap {
  friend ostream & operator << (ostream & os, X_Bbc & x);
public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const X_Bbc* himptr) const;

  enum traffic_type {
    no_tp = 0x00,
    CBR   = 0x04,
    VBR   = 0x08 
  };

  enum timing_req {
    no_timing               = 0x00,
    end_to_end_required     = 0x01,
    end_to_end_not_required = 0x02,
    reserved                = 0x03 
  };

  X_Bbc(clipping clip, conn_type ctype, traffic_type tp, timing_req tr);
  X_Bbc(void);
  X_Bbc(const X_Bbc &him);

  virtual ~X_Bbc();
  
  traffic_type get_traffic_type(void);
  timing_req get_timing_req(void);

  int operator == (const X_Bbc &) const;
  InfoElem* copy(void) const;
  const ie_broadband_bearer_cap * get_bbc_class(void) const;

  int                 encode(u_char * buffer);
  InfoElem::ie_status decode(u_char *, int &);

  virtual char PrintSpecific(ostream & os);

private:

  traffic_type _tp;
  timing_req   _tr;
};


//-------------------------------Class VP BBC----------------------------------

class VP_Bbc : public ie_broadband_bearer_cap {
  friend ostream & operator << (ostream & os, VP_Bbc & x);
public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const VP_Bbc* himptr) const;

  enum traffic_type {
    no_tp = 0x00,
    CBR   = 0x04,
    VBR   = 0x08 
  };

  enum timing_req {
    no_timing               = 0x00,
    end_to_end_required     = 0x01,
    end_to_end_not_required = 0x02,
    reserved                = 0x03 
  };

  VP_Bbc(clipping clip, conn_type ctype, traffic_type tp, timing_req tr);
  VP_Bbc(void);
  VP_Bbc(const VP_Bbc &him);

  virtual ~VP_Bbc();
  
  traffic_type get_traffic_type(void);
  timing_req get_timing_req(void);

  int operator == (const VP_Bbc &) const;
  InfoElem* copy(void) const;
  const ie_broadband_bearer_cap * get_bbc_class(void) const;

  int                 encode(u_char * buffer);
  InfoElem::ie_status decode(u_char *, int &);

  virtual char PrintSpecific(ostream & os);

private:

  traffic_type _tp;
  timing_req   _tr;
};

#endif
