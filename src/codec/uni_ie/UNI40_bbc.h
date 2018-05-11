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
#ifndef _BBC_H_
#define _BBC_H_

#ifndef LINT
static char const _bbc_h_rcsid_[] =
"$Id: UNI40_bbc.h,v 1.4 1998/09/28 16:50:19 mountcas Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class ie_bbc : public InfoElem {
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
    ATC_Absent    = -1,  // David checks this in ACAC
    CBR           = 5, 
    CBR_4         = 4, 
    CBR_6         = 6, 
    CBR_CLR       = 7,
    RT_VBR        = 9,
    RT_VBR_1      = 1,
    RT_VBR_CLR    = 19,
    NonRT_VBR     = 10,
    NonRT_VBR_0   = 0,
    NonRT_VBR_2   = 2,
    NonRT_VBR_8   = 8,
    NonRT_VBR_CLR = 11,
    ABR           = 12
  };

  ie_bbc(Bbc_class bbc, clipping clip, conn_type ctype, trans_caps atc);
  ie_bbc(void);
  ie_bbc(const ie_bbc &him);
  virtual ~ie_bbc();

  int equals(const InfoElem* him) const;
  int equals(const ie_bbc* himptr) const;
  InfoElem * copy(void) const;

  int                 encode(u_char * buffer);
  InfoElem::ie_status decode(u_char *, int &);

  Bbc_class  get_qos_class(void) const;
  clipping   get_clipping(void) const;
  conn_type  get_conn_type(void) const;
  void       set_qos_class(Bbc_class qos_class);
  void       set_clipping(clipping clip);
  void       set_conn_type(conn_type ctype);
  trans_caps get_ATC(void) const;
  void       set_ATC(trans_caps atc);
  int        Length(void);

protected:

  char PrintSpecific(ostream & os);

  Bbc_class  _qos_class;
  clipping   _clip;
  conn_type  _conn_type;
  trans_caps _atc;
};

#endif // _BBC_H_
