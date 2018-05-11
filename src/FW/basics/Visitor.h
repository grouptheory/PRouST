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
#ifndef __VISITOR_H__
#define __VISITOR_H__

#ifndef LINT
static char const _Visitor_h_rcsid_[] =
"$Id: Visitor.h,v 1.1 1999/01/21 14:56:25 mountcas Exp $";
#endif

#include <iostream.h>
#include <DS/containers/list.h>
#include <common/cprototypes.h>
#include <FW/basics/VisitorType.h>

class Visitor;
class Behavior;
class Conduit;

class Mux;
class Adapter;
class Factory;
class Protocol;
class Cluster;

class Expander;
class Terminal;
class Accessor;
class Creator;
class State;
class Actor;

class Registry;
class VisitorType;
class vistype;

class SimEntity;
class SimEvent;

#define VISITOR_TYPE "Visitor"
#define CLUSTERKILLER_VISITOR_TYPE "ClusterKillerVisitor"
#define DISCONNECT_VISITOR_TYPE "DisconnectVisitor"

// ABC for all Visitors.
class Visitor {
  friend class Conduit;
  friend class Behavior;
  friend class Mux;
  friend class Factory;
  friend class Protocol;
  friend class Adapter;

  friend class Actor;
  friend class State;
  friend class Creator;

  friend void InitFW(void);
  friend void DeleteFW(void);

  friend class Debugger;
public:
  
  enum which_side { OTHER, A_SIDE, B_SIDE };
  // Added 5-28-98 - mountcas
  enum behavior_t { UNKNOWN = 0, ADAPTER, PROTOCOL, MUX, FACTORY };  // No at(Cluster, ...) method

  void Suicide(void);

  Conduit * GetLast(void) const;
  void      SetLast(Conduit * c);
  
  bool DumpLog(ostream & os) const;
  void CleanLog(void);
  void SetLoggingOn(void);
  void SetLoggingOff(void);
  
  void LogMyLocation(void);

  void SetOrigin(SimEntity * e);
  SimEntity * GetOrigin(void);

  void       SetReturnEvent(SimEvent * e);
  SimEvent * GetReturnEvent(void);

  virtual const VisitorType GetType(void) const;

  Visitor & operator = (const Visitor & rhs);

  Visitor * duplicate(void) const;

protected:

  // ctor for derived Visitors
  Visitor(vistype & child_type); 
  // copy ctor
  Visitor(const Visitor & rhs);
  
  virtual ~Visitor();

  virtual void on_death(void) const;

  // This method must be redefined in all derived Visitors
  virtual const vistype & GetClassType(void) const;

  which_side EnteredFrom(void);
  // Added 5-28-98 - mountcas
  behavior_t ConduitType(void) const;

  virtual void      at(Mux * m, Accessor * a);
  // This must return 0, or a ptr to the Conduit that was created
  virtual Conduit * at(Factory * f, Creator * c);
  virtual void      at(Protocol * p, State * s);
  virtual void      at(Adapter * a, Terminal * t);

  // Connot not be redefined.
  void at(Behavior * b);

  friend void UpdateProtocol(Protocol * p, State * s);

  Conduit * CurrentConduit(void) const;
  void      CurrentConduit(Conduit * c);

  Conduit * SideA(void);
  Conduit * SideB(void);

  bool EmptyLog(void) const;

  virtual void Fill_Subtype_Name(char* buf) const;

  virtual Visitor * dup(void) const = 0;

private:

  Visitor(void);

  inline void SaveLocation(Behavior * b,Actor * a);

  void _at(Mux * m, Accessor * a);
  void _at(Factory * f, Creator * c);
  void _at(Protocol * p, State * s);
  void _at(Adapter * a, Terminal * t);
  void _at(Behavior * b);

  Conduit         * _last;

  static vistype    _my_type;
  static Registry * _registry;

  bool              _logging_on;
  list<char *>    * _log;

  Conduit         * _current_conduit;
  Behavior        * _current_behavior;
  Actor           * _current_actor;
  
  int               _ref_count;

  SimEntity       * _origin;

  SimEvent        * _ret_event;

  int        _kdb_visitor_id;

  // Added both 5-28-98 - mountcas
  bool       _kissofdeath;
  behavior_t _conduit_type;

  bool       _good;
};

class DisconnectVisitor : public Visitor {
  friend void InitFW(void);
public:

  DisconnectVisitor(void)
    : Visitor(_my_type) { SetLoggingOff(); }

  virtual const VisitorType GetType(void) const { 
    return VisitorType(GetClassType());
  }

protected:

  DisconnectVisitor(vistype& child_type) : Visitor(child_type) { child_type.derived_from(_my_type);}
  virtual ~DisconnectVisitor(void) { }

  virtual const vistype& GetClassType(void) const { return _my_type; }
  virtual Visitor * dup(void) const { return new DisconnectVisitor(); }

private:

  static vistype	_my_type;
};

class ClusterKillerVisitor : public Visitor {
public:

  ClusterKillerVisitor(void) : Visitor(_my_type) { SetLoggingOff(); }

  virtual const VisitorType GetType(void) const {
    return VisitorType(GetClassType());
  }

protected:

  ClusterKillerVisitor(vistype & child_type) : Visitor(child_type) { child_type.derived_from(_my_type); }
  virtual ~ClusterKillerVisitor(void) { }

  virtual const vistype & GetClassType(void) const { return _my_type; }
  virtual Visitor * dup(void) const { return new ClusterKillerVisitor(); }

private:

  static vistype _my_type;
};


#endif // __VISITOR_H__
