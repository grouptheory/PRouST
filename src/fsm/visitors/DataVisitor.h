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
 * File: DataVisitor.h
 * @author talmage
 * @version $Id: DataVisitor.h,v 1.6 1998/08/11 18:31:34 mountcas Exp $
 *
 * DataVisitor carries arbitrary data.  It can return the data as a pointer
 * to unsigned char or as a pointer to void.
 *
 * DataVisitor has three flavors of constructor.  The first requires
 * data as a string.  The second uses the value of an environment
 * variable for the data and a default value if the GetEnv() fails.
 * The third requires a pointer to void, permitting DataVisitor to
 * carry any kind of data at all.
 *
 * DataVisitor returns its data as either a string or as a void *.
 * AsString() and AsVoid() do those jobs, respectively.
 *
 * BUGS: */

#ifndef __DATA_VISITOR_H__
#define __DATA_VISITOR_H__
#ifndef LINT
static char const _DataVisitor_h_rcsid_[] =
"$Id: DataVisitor.h,v 1.6 1998/08/11 18:31:34 mountcas Exp $";
#endif

#include <fsm/visitors/VPVCVisitor.h>

class NodeID;

#define DATA_VISITOR_NAME "DataVisitor"
class DataVisitor : public VPVCVisitor {
public:

  //
  // Make a copy of string.
  //
  DataVisitor(const char *string, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  //
  // Get value from environment_variable, making a copy of it.
  // If environment_variable is NULL, make a copy of default_value.
  //
  DataVisitor(const char *environment_variable, 
	      const char *default_value, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  //
  // Does not copy data.
  //
  DataVisitor(void *data, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  virtual const VisitorType GetType(void) const;

  const char *AsString(void);
  const void *AsVoid(void);

protected:

  virtual Visitor * dup(void) const;

  virtual ~DataVisitor();

  DataVisitor(const DataVisitor & rhs);

  DataVisitor(vistype &child_type, const char *string, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  DataVisitor(vistype &child_type, const char *environment_variable, 
	      const char *default_value, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  DataVisitor(vistype &child_type, void *data, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);


  virtual const vistype &GetClassType(void) const;

  u_char * _data;
  bool     _allocated_data;	// true iff we had to make a copy of the data

private:

  static vistype _my_type;
};


#endif // __DATA_VISITOR_H__
