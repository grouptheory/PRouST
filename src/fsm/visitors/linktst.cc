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

#ifndef LINT
static char const _linktst_cc_rcsid_[] =
"$Id: linktst.cc,v 1.8 1998/08/06 04:05:13 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>
#include "DataVisitor.h"
#include "FastUNIVisitor.h"
#include "VPVCRebindingVisitor.h"
#include <codec/pnni_ig/id.h>

extern void DeleteFW(void);

int main(int argc, char** argv){
  VPVCRebindingVisitor *rbv = 0;
  VPVCVisitor *v = 0;
  FastUNIVisitor *fuv = 0;
  DataVisitor *dv_from_string = new DataVisitor("This is a test");
  DataVisitor *dv_from_env = new DataVisitor("PATH", "NO PATH");
  DataVisitor *dv_from_default = 
    new DataVisitor("SANTA_CLAUSE", "There is no such thing as Santa Clause");
  DataVisitor *dv_from_void = new DataVisitor((void *)1999);

  cout << "Test of the constructors and destructors" << endl;

  cout << "Value in DataVisitor constructed from a string: " 
       << dv_from_string->AsString()
       << endl;

  cout << "Value in DataVisitor constructed from the environment variable PATH: " 
       << dv_from_env->AsString()
       << endl;

  cout << "Value in DataVisitor constructed from the environment variable SANTA_CLAUSE: " 
       << dv_from_default->AsString()
       << endl;

  cout << "Value in DataVisitor constructed from a void: " 
       << (int)dv_from_void->AsVoid()
       << endl;

  dv_from_string->Suicide();
  dv_from_env->Suicide();
  dv_from_default->Suicide();
  dv_from_void->Suicide();

  u_char source_address[22] = {96, 160, 0x47, 0x00, 0x05, 
			       0x80, 0xff, 0xde, 0x00, 0x00, 
			       0x01, 0x00, 0x00, 0x04, 0x01,
			       0xff, 0x1c, 0x06, 0x01, 0x01, 
			       0x01, 0x00 };


  u_char destination_address[22] = {96, 160, 0x47, 0x00, 0x05, 
				    0x80, 0xff, 0xde, 0x00, 0x00, 
				    0x01, 0x00, 0x00, 0x04, 0x09,
				    0xff, 0x1c, 0x06, 0x09, 0x09, 
				    0x09, 0x00 };


  NodeID *source = new NodeID(source_address);
  NodeID *destination = new NodeID(destination_address);

  cout << "Constructing VPVCVisitor on port 0, agg 0, vpvc 0x05" << endl;
  cout << "From: " << *source << endl;
  cout << "To:   " << *destination << endl;
  cout << "This is it: " << endl;
  v = new VPVCVisitor(1, 0, 0x05, source, destination);
  cout << *v;
  cout << "Destroying VPVCVisitor" << endl;
  v->Suicide();

  cout << "Constructing VPVCRebindingVisitor on port 1, agg 0, vpvc 0x05, " 
       << "old vpvc 0x444, new VPVC 0x555"
       << endl;
  cout << "From: " << *source << endl;
  cout << "This is it: " << endl;
  rbv = new VPVCRebindingVisitor(1, 0, 0x05, 0x444, 0x555, source);
  cout << *rbv;
  cout << "Destroying VPVCRebindingVisitor" << endl;
  rbv->Suicide();

  source = new NodeID(source_address);
  destination = new NodeID(destination_address);

  cout << "Constructing FastUNIVisitor with zero for file name." << endl;
  cout << "There should be no DTL printed on death." << endl;
  fuv = new FastUNIVisitor(source, destination);

  cout << "Destroying FastUNIVisitor" << endl;
  fuv->Suicide();

  cout << "Constructing FastUNIVisitor with \"\" for file name." << endl;
  cout << "The DTL should be printed to cout on death." << endl;
  fuv = new FastUNIVisitor(source, destination, 1, 0x5, -1, 0,
			   FastUNIVisitor::FastUNISetup, "");
  cout << "Destroying FastUNIVisitor" << endl;
  fuv->Suicide();

  cout << "Constructing FastUNIVisitor with \"DTL_DUMP_FILE\" for file name." 
       << endl;
  cout << "The DTL should be printed there on death." << endl;
  fuv = new FastUNIVisitor(source, destination, 1, 0x5, -1, 0,
			   FastUNIVisitor::FastUNISetup, "DTL_DUMP_FILE");
  cout << "Destroying FastUNIVisitor" << endl;
  fuv->Suicide();

  delete source;
  delete destination;

  DeleteFW();
  exit(0);
}
