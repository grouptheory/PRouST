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

/* -*- C++ -*-
 * File: PortAccessor.h
 * Author: talmage
 * Version: $Id: PortAccessor.h,v 1.11 1998/07/01 18:17:09 mountcas Exp $
 * Purpose: Maps PortVisitors and PortInstallerVisitors into their 
 * corresponding Conduits.
 * BUGS:
 */

#ifndef __PORT_ACCESSOR_H__
#define __PORT_ACCESSOR_H__
#ifndef LINT
static char const _PortAccessor_h_rcsid_[] =
"$Id: PortAccessor.h,v 1.11 1998/07/01 18:17:09 mountcas Exp $";
#endif

#include <FW/actors/Accessor.h>

#include <DS/containers/dictionary.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/Visitor.h>

class PortAccessor : public Accessor {
public:

  PortAccessor(void);

  virtual Conduit * GetNextConduit(Visitor * v);

protected:

  virtual ~PortAccessor();

  virtual bool Broadcast(Visitor * v);  
  virtual bool Add(Conduit * c, Visitor * v);    
  virtual bool Del(Conduit * c);
  virtual bool Del(Visitor * v);

  dictionary<int, Conduit *>  _access_map;
  static const VisitorType * _port_installer_type;
  static const VisitorType * _port_visitor_type;
  static const VisitorType * _q93b_visitor_type;
};

#endif
