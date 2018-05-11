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
#ifndef __AGG_TOK_H__
#define __AGG_TOK_H__

#ifndef LINT
static char const _agg_tok_h_rcsid_[] =
"$Id: aggregation_token.h,v 1.4 1998/09/23 18:45:01 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
class ostream;
class istream;
class errmsg;

#define SIZE_OF_AGG_TOKEN_IE 8 // type(2), len(2), agg_token (4)

/** Aggregation Token IG is used to control how outside links are 
    aggregated at higher levels of the hierarchy.
 */
class ig_aggregation_token : public InfoGroup {
  /**@name I/O Operators.
   */
  //@{
    /** Output Operator.
      @param os reference to an ostream.
      @param ig reference to an aggregation token.
      */
  friend ostream & operator << (ostream & os, ig_aggregation_token & ig);
    /** Input Operator.
      @param is reference to an istream.
      @param ig reference to an aggregation token.
      */
  friend istream & operator >> (istream & is, ig_aggregation_token & ig);
  //@}

public:

  /** Constructor.
    @param token a long integer token.
    */
  ig_aggregation_token(long token = -1);

  /// Copy Constructor.
  ig_aggregation_token(const ig_aggregation_token & tok);

  /// Destructor.
  virtual ~ig_aggregation_token();

  /**@name Methods for encoding/decoding tokens.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char *buffer, int & buflen);

    /// Descode.
  virtual errmsg *decode(u_char *buffer);
  //@}

  const long GetToken(void) const { return _aggregation_token; }
  void       SetToken(long agg) { _aggregation_token = agg; }

  virtual InfoGroup * copy(void);

  void size(int & length);
  virtual int equals(const InfoGroup * other) const;

protected:

  virtual bool        PrintSpecific(ostream& os);
  virtual bool        ReadSpecific(istream& is);

  long _aggregation_token;
};

#endif // __AGG_TOK_H__
