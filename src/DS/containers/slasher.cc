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
static char const _slasher_cc_rcsid_[] =
"$Id: slasher.cc,v 1.2 1998/08/06 04:02:24 bilal Exp $";
#endif
#include <common/cprototypes.h>
// -*- C++ -*-
#include <DS/containers/list.cc>

extern "C" {
#include <assert.h>
};

#define MAXITER 100

#define Make_List_Test( THE_CLASS )					\
  template class list<THE_CLASS> ;                                      \
void list_slasher_##THE_CLASS(void) {					\
									\
  list<THE_CLASS> obj_stack1;						\
  list<THE_CLASS> * obj_heap1 = new list<THE_CLASS>;			\
  list<THE_CLASS> obj_stack2;						\
  list<THE_CLASS> * obj_heap2 = new list<THE_CLASS>;			\
									\
  obj_stack2 = obj_stack1;						\
  *obj_heap2 = *obj_heap1;						\
									\
  int sz1,sz2;								\
  sz1=obj_stack1.size();						\
  sz2=obj_stack2.size();						\
  cout << "stack versions are of size " << sz1 << " " << sz2 << endl;	\
  sz1=obj_heap1->size();						\
  sz2=obj_heap2->size();						\
  cout << "heap versions are of size " << sz1 << " " << sz2 << endl;	\
									\
  int iter;								\
  for (iter=0;iter<MAXITER;iter++) {					\
    THE_CLASS val = (THE_CLASS)iter;					\
    obj_stack1.append(val);						\
    obj_heap1->append(val);						\
  }									\
									\
  sz1=obj_stack1.size();						\
  sz2=obj_stack2.size();						\
  cout << "stack versions are of size " << sz1 << " " << sz2 << endl;	\
  sz1=obj_heap1->size();						\
  sz2=obj_heap2->size();						\
  cout << "heap versions are of size " << sz1 << " " << sz2 << endl;	\
									\
  for (iter=0;iter<MAXITER;iter++) {					\
    THE_CLASS val = (THE_CLASS)iter;					\
    list_item li1, li2;							\
    li1= obj_stack1.search(val);					\
    li2= obj_heap1->search(val);					\
    assert (li1);							\
    assert (li2);							\
    THE_CLASS d1 = obj_stack1.inf(li1);					\
    THE_CLASS d2 = obj_heap1->inf(li2);					\
    assert (d1==val);							\
    assert (d2==val);							\
    assert (d1== obj_stack1[li1] );					\
    assert (d2== (*obj_heap1)[li2] );					\
  }									\
									\
  obj_stack2 = obj_stack1;						\
  *obj_heap2 = *obj_heap1;						\
									\
  sz1=obj_stack1.size();						\
  sz2=obj_stack2.size();						\
  cout << "stack versions are of size " << sz1 << " " << sz2 << endl;	\
  sz1=obj_heap1->size();						\
  sz2=obj_heap2->size();						\
  cout << "heap versions are of size " << sz1 << " " << sz2 << endl;	\
									\
  assert( !obj_stack2.empty() );					\
  assert( !obj_heap2->empty() );					\
									\
  for (iter=0;iter<MAXITER;iter++) {					\
    THE_CLASS least1;							\
    THE_CLASS sameleast1;						\
									\
    least1 = obj_stack2.pop();						\
    obj_stack2.append(least1);						\
    sameleast1 = obj_stack2.pop();					\
    obj_stack2.append(sameleast1);					\
    /* These asserts will fail until you change append to insert - spm */ \
    /* assert(least1==sameleast1);  if it was really sorted!	*/	\
									\
    least1 = obj_heap2->pop();						\
    obj_heap2->append(least1);						\
    sameleast1 = obj_heap2->pop();					\
    obj_heap2->append(sameleast1);					\
    /* assert(least1==sameleast1);  if it was really sorted!	*/	\
									\
    least1 = obj_stack2.Pop();						\
    obj_stack2.append(least1);						\
    sameleast1 = obj_stack2.Pop();					\
    obj_stack2.append(sameleast1);					\
    /* assert(least1==sameleast1);  if it was really sorted!	*/	\
									\
    least1 = obj_heap2->Pop();						\
    obj_heap2->append(least1);						\
    sameleast1 = obj_heap2->Pop();					\
    obj_heap2->append(sameleast1);					\
    /* assert(least1==sameleast1);  if it was really sorted!	*/	\
  }									\
									\
  while (obj_stack2.empty()) {						\
    list_item reader = obj_stack2.first();				\
    obj_stack2.del_item(reader);					\
  }									\
									\
  while (obj_heap1->empty()) {						\
    list_item reader = obj_heap1->first();				\
    obj_heap1->del_item(reader);					\
  }									\
									\
  sz1=obj_stack1.size();						\
  sz2=obj_stack2.size();						\
  cout << "stack versions are of size " << sz1 << " " << sz2 << endl;	\
  sz1=obj_heap1->size();						\
  sz2=obj_heap2->size();						\
  cout << "heap versions are of size " << sz1 << " " << sz2 << endl;	\
									\
  delete obj_heap1;                                                     \
  delete obj_heap2;                                                     \
}


typedef int * intptr;
Make_List_Test( int );
Make_List_Test( float );
Make_List_Test( double );
Make_List_Test( intptr );

int compare(const int & x, const int & y) {
	if (x==y) return 0;
	if (x<y) return -1;
	return 1;
}

int compare(int * const & x, int * const & y) {
	if (x==y) return 0;
	if (x<y) return -1;
	return 1;
}

int compare(double const & x, double const & y) {
	if (x==y) return 0;
	if (x<y) return -1;
	return 1;
}

int compare(float const & x, float const & y) {
	if (x==y) return 0;
	if (x<y) return -1;
	return 1;
}

main() {	
	cout << "** int\n";		      
  list_slasher_int();		
	cout << "** float\n";	      
  list_slasher_float();		
	cout << "** double\n";		      
  list_slasher_double();	
	cout << "** intptr\n";		      
  list_slasher_intptr();	
	cout << "** done\n";		
}
