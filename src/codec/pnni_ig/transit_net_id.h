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
#ifndef __TRANSIT_NET_ID_H__
#define __TRANSIT_NET_ID_H__

#ifndef LINT
static char const _transit_net_id_h_rcsid_[] =
"$Id: transit_net_id.h,v 1.8 1998/09/24 13:25:25 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>

#define FIXED_TRANSITID_CONTENT 7

/** The transit network identification IG is needed in PNNI.  This information
    group is used to carry such information.
 */
class ig_transit_net_id : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_transit_net_id & ig);
  friend istream & operator >> (istream & is, ig_transit_net_id & ig);

public:

  /// Constructor.
  ig_transit_net_id(u_char net_id_data = 0, u_char * net_id = 0, int net_id_len = 0);
  ig_transit_net_id(const ig_transit_net_id & rhs);

  /// Destructor.
  virtual ~ig_transit_net_id( );

  /**@name Methods for encoding/decoding the IG.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char *buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char *);
  //@}

  const sh_int  GetTNSLength(void) const;
  const u_char  GetNetworkData(void) const;
  const u_char *GetNetworkID(void) const;
  const sh_int  GetNetIDLength(void) const;
  // Just for completeness.
  const sh_int  GetPadding(void) const;

  virtual InfoGroup * copy(void);
  void size(int & length);

protected:

  virtual bool        PrintSpecific(ostream& os);
  virtual bool        ReadSpecific(istream& is);

  sh_int  _TNS_length;            // 2 bytes: # of bytes in Network ID + Network ID Data
  u_char  _network_id_data;       // 1 bytes: See Table 5-26, p 156
  u_char *_network_id;            // Variable N bytes: Network ID
  sh_int  _net_id_length;         // Length of _network_id array
  sh_int  _padding;               // (4 - ((3 + N)    ))    
};

#endif // _TRANSIT_NET_ID_H__
