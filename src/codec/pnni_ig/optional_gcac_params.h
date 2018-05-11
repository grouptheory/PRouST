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
#ifndef __OPT_GCAC_PARAMS_H__
#define __OPT_GCAC_PARAMS_H__

#ifndef LINT
static char const _opt_gcac_params_h_rcsid_[] =
"$Id: optional_gcac_params.h,v 1.4 1998/09/24 13:14:16 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>

#define GCAC_CONTENT 12 //type(2), len(2), CRM(4), VF(4)

/** The Optional GCAC parameters IG is included in the resources availability
    IG.  It provides the cell rate margin and variance factor.
 */
class ig_optional_gcac_params : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_optional_gcac_params & ig);
  friend istream & operator >> (istream & is, ig_optional_gcac_params & ig);

public:

  /// Constructor.
  ig_optional_gcac_params(int crm = 0, int vf = 0);
  ig_optional_gcac_params(const ig_optional_gcac_params & rhs);

  /// Destructor.
  virtual ~ig_optional_gcac_params( );

  /**@name Methods for encoding/decoding the parameters.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}

  const int GetCellRateMargin(void) const;
  const int GetVarianceFactor(void) const;

  virtual InfoGroup * copy(void);
  void size(int & length); 

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  int    _cell_rate_margin;
  int    _variance_factor;
};

#endif // __OPT_GCAC_PARAMS_H__
