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
#include <common/cprototypes.h>
#include "distributions.h"
extern "C" {
#include <math.h>
};

void set_random_seed(long rseed);

uniform_distribution     * probability_distribution::_std_uniform_dist = 0;
exponential_distribution * probability_distribution::_std_exponential_dist = 0;
normal_distribution      * probability_distribution::_std_normal_dist = 0;
gamma_distribution       * probability_distribution::_std_gamma_dist = 0;
poisson_distribution     * probability_distribution::_std_poisson_dist = 0;
binomial_distribution    * probability_distribution::_std_binomial_dist = 0;

bool   probability_distribution::_initialized = false;
long * probability_distribution::_seed = 0;
const double probability_distribution::_PI = 3.141592654;

probability_distribution::probability_distribution(distribution_type t) 
  : _t(t) 
{
  if (!_initialized) {
    initialize();
  }
}

probability_distribution::~probability_distribution() { }

void set_random_seed(long rseed) 
{
  if (rseed > 0) 
    rseed = -1*rseed;

  if (!probability_distribution::_seed) {
    probability_distribution::_seed = new long;
  }
  *probability_distribution::_seed = rseed;
}

float probability_distribution::gammln(float xx) {
  double x,y,tmp,ser;
  static double cof[6]={76.18009172947146,-86.50532032941677,
			24.01409824083091,-1.231739572450155,
			0.1208650973866179e-2,-0.5395239384953e-5};
  int j;
	
  y=x=xx;
  tmp=x+5.5;
  tmp -= (x+0.5)*log(tmp);
  ser=1.000000000190015;
  for (j=0;j<=5;j++) ser += cof[j]/++y;
  return -tmp+log(2.5066282746310005*ser/x);
}

void probability_distribution::initialize(void) {
  if (!_seed) {
    _seed = new long;
    *_seed = -1;
  }
  _initialized=true;

  _std_uniform_dist=new uniform_distribution();
  _std_exponential_dist=new exponential_distribution();
  _std_normal_dist=new normal_distribution();
  _std_gamma_dist=new gamma_distribution();
  _std_poisson_dist=new poisson_distribution();
  _std_binomial_dist=new binomial_distribution();
}

//-------------------------------------------------------------

uniform_distribution::uniform_distribution(void)
  : probability_distribution(uniform) {
}

uniform_distribution::~uniform_distribution() {
}

double uniform_distribution::sample(double min, 
				    double max) {
  if (!_initialized) {
    initialize();
  }

  int j;
  long k;
  static long iy=0;
  static long iv[32];
  float temp;

  if (*_seed <= 0 || !iy) {
    if (-(*_seed) < 1) *_seed=1;
    else *_seed = -(*_seed);
    for (j=32+7;j>=0;j--) {
      k=(*_seed)/127773;
      *_seed=16807*(*_seed-k*127773)-2836*k;
      if (*_seed < 0) *_seed += 2147483647;
      if (j < 32) iv[j] = *_seed;
    }
    iy=iv[0];
  }
  k=(*_seed)/127773;
  *_seed= 16807 *(*_seed-k*127773)-2836*k;
  if (*_seed < 0) *_seed += 2147483647;
  j=iy/(1+(2147483647-1)/32);
  iy=iv[j];
  iv[j] = *_seed;
  double std_sample;

  if ((temp= (1.0/2147483647) * iy) > (1.0-(1.2e-7)))
    std_sample = (double)(1.0-(1.2e-7));
  else std_sample = (double)temp;

  return (min + ((max-min)*std_sample));
}

//-------------------------------------------------------------

exponential_distribution::exponential_distribution(void)
  : probability_distribution(exponential) {
}

exponential_distribution::~exponential_distribution() {
}

double exponential_distribution::sample(double mean) {
  if (!_initialized) {
    initialize();
  }

  float dummy;
  do {
    dummy=_std_uniform_dist->sample(0.0,1.0);
  }
  while (dummy == 0.0);
  double std_sample = -log(dummy);

  return (mean*std_sample);
}

//-------------------------------------------------------------

normal_distribution::normal_distribution(void) 
  : probability_distribution(normal) {
}

normal_distribution::~normal_distribution() {
}

double normal_distribution::sample(double mean, double variance) {
  if (!_initialized) {
    initialize();
  }

  static int iset=0;
  static float gset;
  float fac,rsq,v1,v2;
  double std_sample;

  if  (iset == 0) {
    do {
      v1=2.0* _std_uniform_dist->sample(0.0,1.0) -1.0;
      v2=2.0* _std_uniform_dist->sample(0.0,1.0) -1.0;
      rsq=v1*v1+v2*v2;
    } 
    while (rsq >= 1.0 || rsq == 0.0);
      
    fac=sqrt(-2.0*log(rsq)/rsq);
    gset=v1*fac;
    iset=1;
    std_sample = (double)(v2*fac);
  } else {
    iset=0;
    std_sample = (double)gset;
  }

  return ( sqrt(variance)*std_sample + mean);
}

//-------------------------------------------------------------

gamma_distribution::gamma_distribution(void)
  : probability_distribution(gamma) {
}

gamma_distribution::~gamma_distribution() {
}

double gamma_distribution::sample(double mean, int rank) {
  if (!_initialized) {
    initialize();
  }

  int j;
  float am,e,s,v1,v2,x,y;
  if (rank < 1) {
    abort();  // error in gamma distribution
  }
  if (rank < 6) {
    x=1.0;
    for (j=1;j<=rank;j++) x *= _std_uniform_dist->sample(0.0,1.0);
    x = -log(x);
  } else {
    do {
      do {
	do {
	  v1=2.0* _std_uniform_dist->sample(0.0,1.0) -1.0;
	  v2=2.0* _std_uniform_dist->sample(0.0,1.0) -1.0;
	} while (v1*v1+v2*v2 > 1.0);
	y=v2/v1;
	am=rank-1;
	s=sqrt(2.0*am+1.0);
	x=s*y+am;
      } while (x <= 0.0);
      e=(1.0+y*y)*exp(am*log(x/am)-s*y);
    } while ( _std_uniform_dist->sample(0.0,1.0) > e);
  }
  double std_sample = (double)x;
  return ( mean * std_sample );
}

//-------------------------------------------------------------

poisson_distribution::poisson_distribution(void)
  : probability_distribution(poisson) {
}

poisson_distribution::~poisson_distribution() {
}

double poisson_distribution::sample(double mean) {
  if (!_initialized) {
    initialize();
  }

  static float sq,alxm,g,oldm=(-1.0);
  float em,t,y;

  if (mean < 12.0) {
    if (mean != oldm) {
      oldm=mean;
      g=exp(-mean);
    }
    em = -1;
    t=1.0;
    do {
      ++em;
      t *= _std_uniform_dist->sample(0.0,1.0);
    } while (t > g);
  } else {
    if (mean != oldm) {
      oldm=mean;
      sq=sqrt(2.0*mean);
      alxm=log(mean);
      g=mean*alxm-gammln(mean+1.0);
    }
    do {
      do {
	y=tan(_PI * _std_uniform_dist->sample(0.0,1.0));
	em=sq*y+mean;
      } while (em < 0.0);
      em=floor(em);
      t=0.9*(1.0+y*y)*exp(em*alxm-gammln(em+1.0)-g);
    } while ( _std_uniform_dist->sample(0.0,1.0) > t);
  }
  double std_sample = (double)em;
  return mean * std_sample;
}

//-------------------------------------------------------------

binomial_distribution::binomial_distribution(void)
  : probability_distribution(binomial) {
}

binomial_distribution::~binomial_distribution() {
}

double binomial_distribution::sample(double prob, int num_trials) {
  if (!_initialized) {
    initialize();
  }

  int j;
  static int nold=(-1);
  float am,em,g,angle,p,bnl,sq,t,y;
  static float pold=(-1.0),pc,plog,pclog,en,oldg;
    
  p=(prob <= 0.5 ? prob : 1.0-prob);
  am=num_trials*p;
  if (num_trials < 25) {
    bnl=0.0;
    for (j=1;j<=num_trials;j++)
      if ( _std_uniform_dist->sample(0.0,1.0) < p) ++bnl;
  } else if (am < 1.0) {
    g=exp(-am);
    t=1.0;
    for (j=0;j<=num_trials;j++) {
      t *= _std_uniform_dist->sample(0.0,1.0);
      if (t < g) break;
    }
    bnl=(j <= num_trials ? j : num_trials);
  } else {
    if (num_trials != nold) {
      en=num_trials;
      oldg=gammln(en+1.0);
      nold=num_trials;
    } if (p != pold) {
      pc=1.0-p;
      plog=log(p);
      pclog=log(pc);
      pold=p;
    }
    sq=sqrt(2.0*am*pc);
    do {
      do {
	angle= _PI * _std_uniform_dist->sample(0.0,1.0);
	y=tan(angle);
	em=sq*y+am;
      } while (em < 0.0 || em >= (en+1.0));
      em=floor(em);
      t=1.2*sq*(1.0+y*y)*exp(oldg-gammln(em+1.0)
			     -gammln(en-em+1.0)+em*plog+(en-em)*pclog);
    } while ( _std_uniform_dist->sample(0.0,1.0) > t);
    bnl=em;
  }
  if (p != prob) bnl=num_trials-bnl;
  double std_sample = bnl;
  return ( std_sample );
}


