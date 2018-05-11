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
#include <iostream.h>
#include <sys/time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <common/cprototypes.h>
#include <DS/containers/list.h>
#include <DS/random/distributions.h>
#undef forall_items

// ------------ FOR TESTING ONLY -------------
#include "rb_tree.cc"
int compare(const int & x, const int & y) { return (x - y); }
template class rb_tree<int> ;

#define ONE_MIL 1000000

int SIZE = 100;

double diffTime(struct timeval * start, 
		struct timeval * stop)
{
  double rval = 0;
  
  if (stop->tv_usec < start->tv_usec) {
    stop->tv_sec  = stop->tv_sec - 1;
    stop->tv_usec = stop->tv_usec + ONE_MIL;
  }
  //  cout << "Start " << start->tv_sec << " " << start->tv_usec << " "
  //       << "Stop " << stop->tv_sec << " " << stop->tv_usec << endl;

  rval = (stop->tv_sec - start->tv_sec) + 
         ((stop->tv_usec - start->tv_usec)/ONE_MIL);
  return rval;
}

void testList(list<int> & int_list)
{
  cout << "Building a List of " << SIZE << " elements." << endl;

  int i;
  double size = SIZE;
  double diff = 0.0;

  struct timeval start, stop;
  gettimeofday(&start, 0);
  for (i = SIZE - 1; i >= 0; i--)
    int_list.insert( (int)uniform_distribution::sample( 0, SIZE-1) );
  gettimeofday(&stop, 0);

  // diff += diffTime(&start, &stop);
  if (stop.tv_usec < start.tv_usec) {
    stop.tv_sec -= 1;
    stop.tv_usec += ONE_MIL;
  }
  diff = (stop.tv_usec - start.tv_usec) + (ONE_MIL * (stop.tv_sec - start.tv_sec));
  diff /= ONE_MIL;

  cout << "The build took " << diff << " seconds.  (Linear Time)" << endl;

  assert( int_list.length() == SIZE );

  for (i = 0; i < SIZE / 2; i++) {
    list_item li;
    if (li = int_list.search( (int)uniform_distribution::sample( 0, SIZE-1) )) {
      // cout << "Found " << int_list.inf(li) << endl;
    }
  }

  cout << "Smallest item is " << int_list.head() << endl;
  cout << "Largest item is " << int_list.tail() << endl;
  cout << "Timing results for List of " << int_list.length() 
       << " elements .... (avg over " << SIZE << " iterations)" << endl;

  diff = 0;
  for (i = 0; i < SIZE; i++) {
    gettimeofday(&start, 0);
    int_list.first();
    gettimeofday(&stop, 0);

    // diff += diffTime(&start, &stop);
    if (stop.tv_usec < start.tv_usec) {
      stop.tv_sec -= 1;
      stop.tv_usec += ONE_MIL;
    }
    diff += (stop.tv_usec - start.tv_usec) + (ONE_MIL * (stop.tv_sec - start.tv_sec));
  }
  diff /= ONE_MIL;

  diff /= size;
  cout << "Locating the first item takes " << diff << " seconds.  (Constant Time)" << endl;

  diff = 0.0;
  for (i = 0; i < SIZE; i++) {
    gettimeofday(&start, 0);
    int_list.last();
    gettimeofday(&stop, 0);
    // diff += diffTime(&start, &stop);
    if (stop.tv_usec < start.tv_usec) {
      stop.tv_sec -= 1;
      stop.tv_usec += ONE_MIL;
    }
    assert( stop.tv_sec == start.tv_sec );
    diff += (stop.tv_usec - start.tv_usec);
  }
  diff /= ONE_MIL;

  diff /= size;
  cout << "Locating the last item takes " << diff << " seconds.  (Constant Time)" << endl;

  diff = 0.0;
  for (i = 0; i < SIZE; i++) {
    gettimeofday(&start, 0);
    int_list.search( (int)uniform_distribution::sample( 0, SIZE-1) );
    gettimeofday(&stop, 0);
    // diff += diffTime(&start, &stop);
    if (stop.tv_usec < start.tv_usec) {
      stop.tv_sec -= 1;
      stop.tv_usec += ONE_MIL;
    }
    diff += (stop.tv_usec - start.tv_usec) + (ONE_MIL * (stop.tv_sec - start.tv_sec));
  }
  diff /= ONE_MIL;

  diff /= size;
  cout << "Locating a specific item takes " << diff << " seconds.  (Linear Time)" << endl;
}

void testRedBlack(rb_tree<int> & int_tree)
{
  cout << "Building a Red Black Tree of " << SIZE << " elements." << endl;

  int i;
  double size = SIZE;
  double diff = 0.0; 
  int last;

  struct timeval start, stop;
  gettimeofday(&start, 0);
  for (i = 0; i < SIZE; i++) {
    int x = (int)uniform_distribution::sample( 0, SIZE-1);
    // This way we only get unique numbers in there
    while ( int_tree.lookup( x ) )
      x = (int)uniform_distribution::sample( 0, SIZE-1);

    int_tree.insert( x );
    cout << "------- inserting --------" << x << endl;
    rb_item ri;
    forall_items(ri, int_tree) {
      cout << int_tree.inf(ri) << endl;
    }
    cout << "--------------------------\n";

    double p = uniform_distribution::sample(0.0,1.0);
    if (p>0.3) {
      last = x;
    }
    if (p>0.5) {
      rb_item ri;
      cout << "----deleting " << last << endl;
      rb_item dd = int_tree.lookup(last);
      if (dd) int_tree.del_item(dd);

      forall_items(ri, int_tree) {
	cout << int_tree.inf(ri) << endl;
      }
    }
  }

  gettimeofday(&stop, 0);
  // diff += diffTime(&start, &stop);
  if (stop.tv_usec < start.tv_usec) {
    stop.tv_sec -= 1;
    stop.tv_usec += ONE_MIL;
  }

  diff = (stop.tv_usec - start.tv_usec) + (ONE_MIL * (stop.tv_sec - start.tv_sec));
  diff /= ONE_MIL;

  cout << "The build took " << diff << " seconds.  (Linear Time)" << endl;

  //  assert( int_tree.length() == SIZE );

  for (i = 0; i < SIZE / 2; i++) {
    rb_item ri;
    if (ri = int_tree.search( (int)uniform_distribution::sample( 0, SIZE-1) )) {
      // cout << "Found " << int_tree.inf(ri) << endl;
    }
  }

  cout << "Smallest item is " << int_tree.inf( int_tree.first() ) << endl;
  cout << "Largest item is " << int_tree.inf( int_tree.last() ) << endl;
  cout << "Timing results for RB Tree of " << int_tree.length() 
       << " elements .... (avg over " << SIZE << " iterations)" << endl;

  diff = 0.0;
  for (i = 0; i < SIZE; i++) {
    gettimeofday(&start, 0);
    int_tree.first();
    gettimeofday(&stop, 0);
    // diff += diffTime(&start, &stop);
    if (stop.tv_usec < start.tv_usec) {
      stop.tv_sec -= 1;
      stop.tv_usec += ONE_MIL;
    }
    diff += (stop.tv_usec - start.tv_usec) + (ONE_MIL * (stop.tv_sec - start.tv_sec));
  }
  diff /= ONE_MIL;

  diff /= size; 
  cout << "Locating the first item takes " << diff << " seconds.  (Logarithmic Time)" << endl;

  diff = 0.0;
  for (i = 0; i < SIZE; i++) {
    gettimeofday(&start, 0);
    int_tree.last();
    gettimeofday(&stop, 0);
    // diff += diffTime(&start, &stop);
    if (stop.tv_usec < start.tv_usec) {
      stop.tv_sec -= 1;
      stop.tv_usec += ONE_MIL;
    }
    diff += (stop.tv_usec - start.tv_usec) + (ONE_MIL * (stop.tv_sec - start.tv_sec));
  }
  diff /= ONE_MIL;

  diff /= size;
  cout << "Locating the last item takes " << diff << " seconds.  (Logarithmic Time)" << endl;

  diff = 0.0;
  for (i = 0; i < SIZE; i++) {
    gettimeofday(&start, 0);
    int_tree.search( (int)uniform_distribution::sample( 0, SIZE-1) );
    gettimeofday(&stop, 0);
    // diff += diffTime(&start, &stop);
    if (stop.tv_usec < start.tv_usec) {
      stop.tv_sec -= 1;
      stop.tv_usec += ONE_MIL;
    }
    diff += (stop.tv_usec - start.tv_usec) + (ONE_MIL * (stop.tv_sec - start.tv_sec));
  }
  diff /= ONE_MIL;

  diff /= size;
  cout << "Locating a specific item takes " << diff << " seconds.  (Logarithmic Time)" << endl;
}


void main(int argc, char ** argv)
{
  if (argc < 2) {
    cerr << "usage: " << argv[0] << " [-s size] rb|bin|fib" << endl;
    exit(1);
  }
  int c = 1;

  if (argv[c][0] == '-') {
    c++;
    SIZE = atoi(argv[c++]);
  }
    
  struct timeval tp;
  gettimeofday(&tp, 0);
  srand( tp.tv_usec );

  // as a benchmark
  cout << "------------------------------ BENCHMARK ------------------------------" << endl;
  list<int> * int_list = new list<int>;
  testList(*int_list);
  delete int_list;

  assert( argc > c );
  if (!strcmp(argv[c], "rb")) {
    cout << "-----------------------------------------------------------------------" << endl;
    rb_tree<int> * int_tree = new rb_tree<int>;
    testRedBlack(*int_tree);
    delete int_tree;
  } else if (!strcmp(argv[c], "bin")) {
    cout << "Binomial Heaps have not been implemented yet." << endl;
  } else if (!strcmp(argv[c], "fib")) {
    cout << "Fibonacci Heaps have not been implemented yet." << endl;
  }
}
