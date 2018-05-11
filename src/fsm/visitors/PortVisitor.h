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
 * File: PortVisitor.h
 * @author talmage
 * @version $Id: PortVisitor.h,v 1.29 1998/12/01 13:39:33 mountcas Exp $
 *
 * Base class for a hierarchy of classes that travel through Ports 
 * in a Switch and possibly over Links to other Ports in other Switches.
 *
 * BUGS:
 */

#ifndef __PORT_VISITOR_H__
#define __PORT_VISITOR_H__
#ifndef LINT
static char const _PortVisitor_h_rcsid_[] =
"$Id: PortVisitor.h,v 1.29 1998/12/01 13:39:33 mountcas Exp $";
#endif

#include <FW/basics/Visitor.h>

#define PORT_VISITOR_NAME "PortVisitor"
class PortVisitor : public Visitor {
public:

  PortVisitor(int outport = -1, int agg = -1);

  int  GetOutPort(void) const;
  void SetOutPort(int port);

  int  GetInPort(void) const;
  void SetInPort(int port);

  void SetAgg(int agg);
  int  GetAgg(void) const;
  
  virtual const VisitorType GetType(void) const;

  PortVisitor & operator = (const PortVisitor & rhs);

protected:

  virtual Visitor * dup(void) const;

  virtual ~PortVisitor();
  virtual void on_death(void) const;

  PortVisitor(vistype & child_type, int port = -1, int agg = -1);
  PortVisitor(const PortVisitor & x);

  virtual const vistype & GetClassType(void) const;

  int _in_port;
  int _out_port;
  int _aggregation_token;

private:

  static vistype _mytype;
};

#endif // __PORTVISITOR_H__
