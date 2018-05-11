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
"$Id: linktst.cc,v 1.8 1999/02/19 21:22:59 marsh Exp $";
#endif

#include <iostream.h>
#include "String.h"

int main(int argc, char** argv){
  char *string_of_A = "A";
  char *string_of_B = "B";

  cout << "Testing the constructor ds_String(const char *str)" << endl;
  ds_String *ds_String0 = new ds_String("This is a test");

  cout << "This is ds_String0.  It should be " << ds_String0->size()
       << " bytes" << endl;
  cout << *ds_String0 << endl;
  cout << endl;

  cout << "Testing the constructor ds_String(void)" << endl;
  ds_String *ds_String1 = new ds_String();
  int i;


  cout << "This is ds_String1.  It should be " << ds_String1->size() 
       << " bytes" << endl;
  cout << *ds_String1;
  cout << endl;

  for (i=0; i < 512; i++) 
    (*ds_String1)+= *string_of_A;

  cout << "This is ds_String1.  It should be " << ds_String1->size() 
       << " bytes" << endl;
  cout << *ds_String1;
  cout << endl;

  for (i=512; i < 1025; i++)
    (*ds_String1) += *string_of_B;

  cout << "This is ds_String1.  It should be " << ds_String1->size() 
       << " bytes" << endl;

  cout << *ds_String1;
  cout << endl;
  cout << endl;

  (*ds_String1) += 'C';
  cout << "This is ds_String1.  It should be " << ds_String1->size() 
       << " bytes" << endl;

  cout << *ds_String1;
  cout << endl;
  cout << endl;

  (*ds_String1) += 'D';
  cout << "This is ds_String1.  It should be " << ds_String1->size() 
       << " bytes" << endl;

  cout << *ds_String1;
  cout << endl;
  cout << endl;

  ds_String *ds_String2 = new ds_String(*ds_String1);

  cout << "This is ds_String2.  It is " << ds_String2->size() 
       << " bytes " << endl 
       << "and it must be identical to ds_String1." << endl;

  cout << *ds_String2;
  cout << endl;


  *ds_String2 += *ds_String1;
  cout << "This is ds_String2.  It is " << ds_String2->size() 
       << " bytes " << endl 
       << "and it must be identical to the concatenation of ds_String1 \
with itself." << endl;

  cout << *ds_String2;
  cout << endl;

  delete ds_String1;
  delete ds_String2;
  exit(0);
}
