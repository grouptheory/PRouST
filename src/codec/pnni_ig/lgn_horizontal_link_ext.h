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
#ifndef __LGN_HORIZ_LINK_H__
#define __LGN_HORIZ_LINK_H__

#ifndef LINT
static char const _lgn_horiz_link_h_rcsid_[] =
"$Id: lgn_horizontal_link_ext.h,v 1.12 1999/01/25 14:06:59 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <DS/containers/list.h>

class Database;

#define LGN_EXT_HEADER 8;
// type(2), len(2), reserv(2), hlinkcnt(2)

#define AGGR_PORT_INFO 12;
// agg_token(4), rpid(4), lpid(4)


/** The LGN horizontal link extention carries the state information about all
    of the horizontal links between two Logical Group Nodes.  It is carried
    as an addditional TLV in the same message which is used to maintain
 */
class ig_lgn_horizontal_link_ext : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_lgn_horizontal_link_ext & ig);
  friend istream & operator >> (istream & is, ig_lgn_horizontal_link_ext & ig);
  friend class Database;
public:

  /// Constructor.
  ig_lgn_horizontal_link_ext(void);

  /// Copy Constructor.
  ig_lgn_horizontal_link_ext(const ig_lgn_horizontal_link_ext & rhs);

  /// Destructor.
  virtual ~ig_lgn_horizontal_link_ext( );

  /**@name Methods for encoding/decoding of the IG.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}
  
  /**@name Maniuplation methods for the list of Horizontal Links.
   */
  //@{
    /// Adds a Horizontal Link the list.
  void AddHLink(int a, int l = 0, int r = 0);
    /// Removes a Horitzonal Link from the list.
  bool DelHLink(int a, int l = 0, int r = 0);
  //@}

  // Internal container class.
  class HLinkCont {
  public:
    HLinkCont(int a = -1, int l = -1, int r = -1);
    ~HLinkCont();

    u_char * encode(u_char * buffer, int & buflen);
    bool Print(ostream & os);
    bool Read(istream & is);

    int  GetAggTok(void) const;
    int  GetLocalPort(void) const;
    int  GetRemotePort(void) const;
    int  GetUplinkCount(void) const;
    void SetUplinkCount(int new_count);

    int equals(HLinkCont & rhs);
    int _aggregation_token;
    int _local_lgn_port;
    int _remote_lgn_port;
    // Number of uplinks declaring this aggrToken.
    // Internal purposes only. Not to be encoded.
    int _uplinks_count;
  };

  friend int compare(HLinkCont *const & lhs, HLinkCont *const & rhs);

  const list<HLinkCont *> & GetLinks(void) const;
  const list<HLinkCont *> * ShareLinks(void) const;
  const sh_int GetHLinkCount(void);

  virtual InfoGroup * copy(void);
  void size(int & length);
  HLinkCont * FindHLinkCont(int aggrToken);

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & is);
  
  sh_int            _hlink_count; // _hlinks.length()
  list<HLinkCont *> _hlinks;
};


#endif // __LGN_HORIZ_LINK_H__
