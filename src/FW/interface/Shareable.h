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
#ifndef __SHAREABLE_H__
#define __SHAREABLE_H__

#ifndef LINT
static char const _Shareable_h_rcsid_[] =
"$Id: Shareable.h,v 1.2 1999/03/05 17:30:30 marsh Exp $";
#endif

#include "Moniker.h"
#include <DS/containers/dictionary.h>

class fw_Interface;
class ShareableRegistry;

int compute_depth(char * iter, const char * token);

class Shareable {
  friend class ShareableRegistry;
public:

  Shareable(void);
  virtual ~Shareable();

  // returns the fullySpecified Moniker
  Moniker & FullySpecifiedName(void);
  const Moniker & GetName(void) const;

  bool IsBlocking(void) const;

protected:

  fw_Interface * QueryInterface(const char * requested) const;
  void AddPermission(const char * key, fw_Interface * val);
  void SetMoniker(const char * name);
  void SetBlocking(bool b = true);

private:

  fw_Interface * HasPermission(const Shareable * requestor) const;
  void AddPermission(Moniker * key, fw_Interface * val);

  void SetName(const Moniker & rhs);

  Moniker                            _name;
  Moniker                            _full_name;
  bool                               _blocking;
  dictionary<Moniker *, fw_Interface *> _permissions;
};

#endif // __SHAREABLE_H__
