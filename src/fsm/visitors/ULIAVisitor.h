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
 * File: ULIAVisitor.h
 * @author talmage
 * @version $Id: ULIAVisitor.h,v 1.3 1998/08/11 18:33:54 mountcas Exp $
 *
 * Carries ULIA messages around in a switch.
 *
 * BUGS:
 */
#ifndef __ULIA_VISITOR_H__
#define __ULIA_VISITOR_H__

#ifndef LINT
static char const _ulia_visitor_h_rcsid_[] =
"$Id: ULIAVisitor.h,v 1.3 1998/08/11 18:33:54 mountcas Exp $";
#endif

#include <fsm/visitors/LinkVisitor.h>

class NodeID;
class ig_resrc_avail_info;

#define ULIA_VISITOR_NAME "ULIAVisitor"

class ULIAVisitor : public LinkVisitor {
public:
  // local == source, remote == destination
  ULIAVisitor(int local_port, int remote_port, ig_resrc_avail_info *theRAIG);

  virtual const VisitorType GetType(void) const;

  ig_uplink_info_attr *TakeULIA(void);

protected:

  virtual Visitor * dup(void) const;

  virtual ~ULIAVisitor();

  virtual void on_death(void) const;

  ULIAVisitor(const ULIAVisitor & rhs);
  ULIAVisitor(vistype &child_type, int local_port, 
	      int remote_port, ig_resrc_avail_info *theRAIG);

  virtual const vistype & GetClassType(void) const;

private:

  static vistype _mytype;
};

#endif // __ULIAVISITOR_H__
