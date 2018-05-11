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
 * File: DataVisitor.cc
 * @author talmage
 * @version $Id: DataVisitor.cc,v 1.7 1998/08/10 20:59:29 mountcas Exp $
 *
 * BUGS:
 */

#ifndef LINT
static char const rcsid[] =
"$Id: DataVisitor.cc,v 1.7 1998/08/10 20:59:29 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "DataVisitor.h"
#include <string.h>

// --------------------- DataVisitor ---------------------------

DataVisitor::DataVisitor(const char *string, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  VPVCVisitor(_my_type, port, agg, vpvc, src_nid, dest_nid),
  _allocated_data(false)
{
  if ((_data = (u_char *)strdup(string)) != 0)
    _allocated_data = true;
}

DataVisitor::DataVisitor(const DataVisitor & rhs) : VPVCVisitor(rhs),
  _allocated_data(false), _data(0) { }

DataVisitor::DataVisitor(const char *environment_variable, 
			 const char *default_value, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  VPVCVisitor(_my_type, port, agg, vpvc, src_nid, dest_nid),
  _allocated_data(false)
{
  //
  // Note! getenv() returns a pointer to the value of the variable.
  // It doesn't make a copy of the value.
  //
  _data = (u_char *)getenv(environment_variable);
  if (_data == 0) {
    if ((_data = (u_char *)strdup(default_value)) != 0)
      _allocated_data = true;
    // else raise exception -- strdup() failed
  }
}

DataVisitor::DataVisitor(void *data, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  VPVCVisitor(_my_type, port, agg, vpvc, src_nid, dest_nid),
  _allocated_data(false)
{
  _data = (u_char *)data;
}


DataVisitor::DataVisitor(vistype &child_type, 
			 const char *string, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  VPVCVisitor(child_type.derived_from(_my_type), 
	      port, agg, vpvc, src_nid, dest_nid),
  _allocated_data(false)
{
  if ((_data = (u_char *)strdup(string)) != 0)
    _allocated_data = true;
}


DataVisitor::DataVisitor(vistype &child_type, 
			 const char *environment_variable, 
			 const char *default_value,
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  VPVCVisitor(child_type.derived_from(_my_type), 
	      port, agg, vpvc, src_nid, dest_nid),
  _allocated_data(false)
{
  //
  // Note! getenv() returns a pointer to the value of the variable.
  // It doesn't make a copy of the value.
  //
  _data = (u_char *)getenv(environment_variable);

  if (_data == 0) {
    if ((_data = (u_char *)strdup(default_value)) != 0)
      _allocated_data = true;
    // else raise exception -- strdup() failed
  }
}


DataVisitor::DataVisitor(vistype &child_type, 
			 void *data, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  VPVCVisitor(child_type.derived_from(_my_type), 
	      port, agg, vpvc, src_nid, dest_nid), 
  _allocated_data(false)
{
  _data = (u_char *)data;
}


DataVisitor::~DataVisitor() 
{ 
  if (_allocated_data) free(_data);
}


const vistype &DataVisitor::GetClassType(void) const
{
  return _my_type;
}


const VisitorType DataVisitor::GetType(void) const 
{   return VisitorType(GetClassType());  }


const char *DataVisitor::AsString(void)
{
  return (char *)_data;
}


const void *DataVisitor::AsVoid(void)
{
  return (void *)_data;
}

Visitor * DataVisitor::dup(void) const
{
  return new DataVisitor(*this);
}
