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
#ifndef LINT
static char const _Simple_h_rcsid_[] =
"$Id: Simple.h,v 1.19 1998/08/13 20:12:47 mountcas Exp $";
#endif

// This file implements all of the simple derived 
// Actors used in the examples in this directory.
#include <FW/actors/Terminal.h>
#include <FW/actors/Accessor.h>
#include <FW/actors/Creator.h>
#include <FW/actors/State.h>
#include <FW/actors/Expander.h>
#include <FW/basics/Visitor.h>

#include <DS/containers/dictionary.h>
#include <fstream.h>

class SimpleTerm : public Terminal {
public:

  // Constructor
  SimpleTerm(void);
  // Called when the Terminal receives a Visitor
  void Absorb(Visitor * v);
  // Called upon receipt of a SimEvent
  void Interrupt(SimEvent * se);

  // Ignore these until example3.cc
  // Registers a TimerHandler (see Handlers.h)
  void RegTimer(TimerHandler * t);
  // Registers an InputHandler (see Handlers.h)
  void RegInput(InputHandler * i);
  // Registers an OutputHandler (see Handlers.h)
  void RegOutput(OutputHandler * o);

protected:
  // Destructor (must be virtual, and you'd better know why)
  virtual ~SimpleTerm();
};

class SimpleAccessor : public Accessor {
public:

  // Constructor
  SimpleAccessor(void);
  // Find destination for this visitor.
  Conduit * GetNextConduit(Visitor * v);

protected:

  // Destructor (must be virtual, and you'd better know why)
  virtual ~SimpleAccessor();

  // Called when the Visitor must be Broadcast to all Conduits between the muxes.
  bool Broadcast(Visitor * v);

  // Allows the factory to update the accessor's map.
  bool Add(Conduit * c, Visitor * v);
  bool Del(Conduit * c);
  bool Del(Visitor * v);

private:

  dictionary<int, Conduit *> _int2Conduit;
  static VisitorType       * _simple_vistype;
  static VisitorType       * _not_so_simple_vistype;
};

class SimpleCreator : public Creator {
public:

  // Constructor
  SimpleCreator(int makeClusters = 0);
  // Called when a Factory needs to insert a new Conduit between two muxes
  Conduit * Create(Visitor * v);
  // Called upon receipt of a SimEvent
  void Interrupt(SimEvent * se);

protected:

  // Destructor (must be virtual, and you'd better know why)
  virtual ~SimpleCreator();

private:

  int                  _makeClusters;
  static VisitorType * _simple_vistype;
  static VisitorType * _not_so_simple_vistype;
};

class SimpleState : public State {
public:

  // Constructor
  SimpleState(void);
  // Called upon receipt of a Visitor
  State * Handle(Visitor * v);
  // Called upon receipt of a SimEvent
  void Interrupt(SimEvent * se);
protected:

  // Destructor (must be virtual, and you'd better know why)
  virtual ~SimpleState(); 
  int _ct;
};


class SimpleExpander : public Expander {
public:

  // Constructor
  SimpleExpander(int rc = 0);
  // Returns the underlying Conduit that represents the Expander's A_SIDE
  Conduit * GetAHalf(void) const;
  // Returns the underlying Conduit that represents the Expander's B_SIDE
  Conduit * GetBHalf(void) const;

protected:

  // Destructor (must be virtual, and you'd better know why)
  virtual ~SimpleExpander();

private:

  int       _recursive;
  Conduit * _upper_proto;
  Conduit * _upper_mux;
  Conduit * _factory;
  Conduit * _lower_mux;
  Conduit * _lower_proto;
};

#define SIMPLE_VISITOR_NAME "SimpleVisitor"
// Do not derive your own Visitors from this Visitor
class SimpleVisitor : public Visitor {
public:

  // Constructor takes an integer key
  SimpleVisitor(int i);

  // Returns the VisitorType (which encapsulates the vistype)
  const VisitorType GetType(void) const;

  // Returns the integer key (used by the Accessors)
  const int GetKey(void) const;
  // Sets the key
  void      SetKey(int i);

protected:

  Visitor * dup(void) const { return new SimpleVisitor( _key ); }

  // Other constructors
  SimpleVisitor(const SimpleVisitor & rhs);
  SimpleVisitor(vistype & child_type);

  // returns the vistype which contains all of the parent vistypes, 
  // along with this class name.  Used for RTTI 
  // (Run time type identification) since gcc/g++ doesn't support it at this time.
  const vistype & GetClassType(void) const;

  // Destructor (must be virtual, and you'd better know why)
  virtual ~SimpleVisitor();

private:

  int             _key;    
  static vistype  _my_type;
};

// -----------------------------------
#define NOT_SO_SIMPLE_VISITOR_NAME "NotSoSimpleVisitor"
class NotSoSimpleVisitor : public Visitor {
public:

  // Constructor takes an integer key
  NotSoSimpleVisitor(int i);

  // Returns the VisitorType (which encapsulates the vistype)
  const VisitorType GetType(void) const;

  // Returns the integer key (used by the Accessors)
  const int GetKey(void) const;
  // Sets the key
  void      SetKey(int i);

protected:

  // Other constructors
  NotSoSimpleVisitor(const NotSoSimpleVisitor & rhs);
  NotSoSimpleVisitor(vistype & child_type);

  // returns the vistype which contains all of the parent vistypes, 
  // along with this class name.  Used for RTTI 
  // (Run time type identification) since gcc/g++ doesn't support it at this time.
  const vistype & GetClassType(void) const;

  // Destructor (must be virtual, and you'd better know why)
  virtual ~NotSoSimpleVisitor();

private:

  int             _key;    
  static vistype  _my_type;
};

// --------------- For example3.cc -------------------
#include <FW/kernel/Handlers.h>

class SimpleTimer : public TimerHandler {
public:

  // Constructor takes repeat interval
  SimpleTimer(SimEntity * o, double interval);
  // Destructor
  virtual ~SimpleTimer();

  virtual void Callback(void);

private:

  double _reregister_interval;
};

class SimpleInput : public InputHandler {
public:

  // constructor takes the name of a file to read
  SimpleInput(SimEntity * o, int fd);
  // destructor
  virtual ~SimpleInput();

  virtual void Callback(void);

private:

  ifstream _istr;
};
