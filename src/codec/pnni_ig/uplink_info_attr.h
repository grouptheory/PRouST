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
#ifndef __UPLINK_INFO_H__
#define __UPLINK_INFO_H__

#ifndef LINT
static char const _uplink_info_h_rcsid_[] =
"$Id: uplink_info_attr.h,v 1.5 1998/09/24 13:28:00 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <DS/containers/list.h>

#define ULIA_HEADER 8 // type(2), len(2), seqno(4)

/** The Uplink Information Attribute is a TLV encoded container or wrapper
    used to enclose and identify other information groups that apply to the
    reverse direction on uplinks.
 */
class ig_uplink_info_attr : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_uplink_info_attr & ig);
  friend istream & operator >> (istream & is, ig_uplink_info_attr & ig);

public:

  /// Constructor.
  ig_uplink_info_attr(long seq_num = 0);

  /// Copy Constructor.
  ig_uplink_info_attr(const ig_uplink_info_attr & rhs);

  /// Destructor.
  virtual ~ig_uplink_info_attr();

  /**@name Methods for encoding/decoding the IG.
   */
  //@{
    /// Encode.
  virtual u_char    *encode(u_char * buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}
  
  void AddIG(InfoGroup * ptr);
  const list<InfoGroup *> & GetIGs(void) const;
  
  const long GetSequenceNum(void) const;

  virtual InfoGroup * copy(void);
  void size(int & length);

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  long _seq_num;

  // Contains any outgoing resource availability IGs and/or optional IGs
  list<InfoGroup *> _list;
};

#endif // __UPLINK_INFO_H__
