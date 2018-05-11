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


#include <ostream.h>
#include "distributions.h"

int main(int argc, char** argv) {
  
  if (argc>1) {
    set_random_seed((long(atoi(argv[1]))));
  }

  probability_distribution* d[6];

  for (int i=0;i<6;i++) {
    if (i==0) {
      cout << "uniform_distribution\n";
    }
    if (i==1) {
      cout << "exponential_distribution\n";
    }
    if (i==2) {
      cout << "normal_distribution\n";
    }
    if (i==3) {
      cout << "gamma_distribution\n";
    }
    if (i==4) {
      cout << "poisson_distribution\n";
    }
    if (i==5) {
      cout << "binomial_distribution\n";
    }
    
    for (int j=0;j<10;j++) {
      if (i==0) {
	cout << uniform_distribution::sample(0.0,1.0) << endl;
      }
      if (i==1) {
	cout << exponential_distribution::sample(1.0) << endl;
      }
      if (i==2) {
	cout << normal_distribution::sample(0.0,1.0) << endl;
      }
      if (i==3) {
	cout << gamma_distribution::sample(1.0,1) << endl;
      }
      if (i==4) {
	cout << poisson_distribution::sample(1.0) << endl;
      }
      if (i==5) {
	cout << binomial_distribution::sample(0.5,1) << endl;
      }
    }
  };
}


