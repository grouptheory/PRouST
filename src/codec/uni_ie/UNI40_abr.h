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
/// Available Bit Rate Capability
#ifndef __ABR_H__
#define __ABR_H__

#ifndef LINT
static char const _UNI40_abr_h_rcsid_[] =
"$Id: UNI40_abr.h,v 1.8 1998/07/31 08:34:53 bilal Exp $";
#endif

#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>

/** ABR Additional Paramters
    specifies the set of additional ABR paramters during the call/
    connection establishment.  UNI 4.0  sec. 10.1.2.1
 */
class UNI40_abr_param : public InfoElem {
  friend ostream & operator << (ostream & os, UNI40_abr_param & x);

public:
  ///
  UNI40_abr_param(int fwd_param = 0, int bwd_param = 0) : 
               InfoElem(UNI40_abr_params_id), _fwd_param(fwd_param), 
               _bwd_param(bwd_param) { }

  ///
  UNI40_abr_param(const UNI40_abr_param & abr) : InfoElem(UNI40_abr_params_id), 
               _fwd_param(abr._fwd_param), _bwd_param(abr._bwd_param) { }

  ///
  virtual ~UNI40_abr_param( ) { }

  ///
  enum ident {
    fwd_param_rec_id  = 0xC2,
    bwd_param_rec_id  = 0xC3
  };

  ///
  UNI40_abr_param *copy(void) { return (new UNI40_abr_param(*this)); }
  ///
  InfoElem *copy(void) const { return (InfoElem *)new UNI40_abr_param(this->_fwd_param, 
                                       this->_bwd_param); }

  ///
  int                 encode(u_char *buffer);
  ///
  InfoElem::ie_status decode(u_char *buffer, int & id);

  ///
  int operator == (const UNI40_abr_param & right) const;
  ///
  int equals(const InfoElem * him) const;
  ///
  int equals(const UNI40_abr_param * himptr) const;
  int Length();
 protected:
  ///
  virtual char PrintSpecific(ostream & os);

 private:
  ///
  int  _fwd_param;
  ///
  int  _bwd_param;
};


// ABR Setup Paramters
//   specifies the set of ABR parameters during the call/connection
///   establishment.  UNI 4.0  sec. 10.1.2.2
class UNI40_abr_setup : public InfoElem {
  ///
  friend ostream & operator << (ostream & os, UNI40_abr_setup & x);
 public:
  ///
  enum ident {
    forward_init_cell_rate_id   = 0xC2,
    backward_init_cell_rate_id  = 0xC3,
    forward_trans_buf_exp_id    = 0xC4,
    backward_trans_buf_exp_id   = 0xC5,
    cumulative_fixed_rtt_id     = 0xC6,
    forward_rate_incr_id        = 0xC8,
    backward_rate_incr_id       = 0xC9,
    forward_rate_decr_id        = 0xCA,
    backward_rate_decr_id       = 0xCB
  };

  ///
  enum flags {
    fwd_icr       = 0x001,
    bwd_icr       = 0x002,
    fwd_tbe       = 0x004,
    bwd_tbe       = 0x008,
    cum_rtt       = 0x010,
    fwd_rif       = 0x020,
    bwd_rif       = 0x040,
    fwd_rdf       = 0x080,
    bwd_rdf       = 0x100
  };

  ///
  UNI40_abr_setup(int ficr = 0, int bicr = 0, int ftbe = 0, int btbe = 0, int crtt = 0, 
               int frif = 0, int brif = 0, int frdf = 0, int brdf = 0, int flags = 0) : 
               InfoElem(UNI40_abr_setup_id),
               _fwd_icr(ficr), _bwd_icr(bicr), _fwd_tbe(ftbe), _bwd_tbe(btbe), 
               _cum_rtt(crtt), _fwd_rif(frif), _bwd_rif(brif), 
               _fwd_rdf(frdf), _bwd_rdf(brdf), _flags(flags) { }

  ///
  UNI40_abr_setup(UNI40_abr_setup & a) : 
               InfoElem(UNI40_abr_setup_id), _fwd_icr(a._fwd_icr), _bwd_icr(a._bwd_icr), 
               _fwd_tbe(a._fwd_tbe), _bwd_tbe(a._bwd_tbe), _cum_rtt(a._cum_rtt),
               _fwd_rif(a._fwd_rif), _bwd_rif(a._bwd_rif), _fwd_rdf(a._fwd_rdf), 
               _bwd_rdf(a._bwd_rdf), _flags(a._flags) { }

  ///
  virtual ~UNI40_abr_setup( ) { }

  ///
  UNI40_abr_setup * copy(void) { return (new UNI40_abr_setup(*this)); }
  ///
  InfoElem * copy(void) const { return (InfoElem *)(new UNI40_abr_setup(_fwd_icr, _bwd_icr,
                                _fwd_tbe, _bwd_tbe, _cum_rtt, _fwd_rif, _bwd_rif,
                                _fwd_rdf, _bwd_rdf, _flags)); }

  ///
  int        encode(u_char *buffer);
  ///
  InfoElem::ie_status decode(u_char *buffer, int & id);

  ///
  int operator == (const UNI40_abr_setup & right) const;
  ///
  int equals(const InfoElem * him) const;
  ///
  int equals(const UNI40_abr_setup * himptr) const;

  ///
  void setForwardICR(int icr)    { _fwd_icr = icr; _flags |= fwd_icr; }
  ///
  void setBackwardICR(int icr)   { _bwd_icr = icr; _flags |= bwd_icr; }
  ///
  void setForwardTBE(int tbe)    { _fwd_tbe = tbe; _flags |= fwd_tbe; }
  ///
  void setBackwardTBE(int tbe)   { _bwd_tbe = tbe; _flags |= bwd_tbe; }
  ///
  void setCumulativeRTT(int rtt) { _cum_rtt = rtt; _flags |= cum_rtt; }
  ///
  void setForwardRIF(u_char rif) { _fwd_rif = rif; _flags |= fwd_rif; }
  ///
  void setBackwardRIF(u_char rif){ _bwd_rif = rif; _flags |= bwd_rif; }
  ///
  void setForwardRDF(u_char rdf) { _fwd_rdf = rdf; _flags |= fwd_rdf; }
  ///
  void setBackwardRDF(u_char rdf){ _bwd_rdf = rdf; _flags |= bwd_rdf; }


  ///
  int getForwardICR(void)    { return _fwd_icr; }
  ///
  int getBackwardICR(void)   { return _bwd_icr; }
  ///
  int getForwardTBE(void)    { return _fwd_tbe; }
  ///
  int getBackwardTBE(void)   { return _bwd_tbe; }
  ///
  int getCumulativeRTT(void) { return _cum_rtt; }
  ///
  int getForwardRIF(void)    { return _fwd_rif; }
  ///
  int getBackwardRIF(void)   { return _bwd_rif; }
  ///
  int getForwardRDF(void)   { return _fwd_rdf; }
  ///
  int getBackwardRDF(void)   { return _bwd_rdf; }


  int Length();

 protected:
  ///
  virtual char PrintSpecific(ostream & os);
  
 private:
  ///
  int     _flags;
  ///
  int     _fwd_icr;
  ///
  int     _bwd_icr;
  ///
  int     _fwd_tbe;
  ///
  int     _bwd_tbe;
  ///
  int     _cum_rtt;
  ///
  u_char  _fwd_rif;
  ///
  u_char  _bwd_rif;
  ///
  u_char  _fwd_rdf;
  ///
  u_char  _bwd_rdf;
};

#endif






