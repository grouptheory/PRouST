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
#ifndef __SYSCAP_H__
#define __SYSCAP_H__

#ifndef LINT
static char const _syscap_h_rcsid_[] =
"$Id: system_capabilities.h,v 1.4 1998/09/24 13:22:42 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>

#define FIXED_SYS_CAP_CONTENT  9 // type(2), len(2), len_sys_cap_cont(2)
                                 // ieeeOUI(3)

/**  The System Capabilities IG may be used to indicate support for standard capabilities
     (Specified by the ATM Forum), and proprietary capabilities (which may be specified by
     individual equipment manufacturers).
 */
class ig_system_capabilities : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_system_capabilities & ig);
  friend istream & operator >> (istream & is, ig_system_capabilities & ig);

public:

  /// Constructor.
  ig_system_capabilities(sh_int length = 0, int oui = 0, u_char * info = 0);
  ig_system_capabilities(const ig_system_capabilities & rhs);

  /// Destructor.
  virtual ~ig_system_capabilities();

  /**@name Methods for encoding/decoding the IG.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);

    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}

  const sh_int GetContentLength(void) const;
  const    int GetIEEEOUI(void) const;
  const u_char * GetInfo(void) const;
  const u_char GetPaddingSize(void) const;

  virtual InfoGroup * copy(void);
  void size(int & length);

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  sh_int  _content_length;
  int     _ieee_oui;
  u_char *_info;
  u_char  _padding_sz;
};

#endif // __SYSCAP_H__
