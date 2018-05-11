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
#ifndef LINT
static char const _Fab_cc_rcsid_[] =
"$Id: Fab.cc,v 1.19 1999/03/01 17:03:37 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <fsm/forwarder/Fab.h>
#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/addr.h>

// it's high in fiber!
Fab::Fab(int port, int vp, int vc, int cref, Addr * addr) :
  _port(port), _vp(vp), _vc(vc), _cref(cref), _dest(addr) { }

Fab::Fab(const Fab & rhs) :
  _port(rhs._port), _vp(rhs._vp), _vc(rhs._vc),
  _cref(rhs._cref), _dest(rhs._dest) { }

Fab::~Fab() { }

bool Fab::equals(const Fab & rhs) const
{
  return (_port == rhs._port &&
	  //	  _vp   == rhs._vp   &&
	  //	  _vc   == rhs._vc   &&
	  _cref == rhs._cref);
}

Fab & Fab::operator = (const Fab & rhs)
{
  _port = rhs._port;
  _vp   = rhs._vp;
  _vc   = rhs._vc;
  _cref = rhs._cref;
  _dest = rhs._dest;

  return *this;
}

int operator == (const Fab & lhs, const Fab & rhs)
{
  return lhs.equals(rhs);
}

int operator < (const Fab & lhs, const Fab & rhs)
{
  if (lhs._port < rhs._port)
    return 1;
#if 0
  if (lhs._port == rhs._port &&
      lhs._vp < rhs._vp)
    return 1;
  if (lhs._port == rhs._port &&
      lhs._vp == rhs._vp &&
      lhs._vc < rhs._vc)
    return 1;
#endif
  if (lhs._port == rhs._port &&
      //      lhs._vp == rhs._vp &&
      //      lhs._vc == rhs._vc &&
      lhs._cref < rhs._cref)
    return 1;
  return 0;
}

int operator >  (const Fab & lhs, const Fab & rhs)
{
  if (lhs._port > rhs._port)
    return 1;
#if 0
  if (lhs._port == rhs._port &&
      lhs._vp > rhs._vp)
    return 1;
  if (lhs._port == rhs._port &&
      lhs._vp == rhs._vp &&
      lhs._vc > rhs._vc)
    return 1;
#endif
  if (lhs._port == rhs._port &&
      //      lhs._vp == rhs._vp &&
      //      lhs._vc == rhs._vc &&
      lhs._cref > rhs._cref)
    return 1;
  return 0;
}

ostream & operator << (ostream & os, const Fab & rhs)
{
  os.setf(ios::dec);
  // [Port, VP, VC, CREF] - what about the Address?
  os << "[" << (int)rhs._port << "," << (int)rhs._vp << "," 
     << (int)rhs._vc << "," << hex << (int)rhs._cref << dec << "]" << flush;
  return os;
}

/// Returns the address
Addr * Fab::GetAddr(void) const { return (_dest ? _dest->copy() : 0); }
/// Returns the port part of the Fab.
int Fab::GetPort(void) const    { return _port; }
/// Returns the vp part of the Fab.
int Fab::GetVP(void) const      { return _vp; }
/// Returns the vc part of the Fab.
int Fab::GetVC(void) const      { return _vc; }
/// Returns the call reference value
int Fab::GetCREF(void) const    { return _cref; }
/// Set the port part of the Fab.
bool Fab::SetPort(int x)        { _port = x; return true; }
///  Set the vp part of the Fab.
bool Fab::SetVP(int x)          { _vp = x; return true;  }
///  Set the vc part of the Fab.
bool Fab::SetVC(int x)          { _vc = x; return true;  }
/// Set the call reference
bool Fab::SetCREF(int x)        { _cref = x; return true;  }
///
void Fab::SetAddr(Addr * x)     { _dest = x; }
