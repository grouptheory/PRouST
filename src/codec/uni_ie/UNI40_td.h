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
#ifndef _UNI40_TD_H_
#define _UNI40_TD_H_
#if !defined(LINT)
static char const _UNI40_td_h_rcsid_[] =
"$Id: UNI40_td.h,v 1.16 1998/09/01 20:45:01 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/PNNI_crankback.h>	// enum Dir

/**
 * ATM traffic descriptor
 * see 5.4.5.6 page 203 UNI-3.1  
 */

class UNI40_traffic_desc : public InfoElem {
  ///
  friend ostream & operator << (ostream & os, UNI40_traffic_desc & x);
public:

  ///
  virtual int equals(const InfoElem* him) const;
  ///
  virtual int equals(const UNI40_traffic_desc* himptr) const;

  ///
  enum dir {
    fw = 1,
    bw = 2
  };
  
   /// mask for selecting a forward/backward traffic combination
  enum TPC_FLAGS {
    PCR_0  =    0x1,
    PCR_01 =    0x2,
    SCR_0  =    0x4,
    SCR_01 =    0x8,
    MBS_0  =    0x10,
    MBS_01 =    0x20,
    BE     =    0x40,
    TAGGING=    0x80,
    MCR_01 =    0x100            // Added for UNI 4.0 support
  };

  ///
  enum VALID_TPC {
    TPC_UNSPECIFIED = 0,

    TPC1 = (PCR_0 | PCR_01),
    TPC2 = (PCR_0 | PCR_01 | TAGGING),
    TPC3 = (PCR_01 | SCR_0 | MBS_0),
    TPC4 = (PCR_01 | SCR_0 | MBS_0 | TAGGING),
    TPC5 = (PCR_01),
    TPC6 = (PCR_01 | SCR_01 | MBS_01),
    TPC7 = (PCR_01 | BE),
    // New for UNI 4.0
    TPC8 = (PCR_01 | BE | MCR_01)
  };
   
  /// Returns true if the TPC_FLAGS f is set.
  /// E.g. IsSet(Forward, UNI40_traffic_desc::PCR_01) returns true if 
  /// the object contains the Forward Peak Cell Rate 0+1.
  virtual bool IsSet(enum PNNI_crankback::Dir direction, 
		     enum UNI40_traffic_desc::TPC_FLAGS flags) const;

  /// Traffic Parameter combinations: 1 thru 6 see table 5-6 page 208
  virtual void set_TP1(dir d, int pcr_0, int pcr_01);    
  ///
  virtual void set_TP2(dir d, int pcr_0, int pcr_01);    
  ///
  virtual void set_TP3(dir d, int pcr_01, int scr_0,  int mbs_0);    
  ///
  virtual void set_TP4(dir d, int pcr_01, int scr_0,  int  mbs_0);    
  ///
  virtual void set_TP5(dir d, int pcr_01);    
  ///
  virtual void set_TP6(dir d, int pcr_0, int scr_01, int mbs_01);    
  
  /// Best Effort
  virtual void set_BE(int fpcr_01, int bpcr_01);
  /// BE with Min Cell Rate (UNI 4.0)
  virtual void set_BE(int fpcr_01, int bpcr_01, int fmcr_01, int bmcr_01);

  ///
  UNI40_traffic_desc(void);

  ///
  UNI40_traffic_desc(const UNI40_traffic_desc & him);

  ///
  virtual ~UNI40_traffic_desc(void);

  ///
  int operator == (const UNI40_traffic_desc & tp) const;  
  ///
  UNI40_traffic_desc * copy(void);
  ///
  InfoElem * copy(void) const;

  ///
  virtual u_char IsVariableLen(void) const;

  ///
  virtual int                 encode(u_char * buffer);
  ///
  virtual InfoElem::ie_status decode(u_char *, int &);
  
  // 0 is for HIGH and 1 for LOW priority (CLP = 0 or CLP = 0+1)
  /// peak cell rate
  virtual int get_FPCR_0(void) const;
  ///
  virtual int get_BPCR_0(void) const;

  ///
  virtual int get_FPCR_01(void) const;
  ///
  virtual int get_BPCR_01(void) const;

  /// sustained cell rate
  virtual int get_FSCR_0(void) const;
  ///
  virtual int get_BSCR_0(void) const;

  ///
  virtual int get_FSCR_01(void) const;
  ///
  virtual int get_BSCR_01(void) const;

  /// Maximum Burst Size    
  virtual int get_FMBS_0(void) const;
  ///
  virtual int get_BMBS_0(void) const;

  ///
  virtual int get_FMBS_01(void) const;
  ///
  virtual int get_BMBS_01(void) const;

  /// ABR Min Cell Rate
  virtual int get_FMCR_01(void) const;
  ///
  virtual int get_BMCR_01(void) const;

  ///
  virtual char complete(void);

  ///
  virtual VALID_TPC  ftpc(void) const;
  ///
  virtual VALID_TPC  btpc(void) const;

  ///
  virtual char PrintSpecific(ostream & os);

  /// Is the Best Effort Indicator turned on or not?
  virtual bool get_BE(void) const;
  virtual int Length();

 protected:
  ///
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
    TMO_id             = 0xbf,
    FMCR_01_id         = 0x92,
    BMCR_01_id         = 0x93
  };
  
  ///
  VALID_TPC  _ftpc;
  ///
  VALID_TPC  _btpc;

  ///
  int _ftagging;
  ///
  int _btagging;
  ///
  int _FPCR_0_value;
  ///
  int _BPCR_0_value;
  ///
  int _FPCR_01_value;
  ///
  int _BPCR_01_value;
  ///
  int _FSCR_0_value;
  ///
  int _BSCR_0_value;
  ///
  int _FSCR_01_value;
  ///
  int _BSCR_01_value;
  ///
  int _FMBS_0_value;
  ///
  int _BMBS_0_value;
  ///
  int _FMBS_01_value;
  ///
  int _BMBS_01_value;
  ///
  int _BE_value;
  ///
  int _TMO_value;
  /// UNI 4.0
  int _FMCR_01_value;
  ///
  int _BMCR_01_value;

  ///
  virtual void zerof(void);

  ///
  virtual void zerob(void);
};

// Specifies an alternative ATM traffic descriptor for the negotiation of
///  traffic parameters during call/conncection setup.  UNI 4.0 sec 8.1.2.1

class UNI40_alt_traffic_desc : public UNI40_traffic_desc {
  ///
  friend ostream & operator << (ostream & os, UNI40_alt_traffic_desc & x);
public:
  ///
  UNI40_alt_traffic_desc(void);
  ///
  UNI40_alt_traffic_desc(const UNI40_alt_traffic_desc & atd);
  ///
  virtual ~UNI40_alt_traffic_desc(void);

  ///
  UNI40_alt_traffic_desc & operator = (const UNI40_alt_traffic_desc & rhs);
  ///
  int                   operator ==(const UNI40_alt_traffic_desc & rhs) const;
  ///
  //  int                   equals(const UNI40_alt_traffic_desc * rhs) const;
  ///
  int                   equals(const InfoElem * rhs) const;

  ///
  UNI40_alt_traffic_desc * copy(void);

  ///
  InfoElem * copy(void) const;

  ///
  //virtual int                 encode(u_char * buffer);
  ///
  //InfoElem::ie_status decode(u_char * buffer, int & id);

};


// Specifies the minimum acceptable ATM traffic parameters in the
// negotiation of traffic parameters during call/connection setup.
// The minimum acceptable ATM traffic parameters are the lowest values
// that the user is willing to accept
///  for the call/connection.

class UNI40_min_traffic_desc : public InfoElem {
  ///
  friend ostream & operator << (ostream & os, UNI40_min_traffic_desc & x);
public:
  ///
  enum td_bitv {
    NONE   = 0x00,
    FPCR_0  = 0x01,
    BPCR_0  = 0x02,
    FPCR_01 = 0x04,
    BPCR_01 = 0x08,
    // for min_td
    FAMCR_01 = 0x10,
    BAMCR_01 = 0x20
  };

  ///
  UNI40_min_traffic_desc(void);
  ///
  UNI40_min_traffic_desc(const UNI40_min_traffic_desc & mtd);
  ///
  virtual ~UNI40_min_traffic_desc(void);

  ///
  UNI40_min_traffic_desc & operator = (const UNI40_min_traffic_desc & rhs);
  ///
  int                   operator ==(const UNI40_min_traffic_desc & rhs) const;
  ///
  int                   equals(const UNI40_min_traffic_desc * rhs) const;
  ///
  int                   equals(const InfoElem * rhs) const;

  ///
  inline UNI40_min_traffic_desc * copy(void);

  ///
  inline InfoElem * copy(void) const;

  ///
  int                 encode(u_char * buffer);
  ///
  InfoElem::ie_status decode(u_char * buffer, int & id);

  ///
  virtual u_char IsVariableLen(void) const;

  ///
  void      set_FPCR_0(int fpcr);
  ///
  void      set_BPCR_0(int bpcr);
  ///
  void      set_FPCR_01(int fpcr);
  ///
  void      set_BPCR_01(int bpcr);
  //
  void       set_ForwardABR(int abr);
  ///
  void       set_BackwardABR(int abr);
  ///
  int get_FPCR_0(void) const;
  ///
  int get_BPCR_0(void) const;
  ///
  int get_FPCR_01(void) const;
  ///
  int get_BPCR_01(void) const;
  ///
  int get_ForwardABR(void) const;
  ///
  int get_BackwardABR(void) const;
  ///
  void      reset(void);
  ///
  int Length();

  ///
  char PrintSpecific(ostream & os);

  /// Returns true if the TPC_FLAGS f is set.
  /// E.g. IsSet(UNI40_min_traffic_desc::PCR_01) returns true if 
  /// the object contains the Forward Peak Cell Rate 0+1.
  bool IsSet(enum UNI40_min_traffic_desc::td_bitv flags) const;

private:
  void setBit(td_bitv bit);
  ///
  int  isSet(td_bitv bit) const;

  ///
  int      _FPCR_0;
  ///
  int      _BPCR_0;
  ///
  int      _FPCR_01;
  ///
  int      _BPCR_01;
  ///
  int       _FAMCR_01;
  ///
  int       _BAMCR_01;
  ///
  int      _bits;
};

#endif


