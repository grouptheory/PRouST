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
 * File: VPVCVisitor.h
 * @author talmage
 * @version $Id: VPVCVisitor.h,v 1.31 1998/12/01 13:43:41 mountcas Exp $
 *
 * Base class for a hierarchy of classes that travel through Ports in
 * a Switch over some VPVC and possibly over Links to other Ports in
 * other Switches.
 *
 * BUGS: 
 */
#ifndef __VPVC_VISITOR_H__
#define __VPVC_VISITOR_H__

#ifndef LINT
static char const _VPVCVisitor_h_rcsid_[] =
"$Id: VPVCVisitor.h,v 1.31 1998/12/01 13:43:41 mountcas Exp $";
#endif

#include <fsm/visitors/NodeIDVisitor.h>

//
// A PortExpander expects to see only PortVisitors.
//
#define VPVC_VISITOR_NAME "VPVCVisitor"

class VPVCVisitor : public NodeIDVisitor {
public:

  VPVCVisitor(int port = -1, int agg = -1, int vpvc = -1, 
	      const NodeID * src = 0, const NodeID * dest = 0);

  int  GetInVP(void) const;
  void SetInVP(int vpvc);
  int  GetInVC(void) const;
  void SetInVC(int vpvc);

  int  GetOutVP(void) const;
  void SetOutVP(int vpvc);
  int  GetOutVC(void) const;
  void SetOutVC(int vpvc);

  int  GetSavedVP(void) const;
  void SetSavedVP(int vp);
  int  GetSavedVC(void) const;
  void SetSavedVC(int vc);

  void     ClearRoutingInfo();

  void     SetByteStream(u_char *  data);
  u_char * GetByteStream(void) const;

  void SwapSrcDest(void);

  virtual const VisitorType GetType(void) const;

  VPVCVisitor & operator = (const VPVCVisitor & rhs);

  friend ostream & operator << (ostream & os, const VPVCVisitor & v);

  virtual void   SetCREF(u_long cref);
  virtual u_long GetCREF(void) const;

protected:

  virtual Visitor * dup(void) const;

  virtual ~VPVCVisitor();
  VPVCVisitor(vistype &child_type, int port = 0, int agg = 0, int vpvc = 0, 
	      const NodeID * src_nid = 0, const NodeID * dest_nid = 0);

  VPVCVisitor(const VPVCVisitor & rhs);

  virtual const vistype &GetClassType(void) const;

  u_long _cref;

  int _in_vp;
  int _in_vc;

  int _out_vp;
  int _out_vc;

  int _saved_vp;
  int _saved_vc;

  u_char * _data;

private:

  static vistype  _mytype;
  //   static u_char _VPVCInternalUseOnly[22];
};


#endif // __VPVCVISITOR_H__
