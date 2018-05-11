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

//-*- C++ -*- 

#if !defined(__OPTICAL_VISITOR_H__)
#define __OPTICAL_VISITOR_H__

#ifndef LINT
static char const _XTIadapterInstallerVisitor_h_rcsid_[] =
"$Id: XTIadapterInstallerVisitor.h,v 1.3 1998/12/23 14:13:11 battou Exp $";
#endif

#include <FW/basics/Visitor.h>

#define XTI_VISITOR_NAME "XTIadapterInstallerVisitor"

class XTIadapterInstallerVisitor : public Visitor {
public:
  XTIadapterInstallerVisitor(u_int vpvc, Conduit *c);
  u_int GetVPVC(void);
  Conduit *GetConduit(void);
  virtual const VisitorType GetType(void) const;
protected:
  virtual Visitor *dup(void) const;
  virtual ~XTIadapterInstallerVisitor(void);
  virtual void on_death(void) const;
  XTIadapterInstallerVisitor(vistype &child_type,u_int vpvc, Conduit *c); 
  XTIadapterInstallerVisitor(const XTIadapterInstallerVisitor &x);
  virtual const vistype & GetClassType(void) const;
  u_int _vpvc;
  Conduit *_c;
private:
  static vistype _my_type;
};

#endif

