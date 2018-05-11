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
static char const _LinkStateWrapper_h_rcsid_[] =
"$Id: LinkStateWrapper.h,v 1.8 1998/11/24 14:33:19 mountcas Exp $";
#endif
/* -*- C++ -*-
 * File: LinkStateWrapper.h
 * Author: talmage
 * Version: $Id: LinkStateWrapper.h,v 1.8 1998/11/24 14:33:19 mountcas Exp $
 * Purpose: Encapsulates the context of the Join() of two switch ports.
 * BUGS:
 */

#ifndef __LINK_STATE_WRAPPER_H__
#define __LINK_STATE_WRAPPER_H__

#include <sim/switch/SwitchFunctions.h>
#include <sim/port/LinkState.h>

class Conduit;

class LinkStateWrapper {
  friend LinkStateWrapper * LinkSwitches(Conduit * switch1, int port1, 
					 Conduit * switch2, int port2,
					 list<ig_resrc_avail_info *> * r1, 
					 list<ig_resrc_avail_info *> * r2);

  friend bool UnlinkSwitches(LinkStateWrapper *link);
public:

  inline LinkStateWrapper(Conduit * container = 0, LinkState * state = 0,
			  Conduit * leftNeighbor = 0, Conduit * rightNeighbor = 0,
			  int leftPort = -1, int rightPort = -1) : 
    _container(container), _theState(state),
    _leftNeighbor(leftNeighbor), _rightNeighbor(rightNeighbor),
    _leftPort(leftPort), _rightPort(rightPort) { };

  inline ~LinkStateWrapper() {
    _container = _leftNeighbor = _rightNeighbor = 0;
    _theState = 0;
    _leftPort = _rightPort = -1;
  };

  inline Conduit * Container(void)     const { return _container; };
  inline Conduit * LeftNeighbor(void)  const { return _leftNeighbor; };
  inline Conduit * RightNeighbor(void) const { return _rightNeighbor; };
  inline int       LeftPort(void)      const { return _leftPort; };
  inline int       RightPort(void)     const { return _rightPort; };

  inline void      Disable(void) { assert(_theState != 0); _theState->disable(); }
  inline void      Enable(void)  { assert(_theState != 0); _theState->enable(); }

private:

  Conduit * _container;		/* The Conduit of the Protocol that 
				 * contains the LinkState object.
				 */
  LinkState * _theState;        // The actual LinkState object

  Conduit * _leftNeighbor;	/* The B side of the Conduit to the 
				 * left of the link.
				 */
  Conduit * _rightNeighbor;	/* The B side of the Conduit to the 
				 * right of the link.
				 */
  int _leftPort;		// Port of the left neighbor.
  int _rightPort;		// Port of the right neighbor.
};

#endif
