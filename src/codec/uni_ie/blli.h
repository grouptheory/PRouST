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

#ifndef _BLLI_H_
#define _BLLI_H_
#if !defined(LINT)
static char const _blli_h_rcsid_[] =
"$Id: blli.h,v 1.4 1998/09/28 09:12:00 bilal Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class ie_errmsg;

class    lan_iso_8802;
class    user2;
class    ie_blli;

/*
 * Broadband Lower Layer Information: TODO
 * see 5.4.5.9 page 218 in UNI-3.1 book
 * used to signal the LLC/SNAP encapsulation for IP over ATM check
 * rfc1755 section 6.2
 */


class layer2 {
 friend class ie_blli;
 public:

  enum layer2_types {
    none         = 0x00,
    lan_iso_8802 = 0x4c, // ip over ATM
    user2        = 0x50
  };
  enum layer2_operation {
    undefined_mode  = 0x00,
    normal_mode     = 0x20,
    extended_mode   = 0x40
  };
  layer2(layer2_types id);
  virtual ~layer2( ) { }

  virtual layer2 * copy(void) const = 0;
  // layer2 type matching only
  virtual int operator == (const layer2 &) const;

  // encodes only byte 6
  virtual int encode(u_char *buffer);
  layer2_types get_layer2_type() const;
  void set_layer2_type(layer2_types lt);

 protected:
  char PrintSpecific(ostream & os);
  layer2_types _layer2_type;
};


class l2_user : public layer2 {
public:
  l2_user(int user_info);
  virtual ~l2_user() { }
  virtual layer2 * copy(void) const;

  virtual int encode(u_char *buffer);

protected:

  char PrintSpecific(ostream & os);
  int _user_info;
};

/*
 * used for ip over atm
 */

class l2_lan_iso_8802 : public layer2 {
public:
  l2_lan_iso_8802(void);
  virtual ~l2_lan_iso_8802() { }
  virtual layer2 * copy(void) const;
};


//----------------------------------------------------------------------------
class layer3 {
 friend class ie_blli;
 public:
  enum layer3_types {
    none           = 0x00,
    user3          = 0x70
  };
  enum default_pkt_size {
    pkt_16_bytes   = 0x04,
    pkt_32_bytes   = 0x05,
    pkt_64_bytes   = 0x06,
    pkt_128_bytes  = 0x07,
    pkt_256_bytes  = 0x08,
    pkt_512_bytes  = 0x09,
    pkt_1024_bytes = 0x0a,
    pkt_2048_bytes = 0x0b,
    pkt_4096_bytes = 0x0c
  };
  enum layer3_operation {
    normal_mode   = 0x20,
    extended_mode = 0x40
  };

  layer3(layer3_types id);
  virtual ~layer3() { }
  virtual layer3 * copy(void) const = 0;
  virtual int equals (const layer3 * l3) const = 0;
  virtual int operator == (const layer3 &) const;
  virtual int encode(u_char * buf)= 0;
  layer3_types get_layer3_type() const;

protected:

  char PrintSpecific(ostream & os);
  layer3_types _layer3_type;
};


class l3 : public layer3 {
public:
  l3(layer3::layer3_types lt);
  virtual ~l3();
  virtual layer3 * copy(void) const;
  virtual int equals (const layer3 * l3) const;
  virtual int encode(u_char * buf);
};


class ie_blli : public InfoElem {

 public:

  int equals(const InfoElem* him) const;
  int equals(const ie_blli* himptr) const;

  enum layertype {
    layer_1_id = 0x20,
    layer_2_id = 0x40,
    layer_3_id = 0x60,
    snap_id    = 0x00
  };

  ie_blli(layer2::layer2_types l2, layer3::layer3_types l3);
  ie_blli(const ie_blli & rhs);
  ie_blli(void);

  virtual ~ie_blli();

  int Length();

  // need deep copy
  virtual InfoElem* copy(void) const ;

  // deep equal
  virtual int operator == (const ie_blli & blli) const;

  virtual int get_num_blli(void);

  virtual u_char IsVariableLen(void) const;

  int max_repeats();

  // call respective layer encode
  virtual int                 encode(u_char * buffer);

  // call respective layer decode
  virtual InfoElem::ie_status decode(u_char *, int &);

 protected:

  char PrintSpecific(ostream & os);
  friend ostream & operator << (ostream & os, ie_blli & x);
  int      _l2_len;
  layer2*  _l2;
  int      _l3_len;
  layer3*  _l3;
  int            _num_blli;
  class ie_blli *_next_blli;
  u_char *_blli;
  int     _blli_len;
};

#endif


