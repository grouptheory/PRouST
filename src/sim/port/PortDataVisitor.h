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
 * File: PortDataVisitor.h
 * @author talmage
 * @version $Id: PortDataVisitor.h,v 1.15 1998/07/30 21:10:16 mountcas Exp $
 * @memo Carries data into a Port.
 * BUGS:
 */

#ifndef __PORT_DATA_VISITOR_H__
#define __PORT_DATA_VISITOR_H__
#ifndef LINT
static char const _PortDataVisitor_h_rcsid_[] =
"$Id: PortDataVisitor.h,v 1.15 1998/07/30 21:10:16 mountcas Exp $";
#endif

#include <fsm/visitors/VPVCVisitor.h>

class Accessor;
class Mux;

// When deriving new Visitors you may choose to redefine these methods:
//
//      // Action to be taken when it arrives at a Mux with the specified Accessor.
//      void at(Mux * m, Accessor * a);
//
//      // Action to be taken when it arrives at a Factory with the specified Creator.
//      void at(Factory * f, Creator * c);
//
//      // Action to be taken when it arrives at a Protocol with the specified State.
//      void at(Protocol * p, State * s);
//
//      // Action to be taken when it arrives at an Adapter with the specified Terminal.
//      void at(Adapter * a, Terminal * t);
//

// Uses all default Visitor at methods.  Certain Accessors are able to deal only 
//    with this this type of Visitor.

#define PORT_DATA_VISITOR_NAME "PortDataVisitor"

class PortDataVisitor : public VPVCVisitor {
public:

  PortDataVisitor(int port = 0, int vpvc = 0, void *data = 0);

  virtual const VisitorType GetType(void) const;

  virtual const void * GetData(void) const;
  virtual void SetData(void *data);

protected:

  virtual ~PortDataVisitor();
  PortDataVisitor(vistype &child_type, int port = 0, int vpvc=0, void *data = 0);
  virtual const vistype &GetClassType(void) const;
  virtual Conduit * at(Factory *f, Creator *c);

  void *_data;

private:
  static vistype _mytype;
};

#endif
