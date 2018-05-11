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
 * File: SaveVisitor.cc
 * @author talmage
 * @version $Id: FileIOVisitors.cc,v 1.5 1998/08/06 04:05:08 bilal Exp $
 *
 * BUGS:
 */

#ifndef LINT
static char const rcsid[] =
"$Id: FileIOVisitors.cc,v 1.5 1998/08/06 04:05:08 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include "FileIOVisitors.h"

// --------------------- SaveVisitor ---------------------------

SaveVisitor::SaveVisitor(const char *string, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(_my_type, string, port, agg, vpvc, src_nid, dest_nid)
{
}


SaveVisitor::SaveVisitor(const char *environment_variable, 
			 const char *default_value, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(_my_type, environment_variable, default_value, port, agg, vpvc,
	      src_nid, dest_nid)
{
}


SaveVisitor::SaveVisitor(vistype &child_type,
			 const char *string, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(child_type.derived_from(_my_type), 
	      string, port, agg, vpvc, src_nid, dest_nid)
{
}


SaveVisitor::SaveVisitor(vistype &child_type, 
			 const char *environment_variable, 
			 const char *default_value,
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(child_type.derived_from(_my_type), 
	      environment_variable, default_value, port, agg, vpvc,
	      src_nid, dest_nid)
{
}


SaveVisitor::~SaveVisitor() 
{ 
}


const vistype &SaveVisitor::GetClassType(void) const
{
  return _my_type;
}


const VisitorType SaveVisitor::GetType(void) const 
{   return VisitorType(GetClassType());  }


// --------------------- StreamSaveVisitor --------------------
StreamSaveVisitor::StreamSaveVisitor(ostream & os) : Visitor(_my_type), _os(os) { }

StreamSaveVisitor::~StreamSaveVisitor() { }

ostream & StreamSaveVisitor::GetStream(void) const { return _os; }

const VisitorType StreamSaveVisitor::GetType(void) const 
{   return VisitorType(GetClassType());  }

const vistype & StreamSaveVisitor::GetClassType(void) const
{  return _my_type;  }


// --------------------- ReadVisitor ---------------------------

ReadVisitor::ReadVisitor(const char *string, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(_my_type, string, port, agg, vpvc, src_nid, dest_nid)
{
}


ReadVisitor::ReadVisitor(const char *environment_variable, 
			 const char *default_value, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(_my_type, environment_variable, default_value, port, agg, vpvc,
	      src_nid, dest_nid)
{
}


ReadVisitor::ReadVisitor(vistype &child_type,
			 const char *string, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(child_type.derived_from(_my_type), 
	      string, port, agg, vpvc, src_nid, dest_nid)
{
}


ReadVisitor::ReadVisitor(vistype &child_type, 
			 const char *environment_variable, 
			 const char *default_value,
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(child_type.derived_from(_my_type), 
	      environment_variable, default_value, port, agg, vpvc,
	      src_nid, dest_nid)
{
}


ReadVisitor::~ReadVisitor() 
{ 
}


const vistype &ReadVisitor::GetClassType(void) const
{
  return _my_type;
}


const VisitorType ReadVisitor::GetType(void) const 
{   return VisitorType(GetClassType());  }


// --------------------- ShowVisitor ---------------------------

ShowVisitor::ShowVisitor(const char *string, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(_my_type, string, port, agg, vpvc, src_nid, dest_nid)
{
}


ShowVisitor::ShowVisitor(const char *environment_variable, 
			 const char *default_value, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(_my_type, environment_variable, default_value, port, agg, vpvc,
	      src_nid, dest_nid)
{
}


ShowVisitor::ShowVisitor(vistype &child_type,
			 const char *string, 
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(child_type.derived_from(_my_type), 
	      string, port, agg, vpvc, src_nid, dest_nid)
{
}


ShowVisitor::ShowVisitor(vistype &child_type, 
			 const char *environment_variable, 
			 const char *default_value,
			 int port, int agg, int vpvc, 
			 NodeID * src_nid, NodeID * dest_nid) :
  DataVisitor(child_type.derived_from(_my_type), 
	      environment_variable, default_value, port, agg, vpvc,
	      src_nid, dest_nid)
{
}


ShowVisitor::~ShowVisitor() 
{ 
}


const vistype &ShowVisitor::GetClassType(void) const
{
  return _my_type;
}


const VisitorType ShowVisitor::GetType(void) const 
{   return VisitorType(GetClassType());  }


