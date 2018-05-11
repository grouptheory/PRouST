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
#ifndef __FAB_H__
#define __FAB_H__

#ifndef LINT
static char const _Fab_h_rcsid_[] =
"$Id: Fab.h,v 1.9 1999/03/01 17:03:40 mountcas Exp $";
#endif

#include <iostream.h>
class NodeID;
class Addr;

/** Fab is used to contain a port, vp, vc combination.
     Used by the DataForwarder.
 */
class Fab {
  /// Output operator.
  friend ostream & operator << (ostream & os, const Fab & rhs);
  /// Equivalence operator.
  friend int operator == (const Fab & lhs, const Fab & rhs);
  /// Less-than operator.
  friend int operator <  (const Fab & lhs, const Fab & rhs);
  /// Greater-than operator.
  friend int operator >  (const Fab & lhs, const Fab & rhs);
  /// compare function used by LEDA.
  friend int compare(Fab *const &lhs, Fab *const &rhs);
public:

  /// Constructor takes a port, vp, and vc.
  Fab(int port = 0, int vp = 0, int vc = 0, int cref = 0, Addr * addr = 0);
  /// Copy constructor.
  Fab(const Fab & rhs);
  /// Destructor.
  ~Fab();

  /// Returns true if this Fabs is equivalent to rhs.
  bool equals(const Fab & rhs) const;
  /// Assignment operator.
  Fab & operator = (const Fab & rhs);

  /// Returns the port part of the Fab.
  int GetPort(void) const;
  /// Returns the vp part of the Fab.
  int GetVP(void) const;
  /// Returns the vc part of the Fab.
  int GetVC(void) const;
  /// Returns the call reference value
  int GetCREF(void) const;
  /// Returns the address
  Addr * GetAddr(void) const;

  /// Set the port part of the Fab.
  bool SetPort(int x);
  ///  Set the vp part of the Fab.
  bool SetVP(int x);
  ///  Set the vc part of the Fab.
  bool SetVC(int x);
  /// Set the call reference
  bool SetCREF(int x);
  ///
  void SetAddr( Addr * );

private:

  /// Port.
  int _port;
  /// VP.
  int _vp;
  /// VC.
  int _vc;
  /// Call Reference
  int _cref;
  /// Destination Party Address
  Addr * _dest;
};

#endif // __FAB_H__
