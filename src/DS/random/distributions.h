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
#ifndef __DISTRIBUTIONS_H__
#define __DISTRIBUTIONS_H__

class probability_distribution;
class uniform_distribution;
class exponential_distribution;
class normal_distribution;
class gamma_distribution;
class poisson_distribution;
class binomial_distribution;

extern void set_random_seed(long rseed);

//-------------------------------------------------------------
class probability_distribution {
public:
  friend void set_random_seed(long rseed);

protected:
  enum distribution_type {
    unknown     = 0,
    uniform     = 1,
    exponential = 2,
    normal      = 4,
    gamma       = 8,
    poisson     = 16,
    binomial    = 32
  };

  probability_distribution(distribution_type t);
  virtual ~probability_distribution();

  distribution_type _t;
  static long * _seed;

  static const double _PI;
  static float gammln(float xx);

  static uniform_distribution     * _std_uniform_dist;
  static exponential_distribution * _std_exponential_dist;
  static normal_distribution      * _std_normal_dist;
  static gamma_distribution       * _std_gamma_dist;
  static poisson_distribution     * _std_poisson_dist;
  static binomial_distribution    * _std_binomial_dist;

  static void initialize(void);
  static bool _initialized;
};

//-------------------------------------------------------------
class uniform_distribution : public probability_distribution {
  friend class probability_distribution;
public:
  static double sample(double min, double max);
private:
  uniform_distribution(void);
  virtual ~uniform_distribution();
};

//-------------------------------------------------------------
class exponential_distribution : public probability_distribution {
  friend class probability_distribution;
public:
  static double sample(double mean);
private:
  exponential_distribution(void);
  virtual ~exponential_distribution();
};

//-------------------------------------------------------------
class normal_distribution : public probability_distribution {
  friend class probability_distribution;
public:
  static double sample(double mean, double variance);
private:
  normal_distribution(void);
  virtual ~normal_distribution();
};

//-------------------------------------------------------------
class gamma_distribution : public probability_distribution {
  friend class probability_distribution;
public:
  static double sample(double mean, int rank);
private:
  gamma_distribution(void);
  virtual ~gamma_distribution();
};

//-------------------------------------------------------------
class poisson_distribution : public probability_distribution {
  friend class probability_distribution;
public:
  static double sample(double mean);
private:
  poisson_distribution(void);
  virtual ~poisson_distribution();
};

//-------------------------------------------------------------
class binomial_distribution : public probability_distribution {
  friend class probability_distribution;
public:
  static double sample(double prob, int num_trials);
private:
  binomial_distribution(void);
  virtual ~binomial_distribution();
};

#endif
