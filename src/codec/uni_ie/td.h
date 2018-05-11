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

#ifndef _TD_H_
#define _TD_H_
#if !defined(LINT)
static char const _td_h_rcsid_[] =
"$Id: td.h,v 1.3 1998/02/19 00:00:48 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

/*
 * ATM traffic descriptor
 * see 5.4.5.6 page 203 UNI-3.1  
 */

class ie_traffic_desc : public InfoElem {
  friend ostream & operator << (ostream & os, ie_traffic_desc & x);
public:

  virtual int equals(const InfoElem* him) const;
  virtual int equals(const ie_traffic_desc* himptr) const;

  enum dir
    {
      fw = 1,
      bw = 2
    };

   // mask for selecting a forward/backward traffic combination
  enum TPC_FLAGS {
    PCR_0  =    0x1,
    PCR_01 =    0x2,
    SCR_0  =    0x4,
    SCR_01 =    0x8,
    MBS_0  =    0x10,
    MBS_01 =    0x20,
    BE     =    0x40,
    TAGGING=    0x80
  };

  enum VALID_TPC {
    TPC_UNSPECIFIED = 0,

    TPC1 = (PCR_0 | PCR_01),
    TPC2 = (PCR_0 | PCR_01 | TAGGING),
    TPC3 = (PCR_01 | SCR_0 | MBS_0),
    TPC4 = (PCR_01 | SCR_0 | MBS_0 | TAGGING),
    TPC5 = (PCR_01),
    TPC6 = (PCR_01 | SCR_01 | MBS_01),
    TPC7 = (PCR_01 | BE)
  };
   
  // Traffic Parameter combinations: 1 thru 6 see table 5-6 page 208
  void set_TP1(dir d, int pcr_0, int pcr_01);    
  void set_TP2(dir d, int pcr_0, int pcr_01);    
  void set_TP3(dir d, int pcr_01, int scr_0,  int mbs_0);    
  void set_TP4(dir d, int pcr_01, int scr_0,  int  mbs_0);    
  void set_TP5(dir d, int pcr_01);    
  void set_TP6(dir d, int pcr_0, int scr_01, int mbs_01);    

  // Best Effort
  void set_BE(int fpcr_01, int bpcr_01);

  ie_traffic_desc(void);
  virtual ~ie_traffic_desc();

  int operator == (const ie_traffic_desc & tp) const;  
  InfoElem* copy(void) const;

  virtual u_char IsVariableLen(void);

  virtual int                 encode(u_char * buffer);
  virtual InfoElem::ie_status decode(u_char *, int &);
  
  // 0 is for HIGH and 1 for LOW priority (CLP = 0 or CLP = 0+1)
  // peak cell rate
  int get_FPCR_0(void);
  int get_BPCR_0(void);

  int get_FPCR_01(void);
  int get_BPCR_01(void);

  // sustained cell rate
  int get_FSCR_0(void);
  int get_BSCR_0(void);

  int get_FSCR_01(void);
  int get_BSCR_01(void);

  // Maximum Burst Size    
  int get_FMBS_0(void);
  int get_BMBS_0(void);

  int get_FMBS_01(void);
  int get_BMBS_01(void);

  char complete(void);

  VALID_TPC  ftpc(void) const;
  VALID_TPC  btpc(void) const;

  virtual char PrintSpecific(ostream & os);
  int Length();
 private:
  enum TP_ID {
    FPCR_0_id          = 0x82,
    BPCR_0_id          = 0x83,
    FPCR_01_id         = 0x84,
    BPCR_01_id         = 0x85,
    FSCR_0_id          = 0x88,
    BSCR_0_id          = 0x89,
    FSCR_01_id         = 0x90,
    BSCR_01_id         = 0x91,
    FMBS_0_id          = 0xa0,
    BMBS_0_id          = 0xa1,
    FMBS_01_id         = 0xb0,
    BMBS_01_id         = 0xb1,
    BE_id              = 0xbe,
    TMO_id             = 0xbf
  };
  
  VALID_TPC  _ftpc;
  VALID_TPC  _btpc;

  int _ftagging;
  int _btagging;
  int _FPCR_0_value;
  int _BPCR_0_value;
  int _FPCR_01_value;
  int _BPCR_01_value;
  int _FSCR_0_value;
  int _BSCR_0_value;
  int _FSCR_01_value;
  int _BSCR_01_value;
  int _FMBS_0_value;
  int _BMBS_0_value;
  int _FMBS_01_value;
  int _BMBS_01_value;
  int _BE_value;
  int _TMO_value;

  void zerof(void){
    _ftagging=0;
    _FPCR_0_value=0;
    _FPCR_01_value=0;
    _FSCR_0_value=0;
    _FSCR_01_value=0;
    _FMBS_0_value=0;
    _FMBS_01_value=0;
    _BE_value=0;
    _TMO_value=0;
  }

  void zerob(void){
    _btagging=0;
    _BPCR_0_value=0;
    _BPCR_01_value=0;
    _BSCR_0_value=0;
    _BSCR_01_value=0;
    _BMBS_0_value=0;
    _BMBS_01_value=0;
    _BE_value=0;
    _TMO_value=0;
  }
};

#endif

