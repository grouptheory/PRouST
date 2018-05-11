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
#ifndef __MONIKER_H__
#define __MONIKER_H__

#ifndef LINT
static char const _Moniker_h_rcsid_[] =
"$Id: Moniker.h,v 1.1 1999/01/21 14:55:54 mountcas Exp $";
#endif

void clean_token(char * token);

// the string is a series of period separated keys and values
//  i.e. "Switch1.Port0.ResourceManager.Database"
//       "Switch3.Port1.HelloFSM"
//       "Switch2.Port0.PhysicalNode.Election"
class Moniker {
  //  friend int compare(const Moniker & lhs, const Moniker & rhs);
  friend int compare(Moniker * const & lhs, Moniker * const & rhs);
  friend int compute_score(Moniker & lhs, Moniker & rhs);
public:

  Moniker(const char * string = 0);
  Moniker(const Moniker & rhs);
  ~Moniker();

  const Moniker & operator = (const Moniker & rhs);

  // These two methods require that you obtain the fully 
  //   specified Moniker for the Shareable.
  bool satisfied_by(const Moniker & rhs) const;
  int  difference(const Moniker & rhs, int token_index) const;

  //  operator const char * (void) const;
  const char * asString(void) const;

  char * nextToken(void);
  char * currentToken(void) const;
  char * reverseToken(int index);
  char * identifier(void);
  char * remainder(void) const;
  void   reset(void); // resets the current token pointer
  void   removeIdentifier(void);
  bool   isNull(void) const;

  // These functions are to help Shareable construct fully specified Monikers
  void prepend(const Moniker & rhs);
  void append(const Moniker & rhs);

  void clear(void);

private:

  char * _string;
  char * _current_tok;
};

#endif
