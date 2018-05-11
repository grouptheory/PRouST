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
#ifndef __XQOS_PARAM_H__
#define __XQOS_PARAM_H__
#if !defined(LINT)
static char const _UNI40_xqos_h_rcsid_[] =
"$Id: UNI40_xqos.h,v 1.6 1998/09/01 20:45:26 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/errmsg.h>
#include <codec/uni_ie/cause.h>

/**
 * see 5.4.5.18 page 233 UNI-3.1 book
 * The purpose of this information element is to request and
 * indicate the QOS of a connection.
 * Its length is 2 byte.
 */

class UNI40_xqos_param : public InfoElem {
 public:

  ///
  virtual int equals(const InfoElem* him) const;
  ///
  virtual int equals(const UNI40_xqos_param* himptr) const;

  /// 
  enum origin {
    user = 0x00,
    inet = 0x01
  };

  enum fb_ind {
    none = 0x00,
    forw = 0x01,
    back = 0x02,
    both = 0x03
  };
  ///
  enum xqos_indicator {
    AFP2PCDV = 0x94,
    ABP2PCDV = 0x95,
    CFP2PCDV = 0x96,
    CBP2PCDV = 0x97,
    AFCLR    = 0xA2,
    ABCLR    = 0xA3
  };
  /// 
  UNI40_xqos_param(origin o, fb_ind fb, int fv, int bv, int fcv, int bcv, int fclr, int bclr); // both
  UNI40_xqos_param(const UNI40_xqos_param & him);
  // either forw or backw   
  UNI40_xqos_param(origin o = user, fb_ind fb = none, int v = 0, int cv = 0, int clr = 0);
  ///
  virtual ~UNI40_xqos_param( );

  ///
  UNI40_xqos_param * copy(void);
  ///
  InfoElem *      copy(void) const;

  ///
  int operator == (const UNI40_xqos_param & rs) const;
  
  ///
  virtual int                 encode(u_char * buffer);
  ///
  virtual InfoElem::ie_status decode(u_char * buffer, int & id);

  ///
  int get_forw_cdv(void);
  int get_back_cdv(void);
  int get_forw_cumul_cdv(void);
  int get_back_cumul_cdv(void);
  int get_forw_clr(void);
  int get_back_clr(void);
  origin get_origin(void);


  void set_forw_cdv(int v);
  void set_back_cdv(int v);
  void set_forw_cumul_cdv(int v);
  void set_back_cumul_cdv(int v);
  void set_forw_clr(int v);
  void set_back_clr(int v);
  void set_origin(origin o);

  int Length();

  ///
  // ie_qos_param(void);

  ///
  virtual char PrintSpecific(ostream & os);
  ///
  friend ostream & operator << (ostream & os, UNI40_xqos_param & x);

 private:
  origin _o;
  fb_ind _fb;
  int _fv; // acceptable forward cdv
  int _bv; // acceptable backward cdv
  int _fcv; // acceptable cumulative forward cdv
  int _bcv; // acceptable cumulative backward cdv
  int _fclr; // acceptable forward CLR
  int _bclr; // acceptable backward CLR
};

#endif
