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
 * File: VCallocate.h
 * @author marsh
 * @version $Id: VCAllocator.h,v 1.4 1998/07/01 14:55:47 mountcas Exp $
 *
 *
 * BUGS:
 */
#ifndef _VCALLOCATOR_H_
#define _VCALLOCATOR_H_

#ifndef LINT
static char const _VCallocator_h_rcsid_[] =
"$Id: VCAllocator.h,v 1.4 1998/07/01 14:55:47 mountcas Exp $";
#endif

#include <iostream.h>
#include <DS/containers/list.h>

/** VCAllocator keeps track of all previously allocated VC's and a list of
      free VC's.
 */
class VCAllocator {
public:    

  /// Constructor.
  VCAllocator(void);
  /// Destructor.
  ~VCAllocator();

  /// Returns the next available VC, 0 is a failure.
  unsigned int GetNewVCI(void);
  /// Returns VC if the requested VC is available otherwise it returns 0.
  unsigned int RequestVCI(unsigned int vci);
  /// Places the passed VC back on the free VC list.
  void ReturnVCI(unsigned int vci);         

  void UnReference(void);
  VCAllocator * Reference(void);

private:

  int _ref_count;
  /// List of free VC's.
  list<unsigned int> _svc_free;
  /// List of used VC's.
  list<unsigned int> _svc_in_use;
  /// Used when the VCAllocator must allocate more VC's for its free list.
  unsigned int       _lowest;

  /**@name Local enumerations.
   */
  //@{
  enum { 
    /// The first available VC.
    MinVCI = 32, 
    /// The smallest the free list will ever be allowed to get.
    MinSize = 15 
  };
  //@}
};

#endif // __VCALLOCATOR_H__
