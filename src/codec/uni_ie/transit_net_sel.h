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
#ifndef _TRANSIT_NET_SEL_H_
#define _TRANSIT_NET_SEL_H_

#ifndef LINT
static char const _transit_net_sel_h_rcsid_[] =
"$Id: transit_net_sel.h,v 1.4 1998/02/18 23:50:12 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

/*
 * see 5.4.5.22 page 238 UNI-3.1 book
 * The purpose of this information element is to identify one requested
 * transit network. Its length is 5 bytes.
 */

class ie_transit_net_sel : public InfoElem {
  friend ostream & operator << (ostream & os, ie_transit_net_sel & x);
public:

  int equals(const InfoElem* him) const;
  int equals(const ie_transit_net_sel* himptr) const;

  enum network_id_type {
    national_network = 0x20
  };
  enum network_id_plan {
    carrier_id_code = 0x01
  };
  enum{ NET_ID_LEN = 4 };

  ie_transit_net_sel(char *net_id = 0L);
  ie_transit_net_sel(const ie_transit_net_sel & rhs);
  virtual ~ie_transit_net_sel();

  InfoElem* copy(void) const;

  int operator == (const ie_transit_net_sel & rie) const;
  
  virtual int                 encode(u_char * buffer);
  virtual InfoElem::ie_status decode(u_char * buffer, int & id);

  network_id_type get_net_id_type(void);
  int set_net_id_type(network_id_type net_id_type);
  int get_network_id_len(void);
  int Length();
private:

  virtual char PrintSpecific(ostream & os);
  network_id_type _network_id_type;
  network_id_plan _network_id_plan;
  int  _network_id_len;
  char _network_id[NET_ID_LEN];
};

#endif

