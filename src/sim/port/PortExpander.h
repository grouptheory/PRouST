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

/** -*- C++ -*-
 * File: PortExpander.h
 * @author talmage
 * @version $Id: PortExpander.h,v 1.18 1999/02/10 19:09:12 mountcas Exp $
 * @memo Creates Conduits that behave like the port of a switch.
 * BUGS:
 */

#ifndef __PORT_EXPANDER_H__
#define __PORT_EXPANDER_H__

#ifndef LINT
static char const _PortExpander_h_rcsid_[] =
"$Id: PortExpander.h,v 1.18 1999/02/10 19:09:12 mountcas Exp $";
#endif

#include <FW/actors/Expander.h>

class Mux;
class NodeID;
class Protocol;
class QueueState;
class VPVCAccessor;
class VPVCCreator;

//
// PortExpander models a port of a switch.  It can queue incoming
// and outgoing data.
//
// A PortExpander expects to see only PortProtocolVisitors and
// PortDataVisitors.
//
#define PORT_UPMUX_NAME "PortUpMux"
#define PORT_DOWNMUX_NAME "PortDownMux"
#define PORT_FACTORY_NAME "Factory"

class PortExpander : public Expander {
public:

  PortExpander( int port, const NodeID * node );

  virtual Conduit * GetAHalf(void) const;
  virtual Conduit * GetBHalf(void )const;

  virtual int  GetPort(void);
  virtual void SetPort(int port);

protected:

  virtual ~PortExpander(void);

  //
  // This is the queue at the bottom of the Port.
  //
  Conduit    * _port_hello;
  Conduit    * _port_nni;
  //
  // Every port has a number.
  //
  int _port;
};

#endif // __PORT_EXPANDER_H__
