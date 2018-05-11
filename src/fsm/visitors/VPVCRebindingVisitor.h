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
 * File: VPVCRebindingVisitor.h
 * @author talmage
 * @version $Id: VPVCRebindingVisitor.h,v 1.3 1998/08/11 18:34:11 mountcas Exp $
 *
 * Carries two vpvcs, old and new.  The Muxes that recognize this
 * Visitor change their vpvc->Conduit bindings from old_vpvc->Conduit
 * to new_vpvc->Conduit.  It is not supposed to leave a switch.
 *
 * BUGS: 
 */

#ifndef __VPVC_REBINDING_VISITOR_H__
#define __VPVC_REBINDING_VISITOR_H__
#ifndef LINT
static char const _VPVCRebindingVisitor_h_rcsid_[] =
"$Id: VPVCRebindingVisitor.h,v 1.3 1998/08/11 18:34:11 mountcas Exp $";
#endif

#include <fsm/visitors/VPVCVisitor.h>

class NodeID;

#define VPVC_REBINDING_VISITOR_NAME "VPVCRebindingVisitor"
class VPVCRebindingVisitor : public VPVCVisitor {
public:

  VPVCRebindingVisitor(int port = -1, int agg = -1, int vpvc = -1, 
		       int old_vpvc = -1, int new_vpvc = -1, 
		       const NodeID * src = 0);

  int  GetOldVP(void) const;
  void SetOldVP(int vpvc);
  int  GetOldVC(void) const;
  void SetOldVC(int vpvc);

  int  GetNewVP(void) const;
  void SetNewVP(int vpvc);
  int  GetNewVC(void) const;
  void SetNewVC(int vpvc);

  virtual const VisitorType GetType(void) const;

  VPVCRebindingVisitor & operator = (const VPVCRebindingVisitor & rhs);

  friend ostream & operator << (ostream & os, const VPVCRebindingVisitor & v);

protected:

  virtual Visitor * dup(void) const;

  virtual ~VPVCRebindingVisitor();
  VPVCRebindingVisitor(vistype &child_type, int port = 0, int agg = 0, 
		       int vpvc = -1, 
		       int old_vpvc = -1,
		       int new_vpvc = -1,
		       const NodeID * src_nid = 0);


  VPVCRebindingVisitor(const VPVCRebindingVisitor & x);

  virtual const vistype &GetClassType(void) const;

  int _old_vp;
  int _old_vc;

  int _new_vp;
  int _new_vc;

private:

  static vistype  _mytype;
};


#endif // __VPVC_REBINDING_VISITOR_H__
