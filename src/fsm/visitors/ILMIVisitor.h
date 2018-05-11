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
#ifndef __ILMI_VISITOR_H__
#define __ILMI_VISITOR_H__

#ifndef LINT

static char const _ILMIVisitor_h_rcsid_[] =
"$Id: ILMIVisitor.h,v 1.2 1998/04/23 20:05:21 mountcas Exp $";
#endif

#define ILMI_VPVC 0x10
#define ILMI_VISITOR_NAME "ILMIVisitor"

#if 0
/** If we were implementing ILMI, the ILMIVisitor would look like this:
  */
class ILMIVisitor : public VPVCVisitor {
public:

  /// Constructor
  ILMIVisitor(int port, int vpvc, Addr *local_atm_address, NodeID *local_node);

  /// 
  virtual const VisitorType GetType(void) const { 
    return VisitorType(GetClassType());
  }

  /// Returns the Local Node ID
  NodeID * GetLocalNodeID(void);
  /// Returns the ATM Address
  Addr * GetATMAddress(void);

protected:

  /// Destructor
  virtual ~ILMIVisitor();
  /// Method called before destruction
  virtual void on_death(void) const;
  /// Constructor for derived ILMIVisitors
  ILMIVisitor(vistype &child_type, int port, int vpvc, Addr *local_atm_address, 
	      NodeID *local_node);
  ///
  virtual const vistype &GetClassType(void) const;

  /// Pointer to the Local ATM Address
  Addr *_local_atm_address;
  /// Pointer to the Local Node ID
  NodeID *_local_node;

private:
  ///
  static vistype _mytype;
};
#endif // 0

#endif // __ILMI_VISITOR_H__
