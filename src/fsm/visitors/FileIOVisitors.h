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
 * File: FilIOVisitors.h
 * @author talmage
 * @version $Id: FileIOVisitors.h,v 1.4 1998/08/14 01:49:11 mountcas Exp $
 *
 * These are Visitors that tell objects to read, write, and
 * pretty-print themselves.
 *
 * BUGS: */

#ifndef __FILE_IO_VISITOR_H__
#define __FILE_IO_VISITOR_H__
#ifndef LINT
static char const _SaveVisitor_h_rcsid_[] =
"$Id: FileIOVisitors.h,v 1.4 1998/08/14 01:49:11 mountcas Exp $";
#endif

#include <fsm/visitors/DataVisitor.h>

class NodeID;

//
// Save to a file.
//
#define SAVE_VISITOR_NAME "SaveVisitor"
class SaveVisitor : public DataVisitor {
public:
  //
  // Make a copy of string.
  //
  SaveVisitor(const char *string, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  //
  // Get value from environment_variable, making a copy of it.
  // If environment_variable is NULL, make a copy of default_value.
  //
  SaveVisitor(const char *environment_variable, 
	      const char *default_value, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  virtual const VisitorType GetType(void) const;

protected:

  virtual ~SaveVisitor();

  Visitor * dup(void) const { return new SaveVisitor(*this); }

  SaveVisitor(vistype &child_type, 
	      const char *string,
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  SaveVisitor(vistype &child_type, const char *environment_variable, 
	      const char *default_value, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  virtual const vistype &GetClassType(void) const;

private:

  static vistype _my_type;
};


// This visitor carries around an ostream, and cannot travel very far.
#define STREAM_SAVE_VISITOR_NAME "StreamSaveVisitor"
class StreamSaveVisitor : public Visitor {
public:

  StreamSaveVisitor(ostream & os);
  virtual ~StreamSaveVisitor();

  ostream & GetStream(void) const;

  virtual const VisitorType GetType(void) const;

protected:

  virtual const vistype & GetClassType(void) const;
  Visitor * dup(void) const { return new StreamSaveVisitor(*this); }

private:

  ostream & _os;

  static vistype _my_type;
};

//
// Read from a file.
//
#define READ_VISITOR_NAME "ReadVisitor"
class ReadVisitor : public DataVisitor {
public:
  //
  // Make a copy of string.
  //
  ReadVisitor(const char *string, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  //
  // Get value from environment_variable, making a copy of it.
  // If environment_variable is NULL, make a copy of default_value.
  //
  ReadVisitor(const char *environment_variable, 
	      const char *default_value, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  virtual const VisitorType GetType(void) const;

protected:

  virtual ~ReadVisitor();

  ReadVisitor(vistype &child_type, 
	      const char *string,
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  ReadVisitor(vistype &child_type, const char *environment_variable, 
	      const char *default_value, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  virtual const vistype &GetClassType(void) const;
  Visitor * dup(void) const { return new ReadVisitor(*this); }

private:

  static vistype _my_type;
};


//
// Pretty-print to a file.
//
#define SHOW_VISITOR_NAME "ShowVisitor"
class ShowVisitor : public DataVisitor {
public:
  //
  // Make a copy of string.
  //
  ShowVisitor(const char *string, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  //
  // Get value from environment_variable, making a copy of it.
  // If environment_variable is NULL, make a copy of default_value.
  //
  ShowVisitor(const char *environment_variable, 
	      const char *default_value, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  virtual const VisitorType GetType(void) const;

protected:

  virtual ~ShowVisitor();

  ShowVisitor(vistype &child_type, 
	      const char *string,
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  ShowVisitor(vistype &child_type, const char *environment_variable, 
	      const char *default_value, 
	      int port = -1, int agg = -1, int vpvc = -1, 
	      NodeID * src_nid = 0, NodeID * dest_nid = 0);

  virtual const vistype &GetClassType(void) const;
  Visitor * dup(void) const { return new ShowVisitor(*this); }

private:

  static vistype _my_type;
};


#endif // __FILE_IO_VISITOR_H__
