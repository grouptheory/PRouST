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
static char const _slasher2_cc_rcsid_[] =
"$Id: slasher2.cc,v 1.3 1998/08/06 04:02:25 bilal Exp $";
#endif
#include <common/cprototypes.h>
#ifdef THETYPE_INT_MK
#include "__int.h"
#else
#include "__intptr.h"
#endif

//---------------------------------------------------------------------------------------
// ***************************** NOTHING TO CHANGE BELOW HERE ***************************
//---------------------------------------------------------------------------------------

#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
#include <DS/containers/p_queue.h>

#define PHASES    8
#define DSNUM     30

#ifdef __list__
#define CLASS TEMPLATE <KEYTYPE>
#define INSERT1_IT item = ds->push(k)
#define INSERT2_IT item = ds->append(k)
#define INSERT3_IT item = ds->insert(k)
#define DELETE1    ds->del(k)
#define DELETE1_K  ds->pop()
#define DELETE2_K  ds->Pop()
#define ACCESS1_IT item = ds->search(k)
#define ACCESS2_IT item = ds->lookup(k)
#define ACCESS3_IT item = ds->first()
#define ACCESS4_IT item = ds->last()
#define ACCESS1_K  item = ds->access(k)
#define ACCESS2_K
#define ITEM       list_item
#endif
#ifdef __dictionary__
#define CLASS TEMPLATE <KEYTYPE,VALTYPE>
#define INSERT1_IT item = ds->insert(k,v)
#define INSERT2_IT 
#define INSERT3_IT 
#define DELETE1    ds->del(k)
#define DELETE1_K  
#define DELETE2_K  
#define ACCESS1_IT item = ds->lookup(k)
#define ACCESS2_IT
#define ACCESS3_IT item = ds->first()
#define ACCESS4_IT 
#define ACCESS1_K  item = ds->access(k)
#define ACCESS2_K  item = (*ds)[k]
#define ITEM       dic_item
#endif
#ifdef __p_queue__
#define CLASS TEMPLATE <KEYTYPE,VALTYPE>
#define INSERT1_IT item = ds->insert(k,v)
#define INSERT2_IT 
#define INSERT3_IT 
#define DELETE1    
#define DELETE1_K  
#define DELETE2_K
#define ACCESS1_IT k = ds->del_min()
#define ACCESS2_IT 
#define ACCESS3_IT item = ds->find_min()
#define ACCESS4_IT 
#define ACCESS1_K  
#define ACCESS2_K  
#define ITEM       pq_item
#endif

void allocate_on_heap( CLASS** hp );
void free_on_heap( CLASS** hp );
void insertions( CLASS* ds, int iter );
void deletions( CLASS* ds, int iter );
void accesses( CLASS* ds, int iter );
void itertests( CLASS* ds, int iter );
void assigns( CLASS* ds, int iter );
void test( CLASS* ds, int iter );
void hybrid_interleave( CLASS* ds, int iter );
void ptrpassed( CLASS* ds, int iter );
void stackobjpassed( CLASS ds, int iter );
void stackrefpassed( CLASS& ds, int iter );
int main(int argc, char** argv);
int compare(int* const & x, int* const & y);
int compare(double const & x, double const & y);
int compare(float const & x, float const & y);


void allocate_on_heap( CLASS** hp ) {
  int i;
  for(i=0;i<DSNUM;i++)
    hp[i]=new CLASS;
  cout << "allocate_on_heap finished " << DSNUM << endl;
}

void free_on_heap( CLASS** hp ) {
  int i;
  for(i=0;i<DSNUM;i++)
    delete hp[i];
  cout << "free_on_heap finished " << DSNUM << endl;
}

void insertions( CLASS* ds, int iter ) {
  int i;
  KEYTYPE k;
  VALTYPE v;
  ITEM item;
  for(i=0;i<iter;i++) {
    k=MAKE_KEY(); v=MAKE_VAL(); INSERT1_IT; DEL_KEY(k); DEL_VAL(v);
    k=MAKE_KEY(); v=MAKE_VAL(); INSERT2_IT; DEL_KEY(k); DEL_VAL(v);
    k=MAKE_KEY(); v=MAKE_VAL(); INSERT3_IT; DEL_KEY(k); DEL_VAL(v);
  }
}

void deletions( CLASS* ds, int iter ) {
  int i;
  KEYTYPE k;
  VALTYPE v;
  for(i=0;i<iter;i++) {
    k=MAKE_KEY(); DELETE1; DEL_KEY(k);
    DELETE1_K;
    DELETE2_K;
  }
}

void accesses( CLASS* ds, int iter ) {
  int i;
  KEYTYPE k;
  VALTYPE v;
  ITEM item;
  for(i=0;i<iter;i++) {
    k=MAKE_KEY(); ACCESS1_IT; DEL_KEY(k);
    k=MAKE_KEY(); ACCESS2_IT; DEL_KEY(k);
    ACCESS3_IT;
    ACCESS4_IT;
  }
}
 
void hybrid_interleave( CLASS* ds, int iter ) {
  int i;
  KEYTYPE k;
  VALTYPE v;
  ITEM item;
  for(i=0;i<iter;i++) {
    k=MAKE_KEY(); v=MAKE_VAL(); INSERT1_IT; DEL_KEY(k); DEL_VAL(v);
    k=MAKE_KEY(); v=MAKE_VAL(); INSERT2_IT; DEL_KEY(k); DEL_VAL(v);
    k=MAKE_KEY(); v=MAKE_VAL(); INSERT3_IT; DEL_KEY(k); DEL_VAL(v);

    k=MAKE_KEY(); ACCESS1_IT; DEL_KEY(k);
    k=MAKE_KEY(); ACCESS2_IT; DEL_KEY(k);
    ACCESS3_IT;
    ACCESS4_IT;

    k=MAKE_KEY(); DELETE1; DEL_KEY(k);
    DELETE1_K;
    DELETE2_K;
  }
}

void itertests( CLASS* ds, int iter ) {
  ITEM item;
  while (! ds->empty() ) {
    ACCESS3_IT;
    ds->del_item(item);
  }
}

void assigns( CLASS* ds, int iter ) {
}

void ptrpassed( CLASS* ds, int iter ) {
  insertions(ds,iter);
  deletions(ds,iter);
  accesses(ds,iter);
  itertests(ds,iter);
  hybrid_interleave(ds,iter);
}

void stackobjpassed( CLASS ds, int iter ) {
  ptrpassed( &ds, iter );
}


void stackrefpassed( CLASS& ds, int iter ) {
  ptrpassed( &ds, iter );
}

void test( CLASS* ds, int iter ) {
  ptrpassed(ds,iter);
  //  stackobjpassed(*ds,iter);
  stackrefpassed(*ds,iter);
}


extern "C" {
#include <assert.h>
};



CLASS stack_global[DSNUM];
CLASS* heap_global[DSNUM];

int main(int argc, char** argv) {	
  CLASS stack_local[DSNUM];
  CLASS* heap_local[DSNUM];

  allocate_on_heap(heap_local);
  allocate_on_heap(heap_global);

  int iter=1;
  for (int t=0;t< PHASES ;t++) {
    cout << "--> main loop count " << t << " starting... (iter=" << iter << ")." << endl;
    int ds;
    for (ds = 0 ; ds < DSNUM ; ds++) {
      test(heap_local[ds],iter);
      test(heap_global[ds],iter);
      test( &(stack_local[ds]) ,iter);
      test( &(stack_global[ds]) ,iter);
    }
    cout << "--> main loop count " << t << " finished." << endl;
    iter *=2;
  }
  
  free_on_heap(heap_local);
  free_on_heap(heap_global);
}


//----------------------------------------------------------------------------


int compare(KEYTYPE const & x, KEYTYPE const & y) {
  return KEY_COMPARE(x,y);
}

#if __KEY_SAME_AS_VALUE__
#else
int compare(VALTYPE const & x, VALTYPE const & y) {
  return VAL_COMPARE(x,y);
}
#endif
